#include "../pch.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../../SigmaCore/Json/MinJson.h"
#include "../../Enginelib/Common/profile.h"
#include "../../RanLogic/Msg/LoginMsg.h"
#include "../../RanLogic/Msg/RegisterMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/Msg/JsonMsgDefine.h"
#include "../../RanLogic/Msg/PVEmsg.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/Msg/GLContrlTexasHoldemMsg.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../Util/GLItemCheck.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Database/DbManager.h"
#include "../Database/DBAction/DbActionGame.h"
#include "../Client/NetFieldClientMan.h"
#include "./s_CFieldServer.h"

#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../TriggerManager/GLTriggerManagerFD.h"
#include "../Move/MoveManagerField.h"

#include "../ProvideRewardSystem/GLProvideRewardManager.h"

#include "../../EngineLib/Common/profile.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CFieldServer::initPacketFunc()
{
    m_MsgFunc[NET_MSG_JSON].Msgfunc                  = &CFieldServer::MsgJson;
	m_MsgFunc[NET_MSG_HEARTBEAT_SERVER_REQ].Msgfunc  = &CFieldServer::MsgHeartbeatServerAnswer;
	m_MsgFunc[NET_MSG_HEARTBEAT_CLIENT_ANS].Msgfunc  = &CFieldServer::MsgHeartbeatClientAns;
	m_MsgFunc[NET_MSG_I_AM_AGENT].Msgfunc            = &CFieldServer::MsgIamAgent;
	//m_MsgFunc[NET_MSG_LOGIN_FB_SUB_OK_FIELD].Msgfunc = &CFieldServer::MsgLoginInfoAgent;
	m_MsgFunc[NET_MSG_CLIENT_VERSION_CS].Msgfunc     = &CFieldServer::MsgVersion;
	m_MsgFunc[MET_MSG_GAME_JOIN_FIELDSVR].Msgfunc    = &CFieldServer::MsgGameJoinChar;
	m_MsgFunc[NET_MSG_FIELD_REQ_JOIN].Msgfunc        = &CFieldServer::MsgFieldReqJoin;
	m_MsgFunc[NET_MSG_JOIN_FIELD_IDENTITY].Msgfunc   = &CFieldServer::MsgJoinInfoFromClient;    

    m_MsgFunc[NET_MSG_CLUB_GRADE_NAME_CHANGE_AF].Msgfunc = &CFieldServer::ClubGradeNameChangeAF;
    m_MsgFunc[NET_MSG_CLUB_MEMBER_GRADE_CHANGE_AF].Msgfunc = &CFieldServer::ClubMemberGradeChange;
    m_MsgFunc[NET_LOG_ATTENDANCE_DS].Msgfunc      = &CFieldServer::LogAttendanceDS;
    m_MsgFunc[NET_LOG_ATTENDANCE_TASK_DS].Msgfunc = &CFieldServer::LogAttendanceTaskDS;
	
	//// for global auth
	//m_MsgFunc[NET_MSG_AUTH_CERTIFICATION_ANS].Msgfunc = &CFieldServer::MsgAuthCertificationResponse;
}

int CFieldServer::InsertMsg(int nClient, void* pMsg)
{
    if (pMsg)
	{
		NET_MSG_GENERIC* pPacket = (NET_MSG_GENERIC*) pMsg;
		m_pRecvMsgManager->MsgQueueInsert(nClient, pPacket, pPacket->Size());
	}
	return 0;
}

int CFieldServer::InsertMsg( int nClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc/*=false*/ )
{
	NET_MSG_PACK MsgPack;
	if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
		InsertMsg(nClient, &MsgPack);

	return 0;
}


void CFieldServer::MsgProcess( MSG_LIST* pMsg )
{
	if (!pMsg)
		return;

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*)(pMsg->Buffer);

	if (!nmg)
		return;

	if (nmg->Size() > NET_DATA_BUFSIZE)
		return;

	// 서버 Hang 때문에 임시로 프로파일러에 추가해서 사용
	if( g_bProfileEx == true ) g_ExProfile.BeginNetworkProfile(nmg->nType);

	DWORD dwClient = pMsg->dwClient;
	DWORD dwGaeaID = GAEAID_NULL;


	if (dwClient >= (DWORD)m_pClientManager->GetMaxClient())
	{
		//GASSERT(0&&"m_nMaxClient43");
		// 서버 Hang 때문에 임시로 프로파일러에 추가해서 사용
		if( g_bProfileEx == true ) g_ExProfile.EndNetworkProfile(nmg->nType);
		return;
	}	

	if (dwClient == m_SessionServerSlot)
    {
		// 서버 Hang 때문에 임시로 프로파일러에 추가해서 사용
		if( g_bProfileEx == true ) g_ExProfile.EndNetworkProfile(nmg->nType);

        SessionMsgProcess(pMsg);
		return;
    }
    else if (dwClient == m_CacheServerSlot)
    {
		// 서버 Hang 때문에 임시로 프로파일러에 추가해서 사용
		if( g_bProfileEx == true ) g_ExProfile.EndNetworkProfile(nmg->nType);

        CacheMsgProcess(pMsg);
		return;
    }
	else if ( dwClient == m_IntegrationCacheServerSlot )
	{
		// 서버 Hang 때문에 임시로 프로파일러에 추가해서 사용
		if ( g_bProfileEx == true )
			g_ExProfile.EndNetworkProfile( nmg->nType );

		IntegrationCacheMsgProcess( pMsg );
		return;
	}
    else if ( dwClient == m_dwAgentSlot)
    {
        AgentMsgProcess(pMsg);
    }
	else
	{
		if (m_pClientManager->IsClientSlot(dwClient))
		{
			//클라이언트에서 보낸 메시지에 대해서만 ClientManager의 GaeaID가 유효하다.

			dwGaeaID = m_pClientManager->GetGaeaID(dwClient);

			//
			//NET_MSG_JOIN_FIELD_IDENTITY 메시지 이후에는 반드시 GaeaID가 발급된다.
			//  혹은 nmg->Type() > NET_MSG_GCTRL 와 같이 비교해도 무난하다.
			if (nmg->Type() != NET_MSG_JOIN_FIELD_IDENTITY && dwGaeaID == GAEAID_NULL)
			{		
				// 서버 Hang 때문에 임시로 프로파일러에 추가해서 사용
				if( g_bProfileEx == true ) g_ExProfile.EndNetworkProfile(nmg->nType);
				return;
			}
		}
	}


	//
	// 2.여기부터 AgentServer나 Client로부터 온 메시지들을 처리한다.
	//
	if( dwClient >= (DWORD)m_nMaxClient ) 
	{
		// 서버 Hang 때문에 임시로 프로파일러에 추가해서 사용
		if( g_bProfileEx == true ) g_ExProfile.EndNetworkProfile(nmg->nType);
		return;
	}
	
	EMNET_MSG		emType = nmg->nType;
	if( m_pClientManager->IsClientSlot( dwClient ) )
	{
		if( IsReserveCloseClient( dwClient ) )
		{
			// 서버 Hang 때문에 임시로 프로파일러에 추가해서 사용
			if( g_bProfileEx == true ) g_ExProfile.EndNetworkProfile(nmg->nType);
			return;
		}

		if ( emType != NET_MSG_GCTRL_GOTO )
		{
			if( m_pClientManager->IncPacketCount( dwClient, emType ) )		
			{
				// 서버 Hang 때문에 임시로 프로파일러에 추가해서 사용
				if( g_bProfileEx == true ) g_ExProfile.EndNetworkProfile(nmg->nType);

				m_pClientManager->WritePacketLog( dwClient );

				sc::writeLogError(
					sc::string::format(
					"MsgProcess there are so many packets in short time that mean hack : %1%", dwClient ) );

				ReserveCloseClient( dwClient );
				return;
			} //if
		} //if
	} //if
	
	if( emType < 0 || emType >= NET_MSG_GCTRL_MAX )
	{
		// 서버 Hang 때문에 임시로 프로파일러에 추가해서 사용
		if( g_bProfileEx == true ) g_ExProfile.EndNetworkProfile(nmg->nType);

		sc::writeLogError(
			sc::string::format(
				"MsgProcess Out of range or illegal message %1%", emType));

		CloseClient( dwClient, net::KICKOUT_ILLEGAL_MSG );
		return;
	} //if


	if ( m_MsgFunc[emType].Msgfunc )
	{
		( this->*m_MsgFunc[emType].Msgfunc )( dwClient, nmg );
	}
	else if ( emType > NET_MSG_GCTRL )							// Insert to message queue
	{
		BOOL bReturnValue = m_pGaeaServer->MsgProcess( nmg, dwClient, dwGaeaID );
		if ( FALSE == bReturnValue )
		{
			// GS 413번 NET_MSG_GCTRL_2_FRIEND_FB 이 에이전트에서 필드로 왔는데 왜 인지는 모름
			// 메시지 처리에 실패하고 필드가 에이전트 슬롯을 죽여버린다.
			if ( m_pClientManager->IsClientSlot( dwClient ) )
			{
				// 태국 무작위 패킷을 계속 날리는 유저가 있어서 추가한다. 로그를 계속해서 출력하다가 문제가 생긴 것 같다.
				// 그냥 종료시켜버린다.
				CloseClient( dwClient, net::KICKOUT_ILLEGAL_MSG );
			}
		}
	}

	// 서버 Hang 때문에 임시로 프로파일러에 추가해서 사용
	if( g_bProfileEx == true ) g_ExProfile.EndNetworkProfile(nmg->nType);
}

void CFieldServer::MsgJson(DWORD ClientSlot, NET_MSG_GENERIC* pMsg)
{
    GAEAID GaeaId = GAEAID_NULL;
    //클라이언트에서 보낸 메시지에 대해서만 ClientManager의 GaeaID가 유효하다.
    if (m_pClientManager->IsClientSlot(ClientSlot))
        GaeaId = m_pClientManager->GetGaeaID(ClientSlot);

    NET_JSON* pPacket = (NET_JSON*) pMsg;
    char JsonStr[NET_JSON::BUF_SIZE] = {0};
    StringCchCopy(JsonStr, NET_JSON::BUF_SIZE, pPacket->Buffer);
    MsgJsonParser(ClientSlot, GaeaId, std::string(JsonStr));
}

bool CFieldServer::MsgJsonParser(DWORD ClientSlot, DWORD GaeaID, const std::string& JsonStr)
{
    json_spirit::mValue Value;
    if (!json_spirit::read(JsonStr, Value))
    {
        // Json read error
        sc::writeLogError(std::string("MsgJsonParser json read error"));
        return false;
    }

    const json_spirit::mObject& JsonObject = Value.get_obj();
    int MessageType = 0;
    if (!sc::Json::find_value(JsonObject, "M", MessageType))
    {
        sc::writeLogError(std::string("Json message type 'M' not exist"));
        return false;
    }

    sc::writeLogError( sc::string::format( "Unknown Json message %1%", MessageType) );
    return true;
}

void CFieldServer::MsgIamAgent(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
    NET_I_AM_AGENT* pPacket = (NET_I_AM_AGENT*) nmg;
    if (pPacket->MagicNum != NET_I_AM_AGENT::MAGIC_NUM)
    {
        sc::writeLogError(
            sc::string::format(
                "NET_I_AM_AGENT invalid magic num %1%", pPacket->MagicNum));
        return;
    }

    unsigned int SCrcValue = pPacket->CrcValue;
    unsigned int TCrcValue = sc::string::getCrc32(pPacket, pPacket->Size() - sizeof(unsigned int));
    if (SCrcValue != TCrcValue)
    {
        sc::writeLogError(
            sc::string::format(
                "NET_I_AM_AGENT invalid crc %1%/%2%", SCrcValue, TCrcValue));
        return;
    }

    sc::writeLogInfo(
        sc::string::format(
            "NET_I_AM_AGENT Agent slot %1%", dwClient));
	
	m_dwAgentSlot = dwClient;

	GASSERT((m_pClientManager->GetSlotAgent() == m_dwAgentSlot)&&"AgentSlot is weird!");

	//m_pClientManager->SetSlotAgent(dwClient);
	m_pGaeaServer->SetAgentSlot(dwClient);	
	m_pGaeaServer->ReportUsedMemoryPerLand();
	m_pGaeaServer->SetAgentSlot(dwClient);

	// TexasHoldem
	// 필드서버에서 에이전트서버로 환전 리스트 받아오도록 내부메시지 발생
	GLMSG::NET_TEXAS_HOLD_EM_CHIP_MAP_REQUEST_INNER NetMsg;
	InsertMsg( DBACTION_CLIENT_ID, &NetMsg );

	m_pGaeaServer->UnblockForEmergencySavedUser();
}

void CFieldServer::MsgHeartbeatClientAns(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	m_pClientManager->SetHeartBeat( dwClient );
}

void CFieldServer::MsgHeartbeatServerAnswer(DWORD dwClient, NET_MSG_GENERIC* nmg)
{	
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(std::string("[HEARTBEAT] Recv Heartbeat Request from Agent Svr"));
#endif

	NET_HEARTBEAT_SERVER_ANS msg;
	//SendAgent( dwClient, &msg );
	SendAgent(&msg);
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo(std::string("[HEARTBEAT] Send Heartbeat Response to Agent Svr"));
#endif

}

// Client->Field : Join Msg
void CFieldServer::MsgJoinInfoFromClient(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	/*
		mjeon.AF
		 Client가 해당 Field로 접속한 이후, 최초로 보내는 인증 메시지이다.
		 그러므로, 이 곳에서 ClientID에 대해 GaeaID 및 ChaNum이 setup 된다.
	*/
	
	NET_GAME_JOIN_FIELD_IDENTITY_CF* pMsg = (NET_GAME_JOIN_FIELD_IDENTITY_CF*)nmg;

	if (!pMsg)
		return;

	//mjeon.instance.transfer
	if ( !(pMsg->emType==EMJOINTYPE_FIRST || 
           pMsg->emType==EMJOINTYPE_MOVEMAP|| 
           pMsg->emType==EMJOINTYPE_REBIRTH || 
           pMsg->emType==EMJOINTYPE_TRANSFER ||
		   pMsg->emType==EMJOINTYPE_PVEINDUN ||
		   pMsg->emType==EMJOINTYPE_GATE) )
		return;

	if(!m_pClientManager->IsClientSlot(dwClient))
		return;
	
	GLChar* pChar = m_pGaeaServer->GetChar(pMsg->dwGaeaID);
	if (!pChar)
		return;

	if ( pMsg->ck.guid != pChar->GetCryptKey().guid )
	{
		sc::writeLogError( sc::string::format( "MsgJoinInfoFromClient gaeaid hack. userid %1% chanum %2%", m_pClientManager->GetUserID( dwClient ), m_pClientManager->ChaDbNum( dwClient ) ) );
		return;
	}	

	m_pClientManager->SetCryptKey(dwClient, pMsg->ck);
	m_pClientManager->SetGaeaID(dwClient, pMsg->dwGaeaID);	

	pChar->SetClientSlot(dwClient);

	m_pClientManager->SetUserID(dwClient, pChar->m_szUID);
	
	m_pClientManager->SetUserDbNum(dwClient, pChar->GetUserID());
	m_pClientManager->SetChaNum(dwClient, pChar->CharDbNum());	//mjeon.AF

	// 국가 정보 세팅;
	m_pClientManager->SetCountryInfo( dwClient, pChar->GetCountryInfo() );

	// 입장 처리;
	MoveSystem::CMoveManagerField::Instance()->OnJoinClient(
		pChar->CharDbNum(),
		pMsg->previousMapID,
		pMsg->typeAuthority,
		pMsg->emType );

	/*GLLandMan* pLand = pChar->GetLandMan();
	if ( pLand != 0 )
	{
		GLMSG::NET_INSTANCE_MOVE_MAP _moveMapMessage(pChar->CharDbNum(), pMsg->previousMapID, D3DXVECTOR3(0.0f, 0.0f, 0.0f), -1, pLand->GetGaeaMapID());
		SendMySelf(0, &_moveMapMessage);

		// 인던입장 이벤트 발생;
		// 마지막으로 이곳에서 입장 위치에 대하여 조정할 수 있는 기회를 제공.;
		InstanceSystem::InstanceField* const _pInstance = pLand->GetScriptInstance();
		if ( _pInstance )
		{
			D3DXVECTOR3 _vAdsjustPosition = pChar->GetPosition();
			if ( _pInstance->EventJoin(pChar->CharDbNum(), pLand, _vAdsjustPosition) )
			{
				// 입장시 위치 조정이 있다면 해당하는 위치로 바꾼다.;
				pChar->SetPosition(_vAdsjustPosition);
			}
		}
	}	

	/// 현재 관전자의 개념이 토너먼트에 한정되어 있으므로;
	/// 토너먼트가 켜져 있을 경우에만 동작해야 한다;
	if ( GLUseFeatures::GetInstance().IsUsingTournament() )
	{
		if ( pMsg->typeAuthority == InstanceSystem::EMAUTHORITY_OBSERVER )
		{
			pChar->SetActState(EM_ACT_OBSERVE);
			pChar->m_sSummonable.Clear();

			GLMSG::NET_TOURNAMENT_GAME_POPUP_FC _message(true);
			SendClient(dwClient, &_message);
		}
		else
		{
			pChar->ReSetActState(EM_ACT_OBSERVE);
			//pChar->m_sSummonable.Clear();

			GLMSG::NET_TOURNAMENT_GAME_POPUP_FC _message(false);
			SendClient(dwClient, &_message);
		}
	}

	//클라이언트에 Field 서버와의 접속 인식후 접속 종류에 따른 응답
	switch ( pMsg->emType )
	{
	case EMJOINTYPE_FIRST:
		{
			//	캐릭터 생성 정보 정보 전송.
			pChar->MsgGameJoin();

            // 캐릭터 최초접속 트리거
            triggermanager::GLTriggerManagerFD::GetInstance().Get_Trigger_Connect_To_Field().Do(pChar);
		}
		break;

	case EMJOINTYPE_TRANSFER:
	case EMJOINTYPE_MOVEMAP:
		{			
			if ( pLand )
			{
				//	게이트 이동 확인 메시지.
				GLMSG::SNETREQ_GATEOUT_FB NetMsgGateOutFB;
				NetMsgGateOutFB.bCullByObjectMap = pLand->IsCullByObjectMap();
				NetMsgGateOutFB.dwChaNum = pChar->CharDbNum();
				NetMsgGateOutFB.emFB = EMCHAR_GATEOUT_OK;
				NetMsgGateOutFB.sMapID = pChar->GetCurrentMap();
				NetMsgGateOutFB.vPos = pChar->GetPosition();
				SendAgent(&NetMsgGateOutFB);

				pChar->MsgFieldMoveReset ();
			}			
		}
		break;

	case EMJOINTYPE_REBIRTH:
		{			
			if (pLand)
			{ //	부활 확인 메시지.
				GLMSG::NET_CHAR_RESURRECT_FAFC NetMsg(
					pChar->GetCurrentMap(),
					pChar->GetPosition(),
					pChar->GETHP(),
					pChar->GETMP(),
					pChar->GETSP(),
					pLand ? pLand->IsCullByObjectMap() : true,
					pChar->GetCharID()
					);
	            
				SendAgent(&NetMsg);

				pChar->MsgFieldMoveReset ();
			}
		}
		break;

    case EMJOINTYPE_PVEINDUN:
        {

            if (pLand)
            {
                //	개이트 이동 확인 메시지.
                GLMSG::SNETREQ_GATEOUT_FB NetMsgGateOutFB;
				NetMsgGateOutFB.bCullByObjectMap = pLand->IsCullByObjectMap();
                NetMsgGateOutFB.dwChaNum = pChar->CharDbNum();
                NetMsgGateOutFB.emFB = EMCHAR_GATEOUT_OK;
                NetMsgGateOutFB.sMapID = pChar->GetCurrentMap();
                NetMsgGateOutFB.vPos = pChar->GetPosition();
                SendAgent(&NetMsgGateOutFB);

                // 인던 이동 확인 메시지
                GLMSG::NET_PVE_ENTRANCE_INSTANT_MAP_ACK_FA NetMsg;
                NetMsg.dwCharDbNum = pChar->CharDbNum();
                NetMsg.sInDunID = pLand->GetPVEIndunID();
                NetMsg.bResult = true;
                msgpack::sbuffer buf;
                msgpack::pack(buf, NetMsg);
                SendAgent(NET_MSG_PVE_ENTRANCE_INSTANT_MAP_ACK_FA, buf);

                pChar->MsgFieldMoveReset();
            }
        }
        break;
	};

    pChar->Online();*/
}

void CFieldServer::MsgSndCryptKey(DWORD dwClient)
{
	CRYPT_KEY ck = m_pClientManager->GetNewCryptKey(); // Get crypt key
			
	NET_CRYPT_KEY nck;	
	nck.nType   = NET_MSG_SND_CRYT_KEY;
	nck.ck		= ck;
	SendClient(dwClient, &nck);
	m_pClientManager->SetCryptKey(dwClient, ck); // Set crypt key
}

// Filed->Client
// 일반적 채팅 메시지 전송
void CFieldServer::MsgSndChatNormal(DWORD dwClient, const char* szName, const char* szMsg)
{
    if (!szMsg)
        return;

	GLMSG::NET_CHAT_LINK_FB nc;
    nc.SetData(NET_MSG_CHAT_LINK_FB, CHAT_TYPE_NORMAL, szName, szMsg);
	GLChar* pChar = m_pGaeaServer->GetCharByDbNum(m_pGaeaServer->GetChaDbNumByChaName(szName));
	if ( pChar != NULL )
	{
		if ( pChar->UserLevel() >= USER_USER_GM )
			nc.emType = static_cast<EMNET_MSG_CHAT>(nc.emType | CHAT_ADD_TYPE_FOPR_GM);
	}
	SendClient(dwClient, &nc);
}

// Agent->Field : 캐릭터 접속.
void CFieldServer::MsgGameJoinChar(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	if ( !m_pClientManager->IsAgentSlot( dwClient ) )
	{
		return;
	}

	if (!nmg)
		return;

	int nRetCode = 0;
	DWORD dwItemNum = 0;

	//이 시점에 클라이언트와 필드사이의 세션은 아직 생성되지 않았다.

	//AF
	NET_GAME_JOIN_FIELDSVR* pMsg = (NET_GAME_JOIN_FIELDSVR*)nmg;

    if ( m_pGaeaServer->FindSaveDBUserID( pMsg->nUserNum ) || m_pGaeaServer->FindCreatingCharUserID( pMsg->nUserNum ) )
    {
        sc::writeLogError(
            sc::string::format(
                "Duplicate game join! MsgGameJoinChar UserDbNum %1%", pMsg->nUserNum));
    }
    else        
    {
	    m_pGaeaServer->SetCreatingCharUserID(pMsg->nUserNum);

		SCHARDATA2* pTempCharData = new SCHARDATA2();
		pTempCharData->m_sSummonable = pMsg->sSummonable;

	    AddGameAdoJob(
            db::DbActionPtr(
                new db::CharacterInfoAndJoinField( m_nServerGroup, pMsg, pTempCharData, m_fChaJoinRandom) ) );
    }
}

//DF
void CFieldServer::MsgFieldReqJoin(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	if ( dwClient != DBACTION_CLIENT_ID )
	{
		sc::writeLogError( std::string( "CFieldServer::MsgFieldReqJoin() - dwClient != DBACTION_CLIENT_ID" ) );
		GLMSG::NET_CHAR_JOIN_DF* pMsg = ( GLMSG::NET_CHAR_JOIN_DF* )nmg;
		if ( pMsg != NULL && pMsg->m_pCharData != NULL )
		{
			delete pMsg->m_pCharData;
		}
		return;
	}

	GLMSG::NET_CHAR_JOIN_DF* pMsg = (GLMSG::NET_CHAR_JOIN_DF*)nmg;
	
	if (pMsg == NULL)
		return;

	SCHARDATA2* const pCharData(pMsg->m_pCharData);

	if ( pCharData == NULL )
	{
		sc::writeLogError(std::string("CFieldServer::MsgFieldReqJoin() - pCharData is NULL."));
		return;
	}	
	
	if ( pMsg->fMagicNum != m_fChaJoinRandom )
	{
		sc::writeLogError( sc::string::format( "CFieldServer::MsgFieldReqJoin() - pMsg->fMagicNum != m_fChaJoinRandom chanum:%d", pCharData->m_CharDbNum ) );
		delete pCharData;
		return;
	}

	// 아이템 유효성 검사
	int ValidationResult = Item::GLItemValidation::Instance()->Do( GetServiceProvider(), pCharData );
	switch ( ValidationResult )
	{
	case Item::eInvalidRandomOption:
		{
			m_pGaeaServer->ResetCreatingCharUserID( pCharData->GetUserID() );
			GLMSG::SNETLOBBY_CHARJOIN_FB NetMsgFB( EMCJOIN_FB_OVERSTAT_RANDOMOPTION );
			NetMsgFB.dwChaNum = pCharData->m_CharDbNum;
			SendAgent( &NetMsgFB );
			delete pCharData;
			return;
		}
		break;
	default:
		break;
	}

	GLMSG::NET_CHAR_JOIN_DF::SINFO &sINFO = pMsg->m_sINFO;

	EMIP_BONUS_CLASS emIPBonus = sINFO.m_emIPBonus;

	if (m_ServiceProvider == SP_VIETNAM)
	{
		if( sINFO.m_sVietnamSystem.lastOffLineTime != 0 )
		{
			CTime crtTime	   = CTime::GetCurrentTime();
			CTime lastOffTime  = sINFO.m_sVietnamSystem.lastOffLineTime;
			// 오프라인한 날짜가 다르면 초기화 한다.
			bool bInitialize = FALSE; // 테스트로 무조건 초기화 한다.

			// 년도, 달, 날짜가 다르면 초기화 한다.
			if( lastOffTime.GetYear() != crtTime.GetYear() ||
				lastOffTime.GetMonth() != crtTime.GetMonth() ||
				lastOffTime.GetDay() != crtTime.GetDay() )
			{
				sc::writeLogError(sc::string::format("=-=-=-=-=- INFO:Vietnam Init =-=-=-=-=- gameTime %d", sINFO.m_sVietnamSystem.gameTime) );
				sINFO.m_sVietnamSystem.gameTime   = 0;
				sINFO.m_sVietnamSystem.loginTime  = crtTime.GetTime();
			}
		}

		pCharData->m_sVietnamSystem = sINFO.m_sVietnamSystem;
	}


#ifdef CH_PARAM_USEGAIN
	pCharData->m_sChinaTime	 = sINFO.m_sChinaTime;
#endif

	// 모든 국가에서 Tracing 기능을 사용할 수 있도록 변경
	//if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
	//{
		if( CheckTracingUser( pCharData->m_szUID, pCharData->GetUserID() ) != -1 )
		{
			pCharData->m_bTracingUser   = FALSE;
		}
	//}
	//else
	//{
	//	pCharData->m_bTracingUser   = FALSE;
	//}

	//
	//mjeon.AF
	//  AF/DF 메시지의 경우에는 m_pClientManager로부터 유효한 정보를 획득할 수 없다.
	//  그러므로 메시지를 통해 관련 정보들을 전달한다.	
	//	
	DWORD dwGaeaID		= pMsg->m_dwGaeaID;
	DWORD dwChaNum		= pCharData->m_CharDbNum;	

	//
	//이 시점에는 아직 Client <-> FieldServer 사이에 세션이 맺어지지 않았다. 그러므로 유효한 ClientID가 존재하지 않는다.
	//
	GLChar *pGLChar = m_pGaeaServer->CreateCharacter(
        pCharData,
        /*dwClientID*/dwChaNum,
        dwGaeaID,
		pMsg->m_PartyID,
        pMsg->m_Country,
		pMsg->m_dwCountryID,
        FALSE,
        sINFO.m_sStartMap,
        sINFO.m_dwStartGate,
        sINFO.m_vStartPos,
        sINFO.m_emType,
        sINFO.m_sEventTime.loginTime,
        emIPBonus);	
	if (!pGLChar)
	{
		sc::writeLogError(std::string("CFieldServer::MsgFieldReqJoin() - Initialized character instance failed"));
		m_pGaeaServer->ResetCreatingCharUserID(pCharData->GetUserID());
		delete pCharData;
		return;
	}

//	if (pGLChar->m_wLevel == 1 && pGLChar->m_vSavePos.x == 0.0f && pGLChar->m_vSavePos.z == 0.0f)
//	{
//		if (pGLChar->GetInvenMoney() != 0)
//		{			
//			sc::writeLogError(
  //              sc::string::format(
	//						"#### Money is not correct. Name %1% Money %2% ####",
	//						pGLChar->m_szName,
	//						pGLChar->GetInvenMoney()));
	//		pGLChar->SetInvenMoney(0);
	//	}
//	}
//
	GLMSG::SNETPC_ACTSTATE NetMsgInner(sINFO.m_dwActState);
	pGLChar->MsgActState(&NetMsgInner);

	// 주/보조 무기 설정
	pGLChar->m_bUseArmSub = sINFO.m_bUseArmSub;

    // 출석부 준비 여부.
    if ( pMsg->bChangeField )
    {
        pGLChar->LoadExtraInfo();
        pGLChar->AttendanceLoad();
        pGLChar->ActivityLoad();
    }

	//시작시 퀘스트 시작	 
	pGLChar->QuestStartFromCharJOIN( EMQJ_JOIN );
	pGLChar->QuestStartFromCharJOIN( EMQJ_ATTENDANCE );
	
	// Agent 에 케릭터의 접속 성공을 알림.(Agent 가 Client 에게 새로운 Field 서버에 접속하게 요청.)
	GLLandMan* pLand(pGLChar->GetLandMan());
	const MapID& newMapID(pLand == NULL ? sINFO.m_sStartMap : pLand->GetMapID());

	NET_GAME_JOIN_FIELDSVR_FB MsgJoinFB(
        sINFO.m_emType,
        m_dwFieldServerID,
        dwGaeaID,
        dwChaNum,
        newMapID,
        pGLChar->GetPosition());
	//MsgJoinFB.dwSlotFieldAgent = dwClientID;

	if ( pMsg->cKey.guid != GUID_NULL )
	{
		pGLChar->SetCryptKey( pMsg->cKey );
	}

    // 캐릭터 리셋이 지정된 경우 캐릭터 리셋( 망각 효과 )
    if( pMsg->bResetCharacter )
		m_pGaeaServer->ResetChaSkillNew( pGLChar );

	// Fire Provide Reward Callback Function!
	if ( false == GLUseFeatures::GetInstance().IsUsingWorldBattle() )
	{
		ProvideRewardSystem::GLProvideRewardManager* pProvideRewardMan =
			ProvideRewardSystem::GLProvideRewardManager::Instance();
		if ( pProvideRewardMan )
			pProvideRewardMan->OnLoginCharacter( pGLChar->CharDbNum() );
	}

	
	SendAgent(&MsgJoinFB);
	m_pGaeaServer->ResetCreatingCharUserID(pCharData->GetUserID());

	// FieldServer.xml 에 FORCE_LOG_ITEM_RANDOM_OPTION 항목이 1일 경우에만 남긴다.
	// 캐릭터옵션 검사, 랜덤옵션이 남겨지지 않았다면 남기고, 오버스텟 아이템 검사한다. <- 오버 스텟 검사는 다른곳에서
	// FieldServer.xml 에 랜덤옵션 로그 및 검사 기능 사용에 대한 설정값이 추가되어야 하고
	// GaeaServer 생성할 때 파라미터로 넘겨야 한다.
	// 인벤토리, 장착창, 랜덤옵션 테이블에 강제로 삽입된 값이라고 설정하는 칼럼 추가 - InsertOption
	if ( m_pConfig && 1 == m_pConfig->GetForceLogItemRandomOption() )
	{
		if ( pMsg->CharacterOption & OPTION_ITEM_RANDOMOPTION_INSERT_DONE ? false : true )
		{
			pGLChar->LogRandomOptionAll();

			// 캐릭터옵션 업데이트
			int Option = pMsg->CharacterOption;
			Option |= OPTION_ITEM_RANDOMOPTION_INSERT_DONE;
			AddGameAdoJob( db::DbActionPtr( new db::SetCharacterOption( pGLChar->CharDbNum(), Option ) ) );
		}
	}

	delete pCharData;
}


void CFieldServer::MsgSndChatGlobal(const char* szChatMsg)
{
    if (szChatMsg)
    {
	    DWORD dwSndBytes = 0;
	    sc::writeLogInfo(std::string(szChatMsg));
	    GLMSG::NET_CHAT_LINK_FB ncf;
	    ncf.SetData(CHAT_TYPE_GLOBAL, szChatMsg);
	    SendAllClient(&ncf);
    }
}


void CFieldServer::MsgVersion(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	if (nmg)
    {	    
	    NET_CLIENT_VERSION_SC ncv(m_LauncherVersion, m_PatchVersion);
	    SendClient(dwClient, &ncv);
    }
}

int CFieldServer::ExecuteCommand(char* strCmd)
{
    if (!strCmd)
        return NET_ERROR;

    std::string strCommand(strCmd);
    std::vector<std::string> SplitVec;
    boost::split(SplitVec, strCommand, boost::algorithm::is_any_of(" "));
    if (SplitVec.empty())
        return NET_ERROR;
	
	if (SplitVec[0] == "print_ip")
	{
		// SERVER_UTIL::CAddressChecker::GetInstance()->Save();
	}			
	else if (SplitVec[0] == "chat")
	{
        // 서버 채팅 메시지, global		
		if (SplitVec.size() > 1)
			MsgSndChatGlobal(SplitVec[1].c_str());
	}
	else if (SplitVec[0] == "profile_on")
	{
        // PROFILE 사용.
		g_bProfile = TRUE;
		sc::writeLogInfo(std::string("PROFILE ON"));
	}	
	else if (SplitVec[0] == "profile_off")
	{
        // PROFILE 사용안함.
		g_bProfile = FALSE;
		sc::writeLogInfo(std::string("PROFILE OFF"));
	}
    else
    {
    }
	return 0;
}

void CFieldServer::MsgReferenceCountAF( DWORD dwClient, NET_MSG_GENERIC* nmg )
{
	const GLMSG::SNETPC_GM_REFERENCE_COUNT_CAF* pNetMsg = ( GLMSG::SNETPC_GM_REFERENCE_COUNT_CAF* )nmg;
	switch ( m_ServiceProvider )
	{
	case SP_OFFICE_TEST:
		{
		}
		break;
	default:
		{
			if ( pNetMsg->nCountMax < 100 || pNetMsg->nCountMax > 1000 )
			{
				sc::writeLogError(
					sc::string::format(
					"reference count only between 100 and 1000. your count is %1%", pNetMsg->nCountMax ) );
				return;
			}
		}
		break;
	}

	if ( m_pClientManager )
	{
		m_pClientManager->SetReferenceCountMax( pNetMsg->nCountMax );
		sc::writeLogInfo(
			sc::string::format(
			"current reference count is %1%", pNetMsg->nCountMax ) );
	}
}

BOOL CFieldServer::FindSaveDBUserID( DWORD dwUserDbNum )
{
	return m_pGaeaServer->FindSaveDBUserID( dwUserDbNum );
}
