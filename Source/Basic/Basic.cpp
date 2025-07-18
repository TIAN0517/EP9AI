#define STRICT
#include "stdafx.h"
#include <afxdisp.h>
#include <afxocc.h>
#include <Imm.h>

//#include "../=vld/include/vld.h" // vld.h Çì´õÆÄÀÏÀº stdafx.h ´ÙÀ½¿¡ À§Ä¡ÇØ¾ß ÇÕ´Ï´Ù.
#include "../SigmaCore/SigmaCore.h"
#include "../SigmaCore/gassert.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/File/FileUtil.h"

#include "../EngineLib/GUInterface/Cursor.h"
#include "../Enginelib/GUInterface/GameTextControl.h"
#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/Common/CommonWeb.h"
#include "../EngineLib/DxCommon9/getdxver.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"

#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/VtcGuard.h"
#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLUseFeatures.h"

#include "../MfcExLib/DaumParam/DaumGameParameter.h"
#include "../MfcExLib/MinBugTrap.h"

#include "../RanLogicClient/Tool/DxParamSet.h"

#include "Protection.h"
#include "BasicWnd.h"
#include "Basic.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CBasicApp
//Client Limiter
#include <tlhelp32.h>
#include <string>

//DO_RTC_CATCH _rtc_catch;
//Process Entry Anti Cheat
#define Exe_Name PROCESSENTRY32 *pe32 = GetProcessInfo ("MiniA.exe") 
#define Threads (pe32->cntThreads)

// Nprotect Á¤ÀÇ
//#if defined(MY_PARAM) || defined(MYE_PARAM) || defined(PH_PARAM)//|| defined(_RELEASED) 
CNPGameLib* pNpgl = NULL;
//#endif


BEGIN_MESSAGE_MAP(CBasicApp, CWinApp)
	//{{AFX_MSG_MAP(CBasicApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicApp construction

CBasicApp::CBasicApp()
    : m_bIsActive(TRUE)
    , m_bRedrawScreen(FALSE)
    , m_bShutDown(FALSE)
{
	// ¼ø¼ö °¡»óÇÔ¼ö È£Ãâ °¨Áö ÇÚµé·¯.
	_set_purecall_handler(smtm_PurecallHandler);
    
    sc::initialize();

    if( GLogicData::GetInstance().LoadCountryFile() == false )
		return;

    m_ServiceProvider = GLogicData::GetInstance().GetServiceProvider();
    BUG_TRAP::BugTrapInstall(m_ServiceProvider, std::string("Game"));
    m_pPath = new SUBPATH(m_ServiceProvider);

    //
	//bugtrap v1.3
	//
	BT_SetTerminate(); // set_terminate() must be called from every thread

	sc::LogMan::getInstance()->attachBugTrapLog();

    RENDERPARAM::emSERVICE_TYPE = GLogicData::GetInstance().GetServiceType(m_ServiceProvider);
    m_pVtcGuard = new VtcGuard(m_ServiceProvider);

// #ifdef CH_PARAM
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_CHINA;
// #endif
// 
// #ifdef TH_PARAM
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_THAILAND;
// #endif
// 
// #ifdef MY_PARAM
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_MALAYSIA_CN;
// #endif
// 
// #ifdef MYE_PARAM
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_MALAYSIA_EN;
// #endif
// 
// #ifdef ID_PARAM
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_INDONESIA;
// #endif
// 
// #ifdef PH_PARAM
// 	//RANPARAM::bScrWndHalfSize = FALSE;
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_PHILIPPINES;
// #endif
// 
// #ifdef VN_PARAM
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_VIETNAM;
// #endif
// 
// #ifdef JP_PARAM
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_JAPAN;
// #endif
// 
// #ifdef TW_PARAM
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_FEYA;
// #endif
// 
// #ifdef HK_PARAM
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_FEYA;
// #endif
// 
// //#if defined(KR_PARAM) || defined(KRT_PARAM) 
// #if defined(KR_PARAM)
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_KOREA;
// #endif
// 
// #if defined(GS_PARAM)
// 	RENDERPARAM::emSERVICE_TYPE = EMSERVICE_GLOBAL;
// #endif
// 
// 	RENDERPARAM::emSERVICE_TYPE = (RENDERPARAM::EMSERVICE_TYPE)RENDERPARAM::emSERVICE_TYPE;

	SecureZeroMemory(m_szAppPath, sizeof(m_szAppPath));

    DeleteLogFile();

	m_bLoginProcessLog = FALSE;
}

void CBasicApp::DeleteLogFile()
{	
    std::string strAppPath(sc::getAppPath());
    strAppPath += "\\log";
    sc::file::deleteFiles(strAppPath);
}

CBasicApp::~CBasicApp()
{
    if (m_pVtcGuard)
    {
        m_pVtcGuard->GameLogOut();
        delete m_pVtcGuard;
    }

//	Á¦°Å ÇÒ¶§ ¿À·ù³­´Ù...¾Æ³ö NP...
	// Á¦°ÅÇÒ¶§ ¿À·ù³­´Ù°í ÇÏ´Âµ¥ ÇöÀç±îÁö´Â ÀÌ»ó¾ø¾î¼­ ´Ù½Ã È°¼ºÈ­ ½ÃÅ´. 2014.08.14
	SAFE_DELETE( pNpgl );
	//sc::writeLogDebug(std::string("DELETE pNpgl : ~CBasicApp"));

    delete m_pPath;
    m_pPath = NULL;

	GLogicData::GetInstance().ReleaseInstance();
    sc::finalize();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBasicApp object

CBasicApp theApp;

//Anti Cheat
PROCESSENTRY32 *GetProcessInfo(char *szExeFile)
{
	PROCESSENTRY32  *pe32 = new PROCESSENTRY32 
		[sizeof(PROCESSENTRY32)];
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnapshot == INVALID_HANDLE_VALUE)
		return NULL;	 
	if(!Process32First(hSnapshot, pe32)) 
	{
		CloseHandle(hSnapshot);
		return NULL;        
	}
	while(Process32Next( hSnapshot, pe32 ))
	{
		if( strcmp(szExeFile, pe32->szExeFile) == 0) 
		{		
			CloseHandle(hSnapshot);
			return pe32;
		}
	} 
	return NULL;
}
void CBasicApp::GetWindowName()
{
	// .exe
	std::string exeString = AfxGetAppName() + std::string(1,static_cast<char>(46)) + std::string(1,static_cast<char>(101)) + std::string(1,static_cast<char>(120)) + std::string(1,static_cast<char>(101)) ;

	m_StrName = exeString.c_str() ;
}
/////////////////////////////////////////////////////////////////////////////
// CBasicApp initialization

BOOL CBasicApp::InitInstance()
{
	AfxEnableControlContainer();

	// Note : ½ÇÇàÆÄÀÏÀÇ °æ·Î¸¦ Ã£¾Æ¼­ ÀúÀåÇÑ´Ù. - ¹Ýµå½Ã CBasicWnd(CD3dApp) ÀÌÀü¿¡ ¼öÇàÇØ¾ß ÇÑ´Ù.
	//
	SetAppPath();
 
	RANPARAM::LOAD(m_szAppPath, m_ServiceProvider, false, false);
	DXPARAMSET::INIT();
 
    if (m_ServiceProvider == SP_KOREA )
	{
		// ´ÙÀ½ °ÔÀÓ ÆÄ¶ó¸ÞÅ¸ È®ÀÎ
		CDaumGameParameter cDGP;
		char szParameter[MAX_DGPARAMETER_LENGTH];
		if ( cDGP.Get( _T("Parameter"), szParameter, MAX_DGPARAMETER_LENGTH ) == FALSE )
		{
			MessageBox( NULL, _T("À¥¿¡¼­ ½ÇÇàÇØÁÖ½Ê½Ã¿À."), _T("¶õ¿Â¶óÀÎ"), MB_OK);
			return FALSE;
		}

		//MessageBox ( NULL, szParameter, "key", MB_OK );
		RANPARAM::SETIDPARAM(szParameter);
		
		//	userid´Â ÀÓÀÇ·Î ÁöÁ¤. ( °ÔÀÓ ¿É¼Ç ÀúÀå¿ë. )
		RANPARAM::SETUSERID("userid");

		//	´ÙÀ½ °ÔÀÓ ÆÄ¶ó¸ÞÅ¸ÀÇ ³»¿ëÀ» Áö¿öÁÝ´Ï´Ù.
		cDGP.Empty();
	}

    bool bVtcGuardInit = m_pVtcGuard->Init();

#ifdef NDEBUG
    if (m_ServiceProvider == SP_INDONESIA && !bVtcGuardInit)
    {
        sc::writeLogError(std::string("VtcGuard Init failed"));
        sc::LogMan::releaseInstance();
        exit(1);
        return FALSE;
    }
#endif

	CString StrCmdLine = m_lpCmdLine;

	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( "/" );
	STRUTIL::RegisterSeparator ( " " );

	BOOL bAPM = FALSE;
	//CString strCommandOld = "apm";
	CString strCommand = CString("app") + "_" + "run";

	CStringArray strCmdArray;
	STRUTIL::StringSeparate ( StrCmdLine, strCmdArray );
	
	for ( int i=0; i<strCmdArray.GetCount(); i++ )
	{
		CString strTemp = strCmdArray.GetAt(i);

		if ( strTemp == strCommand )
			bAPM = TRUE;

	//#if defined(TW_PARAM) || defined(HK_PARAM)
        if (m_ServiceProvider == SP_HONGKONG)
        {
		    if ( strTemp == _T("ranonlineglobal") )
		    {
			    //RANPARAM::bScrWndHalfSize = TRUE;	// ÇÊ¿ä¾ø´Â µ¥ÀÌÅÍ
			    RANPARAM::bScrWndFullSize = FALSE;
		    }
        }
	//#else
        else
        {
		    if ( strTemp == _T("windowed") )
		    {
			   // RANPARAM::bScrWndHalfSize = TRUE;	// ÇÊ¿ä¾ø´Â µ¥ÀÌÅÍ
			    RANPARAM::bScrWndFullSize = FALSE;
		    }
        }
	//#endif

		CString strVALUE;
		int nPos(-1);
		CString strKEY;

		strKEY = "TID:";
		nPos = strTemp.Find( strKEY );
		if ( nPos!=-1 )
		{
			strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

			//	°ª ÁöÁ¤.
			RANPARAM::SETIDPARAM ( strVALUE.GetString() );
			
			//	userid´Â ÀÓÀÇ·Î ÁöÁ¤. ( °ÔÀÓ ¿É¼Ç ÀúÀå¿ë. )
			RANPARAM::SETUSERID ( "userid" );
		}


		strKEY = "U=";
		nPos = strTemp.Find( strKEY );
		if ( nPos!=-1 )
		{
			strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

			//	°ª ÁöÁ¤.
			RANPARAM::SETIDPARAM ( strVALUE.GetString() );
			
			//	userid´Â ÀÓÀÇ·Î ÁöÁ¤. ( °ÔÀÓ ¿É¼Ç ÀúÀå¿ë. )
			RANPARAM::SETUSERID ( "userid" );
		}

		strKEY = "UUID=";
		nPos = strTemp.Find( strKEY );
		if ( nPos!=-1 )
		{
			strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

			//	°ª ÁöÁ¤.
			RANPARAM::SETIDPARAM ( strVALUE.GetString() );

			//	userid´Â ÀÓÀÇ·Î ÁöÁ¤. ( °ÔÀÓ ¿É¼Ç ÀúÀå¿ë. )
			RANPARAM::SETUSERID ( "userid" );
		}

		strKEY = "T=";
		nPos = strTemp.Find( strKEY );
		if ( nPos!=-1 )
		{
			strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

			//	°ª ÁöÁ¤.
			RANPARAM::SETIDPARAM2 ( strVALUE.GetString() );
		}

		strKEY = "MD=";
		nPos = strTemp.Find( strKEY );
		if ( nPos!=-1 )
		{
			strVALUE = strTemp.Right ( strTemp.GetLength()-strKEY.GetLength() );

			//	°ª ÁöÁ¤.
			RANPARAM::SETIDPARAM3 ( strVALUE.GetString() );
		}

#ifdef _RELEASED

		strKEY = "-SFDEBUG";
		nPos = strTemp.Find( strKEY );
		if( nPos != -1 )
			NSWIDGET_SCRIPT::g_bWidgetDebug = true;

#endif

		strKEY = "-LOGINLOG";
		nPos = strTemp.Find( strKEY );
		if( nPos != -1 )
			m_bLoginProcessLog = TRUE;
	}

	//	Note : À¥·Î±×ÀÎÀÌ°í cmd line param À¸·Î Àü´ÞµÇ´Â °ªÀÌ ÀÖÀ» °æ¿ì.
	//		°ªÀÌ Á¤È®ÇÏ°Ô ÀúÀåµÇ¾î ÀÖ´ÂÁö Ã¼Å©.
	if( RENDERPARAM::emSERVICE_TYPE==RENDERPARAM::EMSERVICE_MALAYSIA_CN || 
		RENDERPARAM::emSERVICE_TYPE==RENDERPARAM::EMSERVICE_MALAYSIA_EN ||
//		RENDERPARAM::emSERVICE_TYPE==EMSERVICE_JAPAN ||	// JAPAN ·Î±×ÀÎ ¹æ½Ä º¯°æ
		RENDERPARAM::emSERVICE_TYPE==RENDERPARAM::EMSERVICE_PHILIPPINES ||
		RENDERPARAM::emSERVICE_TYPE==RENDERPARAM::EMSERVICE_VIETNAM )
	{
		if (!RANPARAM::VALIDIDPARAM())
		{
            std::string ErrorMsg("Invalid web account");
            sc::writeLogError(ErrorMsg);
			MessageBox(NULL, ErrorMsg.c_str(), "ERROR", MB_OK|MB_ICONEXCLAMATION);
			return FALSE;
		}
	}

	#if defined(NDEBUG) && !defined(_RELEASED)
	if (!bAPM)
	{
		MessageBox(NULL, "This program can not run independently.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	#endif // !NDEBUG

   /* DWORD dwDirectXVersion = 0;
    TCHAR strDirectXVersion[10];
	HRESULT hr = getdxversion(&dwDirectXVersion, strDirectXVersion, 10);

	if (hr==S_OK && dwDirectXVersion<0x00090003)
	{
		MessageBox(NULL, "You will need to install DirectX 9.0c or later.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}*/

	{	// Note : ÀÎÅÍÆäÀÌ½º ½ºÅ³ ÅØ½ºÆ® ·Îµù
		TCHAR szFullPath[MAX_PATH] = {0};

		StringCchCopy(szFullPath, MAX_PATH, ((CBasicApp*) AfxGetApp())->m_szAppPath);
		StringCchCat(szFullPath, MAX_PATH, m_pPath->GuiFileRoot().c_str()); //SUBPATH::GUI_FILE_ROOT);
		CGameTextMan::GetInstance().SetPath(szFullPath);

		if ( !GLUseFeatures::GetInstance().LoadFile(GLogicData::GetInstance().GetUseFeaturesFileName(), m_pPath, FALSE))
		{
#ifdef _RELEASED
			::MessageBox(NULL, "Usefeatures load error", "err", MB_OK|MB_ICONEXCLAMATION);
			
#else
			sc::writeLogError("BasicInitinstance-Usefeatures load error");
#endif
		}

		CGameTextMan::GetInstance().LoadText(RANPARAM::strGameWord.GetString(),   CGameTextMan::EM_GAME_WORD, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT);
		CGameTextMan::GetInstance().LoadText(RANPARAM::strGameInText.GetString(), CGameTextMan::EM_GAME_IN_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT);
		CGameTextMan::GetInstance().LoadText(RANPARAM::strGameExText.GetString(), CGameTextMan::EM_GAME_EX_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT);

		CGameTextMan::GetInstance().LoadText(RANPARAM::strTipText.GetString(), CGameTextMan::EM_TIP_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT);
	}


// NProtect ÃÊ±âÈ­
// Nprotect Á¤ÀÇ
    
//#if defined(MY_PARAM) || defined(MYE_PARAM) //|| defined(_RELEASED) 
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	CString strFileName;

    if (m_ServiceProvider == SP_MALAYSIA_EN || m_ServiceProvider == SP_MALAYSIA)
    {
	    strFileName = m_strAppPath;
	    strFileName += "\\RanOnlineMY.ini";

	    hFind = FindFirstFile(strFileName, &FindFileData);

	    //	¸»·¹ÀÌ½Ã¾Æ NP¼³Á¤ ÆÄÀÏ ÀÖ´ÂÁö È®ÀÎÇÑ´Ù.
	    if (hFind != INVALID_HANDLE_VALUE)
	    {
		    FindClose(hFind);
		    pNpgl = new CNPGameLib("RanOnlineMY");
		    if (!pNpgl)
            {
                sc::writeLogError(std::string("InitInstance RanOnlineMY"));
                AfxMessageBox("ERROR InitInstance RanOnlineMY", MB_OK|MB_ICONEXCLAMATION);
                return FALSE;
            }
	    }
	    else
	    {
		    pNpgl = new CNPGameLib("RanOnlineMYTest");
		    if (!pNpgl)
            {
                AfxMessageBox("ERROR InitInstance RanOnlineMYTest", MB_OK|MB_ICONEXCLAMATION);
                return FALSE;
            }
	    }

	    /*DWORD dwResult = pNpgl->Init();
	    if( dwResult != NPGAMEMON_SUCCESS )
	    {
			sc::writeLogError(sc::string::format("nProtect Init ERROR : %1%", dwResult));

		    ErrorMsgByNP(dwResult);
			SAFE_DELETE( pNpgl );
			//sc::writeLogDebug(std::string("DELETE pNpgl : InitInstance SP_MALAYSIA"));

		    return FALSE;
	    }*/
    }
//#elif defined(PH_PARAM)
    else if (m_ServiceProvider == SP_PHILIPPINES)
    {
	    strFileName = m_strAppPath;
	    strFileName += "\\RanOnlinePH.ini";

	    hFind = FindFirstFile(strFileName, &FindFileData);

	    //	¸»·¹ÀÌ½Ã¾Æ NP¼³Á¤ ÆÄÀÏ ÀÖ´ÂÁö È®ÀÎÇÑ´Ù.
	    if (hFind != INVALID_HANDLE_VALUE)
	    {
		    FindClose(hFind);
		    pNpgl = new CNPGameLib("RanOnlinePH");
		    if (!pNpgl)
            {
                sc::writeLogError(std::string("InitInstance RanOnlinePH"));
                AfxMessageBox("ERROR InitInstance RanOnlinePH", MB_OK|MB_ICONEXCLAMATION);
                return FALSE;
            }
	    }
	    else
	    {
		    pNpgl = new CNPGameLib("RanOnlinePHTest");
		    if (!pNpgl)
            {
				sc::writeLogError(std::string("InitInstance RanOnlinePHTest"));
                AfxMessageBox("ERROR InitInstance RanOnlinePHTest", MB_OK|MB_ICONEXCLAMATION);
                return FALSE;
            }
	    }

	    //pNpgl = new CNPGameLib("RanOnlinePH");
	    //if (!pNpgl)
     //   {
     //       sc::writeLogError(std::string("InitInstance RanOnlinePH"));
     //       AfxMessageBox("ERROR InitInstance RanOnlinePH", MB_OK|MB_ICONEXCLAMATION);
     //       return FALSE;
     //   }

	  //  DWORD dwResult = pNpgl->Init();
	  //  if (dwResult != NPGAMEMON_SUCCESS)
	  //  {
			//sc::writeLogError(sc::string::format("nProtect Init ERROR : %1%", dwResult));

		 //   ErrorMsgByNP(dwResult);
			//SAFE_DELETE( pNpgl );
			////sc::writeLogDebug(std::string("DELETE pNpgl : InitInstance SP_PHILIPPINES"));

		 //   return FALSE;
	  //  }
    }
    else if (m_ServiceProvider == SP_EU)
    {
	    pNpgl = new CNPGameLib("RanOnlineEU");
	    if (!pNpgl)
        {
            sc::writeLogError(std::string("InitInstance RanOnlineEU"));
            AfxMessageBox("ERROR InitInstance RanOnlineEU", MB_OK|MB_ICONEXCLAMATION);
            return FALSE;
        }

	  //  DWORD dwResult = pNpgl->Init();
	  //  if (dwResult != NPGAMEMON_SUCCESS)
	  //  {
			//sc::writeLogError(sc::string::format("nProtect Init ERROR : %1%", dwResult));

		 //   ErrorMsgByNP(dwResult);
			//SAFE_DELETE( pNpgl );
			////sc::writeLogDebug(std::string("DELETE pNpgl : InitInstance SP_EU"));

		 //   return FALSE;
	  //  }
    }
    else if (m_ServiceProvider == SP_KOREA_TEST) // _NP30
    {
		strFileName = m_strAppPath;
	    strFileName += "\\RanOnlineGS.ini";

	    hFind = FindFirstFile(strFileName, &FindFileData);

	    //	¸»·¹ÀÌ½Ã¾Æ NP¼³Á¤ ÆÄÀÏ ÀÖ´ÂÁö È®ÀÎÇÑ´Ù.
	    if (hFind != INVALID_HANDLE_VALUE)
	    {
		    FindClose(hFind);
		    pNpgl = new CNPGameLib("RanOnlineGS");
		    if (!pNpgl)
            {
                sc::writeLogError(std::string("InitInstance RanOnlineGS"));
                AfxMessageBox("ERROR InitInstance RanOnlineGS", MB_OK|MB_ICONEXCLAMATION);
                return FALSE;
            }
	    }
	    else
	    {
		    pNpgl = new CNPGameLib("RanOnlineGSTest");
		    if (!pNpgl)
            {
				sc::writeLogError(std::string("InitInstance RanOnlineGSTest"));
                AfxMessageBox("ERROR InitInstance RanOnlineGSTest", MB_OK|MB_ICONEXCLAMATION);
                return FALSE;
            }
	    }

	    //pNpgl = new CNPGameLib("RanOnlineGS");
	    //if (!pNpgl)
     //   {
     //       sc::writeLogError(std::string("InitInstance RanOnlineGS"));
     //       AfxMessageBox("ERROR InitInstance RanOnlineGS", MB_OK|MB_ICONEXCLAMATION);
     //       return FALSE;
     //   }

	  //  DWORD dwResult = pNpgl->Init();
	  //  if (dwResult != NPGAMEMON_SUCCESS)
	  //  {
			//sc::writeLogError(sc::string::format("nProtect Init ERROR : %1%", dwResult));

		 //   ErrorMsgByNP(dwResult);
			//SAFE_DELETE( pNpgl );
			////sc::writeLogDebug(std::string("DELETE pNpgl : InitInstance SP_GS"));

		 //   return FALSE;
	  //  }
    }
//#endif
    else
    {
    }

	CBasicWnd* pWnd = new CBasicWnd(m_ServiceProvider, m_bLoginProcessLog);
	if (pWnd->Create() == FALSE)
        return FALSE;
	m_pWnd = pWnd;
	return TRUE;
}

void CBasicApp::SetAppPath()
{
	CString strAppPath;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAppPath = szPath;

	if ( !strAppPath.IsEmpty() )
	{
		DWORD dwFind = strAppPath.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			m_strAppPath = strAppPath.Left ( dwFind );
			
			if ( !m_strAppPath.IsEmpty() )
			if ( m_strAppPath.GetAt(0) == '"' )
				m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength()-1 );

			StringCchCopy ( m_szAppPath, MAX_PATH, m_strAppPath.GetString() );
		}
	}
	else 
	{
		MessageBox ( NULL, "SetAppPath Error", "Error", MB_OK );
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////

int CBasicApp::ExitInstance() 
{
	m_pWnd = NULL;

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////

int CBasicApp::Run() 
{
	MSG msg;
	BOOL bGotMsg;

	//PeekMessageW( &msg, NULL, 0, 0, PM_NOREMOVE );

	//CProtection cProtection;

	while( WM_QUIT != msg.message )
	{
		//DWORD dwTime = timeGetTime();

#if defined(NDEBUG) && !defined(_RELEASED)
		/*
        // software breakpoint check
		if (cProtection.Pesudo_IsDebuggerPresent())	
		{
			m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
		}

		// hardware breakpoint check
		GetThreadContext(GetCurrentThread(), &cProtection.CONTEXTDebug);	
		if ( cProtection.CONTEXTDebug.Dr0 || cProtection.CONTEXTDebug.Dr1 ||
			 cProtection.CONTEXTDebug.Dr2 || cProtection.CONTEXTDebug.Dr3 )	
		{
			m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
		}
        */
#endif        
		if ( m_bShutDown == TRUE || m_pWnd == NULL )
		{
			ExitInstance ();
			return 0;
		}
		
		//// 태국어의 경우 PeekMessageW, GetMessageW를 하면 비정상 동작한다;
		//// 스케일폼은 기본적으로 유니코드 기반이라 스케일폼 사용시 W를 쓰도록한다;
		//if( m_bIsActive )
			bGotMsg = PeekMessageW( &msg, NULL, 0, 0, PM_REMOVE );
		//else
		//	bGotMsg = GetMessageW( &msg, NULL, 0, 0 );

		HWND hwndParent = GetForegroundWindow();	// 활성화 여부와 상관없이 가장 탑에 있는걸 찾음.
		if ( hwndParent )
		{
			m_pWnd->SetForegroundWindow( m_pWnd->GetHwndApp() == hwndParent ? TRUE : FALSE );
		}

		if( bGotMsg )
		{
            // mjeon.sns
            // processing Tab-key stroke
            //
            if( msg.message == WM_KEYDOWN && msg.wParam == VK_TAB )
                COMMON_WEB::TranslateAccelerator( msg );

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			//if ( !m_bIsActive && !m_bShutDown )
			//{
			//	if ( FAILED ( m_pWnd->FrameMove3DEnvironment() ) )
			//		m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
			//}
		}
		else
		{
			if ( m_bIsActive )
			{
				// 정상적인 렌더링.
				if ( FAILED ( m_pWnd->Render3DEnvironment() ) )
					m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
			}
			else	// 창이 내려가 있을 경우
			{
				// 내부에서 1초의 딜레이를 주고 있다.
				if ( FAILED ( m_pWnd->FrameMove3DEnvironment() ) )
					m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
			}
		}

		// [shhan][2015.03.12] 문제가 있을 수 있다. 타 클라이언트에서 끊어지는 느낌이 생길수있다.
		//// [shhan][2015.02.24] 다중 클라를 사용하는 유저를 위해 이 작업이 추가되었다.
		////						활성화 되어 있지 않을 경우는 속도를 대폭적으로 줄인다.
		//{
		//	// 이걸로는 현재 내 윈도우 핸들만 얻어온다.
		//	//HWND hwndParent = GetActiveWindow();	// 윈도우가 여러개 띄워있을 경우 활성화 되있는 걸 찾음.
		//	//if (hwndParent == NULL)
		//	//	hwndParent = GetForegroundWindow();	// 활성화 여부와 상관없이 가장 탑에 있는걸 찾음.

		//	HWND hwndParent = GetForegroundWindow();	// 활성화 여부와 상관없이 가장 탑에 있는걸 찾음.
		//	if ( hwndParent )
		//	{
		//		if ( m_pWnd->GetHwndApp() != hwndParent )
		//		{
		//			// cpu 부하를 66%로 줄인다.	100 - (100 / (2+1));
		//			// 백 클라이언트 3개를 켜야 원래 한개 켜는 정도의 속도만 필요하다.
		//			int nDelayTime = (timeGetTime() - dwTime) * 2.f;

		//			// 딜레이를 제한 범위를 적게 주면, 컴터 느린유저가 백 클라이언트 Delay 체감이 잘 안될 것이고,
		//			// 딜레이를 제한 범위를 크게 주면, 맵이동시 딜레이 체감이 생길 수 있다.
		//			// 지금은 적당하게 0.2초, 5fps 로 한다. 거의 게임 불가능. 보는것만 가능상태일 것이다.
		//			if ( nDelayTime > 200 )
		//				nDelayTime = 200;	

		//			Sleep( nDelayTime );
		//		}
		//	}
		//}
	}

	ExitInstance ();
	return static_cast<int>(msg.wParam);

	//while(1)
	//{
	//	if ( m_bShutDown == TRUE )
	//	{
	//		ExitInstance ();
	//		return 0;
	//	}
	//
	//	if ( PeekMessage ( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	//	{
	//		if ( GetMessage ( &msg, NULL, 0, 0 ) == FALSE )
	//		{
	//			ExitInstance ();
	//			return static_cast<int>(msg.wParam);
	//		}
	//		
	//		TranslateMessage (&msg);
	//		DispatchMessage (&msg);
	//	}
	//	else
	//	{
	//		if ( m_bIsActive == TRUE )
	//		{
	//			if ( m_pWnd )
	//			{
	//				if ( FAILED ( m_pWnd->Render3DEnvironment() ) )
	//					m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
	//			}
	//		}
	//		else
	//		{
	//			if ( m_pWnd )
	//			{
	//				if ( FAILED ( m_pWnd->FrameMove3DEnvironment() ) )
	//					m_pWnd->PostMessage ( WM_CLOSE, 0, 0 );
	//			}
	//			//WaitMessage();
	//		}
	//	}
	//}
}
void CBasicApp::SetActive ( BOOL bActive )
{
	m_bIsActive = bActive;
	m_bRedrawScreen = bActive;
}

void CBasicApp::ErrorMsgByNP( DWORD dwResult )
{
	char msg[MAX_PATH]={0};
	
	// ¡®6. ÁÖ¿ä¿¡·¯ÄÚµå¡¯¸¦ ÂüÁ¶ÇÏ¿© »óÈ²¿¡ ¸Â´Â ¸Þ½ÃÁö¸¦ Ãâ·ÂÇØÁÝ´Ï´Ù.
	switch (dwResult)
	{
	case NPGAMEMON_ERROR_EXIST:
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGAMEMON_ERROR_EXIST"));
		break;
	case NPGAMEMON_ERROR_GAME_EXIST:
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGAMEMON_ERROR_GAME_EXIST"));
		break;
	case NPGAMEMON_ERROR_INIT:
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGAMEMON_ERROR_INIT"));
		break;
	case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
	case NPGAMEMON_ERROR_NFOUND_GG:
	case NPGAMEMON_ERROR_AUTH_INI:
	case NPGAMEMON_ERROR_NFOUND_INI:
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGAMEMON_ERROR_AUTH_GAMEGUARD"));
		break;
	case NPGAMEMON_ERROR_CRYPTOAPI:
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGAMEMON_ERROR_CRYPTOAPI"));
		break;
	case NPGAMEMON_ERROR_EXECUTE:
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGAMEMON_ERROR_EXECUTE"));
		break;
	case NPGAMEMON_ERROR_ILLEGAL_PRG:
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGAMEMON_ERROR_ILLEGAL_PRG"));
		break;
	case NPGMUP_ERROR_ABORT:
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGMUP_ERROR_ABORT"));
		break;
	case NPGMUP_ERROR_CONNECT:
	case NPGMUP_ERROR_DOWNCFG:
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGMUP_ERROR_CONNECT"));
		break;
	case NPGMUP_ERROR_AUTH:
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGMUP_ERROR_AUTH"));
		break;
	case NPGAMEMON_ERROR_NPSCAN:
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGAMEMON_ERROR_NPSCAN"));
		break;
	default:
		// Display relevant messages.
		StringCchCopy(msg, MAX_PATH, ID2GAMEEXTEXT("NPGAMEMON_ERROR_DEFAULT"));
		break;
	}

    std::string ErrorMsg("GameGuard Execute Error %1% ");
    ErrorMsg.append(msg);
    ErrorMsg = sc::string::format(ErrorMsg, dwResult);
    sc::writeLogInfo(ErrorMsg);
	MessageBox(NULL, ErrorMsg.c_str(), "ERROR", MB_OK|MB_ICONEXCLAMATION);
}

