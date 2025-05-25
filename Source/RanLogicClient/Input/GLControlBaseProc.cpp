#include "../pch.h"

#include "../../EngineLib/DeviceMan.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/Common/DXInputString.h"
#include "../../EngineLib/Common/CommonWeb.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

#include "../../RanLogic/RANPARAM.h"
#include "../../RanGfxUI/OldUI/Interface/SkillTrayTab.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Tutorial/GLBaseTutorial.h"

#include "./GLInputManager_KeySetting.h"
#include "./GLControlBase.h"

#include "../Macro/MacroManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! Ű ������ �̹� Ű�� ��� �ߴ��� ���ߴ��� üũ�ϴ� ���� �ǹ�
void GLControlBase::Proc_QuickSlot( const SKEYCAPS& cKeyCaps, const UINT nIndex )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->ReqActionQ(static_cast<WORD>(nIndex));

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_IQS_UpdateTraySlot, "-n", nIndex );
	//m_pGaeaClient->GetInterface()->GetSkillTrayTab()->SelectSlot(nIndex);
}

void GLControlBase::Proc_SkillSlot( const SKEYCAPS& cKeyCaps, const UINT nIndex )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	WORD wTab = m_pGaeaClient->GetInterface()->GetSkillTrayTab()->GetTabIndex();//GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_CurrentTab );
	WORD wSlotCount = 10;//GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_TabSlotCount );

	WORD wSkillIndex = wTab * wSlotCount + static_cast< WORD >( nIndex );

	if ( !pCharacter->IsReqSwapArms() && (S_OK == pCharacter->ReqSkillRunSet( wSkillIndex )) )
	{	// ��ų�� ����ÿ� ���� ���⿡ �ش� ��ų�� ����� �� �ִ� ���Ⱑ 
		// �������̶�� �����Ѵ�. ��, �̹� ���� ��� ����ÿ��� ���� �ʴ´�.
		pCharacter->AutoArmSwap();
	}
}

void GLControlBase::Proc_SkillTab( const SKEYCAPS& cKeyCaps, const UINT nIndex )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_SQSGroup, "-n", nIndex );
	m_pGaeaClient->GetInterface()->SkilSlotTraySetTab(nIndex);
}

void GLControlBase::Proc_SkillTrayToggle( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_SQSTray );

	m_pGaeaClient->GetInterface()->SkilSlotTrayToggle();
}

void GLControlBase::Proc_ArmsSlotSwap( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	EMCHARCLASS emClass = pCharacter->m_emClass;
	if( emClass&GLCC_EXTREME_M || emClass&GLCC_EXTREME_W )
	{
		bool bMain = (0!=pCharacter->IsUseArmSub());

		//GLWidgetScript::GetInstance().LuaCallFunc< void >(
		//	NSWIDGET_SCRIPT::g_strFunc_SQS_ArmSlotSwap,
		//	"-b", bMain );

		m_pGaeaClient->GetCharacter()->ReqSlotChange();
	}
}

void GLControlBase::Proc_UseChatMacro(const SKEYCAPS& cKeyCaps, const UINT nIndex)
{
	// Ű ����
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	if( CHAT_MACRO_NUM <= nIndex )
		return;

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_AddChatMacro,
		//"-s", RANPARAM::ChatMacro[ nIndex ].GetBuffer() );
	m_pGaeaClient->GetInterface()->AddChatMacro(nIndex);
}

void GLControlBase::Proc_Inventory( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Inventory );
	m_pGaeaClient->GetInterface()->ToggleWindowInventory();
}

void GLControlBase::Proc_Character( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Character );
	m_pGaeaClient->GetInterface()->ToggleWindowCharacter();
}

void GLControlBase::Proc_Skill( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Skill );
	m_pGaeaClient->GetInterface()->ToggleWindowSkill();
}

void GLControlBase::Proc_Party( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Party );
	m_pGaeaClient->GetInterface()->ToggleWindowParty();
}

void GLControlBase::Proc_Quest( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
		//NSWIDGET_SCRIPT::g_strFunc_HotKey_Quest );
	m_pGaeaClient->GetInterface()->ToggleWindowQuest();
}

void GLControlBase::Proc_Club( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Club);	
	m_pGaeaClient->GetInterface()->ToggleWindowClub();
}

void GLControlBase::Proc_Friend( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Community);	
	m_pGaeaClient->GetInterface()->ToggleWindowFriend();
}

void GLControlBase::Proc_ItemBank( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Bank );
	m_pGaeaClient->GetInterface()->ToggleWindowItemBank();
}

void GLControlBase::Proc_MiniMap( const SKEYCAPS& cKeyCaps )
{
	// Ű ����
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);
	
	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_HotKey_LargeMap );
	m_pGaeaClient->GetInterface()->ToggleWindowMiniMap();
}

void GLControlBase::Proc_PointShop( const SKEYCAPS& cKeyCaps )
{
	// Ű ����
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) || GLUseFeatures::GetInstance().IsUsingPointShop() == false )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	EMSERVICE_PROVIDER sp =	m_pGaeaClient->GetServiceProvider();

	switch ( sp )
	{
	case SP_CHINA:
	case SP_THAILAND:
		{
			//!! SF_TODO
			m_pGaeaClient->GetInterface()->ToggleWindowWebItemShop();
		}
		break;

	default:
		{		
			/*GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_HotKey_PointShop );*/
			m_pGaeaClient->GetInterface()->ToggleWindowPointshop();

		}
		break;
	};
}

void GLControlBase::Proc_ChatMacro( const SKEYCAPS& cKeyCaps )
{
	// Ű ����
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( 
	//		NSWIDGET_SCRIPT::g_strFunc_HotKey_ChatMacro );
	m_pGaeaClient->GetInterface()->ToggleWindowChatMacro();
}

void GLControlBase::Proc_SNS( const SKEYCAPS& cKeyCaps )
{
	// SNS ������.

	/*
	// Ű ����
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//m_pInterface->ToggleWindowSNS();
	*/
}

void GLControlBase::Proc_Help( const SKEYCAPS& cKeyCaps )
{
	// Ű ����
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	/*GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_Help );*/
	m_pGaeaClient->GetInterface()->ToggleWindowHelp();
}

void GLControlBase::Proc_QBox( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter || (pCharacter->IsPartyMem() && FALSE == pCharacter->IsPartyMaster()) )
		return;

	/*GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_HotKey_QBox );*/
	m_pGaeaClient->GetInterface()->ToggleWindowQBox();
}

void GLControlBase::Proc_Pet( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Pet );
	m_pGaeaClient->GetInterface()->ToggleWindowPet();
}

void GLControlBase::Proc_Vehicle( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_HotKey_Vehicle );
	m_pGaeaClient->GetInterface()->ToggleWindowVehicle();
}

void GLControlBase::Proc_Product( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) || GLUseFeatures::GetInstance().IsUsingRenewProduct() == false )
	{
		return;
	}

	GASSERT(m_pGaeaClient);
	
	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Product );
	m_pGaeaClient->GetInterface()->ToggleWindowProduct();
}

void GLControlBase::Proc_SchoolRecord( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) || (GLUseFeatures::GetInstance().IsUsingActivity() == false && GLUseFeatures::GetInstance().IsUsingAttendance() == false) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_HotKey_StudentRecord );
	m_pGaeaClient->GetInterface()->ToggleWindowSchoolRecord();
}

void GLControlBase::Proc_BattleGround( const SKEYCAPS& cKeyCaps )
{
	// Ű ����

	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Competition );
	m_pGaeaClient->GetInterface()->ToggleWindowBattleGround();
}

void GLControlBase::Proc_PickupItem( const SKEYCAPS& cKeyCaps )
{
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->ReqPickupNearItem();
}

void GLControlBase::Proc_ForcedPKMode( const SKEYCAPS& cKeyCaps )
{
	GASSERT(m_pGaeaClient);

	GLCharacter*	pCharacter = m_pGaeaClient->GetCharacter();
	const bool bDominateControl = m_pKeySetting->GetKeyState( EMSHORTCUTS_DOMINATE_CONTROL ) & DXKEY_DOWNED;	
	const bool bForcePKMode = (cKeyCaps.dwKeyState & DXKEY_DOWNED ? true : false);	
	if( (bForcePKMode == true) && (bDominateControl == false) )
	{
		pCharacter->m_bForcedPKMode = true;
		STARGETID&		selectId = pCharacter->GetSelectID();
		if( selectId.ISNULL() )
			return;

		pCharacter->SelectForcedTarget();			 //����Ÿ���� ������ �� ��츸 �ش� �ƴ� �������.
	} //if
	else
		pCharacter->m_bForcedPKMode = false;
}

void GLControlBase::NonEventProc_ForcedPKMode( const SKEYCAPS& cKeyCaps )
{
	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->m_bForcedPKMode = false;
}

void GLControlBase::Proc_RunMode( const SKEYCAPS& cKeyCaps )
{
	if ( !(cKeyCaps.dwKeyState&DXKEY_UP) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->ReqToggleRun ();
}

void GLControlBase::Proc_PeaceMode( const SKEYCAPS& cKeyCaps )
{
	if ( !(cKeyCaps.dwKeyState&DXKEY_UP) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter->IsProducting() )
		pCharacter->ReqTogglePeaceMode ();
}

void GLControlBase::Proc_VehicleActive( const SKEYCAPS& cKeyCaps )
{
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	if (m_pGaeaClient->GetTradeClient()->Valid())
		return;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if (pCharacter->VehicleActiveValue() && cKeyCaps.fElapsedTime < 1.0f)
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_DELAY"));
		return;
	}

	pCharacter->ReqSetVehicle(!pCharacter->VehicleState());
}

void GLControlBase::Proc_VehicleBoost( const SKEYCAPS& cKeyCaps )
{
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter->isAction(GLAT_MOVE) )
	{
		pCharacter->UseBikeBoost();
	}
	if( !pCharacter->isAction(GLAT_MOVE) && pCharacter->VehicleState() == EMVEHICLE_PASSENGER )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BOOST_NOT_USING_ACTION_MOVE") );
	}
	else if( !pCharacter->isAction(GLAT_MOVE) &&  pCharacter->VehicleState() == EMVEHICLE_DRIVER )
	{
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BOOST_NOT_USING_ACTION_MOVE") );
	}
}

void GLControlBase::Proc_ActionCancel( const SKEYCAPS& cKeyCaps )
{	
	// Ű ����
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
	{
		return;
	}

	GASSERT(m_pGaeaClient);

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( NULL == pCharacter )
		return;

	// �޽��� �ڽ� ����
	//if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
	//	NSWIDGET_SCRIPT::g_strFunc_HotKey_CancelMsgBox ) )
	//	return;

	// ���� �����ִ� �������̽� â �ݱ�
	//if( true == GfxInterfaceBase::GetInstance()->EscWidgetClose() )
	//	return;

	// ESC �޴� ����
	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_System );
	//m_pGaeaClient->GetInterface()->ToggleWindowEscMenu();
}

void GLControlBase::Proc_FlyCameraControl( const SKEYCAPS& cKeyCaps )
{
	// Ű ����;
	if ( !(cKeyCaps.dwKeyState&DXKEY_DOWN) )
		return;

	GASSERT(m_pGaeaClient);
	
	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_HotKey_FlyCamCtrl );
	m_pGaeaClient->GetInterface()->ToggleWindowFlyCameraControl();
}

void GLControlBase::Proc_CameraMove(const SKEYCAPS& cKeyCaps, const UINT nMoveDir)
{
	DoProc_CameraMove(cKeyCaps, nMoveDir);
}

void GLControlBase::Proc_CameraRotate(const SKEYCAPS& cKeyCaps, const UINT nMoveDir)
{
	DoProc_CameraRotate(cKeyCaps, nMoveDir);
}

void GLControlBase::Proc_CameraMoveSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	DoProc_CameraMoveSpeed_Variation(cKeyCaps, nUpNDown);
}

void GLControlBase::Proc_CameraRotateSpeed_Variation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	DoProc_CameraRotateSpeed_Variation(cKeyCaps, nUpNDown);
}

void GLControlBase::Proc_SelectTarget(const SKEYCAPS& cKeyCaps)
{
	DoProc_SelectTarget(cKeyCaps);
}

void GLControlBase::Proc_CameraInterpMoveTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	DoProc_CameraInterpMoveTVariation(cKeyCaps, nUpNDown);
}

void GLControlBase::Proc_CameraInterpRotateTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	DoProc_CameraInterpRotateTVariation(cKeyCaps, nUpNDown);
}

void GLControlBase::Proc_CameraInterpClear(const SKEYCAPS& cKeyCaps)
{
	DoProc_CameraInterpClear(cKeyCaps);
}

void GLControlBase::Proc_CameraInterpTargetSelectTVariation(const SKEYCAPS& cKeyCaps, const UINT nUpNDown)
{
	DoProc_CameraInterpTargetSelectTVariation(cKeyCaps, nUpNDown);
}

void GLControlBase::Proc_CameraPlay(const SKEYCAPS& cKeyCaps)
{
	DoProc_CameraPlay(cKeyCaps);
}

void GLControlBase::Proc_CameraStop(const SKEYCAPS& cKeyCaps)
{
	DoProc_CameraStop(cKeyCaps);
}

void GLControlBase::Proc_CameraRewind(const SKEYCAPS& cKeyCaps)
{
	DoProc_CameraRewind(cKeyCaps);
}

void GLControlBase::Proc_Confirm( const SKEYCAPS& cKeyCaps )
{
	if( !( cKeyCaps.dwKeyState & DXKEY_UP ) )
		return;

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_Confirm );
}

void GLControlBase::Proc_InteractDropItem( const SKEYCAPS& cKeyCaps )
{
	//if( cKeyCaps.dwKeyState & DXKEY_DOWN )
	//	GfxInterfaceBase::GetInstance()->SetInteractDropItem( true );
	//else if( cKeyCaps.dwKeyState & DXKEY_UP )
	//	GfxInterfaceBase::GetInstance()->SetInteractDropItem( false );
}