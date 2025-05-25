// UserSearch.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "UserSearch.h"
#include "../MainFrm.h"

#include "../../RanLogicServer/DxServerInstance.h"
#include "../DoCharEdit/PropertySheetCharEdit.h"
#include "../SubDialogs/DlgMakeCronCharacter.h"
#include "../SubDialogs/DlgUserEdit.h"



// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CUserSearch

IMPLEMENT_DYNCREATE(CUserSearch, CFormView)

CUserSearch::CUserSearch()
	: CFormView(CUserSearch::IDD)
	, m_nRadio(CUserSearch::SEARCH_USERNAME)
	, m_bInit(FALSE)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CUserSearch::~CUserSearch()
{
}

void CUserSearch::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SEARCH, m_ListSearch);
	DDX_Control(pDX, IDC_EDIT1, m_EditSearch);
}

BEGIN_MESSAGE_MAP(CUserSearch, CFormView)
	ON_BN_CLICKED(IDC_SEARCH, OnBnClickedSearch)
	ON_BN_CLICKED(IDC_RADIO_USERNAME, OnBnClickedRadioUserName)
	ON_BN_CLICKED(IDC_RADIO_USERNUM, OnBnClickedRadioUsernum)
	ON_BN_CLICKED(IDC_RADIO_IP, OnBnClickedRadioIP)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SEARCH, OnLvnItemchangedListSearch)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SEARCH, OnNMDblclkListSearch)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SEARCH, OnNMRclickListSearch)
	ON_COMMAND(ID_CONTEXT1_MAKECRONCHARACTER, OnContext1Makecroncharacter)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_SEARCH, OnNMCustomdrawListSearch)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CUserSearch �����Դϴ�.

#ifdef _DEBUG
void CUserSearch::AssertValid() const
{
	CFormView::AssertValid();
}

void CUserSearch::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CUserSearch �޽��� ó�����Դϴ�.
void CUserSearch::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

	// cfg �ε强��, db ���Ἲ��, �α��� ����
	if (pFrame->m_bCfgLoad && pFrame->m_bDBConnect && pFrame->m_bLogin)
	{
		// ����Ʈ ��Ʈ�� �ʱ�ȭ
		m_ListSearch.SetExtendedStyle (LVS_EX_FULLROWSELECT);
        InitListHead();	
		m_bInit = TRUE;

		CButton* pButton = (CButton*) GetDlgItem(IDC_RADIO_USERNAME);
		pButton->SetCheck(1);
		m_nRadio = CUserSearch::SEARCH_USERNAME;

		UpdateData(FALSE);
	}
	else
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE);		
	} 
}

void CUserSearch::SearchUserByIP(std::string strIP)
{
	int nRetCode;

	m_vUserInfo.clear();

	sc::string::replaceForDb(strIP);

	std::string Query;

	Query = sc::string::format("\
							   BEGIN \
							   SELECT UserNum, UserID, UserLoginState, UserAvailable, CreateDate, LastLoginDate, UserType \
							   FROM GSUserInfo WHERE UserNum IN ( SELECT UserNum FROM GSLogLogin where LogIpAddress LIKE '%%%1%%%' ) \
							   ORDER BY UserNum desc \
							   END",
							   strIP);

	nRetCode = m_pDBMan->SearchUssr(Query, m_vUserInfo);

	if (nRetCode == DB_ERROR) 
	{
		MessageBox(_T("Database Error"));
	}
	else
	{
		if (m_vUserInfo.size() == 0)
		{
			MessageBox(_T("No Result"));		
		}
		else
		{
			FillData();
		}
		m_strSearch = Query.c_str();
	}	
}


void CUserSearch::SearchUserName(const CString& strUserName)
{
    int nRetCode;
		
    m_vUserInfo.clear();

    std::string UserName(strUserName.GetString());
    sc::string::replaceForDb(UserName);

    std::string Query;

    Query = sc::string::format("\
BEGIN \
    SELECT UserNum, UserID, UserLoginState, UserAvailable, CreateDate, LastLoginDate, UserType \
	FROM GSUserInfo WHERE UserID LIKE '%%%1%%%' \
    ORDER BY UserNum desc \
END",
        UserName);

    nRetCode = m_pDBMan->SearchUssr(Query, m_vUserInfo);

    if (nRetCode == DB_ERROR) 
	{
        MessageBox(_T("Database Error"));
	}
    else
	{
		if (m_vUserInfo.size() == 0)
		{
			MessageBox(_T("No Result"));		
		}
		else
		{
			FillData();
		}
		m_strSearch = Query.c_str();
	}	
}

void CUserSearch::SearchUserNum(int nUserNum)
{
	if (nUserNum == 0)
	{
		MessageBox(_T("Input must be numeric type"));
		return;
	}

    int nRetCode;
		
    m_vUserInfo.clear();

    std::strstream strTemp;
	strTemp << " SELECT UserNum, UserID, UserLoginState, UserAvailable, CreateDate, LastLoginDate, UserType ";
	strTemp << " From GSUserInfo WHERE UserNum=" << nUserNum;
	strTemp << '\0';

    nRetCode = m_pDBMan->SearchUssr(strTemp, m_vUserInfo);

    if (nRetCode == DB_ERROR) 
	{
        MessageBox(_T("Database Error"));
	}
    else
	{
        if (m_vUserInfo.size() == 0)
		{
			MessageBox(_T("No Result"));		
		}
		else
		{
			FillData();
		}
		m_strSearch = strTemp.str();
	}

	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
}

// ����Ʈ ��Ʈ�� ��� �ʱ�ȭ
void CUserSearch::InitListHead() 
{
	LVCOLUMN Col;

	// Num, Name, Class, Tribe, Level,	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= "User Num";
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(0, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 120;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= _T("User Name");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(1, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 120;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Online");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(2, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 30;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Status");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(3, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 120;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Created");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(4, &Col);
	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 120;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Last Login");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(5, &Col);

	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 60;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= _T("Type");
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListSearch.InsertColumn(6, &Col);
}

void CUserSearch::FillData()
{
	// Erase all list items
	m_ListSearch.DeleteAllItems();
	
	for (int i=0; i<(int) m_vUserInfo.size(); i++)
	{		
        // UserNum, UserID, UserLoginState, UserAvailable, CreateDate, LastLoginDate, UserType
		int nCount; 
		CString strTemp;
		nCount = m_ListSearch.GetItemCount();

		LV_ITEM lvItem;
		::memset(&lvItem, 0, sizeof(LV_ITEM));

		lvItem.mask = LVIF_TEXT; // �Ӽ� ����
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;
		strTemp.Format("%d", m_vUserInfo[i].UserNum);
		lvItem.pszText = strTemp.GetBuffer();		
		m_ListSearch.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.

        strTemp.Format("%s", m_vUserInfo[i].UserID);
		m_ListSearch.SetItemText(i, 1, strTemp);       

		if (m_vUserInfo[i].UserLoginState == 1)
			m_ListSearch.SetItemText(i, 2, _T("Online"));
		else
			m_ListSearch.SetItemText(i, 2, _T("Offline"));

		if (m_vUserInfo[i].UserAvailable == 1)
			m_ListSearch.SetItemText(i, 3, _T("Available"));
		else
			m_ListSearch.SetItemText(i, 3, _T("Disabled"));

		strTemp.Format("%s", m_vUserInfo[i].CreateDate);
		m_ListSearch.SetItemText(i, 4, strTemp);   

		strTemp.Format("%s", m_vUserInfo[i].LastLoginDate);
		m_ListSearch.SetItemText(i, 5, strTemp);   

		if (m_vUserInfo[i].UserType == 1)
			m_ListSearch.SetItemText(i, 6, _T("Normal"));
		else
			m_ListSearch.SetItemText(i, 6, _T("GM"));

	}
}

void CUserSearch::OnBnClickedSearch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

    CString strSearch;
    m_EditSearch.GetWindowText(strSearch);

	if (strSearch.GetLength() < 1)
	{
		MessageBox(_T("Please type search string"));
		return;
	}

    int nUserNum;
    int nChaNum;

    switch (m_nRadio)
    {
    case SEARCH_USERNAME:        
        SearchUserName(strSearch);
        break;
    case SEARCH_USERNUM:
		nUserNum = ::atoi(strSearch.GetString());         
        SearchUserNum(nUserNum);
        break;
	case SEARCH_USERIP:
		SearchUserByIP(strSearch.GetString());
		break;
    default:
        break;
    }
}
void CUserSearch::OnBnClickedRadioUserName()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    m_nRadio = CUserSearch::SEARCH_USERNAME;
}

void CUserSearch::OnBnClickedRadioUsernum()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    m_nRadio = CUserSearch::SEARCH_USERNUM;
}

void CUserSearch::OnBnClickedRadioIP()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_nRadio = CUserSearch::SEARCH_USERIP;
}

void CUserSearch::OnLvnItemchangedListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void CUserSearch::OnNMDblclkListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{	
    int nSelected = m_ListSearch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // ���õ� �׸��� ����
	{
        //CDlgUserEdit dlg(m_vUserInfo[nSelected].ChaNum, m_vUserInfo[nSelected].UserNum, _T("User Edit"));
        //dlg.DoModal();

		//CDlgUserEdit dlg(this);
		//dlg.DoModal();

		CDlgUserEdit* pdlg;
		pdlg = new CDlgUserEdit( 1, m_vUserInfo[nSelected].UserNum, m_vUserInfo[nSelected].UserID.GetString(), this );
		pdlg->Create( CWnd::GetDesktopWindow() );
	}

    *pResult = 0;
}

void CUserSearch::OnNMRclickListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// ���õ� ĳ���Ͱ� �ִ��� Ȯ���Ѵ�.
	int nSelected = m_ListSearch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // ���õ� ĳ���Ͱ� ����
	{	
		// ���콺 ��ǥ ���
		POINT point;
		GetCursorPos(&point);

		// �޴� ǥ��
		CMenu menuTemp, *pContextMenu;
		menuTemp.LoadMenu(IDR_CONTEXT_MAIN);
		pContextMenu = menuTemp.GetSubMenu(0);
		pContextMenu->TrackPopupMenu(TPM_LEFTALIGN,		
									point.x,          // �޴��� ǥ�õ� x ��ǥ
									point.y,          // �޴��� ǥ�õ� y ��ǥ
									AfxGetMainWnd()   // WM_COMMAND �޽����� ���� ������
									);
	}
	*pResult = 0;
}

void CUserSearch::OnContext1Makecroncharacter()
{
	//// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	//CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	//if (pFrame->m_nUserLevel < USER_MASTER)
	//{
	//	MessageBox(_T("Permission Denied"));
	//	return;
	//}

	//int nSelected = m_ListSearch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	//
	//if (nSelected != -1) // ���õ� ĳ���Ͱ� ����
	//{
	//	int nChaNum = 
	//	int nUserNum = 0; // ����ڹ�ȣ
	//	CString strChaName;

	//	GLCHARLOGIC* pChar = new GLCHARLOGIC;

	//	if (m_pDBMan->GetCharacterInfo(nChaNum, m_vUserInfo[nSelected].UserNum, pChar) == DB_ERROR)
	//	{			
	//		MessageBox(_T("Database Error"));
	//	}
	//	else if (m_pDBMan->GetUserInven(pChar) == DB_ERROR)
	//	{		
	//		MessageBox(_T("Database Error"));
	//	}
	//	else
	//	{
	//		// ����ڹ�ȣ �� ĳ���� �̸��� �����Ѵ�.
	//		CDlgMakeCronCharacter dlgTemp;			
	//		if (dlgTemp.DoModal() == IDOK)
	//		{
	//			// ĳ���͸� �����Ѵ�.
	//			// ĳ���� �ʱ� ��ġ ����.
	//			nUserNum   = dlgTemp.m_nUserNum;
	//			strChaName = dlgTemp.m_strChaName;

	//			GLCHARLOGIC NewCharLogic;
	//			EMCHARINDEX emIndex = CharClassToIndex(pChar->m_emClass);
	//			NewCharLogic.INIT_NEW_CHAR((EMCHARINDEX)emIndex, (DWORD) nUserNum, (DWORD)m_vUserInfo[nSelected].SGNum, strChaName.GetString(), pChar->m_wSchool, pChar->m_wHair, pChar->m_wFace, pChar->m_wHairColor, pChar->m_wSex);

	//			// ĳ���� ����
 //               CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd(); // ���� ��ȣ
	//			int nNewChaNum = m_pDBMan->CreateNewCharacter2(&NewCharLogic, pFrame->m_nSvrGrp);

	//			if (nNewChaNum == DB_ERROR)
	//			{
	//				MessageBox("Make character failed. check amount of user character / character name");
	//			}
	//			// ������ ĳ���Ϳ� ���� ĳ���� ����Ÿ�� �����ؼ� �����Ѵ�.
	//			pChar->m_CharDbNum = (DWORD) nNewChaNum;

	//			if (nNewChaNum > 0)
	//			{
	//				pChar->m_sSaveMapID = NATIVEID_NULL();
	//				m_pDBMan->SaveCharacterBase     (pChar);
	//				m_pDBMan->SaveCharacterInven    ( pChar, true );
	//				m_pDBMan->SaveCharacterPutOnItem( pChar, true );
	//				m_pDBMan->SaveCharacterSkill    (pChar);
	//				m_pDBMan->SaveCharacterQuest    (pChar);
	//				m_pDBMan->SaveCharacterSkillQuickSlot(pChar);
	//				m_pDBMan->SaveCharacterActionQuickSlot(pChar);
	//				m_pDBMan->SaveCharacterSkillFact(pChar);
	//				// �κ��丮 ������ ��������
	//				// m_pDBMan->SaveCharacterUserInven(pChar);

 //                   //! �⺻ ������ ���� �� �Ŀ� �߰��� ���簡 �ʿ��� �κ�
 //                   m_pDBMan->CopyCharacterProductInfo( nChaNum, nNewChaNum );

	//			}
	//		}
	//	}
	//	delete pChar;
	//}
}

void CUserSearch::OnNMCustomdrawListSearch(NMHDR *pNMHDR, LRESULT *pResult)
{
	// MFC �⺻�ڵ� ����
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	//// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//*pResult = 0;	

	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*> (pNMHDR);
    *pResult = 0;

	// First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
    if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        // This is the notification message for an item.  We'll request
        // notifications before each subitem's prepaint stage.
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
    }
    else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
    {
        // This is the prepaint stage for a subitem. Here's where we set the
        // item's text and background colors. Our return value will tell 
        // Windows to draw the subitem itself, but it will use the new colors
        // we set here.
        // The text color will cycle through red, green, and light blue.
        // The background color will be light blue for column 0, red for
        // column 1, and black for column 2.    
        COLORREF crText, crBkgnd;
        
		crText  = RGB(0, 0, 0);
		crBkgnd = RGB(255, 255, 255);

		if (pLVCD->iSubItem == 5)
		{
			int nItemMain = static_cast<int> (pLVCD->nmcd.dwItemSpec);
			if (m_ListSearch.GetItemText(nItemMain, pLVCD->iSubItem) ==	_T("Online"))
			{
				crText  = RGB(  0,   0,   0);
				crBkgnd = RGB(  0, 204, 255);				
			}
		}
		else if (pLVCD->iSubItem == 6)
		{
			int nItemMain = static_cast<int> (pLVCD->nmcd.dwItemSpec);
			if (m_ListSearch.GetItemText(nItemMain, pLVCD->iSubItem) ==	_T("Deleted"))
			{
				crText  = RGB(  0,   0,   0);				
				crBkgnd = RGB(255, 102, 102);
			}
		}
		else
		{
		}
		
		// Store the colors back in the NMLVCUSTOMDRAW struct.
		pLVCD->clrText   = crText;
		pLVCD->clrTextBk = crBkgnd;
        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;
    }
}

void CUserSearch::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CUserSearch::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (m_bInit)
	{

		int nWidth  = cx - 5;
		int nHeight = cy - 110;

		if (nWidth < 0)  cx = 0;
		if (nHeight < 0) cy = 0;

		m_ListSearch.MoveWindow(0, // rect.left,
								100, // rect.top,
								nWidth,
								nHeight);
	}
}
