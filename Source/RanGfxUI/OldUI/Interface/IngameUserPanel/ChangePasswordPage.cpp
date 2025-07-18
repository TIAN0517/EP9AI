#include "StdAfx.h"
#include "ChangePasswordPage.h"
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

int	CChangePasswordPage::nLIMIT_OPW = 12;
int	CChangePasswordPage::nLIMIT_NPW = 12;
int	CChangePasswordPage::nLIMIT_PW2 = 12;
int	CChangePasswordPage::nLIMIT_RPW2 = 12;
int	CChangePasswordPage::nLIMIT_CP = 7;

CChangePasswordPage::CChangePasswordPage (GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIOuterWindow(pInterface, pEngineDevice)
	, m_pGaeaClient(pGaeaClient)
	, m_pInterface ( pInterface )
	,  m_pRandTextBox(NULL)
	, m_nRandPassNumber(0)
	, m_nRPUpdateCnt(0)
{
	memset( m_szRandomString, 0, sizeof( m_szRandomString ) );
}

CChangePasswordPage::~CChangePasswordPage ()
{
}

void CChangePasswordPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	const int nBUTTONSIZE = CBasicTextButton::SIZE18;

	CBasicTextBox* pTextBox = NULL;
	pTextBox = CreateStaticControl ( "CHANGEPASS_PAGE_OPW", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "CHANGEPASS_PAGE_INFO", 0 ) );
	pTextBox = CreateStaticControl ( "CHANGEPASS_PAGE_PW2", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "CHANGEPASS_PAGE_INFO", 1 ) );
	pTextBox = CreateStaticControl ( "CHANGEPASS_PAGE_NPW", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "CHANGEPASS_PAGE_INFO", 2 ) );
	pTextBox = CreateStaticControl ( "CHANGEPASS_PAGE_RPW2", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );
	pTextBox->SetOneLineText ( (char*)ID2GAMEWORD ( "CHANGEPASS_PAGE_INFO", 3 ) );


	m_pRandTextBox = CreateStaticControl ( "CHANGEPASS_PAGE_CP", pFont9, NS_UITEXTCOLOR::WHITE, TEXT_ALIGN_LEFT );

	
	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "CHANGEPASS_PAGE_OPW_BACK" );
		RegisterControl ( pBasicLineBox );

		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "CHANGEPASS_PAGE_NPW_BACK" );
		RegisterControl ( pBasicLineBox );

		
		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "CHANGEPASS_PAGE_PW2_BACK" );
		RegisterControl ( pBasicLineBox );

		
		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "CHANGEPASS_PAGE_RPW2_BACK" );
		RegisterControl ( pBasicLineBox );

		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxOuter ( "CHANGEPASS_PAGE_CP_BACK" );
		RegisterControl ( pBasicLineBox );

	}

	{
		CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
		pEditBoxMan->CreateSub ( this, "CHANGEPASS_EDITMAN", UI_FLAG_DEFAULT, CHANGEPASS_EDIT_MAN );
		pEditBoxMan->CreateEditBox ( CHANGEPASS_EDIT_OPW, "CHANGEPASS_EDIT_OPW", "CHANGEPASS_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_OPW );
		pEditBoxMan->CreateEditBox ( CHANGEPASS_EDIT_PW2, "CHANGEPASS_EDIT_PW2", "CHANGEPASS_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_PW2 );
		pEditBoxMan->CreateEditBox ( CHANGEPASS_EDIT_NPW, "CHANGEPASS_EDIT_NPW", "CHANGEPASS_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_NPW );
		pEditBoxMan->CreateEditBox ( CHANGEPASS_EDIT_RPW2, "CHANGEPASS_EDIT_RPW2", "CHANGEPASS_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_RPW2 );

		pEditBoxMan->CreateEditBox ( CHANGEPASS_EDIT_CP, "CHANGEPASS_EDIT_RP", "CHANGEPASS_CARRAT", TRUE, UINT_MAX, pFont9, nLIMIT_CP );

		pEditBoxMan->SetHide ( CHANGEPASS_EDIT_OPW, TRUE );
		pEditBoxMan->SetHide ( CHANGEPASS_EDIT_NPW, TRUE );
		pEditBoxMan->SetHide ( CHANGEPASS_EDIT_PW2, TRUE );
		pEditBoxMan->SetHide ( CHANGEPASS_EDIT_RPW2, TRUE );
		RegisterControl( pEditBoxMan );
		m_pEditBoxMan = pEditBoxMan;
	}

	CBasicTextButton* pOKButton = new CBasicTextButton(m_pEngineDevice);
	pOKButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, CHANGEPASS_OK );
	pOKButton->CreateBaseButton ( "CHANGEPASS_PAGE_OK", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CHANGEPASS_PAGE_OKCANCEL", 0 ) );
	pOKButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	RegisterControl ( pOKButton );

	CBasicTextButton* pQuitButton = new CBasicTextButton(m_pEngineDevice);
	pQuitButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, CHANGEPASS_CANCEL );
	pQuitButton->CreateBaseButton ( "CHANGEPASS_PAGE_CANCEL", nBUTTONSIZE, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD ( "CHANGEPASS_PAGE_OKCANCEL", 1 ) );
	pQuitButton->SetShortcutKey ( DIK_ESCAPE );
	RegisterControl ( pQuitButton );

}	

void CChangePasswordPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	CNetClient* pNetClient =m_pGaeaClient->GetGlobalStage()->GetNetClient ();
	if ( !pNetClient->IsOnline() )		//네트워크 연결이 끊어진 경우
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

void CChangePasswordPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case CHANGEPASS_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제
				
				CString strOPW = m_pEditBoxMan->GetEditString ( CHANGEPASS_EDIT_OPW );
				CString strPW2 = m_pEditBoxMan->GetEditString ( CHANGEPASS_EDIT_PW2 );
				CString strPW = m_pEditBoxMan->GetEditString ( CHANGEPASS_EDIT_NPW );
				CString strNPW2 = m_pEditBoxMan->GetEditString ( CHANGEPASS_EDIT_RPW2 );
				CString strRP;
				strRP = m_pEditBoxMan->GetEditString ( CHANGEPASS_EDIT_CP );


				if ( !strOPW.GetLength () || !strPW.GetLength () || !strPW2.GetLength () || !strNPW2.GetLength() )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CPSTAGE_4" ) );
					return ;
				}
				
				if( !strRP.GetLength () )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CPSTAGE_6" ) );
					return ;
				}

				if ( !CheckString ( strOPW ) )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CHANGEPASS_PAGE_OPW_ERROR" ) );
					return ;
				}

				if ( !CheckString ( strPW2 ) )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CHANGEPASS_PAGE_PW2_ERROR" ) );
					return ;
				}	

				if ( !CheckString ( strPW ) )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CHANGEPASS_PAGE_NPW_ERROR" ) );
					return ;
				}

				if ( !CheckString ( strNPW2 ) )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CHANGEPASS_PAGE_NPW2_ERROR" ) );
					return ;
				}		

				if ( strPW.GetLength () < 3 || strNPW2.GetLength () < 3 || strPW2.GetLength () < 3 )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "Password Length must not be less than 3" ) );
					return ;
				}

				if ( !CheckString ( strRP ) )
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CHANGEPASS_PAGE_CP_ERROR" ) );
					return ;
				}
				CNetClient* pNetClient =m_pGaeaClient->GetGlobalStage()->GetNetClient ();
				
				CString strCheck;
				strCheck.Format("%s",m_szRandomString);
				if ( strCheck == strRP )
				{
					 pNetClient->SndChangePassword( strOPW, strPW, strPW2, strRP );									  m_pInterface->DoModal ( ID2GAMEEXTEXT ("CPSTAGE_7") );
				}
				else
				{
					 m_pInterface->DoModal ( ID2GAMEEXTEXT ("CHANGEPASS_PAGE_CP_ERROR"), UI::MODAL_INFOMATION, UI::OK, OUTER_MODAL_PASSTOLOGIN );
				}
			}
		}
		break;

	case CHANGEPASS_CANCEL:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				/*CNetClient* pNetClient =m_pGaeaClient->GetGlobalStage()->GetNetClient();
				if( pNetClient->IsOnline() == true )
				{
					pNetClient->CloseConnect();
					m_pInterface->SetCancelToLogin();
				}

				m_pInterface->ToSelectServerPage ( GetWndID () );*/
				
				m_pInterface->UiHideGroup ( CHANGEPASS_PAGE, true );
			}
		}
		break;
	};
}
BOOL CChangePasswordPage::CheckEmail( CString strTemp )
{	
	strTemp = strTemp.Trim();

	// 문자열 체크 - 들어가면 안되는 특수문자 : ~!@#$%^&*+|":?><\=`',.;[]{}()
	if( !STRUTIL::CheckString_Special2( strTemp.GetString() ) )
	{
		 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CPSTAGE_5" ), UI::MODAL_INFOMATION, UI::OK );		
		return FALSE;
	}

    return TRUE;
}

BOOL CChangePasswordPage::CheckString( CString strTemp )
{	
	strTemp = strTemp.Trim();

	// 문자열 체크 - 들어가면 안되는 특수문자 : ~!@#$%^&*+|":?><\=`',.;[]{}()
	if( STRUTIL::CheckString( strTemp ) )
	{
		 m_pInterface->DoModal ( ID2GAMEEXTEXT ( "CPSTAGE_4" ), UI::MODAL_INFOMATION, UI::OK );		
		return FALSE;
	}

    return TRUE;
}

void CChangePasswordPage::ResetAll ()
{
	m_pEditBoxMan->EndEdit ();

	m_pEditBoxMan->ClearEdit ( CHANGEPASS_EDIT_OPW );
	m_pEditBoxMan->ClearEdit ( CHANGEPASS_EDIT_NPW );
	m_pEditBoxMan->ClearEdit ( CHANGEPASS_EDIT_PW2 );
	m_pEditBoxMan->ClearEdit ( CHANGEPASS_EDIT_RPW2 );

	m_pEditBoxMan->ClearEdit ( CHANGEPASS_EDIT_CP );

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

void CChangePasswordPage::SetVisibleSingle ( BOOL bVisible )
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

CBasicButton* CChangePasswordPage::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );

	return pButton;
}

void CChangePasswordPage::SetCharToEditBox( TCHAR cKey )
{
	if( !m_pEditBoxMan ) return;
	
	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	strTemp += cKey;

	m_pEditBoxMan->SetEditString( nID, strTemp );
}

void CChangePasswordPage::DelCharToEditBox()
{
	if( !m_pEditBoxMan ) return;

	CString strTemp;

	UIGUID nID = m_pEditBoxMan->GetBeginEditBox();

	strTemp = m_pEditBoxMan->GetEditString( nID );
	INT nLenth = strTemp.GetLength();
	strTemp = strTemp.Left( nLenth - 1 );

	m_pEditBoxMan->SetEditString( nID, strTemp );
}

void CChangePasswordPage::GoNextTab()
{
	if( m_pEditBoxMan )
		m_pEditBoxMan->GoNextTab();
}