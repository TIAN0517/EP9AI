#include "StdAfx.h"
#include "../../../SigmaCore/DebugSet.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/String/StringUtils.h"

#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/DxResponseMan.h"
#include "../../../EngineLib/DxTools/RENDERPARAM.h"


#include "../../../RanLogicClient/Network/s_NetClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../RanLogic/RANPARAM.h"

#include "../../OuterInterface.h"
#include "RegisterPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int	CRegisterPage::nLIMIT_USER		= 19;
int	CRegisterPage::nLIMIT_PASS		= 19;
int	CRegisterPage::nLIMIT_MAIL		= 100;
int	CRegisterPage::nLIMIT_SA		= 11;
int	CRegisterPage::nLIMIT_CAPTCHA	= 7;
int	CRegisterPage::nMIN_TEXT		= 4;

CRegisterPage::CRegisterPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIOuterWindow(pInterface, pEngineDevice)
	, m_pGaeaClient(pGaeaClient)
	, m_pInterface ( pInterface )
	, m_pEditBoxMan(NULL)
	, m_pInfoUser(NULL)
	, m_pInfoPass(NULL)
	, m_pInfoPass2(NULL)
	, m_pInfoPin(NULL)
	, m_pInfoPin2(NULL)
	, m_pInfoMail(NULL)
	, m_pInfoCaptcha(NULL)
	, m_pButtonPass(NULL)
	, m_pButtonPass2(NULL)
	, m_pButtonPin(NULL)
	, m_pButtonPin2(NULL)
	, m_pTextCaptcha(NULL)
	, m_pTextStatus(NULL)
	, m_bShowPass(FALSE)
	, m_bShowPass2(FALSE)
	, m_bShowPin(FALSE)
	, m_bShowPin2(FALSE)
{

}

CRegisterPage::~CRegisterPage ()
{
}

void CRegisterPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "RAN_REGISTER_PAGE_LINE_USER" );
	RegisterControl ( pBasicLineBox );

	pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "RAN_REGISTER_PAGE_LINE_PASSWORD" );
	RegisterControl ( pBasicLineBox );

	pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "RAN_REGISTER_PAGE_LINE_PASSWORD2" );
	RegisterControl ( pBasicLineBox );

	pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "RAN_REGISTER_PAGE_LINE_PIN" );
	RegisterControl ( pBasicLineBox );

	pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "RAN_REGISTER_PAGE_LINE_PIN2" );
	RegisterControl ( pBasicLineBox );

	pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "RAN_REGISTER_PAGE_LINE_MAIL" );
	RegisterControl ( pBasicLineBox );

	pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList ( "RAN_REGISTER_PAGE_LINE_CAPTCHA" );
	RegisterControl ( pBasicLineBox );


	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, "RAN_REGISTER_PAGE_STATIC_USER" );
	pStaticText->SetFont ( pFont9 );
	pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT );	
	pStaticText->AddText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 1 ), NS_UITEXTCOLOR::WHITE );
	RegisterControl ( pStaticText );

	pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, "RAN_REGISTER_PAGE_STATIC_PASSWORD" );
	pStaticText->SetFont ( pFont9 );
	pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT );	
	pStaticText->AddText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 2 ), NS_UITEXTCOLOR::WHITE );
	RegisterControl ( pStaticText );

	pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, "RAN_REGISTER_PAGE_STATIC_PASSWORD2" );
	pStaticText->SetFont ( pFont9 );
	pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT );	
	pStaticText->AddText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 3 ), NS_UITEXTCOLOR::WHITE );
	RegisterControl ( pStaticText );

	pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, "RAN_REGISTER_PAGE_STATIC_PIN" );
	pStaticText->SetFont ( pFont9 );
	pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT );	
	pStaticText->AddText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 4 ), NS_UITEXTCOLOR::WHITE );
	RegisterControl ( pStaticText );

	pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, "RAN_REGISTER_PAGE_STATIC_PIN2" );
	pStaticText->SetFont ( pFont9 );
	pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT );	
	pStaticText->AddText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 5 ), NS_UITEXTCOLOR::WHITE );
	RegisterControl ( pStaticText );

	pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, "RAN_REGISTER_PAGE_STATIC_MAIL" );
	pStaticText->SetFont ( pFont9 );
	pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT );	
	pStaticText->AddText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 6 ), NS_UITEXTCOLOR::WHITE );
	RegisterControl ( pStaticText );

	pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, "RAN_REGISTER_PAGE_STATIC_CAPTCHA" );
	pStaticText->SetFont ( pFont9 );
	pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT );	
	pStaticText->AddText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 9 ), NS_UITEXTCOLOR::WHITE );
	RegisterControl ( pStaticText );

	pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, "RAN_REGISTER_PAGE_STATIC_INFO" );
	pStaticText->SetFont ( pFont9 );
	pStaticText->SetTextAlign ( TEXT_ALIGN_LEFT );	
	pStaticText->AddText( ID2GAMEINTEXT ( "RAN_REGISTER_INFO_NOT_CASE_SENSITIVE" ), NS_UITEXTCOLOR::WHITE );
	RegisterControl ( pStaticText );


	m_pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
	m_pEditBoxMan->CreateSub ( this, "RAN_REGISTER_PAGE_EDITMAN", UI_FLAG_DEFAULT, LOBBY_REGISTER_EDIT_MAN );
	m_pEditBoxMan->CreateEditBox ( LOBBY_REGISTER_EDIT_USER, "RAN_REGISTER_PAGE_TEXT_USER", "RAN_REGISTER_PAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_USER );
	m_pEditBoxMan->CreateEditBox ( LOBBY_REGISTER_EDIT_PASS, "RAN_REGISTER_PAGE_TEXT_PASSWORD", "RAN_REGISTER_PAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PASS );
	m_pEditBoxMan->CreateEditBox ( LOBBY_REGISTER_EDIT_PASS2, "RAN_REGISTER_PAGE_TEXT_PASSWORD2", "RAN_REGISTER_PAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PASS );
	m_pEditBoxMan->CreateEditBox ( LOBBY_REGISTER_EDIT_PIN, "RAN_REGISTER_PAGE_TEXT_PIN", "RAN_REGISTER_PAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PASS );
	m_pEditBoxMan->CreateEditBox ( LOBBY_REGISTER_EDIT_PIN2, "RAN_REGISTER_PAGE_TEXT_PIN2", "RAN_REGISTER_PAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PASS );
	m_pEditBoxMan->CreateEditBox ( LOBBY_REGISTER_EDIT_MAIL, "RAN_REGISTER_PAGE_TEXT_MAIL", "RAN_REGISTER_PAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_MAIL );
	m_pEditBoxMan->CreateEditBox ( LOBBY_REGISTER_EDIT_CAPTCHA, "RAN_REGISTER_PAGE_TEXT_CAPTCHA", "RAN_REGISTER_PAGE_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_CAPTCHA );
	m_pEditBoxMan->SetHide ( LOBBY_REGISTER_EDIT_PASS, TRUE );
	m_pEditBoxMan->SetHide ( LOBBY_REGISTER_EDIT_PASS2, TRUE );
	m_pEditBoxMan->SetHide ( LOBBY_REGISTER_EDIT_PIN, TRUE );
	m_pEditBoxMan->SetHide ( LOBBY_REGISTER_EDIT_PIN2, TRUE );
	RegisterControl( m_pEditBoxMan );

	m_pButtonPass = new CBasicTextButton(m_pEngineDevice);
	m_pButtonPass->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, LOBBY_REGISTER_BUTTON_PASS );
	m_pButtonPass->CreateBaseButton ( "RAN_REGISTER_PAGE_SHOW_PASSWORD", CBasicTextButton::SIZE18, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 10 ) );
	RegisterControl ( m_pButtonPass );

	m_pButtonPass2 = new CBasicTextButton(m_pEngineDevice);
	m_pButtonPass2->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, LOBBY_REGISTER_BUTTON_PASS2 );
	m_pButtonPass2->CreateBaseButton ( "RAN_REGISTER_PAGE_SHOW_PASSWORD2", CBasicTextButton::SIZE18, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 10 ) );
	RegisterControl ( m_pButtonPass2 );

	m_pButtonPin = new CBasicTextButton(m_pEngineDevice);
	m_pButtonPin->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, LOBBY_REGISTER_BUTTON_PIN );
	m_pButtonPin->CreateBaseButton ( "RAN_REGISTER_PAGE_SHOW_PIN", CBasicTextButton::SIZE18, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 10 ) );
	RegisterControl ( m_pButtonPin );

	m_pButtonPin2 = new CBasicTextButton(m_pEngineDevice);
	m_pButtonPin2->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, LOBBY_REGISTER_BUTTON_PIN2 );
	m_pButtonPin2->CreateBaseButton ( "RAN_REGISTER_PAGE_SHOW_PIN2", CBasicTextButton::SIZE18, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 10 ) );
	RegisterControl ( m_pButtonPin2 );

	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, LOBBY_REGISTER_BUTTON_REGISTER );
	pTextButton->CreateBaseButton ( "RAN_REGISTER_PAGE_BUTTON_REGISTER", CBasicTextButton::SIZE18, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 12 ) );
	RegisterControl ( pTextButton );

	pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, LOBBY_REGISTER_BUTTON_RESET );
	pTextButton->CreateBaseButton ( "RAN_REGISTER_PAGE_BUTTON_RESET", CBasicTextButton::SIZE18, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 13 ) );
	RegisterControl ( pTextButton );

	pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, LOBBY_REGISTER_BUTTON_CANCEL );
	pTextButton->CreateBaseButton ( "RAN_REGISTER_PAGE_BUTTON_CANCEL", CBasicTextButton::SIZE18, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 14 ) );
	RegisterControl ( pTextButton );

	
	m_pTextCaptcha = new CBasicTextBox(m_pEngineDevice);
	m_pTextCaptcha->CreateSub ( this, "RAN_REGISTER_PAGE_RANDOM_CAPTCHA" );
	m_pTextCaptcha->SetFont ( pFont9 );
	m_pTextCaptcha->SetTextAlign ( TEXT_ALIGN_LEFT );	
	m_pTextCaptcha->AddText( "", NS_UITEXTCOLOR::WHITE );
	RegisterControl ( m_pTextCaptcha );

	m_pTextStatus = new CBasicTextBox(m_pEngineDevice);
	m_pTextStatus->CreateSub ( this, "RAN_REGISTER_PAGE_INFO_STATUS" );
	m_pTextStatus->SetFont ( pFont9 );
	m_pTextStatus->SetTextAlign ( TEXT_ALIGN_LEFT );	
	m_pTextStatus->AddText( "", NS_UITEXTCOLOR::WHITE );
	RegisterControl ( m_pTextStatus );


	
}	

void CRegisterPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if ( !pNetClient->IsOnline() )
	{
		if ( !m_pInterface->UiIsVisibleGroup ( MODAL_WINDOW_OUTER ) )
		{
			if( !m_pInterface->IsLoginCancel() )
			{
				m_pInterface->DoModal ( ID2GAMEEXTEXT ("REGISTERSTAGE_1"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_RECONNECT );
			}
		}
	}

	//	탭 이동
	if ( m_pEditBoxMan )
	{
		CUIControl* pParent = m_pEditBoxMan->GetTopParent ();
		if ( !pParent )	pParent = this;	//	만약 이클래스가 최상위 컨트롤인 경우
		BOOL bFocus = ( pParent->IsFocusControl() );

		if ( bFocus )
		{
			if ( UIKeyCheck::GetInstance()->Check ( DIK_TAB, DXKEY_DOWN ) )
			{
				m_pEditBoxMan->GoNextTab ();
			}
		}
	}
}

void CRegisterPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch (ControlID)
	{
	case LOBBY_REGISTER_BUTTON_REGISTER:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				DoRegister();
			}
		}break;

	case LOBBY_REGISTER_BUTTON_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				EndRegister();
			}
		}break;

	case LOBBY_REGISTER_BUTTON_RESET:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				ResetAll ();
			}
		}break;

	case LOBBY_REGISTER_BUTTON_PASS:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_bShowPass = !m_bShowPass;
				CheckHideStatusPass();
			}
		}break;
	case LOBBY_REGISTER_BUTTON_PASS2:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_bShowPass2 = !m_bShowPass2;
				CheckHideStatusPass2();
			}
		}break;
	case LOBBY_REGISTER_BUTTON_PIN:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_bShowPin = !m_bShowPin;
				CheckHideStatusPin();
			}
		}break;
	case LOBBY_REGISTER_BUTTON_PIN2:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_bShowPin2 = !m_bShowPin2;
				CheckHideStatusPin2();
			}
		}break;
	};
}

void CRegisterPage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if ( bVisible )
	{
		ResetAll();

		if ( m_pTextStatus ) m_pTextStatus->ClearText();

		if ( m_pTextCaptcha )
		{
			WORD wRandomNumber = (WORD)rand() % 32000 + 10001;
			CString strRandom;
			strRandom.Format( "%d", wRandomNumber );
			m_strCaptcha = strRandom.GetString();

			m_pTextCaptcha->ClearText();
			m_pTextCaptcha->SetText( m_strCaptcha.c_str(), NS_UITEXTCOLOR::ORNAGERED );
		}
	}
	else
	{
		if ( m_pEditBoxMan )	m_pEditBoxMan->EndEdit ();

		DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
		CNetClient* pNetClient = pGlobalStage->GetNetClient();
		pNetClient->ResetRandomPassNumber();
	}
}

void CRegisterPage::ResetAll ()
{
	m_bShowPass = FALSE;
	m_bShowPass2 = FALSE;
	m_bShowPin = FALSE;
	m_bShowPin2 = FALSE;

	CheckHideStatusPass();
	CheckHideStatusPass2();
	CheckHideStatusPin();
	CheckHideStatusPin2();

	m_pEditBoxMan->EndEdit ();
	m_pEditBoxMan->ClearEdit ( LOBBY_REGISTER_EDIT_USER );
	m_pEditBoxMan->ClearEdit ( LOBBY_REGISTER_EDIT_PASS );
	m_pEditBoxMan->ClearEdit ( LOBBY_REGISTER_EDIT_PASS2 );
	m_pEditBoxMan->ClearEdit ( LOBBY_REGISTER_EDIT_PIN );
	m_pEditBoxMan->ClearEdit ( LOBBY_REGISTER_EDIT_PIN2 );
	m_pEditBoxMan->ClearEdit ( LOBBY_REGISTER_EDIT_MAIL );
	m_pEditBoxMan->ClearEdit ( LOBBY_REGISTER_EDIT_CAPTCHA );

	m_pEditBoxMan->Init();
	m_pEditBoxMan->BeginEdit();

	if( m_pEditBoxMan->IsMODE_NATIVE() )
	{
		m_pEditBoxMan->DoMODE_TOGGLE();
	}
}

void CRegisterPage::DoRegister()
{
	if ( !m_pEditBoxMan )	return;
	if ( !m_pTextStatus )	return;

	m_pTextStatus->ClearText();
	m_pTextStatus->SetText( "Processing registration, please wait.....", NS_UITEXTCOLOR::GREENYELLOW );

	std::string struser = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_USER );
	std::string strpass = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_PASS );
	std::string strpass2 = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_PASS2 );
	std::string strpin = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_PIN );
	std::string strpin2 = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_PIN2 );
	std::string strmail = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_MAIL );
	std::string strcaptcha = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_CAPTCHA );

	if ( struser.empty() )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_USER_EMPTY" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if ( struser.size() < nMIN_TEXT || struser.size() > nLIMIT_USER )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_USER_SIZE" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if ( strpass.empty() )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_PASS_EMPTY" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if ( strpass.size() < nMIN_TEXT || strpass.size() > nLIMIT_PASS )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_PASS_SIZE" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if (  strcmp( strpass.c_str(), strpass2.c_str() ) != 0 )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_PASS_MISMATCH" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if ( strpin.empty() )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_PASS2_EMPTY" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if ( strpin.size() < nMIN_TEXT || strpin.size() > nLIMIT_PASS )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_PASS2_SIZE" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if ( strcmp( strpin.c_str(), strpin2.c_str() ) != 0 )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_PASS2_MISMATCH" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if ( strmail.empty() )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_MAIL_EMPTY" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if ( strmail.size() < nMIN_TEXT || strmail.size() > nLIMIT_MAIL )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_MAIL_SIZE" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if ( strcaptcha.empty() )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_CAPTCHA_EMPTY" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if ( strcaptcha.size() < nMIN_TEXT || strcaptcha.size() > nLIMIT_CAPTCHA )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_CAPTCHA_SIZE" ), NS_UITEXTCOLOR::RED );
		return;
	}

	if ( strcmp( strcaptcha.c_str(), m_strCaptcha.c_str() ) != 0 )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_CAPTCHA_MISMATCH" ), NS_UITEXTCOLOR::RED );
		return;
	}


	if ( !CheckString ( struser.c_str() ) )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_USER_STRING" ), NS_UITEXTCOLOR::RED );
		return ;
	}

	if ( !CheckString ( strpass.c_str() ) )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_PASS_STRING" ), NS_UITEXTCOLOR::RED );
		return ;
	}

	if ( !CheckString ( strpin.c_str() ) )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_PASS2_STRING" ), NS_UITEXTCOLOR::RED );
		return ;
	}

	if ( !CheckString ( strcaptcha.c_str() ) )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_CAPTCHA_STRING" ), NS_UITEXTCOLOR::RED );
		return ;
	}

	if ( !CheckString_Special2 ( strmail.c_str() ) )
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_MAIL_STRING" ), NS_UITEXTCOLOR::RED );
		return ;
	}

	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if ( pNetClient && pNetClient->IsOnline() )
	{
		std::string strreg_user = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_USER );
		std::string strreg_pass = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_PASS );
		std::string strreg_pass2 = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_PIN );
		std::string strreg_mail = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_MAIL );
		std::string strreg_sa = m_pEditBoxMan->GetEditString( LOBBY_REGISTER_EDIT_SA );

		std::transform ( strreg_user.begin(), strreg_user.end(), strreg_user.begin(), tolower );
		std::transform ( strreg_pass.begin(), strreg_pass.end(), strreg_pass.begin(), tolower );
		std::transform ( strreg_pass2.begin(), strreg_pass2.end(), strreg_pass2.begin(), tolower );
		std::transform ( strreg_mail.begin(), strreg_mail.end(), strreg_mail.begin(), tolower );

		int nServerGroup, nServerChannel;
		m_pInterface->GetConnectServerInfo( nServerGroup, nServerChannel );

		pNetClient->SndRegister( strreg_user.c_str(),  strreg_mail.c_str(), strreg_pass.c_str(), strreg_pass2.c_str(), strcaptcha.c_str(), nServerChannel );
	}
	else
	{
		m_pTextStatus->SetText( ID2GAMEINTEXT( "RAN_REGISTER_FB_ERROR_SERVER" ), NS_UITEXTCOLOR::RED );
		EndRegister();
	}
}

void CRegisterPage::EndRegister()
{
	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CNetClient* pNetClient = pGlobalStage->GetNetClient();
	if( pNetClient->IsOnline() == true )
	{
		pNetClient->CloseConnect();
		m_pInterface->SetCancelToLogin();
	}

	m_pInterface->ToSelectServerPage ( GetWndID () );
}

BOOL CRegisterPage::CheckString( CString strTemp )
{	
	strTemp = strTemp.Trim();
	if( STRUTIL::CheckString( strTemp ) )	return FALSE;
	return TRUE;
}

BOOL CRegisterPage::CheckString_Special2( CString strTemp )
{	
	strTemp = strTemp.Trim();
	if( STRUTIL::CheckString_Special2( strTemp ) )	return FALSE;
	return TRUE;
}

void CRegisterPage::CheckHideStatusPass()
{
	if ( m_pEditBoxMan )
	{
		m_pEditBoxMan->StartEDIT( LOBBY_REGISTER_EDIT_PASS );
		m_pEditBoxMan->SetHide ( LOBBY_REGISTER_EDIT_PASS, !m_bShowPass );
	}

	if ( m_pButtonPass )
	{
		if ( m_bShowPass )
			m_pButtonPass->SetOneLineText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 11 ) );
		else
			m_pButtonPass->SetOneLineText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 10 ) );
	}	
}

void CRegisterPage::CheckHideStatusPass2()
{
	if ( m_pEditBoxMan )
	{
		m_pEditBoxMan->StartEDIT( LOBBY_REGISTER_EDIT_PASS2 );
		m_pEditBoxMan->SetHide ( LOBBY_REGISTER_EDIT_PASS2, !m_bShowPass2 );
	}

	if ( m_pButtonPass2 )
	{
		if ( m_bShowPass2 )
			m_pButtonPass2->SetOneLineText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 11 ) );
		else
			m_pButtonPass2->SetOneLineText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 10 ) );
	}	
}

void CRegisterPage::CheckHideStatusPin()
{
	if ( m_pEditBoxMan )
	{
		m_pEditBoxMan->StartEDIT( LOBBY_REGISTER_EDIT_PIN );
		m_pEditBoxMan->SetHide ( LOBBY_REGISTER_EDIT_PIN, !m_bShowPin );
	}

	if ( m_pButtonPin )
	{
		if ( m_bShowPin )
			m_pButtonPin->SetOneLineText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 11 ) );
		else
			m_pButtonPin->SetOneLineText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 10 ) );
	}	
}

void CRegisterPage::CheckHideStatusPin2()
{
	if ( m_pEditBoxMan )
	{
		m_pEditBoxMan->StartEDIT( LOBBY_REGISTER_EDIT_PIN2 );
		m_pEditBoxMan->SetHide ( LOBBY_REGISTER_EDIT_PIN2, !m_bShowPin2 );
	}

	if ( m_pButtonPin2 )
	{
		if ( m_bShowPin2 )
			m_pButtonPin2->SetOneLineText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 11 ) );
		else
			m_pButtonPin2->SetOneLineText( ID2GAMEWORD ( "RAN_REGISTER_WINDOW", 10 ) );
	}	
}

void CRegisterPage::AddErrorString( std::string strError, DWORD dwColor )
{
	if ( strError.size() == 0 )	return;

	if ( m_pTextStatus )
	{
		m_pTextStatus->ClearText();
		m_pTextStatus->SetText( strError.c_str(), dwColor );
	}
}

void CRegisterPage::SetStatus ( int nIndex )
{
	if ( m_pTextStatus ) m_pTextStatus->SetText ( ID2GAMEEXTEXT ("EM_REGISTER_FB_SUB",nIndex), NS_UITEXTCOLOR::GREENYELLOW );
	switch ( nIndex )
	{
	case 0:
		{
			ResetAll();
			m_pInterface->DoModal( ID2GAMEEXTEXT ("EM_REGISTER_FB_SUB",0), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_WAITCONFIRM );
			m_pInterface->SetModalCallWindowID( GetWndID() );
		}
		break;
	}
}