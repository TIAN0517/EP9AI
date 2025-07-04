#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../MfcExLib/RanFilter.h"

#include "../../EngineLib/DxTools/RENDERPARAM.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Club/GLClubServer.h"
#include "../Database/DBAction/DbActionGameClub.h"
#include "../Party/GLPartyField.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Guidance/GLGuidance.h"
#include "../Util/GLItemLimit.h"
#include "./GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

HRESULT GLChar::MsgClubNew(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_NEW* pNetMsg = (GLMSG::SNET_CLUB_NEW*) nmg;

	CString strClubName(pNetMsg->szClubName)
		, strSaveClubName(sc::string::utf8_to_ansi(std::string(pNetMsg->szClubName)).c_str());

	GLMSG::NET_CLUB_NEW_FB_FC NetMsgFb(false);
	NetMsgFb.SetClubName(strSaveClubName.GetString());

	BOOL bFILTER0 = STRUTIL::CheckString(strClubName);
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter(strClubName);
	if (bFILTER0 || bFILTER1)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_BADNAME;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	// RM #5876
	const INT nCmpCnt(RENDERPARAM::emLangSet == language::LANGFLAG_TAIWAN ? club::CLUBNAME_LIMIT_TAIWAN : club::CLUBNAME_LIMIT );
	if ( strClubName.GetLength() == 0 || strClubName.GetLength() > nCmpCnt )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_BADNAME;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	// 태국어 문자 조합 체크		
    if (RENDERPARAM::emLangSet == language::THAILAND)
    {
	    if (!STRUTIL::CheckThaiString(strClubName))
	    {
		    NetMsgFb.emFB = EMCLUB_NEW_FB_THAICHAR_ERROR;
		    SendToClient(&NetMsgFb);
		    return S_FALSE;
	    }
    }

	// 베트남 문자 조합 체크 
    if (RENDERPARAM::emLangSet == language::VIETNAM)
    {
	    if (STRUTIL::CheckVietnamString(strClubName))
	    {
		    NetMsgFb.emFB = EMCLUB_NEW_FB_VNCHAR_ERROR;
		    SendToClient(&NetMsgFb);
		    return E_FAIL;
	    }
    }

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_FAIL;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	GLCrow* pNpc = pLand->GetCrow(pNetMsg->dwNpcGlobID);
	if (!pNpc)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_FAIL;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	if (!pNpc->IsBasicTalk(SNpcTalk::EM_CLUB_NEW))
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_FAIL;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	if (m_ClubDbNum != CLUB_NULL)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_ALREADY;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	GLPartyField* const pParty = GetMyParty();
	if ( !pParty || !pParty->isMasterByGaeaID(m_dwGaeaID) )
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_NOTMASTER;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	if (pParty->GetNMember() < GLCONST_CHAR::dwCLUB_PARTYNUM)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_NOTMEMBER;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}
		
	PartyMemberGaeaIDs arPartyMemberGaeaIDs;
	const DWORD nMember = pParty->GetPartyMemberGaeaIDs(arPartyMemberGaeaIDs);
	for ( DWORD _i = 0; _i < nMember; ++_i )
	{
		GLChar* pPartyMember = GetChar(arPartyMemberGaeaIDs[_i]);
		if (!pPartyMember)
		{
			NetMsgFb.emFB = EMCLUB_NEW_FB_NOTEXIST_MEM;
			SendToClient(&NetMsgFb);
			return S_FALSE;
		}

		if (pPartyMember->m_ClubDbNum != CLUB_NULL)
		{
			NetMsgFb.emFB = EMCLUB_NEW_FB_MEM_ALREADY;
			SendToClient(&NetMsgFb );
			return S_FALSE;
		}

		// 다른 학원생과 클럽 결성 제한 옵션이 있을 경우 제한함.
		if (!GLCONST_CHAR::bCLUB_2OTHERSCHOOL)
		{
			if (pPartyMember->m_wSchool != m_wSchool)
			{
				NetMsgFb.emFB = EMCLUB_NEW_FB_OTHERSCHOOL;
				SendToClient(&NetMsgFb);
				return S_FALSE;
			}
		}
	}

	if (m_wLevel < GLogicData::GetInstance().GetClubRankData(0).m_dwMasterLvl)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_MASTER_LVL;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	if (m_nLiving < int(GLogicData::GetInstance().GetClubRankData(0).m_dwLivingPoint))
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_AVER_LIVING;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}

	if (GetInvenMoney() < GLogicData::GetInstance().GetClubRankData(0).m_dwPay)
	{
		NetMsgFb.emFB = EMCLUB_NEW_FB_NOTMONEY;
		SendToClient(&NetMsgFb);
		return S_FALSE;
	}	

	// Agent 에게 클럽 생성을 요청함
	GLMSG::SNET_CLUB_NEW_2AGT NetMsg2Agt;
	NetMsg2Agt.dwChaNum = CharDbNum();
	NetMsg2Agt.SetClubName(strSaveClubName.GetString());
	SendToAgent(&NetMsg2Agt);

	return S_OK;
}

// *****************************************************
// Desc: 클럽생성 요청에 대한 결과 처리
// *****************************************************
HRESULT GLChar::MsgClubNew2FLD ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_NEW_2FLD *pNetMsg = (GLMSG::SNET_CLUB_NEW_2FLD *) nmg;

	LONGLONG lnPAY = GLogicData::GetInstance().GetClubRankData(0).m_dwPay;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgClubNew2FLD", m_CharDbNum ) );
	}

	//	Note : 생성 성공시에 생성 비용 지불.
	if ( pNetMsg->emFB==EMCLUB_NEW_FB_OK )
	{
		CheckMoneyUpdate( GetInvenMoney(), lnPAY, FALSE, "Club Create Pay." );

		if ( GetInvenMoney() >= lnPAY )
		{
			SubtractInvenMoneyServer( lnPAY, SUBTRACT_INVEN_MONEY_CLUB_CREATE );
		}
		else
		{
			// 기획 의도인가?
			SetInvenMoney( 0 );
		}
	}

	// 금액 변화 로그
	// 통계 및 로그 수정
	LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB, 0, -lnPAY, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CREATE_CLUB_CHARGE);
	// 통계 및 로그 수정
	LogMoneyExchange(gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CHAR, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT);

	// 금액 변화 알림.
	SendInvenMoneyUpdate();

	//	Note : 생성 요청 결과 반환.
	//
	GLMSG::NET_CLUB_NEW_FB_FC NetMsgFb(false);
	NetMsgFb.emFB = pNetMsg->emFB;
	StringCchCopy ( NetMsgFb.szClubName, CHAR_SZNAME, pNetMsg->szClubName );
	SendToClient(&NetMsgFb );

	//	Note : 클럽 생성 로그 기록.
	// 통계 및 로그 수정
	LogCharAction(
		m_CharDbNum,					//	당사자.
		gamelog::ACT_CLUB_MAKE,			//	행위.
		gamelog::ID_CLUB, pNetMsg->dwCLUBID,	//	상대방.
		0,							//	exp
		0,							//	bright
		0,							//	life
		(int)-lnPAY,					//	money
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap()
	);

	return S_OK;
}
/*
HRESULT GLChar::MsgClubMemberReq ( NET_MSG_GENERIC* nmg )
{
	GLMSG::NET_CLUB_MEMBER_REQ_CA *pNetMsg = (GLMSG::NET_CLUB_MEMBER_REQ_CA *) nmg;

	GLMSG::SNET_CLUB_MEMBER_REQ_FB NetMsgFB(false);

	GLChar* pTarChar = GetChar(pNetMsg->m_TarGaeaID);
	if ( !pTarChar )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	NetMsgFB.SetTarChaName(pTarChar->m_szName);

    //! CTF 참여중이면 취소
    if ( pTarChar->IsCTFPlayer() )
    {
        NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }

	// 다른 학원생과 클럽 결성 제한 옵션이 있을 경우 제한함.
	if ( !GLCONST_CHAR::bCLUB_2OTHERSCHOOL )
	{
		if ( pTarChar->m_wSchool!=m_wSchool )
		{
			NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL;
			SendToClient( &NetMsgFB );
			return S_FALSE;
		}
	}

    // 국가체크
    if (m_Country != pTarChar->GetCountry())
    {
        NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_NOT_SAME_COUNTRY;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }
	
	std::tr1::shared_ptr<GLClubField> pClub = GetMyClub();
	if (!pClub)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( pClub->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_CLUBBATTLE;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( !pClub->HaveMemberJoinAuthority(m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	bool bFOUND = pClub->IsMember(pTarChar->m_CharDbNum);
	if ( bFOUND )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_ALREADY;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	const GLCLUBRANK &sRANK_DATA = GLogicData::GetInstance().GetClubRankData(pClub->m_dwRank);
	if ( pClub->GetMemberNum() >= sRANK_DATA.m_dwMember )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_MAXNUM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}	
    if ( pTarChar->m_ClubDbNum!=CLUB_NULL )
    {
        NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
        SendToClient( &NetMsgFB );
        return S_FALSE;
    }


	GLMSG::SNET_CLUB_MEMBER_REQ_ASK NetMsgAsk;
	NetMsgAsk.SetClubName(pClub->Name());
	NetMsgAsk.dwMaster = m_CharDbNum;
	NetMsgAsk.SetMasterName(pClub->GetMasterName());

	pTarChar->SendToClient(&NetMsgAsk);
	
	return S_OK;
}
*/
/*
HRESULT GLChar::MsgClubMemberAns(NET_MSG_GENERIC* nmg)
{
	GASSERT(m_pGLGaeaServer->IsField());

	GLMSG::SNET_CLUB_MEMBER_REQ_ANS *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_REQ_ANS *) nmg;

	GLChar* pMaster = GetCharByDbNum(pNetMsg->dwMaster);
	if (!pMaster)
		return S_FALSE;

	GLMSG::SNET_CLUB_MEMBER_REQ_FB NetMsgFB(false);
	NetMsgFB.SetTarChaName(m_szName);

	if (m_ClubDbNum != CLUB_NULL)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( !pNetMsg->bOK )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_REFUSE;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}
	
	std::tr1::shared_ptr<GLClubField> pCLUB = GetClub(pMaster->m_ClubDbNum);
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( pCLUB->GetAllBattleNum() > 0 )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_CLUBBATTLE;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( !pCLUB->HaveMemberJoinAuthority(pMaster->m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_FAIL;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	bool bFOUND = pCLUB->IsMember(m_CharDbNum);
	if ( bFOUND )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_ALREADY;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	const GLCLUBRANK &sRANK_DATA = GLogicData::GetInstance().GetClubRankData(pCLUB->m_dwRank);
	if ( pCLUB->GetMemberNum() >= sRANK_DATA.m_dwMember )
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_REQ_FB_MAXNUM;
		pMaster->SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	GLMSG::SNET_CLUB_MEMBER_REQ_2AGT NetMsgAgt;
	NetMsgAgt.dwChaNum = pMaster->CharDbNum();
	NetMsgAgt.dwReqGID = m_dwGaeaID;
	SendToAgent(&NetMsgAgt);

	return S_OK;
}
*/
HRESULT GLChar::MsgClubRank(NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_CLUB_RANK_UP_CF *pNetMsg = (GLMSG::NET_CLUB_RANK_UP_CF *) nmg;

	GLMSG::SNET_CLUB_RANK_FB NetMsgFB;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	GLCrow* pCROW = pLand->GetCrow( pNetMsg->dwNpcGlobID );
	if ( !pCROW )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if (!pCROW->IsBasicTalk(SNpcTalk::EM_CLUB_UP))
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if ( m_ClubDbNum==CLUB_NULL )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
	
	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if ( !pCLUB )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
	/*
	if ( pCLUB->MasterCharDbNum() != m_CharDbNum )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
    */
    if (!pCLUB->HaveRankUpAuthority(m_CharDbNum))
        return S_FALSE;

	if ( pCLUB->m_dwRank >= (GLCONST_CHAR::dwMAX_CLUBRANK-1) )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_MAX;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	DWORD dwRANK = pCLUB->m_dwRank + 1;

	if (pCLUB->GetMasterLevel() < static_cast<int> (GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwMasterLvl))
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_MASTER_LVL;
		NetMsgFB.dwValue = GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwMasterLvl;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
/*
    // Master 의 Living point 를 알 방법이 없음
	if ( m_nLiving < int ( GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwLivingPoint ) )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_AVER_LIVING;
		NetMsgFB.dwValue = GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwLivingPoint;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}
*/
	if ( GetInvenMoney() < GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwPay )
	{
		NetMsgFB.emFB = EMCLUB_RANK_FB_NOTMONEY;
		NetMsgFB.dwValue = GLogicData::GetInstance().GetClubRankData(dwRANK).m_dwPay;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// 랭크 변화 에이젼트에 요청.
	GLMSG::NET_CLUB_RANK_CHANGE_FA NetMsgAgt;
	NetMsgAgt.dwChaNum = CharDbNum();
	SendToAgent(&NetMsgAgt);

	return S_OK;
}

HRESULT GLChar::MsgClubMemberNick(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CLUB_MEMBER_NICK* pNetMsg = (GLMSG::SNET_CLUB_MEMBER_NICK*) nmg;

    char szNick[CHAR_SZNAME] = {0};
    StringCchCopy(szNick, CHAR_SZNAME, pNetMsg->szNick);

	GLMSG::SNET_CLUB_MEMBER_NICK_FB NetMsgFB;

	CString strGuNick(szNick);

    if (m_ServiceProvider == SP_WORLD_BATTLE)
    {
        std::string ClubNick(szNick);
        if (!sc::string::checkEnglishOrNumber(ClubNick))
        {
            NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_WB_ERROR;
            SendToClient( &NetMsgFB );
            return S_FALSE;
        }
    }

    if (m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST)
    {
        std::string ClubNick(szNick);
        if (!sc::string::goodChaNameKorea(ClubNick))
        {
            NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_KR_ERROR;
            SendToClient( &NetMsgFB );
            return S_FALSE;
        }
    }

	BOOL bFILTER0 = STRUTIL::CheckString(strGuNick);
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter(strGuNick);
	if (bFILTER0 || bFILTER1)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_BADNAME;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// 태국어 문자 조합 체크 
    if (RENDERPARAM::emLangSet == language::THAILAND)
    {
	    if (!STRUTIL::CheckThaiString(strGuNick))
	    {
		    NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR;
		    SendToClient( &NetMsgFB );
		    return S_FALSE;
	    }
    }

	// 베트남 문자 조합 체크 
    if (RENDERPARAM::emLangSet == language::VIETNAM)
    {
	    if (STRUTIL::CheckVietnamString(strGuNick))
	    {
		    NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR;
		    SendToClient( &NetMsgFB );
		    return E_FAIL;
	    }
    }

	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if (!pCLUB)
	{
		NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_FAIL;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	std::string strConvNick(szNick);
	if ( m_ServiceProvider == SP_GS )//
		strConvNick = sc::string::utf8_to_ansi(strConvNick);

	SetNickName(strConvNick.c_str());

	AddGameDbJob(
		db::DbActionPtr(
		new db::ClubNickUpdate(m_CharDbNum, strConvNick)));

	GLMSG::SNET_CLUB_INFO_NICK_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID = m_dwGaeaID;
	StringCchCopy(NetMsgBrd.szNickName, CHAR_SZNAME, strConvNick.c_str());
	SendMsgViewAround(&NetMsgBrd);

	NetMsgFB.emFB = EMCLUB_MEMBER_NICK_FB_OK;
	StringCchCopy(NetMsgFB.szNick, CHAR_SZNAME, strConvNick.c_str());
	SendToClient( &NetMsgFB );

	return S_OK;
}

HRESULT GLChar::MsgClubCDCertify ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_CD_CERTIFY *pNetMsg = (GLMSG::SNET_CLUB_CD_CERTIFY *)nmg;

	GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFB;

	GLMSG::SNET_CLUB_CD_CERTIFY_BRD  NetMsgBrd;

	//DxConsoleMsg* pConsoleMsg = m_pGLGaeaServer->GetConsoleMsg();

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return S_FALSE;

	if ( m_ClubDbNum==CLUB_NULL )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTMASTER;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if ( !pCLUB || !pCLUB->IsMemberFlgCDCertify(m_CharDbNum) )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTMASTER;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	// 인증하려는 클럽이 동맹에 가입되어 있을 경우 동맹 클럽중에 선도클럽이 있으면 인증 불가.
	if ( pCLUB->IsAlliance() )
	{
		// 동맹길드 정보는 동맹의 장만 갖고있는것 같다... ;;;
		std::tr1::shared_ptr<GLClubField> pCHIEFCLUB = GetClub(pCLUB->m_dwAlliance);		
		DWORD dwGuidID = 0;

		StringCchCopy ( NetMsgBrd.szAlliance, CHAR_SZNAME, pCHIEFCLUB->Name() );

		CLUB_ALLIANCE_ITER pos = pCHIEFCLUB->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pCHIEFCLUB->m_setAlliance.end();
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sALLIANCE = pos->second;
			if ( sALLIANCE.m_dwID == pCLUB->m_DbNum ) continue;

			std::tr1::shared_ptr<GLClubField> pALLIANCECLUB = GetClub(sALLIANCE.m_dwID);
			if ( pALLIANCECLUB )
			{
				dwGuidID = m_pGaeaServer->GuidanceId ( pALLIANCECLUB->m_DbNum );
				if ( dwGuidID != UINT_MAX ) 
				{
					NetMsgFB.emFB = EMCDCERTIFY_HAVEGUIDCLUB;
					SendToClient( &NetMsgFB );
					return S_FALSE;
				}
			}
		}
	}

	PGLCROW pCROW = pLand->GetCrow(pNetMsg->dwNpcGlobID);
	if ( !pCROW )
	{
		NetMsgFB.emFB = EMCDCERTIFY_BADNPC;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	if (!pCROW->IsBasicTalk(SNpcTalk::EM_CD_CERTIFY))
	{
		NetMsgFB.emFB = EMCDCERTIFY_BADNPC;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	DWORD dwGUID = m_pGaeaServer->GuidanceId ( m_ClubDbNum );
	if (dwGUID != UINT_MAX)
	{
		if (dwGUID==pLand->m_dwClubMapID)
            NetMsgFB.emFB = EMCDCERTIFY_AREADY;
		else
            NetMsgFB.emFB = EMCDCERTIFY_AREADYGUID;

		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	EMCHECKCERTIFY emCERTIFY = m_pGaeaServer->GuidanceCheckCertify(pLand->m_dwClubMapID, m_CharDbNum);
	if ( emCERTIFY!=EMCHECKCERTIFY_OK )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTCONDITION;

		switch ( emCERTIFY )
		{
		case EMCHECKCERTIFY_FAIL:
			NetMsgFB.emFB = EMCDCERTIFY_NOTCONDITION;
			break;
		case EMCHECKCERTIFY_DOING:
			NetMsgFB.emFB = EMCDCERTIFY_AREADYCERTIFY;
			break;
		case EMCHECKCERTIFY_OTHERDOING:
			NetMsgFB.emFB = EMCDCERTIFY_OTHERCERTIFING;
			break;
		case EMCHECKCERTIFY_NOTBATTLE:
			NetMsgFB.emFB = EMCDCERTIFY_NOTTIME;
			break;
		};

		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	D3DXVECTOR3 vDist = pCROW->GetPosition() - m_vPosition;
	float fDist = D3DXVec3Length ( &vDist );
	if ( fDist > GLCONST_CHAR::fCDCERTIFY_DIST )
	{
		NetMsgFB.emFB = EMCDCERTIFY_DISTANCE;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	SINVENITEM *pINVENITEM = m_cInventory.FindItem ( ITEM_CD );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMCDCERTIFY_NOTITEM;
		SendToClient( &NetMsgFB );
		return S_FALSE;
	}

	WORD wPosX = pINVENITEM->wPosX;
	WORD wPosY = pINVENITEM->wPosY;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgClubCDCertify", m_CharDbNum ) );
	}

	//	아이템 삭제 로그    
	// 통계 및 로그 수정
	LogItemExchange(
        pINVENITEM->sItemCustom,
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CLUB, 0,
        ITEM_ROUTE_DELETE, pINVENITEM->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_GUIDANCE_CERTIFICATION,
		false);

	// 인벤에서 아이템 삭제
    InvenDeleteItem( wPosX, wPosY, true, FALSE, 1, true );

	//	Note : 인증 시작.
    m_pGaeaServer->GuidanceDoCertify(pLand->m_dwClubMapID, m_CharDbNum, pNetMsg->dwNpcGlobID, m_vPosition);

	//	Note : 인증 시작을 알림 FB.
	NetMsgFB.emFB = EMCDCERTIFY_OK;
	SendToClient( &NetMsgFB );

	//	Note : 인증 시작을 알림 BRD.
	std::string strZone = m_pGaeaServer->GuidanceGetName(pLand->m_dwClubMapID);
	StringCchCopy ( NetMsgBrd.szZone, GLMSG::SNET_CLUB_CD_CERTIFY_BRD::TEXT_LEN, strZone.c_str() );
	StringCchCopy ( NetMsgBrd.szName, CHAR_SZNAME, m_szName );
	StringCchCopy ( NetMsgBrd.szClub, CHAR_SZNAME, pCLUB->Name() );
	
	SendToAgent(&NetMsgBrd);

	//  [10/17/2013 gbgim];
	// 인증 시작시 은신이 활성화되어있다면 은신이 풀려야한다;
	// GLHIDESET::CheckHideSet(EMHIDE_SPEEDUP_MAX) + 루아파일에서 사용할지말지 적어놓고 쓴다
	// 인증시작할때 루아에 체크되어잇는지보고 하자;
	// GLogicServer Folder - Script - HideSet.Lua;
	if ( GLHIDESET::CheckHideSet( EMHIDE_GUIDANCE_CERTIFY_HIDECANCLE ) )
	{
		RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		RestoreActStateByInvisible();
	}

	return S_OK;
}
/*
HRESULT GLChar::MsgReqClubGetStorage ( NET_MSG_GENERIC* nmg )
{
	GLMSG::NET_CLUB_GETSTORAGE_CA *pNetMsg = (GLMSG::NET_CLUB_GETSTORAGE_CA *) nmg;

	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->MasterCharDbNum() != m_CharDbNum )
	{
		return S_OK;
	}

	if ( !pCLUB->m_bVALID_STORAGE )
	{
		AddGameDbJob(
            db::DbActionPtr(
                new db::ClubStorageGet(m_ClientSlot,m_CharDbNum,m_ClubDbNum)));	
	}
	else
	{
		MsgClubStorage();
	}
	return S_OK;
}
*/
HRESULT GLChar::MsgReqClubStorageToInven( DWORD ClubDbNum, const SITEMCUSTOM& Item, WORD PosX, WORD PosY )
{
	if ( !GLUseFeatures::GetInstance().IsUsingClubLockerItemGet() ) return S_FALSE;

	// 아이템을 넣는데 유저에게 알려주지 않고, 디비 업데이트 없이 넣는다.
    bool bOk = InvenInsertItem( Item, PosX, PosY, false, 1, false );
	if ( !bOk )
    {
        // 아이템 복구
        GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA( m_CharDbNum, ClubDbNum, Item );
        msgpack::sbuffer SendBuffer;
        msgpack::pack( SendBuffer, MsgFA );
        SendToAgent( NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true );
		return S_FALSE;
    }

	SINVENITEM* pInsertedItem = m_cInventory.GetItem( PosX, PosY );
	if ( !pInsertedItem )
	{
		// 이경우는 발생하지 않는데 발생할 경우 이미 넣은 아이템 삭제하고 아이템 복구 메시지 보낸다.
		// 에이전트에서는 클럽락커에서 아이템을 메모리에서만 삭제하고 디비에서는 삭제하기 않았다.
		// 위에서도 메모리에만 넣은 상황
		sc::writeLogError( sc::string::format( "critical error MsgReqClubStorageToInven GetItem chanum: %1%", m_CharDbNum ) );

		// 아이템 삭제
		InvenDeleteItem( PosX, PosY, false, FALSE, 1, false );

		// 아이템 복구
		GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA( m_CharDbNum, ClubDbNum, Item );
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, MsgFA );
		SendToAgent( NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true );

		return S_FALSE;
	}

	// 아이템 삽입 알림
	InvenInsertSend( pInsertedItem );

	// 디비 업데이트, 아이템 전체 업데이트가 아닌 저장 위치 변경만 한다.
	InvenUpdateItem( pInsertedItem, true );

	// 아이템의 소유 이전 경로 기록
	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubStorageToInven", m_CharDbNum ) );
	}

	// 통계 및 로그 수정
	LogItemExchange( 
		Item,
		gamelog::ID_CLUB, ClubDbNum,
		gamelog::ID_CHAR, m_CharDbNum,
		ITEM_ROUTE_CHAR, Item.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_CLUB_STORAGE_MOVE_CLUB_TO_CHAR,
		false );

	// RESET_XXX 말고 우선 CheckItemDuration 로 대체하고 추후 문제가 발생하면 RESET_XXX 두개 다시 쓴다. 쓸데없이 모든 공간 다 검사하고, 
	// 장착창 차고 있는 아이템까지 검사하고 있다. 우선 원래 있던대로 나둔다. RESET_TIMELMT_ITEM 이게 3분 마다 돌도록 되어 있고, GetViewAround 에서 한다.
	// 거기에 해당되지 않으면 소멸 체크가 안되어서 이렇게 하는 것 같다. RESET_TIMELMT_ITEM 이거 확인해서 제대로 사용될 수 있도록 한다.
	// 기간이 만료된 아이템을 소멸.
	//CheckItemDuration( *pInsertedItem );
	RESET_TIMELMT_ITEM();
	RESET_CHECK_ITEM();

	return S_OK;
}

bool GLChar::IsKEEP_CLUB_STORAGE ( DWORD dwCHANNEL )
{
	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if (!pCLUB)
        return false;
	if (pCLUB->MasterCharDbNum() != m_CharDbNum)
        return false;
	if (dwCHANNEL >= MAX_CLUBSTORAGE)
        return false;
	if (pCLUB->m_dwRank < dwCHANNEL)
        return false;
	return true;
}

HRESULT GLChar::MsgReqClubStorageExInven( DWORD ClubDbNum, const SITEMCUSTOM& ClubItem, WORD Channel, WORD ClubStoragePosX, WORD ClubStoragePosY, WORD InvenPosX, WORD InvenPosY )
{
	// 사용되지 않는다.
	return S_OK;

	/*
    SINVENITEM* pInvenItem = m_cInventory.GetItem( InvenPosX, InvenPosY );

	if (!pInvenItem)
    {
        // 아이템 복구
        GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA(m_CharDbNum, ClubDbNum, ClubItem);
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, MsgFA);
        SendToAgent(NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true);
		return S_FALSE;
    }

	const SITEM* pInvenItemData = GLogicData::GetInstance().GetItem(pInvenItem->Id());
	if (!pInvenItemData)
    {
        // 아이템 복구
        GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA(m_CharDbNum, ClubDbNum, ClubItem);
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, MsgFA);
        SendToAgent(NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true);
        return S_FALSE;
    }
	
	//	거래옵션
	//if (!pInvenItemData->sBasicOp.IsEXCHANGE())
	if ( !( pInvenItem->sItemCustom.IsExchange() != NULL ) )
    {
        // 아이템 복구
        GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA(m_CharDbNum, ClubDbNum, ClubItem);
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, MsgFA);
        SendToAgent(NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true);
        return S_FALSE;
    }
	
	// 인벤 아이템 백업
    SITEMCUSTOM sInvenItem = pInvenItem->CustomItem(); 
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if (sInvenItem.bVietnamGainItem)
        {
            // 아이템 복구
            GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA(m_CharDbNum, ClubDbNum, ClubItem);
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, MsgFA);
            SendToAgent(NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true);
			return S_FALSE;
        }
	}
	
	const SITEM* pClubItemData = GLogicData::GetInstance().GetItem(ClubItem.GetNativeID());	
	if (!pClubItemData)
    {
        // 아이템 복구
        GLMSG::NET_CLUB_STORAGE_RESTORE_FA MsgFA(m_CharDbNum, ClubDbNum, ClubItem);
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, MsgFA);
        SendToAgent(NET_MSG_CLUB_STORAGE_RESTORE_FA, SendBuffer, true);
        return S_FALSE;
    }

    SINVENITEM sInven_BackUp = *pInvenItem;	//	인벤 아이템 백업.
    WORD wInvenPosX = sInven_BackUp.wPosX;		//	인벤 아이템 위치.
    WORD wInvenPosY = sInven_BackUp.wPosY;
    	
	// 인벤 아이템 클럽창고에 넣음
    GLMSG::NET_CLUB_STORAGE_EX_INVEN_FA MsgInsert(ClubDbNum, m_CharDbNum, sInvenItem, Channel, ClubStoragePosX, ClubStoragePosY);
    msgpack::sbuffer SendBufferFA;
    msgpack::pack(SendBufferFA, MsgInsert);
    SendToAgent(NET_MSG_CLUB_STORAGE_EX_INVEN_FA, SendBufferFA, true);

    // 인벤 처리.
    {
        //	Inventory에 있던 아이템 삭제.
        BOOL bOk = InvenDeleteItem(sInven_BackUp.wPosX, sInven_BackUp.wPosY, false, FALSE);

        // 종전 손에든 아이템과 들려는 아이템의 크기가 틀릴 경우.
        //	들고 있는 아이템이 들어갈 공간을 찾는다.
        //	if ( !pInvenData->SAME_INVENSIZE ( *pHoldData ) )
        //	{
        //	동일위치에 넣을수 있는지 검사.
        bOk = m_cInventory.IsInsertable( wInvenPosX, wInvenPosY );
        if ( !bOk )
        {
            //	들고있던 아이템 Inventory에 넣을 공간이 있는지 검색.
            bOk = m_cInventory.FindInsertable( wInvenPosX, wInvenPosY );
            if ( !bOk )
            {
                //	roll-back : Inventory에서 삭제했던 아이템을 다시 넣음.
                m_cInventory.InsertItem( sInven_BackUp.sItemCustom, sInven_BackUp.wPosX, sInven_BackUp.wPosY, true );

                return S_FALSE;
            }
        }
        //}

        //	클럽 아이템 Inventory에 넣음.
        m_cInventory.InsertItem( ClubItem, wInvenPosX, wInvenPosY );
        SINVENITEM* pINSERT_IVEN_ITEM = m_cInventory.GetItem( wInvenPosX, wInvenPosY );
        if ( !pINSERT_IVEN_ITEM )
        {
            sc::writeLogError(sc::string::format("error MsgReqClubStorageExInven(), ClubItem, id(%d,%d) to pos(%d,%d)",
                ClubItem.GetNativeID().wMainID, ClubItem.GetNativeID().wSubID, wInvenPosX, wInvenPosY ));

            return S_FALSE;
        }

        GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Inven_Del_Insert;
        NetMsg_Inven_Del_Insert.wDelX = sInven_BackUp.wPosX;					//	삭제될 아이템.
        NetMsg_Inven_Del_Insert.wDelY = sInven_BackUp.wPosY;
        if (pINSERT_IVEN_ITEM)
            NetMsg_Inven_Del_Insert.sInsert = *pINSERT_IVEN_ITEM;	//	삽입되는 아이템.

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, NetMsg_Inven_Del_Insert);
        SendToClient(NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer);

        //	유효기간 초과 점검.	
        CheckItemDuration( *pINSERT_IVEN_ITEM );
    }

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubStorageExInven", m_CharDbNum ) );
	}

	// 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
	LogItemExchange(
        sInvenItem,
        gamelog::ID_CHAR, m_CharDbNum,
        gamelog::ID_CLUB, m_ClubDbNum,
        ITEM_ROUTE_CLUB, sInvenItem.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_CLUB_STORAGE_EXCHANGE_CHAR_TO_CLUB,
		false);	

	// 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
	LogItemExchange(
        ClubItem,
        gamelog::ID_CLUB, m_ClubDbNum,
        gamelog::ID_CHAR, m_CharDbNum,
        ITEM_ROUTE_CHAR, ClubItem.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_CLUB_STORAGE_EXCHANGE_CLUB_TO_CHAR,
		false);	
	
	return S_OK;
	*/
}

bool GLChar::MsgReqClubInvenToStorage( DWORD Channel, WORD PosX, WORD PosY, WORD HoldPosX, WORD HoldPosY)
{	
	if ( !GLUseFeatures::GetInstance().IsUsingClubLockerItemSave() ) return S_FALSE;

	std::tr1::shared_ptr< GLClubField > spClub = GetMyClub();
	if ( !spClub )
		return false;

	if ( Channel >= MAX_CLUBSTORAGE )
		return false;
	
    SINVENITEM* pInvenItem = m_cInventory.GetItem( HoldPosX, HoldPosY );
	if ( !pInvenItem )
		return false;

	// pInvenItem 아이템 디비 상태가 update 상태라고 가정하고 진행된다.(none 이나 insert 상태라면? - 이런 경우는 없다고 봐야 한다.)
	// 인벤에 들어올 때 이미 insert 되거나, 이미 insert 된 아이템이라면 update 까지 된 상태이다. 
	if ( db::DB_UPDATE != pInvenItem->GetDbState() )
	{
		InvenUpdateItem( pInvenItem );
	}

	//	거래옵션
	if ( !( pInvenItem->sItemCustom.IsExchange() != NULL ) )
        return false;

	// #3989 GmCharEdit 에서 거래불가로 넣어준 아이템을 클럽창고에 넣을 수 있는 문제 수정
	if ( pInvenItem->sItemCustom.IsGM_GENITEM() )
		return false;

    if ( !spClub->HaveStorageAuthority( m_CharDbNum ) )
        return false;

// 	if ( m_ServiceProvider == SP_VIETNAM )
// 	{
// 		if ( pInvenItem->CustomItem().bVietnamGainItem )
// 			return false;
// 	}

    // 아이템 백업
    SITEMCUSTOM HoldItem = pInvenItem->CustomItem();

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubInvenToStorage", m_CharDbNum ) );
	}

	// 아이템 제거, 디비업데이트는 하지 않는다. 실제 에이전트서버에서 클럽락커로 인벤타입 및 좌표만 변경된다.
	if ( !InvenDeleteItem( HoldPosX, HoldPosY, true, HoldItem.bVietnamGainItem, 1, false ) )
	{
		return false;
	}

	// 아이템의 소유 이전 경로 기록
	// 통계 및 로그 수정
	LogItemExchange( 
        HoldItem, 
        gamelog::ID_CHAR, m_CharDbNum, 
        gamelog::ID_CLUB, spClub->DbNum(), 
        ITEM_ROUTE_CLUB, HoldItem.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_CLUB_STORAGE_ADD_CHAR_TO_CLUB,
		false );

    // 클럽창고에 삽입 메시지
    // 삽입이 실패하면 다시 손에 들어야 한다.
    GLMSG::NET_CLUB_INVEN_TO_STORAGE_FA SendData( m_ClubDbNum, m_CharDbNum, Channel, PosX, PosY, HoldItem );
    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, SendData );
    SendToAgent( NET_MSG_CLUB_INVEN_TO_STORAGE_FA, SendBuffer, true );
	return S_OK;
}

//bool GLChar::HoldItemRestore(const SITEMCUSTOM& Item)
//{
//    // 이미 아이템을 들고 있다. 인벤토리에 넣어준다.    
//    // 강제로라도 넣어야 한다.
//    if (InvenInsertItem(Item, true, true))
//        return true;
//    else
//        return false;
//}
/*
HRESULT GLChar::MsgReqClubStorageSplit ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_STORAGE_SPLIT *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_SPLIT *)nmg;
	
	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if ( !pCLUB )
	{
		return S_OK;
	}

	if ( pCLUB->MasterCharDbNum() != m_CharDbNum )
	{
		return S_OK;
	}

	if ( pNetMsg->dwChannel >= MAX_CLUBSTORAGE )
	{
		return E_FAIL;
	}

	if ( !IsKEEP_CLUB_STORAGE(pNetMsg->dwChannel) )
	{
		return S_FALSE;
	}

	BOOL bOk(FALSE);

	GLInventory &sInventory = pCLUB->m_cStorage[pNetMsg->dwChannel];

	//	Note : 분리 요청 아이템 찾음.
	SINVENITEM* pInvenItem = sInventory.FindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )									return E_FAIL;

	//	Note : 아이템 정보 가져오기.
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.sNativeID );
	if ( !pItem )
        return E_FAIL;

	//	Note : 분리 가능한 아이템인지 점검.
	bool bSPLIT(false);
	bSPLIT = ( pItem->ISPILE() );
	if ( !bSPLIT )													return E_FAIL;
	if ( pNetMsg->wSplit < 1 )										return E_FAIL;
	if ( pInvenItem->sItemCustom.wTurnNum <= pNetMsg->wSplit  )		return E_FAIL;

	//	Note : 원본 아이템의 변화된 갯수.
	WORD wORGIN_SPLIT = pInvenItem->sItemCustom.wTurnNum - pNetMsg->wSplit;

	//	Note : 분리되어 새로 생기는 아이템.
	SITEMCUSTOM sNEW_ITEM = pInvenItem->sItemCustom;
	sNEW_ITEM.wTurnNum = pNetMsg->wSplit;

	//	Note : 새로 생기는 아이템 위치 찾기.
	WORD wInvenPosX(0), wInvenPosY(0);
	bOk = sInventory.FindInsertable( wInvenPosX, wInvenPosY );
	if ( !bOk )	return E_FAIL;

	//	Note : 새로 생긴 아이템 넣기.
	bOk = sInventory.InsertItem( sNEW_ITEM, wInvenPosX, wInvenPosY );
	if ( !bOk )											return E_FAIL;

	//	Note : 새로 생긴 아이템 정보 가져오기.
	SINVENITEM* pNewItem = sInventory.GetItem( wInvenPosX, wInvenPosY );
	if ( !pNewItem )									return E_FAIL;

	//	Note : 원본 아이템의 갯수 변경 시킴.
	pInvenItem->sItemCustom.wTurnNum = wORGIN_SPLIT;

	//	Note : 인벤에 새로 분리되어 들어 가는 아이템.
	GLMSG::SNET_CLUB_STORAGE_INSERT NetMsgNew;
	NetMsgNew.dwChannel = pNetMsg->dwChannel;
	NetMsgNew.Data = *pNewItem;
	SendToClient(&NetMsgNew);

	//	Note : 원본 아이템 갯수 변경.
	GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM NetMsgStorage;
	NetMsgStorage.dwChannel = pNetMsg->dwChannel;
	NetMsgStorage.wPosX = pNetMsg->wPosX;
	NetMsgStorage.wPosY = pNetMsg->wPosY;
	NetMsgStorage.wTurnNum = wORGIN_SPLIT;
	SendToClient( &NetMsgStorage );

	return S_OK;
}
*/

HRESULT GLChar::MsgReqClubStorageSaveMoney( NET_MSG_GENERIC* nmg )
{
	if ( !GLUseFeatures::GetInstance().IsUsingClubLockerMoneySave() ) return S_FALSE;

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
    GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_CF RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return S_FALSE;

    if ( RecvData.m_MagicNum != GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_CF::MAGIC_NUM )
    {
        sc::writeLogError( 
            sc::string::format( 
                "HACKING! MsgReqClubStorageSaveMoney ChaDbNum %1% Money %2% MagicNum %3%", 
                m_CharDbNum, RecvData.m_DepositMoney, RecvData.m_MagicNum ) );
        return S_FALSE;
    }

	std::tr1::shared_ptr< GLClubField > spClub = GetMyClub();
	if ( !spClub )
        return S_FALSE;

    if ( !spClub->HaveMoneyAuthority( m_CharDbNum ) )
        return S_FALSE;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubStorageSaveMoney", m_CharDbNum ) );
	}

	if ( RecvData.m_DepositMoney <= 0 || GetInvenMoney() < RecvData.m_DepositMoney )
    {
        sc::writeLogError( 
            sc::string::format( 
                "HACKING! MsgReqClubStorageSaveMoney %1% < %2% ChaDbNum %3%", 
                GetInvenMoney(), 
                RecvData.m_DepositMoney, 
                m_CharDbNum ) );
        return S_FALSE;
    }

	CheckMoneyUpdate( GetInvenMoney(), RecvData.m_DepositMoney, FALSE, "Club Storage Save Money." );

	SubtractInvenMoneyServer( RecvData.m_DepositMoney, SUBTRACT_INVEN_MONEY_CLUB_STORAGE_SAVE );
	
	// 금액 로그.
	// 통계 및 로그 수정
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB, m_ClubDbNum, RecvData.m_DepositMoney, ITEM_ROUTE_CLUB, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CLUB_STORAGE_SAVE );
	// 통계 및 로그 수정
	LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB,           0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
	
	// 클라이언트에 돈 액수 변화를 알려줌
	SendInvenMoneyUpdate();

    // Agent 에 입금요청
    GLMSG::NET_CLUB_STORAGE_SAVE_MONEY_FA SendData( m_CharDbNum, m_ClubDbNum, RecvData.m_DepositMoney );
    msgpack::sbuffer SendBuffer;
    msgpack::pack( SendBuffer, SendData );
    SendToAgent( NET_MSG_CLUB_STORAGE_SAVE_MONEY_FA, SendBuffer );

	return S_OK;
}

void GLChar::MsgReqClubStorageSaveMoneyAF(LONGLONG Money)
{	
	if ( !GLUseFeatures::GetInstance().IsUsingClubLockerMoneySave() ) return;

    if ( Money <= 0 )
        return;

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubStorageSaveMoneyAF", m_CharDbNum ) );
	}

	AddInvenMoneyServer( Money, ADD_INVEN_MONEY_CLUB_STORAGE_SAVE_ROLLBACK );

    SendInvenMoneyUpdate();
	
	// 통계 및 로그 수정
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
}

HRESULT GLChar::MsgReqClubStorageDrawMoney( DWORD ClubDbNum, LONGLONG Money )
{		
	if ( !GLUseFeatures::GetInstance().IsUsingClubLockerMoneyDraw() ) return S_FALSE;

	LONGLONG lnMoney = Money;
	if ( lnMoney <= 0 )
    {
         sc::writeLogError( sc::string::format( "HACKING! MsgReqClubStorageDrawMoney %1% < 0 ChaDbNum %3%", lnMoney, m_CharDbNum ) );
        return S_FALSE;
    }

	GLChar* pTarChar = GetCharByDbNum( m_CharDbNum );
	if ( !pTarChar )
	{		
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgReqClubStorageDrawMoney", m_CharDbNum ) );
		return S_FALSE;
	}

	CheckMoneyUpdate( GetInvenMoney(), lnMoney, TRUE, "Club Storage Draw Money." );

	AddInvenMoneyServer( lnMoney, ADD_INVEN_MONEY_CLUB_STORAGE_DRAW );
	
	// 금액 로그    
	// 통계 및 로그 수정
    LogMoneyExchange( gamelog::ID_CHAR, m_CharDbNum, gamelog::ID_CLUB, 0, GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );
	
	// 클라이언트의 인벤토리에 있는 돈 액수 변화를 알려줌.
	SendInvenMoneyUpdate();

	return S_OK;
}
/*
HRESULT GLChar::MsgReqClubInComeReNew(NET_MSG_GENERIC* nmg)
{
	// GLMSG::SNET_CLUB_INCOME_RENEW

	if (m_ClubDbNum == CLUB_NULL)
		return S_OK;

	std::tr1::shared_ptr<GLClubField> pCLUB = GetMyClub();
	if (!pCLUB)
		return S_OK;

	if (pCLUB->MasterCharDbNum() != m_CharDbNum)
		return S_OK;

	GLMSG::SNET_CLUB_INCOME_UP NetMsgUp;
	NetMsgUp.dwClubID = pCLUB->m_DbNum;
	SendToAgent(&NetMsgUp);
	return S_OK;
}
*/
