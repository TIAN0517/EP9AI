#include "../pch.h"
#include "../../Enginelib/Common/profile.h"
#include "../../Enginelib/DxTools/DebugSet.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"

#include "../../MfcExLib/RanFilter.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/ApexMsg.h"
#include "../../RanLogic/Msg/LoginMsg.h"
#include "../../RanLogic/Msg/RegisterMsg.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/Msg/JsonMsgDefine.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../../RanLogic/Msg/GLContrlCountry.h"
#include "../../RanLogic/Event/GLIPEvent.h"
#include "../../RanLogic/Quest/GLQuestMan.h"

#include "../TriggerManager/GLTriggerManagerAG.h"
#include "../Database/DBAction/DbActionUser.h"
#include "../Database/DBAction/DbActionGame.h"
#include "../Database/DBAction/DbActionGameAttendance.h"
#include "../Database/DBAction/DbActionGamePost.h"
#include "../Database/DBAction/DbActionGameSNS.h"
#include "../Database/DbAction/DbActionGameCountry.h"
#include "../Client/NetAgentClientMan.h"
#include "../PacketObserver/GLPacketObserver.h"
#include "../Character/GLCharAG.h"
#include "./AgentServerConfigXml.h"
#include "./AgentServer.h"
#include "../Party/GLPartyAgent.h"
#include "../Move/MoveManagerServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CAgentServer::initPacketFunc()
{	
    m_MsgFunc[NET_MSG_JSON].Msgfunc = &CAgentServer::MsgJson;
	m_MsgFunc[NET_MSG_HEARTBEAT_CLIENT_ANS].Msgfunc = &CAgentServer::MsgHeartbeatClientAns;

	m_MsgFunc[NET_MSG_LOGIN_2].Msgfunc          = &CAgentServer::MsgLogIn;
	m_MsgFunc[IDN_NET_MSG_LOGIN].Msgfunc        = &CAgentServer::IdnMsgLogIn;
    //m_MsgFunc[NET_MSG_WORLD_BATTLE_LOGIN_DATA].Msgfunc = &CAgentServer::MsgWorldBattleLogin;
	m_MsgFunc[CHINA_NET_MSG_LOGIN].Msgfunc      = &CAgentServer::ChinaMsgLogin;
	m_MsgFunc[NET_MSG_REGISTER].Msgfunc			= &CAgentServer::MsgRegister;
	m_MsgFunc[THAI_NET_MSG_LOGIN].Msgfunc       = &CAgentServer::ThaiMsgLogin;
	m_MsgFunc[DAUM_NET_MSG_LOGIN].Msgfunc       = &CAgentServer::DaumMsgLogin;
	m_MsgFunc[TERRA_NET_MSG_LOGIN].Msgfunc      = &CAgentServer::TerraMsgLogin;
	m_MsgFunc[GSP_NET_MSG_LOGIN].Msgfunc        = &CAgentServer::GspMsgLogin;
	m_MsgFunc[EXCITE_NET_MSG_LOGIN].Msgfunc     = &CAgentServer::ExciteMsgLogin;
	m_MsgFunc[JAPAN_NET_MSG_LOGIN].Msgfunc      = &CAgentServer::JapanMsgLogin;
	m_MsgFunc[GS_NET_MSG_LOGIN].Msgfunc         = &CAgentServer::GsMsgLogin;
	m_MsgFunc[GS_NET_MSG_WB_LOGIN].Msgfunc      = &CAgentServer::GsMsgWBLogin;
	m_MsgFunc[GS_NET_MSG_WB_LOGIN_AW].Msgfunc   = &CAgentServer::GsMsgWBLoginAW;
	m_MsgFunc[GS_NET_MSG_WB_LOGIN_WA].Msgfunc   = &CAgentServer::GsMsgWBLoginWA;
	m_MsgFunc[GM_NET_MSG_LOGIN].Msgfunc         = &CAgentServer::GmMsgLogin;
	m_MsgFunc[US_NET_MSG_LOGIN].Msgfunc         = &CAgentServer::UsMsgLogin;
	m_MsgFunc[DAUM_NET_MSG_PASSCHECK].Msgfunc   = &CAgentServer::DaumMsgPassCheck;
	m_MsgFunc[TERRA_NET_MSG_PASSCHECK].Msgfunc  = &CAgentServer::TerraMsgPassCheck;
	m_MsgFunc[EXCITE_NET_MSG_PASSCHECK].Msgfunc = &CAgentServer::ExciteMsgPassCheck;
	m_MsgFunc[NET_MSG_CHA_NEW].Msgfunc          = &CAgentServer::MsgCreateCharacter;
	m_MsgFunc[NET_MSG_REQ_RAND_KEY].Msgfunc     = &CAgentServer::MsgSndRandomNumber;
	m_MsgFunc[NET_MSG_REQ_CHA_BAINFO].Msgfunc   = &CAgentServer::MsgSndChaBasicBAInfo;
	m_MsgFunc[NET_MSG_REQ_CHA_BINFO_CA].Msgfunc = &CAgentServer::MsgSndChaBasicInfo;
	m_MsgFunc[NET_MSG_CHA_DEL].Msgfunc          = &CAgentServer::MsgSndChaDelInfo;
	m_MsgFunc[DAUM_NET_MSG_CHA_DEL].Msgfunc     = &CAgentServer::DaumMsgSndChaDelInfo;
	m_MsgFunc[IDN_NET_MSG_CHA_DEL].Msgfunc		= &CAgentServer::IdnMsgSndChaDelInfo;
	m_MsgFunc[TERRA_NET_MSG_CHA_DEL].Msgfunc    = &CAgentServer::TerraMsgSndChaDelInfo;
	m_MsgFunc[EXCITE_NET_MSG_CHA_DEL].Msgfunc   = &CAgentServer::ExciteMsgSndChaDelInfo;
	m_MsgFunc[GSP_NET_MSG_CHA_DEL].Msgfunc      = &CAgentServer::GspMsgSndChaDelInfo;
	m_MsgFunc[JAPAN_NET_MSG_CHA_DEL].Msgfunc    = &CAgentServer::JapanMsgSndChaDelInfo;
	m_MsgFunc[GS_NET_MSG_CHA_DEL].Msgfunc       = &CAgentServer::GsMsgSndChaDelInfo;
	m_MsgFunc[NET_MSG_LOBBY_CHAR_JOIN_CA].Msgfunc = &CAgentServer::MsgGameJoinCA;
	m_MsgFunc[NET_MSG_AGENT_REQ_JOIN].Msgfunc   = &CAgentServer::MsgAgentReqJoinDA;
    m_MsgFunc[NET_MSG_LOGIN_WB_CA].Msgfunc      = &CAgentServer::MsgLoginWbCA;

	m_MsgFunc[NET_IN_MSG_CHAJOIN_2NDPASSWORD].Msgfunc      = &CAgentServer::MsgChaJoin2ndPassword; // 2차비번
	
    m_MsgFunc[NET_MSG_CLOSE_CLIENT].Msgfunc     = &CAgentServer::MsgCloseClient;

    m_MsgFunc[NET_MSG_LOG_UPDATE_TRACING_CHARACTER].Msgfunc  = &CAgentServer::MsgTracingLogPrint;
	
	m_MsgFunc[MET_MSG_GAME_JOIN_FIELDSVR_FB].Msgfunc = &CAgentServer::MsgLobbyCharJoinFA;	
	//mjeon.instance
	m_MsgFunc[MET_MSG_JOIN_INSTANCESVR_INFO].Msgfunc = &CAgentServer::MsgJoinInstance;

	m_MsgFunc[NET_MSG_APEX_DATA].Msgfunc          = &CAgentServer::MsgApexData;
	m_MsgFunc[NET_MSG_APEX_RETURN].Msgfunc        = &CAgentServer::MsgApexReturn;
	m_MsgFunc[NET_MSG_GAMEGUARD_ANSWER].Msgfunc   = &CAgentServer::MsgGameGuardAnswer;
	m_MsgFunc[NET_MSG_GAMEGUARD_ANSWER_1].Msgfunc = &CAgentServer::MsgGameGuardAnswerFirst;
	m_MsgFunc[NET_MSG_GAMEGUARD_ANSWER_2].Msgfunc = &CAgentServer::MsgGameGuardAnswerSecond;

	m_MsgFunc[NET_MSG_POST_REQ_CHANUM].Msgfunc     = &CAgentServer::MsgReqChaNum;
	m_MsgFunc[NET_MSG_POST_GET].Msgfunc            = &CAgentServer::MsgPostGet;
    m_MsgFunc[NET_MSG_POST_GET_STATE].Msgfunc      = &CAgentServer::MsgPostGetState;
    m_MsgFunc[NET_MSG_POST_GET_SEND_STATE].Msgfunc = &CAgentServer::MsgPostGetSendState;
    m_MsgFunc[NET_MSG_POST_GET_RECV_STATE].Msgfunc = &CAgentServer::MsgPostGetRecvState;
	m_MsgFunc[NET_MSG_POST_OPEN].Msgfunc           = &CAgentServer::MsgPostOpen;
	m_MsgFunc[NET_MSG_POST_OPEN_TAKE].Msgfunc      = &CAgentServer::MsgPostOpenAndTakeCA;
	m_MsgFunc[NET_MSG_POST_DELETE].Msgfunc         = &CAgentServer::MsgPostDelete;
	m_MsgFunc[NET_MSG_POST_RETURN].Msgfunc         = &CAgentServer::MsgPostReturn;
	m_MsgFunc[NET_MSG_POST_NOTIFY].Msgfunc         = &CAgentServer::MsgPostNotify;	

	m_MsgFunc[NET_MSG_SNS_FB_AUTHED].Msgfunc             = &CAgentServer::MsgSNSFBAuthed;
	m_MsgFunc[NET_MSG_SNS_FB_UNAUTHED].Msgfunc           = &CAgentServer::MsgSNSFBUnAuthed;
	m_MsgFunc[NET_MSG_SNS_GETINFO].Msgfunc               = &CAgentServer::MsgSNSGetInfo;
	m_MsgFunc[NET_MSG_SNS_FB_GETUID].Msgfunc             = &CAgentServer::MsgSNSFBGetUID;
	m_MsgFunc[NET_MSG_SNS_TW_AUTHED].Msgfunc             = &CAgentServer::MsgSNSTWAuthed;
	m_MsgFunc[NET_MSG_SNS_TW_UNAUTHED].Msgfunc           = &CAgentServer::MsgSNSTWUnAuthed;
	m_MsgFunc[NET_MSG_SNS_TW_GETUID].Msgfunc             = &CAgentServer::MsgSNSTWGetUID;
	
	m_MsgFunc[NET_MSG_GM_WHERE_PC_MAP].Msgfunc           = &CAgentServer::MsgGmwherePcMap;
	m_MsgFunc[NET_MSG_SERVER_PLAYERKILLING_MODE].Msgfunc = &CAgentServer::MsgGmPkMode;
	m_MsgFunc[NET_MSG_VIETNAM_TIME_RESET].Msgfunc        = &CAgentServer::MsgVietTimeReset;
	
	m_MsgFunc[NET_MSG_FIELDSVR_OUT_FB].Msgfunc             = &CAgentServer::MsgFieldSvrOutFb;
	m_MsgFunc[NET_MSG_RESURRECT_MOVEMAP_FB].Msgfunc              = &CAgentServer::MsgResurrectMoveMapFb;
	m_MsgFunc[NET_MSG_GCTRL_NOTICE_MOVEMAP_RESULT_AFB].Msgfunc               = &CAgentServer::MsgReCallFb;
	m_MsgFunc[NET_MSG_GCTRL_REQ_TELEPORT_FB].Msgfunc             = &CAgentServer::MsgTeleportFb;
	m_MsgFunc[NET_MSG_GCTRL_INVEN_PREMIUMSET_FB].Msgfunc         = &CAgentServer::MsgPremiumSetFb;
	m_MsgFunc[NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_AGT].Msgfunc = &CAgentServer::MsgServerLoginMaintenanceAGT;

    m_MsgFunc[NET_MSG_PS_GM_RELOAD_CA].Msgfunc = &CAgentServer::MsgPointShopGmReloadCA;

	m_MsgFunc[NET_MSG_ATTENDANCE_MAINTENANCE_MANUAL_START_CA].Msgfunc = &CAgentServer::MsgAttendanceMaintenanceManualStartCA;
    m_MsgFunc[NET_MSG_GCTRL_CLUB_NEW_2AGT].Msgfunc = &CAgentServer::MsgCreateClub;

    m_MsgFunc[NET_MSG_GM_LOG_ITEM_RELOAD_CA].Msgfunc = &CAgentServer::MsgGmLogItemReloadCA;
    m_MsgFunc[NET_MSG_GM_GAME_DATA_UPDATE_CA].Msgfunc = &CAgentServer::MsgGmGameDataUpdateCA;

    m_MsgFunc[NET_MSG_USER_CHANGE_COUNTRY_CA].Msgfunc = &CAgentServer::MsgUserChangeCountry;
	//m_MsgFunc[NET_MSG_CTF_JOIN_CA].Msgfunc = &CAgentServer::MsgCTFJoinCA;
    m_MsgFunc[NET_MSG_PROTOCOL_INFO_CA].Msgfunc = &CAgentServer::MsgProtocolInfoCA;
    m_MsgFunc[NET_MSG_GM_DB_UNLOCK_USER].Msgfunc = &CAgentServer::MsgGmDbUnlockUser;
    m_MsgFunc[NET_MSG_CLUB_MENBER_LOBBY_CA].Msgfunc = &CAgentServer::MsgClubMemberLobbyCA;
    m_MsgFunc[NET_MSG_CLUB_NEW_LOBBY_CA].Msgfunc = &CAgentServer::MsgClubNewLobbyCA;

    m_MsgFunc[NET_MSG_CLUB_LOG_CA].Msgfunc     = &CAgentServer::MsgClubLogCA;    
    m_MsgFunc[NET_MSG_CLUB_GRADE_NAME_CHANGE_CA].Msgfunc = &CAgentServer::MsgClubGradeNameChangeCA;
    m_MsgFunc[NET_MSG_CLUB_MEMBER_GRADE_CHANGE_CA].Msgfunc = &CAgentServer::MsgClubMemberGradeChangeCA;

	//개인상점 검색시스템
	m_MsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_REQ_CAH].Msgfunc = &CAgentServer::MsgPmarketSearchReqCA;
	//m_MsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_RESULT_HA].Msgfunc = &CAgentServer::MsgSearchResultHA;   
    //m_MsgFunc[NET_MSG_GCTRL_PMARKET_SEARCH_CLOSE].Msgfunc = &CAgentServer::MsgPmarketSearchClose;

	m_MsgFunc[NET_MSG_REFERENCE_COUNT_CAF].Msgfunc = &CAgentServer::MsgGmReferenceCountCA;

    m_MsgFunc[NET_MSG_GM_LOAD_IPEVENT].Msgfunc = &CAgentServer::MsgGmLoadIPEventCA;

	//// for global auth
	//m_MsgFunc[NET_MSG_AUTH_CERTIFICATION_ANS].Msgfunc = &CAgentServer::MsgAuthCertificationResponse;

	// TexasHoldem
	m_MsgFunc[NET_MSG_GCTRL_TEXAS_HOLD_EM].Msgfunc = &CAgentServer::MsgTexasHolem;

	m_MsgFunc[NET_MSG_USER_SYSTEM_INFO_CA].Msgfunc = &CAgentServer::MsgUserSystemInfoCA;


	// Country
	m_MsgFunc[NET_MSG_GCTRL_INITIALIZE_COUNTRY_FA].Msgfunc = &CAgentServer::MsgUpdateUserCountryFA;
	m_MsgFunc[NET_MSG_GCTRL_COUNTRY_CHANGE_CA].Msgfunc = &CAgentServer::MsgUpdateUserCountryCA;
	m_MsgFunc[NET_MSG_GCTRL_COUNTRY_CHANGE_DA].Msgfunc = &CAgentServer::MsgUpdateUserCountryDA;

	m_MsgFunc[NET_MSG_GM_RANMOBILE_COMMAND_REQUEST].Msgfunc = &CAgentServer::MsgRanMobileCommandRequest;
	m_MsgFunc[NET_MSG_GM_RANMOBILE_COMMAND_RESPONSE].Msgfunc = &CAgentServer::MsgRanMobileCommandResponse;
}


int CAgentServer::InsertMsg(int nClient, void* pMsg)
{
	if (pMsg)
    {
		NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) pMsg;
		m_pRecvMsgManager->MsgQueueInsert(nClient, pNmg, pNmg->Size());
	}
	return 0;
}

int CAgentServer::InsertMsg(int nClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc/*=false */)
{
    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(MsgType, PackBuffer, UseCrc))
        InsertMsg(nClient, &MsgPack);

    return 0;
}


void CAgentServer::MsgProcess(MSG_LIST* pMsg)
{	
	if (!pMsg)
        return;

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*)(pMsg->Buffer);

	if (!nmg)
		return;

	if (nmg->Size() > NET_DATA_BUFSIZE)
		return;
		   
	DWORD dwClient = pMsg->dwClient;
	DWORD dwGaeaID = GAEAID_NULL;

	if (dwClient >= (DWORD)m_pClientManager->GetMaxClient())
	{
		//GASSERT(0&&"m_nMaxClient33");

		return;
	}	

	//
	// 1.Session / Cache / Field 서버 메시지 처리
	//
	if (dwClient == m_SessionServerSlot)
	{
		SessionMsgProcess(pMsg);
		return;
	}
	else if (dwClient == m_CacheServerSlot)
	{
		CacheMsgProcess(pMsg);
		return;
	}
	else if (dwClient == m_MatchServerSlot)
	{
		if (MatchMsgProcess(pMsg))
		{
			return;	//MatchMsgProcess에서 처리되지 않았으면 GLAgentServer->MsgProcess로 넘겨야 한다.
		}
	}
	else if (m_pClientManager->IsAgentFieldSlot(dwClient) ||
		dwClient == DBACTION_CLIENT_ID)
	{
		//
		// mjeon.AF
		//  FieldServer로부터 오는 메시지들 중 우선적으로(서버간 통신 목적 등) 처리되어야 하는 메시지들을 처리.
		//  이 곳에서 처리되는 메시지들은 자동 Relay되지 않으므로, 명시적으로 Relay 처리를 해 주어야 한다.
		//
		if (FieldMsgProcess(pMsg))
		{
			return;
		}
		/*
#ifdef _RELEASED
		else	//FAC or FA
		{
			if (!m_pGLAgentServer->TraceFilter(nmg->nType))
			{
				//mjeon.msg trace (temporary)
				sc::writeLogDebug(sc::string::format("(Slot:%1%)Isn't ChaNum required for the msg(%2%) ?", dwClient, nmg->nType));
			}
		}
#endif
		*/
	}
	else if ( m_pClientManager->IsAgentAgentSlot( dwClient ) )
	{
		/*sc::writeLogInfo( sc::string::format(
			"Received Message -> Agent IP : %1%, MSG Number : %2%",
			m_pClientManager->GetClientIP( dwClient ),
			nmg->Type() ) );*/

		AgentMsgProcess( pMsg );

		return;
	}
	else
	{
		//클라이언트에서 보낸 메시지에 대해서만 ClientManager의 GaeaID가 유효하다.		
		dwGaeaID = m_pClientManager->GetGaeaID(dwClient);

		// APEX 메세지가 전송될때 가이아아이디가 할당되지 않기때문에 APEX메세지는 계속 진행
		// nProtect 메세지가 전송될때 가이아아이디가 없을수도 있으므로 계속 진행
		if (nmg->Type() > NET_MSG_GCTRL && dwGaeaID == GAEAID_NULL && 
			nmg->Type() != NET_MSG_APEX_DATA && nmg->Type() != NET_MSG_GAMEGUARD_ANSWER &&
			nmg->Type() != NET_MSG_GM_RANMOBILE_COMMAND_REQUEST && nmg->Type() != NET_MSG_GM_RANMOBILE_COMMAND_RESPONSE )
		{
			//sc::writeLogError(sc::string::format("Couldn't get a GaeaID from ClientSlot(%1%) for Message(%2%)", dwClient, nmg->Type()));
			return;
		}
	}


	//
	// 2.여기부터 FieldServer나 Client로부터 온 메시지들을 처리한다.
	//

    // MsgCountCheck
    if (dwClient < (UINT) m_nMaxClient) 
    {
        //if (m_pClientManager->IsClientSlot(dwClient) && m_pClientManager->IncPacketCount(dwClient, nmg->nType))
		if (m_pClientManager->IsClientSlot(dwClient))	//FieldServer로부터 오는 메시지는 패스
		{
			// 감시 패킷 검증: 블럭된 캐릭터는 그냥 리턴 시킨다.
			if ( eVerifyPacketBlock == m_pObservePacketMgr->VerifyPacket( nmg->Type(), m_pClientManager->ChaDbNum( dwClient ) ) )
			{
				return;
			}

			if (m_pClientManager->IncPacketCount(dwClient, nmg->nType))
			{
				if (!IsReserveCloseClient(dwClient))
				{
					m_pClientManager->WritePacketLog(dwClient);
					ReserveCloseClient(dwClient);
				}
			}
		}
    }

	EMNET_MSG emType = nmg->nType;
	if (emType < 0 || emType >= NET_MSG_GCTRL_MAX)
	{
		sc::writeLogError(sc::string::format("MsgProcess Out of range or illegal message:%1%", emType));
		CloseClient(dwClient, net::KICKOUT_ILLEGAL_MSG);
	}
	else
	{
		if (NULL == m_MsgFunc[emType].Msgfunc)
		{
			switch ( nmg->nType )
			{
			case CHINA_NET_MSG_PIN:					MsgChangePin ( pMsg );						break;
			case CHINA_NET_MSG_PASS:					MsgChangePass ( pMsg );					break;
			case CHINA_NET_MSG_EMAIL:					MsgChangeEmail ( pMsg );				break;
			case CHINA_NET_MSG_TOPUP:					MsgTopUp ( pMsg );						break;
			case CHINA_NET_MSG_RESETPASS:				MsgResetPass ( pMsg );					break;
			case CHINA_NET_MSG_RESETPIN:				MsgResetPin ( pMsg );					break;
			case CHINA_NET_MSG_RESETPASS2:				MsgResetPass2 ( pMsg );					break;
			case CHINA_NET_MSG_GT2VP:					MsgGameTimeCVT ( pMsg );			break;
			case CHINA_NET_MSG_PP2VP:					/*MsgPremiumPointCVT ( pMsg );	*/		break;
			default:
				{
					if (m_pGLAgentServer->MsgProcess(nmg, dwClient, dwGaeaID) == FALSE)
					{
						if(!m_pClientManager->IsMatchSlot(dwClient))	//Field와 Client 사이에서만 Relay
							MsgAutoRelay(dwClient, nmg);
					}
				}
				break;
			}
		}
		else
		{
			(this->*m_MsgFunc[emType].Msgfunc) (dwClient, dwGaeaID, nmg);
		}
	}
}

//mjeon.AF
void CAgentServer::MsgAutoRelay(DWORD dwClient, NET_MSG_GENERIC *nmg)
{
	NET_MSG_CHARACTER *nmc = NULL;


    __int64 nChaDbNum = -1;
	if(m_pClientManager->IsAgentFieldSlot(dwClient))	//FAC relay
	{
		//FieldServer -> Client로 Relay

		//mjeon.NET_MSG_GENERIC and it's children are non-polymorphic classes !!

		switch(nmg->nType)
		{
			case NET_MSG_LOBBY_CHAR_JOIN:
			{
				GLMSG::SNETLOBBY_CHARJOIN *msg = static_cast<GLMSG::SNETLOBBY_CHARJOIN *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_LOBBY_CHAR_SKILL:
			{
				GLMSG::SNETLOBBY_CHARSKILL *msg = static_cast<GLMSG::SNETLOBBY_CHARSKILL *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_LOBBY_QUEST_END:
			{
				GLMSG::SNETLOBBY_QUEST_END *msg = static_cast<GLMSG::SNETLOBBY_QUEST_END *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);

#ifdef _TEMP_TRACING_LOG
				// QUEST_BUFFER_OVER
				sc::writeLogDebug(sc::string::format("NET_MSG_LOBBY_QUEST_END dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", msg->dwQID, msg->dwSeq, msg->bLastSeq, msg->dwLengthStream, msg->dwSize, msg->dwChaNum));
#endif
				break;
			}

			case NET_MSG_LOBBY_QUEST_PROG:
			{
				GLMSG::SNETLOBBY_QUEST_PROG *msg = static_cast<GLMSG::SNETLOBBY_QUEST_PROG *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
	
#ifdef _TEMP_TRACING_LOG
				// QUEST_BUFFER_OVER
				sc::writeLogDebug(sc::string::format("NET_MSG_LOBBY_QUEST_PROG dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", msg->dwQID, msg->dwSeq, msg->bLastSeq, msg->dwLengthStream, msg->dwSize, msg->dwChaNum));
#endif
				break;
			}

			case NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT2:
				{
					GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT2* msg = static_cast< GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT2* >( nmg );
					nmc = static_cast< NET_MSG_CHARACTER* >( msg );
					break;
				}

			//case NET_MSG_LOBBY_CLUB_INFO:
			//{
			//	GLMSG::SNETLOBBY_CLUB_INFO *msg = static_cast<GLMSG::SNETLOBBY_CLUB_INFO *>(nmg);
			//	nmc = static_cast<NET_MSG_CHARACTER *>(msg);
			//	break;
			//}

			case NET_MSG_LOBBY_CLUB_ALLIANCE:
			{
				GLMSG::SNETLOBY_CLUB_ANCE_2CLT *msg = static_cast<GLMSG::SNETLOBY_CLUB_ANCE_2CLT *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_LOBBY_ITEM_COOLTIME:
			{
				GLMSG::SNETLOBBY_ITEM_COOLTIME *msg = static_cast<GLMSG::SNETLOBBY_ITEM_COOLTIME *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_LOBBY_INSTANCE_LIST:
			{
				GLMSG::SNETLOBBY_INSTANCE_LIST* msg = static_cast<GLMSG::SNETLOBBY_INSTANCE_LIST*>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER*>(msg);
				break;
			}

			case NET_MSG_LOBBY_CHAR_DEFAULT:
			{
				GLMSG::SNETLOBBY_CHAR_DEFAULT *msg = static_cast<GLMSG::SNETLOBBY_CHAR_DEFAULT *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}
			case NET_MSG_LOBBY_CLUB_BATTLE:
			{
				GLMSG::SNETLOBBY_CLUB_BATTLE *msg = static_cast<GLMSG::SNETLOBBY_CLUB_BATTLE *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}
			case NET_MSG_GCTRL_DROP_OUT_FORCED:
			{
				GLMSG::SNET_DROP_OUT_FORCED *msg = static_cast<GLMSG::SNET_DROP_OUT_FORCED *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_LOBBY_CHAR_JOIN_FB:
			{
				GLMSG::SNETLOBBY_CHARJOIN_FB *msg = static_cast<GLMSG::SNETLOBBY_CHARJOIN_FB *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_TRADE_CANCEL_TAR:
			{
				GLMSG::SNET_TRADE_CANCEL_TAR *msg = static_cast<GLMSG::SNET_TRADE_CANCEL_TAR *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_CLUB_INFO_2CLT:
			{
				GLMSG::SNET_CLUB_INFO_2CLT *msg = static_cast<GLMSG::SNET_CLUB_INFO_2CLT *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}
            /*
			case NET_MSG_GCTRL_CLUB_MEMBER_2CLT:
			{
				GLMSG::SNET_CLUB_MEMBER_2CLT *msg = static_cast<GLMSG::SNET_CLUB_MEMBER_2CLT *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}
            */
			case NET_MSG_GCTRL_INVEN_DELETE:
			{
				GLMSG::SNETPC_INVEN_DELETE *msg = static_cast<GLMSG::SNETPC_INVEN_DELETE *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_INVEN_DRUG_UPDATE:
			{
				GLMSG::SNETPC_INVEN_DRUG_UPDATE *msg = static_cast<GLMSG::SNETPC_INVEN_DRUG_UPDATE *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_QUEST_PROG_MOBKILL:
			{
				GLMSG::SNET_QUEST_PROG_MOBKILL *msg = static_cast<GLMSG::SNET_QUEST_PROG_MOBKILL *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_QUEST_PROG_ITEMUSE:
			{
				GLMSG::SNET_QUEST_PROG_ITEMUSE *msg = static_cast<GLMSG::SNET_QUEST_PROG_ITEMUSE *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_QUEST_PROG_ITEMGRINDING:
				{
					GLMSG::SNET_QUEST_PROG_ITEMGRINDING *msg = static_cast<GLMSG::SNET_QUEST_PROG_ITEMGRINDING *>(nmg);
					nmc = static_cast<NET_MSG_CHARACTER *>(msg);
					break;
				}
				
			case NET_MSG_GCTRL_QUEST_PROG_CONFTING:
				{
					GLMSG::SNET_QUEST_PROG_CONFTING *msg = static_cast<GLMSG::SNET_QUEST_PROG_CONFTING *>(nmg);
					nmc = static_cast<NET_MSG_CHARACTER *>(msg);
					break;
				}
			case NET_MSG_GCTRL_QUEST_PROG_LEVEL_UP:
				{
					GLMSG::SNET_QUEST_PROG_LEVEL_UP *msg = static_cast<GLMSG::SNET_QUEST_PROG_LEVEL_UP *>(nmg);
					nmc = static_cast<NET_MSG_CHARACTER *>(msg);
					break;

				}

			case NET_MSG_GCTRL_CONFRONT_FB:
			{
				GLMSG::SNETPC_REQ_CONFRONT_FB *msg = static_cast<GLMSG::SNETPC_REQ_CONFRONT_FB *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_TRADE_TAR:
			{
				GLMSG::SNET_TRADE_TAR *msg = static_cast<GLMSG::SNET_TRADE_TAR *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_TRADE_FB:
			{
				GLMSG::SNET_TRADE_FB *msg = static_cast<GLMSG::SNET_TRADE_FB *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR:
			{
				GLMSG::SNET_TRADE_ITEM_REGIST_TAR *msg = static_cast<GLMSG::SNET_TRADE_ITEM_REGIST_TAR *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR:
			{
				GLMSG::SNET_TRADE_ITEM_REMOVE_TAR *msg = static_cast<GLMSG::SNET_TRADE_ITEM_REMOVE_TAR *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_TRADE_LOCK_TAR:
			{
				GLMSG::SNET_TRADE_LOCK_TAR *msg = static_cast<GLMSG::SNET_TRADE_LOCK_TAR *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_TRADE_AGREE_TAR:
			{
				GLMSG::SNET_TRADE_AGREE_TAR *msg = static_cast<GLMSG::SNET_TRADE_AGREE_TAR *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_TRADE_COMPLETE_TAR:
			{
				GLMSG::SNET_TRADE_COMPLETE_TAR *msg = static_cast<GLMSG::SNET_TRADE_COMPLETE_TAR *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_WITH_VEHICLE_FB:
			{
				GLMSG::SNET_WITH_VEHICLE_FB *msg = static_cast<GLMSG::SNET_WITH_VEHICLE_FB *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

			case NET_MSG_GCTRL_WITH_VEHICLE_TAR:
			{
				GLMSG::SNET_WITH_VEHICLE_TAR *msg = static_cast<GLMSG::SNET_WITH_VEHICLE_TAR *>(nmg);
				nmc = static_cast<NET_MSG_CHARACTER *>(msg);
				break;
			}

            // 메시지팩 메시지이기 때문에 별도 처리
            case NET_MSG_LOBBY_CHAR_PUTON_EX:
            {
                std::vector<GLMSG::CHARPUTON_EX_PACK_DATA> vecRecv;
                MSGPACK_AUTO_UNPACK<GLMSG::CHARPUTON_EX_PACK_DATA> autoUnpack;
                DWORD dwChaDbNum=0;

                if( autoUnpack.GetVecData( nmg, vecRecv, dwChaDbNum ) == false )
                {
                    sc::writeLogError(sc::string::format("MsgAutoRelay ERROR! Type:%1% ChaDbNum:%2%", nmg->nType, dwChaDbNum));
                    return;
                }

                nChaDbNum = dwChaDbNum;
                break;
            }
            case NET_MSG_LOBBY_CHAR_ITEM:
            {
                std::vector<GLMSG::SNETLOBBY_INVENITEM_PACK> vecRecv;
                MSGPACK_AUTO_UNPACK<GLMSG::SNETLOBBY_INVENITEM_PACK> autoUnpack;
                DWORD dwChaDbNum=0;

                if( autoUnpack.GetVecData( nmg, vecRecv, dwChaDbNum ) == false )
                {
                    sc::writeLogError(sc::string::format("MsgAutoRelay ERROR! Type:%1% ChaDbNum:%2%", nmg->nType, dwChaDbNum));
                    return;
                }

                nChaDbNum = dwChaDbNum;
                break;
            }

			default:
				sc::writeLogError(sc::string::format("(FAC)RelayHandler is required for the msg(%1%)", nmg->nType));
				return;
		}


        DWORD dwChaDbNum = 0;

        if( 0 <= nChaDbNum )
        {
            dwChaDbNum = static_cast<DWORD>(nChaDbNum);
        }
        else
        {
            if (nmc == NULL)
            {
                sc::writeLogError(sc::string::format("MsgAutoRelay - nmc is NULL for the msg(%1%)", nmg->nType));
                return;
            }
            dwChaDbNum = nmc->dwChaNum;
        }

        DWORD dwClientSlot = m_pClientManager->GetClientID(dwChaDbNum);
        SendClient(dwClientSlot, nmg);

	}
	else	//CAF relay	or	IAF relay
	{
		//Client -> FieldServer로 Relay
		//ClientSlot을 해당 FieldSlot으로 변경하는 것은 SendField내에 처리해 두었다.

		//단, 이 때 NET_MSG_CHARACTER에 대해 ChaNum을 입력해주는 것은 Agent의 역할이다. 클라이언트가 입력해서 보내는 것은 믿을 수 없기 때문이다.

		DWORD	dwChaNum = m_pClientManager->ChaDbNum(dwClient);
		
		DWORD	dwInstanceServerNum = MAXNUM_OF_INSTANCESERVER_TO_MATCH;
		BOOL	bInstance = m_pClientManager->IsBoundByInstance(dwClient, OUT dwInstanceServerNum);

		BOOL bSendPartyInfo = FALSE;

		switch(nmg->nType)
		{
            case NET_MSG_GCTRL_GET_VEHICLE:
            {
                GLMSG::SNETPC_GET_VEHICLE* pMsg = static_cast<GLMSG::SNETPC_GET_VEHICLE*> (nmg);
                pMsg->dwChaNum = dwChaNum;
                break;
            }
            
			case NET_MSG_GCTRL_REQ_LANDIN:
			{
				GLMSG::SNETREQ_LANDIN *msg = static_cast<GLMSG::SNETREQ_LANDIN *>(nmg);
				GLCharAG* const _pChar(m_pGLAgentServer->GetCharByDbNum(dwChaNum));
				if ( _pChar == NULL )
					return;				
				msg->dwChaNum = dwChaNum;
				msg->dwSvrGroupNum = GetServerGroup();
				msg->tLoginTime = _pChar->m_tLoginTime;

				_pChar->m_bMapMoveFCard = FALSE;

				if( _pChar->GetPartyID() != PARTY_NULL )
				{
					bSendPartyInfo = TRUE;
				}
				
				break;
			}

			case NET_MSG_GCTRL_REQ_READY:
			{
				GLMSG::SNETREQ_READY *msg = static_cast<GLMSG::SNETREQ_READY *>(nmg);
				msg->dwChaNum = dwChaNum;
				msg->dwSvrGroupNum = GetServerGroup();
				break;
			}

			case NET_MSG_GCTRL_TRADE_CANCEL:
			{
				GLMSG::SNET_TRADE_CANCEL *msg = static_cast<GLMSG::SNET_TRADE_CANCEL *>(nmg);
				msg->dwChaNum = dwChaNum;
				break;
			}

			case NET_MSG_GCTRL_TRADE:
			{
				GLMSG::SNET_TRADE *msg = static_cast<GLMSG::SNET_TRADE *>(nmg);
				msg->dwChaNum = dwChaNum;
				break;
			}

			case NET_MSG_GCTRL_TRADE_TAR_ANS:
			{
				GLMSG::SNET_TRADE_TAR_ANS *msg = static_cast<GLMSG::SNET_TRADE_TAR_ANS *>(nmg);
				msg->dwChaNum = dwChaNum;
				break;
			}

			case NET_MSG_GCTRL_REGEN_GATE:
			{
				GLMSG::SNETPC_REQ_REGEN_GATE *msg = static_cast<GLMSG::SNETPC_REQ_REGEN_GATE *>(nmg);
				msg->dwChaNum = dwChaNum;
				break;
			}

			case NET_MSG_GCTRL_GET_OFF_WITH_VEHICLE:
			{
				GLMSG::SNET_GET_OFF_WITH_VEHICLE *msg = static_cast<GLMSG::SNET_GET_OFF_WITH_VEHICLE *>(nmg);
				msg->dwChaNum = dwChaNum;
				break;
			}

			case NET_MSG_GCTRL_WITH_VEHICLE:
			{
				GLMSG::SNET_WITH_VEHICLE *msg = static_cast<GLMSG::SNET_WITH_VEHICLE *>(nmg);
				msg->dwChaNum = dwChaNum;
				break;
			}

			case NET_MSG_GCTRL_WITH_VEHICLE_TAR_ANS:
			{
				GLMSG::SNET_WITH_VEHICLE_TAR_ANS *msg = static_cast<GLMSG::SNET_WITH_VEHICLE_TAR_ANS *>(nmg);
				msg->dwChaNum = dwChaNum;
				break;
			}

			case NET_MSG_GCTRL_CLUB_MEMBER_NICK:
			{
				GLMSG::SNET_CLUB_MEMBER_NICK *msg = static_cast<GLMSG::SNET_CLUB_MEMBER_NICK *>(nmg);
				msg->dwChaNum = dwChaNum;
				break;
			}

			default:
				sc::writeLogError(sc::string::format("(CAF)RelayHandler is required for the msg(%1%)", nmg->nType));
				return;
		}

		if (bInstance)
			SendMatch(nmg);
		else
			SendField(dwClient, nmg);

		// 맵이동순간 파티 맺어질시에 문제 해결을 위한 코드
		/*if( bSendPartyInfo )
		{
			GLCharAG* const _pChar(m_pGLAgentServer->GetCharByDbNum(dwChaNum));

			const GLPartyAgent* pParty = m_pGLAgentServer->GetParty(_pChar->GetPartyID());

			if( pParty ) {
				GLMSG::SNET_PARTY_FNEW_UPDATE msgParty(pParty->m_PartyID, _pChar->GaeaId(), pParty->getMaster(), pParty->m_PartyOption);

				//GLPartyAgent::MEMBER_ITER iter = pParty->m_Member.begin();
				//GLPartyAgent::MEMBER_ITER iter_end = pParty->m_Member.end();

				std::vector<DWORD>::const_iterator iter     = pParty->m_Member.begin();
				std::vector<DWORD>::const_iterator iter_end = pParty->m_Member.end();

				for ( ; iter!=iter_end; ++iter )
				{
					GLCharAG* pCharMem = m_pGLAgentServer->GetChar( (*iter) );
					if ( pCharMem == NULL )
						continue;

					msgParty.ADDMEMBER(GLPARTY_FNET(pCharMem->GaeaId(), pCharMem->ChaDbNum(), pCharMem->m_szName, pCharMem->GetSchool(), pCharMem->m_wLevel, pCharMem->m_emClass, pCharMem->GetCurrentMap()));
				}

				//m_pGLAgentServer->SENDTOALLCHANNEL(&msgParty);
				SendField(dwClient, &msgParty);
			}
		}*/
	}		
}


BOOL CAgentServer::AgentMsgProcess ( MSG_LIST* pMsg )
{
	BOOL bRet = TRUE;

	if ( !pMsg )
		return bRet;

	DWORD dwClient = pMsg->dwClient;
	NET_MSG_GENERIC* pPacket = (NET_MSG_GENERIC*) pMsg->Buffer;

	if ( !pPacket )
		return bRet;

	switch ( pPacket->nType )
	{
	case NET_MSG_HEARTBEAT_SERVER_REQ:
		MsgHeartbeatServerRequest( dwClient, pPacket );
		break;

	case NET_MSG_HEARTBEAT_SERVER_ANS:
		MsgHeartbeatServerAnswer( dwClient, pPacket );
		break;

	case CHINA_NET_MSG_PIN:					MsgChangePin ( pMsg );			break;
	case CHINA_NET_MSG_PASS:					MsgChangePass ( pMsg );			break;
	case CHINA_NET_MSG_EMAIL:					MsgChangeEmail ( pMsg );			break;
	case CHINA_NET_MSG_TOPUP:					MsgTopUp ( pMsg );			break;
	case CHINA_NET_MSG_RESETPASS:				MsgResetPass ( pMsg ); break;
	case CHINA_NET_MSG_RESETPIN:				MsgResetPin ( pMsg ); break;
	case CHINA_NET_MSG_RESETPASS2:				MsgResetPass2 ( pMsg ); break;
	case CHINA_NET_MSG_GT2VP:					MsgGameTimeCVT ( pMsg );			break;
	case CHINA_NET_MSG_PP2VP:					/*MsgPremiumPointCVT ( pMsg );	*/		break;


	case GS_NET_MSG_WB_LOGIN_AW:
	case GS_NET_MSG_WB_LOGIN_WA:
		// CAgentServer 처리 Message;
		( this->*m_MsgFunc[pPacket->nType].Msgfunc ) ( dwClient, GAEAID_NULL, pPacket );
		break;

	case NET_MSG_GCTRL_VCOUNTRY_ADD_AA:
	case NET_MSG_GCTRL_VCOUNTRY_RESET_AA:
		// GLAgentServer 처리 Message;
		m_pGLAgentServer->MsgProcess( pPacket, dwClient, GAEAID_NULL );
		break;

	default:
		// Relay Agent Message;
		{
			NET_MSG_GENERIC_RELAY_AGENT* pRelayMessage =
				static_cast< NET_MSG_GENERIC_RELAY_AGENT* >( pPacket );
			if ( NULL == pRelayMessage )
			{
				break;
			}

			// Relay Message 일 경우 처리한다;
			switch ( pRelayMessage->emRelayType )
			{
			case NET_MSG_GENERIC_RELAY_AGENT::EMMSG_RELAY_TYPE_DEFAULT:
				m_pGLAgentServer->MsgProcess( pPacket, dwClient, GAEAID_NULL );
				break;

			case NET_MSG_GENERIC_RELAY_AGENT::EMMSG_RELAY_TYPE_AGENTTOCLIENT:
				MsgAgentRelayAC( dwClient, pRelayMessage );
				break;

			case NET_MSG_GENERIC_RELAY_AGENT::EMMSG_RELAY_TYPE_AGENTTOFIELD:
				MsgAgentRelayAF( dwClient, pRelayMessage );
				break;

			case NET_MSG_GENERIC_RELAY_AGENT::EMMSG_RELAY_TYPE_AGENTTOALLCLIENT:
				MsgAgentRelayAB( dwClient, pRelayMessage );
				break;

			default:
				sc::writeLogError( sc::string::format(
					"Invalid Message -> MSG Number : %2%",
					pPacket->Type() ) );

				break;
			}
		}
		break;
	}

	return bRet;
}


BOOL CAgentServer::FieldMsgProcess(MSG_LIST* pMsg)
{
	BOOL bRet = TRUE;

	if (!pMsg)
        return bRet;
	
	DWORD dwClient = pMsg->dwClient;
	NET_MSG_GENERIC* pPacket = (NET_MSG_GENERIC*) pMsg->Buffer;

	if (!pPacket)
        return bRet;

	switch (pPacket->nType)
	{
    case NET_MSG_LOGIN_FEEDBACK_DATA2:
        MsgWorldBattleLoginBack(dwClient, pPacket);
        break;
	case NET_MSG_HEARTBEAT_SERVER_ANS:
#ifdef HEARTBEAT_LOG
		sc::writeLogInfo(sc::string::format("[HEARTBEAT] Recv Heartbeat Response from Field Svr ( %1% )", dwClient));
#endif
		m_pClientManager->SetHeartBeat(dwClient);
		break;
	case NET_MSG_GCTRL_REQ_GATEOUT:
	case NET_MSG_GCTRL_REQ_GENITEM_AGT:
	case NET_MSG_FIELDSVR_CHARCHK_FB:	
	case NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_AGT:
	case NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_AGT:
	case NET_MSG_CLUB_CERTIFIED_FA:
	case NET_MSG_GCTRL_CLUB_INCOME_MONEY_AGT:
	//case NET_MSG_GCTRL_CLUB_INCOME_UP:
	//case NET_MSG_GCTRL_CLUB_INCOME_FB:
	case NET_MSG_GCTRL_2_FRIEND_AG:
	case NET_MSG_GM_MOVE2CHAR_AG:
	case NET_MSG_GM_CHAR_INFO_FLD_FB:
	case NET_MSG_USER_CHAR_INFO_FLD_FB:
	case NET_MSG_CLUB_CD_CERTIFY_ING_BRD:
	case NET_MSG_GCTRL_CLUB_CD_CERTIFY_BRD:
	case NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_AGT:
	case NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_AGT:
	case NET_MSG_GCTRL_REQ_TAXI_NPCPOS_FB:
	case NET_MSG_GCTRL_CREATE_INSTANT_MAP_REQ:
	case NET_MSG_GCTRL_CREATE_INSTANT_MAP_DEL:
	case NET_MSG_GCTRL_NPC_COMMISSION:
	case NET_MSG_GCTRL_CLUB_DEATHMATCH_END_AGT:
    case NET_MSG_CHAT_LINK_FB:
    
	//mjeon.post
	case NET_MSG_POST_NOTIFY:							// FieldServer -> AgentServer
	case NET_MSG_POST_OPEN_TAKE_AF_FB:					// FieldServer -> AgentServer	
    case NET_MSG_CHAR_LEVEL_UP_FA:
		m_pGLAgentServer->MsgProcess( pPacket, dwClient, GAEAID_NULL );
		break;
    case NET_MSG_SERVER_GENERALCHAT:
        m_pGLAgentServer->MsgServerGeneralChat(pPacket, dwClient, GAEAID_NULL);
        break;
	case NET_MSG_ACTIVITY_CLOSED_FA:
		MsgActivityClosedFA( dwClient, GAEAID_NULL, pPacket );
		break;
	case NET_MSG_ACTIVITY_COMPLETED_FA:
		MsgActivityCompletedFA( dwClient, GAEAID_NULL, pPacket );
		break;
		
	case NET_MSG_ACTIVITY_SYNC:
		m_pGLAgentServer->MsgActivitySyncFA(pPacket, dwClient, GAEAID_NULL);
		break;
	case NET_MSG_ACTIVITY_REWARD_POINT_SYNC:
		m_pGLAgentServer->MsgActivityPointSyncFA(pPacket, dwClient, GAEAID_NULL);
		break;
	case NET_MSG_ACTIVITY_REWARD_BADGE_SYNC:
		m_pGLAgentServer->MsgActivityBadgeSyncFA(pPacket, dwClient, GAEAID_NULL);
		break;

	case NET_MSG_ATTENDANCE_SYNC_FAC:
		m_pGLAgentServer->MsgAttendanceSyncFAC(pPacket, dwClient, GAEAID_NULL);
		break;
	case NET_MSG_ATTENDANCE_RESET_FAC:
		m_pGLAgentServer->MsgAttendanceResetFAC(pPacket, dwClient, GAEAID_NULL);
		break;
	case NET_MSG_ATTENDANCE_UPGRADE_FAC:
		m_pGLAgentServer->MsgAttendanceUpgradeFAC(pPacket, dwClient, GAEAID_NULL);
		break;
	
	//mjeon.attendance
	case NET_MSG_ATTENDANCE_DAILYMAINTENANCE_READY_FA:	// FieldServer -> AgentServer
		MsgAttendanceDailyMaintenanceReadyFA(dwClient, GAEAID_NULL, pPacket);
		break;
	
	//mjeon.CaptureTheField
	case NET_MSG_CTF_PREPARE_AF_FB:
		m_pGLAgentServer->MsgCTFPrepareAF_FB(pPacket, dwClient, GAEAID_NULL);
		break;

	case NET_MSG_GET_CHA_EXTRA_INFO_FH_FB:
		m_pGLAgentServer->MsgGetChaExtraInfoFA(pPacket, dwClient, GAEAID_NULL);
		break;

	case NET_MSG_CTF_END_FA:
		m_pGLAgentServer->MsgCTFEndFA(pPacket, dwClient, GAEAID_NULL);
		break;


	case NET_MSG_CTF_DONE_AF_FB:
		m_pGLAgentServer->MsgCTFDoneFA_FB(pPacket, dwClient, GAEAID_NULL);
		break;

	case NET_MSG_CTF_RANKING_FAC:
		m_pGLAgentServer->MsgCTFRankingFAC(pPacket, dwClient, GAEAID_NULL);
		break;

	case NET_MSG_CHAR_POINT_FA:
		m_pGLAgentServer->MsgPointShopCharPointFA(pPacket, dwClient, GAEAID_NULL);
		break;    

    case NET_MSG_GCTRL_CLUB_STORAGE_GET_DB:
        m_pGLAgentServer->MsgClubStorageGetDb( pPacket, dwClient, GAEAID_NULL );
        break;

	case NET_MSG_GCTRL_CLUB_STORAGE_GET_DB_END:
		m_pGLAgentServer->MsgClubStorageGetDbEnd( pPacket, dwClient, GAEAID_NULL );
		break;

    case NET_MSG_CLUB_STORAGE_RESTORE_FA:
        m_pGLAgentServer->MsgClubStorageRestoreFA(pPacket, dwClient, GAEAID_NULL);
        break;

    case NET_MSG_CLUB_STORAGE_EX_INVEN_FA:
        m_pGLAgentServer->MsgClubStorageExHoldFA(pPacket, dwClient, GAEAID_NULL);
        break;

    case NET_MSG_CLUB_INVEN_TO_STORAGE_FA:
        m_pGLAgentServer->MsgClubInvenToStorageFA(pPacket, dwClient, GAEAID_NULL);
        break;

    case NET_MSG_CLUB_STORAGE_DRAW_MONEY_FA:
        m_pGLAgentServer->MsgClubStorageDrawMoneyFA(pPacket, dwClient, GAEAID_NULL);
        break;

    case NET_MSG_CLUB_STORAGE_SAVE_MONEY_FA:
        m_pGLAgentServer->MsgClubStorageSaveMoneyFA(pPacket, dwClient, GAEAID_NULL);
        break;

    case NET_MSG_GCTRL_CLUB_NEW_DB2AGT:
        m_pGLAgentServer->MsgClubNewDb2Agt(pPacket, dwClient, GAEAID_NULL);
        break;

    case NET_MSG_ADD_REBUY_ITEM_FA:
        m_pGLAgentServer->MsgAddRebuyItem(pPacket, dwClient, GAEAID_NULL);
        break;

    case NET_MSG_CHAR_POS_FA:
        m_pGLAgentServer->MsgCharPosFA(pPacket, dwClient, GAEAID_NULL);
        break;

    case NET_MSG_CHAR_DB_NUM_REQ_DA:
        m_pGLAgentServer->MsgCharDbNumReqDA(pPacket, dwClient, GAEAID_NULL);
        break;

    case NET_MSG_FRIEND_STATE_REQ_DA:
        m_pGLAgentServer->MsgFriendStateReqDA(pPacket, dwClient, GAEAID_NULL);
        break;

    case NET_LOG_ATTENDANCE_DS:
        MsgLogAttendanceDS(pPacket);
        break;

    case NET_LOG_ATTENDANCE_TASK_DS:
        MsgLogAttendanceTaskDS(pPacket);
        break;

    case NET_MSG_PRODUCT_FA:
        m_pGLAgentServer->MsgProductReqFA( pPacket );
        break;

    case NET_MSG_REQ_LEARN_PRODUCTBOOK_FB:
        m_pGLAgentServer->MsgProductIsLearnBookFA( pPacket );
        break;

    case NET_MSG_PRODUCT_TYPE_DEL_ACK:
        m_pGLAgentServer->MsgProductTypeDelFA( pPacket );
        break;

    case NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_FA:
        MsgPmarketSearchOpenFA(dwClient, pPacket);
        break;

	case NET_MSG_CHA_SAVE_FA:
		{
			GLMSG::SNETPC_MSG_CHA_SAVE_FA* pNetMsg = ( GLMSG::SNETPC_MSG_CHA_SAVE_FA* ) pPacket;
			
			m_pGLAgentServer->ChaSaveUserNumAdd( pNetMsg->dwUserNum );
			
			/// logout but not yet saved to DB 문제로 인해 제거한다;
			// 메시지 보낸 필드만 제외하고 보낸다.
			/*for ( size_t i = 0; i < MAX_CHANNEL_NUMBER; ++i )
			{
				for ( size_t j = 0; j < FIELDSERVER_MAX; ++j )
				{
					DWORD SlotNum = m_FieldServerInfo[i][j].NetSlotNum;
					if ( SlotNum != dwClient && SlotNum != net::INVALID_SLOT && m_pClientManager->IsOnline( SlotNum ) )
					{
						if ( BroadcastToField( SlotNum, pPacket ) == NET_ERROR )
							sc::writeLogError( sc::string::format( "error NET_MSG_CHA_SAVE_FA SendAllChannel Slot %1%", SlotNum ) );
					}
				}
			}*/
		}
		break;

	case NET_MSG_CHA_SAVE_END_FA:
		{
			GLMSG::SNETPC_MSG_CHA_SAVE_END_FA* pNetMsg = ( GLMSG::SNETPC_MSG_CHA_SAVE_END_FA* ) pPacket;
			
			m_pGLAgentServer->ChaSaveUserNumDel( pNetMsg->dwUserNum );

			// 캐릭터 접속종료 알림
// 			cp::NET_CHAR_CLOSE_AH MsgAC( pNetMsg->dwUserNum, pNetMsg->dwChaNum );
// 			SendCache( &MsgAC );
			
			/// logout but not yet saved to DB 문제로 인해 제거한다;
			// 메시지 보낸 필드만 제외하고 보낸다.
			/*for ( size_t i = 0; i < MAX_CHANNEL_NUMBER; ++i )
			{
				for ( size_t j = 0; j < FIELDSERVER_MAX; ++j )
				{
					DWORD SlotNum = m_FieldServerInfo[i][j].NetSlotNum;
					if ( SlotNum != dwClient && SlotNum != net::INVALID_SLOT && m_pClientManager->IsOnline( SlotNum ) )
					{
						if ( BroadcastToField( SlotNum, pPacket ) == NET_ERROR )
							sc::writeLogError( sc::string::format( "error NET_MSG_CHA_SAVE_END_FA SendAllChannel Slot %1%", SlotNum ) );
					}
				}
			}*/
		}
		break;

	case NET_MSG_CHA_VALIDATION_FA:
		m_pGLAgentServer->MsgChaValidataionFA( pPacket, dwClient, GAEAID_NULL );
		break;

	case NET_MSG_PVE_CREATE_INSTANT_MAP_ACK_FA:
		m_pGLAgentServer->MsgPVEAckCreateInstantMapFA( pPacket, dwClient, GAEAID_NULL );
		break;

	case NET_MSG_PVE_ENTRANCE_INSTANT_MAP_ACK_FA:
		m_pGLAgentServer->MsgPVEAckEntranceInstantMapFA( pPacket, dwClient, GAEAID_NULL );
		break;

	case NET_MSG_PVE_MOVE_REQ_FA:
		m_pGLAgentServer->MsgPVEReqMoveFA( pPacket, dwClient, GAEAID_NULL );
		break;

	case NET_MSG_PVE_RETRY_REQ_FA:
		m_pGLAgentServer->MsgPVEReqRetryFA( pPacket, dwClient, GAEAID_NULL );
		break;

	case NET_MSG_PVE_STAGE_RESULT_FA_OR_AF:
		m_pGLAgentServer->MsgPVEStageResultFA( pPacket, dwClient, GAEAID_NULL );
		break;

	case NET_MSG_PVE_RETURN_REQ_FA:
		m_pGLAgentServer->MsgPVEReqReturnFA( pPacket, dwClient, GAEAID_NULL );
		break;

	case NET_MSG_PVE_GM_RELOAD_TRIGGER_ACK_FA:
		m_pGLAgentServer->MsgPVEAckReloadTriggerFA( pPacket, dwClient, GAEAID_NULL );
		break;

	case NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_FA:
		m_pGLAgentServer->MsgPVEReqEnterNumCharge( pPacket, dwClient, GAEAID_NULL );
		break;

	case NET_MSG_BLOCK_USER:
		MsgBlockUserFA( pPacket );
		break;

	case NET_MSG_GCTRL_CLUB_STORAGE_USER_RESET:
		m_pGLAgentServer->MsgClubStorageUserReset( pPacket, dwClient, GAEAID_NULL );
		break;

	case NET_MSG_GCTRL_CLUB_STORAGE_MONEY_USER_RESET:
		m_pGLAgentServer->MsgClubStorageMoneyUserReset( pPacket, dwClient, GAEAID_NULL );
		break;

	case NET_MSG_DISCONNECT_CHARACTER:
		MsgDisconnectCharacterFA( pPacket );
		break;

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.14, 신규등록
     * modified : 
     */
	case NET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_FA:
		m_pGLAgentServer->MsgRandomboxWinnerNotifyFA(pPacket, dwClient, GAEAID_NULL);
		break;

	default:
		{
			bRet = FALSE;
			break;
		}
	};

	return bRet;
}


void CAgentServer::MsgSndRandomNumber( DWORD dwClient , DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	int nRandomNumber = m_pClientManager->GetRandomPassNumber( dwClient );
	if ( nRandomNumber <= 0 )
	{
		nRandomNumber = SERVER_UTIL::makeRandomNumber( MAX_RANDOM_PASSWORD_NUM );
		m_pClientManager->SetRandomPassNumber( dwClient, nRandomNumber );
	}
	NET_RANDOMPASS_NUMBER tempMsg;
	tempMsg.nRandomNumber = nRandomNumber;	
	SendClient(dwClient, &tempMsg);
}

void CAgentServer::MsgSndCryptKey(DWORD dwClient)
{
	CRYPT_KEY ck = m_pClientManager->GetNewCryptKey(); // Get crypt key
	
	m_pClientManager->SetCryptKey(dwClient, ck); // Set crypt key
	
	NET_CRYPT_KEY nck;
	nck.ck = ck;
	
	SendClient(dwClient, &nck);
}


// 새로운 캐릭터를 생성한다.
void CAgentServer::MsgCreateCharacter(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (!nmg)
		return;

	NET_NEW_CHA* pPacket = (NET_NEW_CHA*) nmg;

    unsigned int Crc = sc::string::getCrc32(pPacket, EM_NET_NEW_CHA_MSG_DATA_SIZE);
    if (pPacket->m_Crc != Crc)
    {
        // Crc 체크
        sc::writeLogError(
            sc::string::format(
                "MsgCreateCharacter UserID %1% Crc %2%/%3%",
                m_pClientManager->GetUserID(dwClient),
                Crc,
                pPacket->m_Crc));
        return;
    }

	NET_NEW_CHA_FB	nncfd;
	DWORD			dwSndBytes		= 0;
	int				nUesrNum		= 0;
	int     		ChaClass		= 0;
	bool			bExtreme		= false; // 극강부 생성 가능 여부;
	bool			bActor			= false; // 변술부 생성 가능 여부;
	int				nSvrGrp			= 0;
	int				nChaNum			= 0;
    WORD            wChaRemain		= 0;
	bool	bJumping = false;
	
	// 인증을 통과하지 못함	
	if (!m_pClientManager->IsAccountPass(dwClient))
	{
		CloseClient(dwClient, net::KICKOUT_UNDEFINE);
		return;
	}

    if (m_bTestServer) // 테스트 서버라면
        nncfd.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
    else
        nncfd.wChaRemain = m_pClientManager->GetChaRemain(dwClient);

	nncfd.nType = NET_MSG_CHA_NEW_FB;

    // 극강부 체크
    if (!GLUseFeatures::GetInstance().IsUsingExtremeClass())
    {        
        if (pPacket->nIndex == GLCI_EXTREME_M || pPacket->nIndex == GLCI_EXTREME_W)
        {
            nChaNum = -1; // 캐릭터 에러
            nncfd.nResult = EM_CHA_NEW_FB_SUB_LENGTH; // 글자수 제한
            SendClient(dwClient, &nncfd);
            sc::writeLogError(
                sc::string::format(
                    "MsgCreateCharacter ExtremeClass false but try to create extream class. UserID %1%",
                    m_pClientManager->GetUserID(dwClient)));
            return;
        }
    }

	// 캐릭터 갯수 제한, 만들 수 있는 캐릭터 없음
	if (nncfd.wChaRemain <= 0 || nncfd.wChaRemain > MAX_CHAR_LENGTH)
	{
		nncfd.nResult  = EM_CHA_NEW_FB_SUB_MAX;
		nncfd.nChaNum  = 0;
		SendClient(dwClient, &nncfd);
		return;
	}

	// 캐릭터 인덱스
	ChaClass = pPacket->nIndex;

    switch (GLCONST_CHAR::wENABLE_NEW_CLASS)
    {
    case RESTRICT_SCIENTIST_NEW_CLASS:
        {
            // 캐릭터 인덱스 오류, 캐릭터 만들 수 없음.
            if (ChaClass < 0 || ChaClass >= GLCI_NUM_NEWSEX)	
            {
                nChaNum = -1; // 캐릭터 에러
                nncfd.nResult = EM_CHA_NEW_FB_SUB_LENGTH; // 글자수 제한
                SendClient(dwClient, &nncfd);
                return;
            }

        }
        break;

    case RESTRICT_ASSASSIN_NEW_CLASS:
        {
            // 캐릭터 인덱스 오류, 캐릭터 만들 수 없음.
            if (ChaClass < 0 || ChaClass >= GLCI_NUM_SCIENTIST)
            {
                nChaNum = -1; // 캐릭터 에러
                nncfd.nResult = EM_CHA_NEW_FB_SUB_LENGTH; // 글자수 제한
                SendClient(dwClient, &nncfd);
                return;
            }
        }
        break;

    case RESTRICT_TRICKER_NEW_CLASS:
        {
            if (ChaClass < 0 || ChaClass >= GLCI_NUM_ASSASSIN)
            {
                nChaNum = -1; // 캐릭터 에러
                nncfd.nResult = EM_CHA_NEW_FB_SUB_LENGTH; // 글자수 제한
                SendClient(dwClient, &nncfd);
                return;
            }
        }
        break;

	case RESTRICT_ACTOR_NEW_CLASS:
		{
			if (ChaClass < 0 || ChaClass >= GLCI_NUM_ACTOR)
			{
				nChaNum = -1; // 캐릭터 에러
				nncfd.nResult = EM_CHA_NEW_FB_SUB_LENGTH; // 글자수 제한
				SendClient(dwClient, &nncfd);
				return;
			}
		}
		break;

    case NO_RESTRICT_NEW_CLASS:
        {
            // 캐릭터 인덱스 오류, 캐릭터 만들 수 없음.
            if (ChaClass < 0 || ChaClass >= GLCI_NUM_ACTOR)
            {
                nChaNum = -1; // 캐릭터 에러
                nncfd.nResult = EM_CHA_NEW_FB_SUB_LENGTH; // 글자수 제한
                SendClient(dwClient, &nncfd);
                return;
            }
        }
        break;

    default:
        {
            if (ChaClass < 0 || ChaClass >= GLCI_NUM_NEWSEX )	
            {
                nChaNum = -1; // 캐릭터 에러
                nncfd.nResult = EM_CHA_NEW_FB_SUB_LENGTH; // 글자수 제한
                SendClient(dwClient, &nncfd);
                return;
            }
        }
        break;
    }

	// 캐릭터명
	CString strChaName(pPacket->szChaName);
	strChaName.Trim(_T(" ")); // 앞뒤 공백제거

	//strChaName.Replace(_T(" "),_T(""));// 공백 없애기 



    //if (m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST || m_ServiceProvider == SP_OFFICE_TEST)
    //{
    //    if (!sc::string::goodChaNameKorea(std::string(strChaName.GetString())))
    //    {
    //        nChaNum = -1;
    //        nncfd.nResult = EM_CHA_NEW_FB_SUB_BAD_NAME_KOREA;
    //        SendClient(dwClient, &nncfd);
    //        return;
    //    }
    //}

    if (m_ServiceProvider == SP_WORLD_BATTLE)
    {
        if (!sc::string::checkEnglishOrNumber(std::string(strChaName.GetString())))
        {
            nChaNum = -1;
            nncfd.nResult = EM_CHA_NEW_FB_SUB_BAD_NAME_WB;
            SendClient(dwClient, &nncfd);
            return;
        }
    } 

	// 스페이스 찾기, 캐릭터이름 4글자 이하 에러, 캐릭터 만들 수 없음.
	if ((strChaName.FindOneOf(_T(" ")) != -1) || (strChaName.GetLength() < 4)) 
	{
		nChaNum = -1; // 캐릭터이름 에러
		nncfd.nResult = EM_CHA_NEW_FB_SUB_LENGTH; // 글자수 제한
		SendClient(dwClient, &nncfd);
		return;
	}

	CString strCheckName(strChaName);
	if ( m_ServiceProvider == SP_GS )//
		strCheckName = sc::string::ansi_to_utf8( std::string(strCheckName.GetString()) ).c_str();

	BOOL bFILTER0 = STRUTIL::CheckString(strCheckName);
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter(strCheckName);
	if (bFILTER0 || bFILTER1)
	{
		nChaNum = -1; // 캐릭터이름 에러
		nncfd.nResult = EM_CHA_NEW_FB_SUB_BAD_NAME; // BAD_NAME
		SendClient(dwClient, &nncfd);
		return;
	}

	// 태국어 문자 조합 체크 
    if (RENDERPARAM::emLangSet == language::THAILAND) 
    {
	    if (!STRUTIL::CheckThaiString(strChaName))
	    {
		    nChaNum = -1; // 캐릭터이름 에러
		    nncfd.nResult = EM_CHA_NEW_FB_SUB_THAICHAR_ERROR; // 글자수 제한
		    SendClient(dwClient, &nncfd);
		    return;
	    }
    }

    // 베트남 문자 조합 체크 
    if (RENDERPARAM::emLangSet == language::VIETNAM) 
    {
	    if (STRUTIL::CheckVietnamString(strChaName))
	    {
		    nChaNum = -1; // 캐릭터이름 에러
		    nncfd.nResult = EM_CHA_NEW_FB_SUB_VNCHAR_ERROR; // 글자수 제한
		    SendClient(dwClient, &nncfd);
		    return;
	    }
    }


	// 극강부 생성 가능 체크
	if ( GLCI_EXTREME_M == ChaClass || GLCI_EXTREME_W == ChaClass)
	{
		bExtreme = m_pClientManager->GetExtreme(dwClient);

		if ( bExtreme == false )
		{
			nncfd.nResult = EM_CHA_NEW_FB_SUB_MAX;
			SendClient(dwClient, &nncfd);
			return;
		}
	}

	// 변술부 생성 가능 체크;
	if ( GLCI_ACTOR_M == ChaClass || GLCI_ACTOR_W == ChaClass )
	{
		bActor = m_pClientManager->GetCreateActor( dwClient );

		if ( false == bActor )
		{
			nncfd.nResult = EM_CHA_NEW_FB_SUB_MAX;
			SendClient(dwClient, &nncfd);
			return;
		}
	}

	if (m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST || m_ServiceProvider == SP_OFFICE_TEST)
	{
		if (!sc::string::goodChaNameKorea(std::string(strChaName.GetString())))
		{
			nncfd.nResult = EM_CHA_NEW_FB_SUB_BAD_NAME_KOREA;
			SendClient(dwClient, &nncfd);
			return;	
		}
	}

	if (m_bTestServer) // 테스트 서버라면
	{
		m_pClientManager->SetChaTestRemain(dwClient, nncfd.wChaRemain-1);
	}
	else // 정식 서버 라면...
	{
		m_pClientManager->SetChaRemain(dwClient, nncfd.wChaRemain-1);
	}

	AddGameAdoJob(
        db::DbActionPtr(
            new db::CharacterCreateNew(
                ChaClass, 
                m_pClientManager->UserDbNum(dwClient),
                m_nServerGroup,
                strChaName,
                pPacket->wSchool,
                pPacket->wHair,
                pPacket->wFace,
                pPacket->wHairColor,
                pPacket->wSex,
                dwClient,
                m_pClientManager->GetClientIP(dwClient),
                m_pClientManager->GetClientPort(dwClient), 
				m_pClientManager->GetJumping(dwClient)
                )));	//mjeon.ado
}

void CAgentServer::MsgCreateCharacterBack(
	int nChaNum,
    DWORD dwClient,
    DWORD dwUserNum,
    const char* szUserIP,
    USHORT uPort)
{	
    NET_NEW_CHA_FB	nncfd;
    
    nncfd.nType = NET_MSG_CHA_NEW_FB;
	nncfd.nChaNum   = 0;

	WORD wChaRemain		= 0;
	bool bExtreme		= 0;

	if (nChaNum > 0) // 캐릭터 생성 성공
	{
		nncfd.nChaNum = nChaNum;
		nncfd.nResult = EM_CHA_NEW_FB_SUB_OK;

		//// 로직상의 극강부 남/여 생성 가능 숫자를 받아온다.
		bExtreme = m_pClientManager->GetExtreme( dwClient );

		nncfd.bExtreme = m_pClientManager->GetExtreme( dwClient );
		nncfd.bActor = m_pClientManager->GetCreateActor( dwClient );

		if (m_bTestServer) // 테스트 서버라면
		{
            wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
            if (wChaRemain > MAX_CHAR_LENGTH) 
                wChaRemain = 0;
            //m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            nncfd.wChaRemain = wChaRemain;

			m_pDbManager->UserTestChaNumDecrease(dwUserNum);
		}
		else // 정식 서버 라면...
		{
            wChaRemain = m_pClientManager->GetChaRemain(dwClient);
            if (wChaRemain > MAX_CHAR_LENGTH) 
                wChaRemain = 0;
            //m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            nncfd.wChaRemain = wChaRemain;

			m_pDbManager->UserChaNumDecrease(dwUserNum);
		}
	}
	else
	{
		if (m_bTestServer) // 테스트 서버라면
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain+1);
		}
		else // 정식 서버 라면...
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient);
			m_pClientManager->SetChaRemain(dwClient, wChaRemain+1);
		}

		if (nChaNum == sc::db::DB_ERROR) // 캐릭터 생성(DB) 에러발생...
		{    
			if (m_bTestServer) wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
			else               wChaRemain = m_pClientManager->GetChaRemain(dwClient);
			nncfd.wChaRemain = wChaRemain;
			nncfd.nResult    = EM_CHA_NEW_FB_SUB_DUP;
		}
		else if (nChaNum == sc::db::DB_CHA_MAX) // 만들수 있는 캐릭터 숫자 초과
		{
			if (m_bTestServer) wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
			else               wChaRemain = m_pClientManager->GetChaRemain(dwClient);
			nncfd.wChaRemain = wChaRemain;
			nncfd.nResult  = EM_CHA_NEW_FB_SUB_MAX;
		}
		else if (nChaNum == sc::db::DB_CHA_DUF) // 같은 이름을 가진 캐릭터 존재
		{
			if (m_bTestServer) wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
			else               wChaRemain = m_pClientManager->GetChaRemain(dwClient);
			nncfd.wChaRemain = wChaRemain;
			nncfd.nResult  = EM_CHA_NEW_FB_SUB_DUP;
		}
		else if (nChaNum == sc::db::DB_CHA_GM) // 같은 이름을 가진 캐릭터 존재
		{
			if (m_bTestServer) wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
			else               wChaRemain = m_pClientManager->GetChaRemain(dwClient);
			nncfd.wChaRemain = wChaRemain;
			nncfd.nResult  = EM_CHA_NEW_FB_SUB_GM;
		}
		else // 알수 없는 에러
		{
			if (m_bTestServer) wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
			else               wChaRemain = m_pClientManager->GetChaRemain(dwClient);
			nncfd.wChaRemain = wChaRemain;
			nncfd.nResult  = EM_CHA_NEW_FB_SUB_ERROR;
		}
	}

     // IP/PORT 가 동일할때만 처리
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{
	    SendClient(dwClient, &nncfd);

	    if (nChaNum > 0)
	    {
			m_pClientManager->ValidateChaNum(dwClient, nChaNum);

		    MsgSndChaBasicInfo(dwClient, nChaNum);
	    }
    }
}

/*
void CAgentServer::MsgSndLoginInfoToField(DWORD dwClient)
{
	NET_LOGIN_FEEDBACK_DATA3 MsgAF(
        m_pClientManager->UserDbNum(dwClient),
        m_pClientManager->GetUserID(dwClient),
        m_pClientManager->GetCountry(dwClient));
	m_pClientManager->SendField(dwClient, &MsgAF);
	sc::writeLogInfo(sc::string::format("MsgSndLoginInfoToField %1% %2%", MsgAF.nUserNum, MsgAF.szUserid));
}
*/

// *****************************************************
// Desc: 필드서버에 접속정보 전송(필드서버가 바뀔경우)
// *****************************************************
//FA
void CAgentServer::MsgFieldSvrOutFb ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{	
	GLMSG::SNETPC_FIELDSVR_OUT_FB *msg = (GLMSG::SNETPC_FIELDSVR_OUT_FB *)nmg;

	if (msg == NULL)
		return;

	GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum(msg->dwChaNum);
	if ( !pChar )
		return;

	if ( pChar->m_dwNextChannel == MAX_CHANNEL_NUMBER )
		return;

	if ( pChar->m_dwNextFieldSvr == FIELDSERVER_MAX )
		return;

	int nRet = NET_ERROR;

	if (msg->bToInstance)
	{
		nRet = SetUpInstanceInfo(pChar->ClientSlot(), pChar->m_dwNextFieldSvr /*InstanceServerNum*/, pChar->m_dwNextChannel);
	}
	else
	{
		GASSERT(msg->dwSvrGroupNum == GetServerGroup());
		if (msg->dwSvrGroupNum != GetServerGroup())
		{
			sc::writeLogError(sc::string::format("(CRITICAL) CAgentServer::MsgFieldSvrOutFb - ServerGroup(%1%/%2%) is different !!", msg->dwSvrGroupNum, GetServerGroup()));
			return;
		}

		nRet = SetUpFieldInfo(pChar->ClientSlot(), pChar->m_dwNextFieldSvr, pChar->GaeaId(), pChar->m_dwNextChannel);
	}
	
	//mjeon.AF
	if (nRet == NET_OK)
	{
		MoveSystem::CMoveManagerServer::Instance()->OnJoinServer( msg );

		/*//현재 필드 서버 및 맵ID 지정		
		pChar->SetCurrentField(pChar->m_dwNextFieldSvr, pChar->GetNextMap(), pChar->m_bIsInstance);		
		pChar->SetMapPosition(msg->positionPrevious, false);

		//맵에 새로 진입하기 위해서 Field서버에 메시지 송신
		NET_GAME_JOIN_FIELDSVR msgJoin(pChar->GetCountry());

		StringCchCopy(msgJoin.szUID, DAUM_MAX_UID_LENGTH+1, pChar->m_szUserName);
		msgJoin.dwSlotAgentClient = pChar->ClientSlot();
		msgJoin.emType		= msg->emType;
		msgJoin.nUserNum	= pChar->m_UserDbNum;
		msgJoin.dwUserLvl	= pChar->m_dwUserLvl;
		msgJoin.nChaNum		= pChar->m_CharDbNum;
		msgJoin.dwGaeaID	= pChar->GaeaId();
		msgJoin.PartyID		= pChar->GetPartyID();
		msgJoin.tPREMIUM	= pChar->m_tPREMIUM;
		msgJoin.tCHATBLOCK= pChar->m_tCHATBLOCK;
		msgJoin.sStartMap	= pChar->GetNextMap();
		msgJoin.dwStartGate= pChar->m_dwNextGateID;
		msgJoin.vStartPos	= pChar->m_vNextPos;
		msgJoin.dwActState	= pChar->m_dwActState;
		msgJoin.bUseArmSub	= pChar->m_bUseArmSub;
		msgJoin.emIPBonus	= pChar->m_emIPBonus;
		msgJoin.sChinaTime	= pChar->m_sChinaTime;
		msgJoin.sEventTime	= pChar->m_sEventTime;
		msgJoin.sVietnamGainSystem	= pChar->m_sVietnamSystem;
		msgJoin.bChangeField = msg->bChangeField;
		msgJoin.sSummonable	= msg->sSummonable;
		msgJoin.dwCountryID = pChar->GetCountryInfo().dwID;

		if ( pChar->GetCryptKey().guid != GUID_NULL )
		{
			msgJoin.ckey = pChar->GetCryptKey();
		}

		SendField(pChar->ClientSlot(), &msgJoin);
		//m_pGLAgentServer->SENDTOFIELD(pChar->ClientSlot(), &msgJoin);

		pChar->ResetNextFieldSvr();*/
	}
}

//FA
void CAgentServer::MsgResurrectMoveMapFb(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    /*if ( dwGaeaID != GAEAID_NULL )
        return;

	GLMSG::NET_CHAR_RESURRECT_MOVEMAP_FB *msg = (GLMSG::NET_CHAR_RESURRECT_MOVEMAP_FB *)nmg;

	GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum(msg->dwChaNum);
	if (!pChar)
		return;
	
	if ( pChar->m_dwNextFieldSvr == FIELDSERVER_MAX )
		return;
	
	DWORD dwClientSlot = pChar->ClientSlot();
	
	//m_pClientManager->CloseField(pChar->ClientSlot());
	m_pClientManager->ResetFieldInfo(dwClientSlot);

	//mjeon.AF	
	if ( SetUpFieldInfo(dwClientSlot, pChar->m_dwNextFieldSvr, pChar->GaeaId(), pChar->m_nChannel) == NET_OK )	
	{
		//	에이젼트 서버로 메시지 전달.
		m_pGLAgentServer->MsgProcess(nmg, dwClientSlot, GAEAID_NULL);
		pChar->ResetNextFieldSvr(); //	연결할 필드서버 정보를 리셋.
	}*/
}

void CAgentServer::MsgReCallFb ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	//DWORD dwAgentClient = dwClient - m_pClientManager->GetMaxClient();

	MoveSystem::CMoveManagerServer::Instance()->OnMoveMap_SameServer(
		static_cast< GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB* >( nmg ) );

	/*GLMSG::SNETPC_REQ_RECALL_FB* pNetMsg = (GLMSG::SNETPC_REQ_RECALL_FB *)nmg;

	//GLCharAG* pChar = m_pGLAgentServer->GetChar(dwGaeaID);
	GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum(pNetMsg->dwChaNum);
	if (!pChar)
		return;

	pChar->SetCurrentField(pChar->m_dwCurFieldSvr, pNetMsg->sMAPID);

	SendClient(pChar->ClientSlot(), nmg);*/
}

void CAgentServer::MsgTeleportFb ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	/*GLMSG::SNETPC_REQ_TELEPORT_FB *pNetMsg = (GLMSG::SNETPC_REQ_TELEPORT_FB *) nmg;

	//GLCharAG* pChar = m_pGLAgentServer->GetChar ( dwGaeaID );
	GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum(pNetMsg->dwChaNum);
	if ( !pChar )
		return;

	pChar->SetCurrentField ( pChar->m_dwCurFieldSvr, pNetMsg->sMAPID );

	SendClient ( pChar->ClientSlot(), nmg );*/
}

void CAgentServer::MsgPremiumSetFb ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	//DWORD dwAgentClient = dwClient - m_pClientManager->GetMaxClient();

	GLMSG::SNET_INVEN_PREMIUMSET_FB *pNetMsg = (GLMSG::SNET_INVEN_PREMIUMSET_FB *)nmg;

	//GLCharAG* pChar = m_pGLAgentServer->GetChar ( dwGaeaID );
	GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum(pNetMsg->dwChaNum);
	if (!pChar)
		return;

	pChar->m_tPREMIUM = pNetMsg->tLMT;

	SendClient(pChar->ClientSlot(), nmg);
}

void CAgentServer::MsgGmPkMode ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	int nUSERLVL = m_pClientManager->GetUserType ( dwClient );

	if ( USER_GM1 <= nUSERLVL )
	{
		SendAllChannel ( nmg );
	}
}

void CAgentServer::MsgGmwherePcMap ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_GM_WHERE_PC_MAP *pNetMsg = (GLMSG::SNET_GM_WHERE_PC_MAP *) nmg;

	int nUSERLVL = m_pClientManager->GetUserType ( dwClient );

	if ( USER_GM3 <= nUSERLVL )
	{
		GLCharAG* pCHAR = m_pGLAgentServer->GetCharByDbNum ( pNetMsg->dwCHARID );
		if ( pCHAR )
		{
			GLMSG::SNET_GM_WHERE_PC_MAP_FB NetMsgFB;
			NetMsgFB.dwChannel = (DWORD)pCHAR->m_nChannel;
			NetMsgFB.nidMAP = pCHAR->GetCurrentMap().getGaeaMapID();
			SendClient ( dwClient, &NetMsgFB );
		}
	}
}

void CAgentServer::MsgVietTimeReset ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
//#if defined(VN_PARAM) //vietnamtest%%% && vietnamtest2
	if (m_ServiceProvider == SP_VIETNAM)
	{
		GLMSG::SNETPC_VIETNAM_TIME_RESET* pNetMsg = (GLMSG::SNETPC_VIETNAM_TIME_RESET *) nmg;

		GLCharAG* pChar = m_pGLAgentServer->GetChar(dwGaeaID);
		if( !pChar )
			return;

		if ( pChar->m_dwUserLvl < USER_GM3 )
			return;

		m_pClientManager->SetVTGameTime(dwClient,0);
		m_pClientManager->SetLoginTime(dwClient);
		
		pChar->m_sVietnamSystem.gameTime   = 0;
		pChar->m_sVietnamSystem.loginTime  = CTime::GetCurrentTime().GetTime();


		GLMSG::SNETPC_VIETNAM_TIME_REQ_FB NetMsgFB;
		NetMsgFB.gameTime  = pChar->m_sVietnamSystem.gameTime;
		NetMsgFB.loginTime = pChar->m_sVietnamSystem.loginTime;
		SendField( dwClient, &NetMsgFB );
	}
//#endif

	return;
}

void CAgentServer::MsgApexData( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (nmg)
    {
	    NET_APEX_DATA* msg = (NET_APEX_DATA*)nmg;
	    int nLength = msg->Size() - sizeof(NET_MSG_GENERIC);
        if (nLength < 0 || nLength >= NET_APEX_DATA::MAX_APEX_PACKET_LENGTH)
            return;

	    APEX_SERVER::NoticeApexProxy_UserData(dwClient, msg->szData, nLength);
    }
}

// Apex 적용( 홍콩 )
void CAgentServer::MsgApexReturn( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (nmg == NULL) return;

	NET_APEX_RETURN* msg = (NET_APEX_RETURN*)nmg;
	APEX_SERVER::NoticeApexProxy_UserReturn( dwClient, msg->nReturn );

	sc::writeLogError(sc::string::format("APEX MsgApexReturn::NoticeApexProxy_UserReturn : %d", dwClient));
}


void CAgentServer::MsgTracingLogPrint( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (!nmg)
        return;

//#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	//if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
	//{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR* pPacket = (GLMSG::NET_LOG_UPDATE_TRACINGCHAR*)nmg;
		int nTracing = CheckTracingUser( pPacket->szAccount, pPacket->nUserNum );
		if( nTracing == -1 )
			return;
 		CDebugSet::ToTracingFile(m_vecTracingData[nTracing].m_szAccount, pPacket->szLogMsg);
	//}
//#endif
}

void CAgentServer::MsgHeartbeatClientAns( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	NET_HEARTBEAT_CLIENT_ANS* nmg2 = ( NET_HEARTBEAT_CLIENT_ANS* ) nmg;
	TCHAR* szEncKey = m_pClientManager->GetEncKey( dwClient );
	if ( _tcslen( szEncKey ) > 0  )
	{				
		TCHAR szEncryptKey[ENCRYPT_KEY+1];
		memcpy( szEncryptKey, nmg2->szEnCrypt, sizeof(TCHAR) * ENCRYPT_KEY+1 );

		m_Tea.decrypt (szEncryptKey, ENCRYPT_KEY+1);
		if ( _tcscmp( szEncryptKey, szEncKey ) )
		{
			sc::writeLogError(
                sc::string::format(
                    "HeartBeat %s %s UserID %s CloseClient %d",
                    szEncryptKey,
                    szEncKey,
                    m_pClientManager->GetUserID(dwClient),
                    dwClient));
			return;
		}
	}

	m_pClientManager->SetEncKey( dwClient, nmg2->szEnCrypt );
	m_pClientManager->SetHeartBeat( dwClient );
}

void CAgentServer::MsgServerLoginMaintenanceAGT(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLCharAG* pChar = m_pGLAgentServer->GetChar(dwGaeaID);
	if (!pChar)
        return;
	if (pChar->m_dwUserLvl < USER_GM3)
        return;

	GLMSG::SNET_SERVER_LOGINMAINTENANCE_AGT* pNetMsg = ( GLMSG::SNET_SERVER_LOGINMAINTENANCE_AGT* ) nmg;

	int bOn = (int)pNetMsg->bOn;
	sc::writeLogInfo( sc::string::format("Login Maintenance %d %d" ,  pChar->m_CharDbNum, bOn ));

	SNET_SERVER_LOGINMAINTENANCE_SESSION msg;
	msg.bOn = pNetMsg->bOn;
	SendSession( &msg );			
}

void CAgentServer::MsgGmLogItemReloadCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg)
{
    // NET_GM_LOG_ITEM_RELOAD_CA
    int UserLevel = m_pClientManager->GetUserType(ClientSlot);
    if (UserLevel < USER_GM3)
    {
        sc::writeLogInfo(
            sc::string::format(
                "MsgGmLogItemReloadCA have not permission. %1%", UserLevel));
    }
    else
    {
        GLMSG::NET_GM_LOG_ITEM_RELOAD_AS Msg;
        SendSession(&Msg);

        // Notify to GM
        std::string ChatMsg(std::string("Item list updated."));
        GLMSG::SNET_SERVER_GENERALCHAT SendData;
        if (SendData.SETTEXT(ChatMsg))
        {
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);
            SendClient(ClientSlot, NET_MSG_SERVER_GENERALCHAT, SendBuffer);
        }
    }
}

void CAgentServer::MsgGmGameDataUpdateCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg)
{
	GLMSG::NET_GM_GAME_DATA_UPDATE_CA* pNetMsg = ( GLMSG::NET_GM_GAME_DATA_UPDATE_CA* ) pMsg;

    int UserLevel = m_pClientManager->GetUserType(ClientSlot);
    if (UserLevel < USER_GM3)
    {
        sc::writeLogInfo(
            sc::string::format(
                "MsgGmGameDataUpdateCA have not permission. %1%", UserLevel));
        return;
    }

	switch ( pNetMsg->nDataType )
	{
	case ItemList:
		{
			// UserDB 는 바로 업데이트 한다.
			size_t ItemCount = 0;
			size_t MaxMid = GLogicData::GetInstance().GetItemMaxMid();
			size_t MaxSid = GLogicData::GetInstance().GetItemMaxSid();
			for (size_t i=0; i<MaxMid; ++i)
			{
				for (size_t j=0; j<MaxSid; ++j)
				{
					const SITEM* pItemData = GLogicData::GetInstance().GetItem((WORD) i, (WORD) j);
					if (pItemData)
					{
						const std::string& ItemName = pItemData->GetNameStr();
						m_pAdoManager->AddUserJob(
							db::DbActionPtr(
							new UserDbLogItemDataInsertUpdate(
							(int) i,
							(int) j,
							ItemName)));
						++ItemCount;
					}
				}
			}

			sc::writeLogInfo(
				sc::string::format(
				"MsgGmGameDataUpdateCA Updated %1% Items", ItemCount));
		}
		break;
	}

    // Session 으로 전송해서 모든 서버그룹에 Log DB 업데이트를 알린다.
    GLMSG::NET_GM_GAME_DATA_UPDATE_AS MsgAS;
	MsgAS.nDataType = pNetMsg->nDataType;
    SendSession(&MsgAS);   
}

void CAgentServer::MsgGmGameDataUpdateSS( NET_MSG_GENERIC* nmg )
{
	GLMSG::NET_GM_GAME_DATA_UPDATE_SS* pNetMsg = ( GLMSG::NET_GM_GAME_DATA_UPDATE_SS* ) nmg;

	size_t nCount = 0;
	switch ( pNetMsg->nDataType )
	{
	case ItemList:
		{
			size_t MaxMid = GLogicData::GetInstance().GetItemMaxMid();
			size_t MaxSid = GLogicData::GetInstance().GetItemMaxSid();
			for (size_t i=0; i<MaxMid; ++i)
			{
				for (size_t j=0; j<MaxSid; ++j)
				{
					const SITEM* pItemData = GLogicData::GetInstance().GetItem((WORD) i, (WORD) j);
					if (pItemData)
					{
						std::string ItemName = pItemData->GetNameStr();
						if ( ItemName.empty() )
						{
							ItemName = "NoName";
						}

						m_pAdoManager->AddLogJob(
							db::DbActionPtr(
							new db::LogItemListInsertUpdate(
							(int) i,
							(int) j,
							ItemName)));
						++nCount;
					}
				}
			}

			sc::writeLogInfo(
				sc::string::format(
				"MsgGmGameDataUpdateSS Updated %1% Items", nCount));
		}
		break;
	case QuestList:
		{
			GLQuestMan::MAPQUEST& Quest = GLQuestMan::GetInstance().GetMap();
			GLQuestMan::MAPQUEST_ITER iter = Quest.begin();
			GLQuestMan::MAPQUEST_ITER iter_end = Quest.end();
			for ( ; iter!=iter_end; ++iter )
			{
				GLQuestMan::SNODE& sNODE = iter->second;
				GLQUEST* pQuest = sNODE.pQUEST;

				if ( pQuest )
				{
					WORD wMID = pQuest->m_sNID.Mid();
					WORD wSID = pQuest->m_sNID.Sid();
					std::string strQuestTitle = pQuest->GetTitleStr();
					if ( strQuestTitle.empty() )
					{
						strQuestTitle = "NoName";
					}

					m_pAdoManager->AddLogJob( db::DbActionPtr( new db::LogQuestListInsertUpdate( wMID, wSID, strQuestTitle ) ) );
					nCount++;
				}
			}

			sc::writeLogInfo( 
				sc::string::format( 
				"MsgGmGameDataUpdateSS Updated %1% Quests", nCount ) );
		}
		break;
	case SkillList:
		{
			for ( int i = 0; i < EMSKILLCLASS_NSIZE; i++ )
			{
				for ( int j = 0; j < GLSkillMan::MAX_CLASSSKILL; j++ )
				{
					GLSKILL* pSKILL = GLSkillMan::GetInstance().GetData( i, j );
					if ( !pSKILL )
						continue;

					std::string strSkillName( pSKILL->GetName() );
					if ( strSkillName.empty() )
					{
						strSkillName = "NoName";
					}

					m_pAdoManager->AddLogJob( db::DbActionPtr( new db::LogSkillListInsertUpdate( pSKILL->GetId().wMainID, pSKILL->GetId().wSubID, strSkillName, pSKILL->m_sBASIC.dwGRADE ) ) );
					nCount++;
				}
			}

			sc::writeLogInfo( 
				sc::string::format( 
				"MsgGmGameDataUpdateSS Updated %1% Skills", nCount ) );
		}
		break;
	}

	std::string ChatMsg( "nothing" );
	switch ( pNetMsg->nDataType )
	{
	case ItemList:
		ChatMsg = sc::string::format( "%1% items updated.", nCount );
		break;
	case QuestList:
		ChatMsg = sc::string::format( "%1% quests updated.", nCount );
		break;
	case SkillList:
		ChatMsg = sc::string::format( "%1% skills updated.", nCount );
		break;
	}

	// Notify to GM
	GLMSG::SNET_SERVER_GENERALCHAT SendData;
	if ( SendData.SETTEXT( ChatMsg ) )
	{
		msgpack::sbuffer SendBuffer;
		msgpack::pack( SendBuffer, SendData );
		NET_MSG_PACK MsgPack;
		if ( MsgPack.SetData( NET_MSG_SERVER_GENERALCHAT, SendBuffer, false ) )
		{
			DWORD MaxClient = static_cast< DWORD >( m_nMaxClient );
			for ( DWORD i = 1; i < MaxClient; ++i )
			{
				if ( m_pClientManager->IsGaming( i ) && m_pClientManager->GetUserType( i ) >= USER_GM3 )
					SendClient( i, &MsgPack );
			}
		}
	}
}

void CAgentServer::MsgProtocolInfoCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    NET_PROTOCOL_INFO_CA* pPacket = (NET_PROTOCOL_INFO_CA*) nmg;
    if (pPacket->m_MaxClientProtocolNum != NET_MSG_GCTRL_MAX)
    {
        sc::writeLogError(
            sc::string::format(
                "Client And Server Protocol Mismatch %1% != %2%",
                    pPacket->m_MaxClientProtocolNum,
                    NET_MSG_GCTRL_MAX));
    }

    NET_PROTOCOL_INFO_AC MsgAC(NET_MSG_GCTRL_MAX);
    SendClient(dwClient, &MsgAC);
}

void CAgentServer::MsgGmDbUnlockUser(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_GM_DB_UNLOCK_USER* pPacket = (GLMSG::NET_GM_DB_UNLOCK_USER*) pMsg;
    int UserLevel = m_pClientManager->GetUserType(ClientSlot);
    if (UserLevel < USER_GM3)
    {
        sc::writeLogError("MsgGmDbUnlockUser have not permission");
    }
    else
    {
        GLMSG::NET_GM_DB_UNLOCK_USER_AF MsgAF(pPacket->m_UserDbNum);
        SendAllChannel(&MsgAF);
    }  
}

void CAgentServer::MsgJson(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    NET_JSON* pPacket = (NET_JSON*) nmg;
    char JsonStr[NET_JSON::BUF_SIZE] = {0};
    StringCchCopy(JsonStr, NET_JSON::BUF_SIZE, pPacket->Buffer);
    MsgJsonParser(dwClient, dwGaeaID, std::string(JsonStr));
}

bool CAgentServer::MsgJsonParser(DWORD ClientSlot, DWORD GaeaID, const std::string& JsonStr)
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

    bool bReturn = true;
    switch (MessageType)
    {    
    case jm::MSG_CLUB_MEMBER_CD_CERTIFY_CA:
        MsgClubMemberCdCertifyCA(ClientSlot, GaeaID, JsonObject);
        break;

	case jm::MSG_GM_COMMAND_LOG_CA:
		MsgGmCommandLogCA( ClientSlot, GaeaID, JsonObject );
		break;
    
    default:		
        sc::writeLogError(
            sc::string::format(
                "Unknown Json message %1%", MessageType));
        bReturn = false;
        break;
    }
    return bReturn;
}

void CAgentServer::MsgPmarketSearchReqCA( DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_PMARKET_SEARCH_REQ_CAH nmg;
	GLCharAG* pChar = m_pGLAgentServer->GetChar(GaeaID);
	if (!pChar)
		return;
	nmg.dwcharDbnum = pChar->m_CharDbNum;
	nmg.dwGaeaID = GaeaID;
	SendCache(&nmg);
}

void CAgentServer::MsgSearchResultHA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_PMARKET_SEARCH_RESULT_HAC *pnmg = (GLMSG::SNETPC_PMARKET_SEARCH_RESULT_HAC *)nmg;
	GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum(pnmg->dwcharDbnum);
	if (!pChar)
		return;

	GLMSG::SNETPC_PMARKET_SEARCH_RESULT_HAC nMsg;
	nMsg.dwGaeaID = pnmg->dwGaeaID;
	SendClient(pChar->ClientSlot(), &nMsg);

}

void CAgentServer::MsgPmarketSearchOpenFA(DWORD ClientSlot, NET_MSG_GENERIC* nmg)
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return;
    }

    const GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_FA *pNetMsg = (GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_FA *)nmg;
    GLCharAG* pChar = m_pGLAgentServer->GetChar(pNetMsg->dwGaeaID);
    if (!pChar)
        return;
    //해당 유저 개인상점 검색카드 사용 Flag
    pChar->m_bUsedPMarketSeacrhCard = true;

    //필드 서버에 개인상점 검색카드 사용여부를 확인했음을 알린다.
    GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_AF MsgAF;
    MsgAF.dwGaeaID = pNetMsg->dwGaeaID;
    MsgAF.wPosX = pNetMsg->wPosX;
    MsgAF.wPosY = pNetMsg->wPosY;
    SendClient(ClientSlot, &MsgAF);
	*/
}

void CAgentServer::MsgPmarketSearchClose(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* nmg)
{
	/*
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return;
    }

    const GLMSG::SNET_INVEN_PMARKET_SEARCH_CLOSE *pnmg = (GLMSG::SNET_INVEN_PMARKET_SEARCH_CLOSE *)nmg;
    GLCharAG* pChar = m_pGLAgentServer->GetChar(GaeaID);
    if (!pChar)
        return;
    pChar->m_bUsedPMarketSeacrhCard = false;
	*/
}

void CAgentServer::MsgGmReferenceCountCA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLCharAG* pChar = m_pGLAgentServer->GetChar( dwGaeaID );
	if ( !pChar )
		return;

	if ( pChar->UserType() < USER_GM1 )
		return;

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

		SendAllChannel ( nmg );
	}
}

void CAgentServer::MsgGmLoadIPEventCA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLCharAG* pChar = m_pGLAgentServer->GetChar( dwGaeaID );
    if ( !pChar )
        return;

	if ( pChar->UserType() < USER_GM1 )
		return;

    ran_event::GLIPEvent::Instance()->Load( GLCONST_CHAR::strIPEventFileName, true );

    SendAllChannel( nmg );
}

void CAgentServer::MsgTexasHolem( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	m_pGLAgentServer->MsgTexasHolem( nmg, dwClient, dwGaeaID );
}

void CAgentServer::MsgUserSystemInfoCA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == m_pClientManager->IsAccountPass( dwClient ) )
	{
		return;
	}

	if ( false == m_pClientManager->IsClientSlot( dwClient ) )
	{
		return;
	}

	GLMSG::SNET_USER_SYSTEM_INFO_CA* pPacket = ( GLMSG::SNET_USER_SYSTEM_INFO_CA* ) nmg;
	int nUserNum = m_pClientManager->UserDbNum( dwClient );

	m_pAdoManager->AddUserJob( 
		db::DbActionPtr(
		new UserSystemInfoInsert(
		static_cast< DWORD >( nUserNum ), 
		pPacket->m_dwCpuCoreNumber,
		pPacket->m_dwCpuClock,
		pPacket->m_dwMemorySize,
		pPacket->m_dwResolutionX,
		pPacket->m_dwResolutionY,
		pPacket->m_dwGpuVendor,
		pPacket->m_dwGpuModel,
		pPacket->m_dwShaderModel, 
		pPacket->m_languageCode
		)), 
		m_pAdoManager );
}

void CAgentServer::MsgBlockUserFA( NET_MSG_GENERIC* nmg )
{
	NET_BLOCK_USER* pPacket = ( NET_BLOCK_USER* ) nmg;
	if ( pPacket->bBlock )
	{
		m_BlockUser.insert( std::map< DWORD, DWORD >::value_type( pPacket->dwUserNum, GetTickCount() ) );
	}
	else
	{
		std::map< DWORD, DWORD >::iterator iter = m_BlockUser.find( pPacket->dwUserNum );
		if ( iter != m_BlockUser.end() )
			m_BlockUser.erase( iter );
	}
}

bool CAgentServer::IsBlockUser( DWORD dwUserNum )
{
	std::map< DWORD, DWORD >::iterator iter = m_BlockUser.find( dwUserNum );
	if ( iter != m_BlockUser.end() )
	{
		// 필드서버들이 정상이고 블럭이 된지 많은 시간이 지났다면 뭔가 다른 문제가 있는거다. 우선은 게임할 수 있도록 한다.
		// 필드서버들이 정상인지 확인하는 코드는 현재 없다. 우선은 시간이 지나면 삭제하는 부분 추가 15분후에 삭제(단 로그인 요청을 해야 삭제됨)
		DWORD dwCurrentTickCount = GetTickCount();
		if ( dwCurrentTickCount - iter->second > 900000  )
		{
			m_BlockUser.erase( iter );
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

void CAgentServer::MsgGmCommandLogCA( DWORD ClientSlot, DWORD GaeaID, const json_spirit::mObject& Obj )
{
	GLCharAG* pChar = m_pGLAgentServer->GetChar( GaeaID );
	if ( !pChar )
		return;

	DWORD dwChaDbNum = pChar->ChaDbNum();
	std::string strCommand( "" );
	std::string strParam( "-" );
	bool bHaveParam = false;
	sc::Json::find_value( Obj, "COMMAND", strCommand );
	sc::Json::find_value( Obj, "HAVEPARAM", bHaveParam );
	if ( bHaveParam )
	{
		sc::Json::find_value( Obj, "PARAM", strParam );
	}

	m_pAdoManager->AddLogJob( db::DbActionPtr( new db::LogGmCommandInsert( dwChaDbNum, strCommand, strParam ) ) );
}

void CAgentServer::MsgUpdateUserCountryFA ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	sc::writeLogInfo ( "[ Country Log ] [ Change User Country ( Begin DB ) ]" );

	GLMSG::NET_USE_ITEM_INITIALIZE_COUNTRY_FA* pCountryMsg =
		static_cast< GLMSG::NET_USE_ITEM_INITIALIZE_COUNTRY_FA* >( nmg );

	// Update User Country Data;
	if ( pCountryMsg )
	{
		AddGameAdoJob( db::DbActionPtr(
			new CUpdateUserCountryAction( pCountryMsg->m_dwUserDbNum, Country::SCOUNTRY_INFO() ) ) );
	}
}

void CAgentServer::MsgUpdateUserCountryCA ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	sc::writeLogInfo ( "[ Country Log ] [ Change User Country ( Begin DB ) ]" );

	GLMSG::NET_COUNTRY_CHANGE_CA* pCountryMsg =
		static_cast< GLMSG::NET_COUNTRY_CHANGE_CA* >( nmg );

	// Update User Country Data;
	if ( pCountryMsg )
	{
		// 이 메시지는 Country Info가 세팅되지 않았을 때만 올 수 있다;
		// 세팅되어 있는데 왔다면 문제가 있는 패킷이다;
		if ( false == m_pClientManager->GetCountryInfo( dwClient ).ISNONCOUNTRY() )
		{
			sc::writeLogError ( "[ Country Log ] [ Invalid message 'Change Country' ]" );
			return;
		}

		DWORD dwUserID = static_cast< DWORD >( m_pClientManager->UserDbNum ( dwClient ) );

		AddGameAdoJob( db::DbActionPtr(
			new CUpdateUserCountryAction( dwUserID, pCountryMsg->m_sCountryInfo ) ) );
	}
}

void CAgentServer::MsgUpdateUserCountryDA ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	sc::writeLogInfo ( "[ Country Log ] [ Change User Country ( Begin Server ) ]" );

	GLMSG::NET_COUNTRY_CHANGE_DA* pCountryMsg =
		static_cast< GLMSG::NET_COUNTRY_CHANGE_DA* >( nmg );

	// Update User Country Data;
	if ( pCountryMsg )
	{
		DWORD dwClient = m_pClientManager->GetClientByUserNum( pCountryMsg->m_dwUserDbNum );
		if ( dwClient == MAXDWORD )
		{
			sc::writeLogError ( "[ Country Log ] [ Invalid Client ID / CAgentServer::MsgUpdateUserCountryDA  ]" );
			return;
		}

		m_pClientManager->SetCountryInfo (
			dwClient,
			pCountryMsg->m_sCountryInfo );

		sc::writeLogInfo ( "[ Country Log ] [ Change User Country ( End Server ) ]" );

		// Send Client
		GLMSG::NET_COUNTRY_CHANGE_AFC sMsg (
			pCountryMsg->m_sCountryInfo );
		SendAllChannel( &sMsg );

		SendClient( dwClient, &sMsg );
	}
}


void CAgentServer::MsgHeartbeatServerRequest ( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo( std::string(
		"[HEARTBEAT] Recv Heartbeat Request from Agent Server" ) );
#endif

	NET_HEARTBEAT_SERVER_ANS sMsg;
	SendClient( dwClient, &sMsg );

#ifdef HEARTBEAT_LOG
	sc::writeLogInfo( std::string(
		"[HEARTBEAT] Send Heartbeat Response to Agent Server" ) );
#endif
}

void CAgentServer::MsgHeartbeatServerAnswer ( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
#ifdef HEARTBEAT_LOG
	sc::writeLogInfo( sc::string::format(
		"[HEARTBEAT] Recv Heartbeat Response from another Agent Server ( %1% )",
		dwClient ) );
#endif

	m_pClientManager->SetHeartBeat( dwClient );
}

void CAgentServer::MsgAgentRelayAC ( DWORD _dwClient, NET_MSG_GENERIC_RELAY_AGENT* _pMsg )
{
	if ( NULL == _pMsg )
		return;

	SendClient( _pMsg, _pMsg->dwChaNum );
}

void CAgentServer::MsgAgentRelayAF ( DWORD _dwClient, NET_MSG_GENERIC_RELAY_AGENT* _pMsg )
{
	if ( NULL == _pMsg )
		return;

	GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum( _pMsg->dwChaNum );
	if ( NULL == pChar )
		return;

	SendField( pChar->ClientSlot(), _pMsg );
}

void CAgentServer::MsgAgentRelayAB ( DWORD _dwClient, NET_MSG_GENERIC_RELAY_AGENT* _pMsg )
{
	if ( NULL == _pMsg )
		return;

	SendAllClient( _pMsg );
}

void CAgentServer::MsgDisconnectCharacterFA( NET_MSG_GENERIC* nmg )
{
	// agent hang 의심 코드 수정
	// 메시지 처리에서 CloseClient 하지 않고, ReserveDropOutPC 변경
	NET_DISCONNECT_CHARACTER* pPacket = ( NET_DISCONNECT_CHARACTER* ) nmg;

	GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum( pPacket->ChaDbNum );
	if ( pChar )
	{
		m_pGLAgentServer->ReserveDropOutPC( pChar->m_dwGaeaID );
	}
}

void CAgentServer::MsgRanMobileCommandRequest( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_RANMOBILE_COMMAND_REQUEST* pNetMsg = (GLMSG::SNET_RANMOBILE_COMMAND_REQUEST*) nmg;

	m_pAdoManager->AddGameJob( db::DbActionPtr( new db::RanMobileRequest(
		m_pClientManager->UserDbNum( dwClient ),
		0, m_pGLAgentServer->GetServerGroup(),
		pNetMsg->nCommand, pNetMsg->nValue, "",	0 ) ) );
}

void CAgentServer::MsgRanMobileCommandResponse( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE* pNetMsg = (GLMSG::SNET_RANMOBILE_COMMAND_RESPONSE*) nmg;
	SendClient( m_pClientManager->GetClientByUserNum( pNetMsg->dwUserDbNum ), pNetMsg );
}