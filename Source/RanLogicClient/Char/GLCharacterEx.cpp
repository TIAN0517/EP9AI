#include "../pch.h"
#include <shlobj.h>
#include <tlhelp32.h>
#include <Psapi.h>

#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/Cursor.h"
#include "../../EngineLib/GUInterface/NSGUI.h"
#include "../../EngineLib/DxEffect/EffProj/DxEffProj.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffGameHelp.h"
#include "../../EngineLib/DxEffect/Single/Util/DxEffFanwise.h"
#include "../../EngineLib/DxEffect/Single/Util/DxEffPlane.h"
#include "../../EngineLib/DxEffect/Single/Util/DxEffCircle.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxEffect/DxPostEffect.h" // by luxes.
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxFootColl.h"
#include "../../Enginelib/DxMeshs/SkinMesh/DxVehicle.h"
#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxEnvironment.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxSound/BgmSound.h"
#include "../../EngineLib/Common/DXInputString.h"
#include "../../EngineLib/Common/CommonWeb.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/DxResponseMan.h"
#include "../../EngineLib/GlobalParam.h"
#include "../../EngineLib/DxEffect/EffectLua/NSEffectLuaPlay.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"
//#include "../../RanLogic/Item/GLItemMixMan.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/TransformSet/GLTransformSet.h"
#include "../../RanLogic/stl_Func.h"

#include "../../RanLogicClient/Product/GLProductClient.h"
// 
//#include "../../RanGfxUI/OldUI/Interface/SkillTrayTab.h"

#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"
#include "../Strike/GLStrikeM.h"
#include "../Party/GLPartyClient.h"
#include "../Friend/GLFriendClient.h"
#include "../Post/GLPostClient.h"
#include "../SNS/GLSNSClient.h"
#include "../Effect/GLFactEffect.h"
#include "../Tutorial/GLBaseTutorial.h"
#include "../Attendance/GLAttendanceClient.h"
#include "../CapturedTheField/GLCaptureTheFieldClient.h"
#include "../Command/dxincommand.h"
#include "../GLogicExClient.h"
#include "../Land/GLLandManClient.h"
#include "../Input/GLInputManager.h"
#include "./GLCharacter.h"
#include "../RanLogicClient/Input/GLInputManager.h"
#include "../../RanGfxUI/InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bEFFECT_TOOL;

HRESULT GLCharacter::PlayerUpdate ( float fTime, float fElapsedTime )
{
	//	Note : »ç¿ëÀÚ ÀÔ·Â°ª Àû¿ë.
	//
	DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

	DWORD dwKeyR(NULL);
	DWORD dwKeyX(NULL);
	DWORD dwKeyZ(NULL);
	DWORD dwKeyLAlt(NULL);
	DWORD dwKeyLCtrl(NULL);
	DWORD dwKeyLShift(NULL);
	DWORD dwkeySPACE(NULL);
	DWORD dwML(NULL), dwMM(NULL), dwMR(NULL);

	// Ã¤ÆÃÀ©µµ¿ì È°¼ºÈ­½Ã Å°º¸µå °¡Á® ¿ÀÁö ¾ÊÀ½.
    EngineDeviceMan* pDevice = m_pGaeaClient->GetEngineDevice();
    DXInputString* pInputString = pDevice->GetInputString();
	//if ( !pInputString->IsOn() && COMMON_WEB::GetTopWindowID() < 0 )
	//{
	//	dwKeyR = dxInputDev.GetKeyState(RANPARAM::MenuShotcut[SHOTCUT_RUN]);
	//	dwKeyZ = dxInputDev.GetKeyState(DIK_Z);
	//	dwkeySPACE = dxInputDev.GetKeyState(DIK_SPACE);		
	//	dwKeyX = dxInputDev.GetKeyState(RANPARAM::MenuShotcut[SHOTCUT_ATTACKMODE]);
	//}

    // TODO : ºÒ¹ý ÇÁ·Î±×·¥ °¨Áö Å×½ºÆ®..
	if ( !m_sPMarket.IsOpen() ) 
	{
		if( dxInputDev.IsUpdatInputState() )
		{
			m_fKeyDownCheckTime = 0.0f;
            m_bAwayGesture = false;
		}
        else
        {
			m_fKeyDownCheckTime += fElapsedTime;


            if( m_fKeyDownCheckTime >= GLCONST_CHAR::fBECOME_AWAY_TIME )
			{
                if ( !m_bAwayGesture )
                {
			        m_bAwayGesture = true;

                    ReqGESTURE( GLCONST_CHAR::nAWAY_GESTURE_NUMBER );		
                }
			}

			if(m_pGaeaClient->IsCTFPlayer() &&  GLUseFeatures::GetInstance().IsUsingafkBanCTF() )
			{
				if( m_fKeyDownCheckTime >= GLCONST_CHAR::fCTF_BECOME_AWAY_TIME )
				{
					PrintMsgText(NS_UITEXTCOLOR::CHAT_SYSTEM,ID2GAMEINTEXT("COMPETITIONNOTIFY_IN_AFK_WARNNING_1") );
					//ÆÃ±¸±â
					m_pGaeaClient->ReqCaptureTheFieldQuit();
				}
				else
				{
					float fleftime = GLCONST_CHAR::fCTF_BECOME_AWAY_TIME  -m_fKeyDownCheckTime;
					if(fleftime > 0)
					{
						int vsize = GLCONST_CHAR::vecCTFNotify.size();
						DWORD temppos = -1;

						for(int i = vsize-1 ; i >-1 ; --i  )
						{
							if( fleftime <GLCONST_CHAR::vecCTFNotify[i] )
							{
								temppos = i;
							}
						}

						if(temppos != -1 && temppos != m_nAwayGestureLevel)
						{
							//Ã¤ÆÃ »Ñ¸®±â						
							PrintMsgText(NS_UITEXTCOLOR::CHAT_SYSTEM, sc::string::format(ID2GAMEINTEXT("COMPETITIONNOTIFY_IN_AFK_WARNNING_0") , GLCONST_CHAR::vecCTFNotify[temppos]) );
							m_nAwayGestureLevel = temppos;
						}
					}
				}
			}
			
		}
	}
    else
    {
		m_fKeyDownCheckTime = 0.0f;
	}

	CDebugSet::ToView ( 1, "KeyDownCheckTime %f", m_fKeyDownCheckTime );

	dwKeyLAlt = dxInputDev.GetKeyState ( DIK_LMENU );
	dwKeyLCtrl = dxInputDev.GetKeyState ( DIK_LCONTROL );
	dwKeyLShift = dxInputDev.GetKeyState ( DIK_LSHIFT );


	//;;if( false == GfxInterfaceBase::GetInstance()->IsMouseHitUI() && 
	//	false == GfxInterfaceBase::GetInstance()->IsDragging() )
	{
		dwML = dxInputDev.GetMouseState ( DXMOUSE_LEFT );
		dwMM = dxInputDev.GetMouseState ( DXMOUSE_MIDDLE );
		dwMR = dxInputDev.GetMouseState ( DXMOUSE_RIGHT );
	}
	//else
	//{
	//	dwML = NULL;
	//	dwMM = NULL;
	//	dwMR = NULL;
	//}

	if ( m_sCONFTING.bFIGHT && m_sCONFTING.sOption.bSCHOOL )
	{
		//	Note : ÇÐ±³°£ ´ë·Ã ¸¶Å© Ç¥½Ã Update.
		//!! SF_TODO
		m_pGaeaClient->GetInterface()->SetAcademyUpdateNumber(
			m_pGaeaClient->GetMyPartyClient()->GetNConfrontMember(), (int) m_sCONFTING.setConftMember.size());
	}

	return S_OK;
}

HRESULT GLCharacter::SelectTargetUpdate( const bool bUsingSelectInfo )
{
	// Note : ´ë»óÀ» ¼±ÅÃÇßÀ» °æ¿ì
	if( m_sSelectID.GaeaId != EMTARGET_NULL )
	{
		ClientActor* pACTTAR = m_pGaeaClient->GetCopyActor ( m_sSelectID );
		if ( pACTTAR && (pACTTAR->IsNonTargetable() == FALSE) )
		{
			D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );			
			if( m_sSelectID.emCrow == CROW_PC )
			{
				GLCharClient *pChar = (GLCharClient *)pACTTAR;
				vScale = -pChar->GetMinOrg() / 6.0f;				
			}
			if( m_sSelectID.emCrow == CROW_NPC || m_sSelectID.emCrow == CROW_MOB )
			{
				GLCrowClient *pCrow = (GLCrowClient *)pACTTAR;
				vScale = -pCrow->GetMinOrg() / 6.0f;				
			}else if( m_sSelectID.emCrow == CROW_MATERIAL )
			{
				GLMaterialClient *pMaterial = (GLMaterialClient *)pACTTAR;
				vScale = -pMaterial->GetMinOrg() / 6.0f;
			}else if( m_sSelectID.emCrow == CROW_SUMMON )
			{
				GLSummonClient *pSummon = (GLSummonClient *)pACTTAR;
				vScale = -pSummon->GetMinOrg() / 6.0f;
			}
			float fScale = max(vScale.x,vScale.z);

			D3DXMATRIX matCircleEffect, matArrowEffect;

			D3DXVECTOR3 vPosition	= pACTTAR->GetPosition();
			float		fBodyHeight = pACTTAR->GetBodyHeight()*1.4f/*bjju.temp*/;

			D3DXMatrixTranslation ( &matCircleEffect, vPosition.x, vPosition.y, vPosition.z );
			D3DXMatrixTranslation ( &matArrowEffect,  vPosition.x, vPosition.y + fBodyHeight, vPosition.z );

			m_sSelectID.vPos = pACTTAR->GetPosition();

			EMCROWCOLORTYPE		emColorType			 = m_pGaeaClient->GetCrowIdentifyColorType( m_sSelectID );
			GAMEHELP_EFFECTTYPE emGameHelpEffectType = EMTARGET_EFFECT_NULL;

			if ( bUsingSelectInfo )
			{
				switch ( emColorType )
				{
				case EMCROWCOLORTYPE_ENEMY:		emGameHelpEffectType = EMTARGET_EFFECT_ENEMY;		break;
				case EMCROWCOLORTYPE_NPC:		emGameHelpEffectType = EMTARGET_EFFECT_NPC;			break;
				case EMCROWCOLORTYPE_ALLIANCE:	emGameHelpEffectType = EMTARGET_EFFECT_ALLIANCE;	break;
				};
			}
			else
			{
				emGameHelpEffectType = EMTARGET_EFFECT_DEFAULT;
			}

			DxEffGameHelp::GetInstance().UpdateTargetEffect( emGameHelpEffectType, m_sSelectID,
				matCircleEffect, matArrowEffect, D3DXVECTOR3( fScale, fScale, fScale ) );


			if ( bUsingSelectInfo && pACTTAR->IsHideNaming() == false )
			{
				const EMCROW _emCrow = pACTTAR->GetCrow();
				switch ( _emCrow )
				{
				case CROW_SUMMON:
					{
						GLSummonClient* pSummonClient = MIN_BOOST::polymorphic_downcast<GLSummonClient*>(pACTTAR);
						if ( pSummonClient->m_dwOwnerGaeaID == m_pGaeaClient->GETMYGAEAID() )
						{
							// [shhan][2015.01.28]
							//	³» ¼ÒÈ¯¼ö´Â Á¤º¸¸¦ º¸Áö ¾Êµµ·Ï ÇÑ´Ù.
							//	ÀÌ°É ÇÏÁö ¾ÊÀ¸¸é TARGETINFO°¡ Ç¥½ÃÁßÀÎ »óÈ²¿¡¼­ ³» ¼ÒÈ¯¼ö¿¡ ¼±ÅÃÇÒ ½Ã ÀÌÀü UI °¡ ³²¾ÆÀÖÀ¸¸é¼­ Çã°ø¿¡ ¶°µµ´Â ¹®Á¦°¡ ¹ß»ýÇÒ ¼ö ÀÖÀ½.
							//!! SF_TODO
							m_pGaeaClient->GetInterface()->ResetTargetInfoSelect();
							//GfxInterfaceBase::GetInstance()->ResetTargetInfoSelect();

						}
						else
						{
							//!! SF_TODO
							m_pGaeaClient->GetInterface()->SetTargetInfoSelect( m_sSelectID );
							//GfxInterfaceBase::GetInstance()->GetOldInterface()->SetTargetInfoSelect(m_sSelectID);
							//GfxInterfaceBase::GetInstance()->SetTargetInfoSelect(m_sSelectID);

							
						}
					}
					break;
				default:
					//!! SF_TODO
					m_pGaeaClient->GetInterface()->SetTargetInfoSelect( m_sSelectID );
					//GfxInterfaceBase::GetInstance()->SetTargetInfoSelect(m_sSelectID);

					break;
				};
			}

		}else{
			DxEffGameHelp::GetInstance().BlindTargetEff();

			DxSkinChar* pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform);
			if ( pRenderChar == NULL )
				pRenderChar = m_pSkinChar;

			m_sSelectID.RESET();
			m_ReservationAction.Clear();

			if ( bUsingSelectInfo )
			{
				//!! SF_TODO
				m_pGaeaClient->GetInterface()->ResetTargetInfoSelect();
				//GfxInterfaceBase::GetInstance()->ResetTargetInfoSelect();
			}

			GLMSG::SNETPC_SELECT_TARGET NetMsg;
			NetMsg.emTarCrow = CROW_MOB;
			NetMsg.dwTarID = GAEAID_NULL;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
	}
	else
	{
		DxEffGameHelp::GetInstance().BlindTargetEff();

		if ( bUsingSelectInfo )
		{
			//!! SF_TODO
			m_pGaeaClient->GetInterface()->ResetTargetInfoSelect();
			//GfxInterfaceBase::GetInstance()->ResetTargetInfoSelect();

		}
	}

	return S_OK;
}

//! ¹üÀ§ÁöÁ¤ ½ºÅ³
HRESULT GLCharacter::ScopeSkillUpdate()
{
	SKILLID sSkillID = GetScopeSkillRun();
	DxEffGameHelp::GetInstance().BlindScopeEff();

	if ( sSkillID == SKILLID_NULL )
	{
		return S_FALSE;
	}

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.mid(), sSkillID.sid() );

	if ( !pSkill ) 
	{
		SetScopeSkillRun( SKILLID_NULL );
		return E_FAIL;
	}

	//! Ä¿¼­°¡ À©µµ¿ì ¾È¿¡ ÀÖÀ»°æ¿ì¸¸
	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt();
	if ( !DxViewPort::GetInstance().GetMouseTargetPosWnd(vTargetPt) )
	{
		return S_FALSE;
	}

	switch ( pSkill->m_sBASIC.emIMPACT_REALM )
	{
	default:
		{
			SetScopeSkillRun( SKILLID_NULL );
			return E_FAIL;
		}

	case REALM_SELF:
	case REALM_ZONE:
	case REALM_KEEP_ZONE:
		//! ¿øÇü
		{		
			bool bEnable = true;
			float fRadius = 0.0f;
			D3DXVECTOR3 vCollPos;

			if ( !SkillActivationInfoCircle( sSkillID, vFromPt, vTargetPt, bEnable, vCollPos, fRadius) )
			{
				return E_FAIL;
			}

			//! À§Ä¡ ¾÷µ¥ÀÌÆ®
			D3DXMATRIX matPos;
			D3DXMatrixTranslation ( &matPos, vCollPos.x, vCollPos.y+0.1f, vCollPos.z );

			PLANE_PROPERTY sProperty;
			sProperty.m_fWidth	= fRadius*2.0f;
			sProperty.m_fHeight = fRadius*2.0f;
			sProperty.m_vNormal	= D3DXVECTOR3(0.0f,1.0f,0.0f);

			if ( bEnable )
			{
				sProperty.m_dwColor = 0xFFA0A0FF; /*bjju.temp*/
			}
			else
			{
				sProperty.m_dwColor = 0xFFF01010; /*bjju.temp*/
			}

			//! CIRCLE_PROPERTY´Â »öÀÌ º¯ÇØµµ ¹öÅØ½º¹öÆÛ¸¦ »õ·Î ¸¸µéÁö ¾Ê´Â´Ù.
			CIRCLE_PROPERTY sProperty_OutLine;
			sProperty_OutLine.m_fRadius			 = fRadius;
			sProperty_OutLine.m_fVertexPerDegree = 0.5f;
			sProperty_OutLine.m_fOutLine		 = 1.0f;		/*bjju.temp*/
			sProperty_OutLine.m_dwOutLineColor	 = 0xFFFFFFFF;	/*bjju.temp*/

			DxEffGameHelp::GetInstance().UpdateScopePlaneEffect( sProperty, sProperty_OutLine, matPos );
		}
		break;
		
	case REALM_FANWIZE:
		//! ºÎÃ¤²Ã
		{

			//! À§Ä¡ ¾÷µ¥ÀÌÆ®
			D3DXVECTOR3 vOrgPos = GetPosition();
			D3DXVECTOR3 vPos	= vOrgPos;
			vPos.y += (GetBodyHeight()*0.5f);

			D3DXMATRIX matPos;
			D3DXMatrixTranslation ( &matPos, vPos.x, vPos.y, vPos.z );

			//! ¹æÇâ ¾÷µ¥ÀÌÆ®.
			D3DXVECTOR3 vDir;
			float fRadius = 0.0f;
			float fDegree = 0.0f;
			if ( !SkillActivationInfoFanwise( sSkillID, vFromPt, vTargetPt, vDir, fRadius, fDegree ) )
			{
				return E_FAIL;
			}

			float fThetaY = DXGetThetaYFromDirection ( vDir, D3DXVECTOR3(0.0f, 0.0f, 1.0f) );
			D3DXMATRIX matRotY;
			D3DXMatrixRotationY( &matRotY, fThetaY );

			//! ÃÖÁ¾ ÇÕ¼º
			D3DXMATRIX matWorld;
			matWorld = matRotY * matPos;

			FANWISE_PROPERTY sProperty;
			sProperty.m_fRadius			 = fRadius;
			sProperty.m_fDegree			 = fDegree;
			sProperty.m_fVertexPerDegree = 180.0f/sProperty.m_fDegree;
			sProperty.m_dwColor			 = 0xFFA0A0FF; /*bjju.temp*/
			DxEffGameHelp::GetInstance().UpdateFanwiseEffect( sProperty, matWorld );
		}
		break;
	}

	return S_OK;
}



// *****************************************************
// Desc: °ÔÀÌÆ® Á¶»ç
// *****************************************************
DWORD GLCharacter::DetectGate()
{
	if (IsActState(EM_REQ_GATEOUT))
        return UINT_MAX;

	PLANDMANCLIENT pLandMClient = m_pGaeaClient->GetActiveMap();
	if (!pLandMClient)
        return UINT_MAX;

	DxLandGateMan *pLandGateMan = pLandMClient->GetLandGateMan();
	if (!pLandGateMan)
        return UINT_MAX;
		
	if (pLandMClient->m_fAge < 3.0f)
        return UINT_MAX;

	PDXLANDGATE pLandGate = pLandGateMan->DetectGate(GetPosition());
	if (!pLandGate)
		return UINT_MAX;

	if ( pLandGate->GetToMapID() == SNATIVEID(254,0) && ( !m_bTutorial || !GLUseFeatures::GetInstance().IsUsingTutorialMode() ) )
		return UINT_MAX;
		
	if (pLandGate->GetFlags() & DxLandGate::GATE_OUT)
        return pLandGate->GetGateID();

	return UINT_MAX;
}

// *****************************************************
// Desc: °ÔÀÌÆ®ÀÇ ÀÌ¸§ Á¶»ç
// *****************************************************
void GLCharacter::DetectGateToMapName (MAPNAMEVECTOR& mapNameVector)
{
	mapNameVector.clear();
    CString strReturn("");
	if (IsActState(EM_REQ_GATEOUT))
        return;

	PLANDMANCLIENT pLandMClient = m_pGaeaClient->GetActiveMap();
	if (!pLandMClient)
        return;

	DxLandGateMan *pLandGateMan = pLandMClient->GetLandGateMan();
	if (!pLandGateMan)
        return;
		
	if (pLandMClient->m_fAge < 3.0f)
        return;

	PDXLANDGATE pLandGate = pLandGateMan->DetectGate(GetPosition());
	if (!pLandGate)
        return;

    // ÀÌº¥Æ® °ÔÀÌÆ® ÀÏ‹š´Â ÀÌ¸§À» Ãâ·ÂÇÏÁö ¾Ê´Â´Ù.
    if (pLandGate->GetEvent())
        return;
	
	if ( pLandGate->GetFlags()&DxLandGate::GATE_OUT )
	{
		for ( DWORD _i(0); _i < pLandGate->GetNGateToMapID(); ++_i )
		{
			SNATIVEID sToMapID = pLandGate->GetToMapID(_i);			
			SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode(sToMapID);
			if ( pMapNode )
			{
				const DWORD nGateNameIndex(pLandGate->GetGateNameIndex(_i));
				CString strMapName(m_pGaeaClient->GetMapName( sToMapID ));
				if ( nGateNameIndex == -1 )
					mapNameVector.push_back(strMapName);
				else
				{
					CString strGateToName(ID2GAMEWORD(pLandGate->GetGateNameKey(_i), nGateNameIndex));
					mapNameVector.push_back(strMapName + "," + strGateToName);
				}
			}
		}		
	}
}

float GLCharacter::GetMoveVelo()
{
    return GLOGICEX::GetMoveVelocity(
        IsActState(EM_ACT_RUN),
        GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO,
        GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO,
        m_sVehicle.m_bUseBoost ? true : false,
        GLCHARLOGIC::GETMOVEVELO(),
        GLCHARLOGIC::GETMOVE_ITEM(),
        m_fMoveVeloLimit_Min,
        m_fMoveVeloLimit_Max,
        IsCDMSafeTime(),
        GLCONST_CHAR::fCDM_SAFE_TIME_MOVE_SPEED);
/*
	float fDefaultVelo = IsSTATE(EM_ACT_RUN) ? GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO : GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO;
	float fMoveVelo = 0.0f;
	if (m_sVehicle.m_bUseBoost)
	{
		fMoveVelo = fDefaultVelo * ((GLCHARLOGIC::GETMOVEVELO() + GLCHARLOGIC::GETMOVE_ITEM()) * 1.5f);
	}
	else
	{
		fMoveVelo = fDefaultVelo * (GLCHARLOGIC::GETMOVEVELO() + GLCHARLOGIC::GETMOVE_ITEM());
	}

	if (IsCDMSafeTime())
        fMoveVelo = fMoveVelo * GLCONST_CHAR::fCDM_SAFE_TIME_MOVE_SPEED;

	return fMoveVelo;
*/
}

void GLCharacter::SetLockMovement(const bool bLock)
{
	m_bLockMoveMent = bLock;
	if ( bLock == false )
		return;

	m_ReservationAction.Clear();
}

void GLCharacter::VietnamCalculate()
{
	const int nMax1 = 180;
	const int nMax2 = 300;
	/*const int nMax1 = 30;
	const int nMax2 = 50;*/
	//!! SF_TODO
	m_pGaeaClient->GetInterface()->SET_VNGAINTYPE_GAUGE( 0, nMax2 );
   	if( m_sVietnamSystem.loginTime == 0 ) return;
	if( m_dwVietnamGainType == GAINTYPE_EMPTY ) 
	{
		//!! SF_TODO
		m_pGaeaClient->GetInterface()->SET_VNGAINTYPE_GAUGE( nMax2, nMax2 );
		return;
	}

	CTimeSpan gameTime( 0, (int)m_sVietnamSystem.gameTime / 60, (int)m_sVietnamSystem.gameTime % 60, 0 );


	CTime loginTime   = m_sVietnamSystem.loginTime;
	const CTime& crtTime = m_pGaeaClient->GetCurrentTime();
	CTimeSpan crtGameSpan;
	crtGameSpan			  = gameTime + ( crtTime - loginTime );

	int totalMinute  = (int)crtGameSpan.GetTotalMinutes();
	int totalSecond  = (int)crtGameSpan.GetTotalSeconds();

	CDebugSet::ToView ( 1, "Vietnam total Time M %d S %d", totalMinute, totalSecond );
	CDebugSet::ToView ( 2, "GameTime H %d M %d S %d Value %d", gameTime.GetTotalHours(), gameTime.GetTotalMinutes(), gameTime.GetTotalSeconds(),
						m_sVietnamSystem.gameTime );
	CDebugSet::ToView ( 3, "CrtTime Year %d Mon %d D %d H %d M %d S %d LoginTime Year %d Mon %d D %d H %d M %d S %d", 
							crtTime.GetYear(), crtTime.GetMonth(), crtTime.GetDay(), crtTime.GetHour(), crtTime.GetMinute(), crtTime.GetSecond(), 
							loginTime.GetYear(), loginTime.GetMonth(), loginTime.GetDay(), loginTime.GetHour(), loginTime.GetMinute(), loginTime.GetSecond() );


	if( m_dwVietnamGainType == GAINTYPE_MAX )
	{
		if( totalMinute > nMax1 )
			totalMinute = nMax1;
	}else if( m_dwVietnamGainType == GAINTYPE_HALF )
	{
		if( totalMinute < nMax1 )
			totalMinute = nMax1;
		if( totalMinute > nMax2 )
			totalMinute = nMax2;
	}

	if( totalMinute < 0 )  totalMinute = 0;
	
	//!! SF_TODO
	m_pGaeaClient->GetInterface()->SET_VNGAINTYPE_GAUGE( totalMinute, nMax2 );
	
	
	//	m_sVietnamSystem.currentGameTime = totalHours;
	//	m_sVietnamSystem.currentGameTime = totalMinutes;
	// Å×½ºÆ®´Â ÃÊ·Î ÀúÀå..
}

void GLCharacter::EventCalculate()
{
	if( m_bEventStart == FALSE )
	{
//		CDebugSet::ToView ( 1, "Event Start FALSE" );	
		return;
	}
	if( m_bEventApply == FALSE )
	{
//		CDebugSet::ToView ( 1, "Event Apply FALSE" );	
		return;
	}

	const CTime& crtTime = m_pGaeaClient->GetCurrentTime();
	CTime startTime = m_tLoginTime;
	CTimeSpan crtGameSpan;
	crtGameSpan					 = ( crtTime - startTime );

	if( m_bEventBuster == FALSE )
	{
		m_RemainEventTime  = m_EventStartTime - (int)crtGameSpan.GetTotalSeconds();
		m_RemainBusterTime = m_EventBusterTime;

		if( crtGameSpan.GetTotalSeconds() >= m_EventStartTime )
		{
//			m_bEventBuster = TRUE;	
			m_RemainEventTime = 0;
		}
	}

	if( m_bEventBuster == TRUE )
	{
		m_RemainEventTime  = 0;
		m_RemainBusterTime = (m_EventStartTime + m_EventBusterTime) - (int)crtGameSpan.GetTotalSeconds();
		if( m_RemainBusterTime > m_EventBusterTime )
			m_RemainBusterTime = m_EventBusterTime;

		if( crtGameSpan.GetTotalSeconds() >= m_EventStartTime + m_EventBusterTime )
		{
//			m_bEventBuster = FALSE;
//			m_tLoginTime   = crtTime.GetTime();

			m_RemainEventTime  = m_EventStartTime;
			m_RemainBusterTime = m_EventBusterTime;
		}
	}

	

	
	/*CDebugSet::ToView ( 1, "gameSpan: %d ;; bEventBuster: %d ;; startTime: %d ;; busterTime: %d", 
					    (int)crtGameSpan.GetTotalSeconds(), m_bEventBuster, m_EventStartTime, m_EventBusterTime );

	CDebugSet::ToView ( 2, "RemainEventTime: %d RemainBusterTime: %d", m_RemainEventTime, m_RemainBusterTime );*/

	
}

void GLCharacter::UpdateSpecialSkill( float fElapsedTime )
{
	const D3DXVECTOR3& _vPosition = GetPosition();
	// º¯½Å ½ºÅ³ Ã¼Å©
	if (m_dwTransformSkill != SNATIVEID::ID_NULL && m_dwTransformSkill < SKILLFACT_SIZE)
	{
		if( m_sSKILLFACT[m_dwTransformSkill].IsSpecialSkill( SKILL::EMSSTYPE_TRANSFORM ) )
		{
			// ¾²·¯Áö´Â ¸ð¼ÇÀÌ¸é º¯½ÅÇØÁ¦
			STARGETID sTargetID(CROW_PC,m_dwGaeaID,_vPosition);
			if( isAction( GLAT_FALLING ) )
			{
				FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_dwGaeaID,_vPosition), m_pSkinChar, m_sSKILLFACT[m_dwTransformSkill].sNATIVEID );
			}

			if( m_sSKILLFACT[m_dwTransformSkill].sNATIVEID != NATIVEID_NULL() ) 
			{			
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[m_dwTransformSkill].sNATIVEID.wMainID, m_sSKILLFACT[m_dwTransformSkill].sNATIVEID.wSubID );
				if( pSkill && pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSFORM )
				{
					if( pSkill->m_sSPECIAL_SKILL.strEffectName.size() != 0 )
					{
						if( m_sSKILLFACT[m_dwTransformSkill].fAGE <= pSkill->m_sSPECIAL_SKILL.dwRemainSecond && 
							!m_sSKILLFACT[m_dwTransformSkill].bRanderSpecialEffect )
						{
							D3DXMATRIX matTrans;							
							D3DXMatrixTranslation ( &matTrans, _vPosition.x, _vPosition.y, _vPosition.z );
							DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sSPECIAL_SKILL.strEffectName.c_str(), matTrans, &sTargetID, TRUE, TRUE );
							m_sSKILLFACT[m_dwTransformSkill].bRanderSpecialEffect = TRUE;
						}
					}
				}
			}
			else
			{
				m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, 
					m_sSKILLFACT[ m_dwTransformSkill ].sNATIVEID.dwID, m_dwTransformSkill );

				RESETSKEFF(m_dwTransformSkill);
				UpdateSuit(TRUE);				
				m_dwTransformSkill = SNATIVEID::ID_NULL;
			}

		}
		else
		{
			if( m_IdSkillTransform != NATIVEID_NULL() )
			{
				//m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, 
					//m_sSKILLFACT[ m_dwTransformSkill ].sNATIVEID.dwID, m_dwTransformSkill );
				m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, 
					m_IdSkillTransform.dwID, m_dwTransformSkill );

				if( m_sSKILLFACT[ m_dwTransformSkill ].sNATIVEID == m_IdSkillTransform )
					RESETSKEFF(m_dwTransformSkill);

				UpdateSuit(TRUE);
				m_dwTransformSkill = SNATIVEID::ID_NULL;
				m_IdSkillTransform = NATIVEID_NULL();
			}			
		}
	}
	
}

void GLCharacter::StartDealyAction( const SCHARSKILL& sSkill, GLCharacter* pOwnerChar, const SKILLTARGETDATA* pTargetData, bool bSubSkill )
{
	m_pWorkingSkill->StartDealyAction( sSkill, pOwnerChar, pTargetData, bSubSkill );
}

void GLCharacter::SET_RECORD_CHAT()
{
	if( m_bRecordChat ) return;
	m_bRecordChat = TRUE;
	//!! SF_TODO
	//m_strRecordChat		= m_pInterface->GET_RECORD_CHAT();
	m_recordStartTime	= m_pGaeaClient->GetCurrentTime();

	PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("RECORD_CHAT_START") );
}

void GLCharacter::UPDATE_RECORD_CHAT( bool bCloseClient /* = FALSE */ )
{
	if (!m_bRecordChat)
        return;
	const CTime& curTime = m_pGaeaClient->GetCurrentTime();

//	¼­¹ö Å¸ÀÓÀ¸·Î º¯È¯( TimeZone °è»ê )
//	m_pGaeaClient->GetConvertServerTime( curTime );
	
	CTimeSpan timeSpan = curTime - m_recordStartTime;
	if( timeSpan.GetMinutes() >= 2 || bCloseClient ) 
	{
		//!! SF_TODO
		m_strRecordChat += m_pGaeaClient->GetInterface()->GET_RECORD_CHAT();

		{
			TCHAR szPROFILE[MAX_PATH] = {0};
			TCHAR szFullPathFileName[MAX_PATH] = {0};
			SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

            const SUBPATH* pPath = m_pGaeaClient->GetSubPath();
			StringCchCopy( szFullPathFileName, MAX_PATH, szPROFILE );
			StringCchCat( szFullPathFileName, MAX_PATH, pPath->SaveRoot().c_str()); //SUBPATH::SAVE_ROOT );
			CreateDirectory( szFullPathFileName, NULL );

			CHAR  szFileName[MAX_PATH] = {0};
			sprintf_s( szFileName, MAX_PATH, "chat_[%d%d%d%d%d].txt", curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), 
													      curTime.GetHour(), curTime.GetMinute() );

			StringCchCat ( szFullPathFileName, MAX_PATH, szFileName );

			m_bRecordChat   = FALSE;

			CFile file;
			if ( ! file.Open( _T(szFullPathFileName), CFile::modeCreate|CFile::modeWrite|CFile::typeBinary ) )// ÆÄÀÏ ¿­±â
			{

				CDebugSet::ToListView ( "RECORD_CHAT_FAILED" );
				return;
			}

			CString strSaveChat;			
			for( int i = 0; i < m_strRecordChat.GetLength(); i++ )
			{
				char szTempChar = m_strRecordChat.GetAt(i) ^ 0x2139;
				strSaveChat += szTempChar;
			}

			int nLength = strSaveChat.GetLength();
			file.Write(&nLength, sizeof(int));
			file.Write(strSaveChat.GetString(), nLength);
			file.Close();


			m_strRecordChat = "";
		}

		if( !bCloseClient )
		{
			PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("RECORD_CHAT_END") );
		}

	}
}

void GLCharacter::UpdateMapEffect()
{
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if( !pLand ) return;

	if( pLand->m_bClubBattle ) 
	{
		if ( !m_pGaeaClient->IsClubBattle() ) 
		{
			DISABLEALLMAPEFF();
			return;
		}
	}
	else
	{
		DISABLEALLMAPEFF();
		return;
	}
}

void GLCharacter::UpdateLandEffect()
{
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if( !pLand ) return;

	if( !pLand->GetUseLevelEtcFunc(EMETCFUNC_LANDEFFECT) ) 
	{
		DISABLEALLLANDEFF();
		return;
	}


	// ¸ðµç ¹öÇÁ¸¦ Ãë¼ÒÇØ¾ßÇÒ °æ¿ìÀÇ Ã³¸®
	const D3DXVECTOR3& vPosition = GetPosition();
	for( DWORD i = 0; i < m_vecLandEffect.size(); i++ )
	{
		SLANDEFFECT_VALUE landEffect = m_vecLandEffect[i];
		if( !landEffect.IsUse() ) continue;
		if( landEffect.emLandEffectType == EMLANDEFFECT_CANCEL_ALLBUFF )
		{
			if( m_dwTransformSkill != SNATIVEID::ID_NULL )
			{
				m_dwTransformSkill = SNATIVEID::ID_NULL;
				UpdateSuit(TRUE);
			}
			for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
			{
				FACTEFF::DeleteSkillFactEffect(STARGETID(CROW_PC, m_dwGaeaID, vPosition), m_pSkinChar, GetSkillFactId(i));
				RESETSKEFF(i);
			}

			m_pGaeaClient->PartyMemberRemoveAllBuff( m_dwGaeaID );

			return;
		}
	}
}

HRESULT GLCharacter::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	UPDATE_RECORD_CHAT();
    
//#if defined(VN_PARAM) //vietnamtest%%%
    if (m_ServiceProvider == SP_VIETNAM)
	    VietnamCalculate();
//#endif

#ifndef CH_PARAM_USEGAIN
	EventCalculate();
#endif

	if ( m_dwUserLvl >= USER_GM3 ) DxViewPort::GetInstance().SetZoomOut( 400.0f );
	else DxViewPort::GetInstance().SetZoomOut( 250.0f );

	if ( DxViewPort::GetInstance().GetZoomOut() > 250.0f && m_dwUserLvl < USER_USER_GM ) m_pGaeaClient->GetGlobalStage()->CloseGame();

	//	ÆÄÆ¼Ã¤ÆÃ Á¦ÇÑ½Ã°£ ¾÷µ¥ÀÌÆ®
	if ( m_fPartyRecruitTime <= GLCONST_CHAR::fCHAT_PARTY_RECRUIT_TIME )
		m_fPartyRecruitTime += fElapsedTime;

	//  1ÃÊ¿¡ ÇÑ¹ø¾¿ ¾÷µ¥ÀÌÆ® ÇÑ´Ù.
    m_fCheckActivityItem += fElapsedTime;
    if ( ActivityBaseData::GetInstance()->m_bSync && m_fCheckActivityItem >= 1.0f )
    {
        UpdateActivityItem();
        m_fCheckActivityItem = 0.0f;
    }

	// ÀÎ´ø ÀçÇÑ ½Ã°£¿¡ °üÇÑ ¾÷µ¥ÀÌÆ®
	{
		MAPENTRYINFO_ITER iter = m_mapEntryLimitInfo.begin();
		for( ; iter != m_mapEntryLimitInfo.end();)
		{		
			if( iter->second.IsEntryLimitEnd() )
			{
				m_mapEntryLimitInfo.erase(iter++);
			}else{
				++iter;
			}

		}
	}

	//ºÎ½ºÆ® »ç¿ë½Ã°£ Ã¼Å©
	if( m_sVehicle.m_bUseBoost )
	{
		m_sVehicle.m_fBoostTime += fElapsedTime;
	
		if( m_sVehicle.m_fBoostTime > 6.0f )
		{
			m_sVehicle.m_fBoostTime = 6.0f;
		}
	}

	if( m_sVehicle.m_bUseBoostDelay )
	{
		m_sVehicle.m_fBoostDelay -= fElapsedTime;

		if( m_sVehicle.m_fBoostDelay <= 0.0f )
		{
			m_sVehicle.m_fBoostDelay = 0.0f;
		}
	}

	//¿îÀüÀÚ°¡ ºÎ½ºÅÍ »ç¿ëÁßÀÎÁö Ã¼Å©
	if( m_emVehicle == EMVEHICLE_PASSENGER )
	{
		std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(0) );

		if( pCharClient )
		{
			if( pCharClient->VehicleUseBoost() )
			{
				DxPostEffect::GetInstance().RadialBlurActiveOn();
			}
			else
			{
                DxPostEffect::GetInstance().RadialBlurActiveOff();
			}
		}
	}

	// ¸ÞÀÏ »óÅÂ ¾÷µ¥ÀÌÆ®
	m_pGaeaClient->GetPostClient()->FrameMove( fTime, fElapsedTime );

    // SNS ¾÷µ¥ÀÌÆ®
    m_pGaeaClient->GetSNSClient()->FrameMove( fTime, fElapsedTime );

    // Ãâ¼®ºÎ ¾÷µ¥ÀÌÆ®
    m_pGaeaClient->GetAttendance()->FrameMove( fTime, fElapsedTime );

    // ÇÊµå Á¡·ÉÀü ¾÷µ¥ÀÌÆ®
    m_pGaeaClient->GetCaptureTheField()->FrameMove( fTime, fElapsedTime );

    //  ¾ÆÀÌÅÛ Á¤·Ä °ü·Ã ½Ã°£ ¾÷µ¥ÀÌÆ®.
    m_fLimitTimeItemSort -= fElapsedTime;
    if ( m_fLimitTimeItemSort < 0.0f )
        m_fLimitTimeItemSort = 0.0f;

    if ( m_bSendItemSortMsg )
    {
        m_fItemSortMsgDelayTime -= fElapsedTime;
        if ( m_fItemSortMsgDelayTime < 0.0f )
            SetEnableSendItemSortMsg();
    }

	// ¸¸¾à ±â´Ù¸®°í ÀÖ´Ù¸é
	if ( IsActState(EM_ACT_WAITING) )
	{
		if ( ++m_dwWAIT > EMWAIT_COUNT )
		{
			m_dwWAIT = 0;
			ReSetActState(EM_ACT_WAITING);

			//	Note : ¸Ê¿¡ ÁøÀÔÇÔÀ» ¼­¹ö¿¡ ¾Ë·ÁÁÜ. ( ÁÖÀ§ È¯°æ Á¤º¸¸¦ ¹Þ±â À§ÇØ¼­. )
			//
			GLMSG::SNETREQ_LANDIN NetMsgLandIn(m_emDoActWaitTypePrev,m_emDoActWaitType);
			m_pGaeaClient->NETSENDTOAGENT(&NetMsgLandIn);
			CDebugSet::ToListView ( "GLMSG::SNETREQ_LANDIN" );

			//	Note : ¼­¹ö¿¡ ÁØºñµÇ¾úÀ½À» ¾Ë¸².
			//
			GLMSG::SNETREQ_READY NetMsgReady;
			m_pGaeaClient->NETSENDTOAGENT(&NetMsgReady);
			CDebugSet::ToListView ( "GLMSG::SNETREQ_READY" );

			//	Note : ¼­¹ö¿¡ Ä£±¸ ¸®½ºÆ® Àü¼Û ¿äÃ».
            //m_pGaeaClient->GetFriendClient()->ReqFriendList();

			// PET
			// ÆÖÀÌ È°µ¿Áß¿¡ ¸ÊÀ» ÀÌµ¿Çß´Ù¸é ´Ù½Ã ºÒ·¯ÁØ´Ù.
			if ( m_bIsPetActive )
			{
				 ReqReGenPet ();
				m_bIsPetActive = FALSE;
			}

			/*if ( m_bIsVehicleActive && m_sVehicle.IsActiveValue() ) 
			{
				if ( ReqSetVehicle( true ) == S_OK ) 
				{
					m_bIsVehicleActive = FALSE;
				}
			}*/

			// Å»°Í ÀåÂøÇÏ°í ÀÖ´Ù¸é Á¤º¸ ¿äÃ» 
			if ( m_PutOnItems[SLOT_VEHICLE].GetNativeID() != NATIVEID_NULL()  )
			{
				ReqVehicleUpdate();
			}

			// ÀÎº¥ÀÇ ÀåÂøÇÏ°í ÀÖ´Â Å»°Í Á¤º¸ ¿äÃ»
			if ( !m_bRecivedVehicleItemInfo ) 
			{
				ReqVehicleInvenUpdate();
				m_bRecivedVehicleItemInfo = true;
			}

			// µé°íÀÖ´Â ÆÖÄ«µå Á¤º¸ ¿äÃ»
			if ( !m_bReceivedPetCardInfo )
			{
				// PET
				// µé°íÀÖ´Â ÆÖÄ«µåÀÇ Á¤º¸¸¦ ¿äÃ»ÇÑ´Ù.
				ReqPetCardInfo ();

//#if defined KRT_PARAM || defined _RELEASED
#if defined _RELEASED
				// PET
				// ½ÃÈ¿¸¸·á·Î »ç¶óÁø ÆÖÄ«µåÀÇ ÆÖÁ¤º¸ ¿äÃ»
				ReqPetReviveInfo ();
#endif
				m_bReceivedPetCardInfo = true;
			}
		}
	}

	//	Note : »ì¾ÆÀÖÀ» ¶§¸¸ °»½ÅµÇ´Â °Íµé.
	//

	// Note : µµ½Ã¶ô »ç¿ë °¡´ÉÇÑ Áö¿ªÀÌ¸é ¸ðµç LUNCHBOX¸¦ Enable ÇÑ´Ù
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	BOOL bLunchBoxForbid = pLand ? pLand->IsLunchBoxForbid() : TRUE;
	if ( IsValidBody() && !m_sCONFTING.IsCONFRONTING() && !bLunchBoxForbid )
	{
		SETENABLEALLLUNCHBOX ( TRUE );
	}
	else
	{
		// Note : ¸ðµç LUNCHBOX¸¦ Disable ÇÑ´Ù
		SETENABLEALLLUNCHBOX ( FALSE );
	}

	//	Note : ÇÃ·¡ÀÌ¾î Á¶ÀÛ Ã³¸®.
	//		
	GLInputManager::GetInstance().FrameMove( fTime, fElapsedTime );

	if ( IsValidBody() )
	{
		if ( IsCDMSafeTime() )
			m_fCDMSafeTime -= fElapsedTime;		

		m_fQUEST_TIMER += fElapsedTime;

		if ( m_fQUEST_TIMER > 60.0f )
		{
			if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
			{
				ResetRnAttendaceQuest();
			}
			m_cQuestPlay.FrameMove ( fTime, fElapsedTime );
			m_fQUEST_TIMER = 0;
		}
		UpdateMacroState(fElapsedTime);
		UpdateActState();

		DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

		DWORD dwF7(DXKEY_IDLE);
		DWORD dwF8(DXKEY_IDLE);

		dwF7 = dxInputDev.GetKeyState( DIK_F7 );
		dwF8 = dxInputDev.GetKeyState( DIK_F8 );


		//// test for tutorial. by luxes.
		/*if ( dwF7 & DXKEY_PRESSED )
		{
			m_pGaeaClient->GetTutorial()->SetTutorialStep( GLBaseTutorial::ONE, GLBaseTutorial::ONE_ );
		}*/

		///if ( dwF8 & DXKEY_PRESSED  )
		//{
			//m_pGaeaClient->GetTutorial()->SetTutorialStep( GLBaseTutorial::TWO, GLBaseTutorial::ONE_ );
			//m_pGaeaClient->GetTutorial()->NextSubStep();
		//}
		

		//	Note : Ã¼·Â »óÅÂ °»½Å.
		//
		float fCONFT_POINT_RATE(1.0f);
		if ( m_sCONFTING.IsCONFRONTING() )
			fCONFT_POINT_RATE = m_sCONFTING.sOption.fHP_RATE;

		// ÁöÇü È¿°ú ¾÷µ¥ÀÌÆ®
		UpdateLandEffect();

		//	¸ÊÈ¿°ú ÀÌÆåÆ®
		UpdateMapEffect();

        // ½ºÅÏÁßÀÌ¶óµµ ¹Ð°í´ç±â±â ¾×¼Ç ÁßÀÌ¶ó¸é ¿òÁ÷ÀÓÀ» ¸ØÃßÁö ¾Ê´Â´Ù
        if ( m_bSTATE_STUN && !isAction( GLAT_PUSHPULL ) )
        {
            ResetReservationAction();
            m_sActionID.RESET();
			
            if ( ClientActor::PathIsActive() && m_emVehicle != EMVEHICLE_PASSENGER )
            {
                ClientActor::StopActorMove();
                TurnAction( GLAT_SHOCK );
            }
        }

		m_sAirborne.update(fElapsedTime);
		GLCHARLOGIC::UPDATE_DATA ( fTime, fElapsedTime, TRUE, fCONFT_POINT_RATE );

		if( RF_FEIGN_DEATH( m_EffSkillVarSet).IsOn() )	
		{
			if( !IsActState( EM_ACT_FEIGN_DIE) ){
				SetActState( EM_ACT_FEIGN_DIE );
				TurnAction( GLAT_FEIGN_FALLING );
			}
		}
	
		if ( m_bCLUB_CERTIFY )
		{
			m_fCLUB_CERTIFY_TIMER += fElapsedTime;

			float fRER = m_fCLUB_CERTIFY_TIMER / GLCONST_CHAR::fCDCERTIFY_TIME;
			if ( fRER > 1.0f )
				fRER = 1.0f;

			//!! SF_TODO
			m_pGaeaClient->GetInterface()->SET_CONFT_CONFIRM_PERCENT ( fRER );
		}		

		if( !IsCtrlBlockBody() )
		{
			if( ( !m_pGaeaClient->GetTutorial()->IsTutorial() ) ||
				( m_pGaeaClient->GetTutorial()->IsTutorial() && m_pGaeaClient->GetTutorial()->IsCanMoveInTutorial() ) )
			{
				PlayerUpdate ( fTime, fElapsedTime );
				ReservedActionUpdate ( fTime );
			} //if
		} //if

		if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
		{
			if ( !m_pGaeaClient->GetTutorial()->IsCanMoveInTutorial() )
			{	// Æ©Åä¸®¾ó·Î ÀÎÇØ, ÇÃ·¹ÀÌ¾î Á¶ÀÛÀÌ Àá°ÜÀÖÀ¸¸é, ¸®¾×¼Ç ÃÊ±âÈ­.
				// Memo : È¤½Ã ¸ô¶ó, Æ¯Á¤ ½ºÅÜÀÏ °æ¿ì¿¡¸¸ ¸®¾×¼Ç ÃÊ±âÈ­ ½ÃÅ²´Ù.
				//if ( m_pGaeaClient->GetTutorial()->IsAttackStep()
				//	&& m_pGaeaClient->GetTutorial()->IsSubThreeStep()
				//	|| m_pGaeaClient->GetTutorial()->IsAttackStep()
				//	&& m_pGaeaClient->GetTutorial()->IsSubFiveStep() )
				//{
				ResetReservationAction();
				//}
			}

			m_pGaeaClient->GetTutorial()->FrameMove( fElapsedTime, this );
		}

		if( m_bMiniMapMove && isAction( GLAT_MOVE ) )
		{
			m_fMiniMapMoveTime += fElapsedTime;
			if( m_fMiniMapMoveTime >= 1.0f )
			{
				m_fMiniMapMoveTime = 0.0f;

				// ÀÌ°Å È£ÃâÇØÁÖ¸é ¹Ì´Ï¸Ê Å¬¸¯ ÀÌµ¿ ½Ã ¼­¹ö¿Í µ¿±âÈ­ Â÷ÀÌ¿¡ ÀÇÇÑ
				// °­Á¦ ·Ñ¹é Çö»óÀº ¼öÁ¤ÇÒ ¼ö ÀÖÁö¸¸ ¼­¹ö¿¡ ºÎ´ãÀÌ µÈ´Ù
				LargeMapMoveTo( &m_sActionID.vPos, TRUE );
			}
		}
	}

	//// ¼±ÅÃ ´ë»ó ¾÷µ¥ÀÌÆ®
	//SelectTargetUpdate();

	/*if ( !IsActState(EM_ACT_CONTINUEMOVE) )
	{
		if (m_sLastMsgMove != m_sLastMsgMoveSend)
		{
			m_fLastMsgMoveSend = fTime;
			m_sLastMsgMoveSend = m_sLastMsgMove;
			m_pGaeaClient->NETSENDTOFIELD( &m_sLastMsgMoveSend );
		}
	}*/

	//	Note : Å¬·´¸¶Å© ¹öÀü È®ÀÎ.
	//
	m_pGaeaClient->ReqClubMarkInfo(m_pMyClub->DbNum(), m_pMyClub->m_dwMarkVER);

	//	Æ®·¹ÀÌµå »óÅÂ °»½Å.
	m_pGaeaClient->GetTradeClient()->FrameMove ( fTime, fElapsedTime );

	//	Note : Å¸°ÙÀÌ ¹«È¿È­ µÇ´ÂÁö Á¡°ËÇÑ´Ù.
	//
	ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( m_sActionID );
	if ( !pTarget )
	{
		m_sActionID.GaeaId = EMTARGET_NULL;
		if ( isAction(GLAT_ATTACK) )
			TurnAction ( GLAT_IDLE );
	}

	//	Note : Gate °Ë»ç.
	//
	DWORD dwDetectGate = DetectGate ();
	if ( dwDetectGate!=UINT_MAX )
	{
		m_pGaeaClient->SetGateOpen( TRUE );
		m_pGaeaClient->GetInterface()->SetGateOpen( FALSE );
	}

	//// Note : Çì¾î¸¦ Edit ÇØ¾ß ÇÒ °æ¿ì¸¸ º¯°æÇÑ´Ù.
	//if( m_bEnableHairSytle )
	//	HairStyleUpdate();
	//if( m_bEnableHairColor )	
	//	HairColorUpdate();
	//if( m_bEnableFaceStyle )
	//	FaceStyleUpdate();

	m_pWorkingSkill->FrameMove( fElapsedTime );

	if( m_pSkinCharMulti )
	{
		BOOL bUpdateSuit = FALSE;
		BOOL bIsCloneSkill = IsCheckedSkillFlagSpecial(EMSPECA_CLONE);
		bUpdateSuit = m_pSkinCharMulti->SetUseSkinCharMulti( bIsCloneSkill, m_sClone.GetClonNum());

		if ( m_pSkinCharMulti->IsUpdateSuit() && bUpdateSuit )
			UpdateSuit( TRUE, TRUE );

		if ( !bIsCloneSkill )
			m_sClone.Reset();
	}

	//UpdateAnimation ( fTime, fElapsedTime, GetAction() );
	UpdateAction (fTime, fElapsedTime );

	// [AnimationMove]
	SkinAniControl_AnimationMove();

	// [shhan][2015.01.27] RefreshPosition ¸¦ ¾ÈÇØÁÖ´Ï PUSHPULL µÉ ½Ã Ä³¸¯ÅÍ Çü»ó°ú ÀÌ¸§, Å¸ÄÏÆÃ µÇ´Â À§Ä¡°¡ µû·Î ºÐ¸®µÇ´Â ¹®Á¦°¡ »ý°å´Ù.
	//						m_vPosition °ÍÀ» ¾ø¾Ù °æ¿ì´Â PUSHPULL Test µµ °°ÀÌ ÇØºÁ¾ß ÇÒ°Í °°À½.
	ClientActor::RefreshPosition(0.f);

	//const D3DXVECTOR3& vPosition = ClientActor::GetPositionActor();	//RefreshPosition(0.0f);//GetPosition();

	FACTEFF::UpdateSkillEffect( m_pGaeaClient, STARGETID(CROW_PC,m_dwGaeaID,GetPositionActor()), m_pSkinChar, m_sSKILLFACT, SKILLFACT_SIZE, m_sSTATEBLOWS, TRUE );	// ½ºÅ³ ÀÌÆåÆ® ¾÷µ¥ÀÌÆ®.

	UpdateSkillEff( fTime, fElapsedTime );
	UpdateSpecialSkill( fElapsedTime );

	// ¿©±â¼­ DxSkinChar ÀÇ ÃÖÁ¾ FrameMove °¡ ÀÌ·ç¾î Áø´Ù.
	// ÀÌ ½ÃÁ¡ ºÎÅÍ ·»´õ¸µ ³¡³¯ ¶§ ±îÁö Part ¸¦ º¯°æÇÏ´Â ÀÛ¾÷ÀÌ ÀÌ·ç¾îÁö¸é ¹®Á¦°¡ ¹ß»ýÇÑ´Ù.
	UpdateAnimation ( fTime, fElapsedTime, GetAction() );

	//	?Item Áö¼Ó È­¸é È¿°ú. on/off
	if ( m_emOldQuestionType!=m_sQITEMFACT.emType )
	{
		m_emOldQuestionType = m_sQITEMFACT.emType;

		switch ( m_sQITEMFACT.emType )
		{
		case QUESTION_NONE:
			{
				//	?Item È­¸é È¿°ú.
				DxEffProjMan::GetInstance().EnableGetItemEFF ( FALSE );

				//	?Item BGM.
				if ( !pLand )		break;

				DxLandMan* pLandMan = pLand->GetLandMan();
				if ( !pLandMan )		break;

				const CString &strBgm = pLandMan->GetBgmFile();
				const char *szCurBgm = DxBgmSound::GetInstance().GetFile();
				if ( strBgm==szCurBgm )	break;

				DxBgmSound::GetInstance().ForceStop ();
				DxBgmSound::GetInstance().ClearFile ();

				DxBgmSound::GetInstance().SetFile ( strBgm );
				DxBgmSound::GetInstance().Play();
			}
			break;

		case QUESTION_SPEED_UP:
		case QUESTION_CRAZY:
		case QUESTION_ATTACK_UP:
		case QUESTION_EXP_UP:
		case QUESTION_LUCKY:
		case QUESTION_SPEED_UP_M:
		case QUESTION_MADNESS:
		case QUESTION_ATTACK_UP_M:
			{
				if ( RANPARAM::bBuff )
					DxEffProjMan::GetInstance().EnableGetItemEFF ( TRUE );

				const std::string &strBgm = GLCONST_CHAR::strQITEM_BGM[m_sQITEMFACT.emType];
				if ( strBgm.empty() || strBgm=="null" )		break;

				const char *szCurBgm = DxBgmSound::GetInstance().GetFile();
				if ( strBgm==szCurBgm )	break;

				DxBgmSound::GetInstance().ForceStop ();
				DxBgmSound::GetInstance().ClearFile ();

				DxBgmSound::GetInstance().SetFile ( strBgm.c_str() );
				DxBgmSound::GetInstance().Play();
			}
			break;
		};
	}

	//	if( m_sQITEMFACT.IsACTIVE() ) m_sQITEMFACT.fTime -= fElapsedTime;

	//	Note : ÇöÁ¦ Çàµ¿ »óÅÂ È®ÀÎ¿ë.
	//
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		if( m_pSkinChar )
		{	
			CDebugSet::ToView(5,CGameTextMan::GetInstance().GetCommentText("ANI_MAINTYPE", m_pSkinChar->GETCURMTYPE()).GetString());
			CDebugSet::ToView(5,CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE", m_pSkinChar->GETCURSTYPE()).GetString());
		} 
	}
	else
	{
		if( m_pSkinChar )
		{	
			CDebugSet::ToView ( 5, COMMENT::ANI_MAINTYPE[m_pSkinChar->GETCURMTYPE()].c_str() );
			CDebugSet::ToView ( 6, COMMENT::ANI_SUBTYPE[m_pSkinChar->GETCURSTYPE()].c_str() );
		} 
	}//if

	// ¾Æ·¡ÀÇ COMMENT´Â µ¥ÀÌÅÍ¿¡¼­ ÀÐ¾î¿ÀÁö¾Ê´Â´Ù;
	CDebugSet::ToView ( 8, COMMENT::ACTIONTYPE[GetAction()].c_str() );
	CDebugSet::ToView ( 9, "char pos : %4.1f, %4.1f, %4.1f", ClientActor::GetPositionActor().x, ClientActor::GetPositionActor().y, ClientActor::GetPositionActor().z );
	CDebugSet::ToView ( 10, "target pos : %4.1f, %4.1f, %4.1f", m_sActionID.vPos.x, m_sActionID.vPos.y, m_sActionID.vPos.z );

	// ÃßÀû À¯ÀúÀÏ °æ¿ì¿¡ Á¤º¸µéÀ» ³²±ä´Ù.
	/*if( m_bTracingUser )
	{
		PrintTracingUserWindowInfo();
		PrintTracingUserProcessInfo( fElapsedTime );
	}*/


	if ( m_pGaeaClient->IsCTFPlayer() )
	{
		m_dwActionLimit |= (EMACTION_LIMIT_PET | EMACTION_LIMIT_DRUG_HP | EMACTION_LIMIT_CARD);
	}



	//////////////////////////////////////////////////////////////////////////
	// Skin °ü·Ã À§Ä¡¸¦ »ç¿ëÇÑ´Ù. ( vPositionOfSkin ) ( airbone Àû¿ëµÈ °ª )
	//////////////////////////////////////////////////////////////////////////

	const D3DXVECTOR3& vPositionOfSkin = ClientActor::GetPositionActor() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f);

	m_vMax = vPositionOfSkin + m_vMaxOrg;
	m_vMin = vPositionOfSkin + m_vMinOrg;

	//////////////////////////////////////////////////////////////////////////
	//			ActorWorldLight À» Thread »ó¿¡¼­ È®ÀÎÇÏµµ·Ï À¯µµÇÑ´Ù.
	// Thread »óÀ¸·Î ³Ñ±ä´Ù.
	GLLandManClient* const pLandManClient = m_pGaeaClient->GetLandManClient();
	if ( pLandManClient )
	{
		m_spWorldLight->InitData( pLandManClient->GetLandManSp() );
		NSSkinAniThread::AddActorWorldLight( m_spWorldLight, vPositionOfSkin );
	}

	return S_OK;
}

HRESULT GLCharacter::FrameMoveForTool( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;	

	//	Note : ¿¡´Ï¸ÞÀÌ¼Ç, ½ºÅ² ¾÷µ¥ÀÌÆ®.
	//
//	UpdateAnimation ( fTime, fElapsedTime );


	////	Note : ½ºÅ³ ÀÌÆåÆ® ¾÷µ¥ÀÌÆ®.
	////

	//InitializeActorMove(D3DXVECTOR3( 0.0f, 0.0f, 0.0f ));
	//
	//const D3DXVECTOR3 vDirect = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	//ClientActor::SetDirect(vDirect);

	////	Note : ÇöÀç À§Ä¡¿Í ¹æÇâÀ¸·Î Transform ¸ÞÆ®¸¯½º °è»ê.
	////
	//D3DXMATRIX matTrans, matYRot;
	//D3DXMatrixTranslation ( &matTrans, 0.0f, 0.0f, 0.0f );
	//float fThetaY = DXGetThetaYFromDirection ( vDirect, GetDirectOrig() );
	//D3DXMatrixRotationY ( &matYRot, fThetaY );

	//SetTransMatrix(D3DXMATRIX(matYRot * matTrans));

	// [shhan][2014.09.22] EffectTool ¿¡¼­ ±âº» Ä³¸¯ÅÍ ¹æÇâÀ» º¯°æÇØ ´Þ¶ó°í ÇØ¼­ ÀÛ¾÷ÇÔ.
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	if ( !m_pSkinChar->ISENDANIM() )			
		m_pSkinChar->FrameMove ( fTime, fElapsedTime, FALSE, matIdentity );

	m_vMax = m_vMaxOrg;
	m_vMin = m_vMinOrg;
	return S_OK;
}

void GLCharacter::PrintTracingUserProcessInfo( float fElapsedTime )
{


		// 5ºÐ¸¶´Ù ÇÑ¹ø¾¿
	if( m_fPrintProcessInfoMsgGap == -10.0f || m_fPrintProcessInfoMsgGap > 600 )
	{
		m_fPrintProcessInfoMsgGap = 0.0f;

		// ÇÁ·Î¼¼½º ½º³À¼¦ ÇÚµéÀ» ¾ò´Â´Ù.
		HANDLE h_snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

		// ½º³À¼¦ µÇ¾úÀ»¶§ ÇÁ·Î¼¼½º ÁÖ¼Ò³»¿¡ ¿­°ÅµÈ ¸®½ºÆ®¸¦ ÀúÀåÇÏ´Â ±¸Á¶Ã¼
		PROCESSENTRY32 entry_data;
		entry_data.dwSize = sizeof(entry_data);

		// Ã¹¹øÂ° ÇÁ·Î¼¼½ºÀÇ Á¤º¸¸¦ ¾ò´Â´Ù.
		char continue_flag = Process32First(h_snap_shot, &entry_data);         
		while(continue_flag)
		{
			char temp[256] = {0, };

			char name[MAX_PATH];
			unsigned int id;
			unsigned int thread_counts;
			unsigned int parent_id;
			unsigned int priority;
			PROCESS_MEMORY_COUNTERS pmc;            

			// ÇÁ·Î¼¼½º ¾ÆÀÌµð¸¦ ÀÌ¿ëÇÏ¿© ÇØ´ç ÇÁ·Î¼¼½º ÇÚµé °ªÀ» ¾ò´Â´Ù.
			HANDLE hwnd = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,entry_data.th32ProcessID);

			if(hwnd != NULL){
				// Á¤»óÀûÀ¸·Î ÇÁ·Î¼¼½º¸¦ ¿­¾ú´Ù¸é..
				// ÇÁ·Î¼¼½ºÀÇ °æ·Î¸íÀ» ¾ò´Â´Ù.
				// Á¤»óÀûÀ¸·Î °æ·Î¸íÀ» ¾ò¾ú´Ù¸é ÇØ´ç °æ·Î¸íÀ» ±×·¸Áö ¸øÇß´Ù¸é ÆÄÀÏ¸í¸¸À» Ãâ·ÂÇÑ´Ù.
				if(GetModuleFileNameEx(hwnd, NULL, temp, 256)) 	strcpy_s(name,MAX_PATH,temp);
				else strcpy_s(name, MAX_PATH, entry_data.szExeFile);				
				GetProcessMemoryInfo( hwnd, &pmc, sizeof(pmc));
				CloseHandle(hwnd);
			} else {
				// Á¤»óÀûÀ¸·Î ÇÁ·Î¼¼½º¸¦ ¿­Áö ¸øÇß´Ù¸é..
				strcpy_s(name, MAX_PATH, entry_data.szExeFile);

			}
			id = entry_data.th32ProcessID;
			thread_counts = entry_data.cntThreads;
			parent_id = entry_data.th32ParentProcessID;
			priority = entry_data.pcPriClassBase;
			

			GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
			TracingMsg.nUserNum  = GetUserID();
			TracingMsg.SetAccount(m_szUID);
			CString strTemp;
			int memory = (int)(pmc.WorkingSetSize / 1024);
			strTemp.Format(
                "ProcessInfo, ProcessName[%s], Memory[%dKB], Thread count[%d]",
                name, memory, thread_counts);

			TracingMsg.SetLog(strTemp.GetString());
			m_pGaeaClient->NETSENDTOAGENT(&TracingMsg);

			// ´ÙÀ½ ÇÁ·Î¼¼½ºÀÇ Á¤º¸¸¦ ¾ò´Â´Ù.
			continue_flag = Process32Next(h_snap_shot, &entry_data);
		}

		CloseHandle(h_snap_shot);
	}
    else
    {
		m_fPrintProcessInfoMsgGap += fElapsedTime;
	}
}

void GLCharacter::PrintTracingUserWindowInfo()
{
	BYTE dwID[] = { INVENTORY_WINDOW, CHARACTER_WINDOW, SKILL_WINDOW, PARTY_WINDOW, QUEST_WINDOW, CLUB_WINDOW, FRIEND_WINDOW, MAP_WINDOW };

	if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
		dwID[ 0 ] = INVENTORY_WINDOW_RENEW;

	for( BYTE i = 0; i < 8; i++ )
	{
		//!! SF_TODO
		//m_bNewVisibleTracingUI[i] = (0!=m_pInterface->UiIsVisibleGroup( (UIGUID)dwID[i] ));
		if( m_bOldVisibleTracingUI[i] != m_bNewVisibleTracingUI[i] )
		{
			m_bOldVisibleTracingUI[i] = m_bNewVisibleTracingUI[i];
			if( m_bNewVisibleTracingUI[i] == TRUE )
			{
				GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
				TracingMsg.nUserNum  = GetUserID();
				TracingMsg.SetAccount(m_szUID);
				CString strTemp;

				switch (i)
				{
				case 0:
					strTemp.Format( "Open Inventory Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 1:
					strTemp.Format( "Open Character Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 2:
					strTemp.Format( "Open Skill Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 3:
					strTemp.Format( "Open Party Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 4:
					strTemp.Format( "Open Quest Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 5:
					strTemp.Format( "Open Club Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 6:
					strTemp.Format( "Open Friend Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 7:
					strTemp.Format( "Open Mini Map, [%d][%s]", GetUserID(), m_szName );
					break;
				}
				TracingMsg.SetLog(strTemp.GetString());

				m_pGaeaClient->NETSENDTOAGENT(&TracingMsg);
			}
		}
	}
}

float GLCharacter::GetDirection ()
{
	return DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
}

void GLCharacter::CalculateMatrix( const D3DXVECTOR3& vPosition )
{
	//	Note : ÇöÀç À§Ä¡¿Í ¹æÇâÀ¸·Î Transform ¸ÞÆ®¸¯½º °è»ê.
	//
	std::tr1::shared_ptr<GLCharClient> pCharClient;
	D3DXMATRIX matTrans, matYRot, matPassenger;
	D3DXVECTOR3 vPos;
	DxVehicle* pSkinChar = NULL;	
	if( m_emVehicle )
	{
		for( WORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
		{
			if (m_sVehicle.PassengerId(i) == m_dwGaeaID)
			{
				if( i == 0 )
				{
					pSkinChar = m_sVehicle.m_pSkinChar;
				}
				else
				{
					pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(0) );
					GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
					if( pCharClient )
					{
						vPos = pCharClient->GetPosition();
						ClientActor::InitializeActorMove( vPos );
					}
				}
				break;
			}
		}

		if( pSkinChar )
		{
			D3DXVECTOR3 vFrontHighPos, vFrontLowPos, vBackHighPos, vBackLowPos;
			D3DXVECTOR3 vAnswerFrontPos, vAnswerBackPos;
			DWORD dwCollID;
			BOOL bFrontColl, bBackColl;
			D3DXVECTOR3 vAngle;
			const D3DXVECTOR3& vDirect = GetDirect();
			const D3DXVECTOR3& vDirectOrig = GetDirectOrig();

			vFrontHighPos = vFrontLowPos = vPosition + ( vDirect * pSkinChar->m_vMax.x );
			vFrontHighPos.y += 10.0f;
			vFrontLowPos.y -= 10.0f;

			vBackHighPos = vBackLowPos = vPosition + ( - vDirect * pSkinChar->m_vMax.x );
			vBackHighPos.y += 10.0f;
			vBackLowPos.y -= 10.0f;

			ClientActor::IsCollision(vFrontHighPos, vFrontLowPos, vAnswerFrontPos, dwCollID, bFrontColl);
			ClientActor::IsCollision(vBackHighPos, vBackLowPos, vAnswerBackPos, dwCollID, bBackColl);			

			PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
			DxLandMan* pLandMan(NULL);
			if ( pLand )
			{
				pLandMan = pLand->GetLandMan();
			}

			D3DXMATRIX matTrans;
			if ( pLandMan && pSkinChar->GetFootColl()->GetMatrix( matTrans, pLandMan, vPosition, GetDirect(), GetDirectOrig(), const_cast< NavigationMesh* >( GetParentMesh() ) ) )
			{
				m_matBikeTrans = matTrans;
				m_matBikeTrans._41 = vPosition.x;
				m_matBikeTrans._42 = vPosition.y;
				m_matBikeTrans._43 = vPosition.z;
			}
			else
			{
				if( bFrontColl && bBackColl )
				{
					vAngle = vAnswerFrontPos - vAnswerBackPos;				
					D3DXVec3Normalize( &vAngle, &vAngle );				

					FLOAT fTheta = (vAngle.y * D3DX_PI / 3.0f );
					D3DXMatrixRotationX( &m_matBikeTrans, fTheta );

					D3DXMATRIX matRot;
					fTheta = DXGetThetaYFromDirection( vDirect, vDirectOrig );
					D3DXMatrixRotationY( &matRot, fTheta );
					D3DXMatrixMultiply( &m_matBikeTrans, &m_matBikeTrans, &matRot );

					m_matBikeTrans._41 = vPosition.x;
					m_matBikeTrans._42 = vPosition.y;
					m_matBikeTrans._43 = vPosition.z;
				}
				else
				{				
					D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
					const float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
					D3DXMatrixRotationY ( &matYRot, fThetaY );

					m_matBikeTrans = matYRot * matTrans;
				}
			}
		}
		else
		{
			const D3DXVECTOR3& vDirect = GetDirect();
			const D3DXVECTOR3& vDirectOrig = GetDirectOrig();
			D3DXMATRIX matTrans, matYRot;
			D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
			float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
			D3DXMatrixRotationY ( &matYRot, fThetaY );
			SetTransMatrix(D3DXMATRIX(matYRot * matTrans));
		}
	}
	else
	{
		const D3DXVECTOR3& vDirect = GetDirect();
		const D3DXVECTOR3& vDirectOrig = GetDirectOrig();
		D3DXMATRIX matTrans, matYRot;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
		D3DXMatrixRotationY ( &matYRot, fThetaY );
		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));
	}

	//std::tr1::shared_ptr<GLCharClient> pCharClient;
	//const SPassengerData* pPassengerData = NULL;
	//D3DXMATRIX matYRot, matPassenger;
	//SITEM* pItem = GET_SLOT_ITEMDATA ( SLOT_VEHICLE );

	//if( pItem )
	//{
	//	if( m_emVehicle == EMVEHICLE_DRIVER && pItem->sVehicle.emVehicleType != VEHICLE_TYPE_BOARD )
	//	{
	//		// VehicleÀÇ ¿îÀüÀÚ°¡ ÇöÀç ÀÚ½ÅÀÎÁö È®ÀÎÇÑ´Ù.
	//		if ( m_dwGaeaID != m_sVehicle.PassengerId(0) )
	//			return;

	//		// Skin ÀÌ ¸¸µé¾îÁ³´ÂÁö È®ÀÎÇÑ´Ù.
	//		DxVehicle* pVehicle = m_sVehicle.m_pSkinChar;
	//		if ( !pVehicle )
	//			return;

	//		// PassengerMatrix Data °¡ Á¤»óÀÎÁö ¾Æ´ÑÁö È®ÀÎÇÑ´Ù.
	//		if ( !pVehicle->IsValidPassengerMatrixData() )
	//			return;

	//		//pVehicle->UpdatePassengerPos();

	//		for( DWORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
	//		{
	//			// PassengerData °¡ Á¤»óÀÎÁö È®ÀÎÇÑ´Ù.
	//			pPassengerData = pVehicle->GetPassengerData(i);
	//			if( !pPassengerData )
	//				continue;

	//			if( i == 0 )	// ¿îÀüÀÚ, ÀÚ±â ÀÚ½Å
	//			{
	//				SetVehiclePassengerID( i );
	//				SetTransMatrix(pPassengerData->matWorld);
	//			}
	//			else
	//			{
	//				pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(i) );
	//				if( pCharClient )
	//				{
	//					pCharClient->SetVehiclePassengerID( i );
	//					pCharClient->SetTransMatrix( pPassengerData->matWorld );
	//				}
	//			}
	//		}
	//	}
	//	else if( m_emVehicle == EMVEHICLE_PASSENGER )
	//	{
	//		// Å¾½ÂÀÚ ÀÔÀåÀÌ¶ó¸é, À§Ä¡ÁÂÇ¥´Â ±× Vehicle ÀÇ ¿îÀüÀÚÂÊ¿¡¼­ °è»êÀÌ µÇµµ·Ï ÇÑ´Ù.
	//		// ¿©±â¿¡¼­´Â Vehicle Á¤º¸°¡ ¾ø±â ¶§¹®ÀÌ´Ù.
	//		return;
	//	}
	//	else
	//	{
	//		const D3DXVECTOR3& vPosition = GetPosition();
	//		D3DXMATRIX matTrans = GetTransMatrix();
	//		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
	//		float fThetaY = DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
	//		D3DXMatrixRotationY ( &matYRot, fThetaY );

	//		const D3DXMATRIX matRotTrans = matYRot * matTrans;
	//		SetTransMatrix(matRotTrans);
	//	}

	//}
	//else
	//{
	//	if( m_emVehicle )
	//		return;

	//	const D3DXVECTOR3& vPosition = GetPosition();
	//	D3DXMATRIX matTrans = GetTransMatrix();
	//	D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
	//	float fThetaY = DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
	//	D3DXMatrixRotationY ( &matYRot, fThetaY );

	//	const D3DXMATRIX matRotTrans = matYRot * matTrans;
	//	SetTransMatrix(matRotTrans);
	//}
}

// Vehicle. ³»°¡ ¾î´À À§Ä¡¿¡ ¾É¾Æ ÀÖ´Â°¡~?
void GLCharacter::SetVehiclePassengerID( DWORD dwID )
{
	m_dwVehiclePassengerID = dwID;
}

DWORD GLCharacter::GetVehiclePassengerID() const
{
	return m_dwVehiclePassengerID;
}

void GLCharacter::SkinAniControl_AnimationMove()
{
	if( m_bFirstAnimationMove )
	{
		if( !m_pSkinChar->IsAnimationMove() )
		{
			m_bFirstAnimationMove = FALSE;

			// ¼­¹ö ÂÊ¿¡ ³»À§Ä¡ ¿Å°ÜÁÖ±â.
			GLMSG::SNETPC_ANIMOVE_END_CF NetMsg;
			NetMsg.m_vCurPos = GetPositionActor();
			m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
		}
	}
	else
	{
		if( m_pSkinChar->IsAnimationMove() )
		{
			m_bFirstAnimationMove = TRUE;

			const DxSkinAnimation* pSkinAnimation = m_pSkinChar->GETCURANIM();
			if ( pSkinAnimation )
			{
				// ¼­¹ö ÂÊ¿¡ ³»À§Ä¡ ¿Å°ÜÁÖ±â.
				GLMSG::SNETPC_ANIMOVE_START_CF NetMsg;
				NetMsg.m_fAngleY = DXGetThetaYFromDirection( GetDirect(), GetDirectOrig() );
				m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
			}
		}
	}

	// ¾Ö´Ï¸ÞÀÌ¼Ç¿¡¼­ Ä³¸¯ÅÍ¸¦ ÀÌµ¿½ÃÄÑÁáÀ» ¼öµµ ÀÖÀ¸´Ï È®ÀÎÇÏÀÚ.
	if( m_pSkinChar->IsAnimationMove() )
	{
		D3DXVECTOR3 vPos;
		D3DXMATRIX matRotate( GetTransMatrix() );
		matRotate._41 = 0.f;
		matRotate._42 = 0.f;
		matRotate._43 = 0.f;
		D3DXVec3TransformCoord( &vPos, &m_pSkinChar->GetAnimationMovePos(), &matRotate );

		////  [8/4/2013 gbgim];
		//// - ¾Ö´Ï¸ÞÀÌ¼Ç ¹«ºêÀÏ °æ¿ì ÆÛ¼¾Å×ÀÌÁö ÀÌµ¿ ¿É¼ÇÀÌ ÀÖ´Ù¸é;
		////   Ãß°¡ ÆÛ¼¾Å×ÀÌÁö ¸¸Å­ ÀÌµ¿½ÃÅ²´Ù;
		//GLSkillMain* pSkillMain = m_pSkillPlayer->GetSkillMain();
		//if ( pSkillMain != NULL && IsACTION( GLAT_SKILL ) )
		//{
		//	if ( pSkillMain->IsAnimMoveDistRatioControl() == TRUE )
		//	{
		//		vPos *= pSkillMain->GetAnimMoveDistRatio();
		//	}
		//}

		vPos += GetPositionActor();
		SetPositionActor( vPos );
	}
}

void GLCharacter::UpdateCharPos_RenderChar( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE )
{
	// º¸µåÀÏ °æ¿ì m_sVehicle.m_pSkinChar °¡ NULL ÀÏ ¼öµµ ÀÖ´Ù.
	if( m_emVehicle==EMVEHICLE_DRIVER && m_sVehicle.m_pSkinChar )
	{
		RenderMyChar( pd3dDevice, m_sVehicle.m_pSkinChar, TRUE, bHALF_VISIBLE );
	}

	DxSkinChar* pRenderChar = NULL;
	std::vector<DxSkinChar*> vecRenderCharTest;	
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform )) == NULL )
	{
		pRenderChar = m_pSkinChar;
	}

	RenderMyChar( pd3dDevice, pRenderChar, TRUE, bHALF_VISIBLE );
	
}

void GLCharacter::UpdateCharPos_RenderChar_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE )
{
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// º¸µåÀÏ °æ¿ì m_sVehicle.m_pSkinChar °¡ NULL ÀÏ ¼öµµ ÀÖ´Ù.
	if( m_emVehicle==EMVEHICLE_DRIVER && m_sVehicle.m_pSkinChar )
	{
		RenderMyChar( pd3dDevice, m_sVehicle.m_pSkinChar, FALSE, bHALF_VISIBLE );
	}

	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	RenderMyChar( pd3dDevice, pRenderChar, FALSE, bHALF_VISIBLE );

}

void GLCharacter::UpdateCharPos_RenderChar_MaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE )
{
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// º¸µåÀÏ °æ¿ì m_sVehicle.m_pSkinChar °¡ NULL ÀÏ ¼öµµ ÀÖ´Ù.
	if( m_emVehicle==EMVEHICLE_DRIVER && m_sVehicle.m_pSkinChar )
	{
		RenderMyCharMaterialEffect( pd3dDevice, m_sVehicle.m_pSkinChar, bHALF_VISIBLE );
	}

	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	RenderMyCharMaterialEffect( pd3dDevice, pRenderChar, bHALF_VISIBLE );
}

void GLCharacter::RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharBase* pRenderChar, BOOL bOpaqueState, BOOL bHALF_VISIBLE )
{
	if ( !pRenderChar )
		return;

	// Effect Åø¿¡¼­´Â °Ë»çÇÏÁö ¾Ê´Â´Ù.
	if ( !g_bEFFECT_TOOL )
	{
		if ( m_spWorldLight->IsNotCalculateColor() )
			return;
	}

	NSMaterialSkinManager::SetMob( FALSE );
	pRenderChar->SetWorldLight( m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult );

	if ( bHALF_VISIBLE )	// Ä³¸¯ÅÍ ¹ÝÅõ¸í ·»´õ
	{
		if ( bOpaqueState )
		{
			// ¾Æ¹«°Íµµ ÇÏÁö ¾Ê´Â´Ù.
		}
		else
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
				pRenderChar->RenderFixed( pd3dDevice, TRUE, 0.5f );
			}
			else
			{
				pRenderChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_MY  ? TRUE : FALSE );
				pRenderChar->RenderMaterialOpaque ( pd3dDevice, 0.5f  );
				pRenderChar->RenderMaterialHardAlpha ( pd3dDevice, 0.5f );
				pRenderChar->RenderMaterialSoftAlpha ( pd3dDevice, 0.5f );
			}
		}		
	}
	else		// Á¤»ó·»´õ
	{
		if ( bOpaqueState )
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
				pRenderChar->RenderFixed( pd3dDevice, TRUE, 1.f  );
			}
			else
			{
				pRenderChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_MY  ? TRUE : FALSE );
				pRenderChar->RenderMaterialOpaque ( pd3dDevice, 1.f  );
				pRenderChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
			}
		}
		else
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
			}
			else
			{
				pRenderChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
			}
		}
	}

	if ( m_pSkinCharMulti && m_emVehicle!=EMVEHICLE_DRIVER && !m_sVehicle.m_pSkinChar  )
		m_pSkinCharMulti->RenderMyChar( pd3dDevice, TRUE, bHALF_VISIBLE, m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult, TRUE );
}

void GLCharacter::RenderMyCharMaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharBase* pRenderChar, BOOL bHALF_VISIBLE )
{	
	if ( !pRenderChar )
		return;

	NSMaterialSkinManager::SetMob( FALSE );

	if ( bHALF_VISIBLE )	// Ä³¸¯ÅÍ ¹ÝÅõ¸í ·»´õ
	{
		pRenderChar->RenderMaterialEffect ( pd3dDevice, 0.5f );
	}
	else		// Á¤»ó·»´õ
	{
		pRenderChar->RenderMaterialEffect ( pd3dDevice, 1.f );
	}
}

HRESULT GLCharacter::Render ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	if ( IsActState(EM_REQ_VISIBLENONE) )
        return S_FALSE;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	DxSkinChar* pRenderChar = NULL;
	std::vector<DxSkinChar*> vecRenderChar;
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
	{
		pRenderChar = m_pSkinChar;
	}

	//	Note : m_bSkillFacts[EMSPECA_INVISIBLE]ÀÌ ÂüÀÏ °æ¿ì ¹ÝÅõ¸íÇÏ°Ô Ç¥ÇöÇØ¾ßÇÔ.
	//		
	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) || IsActState(EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE);
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		if ( bHALF_VISIBLE )
		{
			PutPassiveEffect ( pRenderChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &GetDirect() );
		}
		else
		{
			OutEffect ( pRenderChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
		}
	}

	if( RENDERPARAM::bClickEffect )
	{
		STARGETID sTargetID(CROW_PC,m_CharDbNum,m_sActionID.vPos);
		
		if(isAction(GLAT_MOVE) && m_emVehicle != EMVEHICLE_PASSENGER )
		{
			D3DXMATRIX matEffect;
			D3DXMatrixTranslation ( &matEffect, m_sActionID.vPos.x, m_sActionID.vPos.y, m_sActionID.vPos.z );

			DxEffGameHelp::GetInstance().UpdateGameHelpEffect( EMCLICK_EFFECT, matEffect, sTargetID );
		}
		else
		{
			DxEffGameHelp::GetInstance().ResetClickEffect();
			DxEffGameHelp::GetInstance().BlindEff( EMCLICK_EFFECT );
		}
	}

	//// WorldEdit ¿¡¼­ ¾²ÀÌ´Â DxLandMan °ªµµ »ç¿ëµÇ¾î¾ß ÇØ¼­ ÂüÁ¶°ªÀ¸·Î ¹Þ¾Æ¼­ »ç¿ëÇÏ°Ô µÊ.
	//if ( pLandMan )
	//{
	//	CalculateAmbientColor( pd3dDevice, pLandMan, GetPosition() );
	//}

	//if ( m_sVehicle.m_pSkinChar && m_emVehicle == EMVEHICLE_DRIVER )
	//{
	//	//if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	//	//{
	//	//	m_sVehicle.m_pSkinChar->RenderFixed( pd3dDevice, TRUE, 1.f, m_matBikeTrans );
	//	//}
	//	//else
	//	//{
	//	//	m_sVehicle.m_pSkinChar->RenderMaterialOpaque( pd3dDevice, 1.f, m_matBikeTrans );
	//	//	m_sVehicle.m_pSkinChar->RenderMaterialHardAlpha( pd3dDevice, 1.f );
	//	//}
	//	//
	//	//m_sVehicle.m_pSkinChar->UpdatePassengerPos();

	//	if ( pLandMan )
	//	{
	//		m_sVehicle.m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(m_matBikeTrans._41,m_matBikeTrans._42,m_matBikeTrans._43) );
	//	}
	//}

    //
 	UpdateCharPos_RenderChar( pd3dDevice, bHALF_VISIBLE );

	if( dxincommand::bDISP_POSITION )
	{
		EDITMESHS::RENDERSPHERE(
			pd3dDevice,
			m_vServerPos,
			D3DXVECTOR3( 2.0f, 2.0f, 2.0f ),
			TRUE,
			NULL,
			D3DCOLOR_ARGB( 255, 0, 255, 0 ) );
		
		EDITMESHS::RENDERSPHERE(
			pd3dDevice,
			ClientActor::GetPositionActor(),
			D3DXVECTOR3( 3.0f, 3.0f, 3.0f ),
			FALSE,
			NULL,
			D3DCOLOR_ARGB( 255, 255, 0, 0 ) );
	}

	//D3DXVECTOR3 vMax(m_vPos.x+MAX_VIEWRANGE, m_vPos.y+20, m_vPos.z+MAX_VIEWRANGE);
	//D3DXVECTOR3 vMin(m_vPos.x-MAX_VIEWRANGE, m_vPos.y-20, m_vPos.z-MAX_VIEWRANGE);
	//EDITMESHS::RENDERAABB ( pd3dDevice,  vMax, vMin );

	return S_OK;
}

HRESULT GLCharacter::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( IsActState(EM_REQ_VISIBLENONE) )
		return S_FALSE;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	//	Note : m_bSkillFacts[EMSPECA_INVISIBLE]ÀÌ ÂüÀÏ °æ¿ì ¹ÝÅõ¸íÇÏ°Ô Ç¥ÇöÇØ¾ßÇÔ.
	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) || IsActState(EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE);

	UpdateCharPos_RenderChar_SoftAlpha( pd3dDevice, bHALF_VISIBLE );

	return S_OK;
}

HRESULT GLCharacter::RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		return S_OK;

	if ( IsActState(EM_REQ_VISIBLENONE) )
		return S_FALSE;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	//	Note : m_bSkillFacts[EMSPECA_INVISIBLE]ÀÌ ÂüÀÏ °æ¿ì ¹ÝÅõ¸íÇÏ°Ô Ç¥ÇöÇØ¾ßÇÔ.
	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) || IsActState(EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE);

	UpdateCharPos_RenderChar_MaterialEffect( pd3dDevice, bHALF_VISIBLE );

	return S_OK;
}

HRESULT GLCharacter::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWorldSystemOLD )
{
	if ( IsActState(EM_REQ_VISIBLENONE) )
        return S_FALSE;

    if ( IsActState(EM_REQ_VISIBLEOFF) )
        return S_FALSE;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	//	Note : ±×¸²ÀÚ ·£´õ¸µ.
	//
	if ( m_sVehicle.m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob( m_sVehicle.m_pSkinChar, m_matBikeTrans, pd3dDevice, bWorldSystemOLD );
		//m_sVehicle.m_pSkinChar->RenderShadow( pd3dDevice, m_matBikeTrans, TRUE );
	}

	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;		

	if ( pRenderChar )
	{
		//if ( m_emVehicle == EMVEHICLE_PASSENGER )
		//{
		//	const WORD i = m_sVehicle.GetPassengerIndex( m_dwGaeaID );
		//	std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(0) );			
		//	if ( pCharClient )
		//		DxShadowMap::GetInstance().RenderShadowCharMob ( pRenderChar, pCharClient->VehicleSkinChar()->m_sPassengerData[i].matWorld, pd3dDevice, bUsedMaterialSystem );
		//}
		//else
		//{
			DxShadowMap::GetInstance().RenderShadowCharMob ( pRenderChar, GetTransMatrix(), pd3dDevice, bWorldSystemOLD );
		//}
	}

    return S_OK;
}

HRESULT GLCharacter::RenderShadow_SpotLight ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem, DWORD dwSlot )
{
	if ( IsActState(EM_REQ_VISIBLENONE) )
		return S_FALSE;

	if ( IsActState(EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE) )
		return S_FALSE;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	//	Note : ±×¸²ÀÚ ·£´õ¸µ.
	//
	if ( m_sVehicle.m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight( m_sVehicle.m_pSkinChar, m_matBikeTrans, pd3dDevice, bUsedMaterialSystem, dwSlot );
	}

	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;		

	if ( pRenderChar )
		DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight( pRenderChar, GetTransMatrix(), pd3dDevice, bUsedMaterialSystem, dwSlot );

	return S_OK;
}

HRESULT GLCharacter::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice, float fWaterHeight )
{
	if ( IsActState(EM_REQ_VISIBLENONE) )
        return S_FALSE;

    if ( IsActState(EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE) )
        return S_FALSE;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	//	Note : ¹Ý»ç ·£´õ¸µ.
	//
	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;		

	if ( pRenderChar )	
	{
		DxEnvironment::GetInstance().RenderRefelctChar( pRenderChar, pd3dDevice, fWaterHeight );
	}

	if ( m_sVehicle.m_pSkinChar )
	{
		DxEnvironment::GetInstance().RenderRefelctChar( m_sVehicle.m_pSkinChar, pd3dDevice, fWaterHeight );
	}


	return S_OK;
}

HRESULT GLCharacter::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->InitDeviceObjects( pd3dDevice );
	}

	if ( m_pTransformSet )
	{
		m_pTransformSet->InitDeviceObjects( pd3dDevice );
	}

	if ( m_sVehicle.m_pSkinChar )
	{
		m_sVehicle.m_pSkinChar->InitDeviceObjects( pd3dDevice );
	}

	//if ( m_pSkinCharMulti )
	//	m_pSkinCharMulti->InitDeviceObjects( pd3dDevice );

	return S_OK;
}

HRESULT GLCharacter::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}

	if ( m_pTransformSet )
	{
		m_pTransformSet->RestoreDeviceObjects ( pd3dDevice );
	}

	if( m_sVehicle.m_pSkinChar )
	{
		m_sVehicle.m_pSkinChar->RestoreDeviceObjects ( m_pd3dDevice );
	}

	//if ( m_pSkinCharMulti )
	//	m_pSkinCharMulti->RestoreDeviceObjects( pd3dDevice );

	return S_OK;
}

HRESULT GLCharacter::InvalidateDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->InvalidateDeviceObjects ();
	}

	if ( m_pTransformSet )
	{
		m_pTransformSet->InvalidateDeviceObjects ();
	}

	if( m_sVehicle.m_pSkinChar )
	{
		m_sVehicle.m_pSkinChar->InvalidateDeviceObjects();
	}

	//if ( m_pSkinCharMulti )
	//	m_pSkinCharMulti->InvalidateDeviceObjects();

	return S_OK;
}

HRESULT GLCharacter::DeleteDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->DeleteDeviceObjects ();
	}

	if ( m_pTransformSet )
	{
		m_pTransformSet->DeleteDeviceObjects ();
	}

	if ( m_sVehicle.m_pSkinChar )
	{
		m_sVehicle.m_pSkinChar->DeleteDeviceObjects();
	}

	//if ( m_pSkinCharMulti )
	//	m_pSkinCharMulti->DeleteDeviceObjects();

	return S_OK;
}

EMELEMENT GLCharacter::GET_ITEM_ELMT ()
{
	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM* pRHandItem = GET_SLOT_ITEMDATA(emRHand);
	SITEM* pLHandItem = GET_SLOT_ITEMDATA(emLHand);

	EMELEMENT emELEMENT(EMELEMENT_SPIRIT);
	if ( pRHandItem && pRHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		emELEMENT = STATE_TO_ELEMENT ( pRHandItem->sSuitOp.sBLOW.emTYPE );
	}
	else if ( pLHandItem && pLHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		emELEMENT = STATE_TO_ELEMENT ( pLHandItem->sSuitOp.sBLOW.emTYPE );
	}

	return emELEMENT;
}

void GLCharacter::StartAttackProc()
{
	PLANDMANCLIENT		pLAND = m_pGaeaClient->GetActiveMap();
	if( pLAND && pLAND->IsPeaceZone() )
		return;

	if( IsActState(EM_ACT_PEACEMODE) )
		ReqTogglePeaceMode();

	if( m_pSkinChar )
		m_pSkinChar->ResetStrikeAttachBone();

	m_nattSTEP		= 0;
	m_fattTIMER		= 0.0f;

	EMSLOT			emRHand			= GetCurRHand();
	EMSLOT			emLHand			= GetCurLHand();
	EMANI_SUBTYPE	emANISUBTYPE	= CHECK_ATTACK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand], GETCHARINDEX() );
	DWORD			dwAMOUNT		= m_pSkinChar->GETANIAMOUNT ( AN_ATTACK, emANISUBTYPE );

	m_dwANISUBSELECT = m_dwANISUBCOUNT++;
	if( m_dwANISUBCOUNT >= dwAMOUNT )
	{
		m_dwANISUBCOUNT = 0;
	}

	//	Note : °ø°Ý ¸Þ½ÃÁö ¹ß»ý.
	GLMSG::SNETPC_ATTACK NetMsg;
	NetMsg.emTarCrow	= m_sActionID.emCrow;
	NetMsg.dwTarID		= m_sActionID.GaeaId;
	NetMsg.dwAniSel		= m_dwANISUBSELECT;
	NetMsg.dwFlags		= NULL;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_bAttackable = FALSE;
}

void GLCharacter::AttackEffect ( const SANIMSTRIKE &sStrikeEff )
{
	BOOL bOk = m_pGaeaClient->ValidCheckTarget ( m_sActionID );
	if ( !bOk )	return;

	ClientActor* pActor = m_pGaeaClient->GetCopyActor ( m_sActionID );
	if ( pActor )	pActor->ReceiveSwing ();

	//	Å¸°ÙÀÇ À§Ä¡.
	D3DXVECTOR3 vTARPOS = m_pGaeaClient->GetTargetPos ( m_sActionID );
	const D3DXVECTOR3& vPosition = GetPosition();

	D3DXVECTOR3 vDir = vTARPOS - vPosition;
	vDir.y = 0.f;
	D3DXVec3Normalize ( &vDir, &vDir );

	vTARPOS.y += 15.0f;

	D3DXVECTOR3 vTarDir = vTARPOS - vPosition;
	float fLength = D3DXVec3Length ( &vTarDir );

	D3DXVec3Normalize ( &vTarDir, &vTarDir );
	D3DXVECTOR3 vTARPAR = vPosition + vTarDir*fLength * 10.0f;

	//	Å¸°Ý À§Ä¡¸¦ ¾Ë¾Æ³¿.
	STRIKE::SSTRIKE sStrike;

	bool bAttBone = false;
	if ( m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
	{
		bAttBone = !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->m_strAttBoneName.empty();
	}

	if ( bAttBone )
	{		
		if ( !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
			return;

		PDXATTBONE pDxAttBone = m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->GetAttBone();
		if ( !pDxAttBone ) 
			return;

		STRIKE::CSELECTOR Selector ( vTARPOS, pDxAttBone->m_AttBoneArray );
		bOk = Selector.SELECT ( (EMPIECECHAR)ATTBONE_BODY, sStrike );
		if ( !bOk )	return;
	}
	else
	{
		STRIKE::CSELECTOR Selector ( vTARPAR, m_pSkinChar->m_PartArray );
		bOk = Selector.SELECT ( sStrikeEff.m_emPiece, sStrike );
		if ( !bOk )	return;
	}


	EMSLOT emSlot = PIECE_2_SLOT(sStrikeEff.m_emPiece);
	if( emSlot==SLOT_TSIZE )		return;

	//	¿À¸¥¼Õ ¹«±â¸¸ Àû¿ëµÇ°Ô ÇÑ´Ù. µà¾ó CPS ¶§¹®ÀÌ´Ù. 
	//	LHAND µµ±¸ ¼³Á¤Çß´Âµ¥ ¹«±â´Â ¿À¸¥¼Õ¿¡¸¸ ÀÖÀ»°æ¿ì ¶§¹®ÀÌ´Ù.
//	if( emSlot==SLOT_RHAND )		emSlot = GetCurRHand();
//	else if( emSlot==SLOT_LHAND )	emSlot = GetCurLHand();	
	emSlot = GetCurRHand();
    
	D3DXMATRIX matEffect;
	D3DXMatrixTranslation ( &matEffect, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );

	STARGETID sTargetID = m_sActionID;
	sTargetID.vPos = vTARPOS;

	//	Note : ¹«±âÀÇ ¼Ó¼º »ìÆìº½.
	//
	EMELEMENT emELEMENT = GET_ITEM_ELMT ();

	if ( VALID_SLOT_ITEM(emSlot) )
	{
		SITEM *pItem = GLogicData::GetInstance().GetItem(GET_SLOT_ITEM(emSlot).GetNativeID());
		if ( pItem )
		{
			if ( !ISLONGRANGE_ARMS( pItem->sSuitOp.emAttack ) )
			{
				std::string strEFFECT = pItem->GetTargetEffect();
				if ( strEFFECT.empty() )	strEFFECT = GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT);

				DxEffGroupPlayer::GetInstance().NewEffGroup
				(
					strEFFECT.c_str(),
					matEffect,
					&sTargetID,
					TRUE,
					TRUE
				);

				DxEffGroupPlayer::GetInstance().NewEffBody ( GLCONST_CHAR::strAMBIENT_EFFECT.c_str(), &sTargetID, &vDir );
			}
			else
			{
				std::string strEFFECT = pItem->GetTargetEffect();
				if ( strEFFECT.empty() )
				{
					const GLITEM_ATT emItemAtt = CONVERT_ITEMATT( pItem->sSuitOp.emAttack );
					switch ( emItemAtt )
					{
					case ITEMATT_BOW:
						strEFFECT = GLCONST_CHAR::strSTRIKE_ARROW_EFFECT;
						break;

					case ITEMATT_PISTOL:
					case ITEMATT_RAILGUN:
					case ITEMATT_PORTALGUN:
						strEFFECT = GLCONST_CHAR::strSTRIKE_BULLET_EFFECT;
						break;
					}
				}

				if ( false == strEFFECT.empty() )
				{
					SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup(
						strEFFECT.c_str(),
						matEffect,
						&sTargetID,
						TRUE,
						TRUE );

					if ( spEffSingleG.get() )
					{
						spEffSingleG->AddEffAfter( EFFASINGLE, GLCONST_CHAR::GETSTRIKE_EFFECT( emELEMENT ) );
						spEffSingleG->AddEffAfter( EFFABODY, std::string( pItem->GetTargBodyEffect() ) );
						spEffSingleG->AddEffAfter( EFFABODY, GLCONST_CHAR::strAMBIENT_EFFECT );
					}
					else
					{
						DxEffSinglePropGMan::GetInstance().LoadEffectGProp(
							GLCONST_CHAR::GETSTRIKE_EFFECT( emELEMENT ),
							TRUE,
							TRUE );
					}
				}
			}
		}
	}
	else
	{
		//	º¸ÀÌÁö ¾Ê´Â Å¸°¹ÀÏ °æ¿ì Å¸°Ý ÀÌÆÑÆ®´Â »ý·«µÊ.
		DxEffGroupPlayer::GetInstance().NewEffGroup
		(
			GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT),
			matEffect,
			&sTargetID,
			TRUE, 
			TRUE
		);

		DxEffGroupPlayer::GetInstance().NewEffBody ( GLCONST_CHAR::strAMBIENT_EFFECT.c_str(), &sTargetID, &vDir );
	}
}

BOOL GLCharacter::AttackProc ( float fElapsedTime )
{
	const float fAccumElapsed = (GLCHARLOGIC::GETATTVELO() + GLCHARLOGIC::GETATT_ITEM()) * fElapsedTime * UNITANIKEY_PERSEC;
	m_fattTIMER += fAccumElapsed;
	//GASSERT(m_pSkinChar->GETCURANIMNODE());

	//PANIMCONTNODE pAnicontNode = m_pSkinChar->GETCURANIMNODE();
	//GASSERT(pAnicontNode);
	//DxSkinAnimation* pAnimCont = pAnicontNode->m_pDxSkinAni;

	const DxSkinAnimation* pAnimCont = m_pSkinChar->GETCURANIM();
	if ( !pAnimCont )
		return FALSE;

	if ( pAnimCont->m_wStrikeCount == 0 )	return FALSE;

	const DWORD dwThisKey = static_cast<DWORD>(m_fattTIMER);
	const DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;

	if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
	{
		m_pSkinChar->UpdateStrikeAttachBone( dwThisKey, m_nattSTEP );
		if ( dwThisKey>=dwStrikeKey )
		{
			if ( m_pGaeaClient->IsVisibleCV(m_sActionID) )
			{
				//	Note : °ø°Ý½Ã ÀÌÆÑÆ® ¹ßÈÖ..
				//
				AttackEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );
			}
			m_nattSTEP++;
		}
	}

	return TRUE;
}

BOOL GLCharacter::AttackVehicleProc ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime * GETATTVELO ();
	//GASSERT(m_sVehicle.m_pSkinChar->GETCURANIMNODE());

	//PANIMCONTNODE pAnicontNode = m_sVehicle.m_pSkinChar->GETCURANIMNODE();
	//if ( !pAnicontNode )
	//	return FALSE;

	//GASSERT(pAnicontNode);
	//DxSkinAnimation* pAnimCont = pAnicontNode->m_pDxSkinAni;

	const DxSkinAnimation* pAnimCont = m_sVehicle.m_pSkinChar->GETCURANIM();
	if ( !pAnimCont )
		return FALSE;

	if ( pAnimCont->m_wStrikeCount == 0 )	return FALSE;

	DWORD dwThisKey = DWORD(m_fattTIMER*UNITANIKEY_PERSEC);

	DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;
	if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
	{
		m_sVehicle.m_pSkinChar->UpdateStrikeAttachBone( dwThisKey, m_nattSTEP );
		if ( dwThisKey>=dwStrikeKey )
		{
			if ( m_pGaeaClient->IsVisibleCV(m_sActionID) )
			{
				//	Note : °ø°Ý½Ã ÀÌÆÑÆ® ¹ßÈÖ..
				//
				AttackEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );
			}

			m_nattSTEP++;
		}
	}
	return TRUE;
}

BOOL GLCharacter::IsReActionableEnemyCrowPC( const ClientActor* pTARGET , const bool bEnableForcedPkMode /* = m_bForcedPKMode */ )
{
    std::tr1::shared_ptr<GLCharClient>	pCHAR = m_pGaeaClient->GetChar( pTARGET->GetGaeaID() );
    if( !pCHAR )
        return false;

    SDROP_CHAR&		sCHARDATA_TAR = pCHAR->GetCharData();
    if( IsSafeZone() || pTARGET->IsSafeZone() )
        return false;

	if ( m_dwUserLvl >= USER_USER_GM ) 
		return false;

    //	´ë·Ã½Ã °ø°Ý °¡´É.;
    if( ISCONFRONT_TAR( STARGETID(CROW_PC, pTARGET->GetGaeaID()) ) )
        return true;

    // ÆÄÆ¼¿ø °ø°Ý ºÒ°¡;
    if ( m_pGaeaClient->GetMyPartyClient()->isMember(pTARGET->GetGaeaID()) )
        return false;

    PLANDMANCLIENT	pLand			= m_pGaeaClient->GetActiveMap();
    if( pLand ) 			//	¸Ê°ü·Ã ¼Ó¼º Ã¼Å©;
    {
        // ¼±µµÀü;
        if( pLand->m_bClubBattle )
        {
            return !m_pMyClub->IsAllianceGuild_CHIEF( pCHAR->GETCLUBID() );
        }
        else if( pLand->m_bClubDeathMatch && m_pGaeaClient->IsClubDeathMatch() )
        {
            return !(m_pMyClub->DbNum() == pCHAR->GETCLUBID());
        }
        else if( pLand->IsPKMap() ) 
        {
            return !(m_pMyClub->DbNum() != 0 && m_pMyClub->DbNum() == pCHAR->GETCLUBID() );
        }
        else if( pLand->IsClubBattleZone() )
        {
            if( m_pMyClub->IsBattle( pCHAR->GETCLUBID() ) ||
                m_pMyClub->IsBattleAlliance( pCHAR->GETALLIANCEID() ) ) 
            {
                return true;
            } //if
        }
        else if( m_pGaeaClient->IsCTFPlayer() )
        {
            return (m_wSchool != pCHAR->GetCharSchool());
        }
    } //if

    // Àû´ëÀûÀÏ½Ã °ø°Ý °¡´É;
    if( IS_PLAYHOSTILE( pTARGET->GetCharID() ) )
        return true;

    // PKÁ¦ÇÑ ·¹º§;
    if( sCHARDATA_TAR.m_Base.wLevel <= GLCONST_CHAR::nPK_LIMIT_LEVEL )
        return false;

    // PK¼­¹öÀÎ°¡?;
    if( m_pGaeaClient->IsPKServer() )
    {
        if( m_wSchool != sCHARDATA_TAR.m_Base.wSchool && !m_pMyClub->IsAllianceGuild_CHIEF(sCHARDATA_TAR.m_Base.dwGuild ) )
            return true;
    } //if

    // ¼ºÇâ ÀÌº¥Æ®ÀÏ°æ¿ì.;
    if( m_pGaeaClient->IsBRIGHTEVENT() )
        return true;

    //	ÇÐ±³°£ ÇÁ¸® PK.;
    if( !GLCONST_CHAR::bSCHOOL_FREE_PK_Z_FORCED ) 
    {
        bool bSCHOOL_FREEPK = m_pGaeaClient->IsSchoolFreePk();

        if( bSCHOOL_FREEPK && m_wSchool != sCHARDATA_TAR.m_Base.wSchool )
            return true;
    } //if

    // °­Á¦ PK¸ðµå;
    if ( bEnableForcedPkMode )
    {
        return m_bForcedPKMode;
    }

    return false;
} //GLCharacter::IsReActionableEnemyCrowPC

BOOL GLCharacter::IsReActionableEnemy( const ClientActor* pTARGET )
{
	if( pTARGET->IsNonDamaged() )		// °ø°Ý ºÒ°¡´ÉÀÏ °æ¿ì
		return false;	

	switch( pTARGET->GetCrow() )
	{
	case CROW_PC:
		return IsReActionableEnemyCrowPC( pTARGET );

	case CROW_SUMMON:
	case CROW_MOB:
		return !( m_sCONFTING.IsCONFRONTING() || IsSafeZone() );
	}; //switch

	return FALSE;
} //GLCharacter::IsReActionableEnemy

BOOL GLCharacter::IsReActionableNoEnemyCrowPC( const ClientActor* pTARGET )
{
    //ÀÚ½Å ÀÚ½ÅÀÎ°¡?
    if ( m_dwGaeaID == pTARGET->GetGaeaID() )
        return TRUE;

    //»ó´ë¹æ Å¬¶óÀÌ¾ðÆ® ÄÉ¸¯ÅÍ
    std::tr1::shared_ptr<GLCharClient> pCLIENT = m_pGaeaClient->GetChar( pTARGET->GetGaeaID() );
    if( !pCLIENT )
        return FALSE;

	// Á¡·ÉÀü ÁßÀÏ¶§ °°Àº ÇÐ¿ø¿¡°Ô¸¸ °¡´É
	if ( m_pGaeaClient->IsCTFPlayer() )
	{
        return (m_wSchool == pCLIENT->GetCharSchool());
	} //if

    //PVP Áö¿ª¿¡¼­´Â ¾Æ±ºÀÎÁö °Ë»ç
	PLANDMANCLIENT		pLand	= m_pGaeaClient->GetActiveMap();
	if( pLand && 
		( pLand->m_bClubBattle		|| 
		  pLand->m_bClubDeathMatch	|| 
		  pLand->IsFreePK() ) )
	{
        return m_pGaeaClient->GetMyClub()->IsAllianceGuild_CHIEF(pCLIENT->GETCLUBID());
	}  //if

	   //  ´ë·ÃÁßÀÏ½Ã¿¡´Â »ó´ë¹æÀÌ °°Àº ´ë·Ã¿¡ ¼ÓÇÑ ¾Æ±ºÀÏ °æ¿ì¿¡°Ô¸¸ °¡´É;
    if ( m_sCONFTING.IsCONFRONTING() )
    {
        return ISCONFRONT_ALLIANCE(pCLIENT->GetGaeaID());
    }

    //	Àû´ëÀûÀÎ ´ë»óÀÎÁö °Ë»ç
    if ( IS_PLAYHOSTILE ( pTARGET->GetCharID() ) )
        return FALSE;

    return TRUE;
} //GLCharacter::IsReActionableNoEnemyCrowPC

BOOL GLCharacter::IsReActionableNoEnemy( const ClientActor* pTARGET )
{
	switch ( pTARGET->GetCrow() )
	{
	case CROW_PC:
		return IsReActionableNoEnemyCrowPC( pTARGET );
	
	case CROW_NPC:
		return !m_sCONFTING.IsCONFRONTING();

	case CROW_MATERIAL:
		return TRUE;
	}; //switch

	return FALSE;
} //GLCharacter::IsReActionableNoEnemy

// *****************************************************
// Desc: ¿¢¼ÇÀ» ÃëÇÒ ¼ö ÀÖ´Â ´ë»óÀÎÁö check
// *****************************************************
// EMIMPACT_SIDE_PARTY   : ¾Æ±º¿¡°Ô => ÆÄÆ¼¿ø¿¡°Ô;
// EMIMPACT_SIDE_ENEMY   : Àû¿¡°Ô   => Àû´ëÀû ´ë»ó¿¡°Ô;
// EMIMPACT_SIDE_ANYBODY : ¸ðµÎ¿¡°Ô => ÇÃ·¹ÀÌ¾î¿¡°Ô;
const BOOL GLCharacter::IsReActionable( const STARGETID &sTargetID, const EMIMPACT_SIDE emTargetSide /*= EMIMPACT_SIDE_ENEMY*/)
{
    return IsReActionable(sTargetID, emTargetSide, m_bForcedPKMode);
}

const BOOL GLCharacter::IsReActionable( const STARGETID &sTargetID, const EMIMPACT_SIDE emTargetSide, const bool _bForceAction)
{
    ClientActor *pTARGET = m_pGaeaClient->GetCopyActor(sTargetID);
    GLLandManClient* const pLandMan = m_pGaeaClient->GetLandManClient();
    if ( pLandMan == NULL || pTARGET == NULL )
        return FALSE;

	if ( pTARGET->GetCrow() == CROW_SUMMON )			// Å¸°ÙÀÌ ¼ÒÈ¯¼ö¶ó¸é ´ë»ó Å¸°Ù°Ë»ç¸¦ ¼ÒÈ¯¼öÀÇ ÁÖÀÎÀ¸·Î ¹Ù²Û´Ù
	{
		std::tr1::shared_ptr<GLSummonClient> spSummon = m_pGaeaClient->GetSummon(sTargetID.GaeaId);
		if (!spSummon)
			return false;

		pTARGET = m_pGaeaClient->GetCopyActor(STARGETID(CROW_PC, spSummon->m_dwOwnerGaeaID));
		if (!pTARGET)
			return false;
	} //if

	if( pTARGET->GetCrow() == CROW_PC && pTARGET->IsNonTargetable() )
	{
		return false;
	}

    Faction::ManagerClient* const pFactionMan = pLandMan->GetFactionManager();
    if ( pFactionMan )
    {
        const Faction::EMRELATION_TYPE _relation = pFactionMan->getRelationClient(this, pTARGET);
        if ( _relation != Faction::EMRELATION_ERROR )
        {
            switch ( emTargetSide )
            {
            case EMIMPACT_SIDE_ENEMY:  // Àû¿¡°Ô : Àû´ëÀûÀÎ ´ë»ó¿¡°Ô »óÈ£ÀÛ¿ëÀÏ¶§ true;
                if (_relation == Faction::EMRELATION_ENEMY )
                    return TRUE;

                if ( ( _relation == Faction::EMRELATION_NEUTRAL_ENEMY ) && ( _bForceAction ) )  // Àû´ëÀûÀÎ Áß¸³°ü°èÀÇ ´ë»ó¿¡°Ô´Â °­Á¦°ø°ÝÀ¸·Î °ø°Ý °¡´É;
                    return TRUE;

                //	´ë·ÃÀÚ °ø°Ý °¡´É.;
                if( ISCONFRONT_TAR( sTargetID ) )
                    return true;

                // Àû´ëÇàÀ§ÀÚ °ø°Ý °¡´É;
                if( IS_PLAYHOSTILE( pTARGET->GetCharID() ) )
                    return true;

                return false;

            case EMIMPACT_SIDE_PARTY:  // ÆÄÆ¼¿¡°Ô : ÆÄÆ¼¿øÀÎ ´ë»ó¿¡°Ô »óÈ£ÀÛ¿ëÀÏ¶§ true;
			case EMIMPACT_SIDE_OUR:

				// ÆÑ¼ÇÀ¸·Î ÆÇº°ÇÑ´Ù;
				if ( _relation == Faction::EMRELATION_ALLY )
					return TRUE;

				return FALSE;

				// ÆÄÆ¼¿øÀ¸·Î ÆÇº°ÇÏÁö ¾Ê´Â´Ù;
                /*if ( m_pGaeaClient->GetMyPartyClient()->isMember(sTargetID.GaeaId) )
                    return TRUE;*/

            case EMIMPACT_SIDE_ANYBODY: // ¸ðµÎ¿¡°Ô : ¸ðµç ´ë»ó¿¡°Ô true;
                //ÄÚµå°¡ ÀÚ½ÅÀ» ¼ÓÀÏÁö¶óµµ Çò°¥¸®Áö ¸»ÀÚ.. EMIMPACT_SIDE_ANYBODY´Â ¸ðµç´ë»óÀÌ ¾Æ´Ï´Ù.. ÇÃ·¹ÀÌ¾î¿Í NPC¸¸ÀÌ´Ù....;
                if ( ( pTARGET->GetCrow() == CROW_PC ) || ( pTARGET->GetCrow() == CROW_NPC ) )
                    return TRUE;

                return false;

            default:
                break;
            }
        }
    }

    return IsReActionableNonFaction(sTargetID, emTargetSide, _bForceAction);
} 

const BOOL GLCharacter::IsReActionableNonFaction( const STARGETID &sTargetID, const EMIMPACT_SIDE emTargetSide, const bool _bForceAction )
{
    ClientActor *pTARGET = m_pGaeaClient->GetCopyActor(sTargetID);

    if( !pTARGET )
        return false;

    if ( pTARGET->GetCrow() == CROW_SUMMON )			// Å¸°ÙÀÌ ¼ÒÈ¯¼ö¶ó¸é ´ë»ó Å¸°Ù°Ë»ç¸¦ ¼ÒÈ¯¼öÀÇ ÁÖÀÎÀ¸·Î ¹Ù²Û´Ù
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = m_pGaeaClient->GetSummon(sTargetID.GaeaId);
        if (!spSummon)
            return false;

        pTARGET = m_pGaeaClient->GetCopyActor(STARGETID(CROW_PC, spSummon->m_dwOwnerGaeaID));
        if (!pTARGET)
            return false;
    } //if

    switch ( emTargetSide )
    {
    case EMIMPACT_SIDE_PARTY:
        if ( m_pGaeaClient->GetMyPartyClient()->isMember(sTargetID.GaeaId) )
            return TRUE;
        break;

    case EMIMPACT_SIDE_ENEMY :
        {
            if ( ( sTargetID.emCrow == CROW_PC) && ( _bForceAction == false ) )
                return IsReActionableEnemyCrowPC(pTARGET,false);

            return IsReActionableEnemy(pTARGET);
        }

    case EMIMPACT_SIDE_ANYBODY :
        {
            switch ( pTARGET->GetCrow() )
            {
            //PCÀÇ °æ¿ì¿¡´Â Àû´ë,ºñÀû´ë »ó°ü¾øÀÌ °Ë»ç
            case CROW_PC:
                if( IsReActionableEnemyCrowPC(pTARGET) || IsReActionableNoEnemyCrowPC(pTARGET) )
                    return TRUE;

            //NPCÀÇ °æ¿ì¿¡´Â ´ë·ÃÁßÀÌ ¾Æ´Ò¶§¸¸ °¡´É
            case CROW_NPC:
                return !m_sCONFTING.IsCONFRONTING();
                //±× ¿Ü °æ¿ì´Â ºÒ°¡´É
            default:
                break;
            }
        }
        break;

    case EMIMPACT_SIDE_OUR :
        return IsReActionableNoEnemy(pTARGET);

    default:
        return FALSE;
    }
    return FALSE;
}

void GLCharacter::ClearReservationAction(void)
{
	m_ReservationAction.Clear();
}

// bForced;
// ÇöÀç ·¯´× ½ºÅ³ÀÌ ¿¹¾àµÇ¾îÀÖ´Â »óÅÂ¿¡¼­ ÀÌµ¿ÇÏ¶ó´Â ¸í·ÉÀÌ ³»·ÁÁö¸é;
// Äµ½½ÀÌµÇ¾î¾ßÇÏ´Âµ¥ Äµ½½ÀÌ ¾ÈµÊ;
void GLCharacter::ResetReservationAction(bool bForced)
{
	GLREACTION_ITER		iterReaction = m_ReservationAction.FrontIter();
	GLREACTION_ITER		iterEndReaction = m_ReservationAction.EndIter();

	bool bSkillCancle = false;
	while( iterReaction != iterEndReaction )
	{
		GLREACTION&			sReaction = (*iterReaction);
		if( sReaction.GETTYPE() == REACT_SKILL )
		{
			GLREACTION_SKILL&	sSkillReaction = (*iterReaction);
			PGLSKILL			pSkillData = GLSkillMan::GetInstance().GetData( sSkillReaction.GetSKILLID().wMainID, sSkillReaction.GetSKILLID().wSubID );
			GASSERT( pSkillData );

			if( bForced == false && (pSkillData->m_sBASIC.dwFlags & FLAG_SB_RUNNING_CAST) )
			{
				iterReaction++;
				continue;
			} //if
		} //if
		bSkillCancle = true;
		iterReaction = m_ReservationAction.EraseReaction( iterReaction );
	} 
	if ( bSkillCancle == true )
	{
// 		GLWidgetScript::GetInstance().LuaCallFunc< void >(
// 			NSWIDGET_SCRIPT::g_strFunc_ReservationSkillCancel_All);
	}
	//while
} //GLCharacter::ResetReservationAction

bool GLCharacter::IsMoveAble()
{
	if( m_emVehicle == EMVEHICLE_PASSENGER )
		return false;

	return true;
}

BOOL GLCharacter::ISCONFRONT_TAR ( const STARGETID &sTargetID )
{
	if ( sTargetID.emCrow!=CROW_PC )	return FALSE;

	if ( m_sCONFTING.IsFIGHTING() )
	{
		switch ( m_sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			return ( sTargetID.GaeaId==m_sCONFTING.dwTAR_ID );
			break;

		case EMCONFT_PARTY:
			{
				SCONFTING_CLT::CONFT_MEM_ITER iter = m_sCONFTING.setConftMember.find(sTargetID.GaeaId);
				return ( iter!= m_sCONFTING.setConftMember.end() );
			}
			break;

		case EMCONFT_GUILD:
			{
				std::tr1::shared_ptr<GLCharClient> pCHAR = m_pGaeaClient->GetChar ( sTargetID.GaeaId );
				if (!pCHAR)
                    return FALSE;

				SCONFTING_CLT::CONFT_MEM_ITER iter = m_sCONFTING.setConftMember.find(pCHAR->GetCharData().m_Base.dwCharID);
				return ( iter!= m_sCONFTING.setConftMember.end() );
			}
			break;
		};
	}

	return FALSE;
}

BOOL GLCharacter::ISCONFRONT_ALLIANCE( const DWORD dwGaeaID )
{
    std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar(dwGaeaID);
    if ( !pChar )
        return FALSE;

    if ( pChar->GetCrow() != CROW_PC )
        return FALSE;

    if ( m_sCONFTING.IsFIGHTING() )
    {
        switch ( m_sCONFTING.emTYPE )
        {
        case EMCONFT_ONE:
            //°³ÀÎ ´ë·Ã¿¡¼­ ¾Æ±ºÀº ¾øÀ½;
            return FALSE;
            break;

        case EMCONFT_PARTY:
            {
                //¿øÁ¤´ëÀÏ °æ¿ì´Â °í·ÁÇÏÁö ¾ÊÀ½ (±âÈ¹ÆÀ¿¡¼­ Àý´ë ¿øÁ¤´ë ´ë·ÃÀº ¾ø´Ù°í ¸ø¹ÚÀ½);
                //°°Àº ÆÄÆ¼ÀÎ°¡?;
                return m_pGaeaClient->GetMyPartyClient()->isMember(dwGaeaID);
            }
            break;

        case EMCONFT_GUILD:
            {
                //°°Àº ±æµåÀÎ°¡?;
                return m_pGaeaClient->GetMyClub()->IsMember(dwGaeaID);
            }
            break;
        };
    }

    return FALSE;
}

void GLCharacter::DELCONFRONT_MEMBER ( DWORD dwID )
{
	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_ONE:
		break;

	case EMCONFT_PARTY:
		{
			//	Note : ÀÚ±â ÆÄÆ¼¿øÀÏ °æ¿ì.
			m_pGaeaClient->GetMyPartyClient()->ResetConfrontMember(dwID);

			//	Note : »ó´ë¹æ ÆÄÆ¼¿øÀÏ °æ¿ì.
			m_sCONFTING.DEL_CONFT_MEMBER ( dwID );
		}
		break;

	case EMCONFT_GUILD:
		{
			//	Note : ÀÚ±â Å¬·´¿øÀÏ °æ¿ì.
			m_pMyClub->DELCONFT_MEMBER ( dwID );

			//	Note : »ó´ë¹æ Å¬·´¿øÀÏ °æ¿ì.
			m_sCONFTING.DEL_CONFT_MEMBER ( dwID );
		}
		break;
	};

	return;
}

bool GLCharacter::IsSchoolConfting()
{
	if ( m_sCONFTING.IsFIGHTING() && m_sCONFTING.sOption.bSCHOOL )
	{
		return true;
	}

	return false;
}

UINT GLCharacter::GetConftMemberCount()
{
	return static_cast<UINT>(m_sCONFTING.setConftMember.size());
}

void GLCharacter::NewConftBoundEffect ( const EMCONFT_TYPE emCONFT )
{
	//	Note : ÀÌÆÑÆ® ¼³Á¤.
	//
	DxEffGroupPlayer::GetInstance().DeleteAllEff ( GLCONST_CHAR::strCONFRONT_BOUND.c_str() );

	D3DXMATRIX matTrans, matRotY;
	D3DXVECTOR3 vDist;
	D3DXMatrixIdentity ( &matTrans );

	const float fPositionY = GetPosition().y;
	float vPos1 = fPositionY + 100.0f;
	float vPos2 = fPositionY - 100.0f;

	const D3DXVECTOR3 vConfrontPosition = D3DXVECTOR3(m_sCONFTING.vPosition.x, 0.0f, m_sCONFTING.vPosition.z);
	for ( float fRotY=0.0f; fRotY<(D3DX_PI*2.0f); fRotY += (D3DX_PI*2.0f/24.0f) )
	{
		vDist = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		D3DXMatrixRotationY(&matRotY,fRotY);
		D3DXVec3TransformCoord(&vDist,&vDist,&matRotY);

		float fCONFT_DIST(GLCONST_CHAR::fCONFRONT_ONE_DIST);
		switch ( emCONFT )
		{
		case EMCONFT_ONE:
			fCONFT_DIST = GLCONST_CHAR::fCONFRONT_ONE_DIST;
			break;
		case EMCONFT_PARTY:
			fCONFT_DIST = GLCONST_CHAR::fCONFRONT_PY_DIST;
			break;

		case EMCONFT_GUILD:
			fCONFT_DIST = GLCONST_CHAR::fCONFRONT_CLB_DIST;
			break;
		};

		vDist = vConfrontPosition + (vDist * fCONFT_DIST);		

		BOOL bCollision(FALSE);
		D3DXVECTOR3 vCollision;
		DWORD dwCollisionID;

		// ¼Ò¼öÁ¡ Á¤¹Ðµµ °ü·Ã ¹®Á¦ È®ÀÎ IsCollision
		boost::shared_ptr<NavigationMesh> spNaviMesh = m_pGaeaClient->GetActiveMap()->GetNaviMesh();
		if ( spNaviMesh )
		{
			spNaviMesh->IsCollision(vDist+D3DXVECTOR3(0,vPos1,0),vDist+D3DXVECTOR3(0,vPos2,0),vCollision,dwCollisionID,bCollision);
		}

		matTrans._41 = vDist.x;
		matTrans._42 = vCollision.y;		
		matTrans._43 = vDist.z;

		DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCONFRONT_BOUND.c_str(), matTrans, &TARGETID_NULL, TRUE, TRUE );
	}
}

void GLCharacter::DelConftBoundEffect ()
{
	DxEffGroupPlayer::GetInstance().DeleteAllEff ( GLCONST_CHAR::strCONFRONT_BOUND.c_str() );
}

BOOL GLCharacter::IsVaildTradeInvenSpace ()
{
	GLInventory &sTradeBoxTar = m_pGaeaClient->GetTradeClient()->GetTarTradeBox();

	GLInventory sInvenTemp;
	sInvenTemp.Assign ( m_cInventory );

	//	Note : ÀÚ½ÅÀÇ ±³È¯ ¸ñ·Ï¿¡ ¿Ã¶ó°£ ¾ÆÀÌÅÛÀ» ÀÎ¹ê¿¡¼­ Á¦¿Ü.
	//
	GLInventory &sTradeBoxMy = m_pGaeaClient->GetTradeClient()->GetMyTradeBox();
	const GLInventory::CELL_MAP* pItemListMy = sTradeBoxMy.GetItemList();

	GLInventory::CELL_MAP_CITER iter = pItemListMy->begin();
	GLInventory::CELL_MAP_CITER iter_end = pItemListMy->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pTradeItem = (*iter).second;

		BOOL bOk = sInvenTemp.DeleteItem ( pTradeItem->wBackX, pTradeItem->wBackY );
		if ( !bOk )
			return FALSE;
	}

	//	Note : »ó´ë¹æÀÇ ±³È¯ ¹°Ç°ÀÌ µé¾î °¥¼ö ÀÖ´ÂÁö °Ë»ç.
	//
	const GLInventory::CELL_MAP* pItemListTar = sTradeBoxTar.GetItemList();

	iter = pItemListTar->begin();
	iter_end = pItemListTar->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pTradeItem = (*iter).second;

		SITEM *pItem = GLogicData::GetInstance().GetItem ( pTradeItem->sItemCustom.GetNativeID() );
		if ( !pItem )
			return FALSE;

		WORD wPosX = 0, wPosY = 0;
		BOOL bOk = sInvenTemp.FindInsertable ( wPosX, wPosY );
		if ( !bOk )
			return FALSE;

		bOk = sInvenTemp.InsertItem ( pTradeItem->sItemCustom, wPosX, wPosY );
		if ( !bOk )
			return FALSE;
	}
	
	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------------------
//								H	a	i	r				S	t	y	l	e	 &&	 C	o	l	o	r
//----------------------------------------------------------------------------------------------------------------------------------
//void GLCharacter::HairStyleInitData()
//{
//	//m_bEnableHairSytle = TRUE;
//	
//	//m_wHairStyleTEMP = m_wHair;
//
//	//// ÄÃ·¯ º¯È­ ÁßÀÏ °æ¿ì °ÇµéÁö ¾Ê´Â´Ù.
//	//if( !m_bEnableHairColor )
//	//{
//	//	m_wHairColorTEMP = m_wHairColor;
//	//}
//}
//
//void GLCharacter::HairColorInitData()
//{
//	//m_bEnableHairColor = TRUE;
//
//	//m_wHairColorTEMP = m_wHairColor;
//
//	//// ½ºÅ¸ÀÏ º¯È­ ÁßÀÏ °æ¿ì °ÇµéÁö ¾Ê´Â´Ù.
//	//if( !m_bEnableHairSytle )
//	//{
//	//	m_wHairStyleTEMP = m_wHair;
//	//}
//}

void GLCharacter::HairStyleChange( WORD wStyle )
{
	//m_wHairStyleTEMP = wStyle; 

	HairStyleUpdate( wStyle );
}

void GLCharacter::HairColorChange( WORD wColor )
{ 
	//m_wHairColorTEMP = wColor; 

	HairColorUpdate( wColor );
}

void GLCharacter::HairStyleUpdate( WORD wStyle )
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return;

	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	// Çì¾î½ºÅ¸ÀÏ.
	if ( sCONST.dwHAIRNUM > wStyle )
	{
		std::string strHAIR_CPS = sCONST.strHAIR_CPS[wStyle];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	ÇöÀç ÀåÂø ½ºÅ².

		if( pCharPart && strcmp( strHAIR_CPS.c_str(), pCharPart->GetFileName() ) )
		{
			m_pSkinChar->SetPiece( strHAIR_CPS.c_str(), m_pd3dDevice, NULL, NULL, EMSCD_ALL_THREAD );
		}
	}

	//// Note : ÀÌ°ÍÀ» È£ÃâÇÏ¸é¼­.. ¸Ó¸®ÇüÀÌ ¿ø»óÅÂ·Î ¹Ù²î¾î ¹ö¸°´Ù.
	////		±×·¡¼­ FALSE È£Ãâ·Î ¸Ó¸®¸ð¾çÀ» ¾È¹Ù²î°Ô ÇÑ´Ù.
	//UpdateSuit( FALSE, TRUE );
}

void GLCharacter::HairColorUpdate( WORD wColor )
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return;

    if ( m_pSkinChar )
	    m_pSkinChar->SetHairColor( wColor );
}

//void GLCharacter::FaceStyleInitData()
//{
//	//m_bEnableFaceStyle = TRUE;
//	
//	m_wFaceStyleTEMP = m_wFace;
//}

void GLCharacter::FaceStyleChange( WORD wStyle )
{
	//m_wFaceStyleTEMP = wStyle; 

	FaceStyleUpdate( wStyle );
}

void GLCharacter::FaceStyleUpdate( WORD wStyle )
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return;

	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	// ¾ó±¼
	if ( sCONST.dwHEADNUM > wStyle )
	{
		std::string strHEAD_CPS = sCONST.strHEAD_CPS[wStyle];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HEAD);		//	ÇöÀç ÀåÂø ½ºÅ².

		if ( pCharPart && strcmp( strHEAD_CPS.c_str(), pCharPart->GetFileName() ) )
		{
			m_pSkinChar->SetPiece ( strHEAD_CPS.c_str(), m_pd3dDevice, 0x0, 0, EMSCD_ALL_THREAD );
		}
	}

	//// Note : ÀÌ°ÍÀ» È£ÃâÇÏ¸é¼­.. ¸Ó¸®ÇüÀÌ ¿ø»óÅÂ·Î ¹Ù²î¾î ¹ö¸°´Ù.
	////		±×·¡¼­ FALSE È£Ãâ·Î ¸Ó¸®¸ð¾çÀ» ¾È¹Ù²î°Ô ÇÑ´Ù.
	//UpdateSuit( TRUE, FALSE );
}

BOOL GLCharacter::IsMarketClick( DWORD dwGaeaID )
{
	MAPMARKETCLICK_ITER iter = m_mapMarketClick.find( dwGaeaID ) ;
	if( iter!=m_mapMarketClick.end() )
	{
		return TRUE;
	}
	return FALSE;
}

void GLCharacter::InsertMarketClick( DWORD dwGaeaID )
{
	MAPMARKETCLICK_ITER iter = m_mapMarketClick.find( dwGaeaID ) ;
	if( iter!=m_mapMarketClick.end() )
	{
		return;
	}
	m_mapMarketClick.insert( std::make_pair( dwGaeaID, dwGaeaID ) );
}

void GLCharacter::InitAllSkillFactByMoveMap ()
{
	// CPµµ ÃÊ±âÈ­
//	m_sCP.SetData( 0, MAX_CP );
	
	m_dwTransformSkill = SNATIVEID::ID_NULL;

	FactEffectProcessing( EMFACTEFFECTPROCESSING_MOVEMAP );
}

D3DXVECTOR3 GLCharacter::GetPosBodyHeight ()
{ 
	if( NULL == m_pSkinChar )
		return GetPosition();

	if ( m_emVehicle != EMVEHICLE_OFF && m_sVehicle.m_emTYPE == VEHICLE_TYPE_BOARD ) 
		return GetPosition() + D3DXVECTOR3(0.0f, m_pSkinChar->m_fHeight + m_sAirborne.getHeight() + 10.0f, 0.0f);
	else 
		return GetPosition() + D3DXVECTOR3(0.0f, m_pSkinChar->m_fHeight + m_sAirborne.getHeight(), 0.0f);
}

LONGLONG GLCharacter::GetCalcTaxiCharge( int nSelectMap, int nSelectStop )
{
	GLTaxiStation& sTaxiStation = GLTaxiStation::GetInstance();

	STAXI_MAP* pTaxiMap = sTaxiStation.GetTaxiMap( nSelectMap );
	if ( !pTaxiMap )
	{
		GASSERT ( 0 && "pTaxiMap¸¦ Ã£À»¼ö ¾ø½À´Ï´Ù." );
		return 0;
	}

	STAXI_STATION* pSTATION = pTaxiMap->GetStation( nSelectStop );
	if ( !pSTATION )
	{
		GASSERT ( 0 && "STATIONÀ» Ã£À» ¼ö ¾ø½À´Ï´Ù." );
		return 0;
	}

	DWORD dwCurMapID = m_pGaeaClient->GetActiveMapID().getGaeaMapID().Id();
	LONGLONG dwCharge = sTaxiStation.GetBasicCharge();

	if ( pSTATION->dwMAPID != dwCurMapID )
		dwCharge += pSTATION->dwMapCharge;

	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if ( pLand )
	{
		volatile double fSHOP_RATE = GetBuyRate();

        dwCharge = dwCharge * static_cast<LONGLONG>(fSHOP_RATE) / 100;
	}

	return dwCharge;
}

bool GLCharacter::IsCoolTime(const SNATIVEID& sNativeID)
{
	__time64_t tCurTime = m_pGaeaClient->GetCurrentTime().GetTime();
	__time64_t tCoolTime = GetMaxCoolTime(sNativeID);
	if (tCurTime < tCoolTime)
        return true;
    else
	    return false;
}

bool GLCharacter::CheckCoolTime(const SNATIVEID& sNativeID)
{
	if (IsCoolTime(sNativeID))
	{
		SITEM* pItem = GLogicData::GetInstance().GetItem(sNativeID);
		if (!pItem)
            return true;
		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(
                ID2GAMEINTEXT("ITEM_COOLTIME"),
                pItem->GetName()));
		return true;
	}
	return false;
}

__time64_t GLCharacter::GetMaxCoolTime(const SNATIVEID& sNativeID)
{
	//	¾ÆÀÌÅÛ Á¤º¸ °¡Á®¿È.
	SITEM* pItem = GLogicData::GetInstance().GetItem(sNativeID);
	if (!pItem)
        return 0;

	const ITEM_COOLTIME& pCoolTimeType = GetCoolTime( (DWORD) pItem->sBasicOp.emItemType, EMCOOL_ITEMTYPE );
	const ITEM_COOLTIME& pCoolTimeID = GetCoolTime( pItem->sBasicOp.sNativeID.dwID, EMCOOL_ITEMID );		
	return (pCoolTimeType.tCoolTime > pCoolTimeID.tCoolTime) ? pCoolTimeType.tCoolTime : pCoolTimeID.tCoolTime;
}

const ITEM_COOLTIME& GLCharacter::GetCoolTime(const SNATIVEID& sNativeID) const
{
	//	¾ÆÀÌÅÛ Á¤º¸ °¡Á®¿È.
	const SITEM* pItem = GLogicData::GetInstance().GetItem(sNativeID);
	if (!pItem)
    {
        static ITEM_COOLTIME CoolTimeTemp;
        return CoolTimeTemp;
    }

	const ITEM_COOLTIME& pCoolTimeType = GetCoolTime((DWORD) pItem->sBasicOp.emItemType, EMCOOL_ITEMTYPE);
	const ITEM_COOLTIME& pCoolTimeID = GetCoolTime(pItem->sBasicOp.sNativeID.dwID, EMCOOL_ITEMID);
	return (pCoolTimeType.tCoolTime > pCoolTimeID.tCoolTime) ? pCoolTimeType : pCoolTimeID;
}

const ITEM_COOLTIME& GLCharacter::GetCoolTime(DWORD dwCoolID, EMCOOL_TYPE emCoolType) const
{
    static ITEM_COOLTIME CoolTimeTemp;
    
	if (emCoolType == EMCOOL_ITEMID)
	{
		COOLTIME_MAP_CITER pos = m_mapCoolTimeID.find(dwCoolID);
		if (pos==m_mapCoolTimeID.end())
            return CoolTimeTemp;
        else		
		    return pos->second;
	}
	else if ( emCoolType == EMCOOL_ITEMTYPE )
	{
		COOLTIME_MAP_CITER pos = m_mapCoolTimeType.find(dwCoolID);
		if (pos == m_mapCoolTimeType.end())
            return CoolTimeTemp;
        else
		    return pos->second;
	}
	else
    {
		return CoolTimeTemp;
    }
}

bool GLCharacter::SetCoolTime ( ITEM_COOLTIME& sCoolTime, EMCOOL_TYPE emCoolType )
{
	if ( emCoolType == EMCOOL_ITEMID )
	{
		COOLTIME_MAP_ITER iter = m_mapCoolTimeID.find(sCoolTime.dwCoolID);
        if (iter != m_mapCoolTimeID.end())
        {
            ITEM_COOLTIME& CoolTime = iter->second;
            CoolTime = sCoolTime;
        }
        else
        {
            m_mapCoolTimeID.insert(
                COOLTIME_MAP_VALUE(sCoolTime.dwCoolID, sCoolTime));
        }
	}
	else if( emCoolType == EMCOOL_ITEMTYPE )
	{
		COOLTIME_MAP_ITER iter = m_mapCoolTimeType.find(sCoolTime.dwCoolID);
        if (iter != m_mapCoolTimeType.end())
        {
            ITEM_COOLTIME& CoolTime = iter->second;
            CoolTime = sCoolTime;
        }
        else
        {
            m_mapCoolTimeType.insert(
                COOLTIME_MAP_VALUE(sCoolTime.dwCoolID, sCoolTime));
        }
	}

	//GfxInterfaceBase::GetInstance()->SetItemSlotCoolDown( SNATIVEID( sCoolTime.dwID ) );

	return true;
}

const SITEMCUSTOM GLCharacter::GET_ITEM_MIX( int nIndex )
{
	SITEMCUSTOM sItemCustom( NATIVEID_NULL() );

	if( !m_sProductMaterialPos[nIndex].VALID() )
		return sItemCustom;

	SINVENITEM* pResistItem = m_cInventory.GetItem( m_sProductMaterialPos[nIndex].wPosX, m_sProductMaterialPos[nIndex].wPosY );
	if( !pResistItem )
		return sItemCustom;

	sItemCustom = pResistItem->sItemCustom;

	return sItemCustom;
}

void GLCharacter::SetItemMixMoveItem( int nIndex )
{
	if ( nIndex >= ITEMMIX_ITEMNUM )
	{
		m_sPreInventoryItem.RESET();
		return;
	}

	if( m_sPreInventoryItem.VALID() )
	{
		for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i )
		{
			if ( m_sProductMaterialPos[i].wPosX == m_sPreInventoryItem.wPosX &&
				m_sProductMaterialPos[i].wPosY == m_sPreInventoryItem.wPosY )
			{
				m_sPreInventoryItem.RESET();
				return;
			}

		}

		m_sProductMaterialPos[nIndex].SET( m_sPreInventoryItem.wPosX, m_sPreInventoryItem.wPosY );
		
		m_sPreInventoryItem.RESET();
	}
	
	return;
}

void GLCharacter::ReSetItemMixItem( int nIndex )
{
	if ( nIndex >= ITEMMIX_ITEMNUM || m_sPreInventoryItem.VALID() )
	{
		m_sPreInventoryItem.RESET();
		return;
	}

	m_sProductMaterialPos[nIndex].RESET();
}

void GLCharacter::ResetItemMix()
{
	for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		m_sProductMaterialPos[i].RESET();
	}
	m_sPreInventoryItem.RESET();
}

bool GLCharacter::ValidItemMixOpen()
{
	//!! SF_TODO
	/*if (m_pInterface->UiIsVisibleGroup(ITEM_MIX_INVEN_WINDOW) ||
        m_pInterface->UiIsVisibleGroup(ITEM_MIX_WINDOW))
    {
		return true;
    }
    else*/
    {
	    return false;
    }
}

void GLCharacter::CloseCostumeStat()
{
	m_bCostumeStats = false; 
	m_sPreInventoryItem.RESET();
}

bool GLCharacter::ValidCostumeStatOpen()
{
	//!! SF_TODO
	if (m_pGaeaClient->GetInterface()->UiIsVisibleGroup(COSTUME_STATS_WINDOW) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool GLCharacter::IsOpenNPCMarket ()
{
// 	return GLWidgetScript::GetInstance().LuaCallFunc< bool >(
// 		NSWIDGET_SCRIPT::g_strFunc_NpcTalk_IsMarketTabOpen );
	return m_pGaeaClient->GetInterface()->UiIsVisibleGroup( DIALOGUE_WINDOW_RENEWAL );
	//return false;
}

DWORD GLCharacter::GetItemMixMoney()
{
	SProductRecipe sProductRecipe;

	for( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		sProductRecipe.sMeterialItem[i].sNID = GET_ITEM_MIX( i ).GetNativeID();

		if( sProductRecipe.sMeterialItem[i].sNID != NATIVEID_NULL() )
			sProductRecipe.sMeterialItem[i].nNum = (BYTE)GET_ITEM_MIX( i ).wTurnNum;
	}

	GLProductRecipeMan::GetInstance().SortMeterialItem( sProductRecipe );

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	const SProductRecipe* pProductRecipe;
	if (pGlobalStage->IsEmulator())
		pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipeEmulator(sProductRecipe);
	else
		pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipeClient(sProductRecipe);

	if (!pProductRecipe)
        return UINT_MAX;
    else
	    return pProductRecipe->dwPrice;
}

bool GLCharacter::ValidWindowOpen()
{
	if (ValidRebuildOpen() ||
        ValidGarbageOpen() ||
        ValidItemMixOpen() ||
		ValidCostumeStatOpen() ||
        IsOpenMiniGame() ||
        m_pGaeaClient->GetTradeClient()->Valid())
    {
        return true;
    }
    else
    {
	    return false;
    }
}

bool GLCharacter::GetLinkDataName( const SLINK_TYPE& sLinkType, DWORD dwData, CString& strLinkName, DWORD& dwColor )
{
	CString strTemp;

	switch ( sLinkType.wMType ) 
	{
	case ITEM_LINK:
		{
			switch ( sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN:
			case ITEM_LINK_PUTON:
			case ITEM_LINK_USER_STORAGE:
			case ITEM_LINK_CLUB_STORAGE:
			case ITEM_LINK_CROW_SALE:
			case ITEM_LINK_NORMAL:
				{
					// ¾ÆÀÌÅÛ Á¤º¸ °¡Á®¿À±â;
					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = dwData;

					SINVENITEM* pInvenItem = m_cInventory.GetItem( sInvenPos.wPosX, sInvenPos.wPosY );
					if ( NULL == pInvenItem )
						return false;

					const SITEMCUSTOM& sItemCustom = pInvenItem->CustomItem();

					SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
					if ( NULL == pItem )
						return false;
					
					// ¸µÅ© ÀÌ¸§ ¼¼ÆÃ;
					strTemp.Format("%s%s%s", strLINK_LEFT_SIMBOL, sItemCustom.GETNAME().c_str(), strLINK_RIGHT_SIMBOL);					
					strLinkName = strTemp;
                    //¾ÆÀÌÅÛ ¸µÅ©º°·Î »ö»óÀÌ ´Ù¸£µµ·Ï ¼öÁ¤ 
					//dwColor = COMMENT::ITEMCOLOR[pItem->sBasicOp.emLevel];
                    dwColor = ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ];

					return true;
				}
				break;
			}
		}
		break;
	case PARTY_LINK:
		{
			switch ( sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT:
				{
                    CString strLocation("");

                    // Áö¿ª Á¤º¸ Ç¥½Ã
                    if ( m_pGaeaClient )
                    {
                        strLocation = m_pGaeaClient->GetMapName(dwData);
                    }

					strTemp.Format( "%s%s, %s%s", strLINK_LEFT_SIMBOL, strLocation, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 1 ), strLINK_RIGHT_SIMBOL );					
					strLinkName = strTemp;
					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;

					return true;
				}
				break;
			case PARTY_LINK_ACCEPT:
				{
					strTemp.Format( "%s%s%s", strLINK_LEFT_SIMBOL, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 2 ), strLINK_RIGHT_SIMBOL );	
					
					strLinkName = strTemp;
					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;

					return true;
				}
				break;
			case PARTY_LINK_REFUSE:
				{
					strTemp.Format( "%s%s%s", strLINK_LEFT_SIMBOL, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 3 ), strLINK_RIGHT_SIMBOL );			
					
					strLinkName = strTemp;
					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;

					return true;
				}
				break;
			}
		}
		break;
	case QUEST_LINK:
	case SKILL_LINK:
	case CROW_LINK:
	case MAP_LINK:
		{
		}
		break;
	}

	return false;
}

bool GLCharacter::GetLinkData( const SLINK_DATA_BASIC& sLinkDataBasic, CString& strLinkName, DWORD& dwColor )
{
	CString strTemp;

	switch ( sLinkDataBasic.sLinkType.wMType ) 
	{
	case ITEM_LINK:
		{
			switch ( sLinkDataBasic.sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN:
				{
					if ( false == GetLinkDataName( sLinkDataBasic.sLinkType, sLinkDataBasic.dwData1, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;
				}
				break;
			case ITEM_LINK_PUTON:
				{
					EMSLOT emSlot = (EMSLOT)sLinkDataBasic.dwData1;
					if ( emSlot >= SLOT_TSIZE ) break;
					if ( !VALID_SLOT_ITEM( emSlot )) break;

					const SITEMCUSTOM &sItemCustom = m_PutOnItems[emSlot];					

					if ( !GetLinkDataName( sLinkDataBasic.sLinkType, sItemCustom.GetBoxWrappedItemID().dwID, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;

				}
				break;
			case ITEM_LINK_USER_STORAGE:
				{
					DWORD dwChannel = sLinkDataBasic.dwData1;
					if ( dwChannel >= MAX_CLUBSTORAGE )
                        break;
					
					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;
					
					SINVENITEM* pInvenItem = m_cStorage[dwChannel].FindPosItem ( sInvenPos.wPosX, sInvenPos.wPosY );
					if ( !pInvenItem )	break;

					if ( !GetLinkDataName( sLinkDataBasic.sLinkType, pInvenItem->sItemCustom.GetBoxWrappedItemID().dwID, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;
				}
				break;
			case ITEM_LINK_CLUB_STORAGE:
				{
					if (!m_pMyClub->IsValid())
                        break;
					if (m_pMyClub->MasterCharDbNum() != m_CharDbNum)
                        break;

					DWORD dwChannel = sLinkDataBasic.dwData1;
					if ( dwChannel >= MAX_CLUBSTORAGE )
                        break;

					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;
					
					SINVENITEM* pInvenItem = m_pMyClub->FindPosItem(dwChannel, sInvenPos.wPosX, sInvenPos.wPosY);
					if (!pInvenItem)
                        break;

					if (!GetLinkDataName(sLinkDataBasic.sLinkType, pInvenItem->sItemCustom.GetBoxWrappedItemID().dwID, strLinkName, dwColor)) 
						break;

					return true;
				}
				break;
			case ITEM_LINK_CROW_SALE:
				{
					
					DWORD dwNpcGlobID = ( sLinkDataBasic.dwData1 & 0xFFFF0000 ) >> 16;
					DWORD dwChannel = ( sLinkDataBasic.dwData1 & 0x0000FFFF );

					PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
					if ( !pLand )
                        break;
					std::tr1::shared_ptr<GLCrowClient> spCrow = pLand->GetCrow(dwNpcGlobID);
					if (!spCrow)
                        break;

					if (spCrow->m_pCrowData->GetSaleNum() <= dwChannel)
                        break;

					const GLInventory *pInven = spCrow->m_pCrowData->GetSaleInven(dwChannel);
					if (!pInven)
                        break;

					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;
					
					SINVENITEM* pSaleItem = pInven->GetItem ( sInvenPos.wPosX, sInvenPos.wPosY );
					if ( !pSaleItem ) break;

					if ( !GetLinkDataName( sLinkDataBasic.sLinkType, pSaleItem->sItemCustom.GetBoxWrappedItemID().dwID, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;
				}
				break;

			case ITEM_LINK_NORMAL :
				{
					if( false == GetLinkDataName( sLinkDataBasic.sLinkType,
						sLinkDataBasic.dwData1, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;
				}
				break;
			}
		}
		break;
	case PARTY_LINK:
		{
			switch ( sLinkDataBasic.sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT:
			case PARTY_LINK_ACCEPT:
			case PARTY_LINK_REFUSE:
				{	
					GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
					if ( (pMyParty->isValid() == true) && (pMyParty->isMaster(m_dwGaeaID) == false) )
                        break;

					if ( !GetLinkDataName( sLinkDataBasic.sLinkType, sLinkDataBasic.dwData1, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;
				}
				break;
			}
		}
		break;
	case QUEST_LINK:
	case SKILL_LINK:
	case CROW_LINK:
	case MAP_LINK:
		{
		}
		break;
	}

	return false;
}

bool GLCharacter::IsPOSTBOXUSEABLE ( const EMPOSTBOXID_TYPE emIDType, const SNATIVEID& sPostBoxID )
{
	// Note : ¿ìÆíÇÔ »ç¿ë ºÒ°¡´ÉÀÌ¸é
    if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return false;
	}

	if ( !IsValidBody() )
	{
		return false;
	}

	if ( emIDType == EMPOSTBOXID_TYPE_NONE || sPostBoxID == NATIVEID_NULL() )
	{
		return false;
	}

	// Note : ¿ìÆíÇÔ ¾ÆÀÌµð°¡ ÀÖ´Ù¸é
	switch ( emIDType )
	{
	default:
		return false;

	case EMPOSTBOXID_TYPE_NPC:
		{
			PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap();

			if ( !pLandMan )
			{
				return false;
			}

			// ¿ìÃ¼Åë°úÀÇ °Å¸® Ã¼Å©
			std::tr1::shared_ptr<GLCrowClient> spTarget =  pLandMan->GetCrow(sPostBoxID.dwID);
			if (!spTarget || !spTarget->m_pCrowData)
			{
				return false;
			}

			if (spTarget->m_pCrowData->m_emCrow != CROW_NPC || 
                spTarget->m_pCrowData->m_emNPCType != NPC_TYPE_POSTBOX )
			{
				return false;
			}

			const D3DXVECTOR3 &vTarPos = spTarget->GetPosition();

			float fMoveDist = FLT_MAX;
			float fDist = D3DXVec3Length ( &D3DXVECTOR3(GetPosition() - vTarPos) );
			if ( isAction(GLAT_MOVE) )
                fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sActionID.vPos-vTarPos) );

			// ¿ìÆí ÀÌ¿ëÀÌ °¡´ÉÇÑ ¹üÀ§
			WORD wTakeRange   = spTarget->GetBodyRadius() + GETBODYRADIUS() + 30;
			WORD wTakeAbleDis = (WORD)(wTakeRange + GLCONST_POST::fPOSTBOX_DISTANCE);

			// Áö±Ý À§Ä¡°¡ ´ëÈ­°¡ ºÒ°¡´É ÇÏ¸é
			if ( fDist>wTakeAbleDis )
			{
				return false;
			}
		}
		break;

	case EMPOSTBOXID_TYPE_ITEM_CARD:
		{
			SITEM *pITEM = GLogicData::GetInstance().GetItem ( sPostBoxID );
			if ( !pITEM )
			{
				return false;
			}

			// rm #1559 ¿ìÆí¿¬°áÄ«µå ¼ö·® 1°³½Ã ¿ìÆíUI ¶ßÁö ¾Ê´Â ¹®Á¦..
			// ÀÌ¹Ì Â÷°¨ÈÄ¿¡ ¾Æ·¡ÄÚµå °Ë»çÇÏ¸é ¾Æ¹«°Íµµ ¾ø¾î¼­ ¹«Á¶°Ç false ÀÓ.
			// ÀÌ¹Ì ¼­¹ö¿¡¼­ Á¤»ó Ã³¸®µÇ¾î ¿À±â¶§¹®¿¡ ¿©±â¼­ ÀÌ°É ÇÒÇÊ¿ä ¾øÀ»°ÍÀ¸·Î º¸ÀÓ.
			//SINVENITEM* pInvenItem = m_cInventory.FindItem( ITEM_POSTBOX_CARD );
			//if ( !pInvenItem )
			//{
			//	return false;
			//}
		}
		break;
	}

	return true;
}

//  ´ë·Ã Á¾·á½Ã Ã³¸® ÀÛ¾÷
void GLCharacter::EndConfting()
{
    m_sCONFTING.RESET();
    //DisableDebuffSkillFact();
	DelConftBoundEffect();
}

void GLCharacter::ResetLimitTimeItemSort()
{
    m_fLimitTimeItemSort = fLIMIT_TIME_ITEM_SORT;
}

void GLCharacter::SetEnableSendItemSortMsg()
{
    m_bSendItemSortMsg = false;
    m_fItemSortMsgDelayTime = fMAX_ITEM_SORT_MSG_DELAY_TIME;
}

bool GLCharacter::IsEnalbeItemSort()
{
// 	if( false == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
// 		NSWIDGET_SCRIPT::g_strFunc_IsInvenItemSort ) )
// 	{
// 		return false;
// 	}

    if ( m_fLimitTimeItemSort > 0.0f ||
		 m_bSendItemSortMsg )
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::RED,
			ID2GAMEINTEXT( "ITEM_SORT_DELAY_TIME" ) );
        return false;
	}

    return true;
}

void GLCharacter::SetDisableSendItemSortMsg()
{
    m_bSendItemSortMsg = true;
    m_fItemSortMsgDelayTime = fMAX_ITEM_SORT_MSG_DELAY_TIME;
}

void GLCharacter::UpdateActivityItem()
{
    if ( !GLUseFeatures::GetInstance().IsUsingActivity() )
    {
        return;
    }

    MMACTIVITIES& mmActivities = ActivityBaseData::GetInstance()->GetData();

    ENUM_ACTIVITY_MID nMid = ACTIVITY_MID_TAKE; 

    bool m_bUpdateActivity = false;

    for ( ITER_MMACTIVITIES pos = mmActivities.lower_bound(nMid); pos != mmActivities.upper_bound(nMid); ++pos )
    {
        Activity* pActivity = &pos->second;

        if ( !pActivity->GetValidate() ) 
            continue;

        if ( pActivity->GetStatus() != ACTIVITY_STATUS_INPROGRESS ) 
            continue;

        DWORD dwCount(0);
        DWORD dwTargetID(0);
        for ( size_t i = 0; i < pActivity->m_vecTargetID.size(); i++ )
        {
            const DWORD _itemCount = GETHAVEITEM(pActivity->m_vecTargetID[i]);
            if ( dwCount < _itemCount )
            {
                dwCount = _itemCount;
                dwTargetID = pActivity->m_vecTargetID[i];
            }
        }
        DWORD dwPreValue  = pActivity->GetValue();
        DWORD dwBaseValue = pActivity->m_nValueBase;

        //  °ªÀÌ º¯°æµÇ¾ú´Ù.
        if ( dwPreValue != dwCount ) 
        {
            pActivity->SetValue( dwCount );
            m_bUpdateActivity = true;

            // ÁøÇà ÅØ½ºÆ® Ãâ·Â
            if ( dwPreValue < dwCount && dwCount < dwBaseValue )
            {
				//!! SF_TODO
                if ( m_pGaeaClient->GetInterface()->IsActivityProgressDisplay() )
                {
                    m_pGaeaClient->GetInterface()->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_ACTIVITY_PROGRESS, 
                        sc::string::format( ID2GAMEINTEXT( "ACTIVITY_UPDATE_PROGRESS_PROGRESSDISPLAY" ), pActivity->GetTitle(), dwCount, dwBaseValue ) );
                }
				/*if ( strlen(pActivity->GetTitle().c_str()) != 0 )
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_StudentRecord_ProgressMessege
						, "-s, -n, -n", pActivity->GetTitle().c_str(), dwCount, dwBaseValue);
				}*/
            }
        }

        if ( dwCount >= pActivity->m_nValueBase ) 
        {
            // ¿äÃ» Ã³¸®
            GLMSG::SNET_ACTIVITY_DONE_ITEMTAKE NetMsg;
            NetMsg.dwActivityID = pActivity->m_id.dwID;
            NetMsg.dwItemID = dwTargetID;
            m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

            // ÀÓ½Ã·Î ¿Ï·á Ã³¸®
            pActivity->SetStatus( ACTIVITY_STATUS_ORDERING );  
        }

    }


    if ( m_bUpdateActivity ) 
    {
// 		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
// 			NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Act );
    }
}

void GLCharacter::ResetRnAttendaceQuest()
{
	//! Äù½ºÆ® Æ÷±â.
	GLQUESTPROG* pQuestProg = NULL;

	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		pQuestProg = (*iter).second;

		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( pQuestProg->m_sNID.dwID );
		if ( !pQUEST )
		{
			++iter;
			continue;
		}

		CTime TToday = CTime::GetCurrentTime();

		if (pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
			pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE)
		{
			CTime CQuestStartTime(pQuestProg->m_tStartTime);

			if ( CQuestStartTime.GetDay() != TToday.GetDay() )
			{
				pQuestProg ->DoGIVEUP();

				//	¿Ï·áµÈ ¸®½ºÆ®¿¡ Ãß°¡.
				m_cQuestPlay.InsertEnd(*pQuestProg, false);

				//	ÁøÇàÁ¤º¸ »èÁ¦.
				//m_cQuestPlay.DeleteProc ( pQuestProg->m_sNID.dwID  );

				 m_cQuestPlay.DeleteMobProc( pQuestProg->m_sNID.dwID );
				 m_cQuestPlay.DeleteItemUseProc(pQuestProg->m_sNID.dwID);
				 SAFE_DELETE ( (*iter).second );
				 m_cQuestPlay.GetQuestProc().erase ( iter++ );
//				return true;
			}
			else
			{
				++iter;
			}
		}
		else
		{
			++iter;
		}
	}
}

BOOL GLCharacter::CHECK_ACTIVITY_POINT ( const SITEM& sItem )
{
    //  È°µ¿ ¿ä±¸ Æ÷ÀÎÆ® Ã¼Å©
    if ( sItem.sBasicOp.wReqActPointDW || sItem.sBasicOp.wReqActPointUP )
    {
        int emType = sItem.sBasicOp.emReqActivityType;
        if ( m_llActivityPoint[emType] < sItem.sBasicOp.wReqActPointDW ) return FALSE;
        if ( sItem.sBasicOp.wReqActPointUP && m_llActivityPoint[emType] >  sItem.sBasicOp.wReqActPointUP ) return FALSE;
    }

    return TRUE;
}

BOOL GLCharacter::CHECK_CONTRIBUTION_POINT( const SITEM& sItem )
{
	//  È°µ¿ ±â¿©µµ Ã¼Å©
	if ( sItem.sBasicOp.dwReqContribution )
	{
		if ( (DWORD)m_nContributionPoint < sItem.sBasicOp.dwReqContribution )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void GLCharacter::MsgPremiumState(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PREMIUM_STATE *pNetMsg = (GLMSG::SNETPC_PREMIUM_STATE *) pMsg;

    if ( pNetMsg->bPREMIUM==false && m_bPREMIUM==true )
    {
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PREMIUMSERVICE_END") );
    }

    m_bPREMIUM = pNetMsg->bPREMIUM;

// 	GLWidgetScript::GetInstance().LuaCallFunc< void >(
// 		NSWIDGET_SCRIPT::g_strFunc_UpdatePremium );
}

void GLCharacter::MsgGetCharedItemFromDbFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_GET_CHARGED_ITEM_FC* pNetMsg = (GLMSG::NET_GET_CHARGED_ITEM_FC*)pMsg;
    switch (pNetMsg->emFB)
    {
    case EMREQ_CHARGEDITEM_FROMDB_FB_END:	
// 		GLWidgetScript::GetInstance().LuaCallFunc< void >(
// 			NSWIDGET_SCRIPT::g_strFunc_Inventory_BankAllSlotUpdate );

		m_pGaeaClient->GetInterface()->SetItemBankInfo ();
        break;

    case EMREQ_CHARGEDITEM_FROMDB_FB_OK:
        ADDSHOPPURCHASE(pNetMsg->szPurKey, pNetMsg->nidITEM);
        break;

    case EMREQ_CHARGEDITEM_FROMDB_FB_END_FIRST_JOIN:
        break;
    }
}

void GLCharacter::MsgChargedItemGetFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CHARGED_ITEM_GET_FB *pNetNsg = (GLMSG::SNET_CHARGED_ITEM_GET_FB *)pMsg;
    switch ( pNetNsg->emFB )
    {
    case EMCHARGED_ITEM_GET_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_FAIL") );
        break;
    case EMCHARGED_ITEM_GET_FB_OK:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_OK") );
        break;
    case EMCHARGED_ITEM_GET_FB_NOITEM:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_NOITEM") );
        break;
    case EMCHARGED_ITEM_GET_FB_NOINVEN:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_NOINVEN") );
        break;
    };
}

void GLCharacter::MsgChargedItemDel(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CHARGED_ITEM_DEL *pNetNsg = (GLMSG::SNET_CHARGED_ITEM_DEL *)pMsg;
    DELSHOPPURCHASE ( pNetNsg->dwID );

	SNATIVEID sPos( pNetNsg->dwID );
	m_pGaeaClient->GetInterface()->REFRESH_ITEMBANK();
// 	GLWidgetScript::GetInstance().LuaCallFunc< void >(
// 		NSWIDGET_SCRIPT::g_strFunc_Inventory_BankSlotUpdate,
// 		"-n, -n", sPos.wMainID, sPos.wSubID );
}

void GLCharacter::MsgPlayerKillingAdd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PLAYERKILLING_ADD *pNetMsg = (GLMSG::SNETPC_PLAYERKILLING_ADD *) pMsg;

    PLANDMANCLIENT	pLand			= m_pGaeaClient->GetActiveMap();	
    bool 			bBRIGHTEVENT    = m_pGaeaClient->IsBRIGHTEVENT();
    bool 			bSCHOOLFREEPK   = m_pGaeaClient->IsSchoolFreePk();
    bool 			bADD			= false;

    if( !( bBRIGHTEVENT || bSCHOOLFREEPK || pLand->m_bClubBattle || pLand->m_bClubDeathMatch ) )
        bADD = ADD_PLAYHOSTILE( pNetMsg->dwCharID, pNetMsg->bBAD, GLCONST_CHAR::fPK_JUSTNESS_TIME );

    // ÇÐ¿ø°£ ÀÚÀ¯ÇÇÄÉ ÁøÇàÁßÀÌ¸ç »ó´ë°¡ °°Àº ÇÐ±³ÀÏ°æ¿ì Àû´ëÇàÀ§ ´ë»óÀÚ·Î ±¸ºÐÇÏ¿© °ü¸®ÇÔ.
    if( bSCHOOLFREEPK && m_wSchool == pNetMsg->wSchoolID )
        bADD = ADD_PLAYHOSTILE( pNetMsg->dwCharID, pNetMsg->bBAD, pNetMsg->fTime );

    // ¸®¼Ò½º°¡ ¸¸µé¾î Áú¶§±îÁö ÁÖ¼® Ã³¸® 2012.4.13
	//InsertAdditionalIconTimer( SKILL_ADDITIONAL_BATTLE, pNetMsg->fTime );

    if( bADD )
    {
		if( !pNetMsg->bBAD )
		{
			if( pNetMsg->bClubBattle )
			{
				GLCharClient* pCLIENT = (GLCharClient*) m_pGaeaClient->GetCopyActor( pNetMsg->szName );
				if( pCLIENT ) 
				{
					std::string strText;
					if ( m_pMyClub->IsBattle ( pCLIENT->GETCLUBID() ) )
					{
						strText = sc::string::format( ID2GAMEINTEXT("CLUB_PLAYHOSTILE_ADD"), pCLIENT->GetClubName(), pNetMsg->szName );
						m_pGaeaClient->PrintConsoleTextDlg ( strText ) ;
					}
					else if ( m_pMyClub->IsBattleAlliance ( pCLIENT->GETALLIANCEID() ) )
					{
						strText = sc::string::format( ID2GAMEINTEXT("ALLIANCE_PLAYHOSTILE_ADD"), pCLIENT->GetClubName(), pNetMsg->szName );
						m_pGaeaClient->PrintConsoleTextDlg ( strText ) ;
					} //if..else if
				} //if
			}
			else
			{
				std::string strText = sc::string::format( ID2GAMEINTEXT("PK_PLAYHOSTILE_ADD"), pNetMsg->szName );
				m_pGaeaClient->PrintConsoleTextDlg ( strText );
			} //if..else
		} //if
    } //if
}

void GLCharacter::MsgPlayerKillingDel(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PLAYERKILLING_DEL *pNetMsg = (GLMSG::SNETPC_PLAYERKILLING_DEL *) pMsg;
    DEL_PLAYHOSTILE ( pNetMsg->dwCharID );
}


HRESULT GLCharacter::UpdateClientState ( GLMSG::SNETPC_UPDATE_STATE *pNetMsg )
{
    //pNetMsg->DECODE ( m_dwGaeaID );
    m_sHP = pNetMsg->sHP;
    m_sMP = pNetMsg->sMP;
    m_sSP = pNetMsg->sSP;

    m_fTempVarHP = 0.0f;
    m_fTempVarMP = 0.0f;
    m_fTempVarSP = 0.0f;

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );

    return S_OK;
}

// ¼­¹ö °´Ã¼¿Í µ¿ÀÏÇØ¾ßÇÑ´Ù;
void GLCharacter::UpdateActState(void)
{
	if ( IsActState( EM_ACT_EMERGENCY ) )
	{
		const float fHPRate(static_cast<float>(GETHP())/static_cast<float>(GETMAXHP()));
		if ( GLCONST_CHAR::fEmergency_CharHPR < fHPRate )
		{
			ReSetActState( EM_ACT_EMERGENCY );
		}
	}
	else
	{
		const float fHPRate(static_cast<float>(GETHP())/static_cast<float>(GETMAXHP()));
		if ( fHPRate <= GLCONST_CHAR::fEmergency_CharHPR )
		{
			SetActState( EM_ACT_EMERGENCY );
		}
	}
}

/*void GLCharacter::DisableDebuffSkillFact()
{
    for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
    {
        if (i >= SKILLFACT_SIZE || m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL())
            continue;

        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
        if ( !pSkill )
            continue;

        // ÀûÀÌ °Ç ½ºÅ³ È¿°ú¸¸ Á¦°Å
        if ( pSkill->m_sBASIC.emIMPACT_SIDE == EMIMPACT_SIDE_ENEMY  )
        {
            FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC, m_dwGaeaID, GetPosition()), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );

			m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, m_sSKILLFACT[ i ].sNATIVEID.dwID, i );
            RESETSKEFF( i );
        }
    }
}*/

void GLCharacter::ReceiveDamage(const int nDamage, DWORD dwDamageFlag, WORD wSchool, const STARGETID& sACTOR)
{
    if ( dwDamageFlag & DAMAGE_TYPE_SHOCK )	ReceiveShock();

    //	Note : È­¸é¿¡ Å¸°Ý °ª Ãâ·Â.
    D3DXVECTOR3 vPos = GetPosBodyHeight();
	//GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, nDamage, dwDamageFlag, 
	//	UI_UNDERATTACK, sACTOR, STARGETID( CROW_PC, GetGaeaID() ) );
	m_pGaeaClient->GetInterface()->SetDamage(
		vPos, nDamage, dwDamageFlag, 
		UI_UNDERATTACK );

    if ( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
    {
        // °­ÇÑÅ¸°Ý ÀÌÆåÆ®
		const D3DXVECTOR3& vPosition = GetPosition();
        D3DXVECTOR3 vDIR = sACTOR.vPos - vPosition;

        D3DXVECTOR3 vDIR_ORG(1,0,0);
        float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

        D3DXMATRIX matTrans;
        D3DXMatrixRotationY ( &matTrans, fdir_y );
        matTrans._41 = vPosition.x;
        matTrans._42 = vPosition.y + 10.0f;
        matTrans._43 = vPosition.z;

        //	Note : ÀÚ±â À§Ä¡ ÀÌÆåÆ® ¹ß»ý½ÃÅ´.
        DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR, TRUE, TRUE );
    }

    //	Note : Å¸°Ý°ª ¹Ý¿µ.
    GLCHARLOGIC::RECEIVE_DAMAGE ( nDamage );

	m_pGaeaClient->logDamageRecord(sACTOR, nDamage);

    if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
    {
		//!! SF_TODO
        if ( !m_pGaeaClient->GetInterface()->UiIsVisibleGroup( TUTORIAL_DAMAGE_ALARM ) )
            m_pGaeaClient->GetInterface()->UiShowGroupTop( TUTORIAL_DAMAGE_ALARM );
    }

    //	Note : ¹æ¾î½ºÅ³ÀÇ ÀÌÆåÆ®°¡ ÀÖÀ»¶§ ¹ßµ¿½ÃÅ´.
    SKT_EFF_HOLDOUT ( sACTOR, dwDamageFlag );

    // ÇÇ°Ý½Ã ³»±¸µµ °¨¼Ò
    // °ø°Ý »ó´ë°¡ »ç¶÷ÀÎÁö
    // ÀÏ¹Ý°ø°Ý½Ã Å¬¶óÀÌ¾ðÆ®¿¡°Ô Àü´ÞµÇ´Â ¸Þ½ÃÁö
    if( sACTOR.emCrow == CROW_PC )
        DurabilityDropHit( abs( nDamage ), TRUE );
    else
        DurabilityDropHit( abs( nDamage ), FALSE );

	// UI ¿¡ Ä³¸¯ÅÍ°¡ ÇÇÇØ¸¦ ¹Þ¾ÒÀ½À» ¾Ë¸°´Ù;
	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_ReceiveDamage_Character );
}

void GLCharacter::ReceiveHeal(const int nHeal, DWORD dwHealFlag, WORD wSchool, const STARGETID& sACTOR)
{
	//	Note : È­¸é¿¡ Å¸°Ý °ª Ãâ·Â.
	D3DXVECTOR3 vPos = GetPosBodyHeight();
	//GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, nHeal, dwHealFlag, 
		//UI_UNDERATTACK, sACTOR, STARGETID( CROW_PC, GetGaeaID() ) );
	m_pGaeaClient->GetInterface()->SetDamage(
		vPos, nHeal, dwHealFlag, 
		UI_UNDERATTACK );

	//	Note : Å¸°Ý°ª ¹Ý¿µ.
	GLCHARLOGIC::RECEIVE_HEAL(nHeal);
}

void GLCharacter::ReceiveBlocking(const STARGETID& sACTOR)
{
    D3DXVECTOR3 vPos = GetPosBodyHeight();
	//GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, 0, DAMAGE_TYPE_NONE, 
	//	UI_UNDERATTACK, sACTOR, STARGETID( CROW_PC, GetGaeaID() ) );
	m_pGaeaClient->GetInterface()->SetDamage(
		vPos, 0, DAMAGE_TYPE_NONE, 
		UI_UNDERATTACK );
}

void GLCharacter::ReceiveAVoid( const STARGETID& sACTOR )
{
    D3DXVECTOR3 vPos = GetPosBodyHeight();
	//GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, 0, DAMAGE_TYPE_NONE, 
	//UI_UNDERATTACK, sACTOR, STARGETID( CROW_PC, GetGaeaID() ) );
	m_pGaeaClient->GetInterface()->SetDamage(
		vPos, 0, DAMAGE_TYPE_NONE, 
		UI_UNDERATTACK );
}

void GLCharacter::ReceiveSwing()
{
    if (IsValidBody() && !IsCtrlBlockBody())
        m_pSkinChar->DOSHOCKMIX();
}

void GLCharacter::ReceiveShock()
{
    if (IsValidBody() && !IsCtrlBlockBody())
        TurnAction(GLAT_SHOCK);
}

// Note : bjju. »óÈ²¿¡ µû¶ó Effect¸¦ Ã³¸®ÇÏ´Â ÇÔ¼ö
void GLCharacter::FactEffectProcessing ( EMFACTEFFPROCESSING emFactEffCase )
{
    switch ( emFactEffCase )
    {
	case EMFACTEFFECTPROCESSING_DESTRUCTOR:
		{
			FACTEFF::DeleteAllEffect ( STARGETID(GetCrow(), GetGaeaID(), GetPosition()), GetSkinChar(), m_sSKILLFACT, SKILLFACT_SIZE, m_sSTATEBLOWS );

			NSEffectPlay::DeleteALL();
		}
		break;
    case EMFACTEFFECTPROCESSING_MOVEMAP:
        {
            FACTEFF::DeleteAllEffect ( STARGETID(GetCrow(), GetGaeaID(), GetPosition()), GetSkinChar(), m_sSKILLFACT, SKILLFACT_SIZE_NORMAL, m_sSTATEBLOWS );

			NSEffectPlay::DeleteALL();
        }
		break;
	case EMFACTEFFECTPROCESSING_REBIRTH:
		{
			RESET_SKILL_DATA();
			NSEffectPlay::DeleteALL();
			FACTEFF::DeleteEffect ( STARGETID(GetCrow(), GetGaeaID(), GetPosition()), GetSkinChar(), m_sSKILLFACT, SKILLFACT_SIZE_NORMAL, m_sSTATEBLOWS );

			for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
				RESETSKEFF(i);

			for (size_t i=SKILLFACT_INDEX_SYSTEM_START; i<SKILLFACT_INDEX_SYSTEM_END; ++i)
				SetSkillFact(i, TRUE);

			for (size_t i=SKILLFACT_INDEX_LUNCHBOX_START; i<SKILLFACT_INDEX_EXP_LUNCHBOX_END; ++i)
				SetSkillFact(i, TRUE);
		}
		break;

	case EMFACTEFFECTPROCESSING_DEAD:
		{
			RESET_SKILL_DATA();
			NSEffectPlay::DeleteALL();
			FACTEFF::DeleteEffect ( STARGETID(GetCrow(), GetGaeaID(), GetPosition()), GetSkinChar(), m_sSKILLFACT, SKILLFACT_SIZE_NORMAL, m_sSTATEBLOWS );

			//for (size_t i=0; i<EMBLOW_MULTI; ++i)
			///	m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;

			//RemoveSkillFactofNormalAll();
			for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
				RESETSKEFF(i);

			for (size_t i=SKILLFACT_INDEX_LUNCHBOX_START; i<SKILLFACT_INDEX_EXP_LUNCHBOX_END; ++i)
				SetSkillFact(i, FALSE);

			for (size_t i=SKILLFACT_INDEX_SYSTEM_START; i<SKILLFACT_INDEX_SYSTEM_END; ++i)
				SetSkillFact(i, FALSE);

			//m_pGaeaClient->PartyMemberRemoveAllBuff( m_dwGaeaID );
		}
		break;
    };
}

const WORD GLCharacter::FindQuickSkillSlot( const SKILLID& sSkillID )
{
	//! ¹è¿î ½ºÅ³ÀÌ ¾Æ´Ò °æ¿ì Ãë¼ÒµÊ.
	if ( !ISLEARNED_SKILL(sSkillID.sSKILLID) )
		return USHRT_MAX;

	/*WORD wTab = GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_CurrentTab );
	WORD wSlotCount = GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_TabSlotCount );

	UINT nBeginIndex = wTab * wSlotCount;
	UINT nIndex		 = nBeginIndex;

	for( UINT i=0; i<EMSKILLQUICK_SIZE; ++i, ++nIndex )
	{
		if ( EMSKILLQUICK_SIZE <= nIndex )
		{
			nIndex %= EMSKILLQUICK_SIZE;
		}
		
		if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
		{
			if ( sSkillID.sSKILLID == m_sChangeQUICK[nIndex] )
			{
				return static_cast<WORD>(nIndex);
			}
		}
		else
		{
			if ( sSkillID.sSKILLID == m_sSKILLQUICK[nIndex] )
			{
				return static_cast<WORD>(nIndex);
			}
		}
	}*/

	return USHRT_MAX;
}

void GLCharacter::ResetSelectID()
{
    if ( IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) || IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ) )
    {
        return;
    }
	m_setTAR_SELECTED.clear();
	m_sSelectID.RESET();
}

void GLCharacter::SetSelectID( const STARGETID& sTarget, const bool bResetSelected )
{
    STARGETID sTargetID = sTarget;
    if ( IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
    {
        sTargetID = m_sTaunt.sTauntTargetID;
    }
	
	if ( bResetSelected )
	{
		m_setTAR_SELECTED.clear();
	}

	TARIDRC_SET_ITER finditer = m_setTAR_SELECTED.find( sTargetID );

	if ( finditer != m_setTAR_SELECTED.end() )
	{
		STARGETIDREFCOUNTER& sTargetID = (*finditer);
		++sTargetID.nRefCount;
	}
	else
	{
		m_setTAR_SELECTED.insert( sTargetID );
	}

	m_sSelectID = sTargetID;

	//! ¼¿·ºÆ® Å¸°Ù ¸®½ºÆ®¸¦ ¸®¼ÂÇÏ´Â °¹¼ö (ÃÖ´ë °ü¸®°¹¼ö)
	{
		UINT nNumManaged = GLInputManager::GetInstance().GetNumManagedSelectTarget();
		
		if ( nNumManaged != 0 && nNumManaged <= static_cast<UINT>(m_setTAR_SELECTED.size()) )
		{
			m_setTAR_SELECTED.clear();
		}	
	}
}

STARGETID* GLCharacter::FindForcedTarget( VEC_SK_TAR* pDetectMap )
{
	for( unsigned int i = 0; i < pDetectMap->size(); i++ )
	{
		const STARGETID&	targetid = (*pDetectMap)[i];
		if( targetid.emCrow == CROW_PC && targetid.GaeaId != m_dwGaeaID )
		{
			GLCharClient*	pTarget = (GLCharClient*)m_pGaeaClient->GetCopyActor( targetid );
			if ( !pTarget )
				continue;

			if( pTarget->IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
				return &(*pDetectMap)[i];

			if( pTarget->IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ))
				return &(*pDetectMap)[i];
		} //if
	} //for

	return NULL;
} //GLCharacter::FindForcedTarget

const STARGETID GLCharacter::GetSelectForcedTargetAround( const UINT nDegree, const float fRange )
{
	VEC_SK_TAR*	pDetectMap = m_pGaeaClient->DetectCrowAll( 0xFFFFFFFF, GetPosition(), fRange );
	if ( !pDetectMap )
		return TARGETID_NULL;

	for( unsigned int i = 0; i < pDetectMap->size(); i++ )
	{
		const STARGETID&	targetid = (*pDetectMap)[i];
		if( targetid.emCrow == CROW_PC && targetid.GaeaId != m_dwGaeaID )
		{
			GLCharClient*	pTarget = (GLCharClient*)m_pGaeaClient->GetCopyActor( targetid );
			if ( !pTarget )
				continue;

			if( pTarget->IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
				return (*pDetectMap)[i];

			if( pTarget->IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ) )
				return (*pDetectMap)[i];
		} //if
	} //for

	return TARGETID_NULL;
} //GLCharacter::GetSelectForcedTargetAround

BOOL GLCharacter::SelectForcedTarget()
{
	const STARGETID		selectId = GetSelectForcedTargetAround();
	if( selectId.ISNULL() )
		return FALSE;

	m_sSelectID = selectId;

	return TRUE;
} //GLCharacter::SelectForcedTarget

const STARGETID GLCharacter::GetSelectTargetAround( const EMFIND_TAR		emFindTar, 
												    const D3DXVECTOR3&		vPosition, 
												    const float				fRange, 
												    const UINT				nDegree )
{
	
	float fApplyAngle  = D3DXToRadian(static_cast<float>(nDegree)) * 0.5f;

	DWORD emCrow = NULL;

	bool bANY(false), bENEMY(false), bOUR(false), bDIE(false);

	switch ( emFindTar )
	{
	default:
	case ENFIND_TAR_NULL:
		emCrow = ( CROW_EX_PC | CROW_EX_MOB | CROW_EX_SUMMON );
		bENEMY = true;
		bANY   = true;
		bOUR   = true;
		break;
	case EMFIND_TAR_ENEMY:
		emCrow = ( CROW_EX_PC | CROW_EX_MOB | CROW_EX_SUMMON );
		bENEMY = true;
		break;
	case EMFIND_TAR_ANY:
		emCrow = ( CROW_EX_PC );
		bANY = true;
		break;
	case EMFIND_TAR_OUR:
		emCrow = ( CROW_EX_PC );
		bOUR = true;
		break;
	case EMFIND_TAR_ANY_DIE:
		emCrow = ( CROW_EX_PC );
		bANY = true;
		bDIE = true;
		break;
	case EMFIND_TAR_OUR_DIE:
		emCrow = ( CROW_EX_PC );
		bOUR = true;
		bDIE = true;
		break;
	};

	CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume ();

	VEC_SK_TAR *pDetectMap = m_pGaeaClient->DetectCrowAll ( emCrow, vPosition, fRange );
	if ( !pDetectMap )
		return TARGETID_NULL;

	//! ¸÷Å¸ÀÔ,°Å¸® ¼ø¼­´ë·Î »Ì¾Æ³½´Ù
	std::sort ( pDetectMap->begin(), pDetectMap->end(), std_afunc::CCompareSelectTargetDist(m_setTAR_SELECTED, vPosition) );

	VEC_SK_ITER iter		= pDetectMap->begin();
	VEC_SK_ITER iterEnd  = pDetectMap->end();

	for ( ; iter!=iterEnd; ++iter )
	{
		STARGETID& sTarget = (*iter);
		if ( sTarget.ISNULL() )
			continue;

		////! ¼±ÅÃµÈ Å¸°Ù ¸®½ºÆ®¸¦ °Ë»çÇÑ´Ù.
		//TARID_SET_ITER finditer = m_setTAR_SELECTED.find(sTarget);

		//if ( finditer != m_setTAR_SELECTED.end() )
		//{
		//	continue;
		//}

		//! »çÁ¤°Å¸®¿¡ µéÁö ¸øÇÏ¸é Á¦¿Ü
		if ( nDegree < 360 )
		{
			D3DXVECTOR3 vTarDir = sTarget.vPos - vPosition;
			float fDir = DXGetThetaYFromDirection ( vTarDir, GetDirect() );

			if ( fApplyAngle < fabs(fDir) )
				continue;
		}

		ClientActor* pTarget = m_pGaeaClient->GetCopyActor( sTarget );
		if ( !pTarget )
			continue;

		//! Ä«¸Þ¶ó¿¡ º¸ÀÌ´ÂÁö
		if ( !COLLISION::IsCollisionVolume ( CV, 
			D3DXVECTOR3(pTarget->GetPosition() + pTarget->GetMaxOrg()), 
			D3DXVECTOR3(pTarget->GetPosition() + pTarget->GetMinOrg()) ) )
		{
			continue;
		}

		//! Á×Àº Å¸°ÙÀ» Ã¼Å©ÇÒÁö
		if ( bDIE != (pTarget->IsValidBody() ? false : true) )
			continue;

		//! ÇÃ·¹ÀÌ¾îÀÎÁö Ã¼Å©
		if ( (bANY || bOUR) && (sTarget.emCrow == CROW_PC) )
			continue;

		//! ÆÄÆ¼ÀÎÁö Ã¼Å©
		if ( bOUR && (m_pGaeaClient->GetMyPartyClient()->isMember(sTarget.GaeaId) == true) )
			continue;

        if ( bANY && !IsReActionable(sTarget, EMIMPACT_SIDE_ANYBODY) )
            continue;

        if ( bENEMY && !IsReActionable(sTarget, EMIMPACT_SIDE_ENEMY) )
            continue;
        
        if ( bOUR && !IsReActionable(sTarget, EMIMPACT_SIDE_OUR) )
            continue;

		return sTarget;
	}

	return TARGETID_NULL;
}

//! Ä³¸¯ÅÍ ÁÖº¯ÀÇ Å¸°ÙÀ» °¡Á®¿Â´Ù
// °¢µµ´Â ÇöÀç È­¸éÀÌ ¹Ù¶óº¸´Â ¹æÇâÀ» ±âÁØÀ¸·Î °Ë»çÇÑ´Ù
const STARGETID GLCharacter::GetSelectTargetBodyAround( const EMFIND_TAR emFindTar, const float fRange, const UINT nDegree )
{
	return GetSelectTargetAround( emFindTar, GetPosition(), fRange, nDegree );
}

//! ¸¶¿ì½º ÁÖº¯ÀÇ Å¸°ÙÀ» °¡Á®¿Â´Ù
const STARGETID	GLCharacter::GetSelectTargetMouseAround( const EMFIND_TAR emFindTar, const float fRange )
{
	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt();
	if ( !DxViewPort::GetInstance().GetMouseTargetPosWnd(vTargetPt) )
		return TARGETID_NULL;
	
	//! Ä¿¼­¿¡ ¿Ã·ÁÁø Ä³¸¯ÅÍ°¡ ÀÖ´Ù¸é ±× Å¸°ÙÀ» ±âÁØÀ¸·Î °Ë»ç
	STARGETID& sCursorID = GetCursorID();
	if ( !sCursorID.ISNULL() )
		return GetSelectTargetAround( emFindTar, sCursorID.vPos, fRange, 360 );

	//! Ä³¸¯ÅÍ°¡ ¼­ÀÖ´Â Æò¸éÀ» ±âÁØÀ¸·Î ÇÈÅ· À§Ä¡¸¦ °¡Á®¿Â´Ù
	D3DXVECTOR3 vOrgPos = GetPosition();
	D3DXVECTOR3 vPos	= vOrgPos;

	D3DXPLANE plCollision;
	D3DXVECTOR3 vCollision;
	D3DXPlaneFromPointNormal( &plCollision, &vOrgPos, &D3DXVECTOR3(0.0f, 1.0f, 0.0f) );

	if ( D3DXPlaneIntersectLine(&vCollision, &plCollision, &vFromPt, &vTargetPt) )
		return GetSelectTargetAround( emFindTar, vCollision, fRange, 360 );
	
	return TARGETID_NULL;
}

void GLCharacter::OffLine()
{
	m_pGaeaClient->GetTexasHoldemManager()->Offline();
}

void GLCharacter::GetItemCoolDownTime( const SNATIVEID& sID, float& fCoolDownTime, float& fCoolDownRatio )
{
	if( false == IsCoolTime( sID ) )
		return;

	const ITEM_COOLTIME sItemCoolTime = GetCoolTime( sID );
	if( true == sItemCoolTime.IsZeroTime() )
		return;

	__time64_t tCurTime = m_pGaeaClient->GetCurrentTime().GetTime();
	__time64_t tUseTime = sItemCoolTime.tUseTime;
	__time64_t tCoolTime = sItemCoolTime.tCoolTime;

	float fCoolTime = static_cast< float >( tCoolTime - tUseTime );

	fCoolDownTime = fCoolTime - (tCurTime - tUseTime);
	if( fCoolTime == 0.0f )
		fCoolTime = 1.0f;

	fCoolDownRatio = 1.0f - (fCoolDownTime / fCoolTime);
}

const bool GLCharacter::IsExistReservationAction(EMREACTION emFind)
{
	return m_ReservationAction.IsExistReaction(emFind);
}

const BOOL GLCharacter::IsNonTargetable( ) const
{
	if( m_sHALLUCINATE.bNonTargetable || RF_FEIGN_DEATH(m_EffSkillVarSet).IsSelectDisable() )
		return TRUE;

	return FALSE;
}

void GLCharacter::SummonStateVisibleUpdate(void)
{
	bool bHire(m_sBasicSummonID != SNATIVEID(false));
	// UI on/off;
// 	GLWidgetScript::GetInstance().LuaCallFunc<void>( 
// 		NSWIDGET_SCRIPT::g_strFunc_SetVisibleSummonState
// 		, "-b, -b", m_sSummonable.bSummonable, bHire );
}

//LuaTable GLCharacter::GetSummonHPInfo(void)
//{
//	LuaTable tbHPInfo( GLWidgetScript::GetInstance().GetLuaState() );
//
//	// ÀúÀåµÈ °í¿ë ¼ÒÈ¯¼ö Á¤º¸;
//	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sBasicSummonID );
//	// ½ÇÁ¦ ¼ÒÈ¯µÈ ¼ÒÈ¯¼ö Á¤º¸;
//	std::tr1::shared_ptr< GLSummonClient > spSummon =
//		m_pGaeaClient->GetSummon(m_sSummonable.hireInfo.dwGaeaID);
//	if ( spSummon )
//	{
//		tbHPInfo.set( 1, spSummon->GETHP() );
//		tbHPInfo.set( 2, spSummon->GETMAXHP() );
//	}
//	return tbHPInfo;
//}

const char* GLCharacter::GetSummonName(void)
{
	// ÀúÀåµÈ °í¿ë ¼ÒÈ¯¼ö Á¤º¸;
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sBasicSummonID );
	if ( pCrowData )
		return pCrowData->GetName();

	return NULL;
}

void GLCharacter::UpdateSkillEff( float fTime, float fElapsedTime )
{
	/// º¯Àå ///////////////////////////////////////////////////
	BOOL bOn   = RF_DISGUISE( m_EffSkillVarSet ).IsOn();
	UINT Index = RF_DISGUISE( m_EffSkillVarSet ).GetIndex();

	SITEMCUSTOM sLHandItem = m_PutOnItems[ SLOT_LHAND ];
	SITEM* pItem = GLItemMan::GetInstance().GetItem( sLHandItem.GetRealNativeID().dwID );
	if ( pItem )
	{
		// Table ID °¡ °°À» °æ¿ì¿¡¸¸ Index¸¦ º¯°æÇÑ´Ù;
		// µ¥ÀÌÅÍ¸¦ ´Ã¸®Áö ¾Ê±â À§ÇÏ¿© Pet Á¤º¸¸¦ ÀÌ¿ëÇÑ´Ù;
		if ( RF_DISGUISE( m_EffSkillVarSet ).GetTableID() == pItem->sPet.sPetID.Mid() )
			Index = pItem->sPet.sPetID.Sid();
	}

	if( m_pTransformSet->AutoResetDisting( Index, bOn ) ) {
		Stop_Disguise();
	}
	else {
		Play_Disguise( Index, bOn );
	}
	/// ½ºÅ©¸° È¿°ú ///////////////////////////////////////////
	for( int i = 0; i < GLCONST_CHAR::SCREEN_EFFECT_NUM; i++ )
	{
		if( m_SkillSTrow.AutoReset(i, RF_SCREEN_THROW( m_EffSkillVarSet ).IsOn(i)) ) {
			Stop_ScreenThrow(i);
		}
		else {			
			Play_ScreenThrow( i, RF_SCREEN_THROW( m_EffSkillVarSet ).GetTargetID( i ) );
		}
	}
	/// Ä«¸Þ¶ó Ä¡±â ////////////////////////////////////////////
	WORD wEffNum = RF_CAMERA_HIT( m_EffSkillVarSet ).GetEffNum();

	if( m_SkillCHit.AutoReset( wEffNum, RF_CAMERA_HIT( m_EffSkillVarSet ).IsOn()) ) {
		Stop_CameraHit( wEffNum );
	}
	else {
		Play_CameraHit( wEffNum );
	}
	/// µµ¹ß ////////////////////////////////////////////
	if( RF_PROVOKE( m_EffSkillVarSet ).IsOn() ) {
		const STARGETID& sID = RF_PROVOKE( m_EffSkillVarSet ).GetHostID();

		if( !sID.ISNULL() && sID != m_sSelectID ) SetSelectID( sID );

		UpdateSkillRandomBuild( fTime, fElapsedTime );
	}
}

BOOL GLCharacter::IsSelectTargetLock()
{
	if( IsCheckedSkillFlagSpecial( EMSPECA_SCREEN_THROW ) ) return TRUE;

	return FALSE;
}

const BOOL GLCharacter::IsReActionableOver( const STARGETID &sTargetID, const EMIMPACT_SIDE emTargetSide, const bool _bForceAction)
{
    ClientActor *pTARGET = m_pGaeaClient->GetCopyActor(sTargetID);
    GLLandManClient* const pLandMan = m_pGaeaClient->GetLandManClient();
    if ( pLandMan == NULL || pTARGET == NULL )
        return FALSE;

    Faction::ManagerClient* const pFactionMan = pLandMan->GetFactionManager();
    if ( pFactionMan )
    {
        const Faction::EMRELATION_TYPE _relation = pFactionMan->getRelationClient(this, pTARGET);
        if ( _relation != Faction::EMRELATION_ERROR )
        {
            switch ( emTargetSide )
            {
            case EMIMPACT_SIDE_ENEMY:  // Àû¿¡°Ô : Àû´ëÀûÀÎ ´ë»ó¿¡°Ô »óÈ£ÀÛ¿ëÀÏ¶§ true;
                if (_relation == Faction::EMRELATION_ENEMY )
                    return TRUE;

                if ( ( _relation == Faction::EMRELATION_NEUTRAL_ENEMY ) && ( _bForceAction ) )  // Àû´ëÀûÀÎ Áß¸³°ü°èÀÇ ´ë»ó¿¡°Ô´Â °­Á¦°ø°ÝÀ¸·Î °ø°Ý °¡´É;
                    return TRUE;

                //	´ë·ÃÀÚ °ø°Ý °¡´É.;
                if( ISCONFRONT_TAR( sTargetID ) )
                    return true;

                // Àû´ëÇàÀ§ÀÚ °ø°Ý °¡´É;
                if( IS_PLAYHOSTILE( pTARGET->GetCharID() ) )
                    return true;

                return false;

            case EMIMPACT_SIDE_PARTY:  // ÆÄÆ¼¿¡°Ô : ÆÄÆ¼¿øÀÎ ´ë»ó¿¡°Ô »óÈ£ÀÛ¿ëÀÏ¶§ true;
			case EMIMPACT_SIDE_OUR:

				// ÆÑ¼ÇÀ¸·Î ÆÇº°ÇÑ´Ù;
				if ( _relation == Faction::EMRELATION_ALLY )
					return TRUE;

				return FALSE;

				// ÆÄÆ¼¿øÀ¸·Î ÆÇº°ÇÏÁö ¾Ê´Â´Ù;
                /*if ( m_pGaeaClient->GetMyPartyClient()->isMember(sTargetID.GaeaId) )
                    return TRUE;*/

            case EMIMPACT_SIDE_ANYBODY: // ¸ðµÎ¿¡°Ô : ¸ðµç ´ë»ó¿¡°Ô true;
                //ÄÚµå°¡ ÀÚ½ÅÀ» ¼ÓÀÏÁö¶óµµ Çò°¥¸®Áö ¸»ÀÚ.. EMIMPACT_SIDE_ANYBODY´Â ¸ðµç´ë»óÀÌ ¾Æ´Ï´Ù.. ÇÃ·¹ÀÌ¾î¿Í NPC¸¸ÀÌ´Ù....;
                if ( ( pTARGET->GetCrow() == CROW_PC ) || ( pTARGET->GetCrow() == CROW_NPC ) )
                    return TRUE;

                return false;

            default:
                break;
            }
        }
    }

    return IsReActionableNonFaction(sTargetID, emTargetSide, _bForceAction);
} 
