#include "pch.h"
#include "UIEditBoxMan.h"
#include "UIEditBox.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const	int	CUIEditBoxMan::nDEFAULT_TABINDEX = 1;

CUIEditBoxMan::CUIEditBoxMan (EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
	, m_nCurTabIndex ( nDEFAULT_TABINDEX )
    , m_pEditBox ( NULL )
{
}

CUIEditBoxMan::~CUIEditBoxMan ()
{
}

void CUIEditBoxMan::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
	{
		const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
		CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
		CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();

		for( ; citer != citer_end; ++citer )
		{
			CUIEditBox * pEditBox(NULL);
			pEditBox = (CUIEditBox*)(*citer);

			if( pEditBox && pEditBox->IsBegin() )
			{
				EndEdit();

				// 에디트 박스가 포커스를 잃을 때 실행해야 할 함수 실행;
				for( unsigned int i=0; i<m_confirmFuncs.size(); ++i )
				{
					FunctionV1 fnConfirm = m_confirmFuncs.at( i );
					if( fnConfirm )
						fnConfirm( pEditBox->GetWndID() );
				}
			}
		}

		//CDebugSet::ToView ( 1, 19, "%d", ControlID );
		CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( ControlID );
		if ( !pEditBox )
		{
			GASSERT ( 0 && "컨트롤을 찾을 수 없습니다." );
			return ;
		}

		m_nCurTabIndex = pEditBox->GetTabIndex ();

		//EndEdit ();
		SetEditBox ( pEditBox );
		BeginEdit ();
	}
	else if( CHECK_LB_DOWN_LIKE( dwMsg ) || 
		CHECK_RB_DOWN_LIKE( dwMsg ) ||
		( DxInputDevice::GetInstance().GetKeyState( DIK_ESCAPE ) & DXKEY_UP) )
	{
		CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( ControlID );
		if ( !pEditBox )
		{
			GASSERT ( 0 && "컨트롤을 찾을 수 없습니다." );
			return ;
		}
		if (pEditBox->IsBegin())
		{
			EndEdit ();

			// 에디트 박스가 포커스를 잃을 때 실행해야 할 함수 실행;
			for( unsigned int i=0; i<m_confirmFuncs.size(); ++i )
			{
				FunctionV1 fnConfirm = m_confirmFuncs.at( i );
				if( fnConfirm )
					fnConfirm( ControlID );
			}
		}
	}
}

BOOL CUIEditBoxMan::RegisterControl ( CUIControl* pNewControl )
{
	return CUIGroup::RegisterControl ( pNewControl );
}

void CUIEditBoxMan::CreateEditBox ( UIGUID WndID,const char* szEditBox,const char* szCarrat,
	BOOL bCarratUseRender, DWORD dwCarratDiffuse, CD3DFontPar* pFont, int nLIMITCHAR, BOOL bUseOnlyNumber, BOOL bUseOnlyDecimalNumber )
{
	int nCount = m_ControlContainer.size ();

	CUIEditBox* pEditBox = new CUIEditBox(m_pEngineDevice);
	pEditBox->CreateSub ( this, szEditBox, UI_FLAG_DEFAULT, WndID );
	//CString strLineBox,strLineBoxOver;
	//strLineBox.Format ( "%s_BACK",szEditBox );
	//strLineBoxOver.Format ( "%s_OVER",strLineBox );
	//pEditBox->CreateLineBoxEdit ( (char*)strLineBox.GetString(), (char*)strLineBoxOver.GetString() );
	pEditBox->CreateCarrat ( szCarrat, bCarratUseRender, dwCarratDiffuse );
	pEditBox->SetFont ( pFont );
	pEditBox->SetTabIndex ( nDEFAULT_TABINDEX + nCount );
	pEditBox->SetLimitInput ( nLIMITCHAR );
	if ( bUseOnlyNumber && bUseOnlyDecimalNumber )
	{
		bUseOnlyNumber = FALSE;
	}
	pEditBox->DoUSE_NUMBER( getBool(bUseOnlyNumber) );
	pEditBox->DoUSE_DECIMALNUMBER( getBool(bUseOnlyDecimalNumber) );
	RegisterControl ( pEditBox );	
}

void CUIEditBoxMan::AddEditBox( CUIEditBox* pEditBox )
{
	int nCount = m_ControlContainer.size();
	pEditBox->SetTabIndex( nDEFAULT_TABINDEX + nCount );
	RegisterControl( pEditBox );
}

void CUIEditBoxMan::StartEDIT( const UIGUID& cID )
{
	CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( cID );
	if ( pEditBox )
	{
		m_nCurTabIndex = pEditBox->GetTabIndex ();

		EndEdit ();
		SetEditBox ( pEditBox );
		BeginEdit ();
	}
}

void CUIEditBoxMan::Init ()
{
	m_nCurTabIndex = nDEFAULT_TABINDEX;
	CUIControlContainer::UICONTROL_LIST_CITER citer = m_ControlContainer.GetControl ().begin ();
	CUIControl* pControl = (*citer);	//	HEAD임
	SetEditBox ( (CUIEditBox*) pControl );
//	EndEdit ();
//	BeginEdit ();
}

void CUIEditBoxMan::BeginEdit ()
{
	//if ( !m_pEditBox )
	//{
	//	GASSERT ( 0 && "에디트 컨트롤을 사용중이지 않습니다." );
	//	return ;
	//}

	if( m_pEditBox )
		m_pEditBox->BeginEdit ();
}

void CUIEditBoxMan::EndEdit ()
{
	//if ( !m_pEditBox )
	//{
	//	GASSERT ( 0 && "에디트 컨트롤을 사용중이지 않습니다." );
	//	return ;
	//}

	if( m_pEditBox )
		m_pEditBox->EndEdit ();
}

CString	CUIEditBoxMan::GetEditString()
{
	if (m_pEditBox)
    {
        return m_pEditBox->GetEditString ();
    }
    else
	{
		//GASSERT(0 && "에디트 컨트롤을 사용중이지 않습니다.");
		return CString("");
	}
}

int CUIEditBoxMan::GetEditLength ()
{
	if (m_pEditBox)
	{
        return m_pEditBox->GetEditLength();
    }
    else
    {
		//GASSERT ( 0 && "에디트 컨트롤을 사용중이지 않습니다." );
		return 0;
	}
}

UIGUID CUIEditBoxMan::GetBeginEditBox()
{
	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();

	for( ; citer != citer_end; ++citer )
	{
		CUIEditBox * pEditBox(NULL);
		pEditBox = (CUIEditBox*)(*citer);

		if( pEditBox && pEditBox->IsBegin() )
		{
			return pEditBox->GetWndID();
		}
	}

	return NO_ID;
}

void CUIEditBoxMan::SetVisibleEdit ( const UIGUID& cID, const BOOL& bVisible )
{
	CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( cID );
	if ( pEditBox ) pEditBox->SetVisibleSingle ( bVisible );	
}

CUIEditBox*	CUIEditBoxMan::GetEditBox ( const UIGUID& cID )
{
	CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( cID );
	return pEditBox;
}

BOOL CUIEditBoxMan::FindTabIndexControl ()
{
	CUIEditBox* pEditBox = NULL;
	bool bNativeMode = m_pEditBox->IsMODE_NATIVE();

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();

	bool bRESET_EVENT = false;
	bool bRESET = false;
	for ( ; citer != citer_end || bRESET; ++citer )
	{
		if ( bRESET && !bRESET_EVENT )
		{
			citer = list.begin ();
			bRESET = false;
			bRESET_EVENT = true;
		}

		pEditBox = (CUIEditBox*) (*citer);
		if ( !pEditBox )
		{
			GASSERT ( 0 && "리스트 노드에 널이 포함되어 있습니다." );
			return FALSE;
		}

		if ( !pEditBox->IsVisible () )
		{
			m_nCurTabIndex++;	
			int nCount = m_ControlContainer.size ();	
			if ( nCount < m_nCurTabIndex )
			{
				m_nCurTabIndex = nDEFAULT_TABINDEX;
				bRESET = true;
			}

			continue;
		}

		if ( pEditBox->GetTabIndex () == m_nCurTabIndex )
		{
			if( bNativeMode != pEditBox->IsMODE_NATIVE() )
			{
				pEditBox->DoMODE_TOGGLE();
			}

			SetEditBox ( pEditBox );			
			return TRUE;
		}
	}

	GASSERT ( 0 && "에디트 컨트롤을 발견하지 못했습니다." );
	return FALSE;
}

void CUIEditBoxMan::GoNextTab ()
{
	m_nCurTabIndex++;	
	int nCount = m_ControlContainer.size ();	
	if ( nCount < 2 ) return ;	//	컨트롤이 1개일때는 아무런 처리가 필요없다.
	if ( nCount < m_nCurTabIndex )	m_nCurTabIndex = nDEFAULT_TABINDEX;

	//	구 컨트롤, 에디트 해제	
	EndEdit ();	

	//	새 컨트롤, 에디트 지정
	if ( !FindTabIndexControl () )
	{		
		return ;
	}
	BeginEdit ();
}

void CUIEditBoxMan::GoThisTab ()
{
	if ( !FindTabIndexControl () )
	{		
		return ;
	}
	BeginEdit ();
}

CString CUIEditBoxMan::GetEditString ( UIGUID ControlID )
{
	CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( ControlID );
	if ( pEditBox ) return pEditBox->GetEditString ();
	return NULL;
}

int CUIEditBoxMan::GetEditLength ( UIGUID ControlID )
{
	CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( ControlID );
	if ( pEditBox ) return pEditBox->GetEditLength ();
	return 0;
}

void	CUIEditBoxMan::ClearEdit ( UIGUID ControlID )
{
	CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( ControlID );
	if ( pEditBox ) return pEditBox->ClearEdit ();
	return ;
}

void CUIEditBoxMan::SetHide ( UIGUID ControlID, BOOL bHide )
{
	CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( ControlID );
	if ( pEditBox ) return pEditBox->SetHide ( bHide );
	return ;
}

BOOL CUIEditBoxMan::IsHide ( UIGUID ControlID )
{
	CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( ControlID );
	if ( pEditBox ) return pEditBox->IsHide ();
	return FALSE;
}

void CUIEditBoxMan::SetEditString ( const CString& strInput )
{
	if ( m_pEditBox )
		m_pEditBox->SetEditString( strInput );
}

void CUIEditBoxMan::SetEditString ( UIGUID ControlID, const CString& strInput )
{
	CUIEditBox* pEditBox = (CUIEditBox*) FindControl ( ControlID );
	if ( pEditBox ) return pEditBox->SetEditString ( strInput );
}

void CUIEditBoxMan::DoMODE_TOGGLE ()
{
	if( m_pEditBox ) m_pEditBox->DoMODE_TOGGLE();
}

bool CUIEditBoxMan::IsMODE_NATIVE ()
{
	GASSERT( m_pEditBox );
	return m_pEditBox->IsMODE_NATIVE();
}

void CUIEditBoxMan::DisableKeyInput()
{
	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();

	for( ; citer != citer_end; ++citer )
	{
		CUIEditBox * pEditBox(NULL);
		pEditBox = (CUIEditBox*)(*citer);

		if( pEditBox )
		{
			pEditBox->DisableKeyInput();
		}
	}
}

void CUIEditBoxMan::UsableKeyInput()
{
	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();

	for( ; citer != citer_end; ++citer )
	{
		CUIEditBox * pEditBox(NULL);
		pEditBox = (CUIEditBox*)(*citer);

		if( pEditBox )
		{
			pEditBox->UsableKeyInput();
		}
	}
}

int CUIEditBoxMan::GetLimitInput( UIGUID ControlID )
{
    CUIEditBox* pEditBox = (CUIEditBox*)FindControl( ControlID );
    if ( pEditBox )
        return pEditBox->GetLimitInput();

    return 0;
}

void CUIEditBoxMan::AddConfirmFunc( FunctionV1 fnConfirm )
{
	m_confirmFuncs.push_back( fnConfirm );
}