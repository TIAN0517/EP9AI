// TexEncryptDlg.cpp : ���� ����
//

#include "stdafx.h"

#include "../SigmaCore/Compress/Zipper.h"
#include "../SigmaCore/Compress/UnZipper.h"
#include "../EngineLib/DxTools/EncryptDataDef.h"
#include "../EngineLib/DxTools/PrivateResource.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"
#include "../SigmaCore/File/FileUtil.h"

#include "TexEncrypt.h"
#include "TexEncryptDlg.h"

#include "../SigmaCore/Compress/ZipMan.h"
#include "../SigmaCore/xml/XMLEncrypt.h"

#include "../SigmaCore/Lua/lua_tinker.h"
extern "C"
{
#include "../=LuaPlus/src/LuaPlus/src/lua.h"
#include "../=LuaPlus/src/LuaPlus/src/lualib.h"
#include "../=LuaPlus/src/LuaPlus/src/lauxlib.h"
}


namespace NSWIDGET_SCRIPT_LIST
{
	const std::string g_strScriptList = "widgetscriptlist.lua";
	const std::string g_strWidgetScriptFile = "widgetscript.wsf";
	const std::string g_strWidgetScriptRCCFile = "widgetscript.rcc";
}

namespace NSWIDGETLIST
{
	static const char* g_strWidgetListFile = "widgetslist.lua";
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTexEncryptDlg ��ȭ ����



CTexEncryptDlg::CTexEncryptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTexEncryptDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTexEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPTION_COMBO, m_cCombo);
	DDX_Control(pDX, IDC_EDIT_NOTE, m_cEdit);
}

BEGIN_MESSAGE_MAP(CTexEncryptDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_OPEN_BUTTON, OnBnClickedOpenButton)
	ON_BN_CLICKED(ID_RUN_BUTTON, OnBnClickedRunButton)
	ON_CBN_SELCHANGE(IDC_OPTION_COMBO, OnCbnSelchangeOptionCombo)
END_MESSAGE_MAP()


// CTexEncryptDlg �޽��� ó����

BOOL CTexEncryptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ENCRYPT_DATA::InitKey();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.


	m_cCombo.InsertString( 0, "Encrypt Tex File" );
	m_cCombo.InsertString( 1, "Encrypt Tex Folder" );
	m_cCombo.InsertString( 2, "Decrypt Tex File" );
	m_cCombo.InsertString( 3, "Decrypt Tex Folder" );
	m_cCombo.InsertString( 4, "Encrypt Skin File" );
	m_cCombo.InsertString( 5, "Encrypt Skin Folder" );
	m_cCombo.InsertString( 6, "Decrypt Skin File" );
	m_cCombo.InsertString( 7, "Decrypt Skin Folder" );
	m_cCombo.InsertString( 8, "Encrypt Widget Script" );
	m_cCombo.InsertString( 9, "Decrypt Widget Script" );
	m_cCombo.InsertString( 10, "Luc to Lua" );
	m_cCombo.InsertString( 11, "Encrypt Files New" );
	m_cCombo.InsertString( 12, "Decrypt Files New" );
	m_cCombo.InsertString( 13, "XML Encrypt" );
	m_cCombo.InsertString( 14, "XML Decrypt" );


	m_cCombo.SetCurSel( 0 );
	m_cEdit.SetLimitText( 0 );

	m_bDelete = false;

	SetAppPath();

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CTexEncryptDlg::SetAppPath()
{
	CString strAppPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		strAppPath = strCommandLine.Left ( strCommandLine.ReverseFind ( '\\' ) );
		
		if ( !strAppPath.IsEmpty() )
		if ( strAppPath.GetAt(0) == '"' )
			strAppPath = strAppPath.Right ( strAppPath.GetLength() - 1 );

		m_strAppPath = strAppPath.GetString();
	}
	else 
	{
		MessageBox ( "SetAppPath Error", "Error", MB_OK );
		return;
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CTexEncryptDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CTexEncryptDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTexEncryptDlg::Init()
{
	SetDlgItemText( IDC_INPUT_EDIT, "" );
	SetDlgItemText( IDC_OUTPUT_EDIT, "" );
	m_cEdit.SetWindowText("");
}

void CTexEncryptDlg::OnBnClickedOpenButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	Init();


	int nSelect = m_cCombo.GetCurSel();
	if ( nSelect < ENCRYPT_TEX_FILE || nSelect >= OPTION_SIZE )	return;

	switch ( nSelect )
	{
	case ENCRYPT_TEX_FILE:		OpenEncryptTexFile();		break;
	case ENCRYPT_TEX_FOLDER:	OpenSelectFolder();			break;
	case DECRYPT_TEX_FILE:		OpenDecryptTexFile();		break;
	case DECRYPT_TEX_FOLDER:	OpenSelectFolder();			break;
	case ENCRYPT_SKIN_FILE:		OpenEncryptSkinFile();		break;
	case ENCRYPT_SKIN_FOLDER:	OpenSelectFolder();			break;
	case DECRYPT_SKIN_FILE:		OpenDecryptSkinFile();		break;
	case DECRYPT_SKIN_FOLDER:	OpenSelectFolder();			break;
	case ENCRYPT_WIDGET_SCRIPT:	OpenSelectFolder();			break;
	case DECRYPT_WIDGET_SCRIPT:	OpenSelectFolder();			break;
	case UNZIP_WIDGET_SCRIPT:	OpenSelectFolder();			break;
	case ENCRYPT_NEW_FILE:		OpenSelectFolder();			break;
	case DECRYPT_NEW_FILE:		OpenSelectFolder();			break;
	case XML_ENCRYPT:	OpenSelectFolder();			break;
	case XML_DECRYPT:	OpenSelectFolder();			break;
	};
}

void CTexEncryptDlg::OnBnClickedRunButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CString strInput; 
	CString strOutput;
	
	GetDlgItemText( IDC_INPUT_EDIT, strInput );
	GetDlgItemText( IDC_OUTPUT_EDIT, strOutput );

	if ( strInput.IsEmpty() || strOutput.IsEmpty() )	
		return;
	
	int nSelect = m_cCombo.GetCurSel();
	if ( nSelect < ENCRYPT_TEX_FILE || nSelect >= OPTION_SIZE )	
		return;	

	m_bDelete = GetWin_Check( this, IDC_CHECK_DELETE ) ? TRUE : FALSE;

	switch( nSelect )
	{
	case ENCRYPT_TEX_FILE :
		{
			if ( RunEncryptTexFile(strInput, strOutput) )
			{
				AddEditString( "Complate Encrypt Tex File" );
				AddEditString( strInput );
			}
			else
			{
				AddEditString( "Error Encrypt Tex File" );
				AddEditString( strInput );
			}			
		}
		break;
	case ENCRYPT_TEX_FOLDER :
		{
			if ( RunEncryptTexFolder(strInput, strOutput) )
			{
				AddEditString( "Complate Encrypt Tex File in Folder" );
			}
			else
			{
				AddEditString( "Complate Encrypt Tex File in Folder" );
			}
			
		}
		break;
	case DECRYPT_TEX_FILE :
		{
			if ( RunDecryptTexFile(strInput, strOutput) )
			{
				AddEditString( "Complate Decrypt Tex File" );
				AddEditString( strInput );
			}
			else
			{
				AddEditString( "Error Decrypt Tex File" );
				AddEditString( strInput );
			}
			
		}
		break;
	case DECRYPT_TEX_FOLDER :
		{
			if ( RunDecryptTexFolder(strInput, strOutput) ) 
			{
				AddEditString( "Complate Decrypt Tex File in Folder" );				
			}
			else
			{
				AddEditString( "Error Decrypt Tex File in Folder" );
			}
		}
		break;

	case ENCRYPT_SKIN_FILE:
		{
			if ( RunEncryptSkinFile(strInput, strOutput) )
			{
				AddEditString( "Complate Encrypt Skin File" );
				AddEditString( strInput );
			}
			else
			{
				AddEditString( "Error Encrypt Skin File" );
				AddEditString( strInput );
			}
		}
		break;
	case ENCRYPT_SKIN_FOLDER:
		{
			if ( RunEncryptSkinFolder(strInput, strOutput) )
			{
				AddEditString( "Complate Encrypt Skin File in Folder" );
			}
			else
			{
				AddEditString( "Complate Encrypt Skin File in Folder" );
			}
		}
		break;
	case DECRYPT_SKIN_FILE:
		{
			if ( RunDecryptSkinFile(strInput, strOutput) )
			{
				AddEditString( "Complate Decrypt Skin File" );
				AddEditString( strInput );
			}
			else
			{
				AddEditString( "Error Decrypt Skin File" );
				AddEditString( strInput );
			}
		}
		break;
	case DECRYPT_SKIN_FOLDER:
		{
			if ( RunDecryptSkinFolder(strInput, strOutput) ) 
			{
				AddEditString( "Complate Decrypt Skin File in Folder" );				
			}
			else
			{
				AddEditString( "Error Decrypt Skin File in Folder" );
			}
		}
		break;
	case ENCRYPT_WIDGET_SCRIPT:
		{
			if ( RunEncryptWidgetScriptFolder(strInput, strOutput) )
			{
				AddEditString( "Complate Decrypt WidgetScript File in Folder" );
				AddEditString( strInput );
			}
			else
			{
				AddEditString( "Error Decrypt WidgetScript File in Folder" );
				AddEditString( strInput );
			}
		}
		break;
	case DECRYPT_WIDGET_SCRIPT:
		{
			if ( RunDecryptWidgetScriptFolder(strInput, strOutput) ) 
			{
				AddEditString( "Complate Decrypt Skin File in Folder" );				
			}
			else
			{
				AddEditString( "Error Decrypt Skin File in Folder" );
			}
		}
		break;
	case UNZIP_WIDGET_SCRIPT:
		{
			if ( RunUnzipWidgetScript(strInput, strOutput) ) 
			{
				AddEditString( "Complate Unzip Widget Script File in Folder" );				
			}
			else
			{
				AddEditString( "Error Unzip Widget Script in Folder" );
			}
		}
		break;

	case ENCRYPT_NEW_FILE:
		{
			if ( RunEncryptFilesFolder(strInput, strOutput) ) 
			{
				AddEditString( "Complate Encrypt Files" );				
			}
			else
			{
				AddEditString( "Error Encrypt Files" );
			}
		}
		break;
	case DECRYPT_NEW_FILE:
		{
			if ( RunDecryptFilesFolder(strInput, strOutput) ) 
			{
				AddEditString( "Complate Decrypt Files" );				
			}
			else
			{
				AddEditString( "Error Decrypt Files" );
			}
		}
		break;
	case XML_ENCRYPT:
		{
			if ( RunEncryptXMLFolder(strInput, strOutput) ) 
			{
				AddEditString( "Complate RunEncryptXMLFolder" );				
			}
			else
			{
				AddEditString( "Error RunEncryptXMLFolder" );
			}
		}
		break;
	case XML_DECRYPT:
		{
			if ( RunDecryptXMLFolder(strInput, strOutput) ) 
			{
				AddEditString( "Complate Unzip RunDecryptXMLFolder" );				
			}
			else
			{
				AddEditString( "Error RunDecryptXMLFolder" );
			}
		}
		break;
	}
}

void CTexEncryptDlg::OpenEncryptTexFile()
{
	CString szFilterInput = "Texture File (*.dds,*.tga)|*.dds;*.tga|";

	CFileDialog dlgInput(TRUE,".dds",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilterInput,this);
	dlgInput.m_ofn.lpstrInitialDir = m_strAppPath;

	if ( dlgInput.DoModal() != IDOK ) return;
	
	CString strFileName = dlgInput.GetFileName();
	strFileName = strFileName.Left( strFileName.ReverseFind( '.' ) );

	CString szFilterOutPut = "Texture File (*.mtf)|*.mtf|";
	CFileDialog dlgOutput( FALSE,".mtf",strFileName,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilterOutPut,this );

	if ( dlgOutput.DoModal() != IDOK ) return;
		
	SetDlgItemText( IDC_INPUT_EDIT, dlgInput.GetPathName() );
	SetDlgItemText( IDC_OUTPUT_EDIT, dlgOutput.GetPathName() );
}

void CTexEncryptDlg::OpenDecryptTexFile()
{
	CString szFilterInput = "Texture File (*.mtf)|*.mtf|";
	CFileDialog dlgInput( TRUE,".mtf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilterInput,this );

	dlgInput.m_ofn.lpstrInitialDir = m_strAppPath;

	if ( dlgInput.DoModal() != IDOK ) return;
	
	CString strFileName = dlgInput.GetFileName();
	strFileName = strFileName.Left( strFileName.ReverseFind( '.' ) );

	CString szFilterOutPut = "Texture File (*.dds,*.tga)|*.dds;*.tga|";
	CFileDialog dlgOutput(FALSE,".dds",strFileName,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilterOutPut,this);			

	if ( dlgOutput.DoModal() != IDOK ) return;
		
	SetDlgItemText( IDC_INPUT_EDIT, dlgInput.GetPathName() );
	SetDlgItemText( IDC_OUTPUT_EDIT, dlgOutput.GetPathName() );
}

void CTexEncryptDlg::OpenEncryptSkinFile()
{
	CString szFilterInput = "SkinMesh File (*.x)|*.x|";

	CFileDialog dlgInput(TRUE,".x",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilterInput,this);
	dlgInput.m_ofn.lpstrInitialDir = m_strAppPath;

	if ( dlgInput.DoModal() != IDOK ) return;
	
	CString strFileName = dlgInput.GetFileName();
	strFileName = strFileName.Left( strFileName.ReverseFind( '.' ) );

	CString szFilterOutPut = "SkinMesh File (*.mxf)|*.mxf|";
	CFileDialog dlgOutput( FALSE,".mxf",strFileName,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilterOutPut,this );

	if ( dlgOutput.DoModal() != IDOK ) return;
		
	SetDlgItemText( IDC_INPUT_EDIT, dlgInput.GetPathName() );
	SetDlgItemText( IDC_OUTPUT_EDIT, dlgOutput.GetPathName() );

}

void CTexEncryptDlg::OpenDecryptSkinFile()
{
	CString szFilterInput = "SkinMesh File (*.mxf)|*.mxf|";
	CFileDialog dlgInput( TRUE,".mxf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilterInput,this );

	dlgInput.m_ofn.lpstrInitialDir = m_strAppPath;

	if ( dlgInput.DoModal() != IDOK ) return;
	
	CString strFileName = dlgInput.GetFileName();
	strFileName = strFileName.Left( strFileName.ReverseFind( '.' ) );

	CString szFilterOutPut = "SkinMesh File (*.x)|*.x|";
	CFileDialog dlgOutput(FALSE,".x",strFileName,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilterOutPut,this);			

	if ( dlgOutput.DoModal() != IDOK ) return;
		
	SetDlgItemText( IDC_INPUT_EDIT, dlgInput.GetPathName() );
	SetDlgItemText( IDC_OUTPUT_EDIT, dlgOutput.GetPathName() );

}

void CTexEncryptDlg::OpenSelectFolder()
{
	BROWSEINFO biInput;
	ZeroMemory(&biInput, sizeof(BROWSEINFO));
	biInput.hwndOwner = m_hWnd; // ���� �ڵ�
	biInput.lpszTitle = "Select Input Folder"; // ��������â �������� �������� ����
	biInput.pidlRoot = NULL;
	biInput.lpfn = BrowseCallbackProc; // �ݹ��Լ�
	biInput.lParam = (LPARAM)(LPCSTR)m_strAppPath;
	biInput.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_NONEWFOLDERBUTTON | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pidlFolderInput = SHBrowseForFolder(&biInput);
	
	if (pidlFolderInput == NULL) 
		return;

	TCHAR szPathInput[_MAX_PATH] = {0};
	SHGetPathFromIDList(pidlFolderInput, szPathInput);

	
	BROWSEINFO biOutput;
	ZeroMemory(&biOutput, sizeof(BROWSEINFO));
	biOutput.hwndOwner = m_hWnd; // ���� �ڵ�
	biOutput.lpszTitle = "Select Ouput Folder"; // ��������â �������� �������� ����
	biOutput.pidlRoot = NULL;
	biOutput.lpfn = BrowseCallbackProc; // �ݹ��Լ�
	biOutput.lParam = (LPARAM)(LPCSTR)szPathInput;
	biOutput.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_NONEWFOLDERBUTTON | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pidlFolderOutput = SHBrowseForFolder(&biOutput);
	
	if (pidlFolderOutput == NULL) return;

	TCHAR szPathOutput[_MAX_PATH] = {0};
	SHGetPathFromIDList(pidlFolderOutput, szPathOutput);

	
	SetDlgItemText( IDC_INPUT_EDIT, szPathInput );
	SetDlgItemText( IDC_OUTPUT_EDIT, szPathOutput );

}


int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM dwData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		SendMessage( hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM) dwData );
		break;
	}
	return 0;
}


void CTexEncryptDlg::OnCbnSelchangeOptionCombo()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	Init();
}

BOOL CTexEncryptDlg::RunEncryptTexFile( const CString& strInput, const CString& strOutput )
{
	CPrivateResource cResource( CPrivateResource::ETypeTexture );

	return cResource.SaveFileEnc( strInput, strOutput, m_bDelete );
}

BOOL CTexEncryptDlg::RunDecryptTexFile( const CString& strInput, const CString& strOutput )
{
	CPrivateResource cResource( CPrivateResource::ETypeTexture );

	return cResource.SaveFileDec( strInput, strOutput, m_bDelete );
}

BOOL CTexEncryptDlg::RunEncryptTexFolder( const CString& strInput, const CString& strOutput )
{
	int nCntFile=0, nCntSuccess = 0;

	AddEditString( "Run Encrypt Tex File in Folder" );
	AddEditString( strInput );

	for ( int i = 0; i < TEXTURE_TYPE; ++i )
	{
		CString strFind = strInput;
		strFind += "\\*";
		strFind += ENCRYPT_DATA::g_szTexFileExt[i];

		CString strOutFile, strOutPath;
		strOutPath = strOutput;
		strOutPath += "\\";

		CString strInFile, strInPath;
		strInPath = strInput;
		strInPath += "\\";

		CString strFile;
		
		CFileFind finder;	
		BOOL bFind ( TRUE );

		if ( !finder.FindFile( strFind ) )
		{
			continue;
		}

		CString strTemp;

		while( bFind )
		{	
			nCntFile++;

			bFind = finder.FindNextFile();
			strFile = finder.GetFileName();
			strInFile = strInPath;
			strInFile += strFile;
			strOutFile = strOutPath;
			strOutFile += strFile.Left( strFile.ReverseFind('.'));
			strOutFile += ".mtf";
			
			if ( !RunEncryptTexFile( strInFile, strOutFile ) )
			{
//				CDebugSet::ToLogFile( "Texture File Encrypt Error : %s", strInFile );
				strTemp.Format( " Error Encrypt Tex File : %s ", strFile );
				AddEditString( strTemp );
				continue;
			}
			
			strTemp.Format( " Success Encrypt Tex File : %s ", strFile );
			AddEditString( strTemp );

			nCntSuccess++;
		}
	}

	CString strResult;
	strResult.Format( "%d in %d Tex File Encrypt Success", nCntSuccess, nCntFile );
	AddEditString( strResult );

	return TRUE;
}

BOOL CTexEncryptDlg::RunDecryptTexFolder( const CString& strInput, const CString& strOutput )
{
	AddEditString( "Run Decrypt Tex File in Folder" );
	AddEditString( strInput );

	// DDS FILE ��ȯ
	CString strFind = strInput;
	strFind += "\\*.mtf";

	CString strOutFile, strOutPath;
	strOutPath = strOutput;
	strOutPath += "\\";

	CString strInFile, strInPath;
	strInPath = strInput;
	strInPath += "\\";

	CString strFile;
	
	CFileFind finder;	
	BOOL bFind ( TRUE );

	if ( !finder.FindFile( strFind ) )
	{
		bFind = FALSE;
	}

	int nCntFile =0, nCntSuccess = 0;
	CString strTemp;

	while( bFind )
	{
		nCntFile++;
		bFind = finder.FindNextFile();
		strFile = finder.GetFileName();
		strInFile = strInPath;
		strInFile += strFile;
		strOutFile = strOutPath;
		strOutFile += strFile.Left( strFile.ReverseFind('.'));
		strOutFile += ".dds";
		
		if ( !RunDecryptTexFile( strInFile, strOutFile ) )
		{
//			CDebugSet::ToLogFile( "Texture File Decrypt Error : %s", strInFile );	
			strTemp.Format( " Error Decrypt Tex File : %s ", strFile );
			AddEditString( strTemp );
			continue;
		}
		
		strTemp.Format( " Success Decrypt Tex File : %s ", strFile );
		AddEditString( strTemp );

		nCntSuccess++;
	}

	CString strResult;
	strResult.Format( "%d in %d Tex File Encrypt Success", nCntSuccess, nCntFile );
	AddEditString( strResult );

	return TRUE;
}

BOOL CTexEncryptDlg::RunEncryptSkinFile( const CString& strInput, const CString& strOutput )
{
	CPrivateResource cResource( CPrivateResource::ETypeSkin );

	return cResource.SaveFileEnc( strInput, strOutput, m_bDelete );
}

BOOL CTexEncryptDlg::RunDecryptSkinFile( const CString& strInput, const CString& strOutput )
{
	CPrivateResource cResource( CPrivateResource::ETypeSkin );

	return cResource.SaveFileDec( strInput, strOutput, m_bDelete );
}

BOOL CTexEncryptDlg::RunEncryptSkinFolder( const CString& strInput, const CString& strOutput )
{
	int nCntFile=0, nCntSuccess = 0;

	AddEditString( "Run Encrypt Skin File in Folder" );
	AddEditString( strInput );

	for ( int i = 0; i < SKINMESH_TYPE; ++i )
	{
		CString strFind = strInput;
		strFind += "\\*";
		strFind += ENCRYPT_DATA::g_szSkinFileExt[i];

		CString strOutFile, strOutPath;
		strOutPath = strOutput;
		strOutPath += "\\";

		CString strInFile, strInPath;
		strInPath = strInput;
		strInPath += "\\";

		CString strFile;
		
		CFileFind finder;	
		BOOL bFind ( TRUE );

		if ( !finder.FindFile( strFind ) )
		{
			continue;
		}

		CString strTemp;

		while( bFind )
		{	
			nCntFile++;

			bFind = finder.FindNextFile();
			strFile = finder.GetFileName();
			strInFile = strInPath;
			strInFile += strFile;
			strOutFile = strOutPath;
			strOutFile += strFile.Left( strFile.ReverseFind('.'));
			strOutFile += ".mxf";
			
			if ( !RunEncryptSkinFile( strInFile, strOutFile ) )
			{
//				CDebugSet::ToLogFile( "Texture File Encrypt Error : %s", strInFile );
				strTemp.Format( " Error Encrypt Skin File : %s ", strFile );
				AddEditString( strTemp );
				continue;
			}
			
			strTemp.Format( " Success Encrypt Skin File : %s ", strFile );
			AddEditString( strTemp );

			nCntSuccess++;
		}
	}

	CString strResult;
	strResult.Format( "%d in %d Skin File Encrypt Success", nCntSuccess, nCntFile );
	AddEditString( strResult );

	return TRUE;
}

BOOL CTexEncryptDlg::RunDecryptSkinFolder( const CString& strInput, const CString& strOutput )
{
	AddEditString( "Run Decrypt Skin File in Folder" );
	AddEditString( strInput );

	// DDS FILE ��ȯ
	CString strFind = strInput;
	strFind += "\\*.mxf";

	CString strOutFile, strOutPath;
	strOutPath = strOutput;
	strOutPath += "\\";

	CString strInFile, strInPath;
	strInPath = strInput;
	strInPath += "\\";

	CString strFile;
	
	CFileFind finder;	
	BOOL bFind ( TRUE );

	if ( !finder.FindFile( strFind ) )
	{
		bFind = FALSE;
	}

	int nCntFile =0, nCntSuccess = 0;
	CString strTemp;

	while( bFind )
	{
		nCntFile++;
		bFind = finder.FindNextFile();
		strFile = finder.GetFileName();
		strInFile = strInPath;
		strInFile += strFile;
		strOutFile = strOutPath;
		strOutFile += strFile.Left( strFile.ReverseFind('.'));
		strOutFile += ".x";
		
		if ( !RunDecryptSkinFile( strInFile, strOutFile ) )
		{
//			CDebugSet::ToLogFile( "Texture File Decrypt Error : %s", strInFile );	
			strTemp.Format( " Error Decrypt Skin File : %s ", strFile );
			AddEditString( strTemp );
			continue;
		}
		
		strTemp.Format( " Success Decrypt Skin File : %s ", strFile );
		AddEditString( strTemp );

		nCntSuccess++;
	}

	CString strResult;
	strResult.Format( "%d in %d Skin File Encrypt Success", nCntSuccess, nCntFile );
	AddEditString( strResult );

	return TRUE;
}

// lua -> luc -> rcc -> wsf
BOOL CTexEncryptDlg::RunEncryptWidgetScriptFolder( const CString& strInput, const CString& strOutput )
{
	int nCntFile=0, nCntSuccess = 0;

	AddEditString( "Run Encrypt Widget Script in Folder" );
	AddEditString( strInput );

	CString strFind = strInput;
	strFind += "\\*";
	strFind += ENCRYPT_DATA::g_szWidgetScriptExt;

	CString strOutFile, strOutPath;
	strOutPath = strOutput;
	strOutPath += "\\";

	CString strInFile, strInPath;
	strInPath = strInput;
	strInPath += "\\";

	CString strFile;
	CString strZipFullPath;

	CFileFind finder;	
	BOOL bFind ( TRUE );


	TSTRING strLuaPath = m_strAppPath.GetString();
	strLuaPath += _T("\\luaplusc.exe");
	FILE* file = _fsopen ( strLuaPath.c_str(), "rb", _SH_DENYNO );
	if ( !file )
	{
		CString strTemp;
		strTemp.Format( "Error : %s", strLuaPath.c_str() );
		AddEditString( strTemp );
		return FALSE;
	}
	fclose(file);


	if ( finder.FindFile( strFind ) )
	{
		CString strTemp;

		CZipper cZipFile;
		strZipFullPath = strInPath;
		strZipFullPath += "\\widgetscript.rcc";
		cZipFile.OpenZip( strZipFullPath, NULL, FALSE );

		while( bFind )
		{	
			nCntFile++;

			bFind = finder.FindNextFile();
			strFile = finder.GetFileName();
			strInFile = strInPath;
			strInFile += strFile;
			strOutFile = strOutPath;
			strOutFile += strFile.Left( strFile.ReverseFind('.'));
			strOutFile += ".luc";

			// lua -> luc
			if ( !RunEncryptWidgetScript( strInFile, strOutFile ) )
			{
				strTemp.Format( " Error RunEncryptWidgetScript : %s ", strFile );
				AddEditString( strTemp );

				CString strFinalWSF( strZipFullPath.Left( strZipFullPath.ReverseFind('.')) );
				strFinalWSF += ".wsf";

				// ���� ���� ����
				remove( strZipFullPath );
				remove( strFinalWSF );

				return FALSE;
			}

			CString strFileLuc( strFile.Left( strFile.ReverseFind('.')) );
			strFileLuc += ".luc";
			strFileLuc.MakeLower();

			// ���� ����� ���´�.
			HANDLE hFile = CreateFile ( strOutFile, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
			int nError = GetLastError();
			strTemp.Format( "CreateFile : %s returned %d", strFileLuc, nError );
			AddEditString( strTemp );


			// �̻��ϰ� ���а� ���� 100 �� ���� ��������.
			int nCount(0);
			while ( hFile == INVALID_HANDLE_VALUE )
			{
				++nCount;
				if ( nCount > 100 )
				{
					strTemp.Format( " Error CreateFile : %s ", strFileLuc );
					AddEditString( strTemp );

					CString strFinalWSF( strZipFullPath.Left( strZipFullPath.ReverseFind('.')) );
					strFinalWSF += ".wsf";

					// ���� ���� ����
					remove( strOutFile );
					remove( strZipFullPath );
					remove( strFinalWSF );

					return FALSE;
				}

				Sleep( 1 );
				hFile = CreateFile ( strOutFile, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );//CreateFile ( strOutFile, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL );
				nError = GetLastError();
				strTemp.Format( "CreateFile : %s returned %d", strFileLuc, nError );
				AddEditString( strTemp );
			}
			DWORD dwFileSize = GetFileSize(hFile, NULL);
			CloseHandle(hFile);


			// Password ����
			// ��ȣ ��� ��ġ�� ZipMan::UnzipToMemory
			TSTRING strPass(strFileLuc);
			for ( DWORD i=0; i<strPass.size(); ++i )
			{
				strPass[i] = strPass[i] + 0x54;
				strPass[i] = strPass[i] ^ 0xa4;
				strPass[i] = strPass[i] + 0x17;
			}
			TSTRINGSTREAM sstrPass;
			sstrPass << strPass.c_str();
			sstrPass << dwFileSize;


			// ���� ���Ͽ� �߰� ( luc -> rcc )
			cZipFile.AddFileToZip( strOutFile, false, sstrPass.str().c_str() );

			// luc ������ �����Ѵ�.
			// don't remove test for decrypting luc to lua
			//remove( strOutFile );

			strTemp.Format( " Success Encrypt Widget File : %s ", strFile );
			AddEditString( strTemp );

			nCntSuccess++;
		}
	}

	CString strResult;
	strResult.Format( "%d in %d Encrypt Widget File Success", nCntSuccess, nCntFile );
	AddEditString( strResult );


	CString strFinal;
	strFinal = strZipFullPath.Left( strZipFullPath.ReverseFind('.'));
	strFinal += ".wsf";

	// rcc -> wsf
	// rcc ������ ���쵵�� �Ѵ�.
	CPrivateResource cResource( CPrivateResource::ETypeWidgetScript );
	return cResource.SaveFileEnc( strZipFullPath, strFinal, TRUE );
}

BOOL CTexEncryptDlg::RunEncryptWidgetScript( const CString& strInput, const CString& strOutput )
{
	// lua -> luo �� ������.
	{
		TSTRING strCommand(m_strAppPath.GetString());
		strCommand += _T("\\luaplusc.exe -o ");
		strCommand += strOutput.GetString();
		strCommand += _T(" ");
		strCommand += strInput.GetString();

		if ( !Compile_lua_luc( strCommand ) )
			return FALSE;
	}

	if( m_bDelete )
		DeleteFile( strInput );

	return TRUE;
}

BOOL CTexEncryptDlg::Compile_lua_luc( const TSTRING& strCommand )
{
	DWORD dwExitCode = 0; 

	STARTUPINFO siStartupInfo; 
	PROCESS_INFORMATION piProcessInfo; 
	memset(&siStartupInfo, 0, sizeof(siStartupInfo)); 
	memset(&piProcessInfo, 0, sizeof(piProcessInfo)); 
	siStartupInfo.cb = sizeof(siStartupInfo); 

	if (CreateProcess( NULL, 
		const_cast<LPSTR>(strCommand.c_str()), 
		0, 
		0, 
		FALSE,         //pwszParam
		CREATE_NO_WINDOW, 
		0, 
		m_strAppPath.GetString(), 
		&siStartupInfo, 
		&piProcessInfo) != false) 
	{ 
		/* Watch the process. */ 
		dwExitCode = WaitForSingleObject( piProcessInfo.hProcess, 100 ); 
	}

	CloseHandle(piProcessInfo.hProcess); 
	CloseHandle(piProcessInfo.hThread);

	return TRUE;
}

// lua <- luc <- rcc <- wsf ( vice versa )
BOOL CTexEncryptDlg::RunDecryptWidgetScriptFolder( const CString& strInput, const CString& strOutput )
{
	m_pLuaState = NULL;
	if ( m_pLuaState != NULL )
	{
		lua_close( m_pLuaState );
		m_pLuaState = NULL;
	}

	m_pLuaState = lua_open();
	luaL_openlibs( m_pLuaState );

	CString strOutFile, strOutPath;
	strOutPath = strOutput;
	strOutPath += "\\";

	if ( m_pLuaState == NULL )
	{
		AddEditString( "!m_pLuaState" );
		return FALSE;
	}

	int nCntFile=0, nCntSuccess = 0;

	AddEditString( "Run Decrypt Widget Script in Folder" );
	AddEditString( strInput );

	CString strFind = strInput;
	
	std::string strPathWSF( strInput );;
	strPathWSF += "\\";
	strPathWSF.append( NSWIDGET_SCRIPT_LIST::g_strWidgetScriptFile );
	
	FILE* pFile(NULL);
	std::string strPathTemp( strPathWSF );
	strPathTemp.append( _T(".rcc") );

	BOOL bReturn(TRUE);
	CPrivateResource cResource( CPrivateResource::ETypeWidgetScript );
	return cResource.SaveFileDec( strPathWSF.c_str(), strPathTemp.c_str(), FALSE );
	//if ( pFile = cResource.LoadFile_Decrypt_NewFile( strPathWSF.c_str(), strPathTemp.c_str() ) )
	//{
	//	
	//	if ( !ZipMan::GetInstance().LOAD_Memory( strPathTemp, pFile ) )
	//	{
	//		bReturn = FALSE;
	//		AddEditString( strPathTemp.c_str() );
	//	}

	//	BYTE* pByte(NULL);
	//	std::string strLucFile( sc::file::changeFileExt( NSWIDGETLIST::g_strWidgetListFile, "luc" ) );
	//	std::transform( strLucFile.begin(), strLucFile.end(), strLucFile.begin(), tolower );
	//	UINT size = ZipMan::GetInstance().UnzipToMemory(strPathTemp, strLucFile, pByte);
	//	if ( size <= 0 || pByte == NULL )
	//	{
	//		return FALSE;
	//	}

	//	if( false == lua_tinker::dobuffer( m_pLuaState, reinterpret_cast<char*>( pByte ), size ) )
	//	{
	//		return FALSE;
	//	}

	//	{
	//		
	//	pByte = NULL;
	//	// ��ũ��Ʈ ����Ʈ �ε�
	//	std::string strLucFile( sc::file::changeFileExt( NSWIDGET_SCRIPT_LIST::g_strScriptList, "luc" ) );
	//	std::transform( strLucFile.begin(), strLucFile.end(), strLucFile.begin(), tolower );
	//	UINT size = ZipMan::GetInstance().UnzipToMemory(strPathTemp, strLucFile, pByte);
	//	if ( size <= 0 || pByte == NULL )
	//	{
	//		return FALSE;
	//	}

	//	if( false == lua_tinker::dobuffer( m_pLuaState, reinterpret_cast<char*>( pByte ), size ) )
	//	{
	//		return FALSE;
	//	}

	//	// ��ũ��Ʈ ����Ʈ���� ������ ���Ǵ� ��ũ��Ʈ�� �����´�.
	//	lua_tinker::table luaScriptList = lua_tinker::get< lua_tinker::table >( m_pLuaState, "WIDGET_SCRIPT_LIST" );
	//	for( int i = 1; i <= luaScriptList.TableLen(); ++i )
	//	{
	//		strLucFile = sc::file::changeFileExt( std::string(luaScriptList.get< const char* >( i )), "luc" );
	//		std::transform( strLucFile.begin(), strLucFile.end(), strLucFile.begin(), tolower );
	//		size = ZipMan::GetInstance().UnzipToMemory(strPathTemp, strLucFile, pByte);
	//		if ( size <= 0 || pByte == NULL )
	//		{
	//			return FALSE;
	//		}

	//		if( false == lua_tinker::dobuffer( m_pLuaState, reinterpret_cast<char*>( pByte ), size ) )
	//		{
	//			return FALSE;
	//		}
	//		CString strLuc = strLucFile.c_str();
	//		AddEditString( strLucFile.c_str() );

	//		strOutFile = strOutPath;
	//		strOutFile += strLuc.Left( strLuc.ReverseFind('.'));
	//		strOutFile += ".lua";
	//		strOutFile.MakeLower();

	//		if ( RunDecryptWidgetScript ( strLuc, strOutFile ) )
	//		{

	//			// ���� ����� ���´�.
	//			//HANDLE hFile = CreateFile ( strOutFile, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	//			//int nError = GetLastError();
	//			CString strTemp;
	//			strTemp.Format( "RunDecryptWidgetScript : %s - %s ",strLuc, strOutFile );
	//			AddEditString( strTemp );
	//		}
	//		else
	//		{
	//			return FALSE;
	//			AddEditString( "!RunDecryptWidgetScript" );
	//			AddEditString( strOutFile.GetString() );
	//		}
	//	}
	//	}
	//	ZipMan::GetInstance().DeleteMemory( strPathTemp );
	//	AddEditString( "DeleteMemory" );
	//}
	//else 
	//{
	//	AddEditString( "!LoadFile_Decrypt_NewFile" );
	//	AddEditString( strPathWSF.c_str());
	//	AddEditString( strPathTemp.c_str() );
	//	bReturn = FALSE;
	//}
	//AddEditString( "Done" );
	return bReturn;
}

BOOL CTexEncryptDlg::RunDecryptWidgetScript( const CString& strPath, const CString& strInput, const CString& strOutput )
{
	// lua -> luo �� ������.
	{
		TSTRING strCommand(strPath.GetString());
		strCommand += _T("\\luadec.exe");
		strCommand += _T(" ");
		strCommand += strInput.GetString();
		strCommand += _T(" > ");
		strCommand += strOutput.GetString();

		if ( !Compile_lua_luc( strCommand ) )
			return FALSE;

		AddEditString ( strCommand.c_str() );

		FILE* pFile = _fsopen( "luctolualistdecrypt test.bat", "rb", _SH_DENYNO );
		if ( pFile )
		{
			fprintf(pFile, "%s\n", strCommand.c_str());
			fclose ( pFile );
		}
	}

	if( m_bDelete )
		DeleteFile( strInput );

	return TRUE;
}

BOOL CTexEncryptDlg::RunUnzipWidgetScript( const CString& strInput, const CString& strOutput )
{
	int nCntFile=0, nCntSuccess = 0;

	AddEditString( "Run Unzip Widget Script in Folder" );
	AddEditString( strInput );

	CString strFind = strInput;
	strFind += "\\*";
	strFind += ".luc";

	CString strOutFile, strOutPath;
	strOutPath = strOutput;
	strOutPath += "\\";

	CString strInFile, strInPath;
	strInPath = strInput;
	strInPath += "\\";

	CString strFile;
	CString strZipFullPath;

	CFileFind finder;	
	BOOL bFind ( TRUE );
	CString strTemp;

	if ( finder.FindFile( strFind ) )
	{
		while( bFind )
		{	
			nCntFile++;

			bFind = finder.FindNextFile();
			strFile = finder.GetFileName();
			strInFile = strInPath;
			strInFile += strFile;
			strOutFile = strOutPath;
			strOutFile += strFile.Left( strFile.ReverseFind('.'));
			strOutFile += ".lua";

			std::string strOutFile2 = strFile.GetString();

			// luc -> lua
			if ( !RunDecryptWidgetScript( strInPath, strFile, sc::file::changeFileExt( strOutFile2, "lua" ).c_str() ) )
			{
				strTemp.Format( " Error RunDecryptLucLua : %s ", strFile );
				AddEditString( strTemp );
			}
			nCntSuccess++;
		}
	}

	if ( nCntSuccess == 0)
		return FALSE;

	
	strTemp.Format( "Luc -> Lua : Success %d ", nCntSuccess );
	AddEditString( strTemp );

	//CString strFind = strInput;

	//std::string strPathWSF( strInput );;
	//strPathWSF += "\\";
	//strPathWSF.append( NSWIDGET_SCRIPT_LIST::g_strWidgetScriptFile );

	//FILE* pFile(NULL);
	//std::string strPathTemp( strPathWSF );
	//strPathTemp.append( _T(".rcc") );

	//BOOL bReturn(TRUE);
	//CPrivateResource cResource( CPrivateResource::ETypeWidgetScript );
	//if ( pFile = cResource.LoadFile_Decrypt_NewFile( strPathWSF.c_str(), strPathTemp.c_str() ) )
	//{
	//	
	//	if ( !ZipMan::GetInstance().LOAD_Memory( strPathWSF, pFile ) )
	//	{
	//		//bReturn = FALSE;
	//		AddEditString( "LOAD_Memory Failed" );
	//		AddEditString( strPathWSF.c_str() );
	//		return FALSE;
	//	}

	//	BYTE* pByte(NULL);
	//	std::string strLucFile( sc::file::changeFileExt( NSWIDGETLIST::g_strWidgetListFile, "luc" ) );
	//	std::transform( strLucFile.begin(), strLucFile.end(), strLucFile.begin(), tolower );
	//	UINT size = ZipMan::GetInstance().UnzipToMemory(strPathWSF, strLucFile, pByte);
	//	if ( size <= 0 || pByte == NULL )
	//	{
	//		AddEditString( "Failed to Open" );
	//		AddEditString( strLucFile.c_str() );
	//		CString strLuc,buffer;
	//		if ( !pByte )
	//		buffer.Format("null");
	//		else buffer.Format("not null");
	//		strLuc.Format("ERROR : SIZE %d buffer %s",size,buffer);
	//		AddEditString( strLuc.GetString() );
	//		return FALSE;
	//	}
	//	CString strLuc;
	//	strLuc.Format("Unzipping %s",strLucFile.c_str());
	//	AddEditString( strLucFile.c_str() );

	//	if( false == lua_tinker::dobuffer( m_pLuaState, reinterpret_cast<char*>( pByte ), size ) )
	//	{
	//		return FALSE;
	//	}

	//	{
	//		
	//		pByte = NULL;
	//		// ��ũ��Ʈ ����Ʈ �ε�
	//		std::string strLucFile( sc::file::changeFileExt( NSWIDGET_SCRIPT_LIST::g_strScriptList, "luc" ) );
	//		std::transform( strLucFile.begin(), strLucFile.end(), strLucFile.begin(), tolower );
	//		UINT size = ZipMan::GetInstance().UnzipToMemory(strPathWSF, strLucFile, pByte);
	//		if ( size <= 0 || pByte == NULL )
	//		{
	//			return FALSE;
	//		}
	//		if( false == lua_tinker::dobuffer( m_pLuaState, reinterpret_cast<char*>( pByte ), size ) )
	//		{
	//			return FALSE;
	//		}
	//		CString strLuc;
	//		strLuc.Format("Unzipping %s",strLucFile.c_str());
	//		AddEditString( strLucFile.c_str() );

	//		// ��ũ��Ʈ ����Ʈ���� ������ ���Ǵ� ��ũ��Ʈ�� �����´�.
	//		lua_tinker::table luaScriptList = lua_tinker::get< lua_tinker::table >( m_pLuaState, "WIDGET_SCRIPT_LIST" );
	//		for( int i = 1; i <= luaScriptList.TableLen(); ++i )
	//		{
	//			strLucFile = sc::file::changeFileExt( std::string(luaScriptList.get< const char* >( i )), "luc" );
	//			std::transform( strLucFile.begin(), strLucFile.end(), strLucFile.begin(), tolower );
	//			size = ZipMan::GetInstance().UnzipToMemory(strPathWSF, strLucFile, pByte);
	//			if ( size <= 0 || pByte == NULL )
	//			{
	//				return FALSE;
	//			}

	//			if( false == lua_tinker::dobuffer( m_pLuaState, reinterpret_cast<char*>( pByte ), size ) )
	//			{
	//				return FALSE;
	//			}
	//			CString strLuc;
	//			strLuc.Format("Unzipping %s",strLucFile.c_str());
	//			AddEditString( strLucFile.c_str() );

	//			//strOutFile = strOutPath;
	//			//strOutFile += strLuc.Left( strLuc.ReverseFind('.'));
	//			//strOutFile += ".lua";
	//			//strOutFile.MakeLower();

	//			//if ( RunDecryptWidgetScript ( strLuc, strOutFile ) )
	//			//{

	//			//	// ���� ����� ���´�.
	//			//	//HANDLE hFile = CreateFile ( strOutFile, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	//			//	//int nError = GetLastError();
	//			//	CString strTemp;
	//			//	strTemp.Format( "RunDecryptWidgetScript : %s - %s ",strLuc, strOutFile );
	//			//	AddEditString( strTemp );
	//			//}
	//			//else
	//			//{
	//			//	return FALSE;
	//			//	AddEditString( "!RunDecryptWidgetScript" );
	//			//	AddEditString( strOutFile.GetString() );
	//			//}
	//		}
	//	}
	//	ZipMan::GetInstance().DeleteMemory( strPathWSF );
	//}
	//else 
	//{
	//	AddEditString( "!LoadFile_Decrypt_NewFile" );
	//	AddEditString( strPathWSF.c_str());
	//	AddEditString( strPathTemp.c_str() );
	//	bReturn = FALSE;
	//}
	return TRUE;
}


void CTexEncryptDlg::AddEditString( const char* szInfo )
{
    int len = m_cEdit.GetWindowTextLength();

	m_cEdit.SetSel( len, len );
	m_cEdit.ReplaceSel( szInfo );	

	len = m_cEdit.GetWindowTextLength();
	m_cEdit.SetSel( len, len );
	m_cEdit.ReplaceSel( "\r\n" );

	return;
}

BOOL CTexEncryptDlg::RunEncryptFilesFolder( const CString& strInput, const CString& strOutput )
{
	int nCntFile=0, nCntSuccess = 0;

	AddEditString( "Run Encrypt Files in Folder" );
	AddEditString( strInput );

	{
		CString strFind = strInput;
		strFind += "\\*";
		strFind += ".*";

		CString strOutFile, strOutPath;
		strOutPath = strOutput;
		strOutPath += "\\";

		CString strInFile, strInPath;
		strInPath = strInput;
		strInPath += "\\";

		CString strFile;

		CFileFind finder;	
		BOOL bFind ( TRUE );

		if ( !finder.FindFile( strFind ) )
		{
			return false;
		}

		CString strTemp;

		while( bFind )
		{	
			nCntFile++;

			bFind = finder.FindNextFile();
			strFile = finder.GetFileName();
			strInFile = strInPath;
			strInFile += strFile;
			strOutFile = strOutPath;
			strOutFile += strFile;

			if ( !RunEncryptFiles( strInFile, strOutFile ) )
			{
				//				CDebugSet::ToLogFile( "Texture File Encrypt Error : %s", strInFile );
				strTemp.Format( " Error Encrypt File : %s ", strFile );
				AddEditString( strTemp );
				continue;
			}

			strTemp.Format( " Success Encrypt File : %s ", strFile );
			AddEditString( strTemp );

			nCntSuccess++;
		}
	}

	CString strResult;
	strResult.Format( "%d in %d File Encrypt Success", nCntSuccess, nCntFile );
	AddEditString( strResult );
	return TRUE;
}

BOOL CTexEncryptDlg:: RunDecryptFilesFolder( const CString& strInput, const CString& strOutput )
{
	AddEditString( "Run Decrypt Files in Folder" );
	AddEditString( strInput );

	// DDS FILE ��ȯ
	CString strFind = strInput;
	strFind += "\\*.*";

	CString strOutFile, strOutPath;
	strOutPath = strOutput;
	strOutPath += "\\";

	CString strInFile, strInPath;
	strInPath = strInput;
	strInPath += "\\";

	CString strFile;

	CFileFind finder;	
	BOOL bFind ( TRUE );

	if ( !finder.FindFile( strFind ) )
	{
		bFind = FALSE;
	}

	int nCntFile =0, nCntSuccess = 0;
	CString strTemp;

	while( bFind )
	{
		nCntFile++;
		bFind = finder.FindNextFile();
		strFile = finder.GetFileName();
		strInFile = strInPath;
		strInFile += strFile;
		strOutFile = strOutPath;
		strOutFile += strFile;

		if ( !RunDecryptFiles( strInFile, strOutFile ) )
		{
			//			CDebugSet::ToLogFile( "Texture File Decrypt Error : %s", strInFile );	
			strTemp.Format( " Error Decrypt File : %s ", strFile );
			AddEditString( strTemp );
			continue;
		}

		strTemp.Format( " Success Decrypt File : %s ", strFile );
		AddEditString( strTemp );

		nCntSuccess++;
	}

	CString strResult;
	strResult.Format( "%d in %d File Decrypt Success", nCntSuccess, nCntFile );
	AddEditString( strResult );
	return TRUE;
}


BOOL CTexEncryptDlg::RunEncryptFiles( const CString& strInput, const CString& strOutput )
{
	CPrivateResource cResource( CPrivateResource::ETypeSENSEI );

	return cResource.SaveFileDec( strInput, strOutput, m_bDelete );
}
BOOL CTexEncryptDlg::RunDecryptFiles( const CString& strInput, const CString& strOutput )
{
	CPrivateResource cResource( CPrivateResource::ETypeSENSEI );

	return cResource.SaveFileDec( strInput, strOutput, m_bDelete );
}


BOOL CTexEncryptDlg::RunEncryptXMLFolder( const CString& strInput, const CString& strOutput )
{
	int nCntFile=0, nCntSuccess = 0;

	AddEditString( "Run Encrypt XML in Folder" );
	AddEditString( strInput );

	{
		CString strFind = strInput;
		strFind += "\\*";
		strFind += ".xml";

		CString strOutFile, strOutPath;
		strOutPath = strOutput;
		strOutPath += "\\";

		CString strInFile, strInPath;
		strInPath = strInput;
		strInPath += "\\";

		CString strFile;

		CFileFind finder;	
		BOOL bFind ( TRUE );


		CString strTemp;
		if ( !finder.FindFile( strFind ) )
		{
			strTemp.Format( "Failed to find File: '%s'", strFind.GetString() );
			AddEditString( strTemp );
			return FALSE;
		}

		while( bFind )
		{	
			nCntFile++;

			bFind = finder.FindNextFile();
			strFile = finder.GetFileName();
			strInFile = strInPath;
			strInFile += strFile;
			strOutFile = strOutPath;
			strOutFile += strFile;

			FILE* pFileInput = NULL;
			pFileInput = fopen( strInFile.GetString(), "rb" );
			if ( !pFileInput )
			{
				strTemp.Format( "Failed Open Input File: '%s'", strInFile.GetString() );
				AddEditString( strTemp );
				continue;
			}

			fseek ( pFileInput, 0, SEEK_END );
			int nSize = ftell ( pFileInput );
			fseek ( pFileInput, 0, SEEK_SET );	

			BYTE* pBuffer = new BYTE[nSize];
			fread( pBuffer , sizeof(BYTE), nSize, pFileInput );
			fclose( pFileInput );

			NS_XMLCRYPT::Encode( pBuffer, nSize );

			FILE* pFileOutput = NULL;
			pFileOutput = fopen( strOutFile.GetString(), "wb" );
			if ( !pFileOutput )
			{
				strTemp.Format( "Failed Open Output File: '%s'", strOutFile.GetString() );
				AddEditString( strTemp );
				delete[] pBuffer;
				continue;
			}

			fwrite( pBuffer, sizeof(BYTE), nSize, pFileOutput );
			fclose( pFileOutput );

			delete[] pBuffer;

			strTemp.Format( " Success Encrypt XML File : %s ", strFile );
			AddEditString( strTemp );

			nCntSuccess++;
		}
	}

	CString strResult;
	strResult.Format( "%d in %d XML File Encrypt Success", nCntSuccess, nCntFile );
	AddEditString( strResult );

	return TRUE;
}

BOOL CTexEncryptDlg::RunDecryptXMLFolder( const CString& strInput, const CString& strOutput )
{
	AddEditString( "Run Decrypt Skin File in Folder" );
	AddEditString( strInput );

	CString strFind = strInput;
	strFind += "\\*.xml";

	CString strOutFile, strOutPath;
	strOutPath = strOutput;
	strOutPath += "\\";

	CString strInFile, strInPath;
	strInPath = strInput;
	strInPath += "\\";

	CString strFile;

	CFileFind finder;	
	BOOL bFind ( TRUE );

	if ( !finder.FindFile( strFind ) )
	{
		bFind = FALSE;
	}

	int nCntFile =0, nCntSuccess = 0;
	CString strTemp;

	while( bFind )
	{
		nCntFile++;
		bFind = finder.FindNextFile();
		strFile = finder.GetFileName();
		strInFile = strInPath;
		strInFile += strFile;
		strOutFile = strOutPath;
		strOutFile += strFile;

		FILE* pFileInput = NULL;
		pFileInput = fopen( strInFile.GetString(), "rb" );
		if ( !pFileInput )
		{
			strTemp.Format( "Failed Open Input File: '%s'", strInFile.GetString() );
			AddEditString( strTemp );
			continue;
		}

		fseek ( pFileInput, 0, SEEK_END );
		int nSize = ftell ( pFileInput );
		fseek ( pFileInput, 0, SEEK_SET );	

		BYTE* pBuffer = new BYTE[nSize];
		fread( pBuffer , sizeof(BYTE), nSize, pFileInput );
		fclose( pFileInput );

		NS_XMLCRYPT::Decode( pBuffer, nSize );

		FILE* pFileOutput = NULL;
		pFileOutput = fopen( strOutFile.GetString(), "wb" );
		if ( !pFileOutput )
		{
			strTemp.Format( "Failed Open Output File: '%s'", strOutFile );
			AddEditString( strTemp );
			delete[] pBuffer;
			continue;
		}

		fwrite( pBuffer, sizeof(BYTE), nSize, pFileOutput );
		fclose( pFileOutput );

		delete[] pBuffer;

		strTemp.Format( " Success Decrypt Skin File : %s ", strFile );
		AddEditString( strTemp );

		nCntSuccess++;
	}

	CString strResult;
	strResult.Format( "%d in %d Skin File Encrypt Success", nCntSuccess, nCntFile );
	AddEditString( strResult );

	return TRUE;
}