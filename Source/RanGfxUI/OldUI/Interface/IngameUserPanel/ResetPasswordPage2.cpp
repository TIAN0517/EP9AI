#include "StdAfx.h"
#include "ResetPasswordPage2.h"
#include "../../../../SigmaCore/DebugSet.h"
#include "../../../../SigmaCore/String/StringFormat.h"
#include "../../../../SigmaCore/String/StringUtils.h"

#include "../../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../../EngineLib/Common/DXInputString.h"
#include "../../../../EngineLib/DxResponseMan.h"
#include "../../../../EngineLib/DxTools/RENDERPARAM.h"


#include "../../../../RanLogicClient/Network/s_NetClient.h"
#include "../../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"

#include "../../../../RanLogic/RANPARAM.h"

#include "../../../OuterInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int	CResetPasswordPage2::nLIMIT_NPW = 12;
int	CResetPasswordPage2::nLIMIT_PW2 = 12;
int	CResetPasswordPage2::nLIMIT_RPW2 = 12;
int	CResetPasswordPage2::nLIMIT_CP = 7;

CResetPasswordPage2::CResetPasswordPage2 (GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIOuterWindow(pInterface, pEngineDevice)
	, m_pGaeaClient(pGaeaClient)
	, m_pInterface ( pInterface )
	,  m_pRandTextBox(NULL)
	, m_nRandPassNumber(0)
	, m_nRPUpdateCnt(0)
{
	memset( m_szRandomString, 0, sizeof( m_szRandomString ) );
}

CResetPasswordPage2::~CResetPasswordPage2 ()
{
}

void CResetPasswordPage2::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nBUTTONSIZE = CBasicTextButton::SIZE18;

	CBasicTextBox* pTextBox = NULL;
	pTextBox = CreateStaticControl ( "RESETPASS2_PAGE_PW2", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "RESETPASS2_PAGE_INFO", 0 ) );
	pTextBox = CreateStaticControl ( "RESETPASS2_PAGE_NPW", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "RESETPASS2_PAGE_INFO", 1 ) );
	pTextBox = CreateStaticControl ( "RESETPASS2_PAGE_RPW2", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "RESETPASS2_PAGE_INFO", 2 ) );


	m_pRandTextBox = CreateStaticControl ( "RESETPASS2_PAGE_CP", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );

	
	{

		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "RESETPASS2_PAGE_PW2_BACK" );
		RegisterControl ( pBasicLineBox );

		
		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "RESETPASS2_PAGE_NPW_BACK" );
		RegisterControl ( pBasicLineBox );

		
		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "RESETPASS2_PAGE_RPW2_BACK" );
		RegisterControl ( pBasicLineBox );

		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "RESETPASS2_PAGE_CP_BACK" );
		RegisterControl ( pBasicLineBox );

	}

	{
		CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
		pEditBoxMan->CreateSub ( this, "RESETPASS2_EDITMAN", UI_FLAG_DEFAULT, RESETPASS2_EDIT_MAN );
		pEditBoxMan->CreateEditBox ( RESETPASS2_EDIT_PW2, "RESETPASS2_EDIT_PW2", "RESETPASS2_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PW2 );
		pEditBoxMan->CreateEditBox ( RESETPASS2_EDIT_NPW, "RESETPASS2_EDIT_NPW", "RESETPASS2_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_NPW );
		pEditBoxMan->CreateEditBox ( RESETPASS2_EDIT_RPW2, "RESETPASS2_EDIT_RPW2", "RESETPASS2_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_RPW2 );

		pEditBoxMan->CreateEditBox ( RESETPASS2_EDIT_CP, "RESETPASS2_EDIT_RP", "RESETPASS2_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_CP );

		pEditBoxMan->SetHide ( RESETPASS2_EDIT_NPW, TRUE );
		pEditBoxMan->SetHide ( RESETPASS2_EDIT_PW2, TRUE );
		pEditBoxMan->SetHide ( RESETPASS2_EDIT_RPW2, TRUE );
		RegisterControl( pEditBoxMan );
		m_pEditBoxMan = pEditBoxMan;
	}

	CBasicTextButton* pOKButton = new CBasicTextButton(m_pEngineDevice);
	pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, RESETPASS2_OK );
	pOKButton->CreateBaseButton ( "RESETPASS2_PAGE_OK", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "RESETPASS2_PAGE_OKCANCEL", 0 ) );
	pOKButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	RegisterControl ( pOKButton );

	CBasicTextButton* pQuitButton = new CBasicTextButton(m_pEngineDevice);
	pQuitButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, RESETPASS2_CANCEL );
	pQuitButton->CreateBaseButton ( "RESETPASS2_PAGE_CANCEL", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "RESETPASS2_PAGE_OKCANCEL", 1 ) );
	pQuitButton->SetShortcutKey ( DIK_ESCAPE );
	RegisterControl ( pQuitButton );

}	

void CResetPasswordPage2::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	CNetClient* pNetClient =m_pGaeaClient->GetGlobalStage()->GetNetClient ();
	if ( !pNetClient->IsOnline() )		//��Ʈ��ũ ������ ������ ���
	{
		if ( !m_pInterface->UiIsVisibleGroup ( MODAL_WINDOW_OUTER ) )
		{
			if( !m_pInterface->IsLoginCancel() )
			{
				 m_pInterface->DoModal ( ID2GAMEEXTEXT ("CPSTAGE_1"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_RECONNECT );
			}
		}
	}
	else
	{
		if (m_nRandPassNumber == -1 )
		{
			CString strTemp;
			CString RandomStr;
			sc::string::generateRandomString(RandomStr, static_cast<int> (7), 1, 1, 1);
			StringCchCopy( m_szRandomString, 7, RandomStr.GetString()  );
			strTemp.Format( "Captcha %s", m_szRandomString  );
			m_nRandPassNumber = 1;
			if( m_pRandTextBox ) m_pRandTextBox->SetOneLineText( strTemp );	
		}
		else
		{
			if( m_nRPUpdateCnt++ > 5 && m_nRandPassNumber == 0 )
			{
				pNetClient->SndRequestRandomKey();
				m_nRandPassNumber = -1;
				m_nRPUpdateCnt = 0;
			}
		}
	}

	//	�� �̵�
	if ( m_pEditBoxMan )
	{
		CUIControl* pParent = m_pEditBoxMan->GetTopParent ();
		if ( !pParent )	pParent = this;	//	���� ��Ŭ������ �ֻ��� ��Ʈ���� ���
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

void CResetPasswordPage2::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case RESETPASS2_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus ������ �޼��� ����
				
				CString strPW2 = m_pEditBoxMan->GetEditString ( RESETPASS2_EDIT_PW2 );
				CString strPW = m_pEditBoxMan->GetEditString ( RESETPASS2_EDIT_NPW );
				CString strNPW2 = m_pEditBoxMan->GetEditString ( RESETPASS2_EDIT_RPW2 );
				CString strRP;
				strRP = m_pEditBoxMan->GetEditString ( RESETPASS2_EDIT_CP );


				if ( !strPW.GetLength () || !strPW2.GetLength () || !strNPW2.GetLength() )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CPSTAGE_4" ) );
					return ;
				}
				
				if( !strRP.GetLength () )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CPSTAGE_6" ) );
					return ;
				}

				if ( !CheckString ( strPW2 ) )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "RESETPASS2_PAGE_PW2_ERROR" ) );
					return ;
				}

				if ( strPW.GetLength () < 3 || strNPW2.GetLength () < 3 || strPW2.GetLength () < 3 )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "Password Length must not be less than 3" ) );
					return ;
				}

				if ( !CheckString ( strPW ) )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "RESETPASS2_PAGE_NPW_ERROR" ) );
					return ;
				}

				if ( !CheckString ( strNPW2 ) )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "RESETPASS2_PAGE_NPW2_ERROR" ) );
					return ;
				}		
				

				if ( !CheckString ( strRP ) )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "RESETPASS2_PAGE_CP_ERROR" ) );
					return ;
				}
				CNetClient* pNetClient =m_pGaeaClient->GetGlobalStage()->GetNetClient ();
				
				CString strCheck;
				strCheck.Format("%s",m_szRandomString);
				if ( strCheck == strRP )
				{
					 pNetClient->SndResetPassword2( strPW, strPW2, strRP );									 
					  m_pInterface->DoModal ( ID2GAMEEXTEXT ("CPSTAGE_7") );
				}
				else
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ("RESETPASS2_PAGE_CP_ERROR"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_PASSTOLOGIN );
				}
			}
		}
		break;

	case RESETPASS2_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CNetClient* pNetClient =m_pGaeaClient->GetGlobalStage()->GetNetClient();
				if( pNetClient->IsOnline() == true )
				{
					pNetClient->CloseConnect();
					m_pInterface->SetCancelToLogin();
				}

				m_pInterface->ToSelectServerPage ( GetWndID () );
			}
		}
		break;
	};
}
BOOL CResetPasswordPage2::CheckString( CString strTemp )
{	
	strTemp = strTemp.Trim();

	// ���ڿ� üũ - ���� �ȵǴ� Ư������ : ~!@#$%^&*+|":?><\=`',.;[]{}()
	if( STRUTIL::CheckString( strTemp ) )
	{
		 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CPSTAGE_4" ), UI::MODAL_INFOMATION, UI::OK );		
		return FALSE;
	}

    return TRUE;
}

void CResetPasswordPage2::ResetAll ()
{
	m_pEditBoxMan->EndEdit ();

	m_pEditBoxMan->ClearEdit ( RESETPASS2_EDIT_NPW );
	m_pEditBoxMan->ClearEdit ( RESETPASS2_EDIT_PW2 );
	m_pEditBoxMan->ClearEdit ( RESETPASS2_EDIT_RPW2 );

	m_pEditBoxMan->ClearEdit ( RESETPASS2_EDIT_CP );

	m_nRandPassNumber = 0;
	m_szRandomString[7] = 0;
	m_nRPUpdateCnt = 0;


	m_pEditBoxMan->Init();
	m_pEditBoxMan->BeginEdit();

	if( m_pEditBoxMan->IsMODE_NATIVE() )
	{
		m_pEditBoxMan->DoMODE_TOGGLE();
	}
}

void CResetPasswordPage2::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if ( bVisible )
	{

		ResetAll();

		if( m_pRandTextBox ) m_pRandTextBox->ClearText();
	}
	else
	{
		m_pEditBoxMan->EndEdit ();

		CNetClient* pNetClient =m_pGaeaClient->GetGlobalStage()->GetNetClient();
		pNetClient->ResetRandomPassNumber();
	}
}

CBasicButton* CResetPasswordPage2::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );

	return pButton;
}

void CResetPasswordPage2::SetCharToEditBox( TCHAR cKey )
{
	if( !m_pEditBoxMan ) return;
	
	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	strTemp += cKey;

	m_pEditBoxMan->SetEditString( nID, strTemp );
}

void CResetPasswordPage2::DelCharToEditBox()
{
	if( !m_pEditBoxMan ) return;

	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	INT nLenth = strTemp.GetLength();
	strTemp = strTemp.Left( nLenth - 1 );

	m_pEditBoxMan->SetEditString( nID, strTemp );
}

void CResetPasswordPage2::GoNextTab()
{
	if( m_pEditBoxMan )
		m_pEditBoxMan->GoNextTab();
}