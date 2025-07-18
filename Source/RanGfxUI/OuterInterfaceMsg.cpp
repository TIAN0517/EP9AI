#include "StdAfx.h"
#include "../SigmaCore/Database/DbDefine.h"

#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/BasicButton.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/Msg/LoginMsg.h"
#include "../../RanLogic/Msg/RegisterMsg.h"

#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/GLGaeaClient.h"

#include "OldUI/Util/ModalWindow.h"
#include "OldUI/Interface/SelectCharacterPage.h"
#include "OldUI/Interface/SelectCharacterLeftPage.h"
#include "OldUI/Interface/CreateCharacterPage.h"
#include "OldUI/Interface/SelectServerPage.h"
#include "OldUI/Interface/NewCreateCharPage.h"
#include "OldUI/Interface/CreateCharacterWindow.h"
#include "OuterInterface.h"
#include "OldUI/Interface/IngameUserPanel/ResetPasswordPage2.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void COuterInterface::MsgProcess ( LPVOID pBuffer )
{
	if( !pBuffer ) return ;

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) pBuffer;

	EMSERVICE_PROVIDER sp = m_pGaeaClient->GetServiceProvider();

	switch ( nmg->nType )
	{

	case NET_MSG_REGISTER_FB:
		{
			NET_REGISTER_FEEDBACK_DATA* pPacket = (NET_REGISTER_FEEDBACK_DATA *) nmg;

			m_pRegisterPage->SetStatus ( (int)pPacket->m_Result );
		}
		break;

		//userpanel
	case CHINA_NET_MSG_RESETPIN_FB:
		{
			NET_RPN_FEEDBACK_DATA2* nfld = (NET_RPN_FEEDBACK_DATA2*) nmg;
			switch (nfld->nResult)
			{
			case EM_RESETPIN_FB_SUB_FAIL : 
				{
					DoModal ( "Reset Pin Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			case EM_RESETPIN_FB_SUB_OK :
				{
					CString strText;
					strText.Format("Reset Pin Success : %s",nfld->szUserpass2Hold);
					DoModal ( strText.GetString(), UI::MODAL_INFOMATION,  UI::OK, OUTER_MODAL_RESETPIN );
				}
				break;
			default :
				{
					DoModal ( "Reset Pin Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			}
		}break;
	case CHINA_NET_MSG_RESETPASS_FB:
		{
			NET_RPS_FEEDBACK_DATA2* nfld = (NET_RPS_FEEDBACK_DATA2*) nmg;
			switch (nfld->nResult)
			{
			case EM_RESETPASS_FB_SUB_FAIL : 
				{
					DoModal ( "Reset Pass Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			case EM_RESETPASS_FB_SUB_OK :
				{
					CString strText;
					strText.Format("Reset Password : %s",nfld->szUserid);
					DoModal ( "Information , Passed. the system is now prompting you to Change Password Page.", UI::MODAL_INFOMATION,  UI::OK, OUTER_MODAL_RESETPASS );
					m_pResetPasswordPage2->SetTitleName(strText);
				}
				break;
			default :
				{
					DoModal ( "Reset Pass Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			}
		}break;
	case CHINA_NET_MSG_RESETPASS2_FB:
		{
			NET_RPS2_FEEDBACK_DATA2* nfld = (NET_RPS2_FEEDBACK_DATA2*) nmg;
			switch (nfld->nResult)
			{
			case EM_EMAIL_FB_SUB_FAIL : 
				{
					DoModal ( "Reset Pass Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			case EM_EMAIL_FB_SUB_OK :
				{
					DoModal ( "Reset Pass Success", UI::MODAL_INFOMATION,  UI::OK, OUTER_MODAL_RESETPASS2 );
				}
				break;
			default :
				{
					DoModal ( "Reset Pass Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			}
		}break;

	case CHINA_NET_MSG_EMAIL_FB:
		{
			NET_CE_FEEDBACK_DATA2* nfld = (NET_CE_FEEDBACK_DATA2*) nmg;

			switch (nfld->nResult)
			{
			case EM_EMAIL_FB_SUB_FAIL : 
				{
					DoModal ( "Email Update Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			case EM_EMAIL_FB_SUB_OK :
				{
					DoModal ( "Email Update Success", UI::MODAL_INFOMATION,  UI::OK, OUTER_MODAL_CHANGEEMAIL );
				}
				break;
			default :
				{
					DoModal ( "Email Update Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			}

		}break;

	case CHINA_NET_MSG_GT2VP_FB:
		{
			NET_GTCVT_FEEDBACK_DATA2* nfld = (NET_GTCVT_FEEDBACK_DATA2*) nmg;

			switch (nfld->nResult)
			{
			case EM_GT2VP_FB_SUB_FAIL : 
				{
					DoModal ( "You must reach Level 210 to convert your GT!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			case EM_GT2VP_FB_SUB_OK :
				{
					DoModal ( "GameTime Conversion Success", UI::MODAL_INFOMATION,  UI::OK, OUTER_MODAL_GAMETIMECVT );
				}
				break;
			default :
				{
					DoModal ( "GameTime Conversion Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			}

		}break;

	case CHINA_NET_MSG_PP2VP_FB:
		{
			NET_PPCVT_FEEDBACK_DATA2* nfld = (NET_PPCVT_FEEDBACK_DATA2*) nmg;

			switch (nfld->nResult)
			{
			case EM_PP2VP_FB_SUB_FAIL : 
				{
					DoModal ( "Premium Point Conversion Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			case EM_PP2VP_FB_SUB_OK :
				{
					DoModal ( "Premium Point Conversion Success", UI::MODAL_INFOMATION,  UI::OK, OUTER_MODAL_PREMIUMPOINTCVT );
				}
				break;
			default :
				{
					DoModal ( "Premium Point Conversion Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			}

		}break;

	case CHINA_NET_MSG_TOPUP_FB:
		{
			NET_TOPUP_FEEDBACK_DATA2* nfld = (NET_TOPUP_FEEDBACK_DATA2*) nmg;

			switch (nfld->nResult)
			{
			case EM_TOPUP_FB_SUB_FAIL : 
				{
					DoModal ( "Top Up Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			case EM_TOPUP_FB_SUB_OK :
				{
					DoModal ( "Top Up Success", UI::MODAL_INFOMATION,  UI::OK, OUTER_MODAL_TOPUP );
				}
				break;
			default :
				{
					DoModal ( "Top Up Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			}

		}break;

	case CHINA_NET_MSG_PIN_FB:
		{
			NET_CPN_FEEDBACK_DATA2* nfld = (NET_CPN_FEEDBACK_DATA2*) nmg;

			switch (nfld->nResult)
			{
			case EM_PIN_FB_SUB_FAIL : 
				{
					DoModal ( "Pin Update Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			case EM_PIN_FB_SUB_OK :
				{
					DoModal ( "Pin Update Success", UI::MODAL_INFOMATION,  UI::OK, OUTER_MODAL_CHANGEPIN );
				}
				break;
			default :
				{
					DoModal ( "Pin Update Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			}

		}break;

	case CHINA_NET_MSG_PASS_FB:
		{
			NET_CP_FEEDBACK_DATA2* nfld = (NET_CP_FEEDBACK_DATA2*) nmg;

			switch (nfld->nResult)
			{
			case EM_PASS_FB_SUB_FAIL : 
				{
					DoModal ( "Password Update Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			case EM_PASS_FB_SUB_OK :
				{
					DoModal ( "Password Update Success", UI::MODAL_INFOMATION,  UI::OK, OUTER_MODAL_CHANGEPASS );
				}
				break;
			default :
				{
					DoModal ( "Password Update Error!", UI::MODAL_INFOMATION,  UI::OK );

				}
				break;
			}

		}break;

	case NET_MSG_LOGIN_FB:
		{
            DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
			if ( NULL == pGlobalStage )
				return;

			NET_LOGIN_FEEDBACK_DATA* pPacket = (NET_LOGIN_FEEDBACK_DATA *) nmg;

			pGlobalStage->SetWorldBattleID( pPacket->m_szDaumGID );

			int nServerPatchProgramVer = pPacket->m_LauncherVersion;
			int nServerGameProgramVer = pPacket->m_GameVersion;

			SetCreateExtreme( pPacket->m_Extreme );
			SetCharRemain(pPacket->m_ChaRemain);
            SetCountry(pPacket->m_Country);
            if ( pPacket->m_Result == EM_LOGIN_FB_SUB_OK )
            {
                if ( GetCountry() == wb::UNKNOWN_COUNTRY )
                    ToSelectCountryPage();
            }

            int nClientPatchProgramVer;
			int nClientGameProgramVer;

			BOOL bResult = FALSE;
			bResult = LoadClientVersion(nClientPatchProgramVer, nClientGameProgramVer);
			if (!bResult)
			{
				UiHideGroup ( MODAL_WINDOW_OUTER );
				DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED_VER_FILE"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				return;
			}

            BOOL bDifferentVer = FALSE;
            if ( (nClientPatchProgramVer < nServerPatchProgramVer) ||
				(nClientGameProgramVer < nServerGameProgramVer) )
			{
				bDifferentVer = TRUE;
			}
	
			switch ( pPacket->m_Result )
			{
			case EM_LOGIN_FB_SUB_OK:	// 로그인 성공				
			case EM_LOGIN_FB_KR_OK_USE_PASS:	// 로그인 성공
			case EM_LOGIN_FB_KR_OK_NEW_PASS:	// 로그인 성공
				{

				// 0 : 2차비번 사용안함. 1 : 2차비번 사용 , 2 : 2차비번 신규설정
				if(pPacket->m_Result == EM_LOGIN_FB_KR_OK_USE_PASS)
					m_pGaeaClient->SetUse2ndPass( 1 );
				else if(pPacket->m_Result == EM_LOGIN_FB_KR_OK_NEW_PASS)
					m_pGaeaClient->SetUse2ndPass( 2 );
				else
					m_pGaeaClient->SetUse2ndPass( 0 );

				CString strEmail , strText,strText2,strText3 , strUserID;
				strUserID.Format("%s",pPacket->m_szDaumGID);
				strEmail.Format("%s",pPacket->szUserEmail);
				if ( strlen(strEmail) > 5 )
				{
					int AtOffset = -1;
					unsigned int Length = (int)strlen(pPacket->szUserEmail); // Length = StringLength (strlen) of EmailAddress
					for(unsigned int i = 0; i < Length; i++)
					{
						if(pPacket->szUserEmail[i] == '@') // If one of the characters is @, store it's position in AtOffset
							AtOffset = (int)i;
					}

					strText.Format("%s", strEmail.Left( 1 ));
					for (unsigned int i = 0; i < AtOffset-3; i++)
					{
						strText += "*";
					}
					strText2.Format("%s",strEmail.Mid(AtOffset-1,Length));
					strText3.Format("%s%s",strText.GetString(),strText2.GetString());
					SetUserInfo ( pPacket->uUserPoints , pPacket->uBattlePoints  , pPacket->dwGameTime , strText3.GetString() , strUserID );
				}
				else
					SetUserInfo ( pPacket->uUserPoints , pPacket->uBattlePoints  , pPacket->dwGameTime , "*************" , strUserID );


				UiHideGroup ( MODAL_WINDOW_OUTER );

				//	버전이 틀릴 경우
	
				if ( bDifferentVer )
				{
					DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED_INVALID_VER"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
					return ;
				}
/*
//#if defined(KRT_PARAM) || defined(KR_PARAM)
#if defined ( JP_PARAM )	// 2차 패스워드
				if ( RANPARAM::VALIDIDPARAM () )
				{
					SetDaumGID( CString(nlfd->szDaumGID) );
					ToSecPassPage ( SELECT_SERVER_PAGE, nlfd->nCheckFlag );
				}
				else
				{
					ToSecPassPage ( LOGIN_PAGE, nlfd->nCheckFlag );
				}
#else
*/

                if ( m_pGaeaClient->GetServiceProvider() == SP_WORLD_BATTLE )
                {
                    return;
                }

				//	로그인 성공 & 캐릭터 선택 화면으로 이동
				pGlobalStage->OnInitLogin();
                
                if (m_pGaeaClient)
				{
                    m_pGaeaClient->SetUserCountry(pPacket->m_Country);
					m_pGaeaClient->SetCountryInfo(pPacket->m_sCountryInfo);
				}

				pGlobalStage->GetNetClient()->SndChaAllBasicInfo();
				// 로그인 프로세스 확인을 위한 로그
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Request Basic Char Info ( Recv Ret )"));

                if ( RANPARAM::VALIDIDPARAM() )
					ToSelectCharacterPage( SELECT_SERVER_PAGE );
				else
					ToSelectCharacterPage( LOGIN_PAGE );

				if (sp == SP_CHINA)
				{
//#if defined ( CH_PARAM ) // 중국 MMOSPEED
					//RunMMOSpeed();
				}
//#endif


				}
				break;

			case EM_LOGIN_FB_SUB_FAIL:		// 로그인 실패				
				DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );				
				break;

			case EM_LOGIN_FB_SUB_SYSTEM:	// 시스템에러로 로그인 실패
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_8"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );				
				break;

			case EM_LOGIN_FB_SUB_USAGE:		// 허가된 사용시간 에러
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_9"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );				
				break;

			case EM_LOGIN_FB_SUB_DUP:		// 중복된 접속
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_10"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );				
				break;

			case EM_LOGIN_FB_SUB_INCORRECT:	// ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_11"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );				
				break;

			case EM_LOGIN_FB_SUB_IP_BAN:	// ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_12"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				break;

			case EM_LOGIN_FB_CH_FULL: // 최대 동접 인원 도달시 NET_MSG_SVR_FULL 메시지가 온다.
				m_pSelectServerPage->SetChannelFull();
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_FULL"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				ToSelectServerPage ( LOGIN_PAGE );
				break;

			// 2004-02-26 jgkim 추가
			case EM_LOGIN_FB_SUB_BLOCK:		// ID/PWD 일치하지만 ID 가 Block 된 상태
                DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_BLOCK"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				break;

			case EM_LOGIN_FB_SUB_RANDOM_PASS:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_RANDOM_PASS_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				break;

			case EM_LOGIN_FB_SUB_ALREADYOFFLINE: // // 로그인 실패이고 이미 오프라인일때
				DoModal ( ID2GAMEEXTEXT ("LOGIN_ALREADYOFFLINE"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Daum : 비정상적 로그인데이타 (새롭게 웹에서 로그인후 게임실행)
			case EM_LOGIN_FB_SUB_UNCON:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_INVALID_LOGIN"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Daum : 인증시간이 초과된 사용자이기 때문에, 웹에 재접속을 하라는 메시지를 보낸다
			case EM_LOGIN_FB_SUB_EXPIRED:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_EXPIRED_USER"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Daum : GID가 없음 웹에서 새롭게 로그인
			case EM_LOGIN_FB_SUB_GID_ERR:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_NOGID"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Daum : UID가 없음 웹에서 새롭게 로그인
			case EM_LOGIN_FB_SUB_UID_ERR:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_NOUID"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Daum : 유저 삽입실패
			case EM_LOGIN_FB_SUB_UNKNOWN:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_UNKNOWN_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;		

			// Daum : 주민번호오류
			case EM_LOGIN_FB_SUB_SSNHEAD:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_SSN_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;		

			// Daum : 성인인증실패
			case EM_LOGIN_FB_SUB_ADULT:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_ADULT_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Thai : 18 세미만 3 시간 경과
			case EM_LOGIN_FB_THAI_UNDER18_3HOUR:
				DoModal( ID2GAMEEXTEXT("LOGINSTAGE_18LESS_TIME_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Thai : 18 세미만 접속불가능 시간
			case EM_LOGIN_FB_THAI_UNDER18_TIME:
				DoModal( ID2GAMEEXTEXT("LOGINSTAGE_18LESS_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// Thai : 18 세이상 접속불가능 시간
			case EM_LOGIN_FB_THAI_OVER18_TIME:
				DoModal( ID2GAMEEXTEXT("LOGINSTAGE_18MORE_FAILED"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			// 말레샤 ip차단 %%%
			// My : 말레이시아 접속 불간능한 ip
			case EM_LOGIN_FB_SUB_IMPOSSIBLEIP:
				DoModal( ID2GAMEEXTEXT("EM_LOGIN_FB_SUB_IMPOSSIBLEIP"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			case EM_LOGIN_FB_SUB_BETAKEY:
				DoModal( ID2GAMEEXTEXT("EM_LOGIN_FB_SUB_BETAKEY"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				break;

			//case WAIT_MSG_FROM_SERVER:
			//	DoModalOuter ( ID2GAMEEXTEXT ("LOGINSTAGE_6"), UI::MODAL_INFOMATION, UI::OK );				
			//	break;

            case EM_LOGIN_FB_KOR_SHUTDOWN:
                DoModal(ID2GAMEEXTEXT("EM_LOGIN_FB_KOR_SHUTDOWN"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME);
                break;

			default:
				break;
			};
		}
		break;

	case NET_MSG_PASSCHECK_FB:
		{
			NET_PASSCHECK_FEEDBACK_DATA* nlfd = (NET_PASSCHECK_FEEDBACK_DATA *) nmg;
			UiHideGroup(MODAL_WINDOW_OUTER);
            DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();

			switch ( nlfd->nResult )
			{
			case EM_LOGIN_FB_SUB_OK : // 로그인 성공
				//	로그인 성공 & 캐릭터 선택 화면으로 이동                
				pGlobalStage->OnInitLogin();
                if (m_pGaeaClient)
                    m_pGaeaClient->SetUserCountry(m_Country);
				pGlobalStage->GetNetClient()->SndChaAllBasicInfo();
				// 로그인 프로세스 확인을 위한 로그
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Request Basic Char Info ( Success PassCheck )"));
				ToSelectCharacterPage ( SECPASS_CHECKPAGE );
				break;

			case EM_LOGIN_FB_SUB_FAIL : // ID / PWD 불일치 (로그인 실패)
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_11"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				SetModalCallWindowID( SECPASS_CHECKPAGE );
				break;

			case EM_LOGIN_FB_SUB_PASS_OK : // 초기 패스워드 입력 성공
				//	로그인 성공 & 캐릭터 선택 화면으로 이동
				pGlobalStage->OnInitLogin();
                if (m_pGaeaClient)
                    m_pGaeaClient->SetUserCountry(m_Country);
				pGlobalStage->GetNetClient()->SndChaAllBasicInfo();
				// 로그인 프로세스 확인을 위한 로그
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN PROCESS. Request Basic Char Info ( Init PassCheck )"));

				ToSelectCharacterPage ( SECPASS_SETPAGE );
				break;
			case EM_LOGIN_FB_SUB_SECID_ALREADY:
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_13"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				SetModalCallWindowID( SECPASS_SETPAGE );
				break;

            case sc::db::DB_ERROR : // 초기 패스워드 업데이트 실패
			case USHRT_MAX :
				DoModal ( ID2GAMEEXTEXT ("LOGINSTAGE_11"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_GENERAL_LOGINFAIL );
				SetModalCallWindowID( SECPASS_SETPAGE );
				break;	
			}
		}
		break;
	case JAPAN_NET_MSG_UUID:
		{
			JAPAN_NET_LOGIN_UUID* netMsg = (JAPAN_NET_LOGIN_UUID*) nmg;
			RANPARAM::SETJAPANUUID( netMsg->szUUID );
		}
		break;
	case NET_MSG_LOBBY_REQ_GAME_JOIN:
		{
			//// 2차비번
			EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

			if( sp == SP_HONGKONG )
			{
				DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_16"), UI::MODAL_INPUT, UI::EDITBOX, OUTER_MODAL_JOIN_SECOUNDPASSWORD, TRUE, TRUE );
			}
			else
			{
				// 2차비번
				if( m_pGaeaClient->GetUse2ndPass() == 0 )
				{
					GetSelectCharacterPage()->SndGameJoin ();
				}
				else if( m_pGaeaClient->GetUse2ndPass() == 1 )
				{
					DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_16"), UI::MODAL_INPUT, UI::EDITBOX, OUTER_MODAL_JOIN_SECOUNDPASSWORD_GS, TRUE, TRUE );
				}
				else if( m_pGaeaClient->GetUse2ndPass() == 2 )
				{
					DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_20"), UI::MODAL_INPUT, UI::EDITBOX_CHECK_PWD, OUTER_MODAL_JOIN_SECOUNDPASSWORD_GS, TRUE, TRUE );
				}
				else
				{
					sc::writeLogError(std::string("NET_MSG_LOBBY_REQ_GAME_JOIN - ERROR"));
				}
			}
		}
		break;

	case NET_MSG_LOBBY_GAME_COMPLETE:
		{
			CDebugSet::ToListView ( "NET_MSG_LOBBY_GAME_COMPLETE" );

            DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
			if ( pGlobalStage )
			{
				pGlobalStage->ToChangeStage(DxGlobalStage::EM_STAGE_GAME);
			}
			else
			{
				sc::writeLogError( "Failed Join Game...!!!" );
			}
		}
		break;

	case NET_MSG_LOBBY_CHAR_JOIN_FB:
		{
			GLMSG::SNETLOBBY_CHARJOIN_FB *pNetMsg = (GLMSG::SNETLOBBY_CHARJOIN_FB *) nmg;
			DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
			
			switch (pNetMsg->emCharJoinFB )
			{
			case EMCJOIN_FB_NOWLOGIN: //접속중
				// 로그인 프로세스 확인을 위한 로그
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 0"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_10" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_ALREADYCONNECT );
				break;
			case EMCJOIN_FB_ERROR: //에러
				// 로그인 프로세스 확인을 위한 로그
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 1"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_11" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CREATEFAILED );
				break;
			case EMCJOIN_FB_PKPOINT: //pk 성향으로 오류
				// 로그인 프로세스 확인을 위한 로그
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 2"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_12" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_NON_PK_CHANNEL );
				break;
			case EMCJOIN_FB_SAVE_CHA: //아직 저장중인 캐릭터가 있어서 접속 할 수 없다.
				// 로그인 프로세스 확인을 위한 로그
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 3"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_14" ), UI::MODAL_INFOMATION, UI::OK );
				break;
			// 2차비번
			case EMCJOIN_FB_WAIT: // 잠시후 다시 시도해주세요
				// 로그인 프로세스 확인을 위한 로그
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 4"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_15" ), UI::MODAL_INFOMATION, UI::OK );
				break;
			case EMCJOIN_FB_SAVE_CHA2: //아직 저장중인 캐릭터가 있어서 접속 할 수 없다.
				// 로그인 프로세스 확인을 위한 로그
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 5"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_14" ), UI::MODAL_INFOMATION, UI::OK );
				break;
			case EMCJOIN_FB_OVERSTAT_RANDOMOPTION: //랜덤옵션이 문제가 있는 캐릭터이다.
				// 로그인 프로세스 확인을 위한 로그
				if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. OutInterface JOIN ERROR CODE 6"));

				DoModal ( ID2GAMEEXTEXT ( "CHARACTERSTAGE_RANDOMOPTION_ERROR" ), UI::MODAL_INFOMATION, UI::OK );
				break;
			//////////////////////////////////////////
			};
		}
		break;

		// 2차비번
	case NET_MSG_LOBBY_CHARJOIN_2NDPASS:
		{
			GLMSG::SNETLOBBY_CHARJOIN_2NDPASS *pNetMsg = (GLMSG::SNETLOBBY_CHARJOIN_2NDPASS *) nmg;

			switch (pNetMsg->nResult )
			{				
			case EMPASS_FAIL:
				DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_17"), UI::MODAL_INFOMATION, UI::OK );
				break;
			case EMPASS_BLOCK:
				DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_18"), UI::MODAL_INFOMATION, UI::OK );
				break;
			default:
				DoModal ( ID2GAMEEXTEXT ("CHARACTERSTAGE_19"), UI::MODAL_INFOMATION, UI::OK );
				break;
			}
		}
		break;
		/////////////////////////////////////////////////////

	case NET_MSG_CHA_DEL_FB_OK:
		{
			UiHideGroup( MODAL_WINDOW_OUTER, true );

			NET_CHA_DEL_FB* pCharDelFb = (NET_CHA_DEL_FB*) nmg;
			//GASSERT( m_pDxLobyStage );
			m_pGaeaClient->GetGlobalStage()->GetLobyStage()->DelChar( pCharDelFb->nChaNum );

			//	캐릭터 카드수 재조정
			SetCreateExtreme( pCharDelFb->bExtreme );
			SetCharRemain( static_cast<USHORT>(pCharDelFb->wChaRemain) );

			DoModal ( ID2GAMEEXTEXT ( "CHAR_DEL_SUCCEED" ), UI::MODAL_INFOMATION, UI::OK );
		}
		break;

	case NET_MSG_CHA_DEL_FB_CLUB:
		{
			UiHideGroup( MODAL_WINDOW_OUTER, true );
			DoModal ( ID2GAMEEXTEXT ( "CHAR_DEL_CLUB_FAILED" ), UI::MODAL_INFOMATION, UI::OK );
		}
		break;

	case NET_MSG_CHA_DEL_FB_ERROR:
		{
			UiHideGroup( MODAL_WINDOW_OUTER, true );
			DoModal ( ID2GAMEEXTEXT ( "CHAR_DEL_FAILED" ), UI::MODAL_INFOMATION, UI::OK );
		}
		break;

	case NET_MSG_CHA_NEW_FB:
		{
			NET_NEW_CHA_FB* pCharNewFb = (NET_NEW_CHA_FB*) nmg;
			//pCharNewFb->nChaNum;

			switch ( pCharNewFb->nResult )
			{
			case EM_CHA_NEW_FB_SUB_OK:	// 캐릭터 생성 성공
				{						
					DoModal ( ID2GAMEEXTEXT ( "CREATESTAGE_4" ), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CREATESUCCEED );

					//	캐릭터 카드수 재조정
					SetCreateExtreme( pCharNewFb->bExtreme );
					//SetCreateJumping( pCharNewFb->m_bJumping );
					SetCharRemain( static_cast<USHORT>(pCharNewFb->wChaRemain) );
				}
				break;		

			case EM_CHA_NEW_FB_SUB_DUP: // 이미 같은 이름의 캐릭터가 있음
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_5"), UI::MODAL_INFOMATION, UI::OK);
				break;

			case EM_CHA_NEW_FB_SUB_GM: // GM 계정은 캐릭터 생성 불가
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_11"), UI::MODAL_INFOMATION, UI::OK);
				break;

			case EM_CHA_NEW_FB_SUB_LENGTH: // 허용된 캐릭터 이름 길이보다 짧음
				DoModal(ID2GAMEEXTEXT("NEWCHAR_NAME_TOO_SHORT"), UI::MODAL_INFOMATION, UI::OK);
				break;

			case EM_CHA_NEW_FB_SUB_MAX: // 생성할수 있는 최대 캐릭터수 초과
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_6"), UI::MODAL_INFOMATION, UI::OK);
				break;

			case EM_CHA_NEW_FB_SUB_ERROR: // 시스템에러 또는 허용되지 않는 캐릭터 이름
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_7"), UI::MODAL_INFOMATION, UI::OK);
				break;
			case EM_CHA_NEW_FB_SUB_THAICHAR_ERROR:
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_9"), UI::MODAL_INFOMATION, UI::OK);
				break;
			case EM_CHA_NEW_FB_SUB_VNCHAR_ERROR:
				DoModal(ID2GAMEEXTEXT("CREATESTAGE_10"), UI::MODAL_INFOMATION, UI::OK);
				break;
			case EM_CHA_NEW_FB_SUB_BAD_NAME:
				DoModal(ID2GAMEEXTEXT("CHARACTER_BADNAME"), UI::MODAL_INFOMATION, UI::OK);
				break;
            case EM_CHA_NEW_FB_SUB_BAD_NAME_KOREA:
                DoModal(ID2GAMEEXTEXT("CREATE_CHARACTER_NAME_ERROR_KOREA"), UI::MODAL_INFOMATION, UI::OK);
                break;
            case EM_CHA_NEW_FB_SUB_BAD_NAME_WB:
                DoModal(ID2GAMEEXTEXT("CREATE_CHARACTER_NAME_ERROR_WB"), UI::MODAL_INFOMATION, UI::OK);
                break;
            default:
                DoModal("NET_MSG_CHA_NEW_FB unknown type", UI::MODAL_INFOMATION, UI::OK);
                break;
			}
			m_pCreateCharacterWindow->UseCancelButton();
		}
		break;
	case GS_NET_MSG_WB_LOGIN_FB :
		if( sp == SP_GS )
		{
			GS_NET_WB_LOGIN_DATA_FB* pPacket = (GS_NET_WB_LOGIN_DATA_FB *) nmg;

			// 전장서버에 접속해 있지 않거나 전장서버가 꺼져있는경우
			if( pPacket->bLoginWB == false || pPacket->bWBOff == true )
			{
				DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
				if ( NULL == pGlobalStage )
					return;

				CNetClient* pNetClient = pGlobalStage->GetNetClient();

				if( strcmp( pGlobalStage->GetWorldBattleID(), pPacket->szUserid ) == 0 )
				{
					pNetClient->GsSndLogin( pGlobalStage->GetWorldBattleID(), pGlobalStage->GetPWDForWB(), pPacket->nChannel );
				}
				else
				{
					DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED_ID_WB"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
					return;
				}
			}
			else
			{
				DoModal ( ID2GAMEEXTEXT ("LOGIN_FAILED_WB"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_CLOSEGAME );
				return ;
			}

		}
		else
		{
			sc::writeLogError( std::string("WB Login can use only GS") );
		}
		break;
//#ifdef CH_PARAM_USEGAIN //chinaTest%%%
	case NET_MSG_LOBBY_CHINA_ERROR:
		if (sp == SP_CHINA)
		{
			DoModal ( ID2GAMEEXTEXT ( "CHINA_LOGIN_GAINTIME_ERROR" ), UI::MODAL_INFOMATION, UI::OK );
		}
//#endif
		break;
	};
}


void COuterInterface::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{	
	switch ( ControlID )
	{
	case SELECT_SERVER_PAGE:
	case LOGIN_PAGE:
	case REGISTER_PAGE:
	case CREATE_CHARACTER_PAGE:
	case CREATE_CHARACTER_BUTTON_PAGE:
	case CREATE_CHARACTER_WEB:
	case SELECT_CHINA_AREA_PAGE:
	case CHINA_TITLE:
	case COPYRIGHT:
	case SECPASS_SETPAGE:
	case SECPASS_CHECKPAGE:
	case PASS_KEYBOARD:
	case MODAL_WINDOW_OUTER:
	case OUTER_UP_IMAGE:
	case OUTER_DN_IMAGE:
    case SELECT_COUNTRY_CONFIRM_MODAL:
    case SELECT_COUNTRY_WINDOW:
    case SELECT_CLUB_WINDOW:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) ) SetOnTheInterface ();
		}
		break;

	case SELECT_COUNTRY_UI:
		{
			if ( UIMSG_MOUSEIN_SELECT_COUNTRYUI & dwMsg ) SetOnTheInterface ();
		}
		break;

	case SELECT_CONTINENT_LIST_UI:
		{
			if ( UIMSG_MOUSEIN_SELECT_CONTINENT_LIST_UI & dwMsg ) SetOnTheInterface ();
		}
		break;

	case SELECT_COUNTRY_LIST_UI:
		{
			if ( UIMSG_MOUSEIN_SELECT_COUNTRY_LIST_UI & dwMsg ) SetOnTheInterface ();
		}
		break;

	case SELECT_CHARACTER_PAGE:
		{
			if ( UIMSG_MOUSEIN_SELECT_CHARACTERPAGE & dwMsg ) SetOnTheInterface ();
		}
		break;

	case SELECT_CHARACTER_LEFTPAGE:
		{
			if ( UIMSG_MOUSEIN_SELECT_CHARACTERLEFTPAGE & dwMsg ) SetOnTheInterface ();
		}
		break;

	case FULLSCREEN_OUTER:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					const float fBUTTON_CLICK = CBasicButton::fBUTTON_CLICK;

					const UIRECT& rcPos = m_pFullScreenButtonDummy->GetGlobalPos ();				
					m_pFullScreenButton->SetGlobalPos ( D3DXVECTOR2 ( rcPos.left + fBUTTON_CLICK, rcPos.top + fBUTTON_CLICK ) );
				}

				if (CHECK_MOUSEIN_LBUPLIKE(dwMsg))
				{
                    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
					pGlobalStage->ChangeWndMode();
				}
			}
		}
		break;
	}

	switch ( ControlID )
	{
	case MODAL_WINDOW_OUTER:
		{			
			if ( m_pModalWindow )
			{
				UIGUID CallerID = m_pModalWindow->GetCallerID ();
				ModalMsgProcess ( CallerID, dwMsg );
			}
		}
		break;		
	}
}
