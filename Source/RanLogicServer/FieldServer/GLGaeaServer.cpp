#include "../pch.h"
#include <process.h>
#include "Psapi.h"
#pragma comment(lib, "Psapi.lib")

#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Compress/MinLzo.h"

#include "../../EngineLib/DxMeshs/SkinMesh/DxVehicle.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/Common/profile.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../../RanLogic/HideSet/GLHideSet.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Market/GLSearchDelay.h"
#include "../../RanLogic/Skill/GLSkillReset.h"

#include "../../BugTrap/BugTrap.h"
#include "../../MfcExLib/ExceptionHandler.h"

#include "../Club/GLClubManField.h"
#include "../Club/GLClubField.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Damage/ServerDamage.h"
#include "../Database/DbManager.h"
#include "../Database/EmulDbMan.h"
#include "../Database/DBAction/DbActionGamePost.h"
#include "../Database/DBAction/DbActionGameVehicle.h"
#include "../Database/DBAction/DbActionGamePet.h"
#include "../Database/DBAction/DbActionGameItem.h"
#include "../Event/GLEventProc.h"
#include "../Guidance/GuidanceField.h"
#include "../PK/GLSchoolFreePK.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../MiniGame/GLMiniGameManager.h"
#include "../Party/GLPartyManagerField.h"
#include "../Party/GLPartyField.h"
#include "../Party/GLPartyTenderManagerField.h"
#include "../PointShop/PointShopField.h"
#include "../PrivateMarket/PrivateMarketMan.h"
#include "../PrivateMarket/GLConsignmentSaleField.h"
#include "../PingManager/Field/PingManagerField.h"

#include "../Net/NetCachePacket.h"
#include "../Util/GLItemLimit.h"
#include "../Vehicle/GLVehicleField.h"
#include "../Vehicle/VehicleCacheField.h"
#include "../BuffManager/GLBuffManagerFD.h"
#include "../TriggerManager/GLTriggerManagerFD.h"
#include "../TriggerSystem/ServerTriggerSystem.h"
#include "../Msg/GLMsgMgr.h"
#include "../TexasHoldem/GLTexasHoldemField.h"
#include "./CharManField.h"
#include "./GLGaeaServer.h"
#include "../MatchSystem/MatchingManager.h"
#include "../CharacterRecovery/CharacterBackup.h"
#include "../PostRenew/PostRenewField.h"
#include "../Util/GLItemCheck.h"
#include "../Util/GLUserLockerLockMgr.h"

// Lotto System;
#include "../LottoSystem/GLLottoSystemManField.h"


// AI System;
#include "../AI/GLFSMContainer.h"

// Country System;
#include "../Country/GLCountryManServer.h"
#include "../Country/GLVictoriousCountryManField.h"

// Provide Reward System;
#include "../ProvideRewardSystem/GLProvideRewardManager.h"

// Move System;
#include "../Move/MoveManagerField.h"
#include "../PrivateMarket/PrivateMarketField.h"

//
//mjeon.tbb
//
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/exception/all.hpp>


#ifdef _USE_TBB

#include "../../=TBB/include/tbb/tbbmalloc_proxy.h"
#include "../../=TBB/include/tbb/tbb.h"
#include "../../=TBB/include/tbb/task.h"
#include "../../=TBB/include/tbb/parallel_for.h"
#include "../../=TBB/include/tbb/parallel_for_each.h"
#include "../../=TBB/include/tbb/parallel_do.h"
#include "../SigmaCore/File/FileUtil.h"

#define MINNUM_OF_INSTANTMAP_FOR_PARALLELIZE	10	//temporary

#endif

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

float		g_fCurrentFrameTime = 0.0f;
float GetCurrentFrameTime()
{
	return g_fCurrentFrameTime;
} //GetCurrentFrameTime


GLGaeaServer::GLGaeaServer(EMSERVICE_PROVIDER ServiceProvider, SUBPATH* pPath)
    :m_ServiceProvider(ServiceProvider)
    ,m_pPath(pPath)
	,m_pInstanceManager(NULL)
	,m_pMiniGameManager(NULL)
	,m_pPingManager(NULL)
    ,m_pPartyFieldMan(NULL)
	,m_pPartyTenderManager(NULL)
    ,m_pd3dDevice(NULL)
    ,m_pMsgServer(NULL)
    ,m_pDBMan(NULL)
	,m_pAdoMan(NULL)
    ,m_nServerChannel(0)
    ,m_dwFieldSvrID(0)    
    ,m_dwAgentSlot(0)
	,m_bTracePing(false) 
	,m_bEmulator(false)
    ,m_bUpdate(TRUE)
	,m_bGenItemHold(true)
	,m_dwMaxClient(1000)
    ,m_fTIMER_CLUB(0)
    ,m_bBigHead(false)
    ,m_bBigHand(false)
	,m_fBigHead(1)
	,m_fBigHand(1)
    ,m_bPKServer(false)
    ,m_bBrightEvent(false)
    ,m_bReservedStop(false)
    ,m_bClubDMStarted(false)
    ,m_fDelayMilliSec(1.0f)
    ,m_dwServerCrashTime(0)
    ,m_fCrashTimer(0)
    ,m_wSkipPacketNum(0)
    ,m_bEmptyMsg(false)    
    ,m_pDeathMatchMan(NULL)
    ,m_pClubMan(NULL)
    ,m_pCharMan(NULL)
    ,m_pItemLimit(NULL)
    //,m_PCArray(NULL)
    //,m_PETArray(NULL)
	,m_ctLowerLimit(1971, 1, 1, 0, 0, 0)		// <= CTime
	,m_ctUpperLimit(2999, 12, 31, 23, 59, 59)	// >  CTime
	,m_pCharacterBackup( NULL )
	,m_pUserLockerLockMgr( NULL )
	,m_dwError( SERVER_STOP_NOMAL )
	,m_pMatchingCore(NULL)
	,m_pEventManager(NULL)
	, m_pMoneyMan ( NULL )
	, m_pPrivateMarketSearchDelayMgr( NULL )
	, m_pPrivateMarketLogDelayMgr( NULL )

	, m_poolPET(_T("PET"))
	, m_poolCHAR(_T("Char"))
	, m_poolMobSchedule ( _T( "MobSchedule" ) )
	, m_poolCROW(_T("Crow"))
	, m_poolMATERIAL(_T("GLMaterial"))
	, m_poolSummon(_T("GLSummonField"))
	, m_poolFIELDCROW(_T("SFIELDCROW"))
	, m_poolFIELDLANDEVENT(_T("SFIELDLANDEVENT"))
{
	initPacketFunc();

	InitializeCriticalSectionAndSpinCount(&m_CSPCLock, sc::CRITICAL_SECTION_SPIN_COUNT);
	//InitializeCriticalSectionAndSpinCount(&m_csCreatingCharUserID, sc::CRITICAL_SECTION_SPIN_COUNT);	
	InitializeCriticalSectionAndSpinCount(&m_CS_ClubBattle, sc::CRITICAL_SECTION_SPIN_COUNT);

	InitializeCriticalSectionAndSpinCount(&m_CS_Char, sc::CRITICAL_SECTION_SPIN_COUNT);
	InitializeCriticalSectionAndSpinCount(&m_CS_MobSchedule, sc::CRITICAL_SECTION_SPIN_COUNT);
	InitializeCriticalSectionAndSpinCount(&m_CS_Crow, sc::CRITICAL_SECTION_SPIN_COUNT);
	InitializeCriticalSectionAndSpinCount(&m_CS_FieldCrow, sc::CRITICAL_SECTION_SPIN_COUNT);
	InitializeCriticalSectionAndSpinCount(&m_CS_Land, sc::CRITICAL_SECTION_SPIN_COUNT);
	InitializeCriticalSectionAndSpinCount(&m_CS_LandEvent, sc::CRITICAL_SECTION_SPIN_COUNT);
	InitializeCriticalSectionAndSpinCount(&m_CS_Pet, sc::CRITICAL_SECTION_SPIN_COUNT);
    InitializeCriticalSectionAndSpinCount(&m_CS_Pet2, sc::CRITICAL_SECTION_SPIN_COUNT);
	InitializeCriticalSectionAndSpinCount(&m_CS_Summon, sc::CRITICAL_SECTION_SPIN_COUNT);
	InitializeCriticalSectionAndSpinCount(&m_CS_Material, sc::CRITICAL_SECTION_SPIN_COUNT);

	for ( DWORD _i = 0; _i < MAX_LAND_MID; ++_i )
		m_pLandManager[_i] = NULL;

    //InitializeCriticalSectionAndSpinCount(&m_CS_queDestroyInstanceMapID, sc::CRITICAL_SECTION_SPIN_COUNT);

    m_pMapList = new GLMapListServer;
	m_pEventProc = new GLEventProc(this);
	m_pGuidance = new GLGuidanceFieldMan(this);
	m_pSchoolFreePk = new GLSchoolFreePKField;
	m_pPointShop = new pointshop::PointShopField;
	m_pTexasHoldemMgr = new TexasHoldem::GLTexasHoldemField( this );

	m_pAttendanceScriptMan		= new AttendanceScriptMan;
	m_pCTFMan					= new CaptureTheFieldManFld(this);
	m_pBuffMan                  = new buffmanager::GLBuffManagerFD(this);
    //m_pCharMan                  = new CharManField();

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
    m_pRandomChanceCtrl         = new randomchance::RandomChanceCtrl;

    m_pVehicleCache = new VehicleCacheField;

    m_pPrivateMarketMan = new PrivateMarketMan(this);
    m_pConsignmentSaleField = new GLConsignmentSaleField(this);

	m_pNetMsgMgr = new GLNetMsgDelayMgr;

	// Lotto System;
	LottoSystem::GLLottoSystemManField::Instance()->SetGaeaServer( this );

	// Provide Reward System;
	ProvideRewardSystem::GLProvideRewardManager* pProvideRewardMan =
		ProvideRewardSystem::GLProvideRewardManager::Instance();
	if ( pProvideRewardMan )
	{
		pProvideRewardMan->SetServer( this );
		pProvideRewardMan->OnInitialize( );
	}

	// Move System;
	MoveSystem::CMoveManagerField* pMoveMan =
		MoveSystem::CMoveManagerField::Instance();
	if ( pMoveMan )
		pMoveMan->SetServer( this );

	switch ( m_ServiceProvider )
	{
	case SP_OFFICE_TEST:
	case SP_KOREA:
	case SP_KOREA_TEST:
	case SP_TAIWAN:
	case SP_CHINA:
	case SP_THAILAND:
	case SP_INDONESIA:
	case SP_JAPAN:
	case SP_MALAYSIA:
	case SP_MALAYSIA_EN:
	case SP_PHILIPPINES:
	case SP_GLOBAL:
	case SP_GS:
	case SP_WORLD_BATTLE:
	case SP_EU:
	case SP_US:
		m_pDamageCalculator = new CalcDamage_20060328(GLUseFeatures::GetInstance().IsUsingNewDefenseRate());
		break;
	default:
		m_pDamageCalculator = new CalcDamage_2004(GLUseFeatures::GetInstance().IsUsingNewDefenseRate());
		break;
	}    
	
	if( GLUseFeatures::GetInstance().IsUsingPostRenew() )
		m_spPostField = PostRenewFieldPtr( new PostRenewField(this) );
}

GLGaeaServer::~GLGaeaServer(void)
{
	//CleanUp();	//MUST be called before the destructor of GLGaeaServer.
	SAFE_DELETE(m_pConsignmentSaleField);
    SAFE_DELETE(m_pPrivateMarketMan);
    SAFE_DELETE(m_pVehicleCache);
	SAFE_DELETE(m_pDamageCalculator);
	SAFE_DELETE(m_pItemLimit);
    SAFE_DELETE(m_pPointShop);
    SAFE_DELETE(m_pSchoolFreePk);
    SAFE_DELETE(m_pGuidance);
    SAFE_DELETE(m_pEventProc);
	SAFE_DELETE(m_pInstanceManager);
	SAFE_DELETE(m_pMiniGameManager);
	SAFE_DELETE(m_pPartyFieldMan);
	SAFE_DELETE(m_pPartyTenderManager);
    SAFE_DELETE(m_pDeathMatchMan);
    SAFE_DELETE(m_pClubMan);
	SAFE_DELETE(m_pTexasHoldemMgr);
	SAFE_DELETE(m_pPingManager);

    SAFE_DELETE(m_pAttendanceScriptMan);
    SAFE_DELETE(m_pCTFMan);
    SAFE_DELETE(m_pBuffMan);
	SAFE_DELETE(m_pMatchingCore);
	SAFE_DELETE(m_pEventManager);
	SAFE_DELETE( m_pMoneyMan );

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
    SAFE_DELETE( m_pRandomChanceCtrl );

	ActivityBaseData::ReleaseInstance();

    size_t tSize = m_landManagerMap.size();
    for( size_t i=0; i<tSize; ++i )
    {
		RELEASE_GLLANDMANAGER(m_landManagerMap[i].pLandManager);
	}
	m_landManagerMap.clear();

	SAFE_DELETE(m_pMapList);
    //SAFE_DELETE(m_pCharMan);

	SAFE_DELETE( m_pNetMsgMgr );
	SAFE_DELETE( m_pCharacterBackup );
	SAFE_DELETE( m_pUserLockerLockMgr );

	SAFE_DELETE( m_pPrivateMarketSearchDelayMgr );
	SAFE_DELETE( m_pPrivateMarketLogDelayMgr );

	DeleteCriticalSection(&m_CS_Char);
	DeleteCriticalSection(&m_CS_MobSchedule);
	DeleteCriticalSection(&m_CS_Crow);
	DeleteCriticalSection(&m_CS_FieldCrow);
    DeleteCriticalSection(&m_CS_Pet2);
	DeleteCriticalSection(&m_CS_Pet);
	DeleteCriticalSection(&m_CS_Summon);
	DeleteCriticalSection(&m_CS_Material);

    //DeleteCriticalSection(&m_CS_queDestroyInstanceMapID);

	DeleteCriticalSection(&m_CS_ClubBattle);
	
	//DeleteCriticalSection(&m_csCreatingCharUserID);
	DeleteCriticalSection(&m_CSPCLock);		//CS는 가장 마지막에 삭제하도록 하자. 앞선 deletion에서 Lock을 사용할 수 있다. (ex> RELEASE_CROW)

	DeleteCriticalSection(&m_CS_LandEvent);
	DeleteCriticalSection(&m_CS_Land);
}

void GLGaeaServer::AddGameDbJob(db::DbActionPtr spJob)
{
	if (IsInstance())
	{
		sc::writeLogError(sc::string::format("Ignore GameDbAction %1%", typeid(*spJob).name()));
		return;
	}
	
    m_pDBMan->AddGameJob(spJob, m_pAdoMan);
}

void GLGaeaServer::AddGameAdoJob(db::DbActionPtr spJob)
{
	if (IsInstance())
	{
		sc::writeLogError(sc::string::format("Ignore AdoGameDbAction %1%", typeid(*spJob).name()));
		return;
	}

    if (m_pAdoMan)
    {
        m_pAdoMan->AddGameJob(spJob, m_pAdoMan);
        return;
    }
    
    if (spJob)
    {
        sc::writeLogError(
            sc::string::format(
                "GLGaeaServer::AddGameAdoJob m_pAdoMan NULL %1%",
                typeid(*spJob).name()));
    }
}

void GLGaeaServer::AddUserAdoJob(db::DbActionPtr spJob)
{
	if (IsInstance())
	{
		sc::writeLogError(sc::string::format("Ignore AddUserAdoJob %1%", typeid(*spJob).name()));
		return;
	}
	
    if (m_pAdoMan)
    {
        m_pAdoMan->AddUserJob(spJob, m_pAdoMan);
        return;
    }
    
    if (spJob)
    {
        sc::writeLogError(
            sc::string::format(
                "GLGaeaServer::AddUserAdoJob m_pAdoMan NULL %1%",
                typeid(*spJob).name()));
    }
}

void GLGaeaServer::AddLogAdoJob(db::DbActionPtr spJob)
{
	if (IsInstance())
	{
		sc::writeLogError(sc::string::format("Ignore AdoLogDbAction %1%", typeid(*spJob).name()));
		return;
	}

    if (m_pAdoMan)
    {
        m_pAdoMan->AddLogJob(spJob, m_pAdoMan);
        return;
    }

    if (spJob)
    {
        sc::writeLogError(
            sc::string::format(
                "GLGaeaServer::AddLogAdoJob m_pAdoMan NULL %1%",
                typeid(*spJob).name()));
    }
}

void GLGaeaServer::AddShopAdoJob(db::DbActionPtr spJob)
{
	if (IsInstance())
	{
		sc::writeLogError(sc::string::format("Ignore AdoShopDbAction %1%", typeid(*spJob).name()));
		return;
	}

    if (m_pAdoMan)
    {
        m_pAdoMan->AddShopJob(spJob, m_pAdoMan);
        return;
    }
    
    if (spJob)
    {
        sc::writeLogError(
            sc::string::format(
                "GLGaeaServer::AddShopAdoJob m_pAdoMan NULL %1%",
                typeid(*spJob).name()));
    }
}

GLMaterial* GLGaeaServer::NEW_MATERIAL()
{
	EnterCriticalSection(&m_CS_Material);
	GLMaterial* returnMaterial = m_poolMATERIAL.construct(this);
	if ( NULL == returnMaterial )
	{
		m_poolMATERIAL.set_next_size( 32 );
		returnMaterial = m_poolMATERIAL.construct(this);
	}
	LeaveCriticalSection(&m_CS_Material);
	return returnMaterial;
}

void GLGaeaServer::RELEASE_MATERIAL ( GLMaterial* pMaterial )
{
	GASSERT(pMaterial&&"GLGaeaServer::RELEASE_MATERIAL()");
	if (!pMaterial)
        return;
    EnterCriticalSection(&m_CS_Material);	
	m_poolMATERIAL.destroy(pMaterial);
    LeaveCriticalSection(&m_CS_Material);
}

GLChar* GLGaeaServer::NEW_CHAR(wb::EM_COUNTRY Country)
{
	EnterCriticalSection(&m_CS_Char);	
    GLChar* pChar = m_poolCHAR.construct(m_dwMaxClient, this, Country);
	if ( NULL == pChar )
	{
		m_poolCHAR.set_next_size( 32 );
		pChar = m_poolCHAR.construct(m_dwMaxClient, this, Country);
	}
	LeaveCriticalSection(&m_CS_Char);
	return pChar;
}

void GLGaeaServer::RELEASE_CHAR(GLChar* pCHAR)
{
	GASSERT(pCHAR&&"GLGaeaServer::RELEASE_CHAR()");
	if (!pCHAR)
        return;

	EnterCriticalSection(&m_CS_Char);
    m_poolCHAR.destroy(pCHAR);
	LeaveCriticalSection(&m_CS_Char);
}

GLMobSchedule* GLGaeaServer::NEW_MOBSCHEDULE ()
{
	EnterCriticalSection( &m_CS_MobSchedule );
	GLMobSchedule* pSch = m_poolMobSchedule.construct();
	if ( NULL == pSch )
	{
		m_poolMobSchedule.set_next_size( 32 );
		pSch = m_poolMobSchedule.construct();
	}

	LeaveCriticalSection( &m_CS_MobSchedule );

	return pSch;
}

void GLGaeaServer::RELEASE_MOBSCHEDULE ( GLMobSchedule* pSch )
{
	GASSERT( pSch && "GLGaeaServer::RELEASE_MOBSCHEDULE()" );

	if ( NULL == pSch )
		return;

	EnterCriticalSection( &m_CS_MobSchedule );
	m_poolMobSchedule.destroy( pSch );
	LeaveCriticalSection( &m_CS_MobSchedule );
}

GLCrow* GLGaeaServer::NEW_CROW()
{
    EnterCriticalSection(&m_CS_Crow);
    GLCrow* pCrow = m_poolCROW.construct(this);
	if ( NULL == pCrow )
	{
		m_poolCROW.set_next_size( 32 );
		pCrow = m_poolCROW.construct(this);
	}
    LeaveCriticalSection(&m_CS_Crow);
    return pCrow;
}

void GLGaeaServer::RELEASE_CROW(GLCrow* pCrow)
{
    GASSERT(pCrow && "GLGaeaServer::RELEASE_CROW()");
    if ( !pCrow )
        return;
    
	EnterCriticalSection(&m_CS_Crow);
    m_poolCROW.destroy(pCrow);
    LeaveCriticalSection(&m_CS_Crow);
}

SFIELDCROW* GLGaeaServer::NEW_FIELDCROW ()
{
	//EnterCriticalSection(&m_CS_FieldCrow);
	//SFIELDCROW* returnCrow = m_poolFIELDCROW.getMem();
	SFIELDCROW* returnCrow = m_poolFIELDCROW.construct();
	//LeaveCriticalSection(&m_CS_FieldCrow);

	return returnCrow;
}

void GLGaeaServer::RELEASE_FIELDCROW ( SFIELDCROW* pFIELDCROW )
{
	GASSERT(pFIELDCROW&&"GLGaeaServer::RELEASE_FIELDCROW()");
	if ( !pFIELDCROW )
		return;

	//EnterCriticalSection(&m_CS_FieldCrow);
	pFIELDCROW->RESET();
	//m_poolFIELDCROW.releaseMem( pFIELDCROW );
	m_poolFIELDCROW.destroy( pFIELDCROW );
	//LeaveCriticalSection(&m_CS_FieldCrow);
}


SFIELDLANDEVENT* GLGaeaServer::NEW_FIELDLANDEVENT ()
{
	//EnterCriticalSection(&m_CS_LandEvent);
	//SFIELDLANDEVENT* returnLandEvent = m_poolFIELDLANDEVENT.getMem();
	SFIELDLANDEVENT* returnLandEvent = m_poolFIELDLANDEVENT.construct();
	//LeaveCriticalSection(&m_CS_LandEvent);

	return returnLandEvent;
}

void GLGaeaServer::RELEASE_FIELDLANDEVENT ( SFIELDLANDEVENT* pFIELDLANDEVENT )
{
	GASSERT(pFIELDLANDEVENT&&"GLGaeaServer::RELEASE_LANDEVENT()");
	if ( !pFIELDLANDEVENT )
		return;

	//EnterCriticalSection(&m_CS_LandEvent);
	pFIELDLANDEVENT->RESET();
	//m_poolFIELDLANDEVENT.releaseMem( pFIELDLANDEVENT );
	m_poolFIELDLANDEVENT.destroy( pFIELDLANDEVENT );
	//LeaveCriticalSection(&m_CS_LandEvent);
}

GLPetField* GLGaeaServer::NEW_PET ()
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CS_Pet);
	GLPetField* returnPet = m_poolPET.construct(this);
	if ( NULL == returnPet )
	{
		m_poolPET.set_next_size( 32 );
		returnPet = m_poolPET.construct(this);
	}
	return returnPet;
}

void GLGaeaServer::RELEASE_PET ( GLPetField* pPet )
{
	GASSERT ( pPet && "GLGaeaServer::RELEASE_PET()" );
	if (!pPet)
        return;

	sc::CriticalSectionOwnerLock OwnerLock(m_CS_Pet);
    pPet->CleanUp();
	m_poolPET.destroy(pPet);
}

GLLandManager* GLGaeaServer::NEW_GLLANDMANAGER(const DWORD nInstanceMapID, const DWORD nBaseMapID)
{ // 새로 생성 및 기존의 생성 되어 있는 객체 반환;
	const DWORD landManagerID(nInstanceMapID == GAEAID_NULL ? nBaseMapID : nInstanceMapID);	
	if ( landManagerID >= MAX_LAND_MID )
		return NULL;

    if ( m_pLandManager[landManagerID] == 0 )
    {
        GLLandManager* pNewLandManager(LandPoolField::Instance()->newLandManager(this, nInstanceMapID, nBaseMapID));
        return pNewLandManager;				
    }
    return m_pLandManager[landManagerID];
}

void GLGaeaServer::RELEASE_GLLANDMANAGER(GLLandManager* const _pLandManager)
{
	const DWORD dwInstanceMapID(_pLandManager->getInstanceMapMainID());
    LandPoolField::Instance()->release(_pLandManager);
	if ( dwInstanceMapID < MAX_LAND_MID )
		m_pLandManager[dwInstanceMapID] = 0;
}

PGLSUMMONFIELD GLGaeaServer::NEW_SUMMON ()
{
	EnterCriticalSection(&m_CS_Summon);
	GLSummonField* pSummon = m_poolSummon.construct(this);
	if ( NULL == pSummon )
	{
		m_poolSummon.set_next_size( 32 );
		pSummon = m_poolSummon.construct(this);
	}
	LeaveCriticalSection(&m_CS_Summon);
	return pSummon;
}

void GLGaeaServer::RELEASE_SUMMON(PGLSUMMONFIELD pSummon)
{
	GASSERT ( pSummon && "GLGaeaServer::RELEASE_SUMMON()" );
	if ( !pSummon)
        return;

	EnterCriticalSection(&m_CS_Summon);	
    m_poolSummon.destroy(pSummon);
	LeaveCriticalSection(&m_CS_Summon);
}

// 모든 클럽의 배틀의 현재 상황을 저장한다. ( 킬/데스를 저장한다. ) 
BOOL GLGaeaServer::SaveClubBattle()
{
	if(IsInstance())
	{
		return TRUE;
	}
	
	EnterCriticalSection(&m_CS_ClubBattle);
	GASSERT(m_pClubMan);
	if (m_pClubMan)
	{
		m_pClubMan->SaveClubBattle();
	}
	LeaveCriticalSection(&m_CS_ClubBattle);

	return TRUE;
}

//
//mjeon.tbb
//
// GetRootMap(): 서버상에서는 안쓰이는듯.. 툴에서만 사용되는 것 같음.
//
GLLandMan* GLGaeaServer::GetRootMap()
{
    if ( m_landManagerMap.empty() )
        return NULL;

    ITER_CVECLANDMANAGER _iteratorLandManager(m_landManagerMap.begin());
    if ( _iteratorLandManager == m_landManagerMap.end() )
        return NULL;

    GLLandManager* pLandManager(_iteratorLandManager->pLandManager);
    return pLandManager->getLand();
}

//일반Map
GLLandMan* const GLGaeaServer::GetLand(const MapID &mapID)
{
	GLLandMan* const pLand(GLGaeaServer::GetLand(mapID.getGaeaMapID()));
	return pLand;
}
GLLandMan* const GLGaeaServer::GetLand(const SNATIVEID& mapID)
{
    GASSERT(IsField());

	const DWORD _mapIDMain(mapID.Mid());
	const DWORD _mapIDSub(mapID.Sid());

    if ( _mapIDMain >= MAX_LAND_MID )
        return NULL;

    if ( _mapIDSub >= MAX_LAND_SID )
        return NULL;

    if ( m_pLandManager[_mapIDMain] == NULL )
        return NULL;

    return m_pLandManager[_mapIDMain]->getLand(_mapIDSub);
}

GLLandManager* const GLGaeaServer::GetLandManager(const MapID& mapID)
{
	const DWORD _mapIDMain(mapID.getGaeaMapID().Mid());
	return GetLandManager(_mapIDMain);
}

GLLandManager* const GLGaeaServer::GetLandManager(const SNATIVEID& mapID)
{
	const DWORD _mapIDMain(mapID.Mid());
    return GetLandManager(_mapIDMain);
}

GLLandManager* const GLGaeaServer::GetLandManager(const DWORD mapID)
{
    GASSERT(IsField());
    if ( mapID >= MAX_LAND_MID )
        return NULL;	

    return m_pLandManager[mapID];
}

// new Instance System;
InstanceSystem::ManagerField* const GLGaeaServer::GetInstanceManagerField()
{
	return m_pInstanceManager;
}

// event Manager;
GLEvent::ManagerField* const GLGaeaServer::GetEventManagerField(void)
{
	return m_pEventManager;
}

Money::CMoneyManagerField* const GLGaeaServer::GetMoneyManager ( void )
{
	return m_pMoneyMan;
}

HRESULT GLGaeaServer::OneTimeSceneInit ()
{
	m_FreePETGIDs.RemoveAll ();

	for (DWORD i=0; i<m_dwMaxClient; ++i)
        m_FreePETGIDs.AddTail(i);

	m_PetFieldMap.clear();

	return S_OK;
}

HRESULT GLGaeaServer::Create(
    DWORD dwMaxClient,
    DxMsgServer* pMsgServer,
    db::IDbManager* pDBMan,
	db::IDbManager* pAdoMan,
    EMSERVICE_PROVIDER emServiceProvider,
    const char* szMapList,
    DWORD dwFieldSID,
    int nChannel,
    BYTE ExcessExpProcessType,
    bool bToolMode)
{
	HRESULT hr;
	CleanUp();    

	m_bReservedStop = false;

	m_dwMaxClient = dwMaxClient;
	m_pMsgServer = pMsgServer;
	//m_pConsoleMsg = pConsoleMsg;
	m_pDBMan = pDBMan;
	m_pAdoMan = pAdoMan;

	m_nServerChannel = nChannel;

	//mjeon.instance
	m_dwSvrGroupNum = m_pMsgServer->GetServerGroup();
	m_dwBaseGaeaId = m_dwSvrGroupNum * BASE_GAEAID;//m_dwMaxClient;	//서버군마다 최대 접속자 수가 다를 수 있으므로 충분히 큰 수(10만)로 대체한다.	

	sc::SeqUniqueGUID::Instance()->setServerGroup( static_cast< unsigned char >( m_dwSvrGroupNum ) );

	m_pCharMan	= new CharManField(GetServerType());

    GLCONST_CHAR::SET_EXCESS_EXP_TYPE( ExcessExpProcessType );

	m_ServiceProvider = emServiceProvider;

	// 필드 서버 ID가 무효일 경우 ( 에뮬레이터 설정. )
	m_dwFieldSvrID = dwFieldSID;
	if (dwFieldSID == FIELDSERVER_MAX)
	{
		m_bEmulator = true;
		m_dwFieldSvrID = 0;
	}
	
	if (!m_pDBMan)
	{
		if (m_pMsgServer->GetServerType() == net::SERVER_INSTANCE)
		{
			sc::writeLogInfo(std::string("GLGaeaServer is created on Instance mode."));

			//InstanceServer는 DB를 사용하지 않으므로 반드시 DB manager들은 NULL이어야 한다.
			GASSERT(m_pDBMan == NULL);
			GASSERT(m_pAdoMan == NULL);
		}
		else
		{
			if (m_bEmulator)
			{
				sc::writeLogWarn(std::string("--- DB Manager runs on Emulator mode! ---"));
				m_pDBMan = new GLEmulatorDBMan(NULL, db::EMULATOR_SERVER, emServiceProvider); //Dummy DbManager for Emulator.
			}
			else
			{
				sc::writeLogError(std::string("GLGaeaServer::Create() - DB Manager is NULL!"));
				return E_FAIL;
			}
		}
	}

    // 선도지역 로드.
    m_pGuidance->Load(GLCONST_CHAR::vecGUIDANCE_FILE, bToolMode);

	hr = OneTimeSceneInit();
	
	if (FAILED(hr))
		return E_FAIL;
	
	//
	//mjeon
	//class GLGaeaServer public GLMapList.
	//GLMapList::LoadMapList reads maplist and loads all map files to a list.
	//
	hr = LoadMapList(FALSE, NULL, FIELDSERVER_MAX);
	if (FAILED(hr))
		return E_FAIL;

	//
	//Load Script
	//
	std::string strScriptPath(sc::getAppPath());
	strScriptPath.append(m_pPath->GLogicServerPath()); //SUBPATH::GLogicServerPath());
	strScriptPath.append("scripts\\");

	//
	//mjeon.attendance.script
	//
	std::string strAttendanceScriptPath(strScriptPath);    
    strAttendanceScriptPath.append(GLogicData::GetInstance().GetAttendanceScriptName());

	m_pAttendanceScriptMan->LoadScript(strAttendanceScriptPath.c_str());


    //
    //bjju.buffmanager
    //

    if ( GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        std::string strBuffManagerScriptPath(strScriptPath);    
        strBuffManagerScriptPath.append(GLogicData::GetInstance().GetBuffManagerScriptName());

        m_pBuffMan->LoadScript(strBuffManagerScriptPath.c_str());
    }

	// ItemValidation.luc load
	std::string strItemValidationFileName( "ItemValidation.luc" );
	std::string strItemValidationFilePath( sc::getAppPath() );
	strItemValidationFilePath.append( m_pPath->GLogicServerPath() );
	strItemValidationFilePath.append( strItemValidationFileName );
	Item::GLItemValidation::Instance()->Load( strItemValidationFilePath, this );

    //! 확률/아이템 리스트 제어 기능 : sckim Redmine #4544
    if ( !RandomChanceLoadScript() )
    {
        sc::writeLogWarn(std::string("--- Random chance lua script load fail!!! ---"));
    }

	//
	//mjeon
	// GLITEMLMT is a singleton.
	SAFE_DELETE(m_pItemLimit);
    m_pItemLimit = new GLItemLimit(pMsgServer, pDBMan, pAdoMan, dwFieldSID);
	m_pItemLimit->ReadMaxKey();
    m_pItemLimit->ReadLogItem();

    GLMapList::FIELDMAP_ITER iter = GetMapList().begin ();
	GLMapList::FIELDMAP_ITER iter_end = GetMapList().end ();
		
	for ( ; iter!=iter_end; ++iter )
	{
		DWORD StartTime = ::GetTickCount();
		
		SMAPNODE *pMapNode = &(iter->second);

		// 필드 서버 ID가 무효일 경우 ( 에뮬레이터 설정. )
		if ( dwFieldSID >= FIELDSERVER_MAX )
		{
			pMapNode->m_FieldServerID = 0;
		}
	
		// LandMan 생성후 초기화.
		//		
		const bool bNormalMap(pMapNode->GetInstanceType() == SMAPNODE::EMINSTANCE_NORMAL);
		const bool bValidFieldServer(m_bEmulator == true ? true : (pMapNode->GetFieldServerNum() < FIELDSERVER_MAX));
		const bool bSameFieldServer(m_bEmulator == true ? true : (pMapNode->GetFieldServerNum() == dwFieldSID));		

        const bool bRawData((bNormalMap == false) || (bValidFieldServer == false) || (bSameFieldServer == false)); // 인던에서 사용할 복사본 맵인가?;
        const SNATIVEID& _mapID = pMapNode->GetId();
        const DWORD nMainMapID(_mapID.wMainID);
        const DWORD nSubMapID(_mapID.wSubID);
		const DWORD nInstanceMapID(bRawData == true ? GAEAID_NULL : nMainMapID);
        GLLandManager* const pNewLandMan = NEW_GLLANDMANAGER(nInstanceMapID, nMainMapID);
		if ( NULL == pNewLandMan )
		{
			sc::writeLogError(sc::string::format("GLGaeaServer::CreateInstanceMaps() - failed to NEW_GLLANDMAN(); (instanceMapID:%1%, baseMapID:%2%/%3%)", nInstanceMapID, nMainMapID, nSubMapID));
			continue;
		}		

		const int nPreviousUsedMemoryPerLand = static_cast<int>(sc::getMemoryWorkingSetKB());
		//
		//mjeon.ToDo
		//There is meaningless boolean-comparison. Remove these codes. (pMapNode->bPeaceZone!=FALSE, pMapNode->bPKZone
		//
        if ( pNewLandMan->addLand(nSubMapID, pMapNode->IsPeaceZone()!=false, pMapNode->IsPkZone()==true) == false )
        {
			RELEASE_GLLANDMANAGER ( pNewLandMan );
			sc::writeLogError(sc::string::format("GLGaeaServer::Create() - failed to addLand()! (instanceMapID:%1%, baseMapID:%2%/%3%)", nInstanceMapID, nMainMapID, nSubMapID));
            continue;
        }

		//pNewLandMan->SetEmulator ( m_bEmulator );
		BOOL bOk = pNewLandMan->loadLevelFile (nSubMapID, pMapNode->m_LevelFile.c_str(), m_pMapList->GetAxisMan() );
		
		if ( !bOk )
		{
			RELEASE_GLLANDMANAGER ( pNewLandMan );
			//SAFE_DELETE( pNewLandMan );

			//
			//mjeon.ToDo
			//CString should be replaced with c-string or c++ string for better performance.
			//
			//And make a common error-handler or error-printer to capsule messy routines as following.
			CString strTemp = pMapNode->m_LevelFile.c_str();
			strTemp += " : GLLandMan::LoadFile() Failure.";
			MessageBox ( NULL, strTemp, "ERROR", MB_OK );
			continue;
		}
		
		hr = InsertMap ( pNewLandMan );

		DWORD CheckTime = ::GetTickCount();
		DWORD ElspTime  = CheckTime - StartTime;

		if ( SUCCEEDED(hr) )
		{
			const int nUsedMemoryPerLand(int(sc::getMemoryWorkingSetKB()) - nPreviousUsedMemoryPerLand);
			pNewLandMan->addUsedMemory(nUsedMemoryPerLand < 0 ? 0 : nUsedMemoryPerLand); // 대략적인 양이므로 넉넉히 잡아준다;

			GLLandMan* const _pLand(pNewLandMan->getLand(nSubMapID));
			if ( _pLand != NULL )
				_pLand->SetUsedMemory(nUsedMemoryPerLand < 0 ? 0 : nUsedMemoryPerLand);

			sc::writeLogInfo(sc::string::format ( "[Add Map] %1%  / [%2%/%3%] Time %4%msec, used memory %5%KB;", pMapNode->m_LevelFile.c_str(), pMapNode->GetId().wMainID, pMapNode->GetId().wSubID, ElspTime, nUsedMemoryPerLand ) );
		}		
	} // for(;;) ends.	
	

	m_pInstanceManager = new InstanceSystem::ManagerField(this, m_nServerChannel);
	m_pMiniGameManager = new MiniGame::GLManagerField(this);
	m_pPingManager = new Ping::ManagerField(this);	
    m_pPartyFieldMan = new GLPartyManagerField(this, m_dwMaxClient);	
	m_pPartyTenderManager = new GLPartyTenderManagerField(this);
	m_pEventManager = new GLEvent::ManagerField(this);
	m_pMoneyMan = new Money::CMoneyManagerField( this, 60.f );

	// mjeon
	// 각 학원의 시작 맵과 시작 Gate가 정상적으로 존재하는지 확인
	//
	for ( WORD i=0; i<GLCONST_CHAR::wSCHOOLNUM; ++i )
	{
		//
		//mjeon
		//Each School has the startMap and startGate according to this routine.
		//
		//Variables such as wSCHOOLNUM, nidSTARTMAP, dwSTARTGATE were set by
		//GLogicData::LoadData() before calling for Create().
		//
		SNATIVEID nidSTARTMAP = GLCONST_CHAR::nidSTARTMAP[i];
		DWORD dwSTARTGATE = GLCONST_CHAR::dwSTARTGATE[i];

		const SMAPNODE *pMapNode = FindMapNode ( nidSTARTMAP.dwID );
		
		if( pMapNode == NULL )
			continue;

		// 지정된 서버군에 포함되지 않는 map 은 제외 시킴.
		if ( dwFieldSID != FIELDSERVER_MAX && pMapNode->GetFieldServerNum() != dwFieldSID )
			continue;

		GLLandMan* pLandMan = GetLand ( nidSTARTMAP );
		if ( !pLandMan )
		{			
            std::string Msg(
                sc::string::format(
                    "Charactor's Start Map setting Error. %1% MapID %2%/%3% Not Found.",
                    GLogicData::GetInstance().GetMapListFileName(),
                    nidSTARTMAP.wMainID,
                    nidSTARTMAP.wSubID));
			MessageBox(NULL, Msg.c_str(), "ERROR", MB_OK|MB_ICONEXCLAMATION);
			//
			//mjeon
			//
			//Just continue??
			//Is this OK? That there's no startMap! Isn't it going to be a problem?
			//
			continue;
		}

		DxLandGateMan* pGateMan = pLandMan->GetLandGateMan ();
		PDXLANDGATE pGate = pGateMan->FindLandGate ( dwSTARTGATE );
		if ( !pGate )
		{
            std::string Msg(
                sc::string::format(
                    "Charactor Start Map GATE Not Found.\n %s Map %u GATE ID Must Check.",
                    pLandMan->GetFileName(),
                    dwSTARTGATE));
			MessageBox(NULL, Msg.c_str(), "ERROR", MB_OK);
			//
			//mjeon
			//
			//Just continue??
			//Is this OK? That there's no startGate! Isn't it going to be a problem?
			//
			continue;
		}
	} // for(;;) ends.



	// 클럽의 정보를 db에서 읽어온다.
	//
    SAFE_DELETE(m_pClubMan);
    m_pClubMan = new GLClubManField(this);
	m_pClubMan->LoadFromDB(m_pDBMan, m_pAdoMan);
	

	if ( m_pDBMan )
	{
		// 선도 클럽 지역 설정.
		//
		VECGUID_DB vecGuidDb;
		m_pDBMan->GetClubRegion(vecGuidDb);
        m_pGuidance->SetState(vecGuidDb);
    }

    if (m_pAdoMan)
    {
		// mjeon.activity
		// called directly to load Closed-Activities into ActivityBaseData.
		m_pAdoMan->ActivityGetClosedActivityID(*(ActivityBaseData::GetInstance()));
	}
	
	SetMapState();
	m_pGuidance->SetMapState();
	
    // 클럽 데스매치 로드.
    LoadClubDeathMatch(GLCONST_CHAR::vecClubDM_FILE, bToolMode); 	

	//
	//mjeon.CaptureTheField
	//  CTF must be initialized after loading all maps.
	//
	m_pCTFMan->Initialize();


	TriggerSystem::InitServerTriggerSystem();

	if ( GLUseFeatures::GetInstance().IsUsingTournament() )
	{
		SAFE_DELETE(m_pMatchingCore);
		m_pMatchingCore = new MatchingSystem::MatchingCore(this);
		m_pMatchingCore->OnLoad();
	}

	/************************************************************************/
	/* GLFSMContainer;                                                      */
	/************************************************************************/
	AISystem::GLFSMContainer::Instance()->LoadScriptFile();
	/************************************************************************/
	
	/************************************************************************/
	/* GLCountryManagerServer                                               */
	/************************************************************************/
	Country::GLCountryManServer::Instance()->LoadData();

	Country::GLVictoriousCountryManField::Instance()->SetServer( this );
	Country::GLVictoriousCountryManField::Instance()->LoadDBAction();
	/************************************************************************/

	SAFE_DELETE( m_pUserLockerLockMgr );
	m_pUserLockerLockMgr = new GLUserLockerLockMgr( m_pAdoMan );
	if ( m_pUserLockerLockMgr )
	{
		// 서버가 시작되면서 잠겨있던 유저락커 해제
		m_pUserLockerLockMgr->UnLock();
	}

	SAFE_DELETE( m_pCharacterBackup );
	m_pCharacterBackup = new CharacterBackup( m_pAdoMan );
	if ( m_pCharacterBackup )
	{
		// 서버가 시작되면서 그전에 sqlite 에 저장해두었던 캐릭터들 게임디비에 저장
		m_pCharacterBackup->SaveCharacterInGameDatabase();
	}

	SAFE_DELETE( m_pPrivateMarketSearchDelayMgr );
	m_pPrivateMarketSearchDelayMgr = new private_market::GLSearchDelayMgr();

	// 개인상점 검색 딜레이 설정
	if ( m_pPrivateMarketSearchDelayMgr )
	{
		m_pPrivateMarketSearchDelayMgr->SetDelay( private_market::GLPrivateMarketDefine::Instance()->GetSearchDelayMillisecond() );
	}

	SAFE_DELETE( m_pPrivateMarketLogDelayMgr );
	m_pPrivateMarketLogDelayMgr = new private_market::GLSearchDelayMgr();

	// 개인상점 로그 딜레이 설정
	if ( m_pPrivateMarketLogDelayMgr )
	{
		m_pPrivateMarketLogDelayMgr->SetDelay( private_market::GLPrivateMarketDefine::Instance()->GetLogDelayMillisecond() );
	}

	// 퀘스트 매핑 기능 로그
	GLQuestMan::GetInstance().LoadGroupQuest(GLCONST_CHAR::strQuestGroupFileName);

	// Initialize Lotto;
	if ( GLUseFeatures::GetInstance().IsUsingLottoSystem() )
	{
		LottoSystem::GLLottoSystemManField::Instance()->Initialize();
	}

	return S_OK;
}

HRESULT GLGaeaServer::Create4Level ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	CleanUp ();

	m_pd3dDevice = pd3dDevice;

	OneTimeSceneInit ();

	// LandMan 생성후 초기화.
	//
	GLLandManager* const pNewLandMan(NEW_GLLANDMANAGER(0, 0));
	if ( NULL == pNewLandMan )
	{
		sc::writeLogError(std::string("GLGaeaServer::Create4Level() - NEW_GLLANDMAN pNewLandMan is NULL! (instanceMapID:0, baseMapID:0/0)"));
		return E_FAIL;
	}
    pNewLandMan->addLand(0, false, true);
    pNewLandMan->setInformation(m_pd3dDevice, true);

	// 생성된 LandMan 등록.
	//
	InsertMap ( pNewLandMan );

	return S_OK;
}

void GLGaeaServer::SetMapState()
{
    size_t tSize = m_landManagerMap.size();
    for( size_t i=0; i<tSize; ++i )
    {
        m_landManagerMap[i].pLandManager->setMapState();
    }
}

//
//mjeon.tbb
//
HRESULT GLGaeaServer::InsertMap ( GLLandManager* const pNewLandMan )
{	
	GASSERT(pNewLandMan);

    const DWORD _instanceMapID(pNewLandMan->getInstanceMapMainID());
    const DWORD dwMapID(_instanceMapID == GAEAID_NULL ? pNewLandMan->getBaseMapMainID() : _instanceMapID);

    GASSERT(dwMapID < MAX_LAND_MID);
	if ( dwMapID >= MAX_LAND_MID )
		return E_FAIL;
	
	//
	//mjeon.tbb
	//
	m_landManagerMap.push_back( LandManagerPair(dwMapID, pNewLandMan) );
	
	m_pLandManager[dwMapID] = pNewLandMan;

	return S_OK;
}

HRESULT GLGaeaServer::CleanUp ()
{
    ClearDropObj ();	

    size_t tSize = m_landManagerMap.size();
    for( size_t i=0; i<tSize; ++i )
    {
        RELEASE_GLLANDMANAGER(m_landManagerMap[i].pLandManager);
    }
    m_landManagerMap.clear();
	for ( DWORD _i = 0; _i < MAX_LAND_MID; ++_i )
		m_pLandManager[_i] = NULL;

	m_pDBMan = NULL;
	m_pAdoMan = NULL;

	m_FreePETGIDs.RemoveAll();

	SAFE_DELETE(m_pInstanceManager);
	SAFE_DELETE(m_pMiniGameManager);
    SAFE_DELETE(m_pPartyFieldMan);
	SAFE_DELETE(m_pPartyTenderManager);
    SAFE_DELETE(m_pDeathMatchMan);
    SAFE_DELETE(m_pClubMan);
	SAFE_DELETE(m_pCharMan);
	
	m_pMsgServer = NULL;

	TriggerSystem::DestroyServerTriggerSystem();

	return S_OK;
}

GLChar* GLGaeaServer::CreateCharacter(
    PCHARDATA2 pCharData,
	DWORD dwChaNum,	//DWORD ClientSlot,    
    DWORD GaeaId,
	GLPartyID PartyID,
    wb::EM_COUNTRY Country,	// 사용하지 않는다;
	DWORD dwCountryID,
    BOOL bNEW,
    MapID _StartMap,
    DWORD _dwStartGate,
    D3DXVECTOR3 _vPos,
    EMGAME_JOINTYPE emJOINTYPE,
    __time64_t loginTime,
    EMIP_BONUS_CLASS emIPBonus)
{
	//
	//mjeon.AF.이 시점에는 아직 Field <-> Client간 세션이 맺어지지 않았다
	//

	HRESULT hr = S_OK;
	if (!pCharData)
	{
        sc::writeLogError(std::string("GLGaeaServer::CreateCharacter pCharData NULL"));
		return NULL;
	}

	BOOL bDB(FALSE);
	GLChar* pChar = NULL;
	GLLandMan* pLandMan = NULL;

	DxLandGateMan* pGateMan = NULL;
	PDXLANDGATE pGate = NULL;
	D3DXVECTOR3 vStartPos(0,0,0);

	CLIENTMAP_ITER client_iter;
	GLCHAR_MAP_ITER name_iter;

	if ( !IsValidGaeaID(GaeaId) ) // >= GetMaxClient() )
	{
		sc::writeLogError(sc::string::format("GaeaID(%1%) is out of range(%2%,%3%)", GaeaId, m_dwBaseGaeaId, m_dwBaseGaeaId + m_dwMaxClient));
		return CreateCharacterFail(pChar, dwChaNum);
	}

	// user id가 지금 db에 저장중인지 점검 (logout 했지만 아직 db에 저장이 안된 상태로 있을때)
	bDB = m_pCharMan->ReqSaveDbUserFind(pCharData->GetUserID());
	if (bDB)
	{
		sc::writeLogError(std::string("logout but not yet saved to DB"));
		return CreateCharacterFail(pChar, dwChaNum);
	}

	// 가이아 ID의 할당을 Agent 서버에서 할당하게 됨에 따라 잘못되어 가이아 ID가 반환되지 않은 상태에서 다시 사용될 가능성이 있음
	if ( CheckCharacterExistance(GaeaId) )
	{
		sc::writeLogError(std::string("Character with the GaeaID already exists."));
		return CreateCharacterFail(pChar, dwChaNum);
	}

	// 같은 캐릭터가 이미 접속되어 있는지 검사합니다.
	if (m_pCharMan->ChaNameExist(pCharData->m_szName))
	{
		sc::writeLogError(std::string("char name already exist"));
		return CreateCharacterFail(pChar, dwChaNum);
	}

	pChar = NEW_CHAR(Country);
	if ( NULL == pChar )
	{
		sc::writeLogError(std::string("NEW_CHAR fail."));
		return CreateCharacterFail(pChar, dwChaNum);
	}

	// 특정한 MapID에서 생성하고자 할 경우.
	bool bValidMoveMap = false;
	if (_StartMap != MapID())
	{
		pLandMan = GetLand(_StartMap.getGaeaMapID());
		if (pLandMan)
		{
			pGateMan = pLandMan->GetLandGateMan();

			if ( _dwStartGate!=UINT_MAX )
			{
				pGate = pGateMan->FindLandGate((DWORD)_dwStartGate);

				if ( pGate )
                {
                    vStartPos = pGate->GetGenPos(DxLandGate::GEN_RENDUM);
                }
				else
				{
					pGate = pGateMan->FindLandGate((DWORD)0);
					if (!pGate)
					{
						sc::writeLogError(
                            sc::string::format(
                                "pGate = NULL, UserID %s UserLv %d Money %d",
                                pCharData->m_szName,
                                pCharData->m_wLevel,
                                pCharData->GetInvenMoney()));

						// 유저락커 잠금 해제
						UpdateUserLockerOption( pCharData->GetUserID(), 0 );

						return CreateCharacterFail(pChar, dwChaNum);
					}
					_dwStartGate = pGate->GetGateID ();
					if ( pGate )
                        vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
				}
			}
			else
			{
				//	특정 위치에서 생성코자 할 경우.
				vStartPos = _vPos;
			}

			// GaeaMapID 가 없는 경우는 생성되지 않은 인던에 들어가려는 경우이다;
			// 즉, 유효하지 않은 맵이다;
			// GaeaMapID 가 있는 경우에만 유효한 맵으로 판단한다;
			if ( pLandMan->GetGaeaMapID() != SNATIVEID(false) )
				bValidMoveMap = true;
		}
	}
	
	if ( false == bValidMoveMap )
	{
		pLandMan = GetLand ( pCharData->m_sStartMapID );
		if ( pLandMan )
		{
			pGateMan = pLandMan->GetLandGateMan ();
			pGate = pGateMan->FindLandGate ( DWORD(pCharData->m_dwStartGate) );

			if ( pGate )
            {
                vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
            }
			else
			{
				pGate = pGateMan->FindLandGate ( DWORD(0) );
				if ( pGate )
                    vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
			}
		}
	}

	if ( !pLandMan || ((_dwStartGate!=UINT_MAX)&&!pGate) )
	{
		SNATIVEID nidSTARTMAP = GLCONST_CHAR::nidSTARTMAP[pCharData->m_wSchool];
		DWORD dwSTARTGATE = GLCONST_CHAR::dwSTARTGATE[pCharData->m_wSchool];

		pLandMan = GetLand(nidSTARTMAP);
		if (!pLandMan)
		{
            sc::writeLogError(
                sc::string::format(
                    "CreateCharacter GetLand pLandMan NULL. School %1% Map %2%/%3%",
                    pCharData->m_wSchool,
                    nidSTARTMAP.Mid(),
                    nidSTARTMAP.Sid()));

			// 유저락커 잠금 해제
			UpdateUserLockerOption( pCharData->GetUserID(), 0 );

			return CreateCharacterFail(pChar, dwChaNum);
		}

		pGateMan = pLandMan->GetLandGateMan ();
		if ( !pGateMan )
		{
            sc::writeLogError(
                sc::string::format(
                "CreateCharacter GetLandGateMan pGateMan NULL. School %1% Map %2%/%3%",
                    pCharData->m_wSchool,
                    nidSTARTMAP.Mid(),
                    nidSTARTMAP.Sid()));

			// 유저락커 잠금 해제
			UpdateUserLockerOption( pCharData->GetUserID(), 0 );

			return CreateCharacterFail(pChar, dwChaNum);

		}
		pGate = pGateMan->FindLandGate ( dwSTARTGATE );
		if ( !pGate )
		{
            sc::writeLogError(
                sc::string::format(
                    "CreateCharacter FindLandGate pGate NULL. School %1% Map %2%/%3% Gate %4%",
                    pCharData->m_wSchool,
                    nidSTARTMAP.Mid(),
                    nidSTARTMAP.Sid(),
                    dwSTARTGATE));
			vStartPos = D3DXVECTOR3(0,0,0);
		}
		else
		{
			vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
		}
	}

	// 캐릭터 등록
	pCharData->m_emIPBonus = emIPBonus;
	pCharData->m_sEventTime.Init();
	pCharData->m_sEventTime.loginTime = loginTime;

	// 신규 접속의 경우 일반, 버프 타입의 적용스킬 값을 초기화시킨다;
	if ( EMJOINTYPE_FIRST == emJOINTYPE )
	{
		for ( int i=0; i<SKILLFACT_SIZE; ++i )
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pCharData->m_sSAVESKILLFACT[i].sNATIVEID );
			if ( NULL == pSkill )
				continue;

			/// 도시락의 경우 무제한으로 존재할 수 없다;
			/// Age 가 0 이하일 수 없다;
			EMSKILLFACT_TYPE emType = EMSKILLFACTTYPE_BY_INDEX ( pCharData->m_sSAVESKILLFACT[i].wSLOT );
			float fAge = pCharData->m_sSAVESKILLFACT[i].fAGE;
			if ( fAge < 0.f ||
				( _SKILLFACT_INFINITY == fAge &&
				( EMSKILLFACT_TYPE_EXP_LUNCHBOX == emType || EMSKILLFACT_TYPE_LUNCHBOX == emType ) ) )
			{
				sc::writeLogError( sc::string::format(
					"[ Skill ] [ Invalid SAVESKILLFACT. ( Slot : %1%, ID : %2%/%3%, Age : %4% ) ]",
					i,
					pCharData->m_sSAVESKILLFACT[i].sNATIVEID.Mid(),
					pCharData->m_sSAVESKILLFACT[i].sNATIVEID.Sid(),
					pCharData->m_sSAVESKILLFACT[i].fAGE ) );

				pCharData->m_sSAVESKILLFACT[i].Reset();

				continue;
			}

			SKILL::EMACTION_TYPE emActionType = pSkill->m_sBASIC.emACTION_TYPE;
			if ( SKILL::EMACTION_TYPE_NORMAL == emActionType ||
				SKILL::EMACTION_TYPE_BUFF == emActionType )
			{
				// 도시락버프(유료버프), CTF 버프는 유지시킨다;
				const bool bLunchBoxBuff(SKILLFACT_INDEX_LUNCHBOX_START <= pCharData->m_sSAVESKILLFACT[i].wSLOT
					&& pCharData->m_sSAVESKILLFACT[i].wSLOT < SKILLFACT_SIZE_LUNCHBOX + SKILLFACT_SIZE_EXP_LUNCHBOX + SKILLFACT_INDEX_LUNCHBOX_START)
					, bSystemBuff( SKILLFACT_INDEX_SYSTEM_START <= pCharData->m_sSAVESKILLFACT[i].wSLOT
						&& pCharData->m_sSAVESKILLFACT[i].wSLOT <= SKILLFACT_INDEX_SYSTEM_END);

				if ( !bLunchBoxBuff && !bSystemBuff)
					pCharData->m_sSAVESKILLFACT[i].Reset();
			}
		}
	}

	hr = pChar->CreateChar(pLandMan, vStartPos, pCharData, bNEW);
	if ( FAILED(hr) )
	{
        sc::writeLogError(
            sc::string::format(
                "CreateCharacter CreateChar. ChaName %1% ChaDbNum %2% School %3% Map %4%/%5% Pos %6%/%7%/%8%",
                pCharData->m_szName,
                pCharData->CharDbNum(),
                pCharData->m_wSchool,
                pLandMan->GetBaseMapID().Mid(),
                pLandMan->GetBaseMapID().Sid(),
                vStartPos.x,
                vStartPos.y,
                vStartPos.z));
        
		// 유저락커 잠금 해제
		UpdateUserLockerOption( pCharData->GetUserID(), 0 );

		return CreateCharacterFail(pChar, dwChaNum);
	}

	pChar->SetGaeaID(GaeaId);
	pChar->SetPartyID(PartyID);	

	// 국가정보 세팅;
	Country::SCOUNTRY_INFO sCountryInfo;
	sCountryInfo.dwID = dwCountryID;
	pChar->SetCountryInfo( sCountryInfo );
		
	BOOL bOk = DropPC(pLandMan, vStartPos, pChar);
	if ( !bOk )
	{
        sc::writeLogError(
            sc::string::format(
                "CreateCharacter CreateChar DropPC. ChaName %1% ChaDbNum %2% School %3% Map %4%/%5% Pos %6%/%7%/%8%",
                pCharData->m_szName,
                pCharData->CharDbNum(),
                pCharData->m_wSchool,
                pLandMan->GetBaseMapID().Mid(),
                pLandMan->GetBaseMapID().Sid(),
                vStartPos.x,
                vStartPos.y,
                vStartPos.z));

		// 유저락커 잠금 해제
		UpdateUserLockerOption( pCharData->GetUserID(), 0 );

		return CreateCharacterFail(pChar, dwChaNum);
	}

	//
	//mjeon.이 시점에는 아직 Field <-> Client간 세션이 맺어지지 않았다.
	//

	//
	//mjeon.CaptureTheField
	//
	// 이 함수는 기존의 ChaInfo에, 새롭게 추가된 Character 관련 정보들을 별도로 관리하기 위함이다.
	// CTF의 기여도(점수)와 PointShop 관련 정보들 또한 여기에서 관리한다.
	//
	//pChar->LoadExtraInfo();

	//
	//mjeon.activity
	//
	//pChar->ActivityLoad();

	//
	//mjeon.attendance
	//
	//pChar->AttendanceLoad();

	//mjeon.위와 같은 Load 관련 함수들은 가능한 빠른 시점에 호출하라. DB Thread 혹은 Cache서버를 거치면서 처리되다 보면 동기화 문제가 발생한다.
	//실제로 이 함수의 마지막에서 호출했을 경우에 동기화 문제가 발생했다.(대만)
	//빠른 시점에서 호출해도 문제가 발생한다면, 별도의 방법으로 동기화시켜야 한다.


	if ( m_ServiceProvider == SP_HONGKONG)
	{
		bool bDifferTempValue = FALSE;

		if( pChar->GetInvenMoney() != pChar->GetTempInvenMoney() )
		{
			{
				sc::writeLogWarn(sc::string::format(
					"Different Login Money!!, Account[%1%], ID[%2%], Money %3%, TempMoney %4%, Money Gap %5%", 
					pChar->m_szUID,
					pChar->m_szName,
					pChar->GetInvenMoney(),
					pChar->GetTempInvenMoney(),
					pChar->GetInvenMoney() - pChar->GetTempInvenMoney()) );
		
				bDifferTempValue  = TRUE;			

				// 무조건 TempMoney를 Money 값에 넣지 않는다.
				LONGLONG lnGap = pChar->GetInvenMoney() - pChar->GetTempInvenMoney();
				if( lnGap >= 10000 || lnGap <= -10000 )
				{
					if( pChar->GetInvenMoney() > pChar->GetTempInvenMoney() )
					{
						pChar->SetInvenMoney(pChar->GetTempInvenMoney());
					}
					else
					{
						pChar->SetTempInvenMoney( pChar->GetInvenMoney() );
					}
				}
				else
				{
					pChar->SetInvenMoney(pChar->GetTempInvenMoney());
				}
			}
			
		}

		if( pChar->GetStorageMoney() != pChar->GetTempStorageMoney() )
		{
			{
				sc::writeLogWarn(sc::string::format(
					"Different Login Storage Money!!, Account[%1%], ID[%2%], Storage Money %3%, TempStorage Money %4%, Storage Money Gap %5%", 
					pChar->m_szUID,
					pChar->m_szName,
					pChar->GetStorageMoney(),
					pChar->GetTempStorageMoney(), 
					pChar->GetStorageMoney() - pChar->GetTempStorageMoney()) );

				bDifferTempValue		 = TRUE;
				
				LONGLONG lnGap = pChar->GetStorageMoney() - pChar->GetTempStorageMoney();
				if( lnGap > 10000 || lnGap < -10000 )
				{
					if( pChar->GetStorageMoney() > pChar->GetTempStorageMoney() )
					{
						pChar->SetStorageMoney( pChar->GetTempStorageMoney() );
					}
					else
					{
						pChar->SetTempStorageMoney( pChar->GetStorageMoney() );
					}
				}
				else
				{
					pChar->SetStorageMoney( pChar->GetTempStorageMoney() );
				}	
			}
			
		}

		if( bDifferTempValue )
		{
			sc::writeLogInfo( sc::string::format(
				"[ Character ] [ Reserve Drop Out PC %1%, GLGaeaServer::CreateCharacter ]",
				dwChaNum ) );

			ReserveDropOutPC( pChar->GetGaeaID(), db::DbActionNull::Null );

			//	접속 시도자에게  메시지를 보냅니다.
			GLMSG::SNETLOBBY_CHARJOIN_FB NetMsgFB;
			NetMsgFB.dwChaNum		= dwChaNum;
			NetMsgFB.emCharJoinFB	= EMCJOIN_FB_ERROR;
			SENDTOAGENT(&NetMsgFB );
			return NULL;
		}
	}
	else
	{
		pChar->m_wTempLevel		  = pChar->m_wLevel;
		pChar->SetTempInvenMoney( pChar->GetInvenMoney() );
		pChar->SetTempStorageMoney( pChar->GetStorageMoney() );
	}


	/*if ( pChar->m_bTracingUser )
	{
		GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
		TracingMsg.nUserNum  = pChar->GetUserID();
		TracingMsg.SetAccount(pChar->m_szUID);

		CString strTemp;
		GLLandMan* const pLand(pChar->GetLandMan());
		if ( pLand )
		{
			int nPosX(0);
			int nPosY(0);
			pLand->GetMapAxisInfo().Convert2MapPos ( pChar->GetPosition().x, pChar->GetPosition().z, nPosX, nPosY );

			strTemp.Format(
				"FieldServer in!!, [%s][%s], MAP:mid[%d]sid[%d], StartPos:[%d][%d], Money:[%I64d]",
				pChar->m_szUID,
				pChar->m_szName,
				pChar->GetCurrentMap().getGaeaMapID().Mid(),
				pChar->GetCurrentMap().getGaeaMapID().Sid(),
				nPosX,
				nPosY,
				pChar->GetInvenMoney());
		}
		else
		{			
			strTemp.Format(
				"FieldServer in!!, [%s][%s], MAP:mid[%d]sid[%d], Money:[%I64d]",
				pChar->m_szUID,
				pChar->m_szName,
				pChar->GetCurrentMap().getGaeaMapID().Mid(),
				pChar->GetCurrentMap().getGaeaMapID().Sid(),
				pChar->GetInvenMoney());
		}

		TracingMsg.SetLog(strTemp.GetString());
		SENDTOAGENT(&TracingMsg);
	}*/

	return pChar;	//	케릭터 생성 성공.
}

//mjeon.instance.transfer
GLChar* GLGaeaServer::InitSerializedChar(GLChar *pChar, DWORD dwKey, SNATIVEID _StartMap, DWORD _dwStartGate, D3DXVECTOR3 _vPos)
{
	GASSERT(pChar);
	if ( pChar == NULL )
	{
		sc::writeLogError(std::string("GLGaeaServer::InitSerializedChar - pChar is NULL"));
		return NULL;
	}

	if (IsInstance())
	{
		GASSERT(dwKey != INVALID_INSTANCE_KEY);
		if (dwKey == INVALID_INSTANCE_KEY)
		{
			sc::writeLogError(sc::string::format("InitSerializedChar - InstanceKey(%1%) is invalid.", dwKey));
			
			RELEASE_CHAR(pChar);
			return NULL;
		}
	}
	else
	{
		GASSERT(dwKey == INVALID_INSTANCE_KEY);
		if (dwKey != INVALID_INSTANCE_KEY)
		{
			sc::writeLogError(sc::string::format("InitSerializedChar - InstanceKey(%1%) MUST be invalid in FieldServer.", dwKey));
			
			RELEASE_CHAR(pChar);
			return NULL;
		}
	}

	//
	//1.GaeaID를 세팅한 이후의 작업들 처리
	//

	SCHARDATA2 *pCharData	= static_cast<SCHARDATA2 *>(pChar);
	DWORD dwChaNum			= pChar->CharDbNum();
	DWORD GaeaId			= pChar->GetGaeaID();
	
    BOOL bNEW				= TRUE;
	EMGAME_JOINTYPE	emJOINTYPE = EMJOINTYPE_TRANSFER;

	//
	//mjeon.AF.이 시점에는 아직 Field <-> Client간 세션이 맺어지지 않았다
	//
	GASSERT(pChar);
	HRESULT hr = S_OK;
	
	if (pCharData == NULL)
	{
        sc::writeLogError(std::string("GLGaeaServer::InitSerializedChar - pCharData is NULL"));

		RELEASE_CHAR(pChar);
		return NULL;
	}

	// 가이아 ID의 할당을 Agent 서버에서 할당하게 됨에 따라 잘못되어 가이아 ID가 반환되지 않은 상태에서 다시 사용될 가능성이 있음
	//if (m_PCArray[GaeaId])
	if ( CheckCharacterExistance(GaeaId) )
	{
		sc::writeLogError(std::string("InitSerializedChar - Character with the GaeaID already exists."));
		
		RELEASE_CHAR(pChar);
		return NULL;
	}

	if (_StartMap == SNATIVEID(false))
	{
		sc::writeLogError(sc::string::format("InitSerializedChar - MapID for the InstanceKey(%1%) is invalid", dwKey));
		
		RELEASE_CHAR(pChar);
		return NULL;
	}

	GLLandMan* pLandMan(GetLand(_StartMap));
	if ( pLandMan == NULL )
    {
        sc::writeLogError(sc::string::format("InitSerializedChar - LandMan(%1%, %2%) does not exist.", _StartMap.Mid(), _StartMap.Sid()));

        RELEASE_CHAR(pChar);
        return NULL;
    }
	
	D3DXVECTOR3 vStartPos(0,0,0);
    DxLandGateMan *pGateMan = pLandMan->GetLandGateMan();
    if (!pGateMan)
    {
        sc::writeLogError(sc::string::format("InitSerializedChar - GateMan does not exist in the Instance(%1%, %2%).", _StartMap.Mid(), _StartMap.Sid()));

        RELEASE_CHAR(pChar);
        return NULL;
    }

    if (_dwStartGate != UINT_MAX)
    {
        DxLandGate *pGate = pGateMan->FindLandGate((DWORD)_dwStartGate);
        if (!pGate)
        {
            pGate = pGateMan->FindLandGate((DWORD)0);
            if (!pGate)
            {
                sc::writeLogError(sc::string::format("InitSerializedChar - Gate(%1% and 0) does not exist in the Instance(%3%,%4%).", _dwStartGate, _StartMap.Mid(), _StartMap.Sid()));

                RELEASE_CHAR(pChar);
                return NULL;
            }
            _dwStartGate = pGate->GetGateID();
        }
        vStartPos = pGate->GetGenPos(DxLandGate::GEN_RENDUM);
    }
    else
    {
        //Gate 대신 특정 좌표로 설정
        vStartPos = _vPos;
    }

	BOOL bRet = pChar->InitSerializedChar(pLandMan, bNEW);
	if (!bRet)
	{
        sc::writeLogError(
            sc::string::format(
				"GLGaeaServer::InitSerializedChar failure - ChaName %1% ChaDbNum %2% School %3% Map %4%/%5% Pos %6%/%7%/%8%",
                pCharData->m_szName,
                pCharData->CharDbNum(),
                pCharData->m_wSchool,
                pLandMan->GetBaseMapID().Mid(),
                pLandMan->GetBaseMapID().Sid(),
                vStartPos.x,
                vStartPos.y,
                vStartPos.z));
        
		RELEASE_CHAR(pChar);
		return NULL;
	}
		
	pChar->SetPosition(vStartPos);
		
	BOOL bOk = DropPC(pLandMan, vStartPos, pChar);
	if (!bOk)
	{
        sc::writeLogError(sc::string::format("GLGaeaServer::InitSerializedChar - Cannot DropOut. ChaName %1% ChaDbNum %2% School %3% Map %4%/%5% Pos %6%/%7%/%8%",
										pCharData->m_szName,
										pCharData->CharDbNum(),
										pCharData->m_wSchool,
										pLandMan->GetBaseMapID().Mid(),
										pLandMan->GetBaseMapID().Sid(),
										vStartPos.x,
										vStartPos.y,
										vStartPos.z));
		
		RELEASE_CHAR(pChar);
		return NULL;
	}

	//
	//mjeon.이 시점에는 아직 Field <-> Client간 세션이 맺어지지 않았다.
	//

	//직렬화된 캐릭터의 경우 DB 접근은 하지 않는다	
	//pChar->LoadExtraInfo();
	
	//
	//mjeon.attendance
	//
	pChar->AttendanceLoad(TRUE);

	//
	//mjeon.activity
	//
	pChar->ActivityLoad(TRUE);

	return pChar;
}


//mjeon.instance.transfer
int GLGaeaServer::SerializeFromChar(IN GLChar *pChar, OUT BYTE bufferCompressed[MAX_SERIALIZED_CHAR_SIZE])
{
	GASSERT(pChar && bufferCompressed);

	std::stringstream ss(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	boost::archive::binary_oarchive oa(ss);

	oa << (*pChar);

	ss.seekg(0, std::ios::end);
	
	int nOriginalLength = ss.tellg();
	
	ss.seekg(0, std::ios::beg);

	sc::writeLogDebug(sc::string::format("<----- Serialization Source: Stream size (%1% bytes)", nOriginalLength));

	std::string strTemp = ss.str();
	const char* pBytes = strTemp.c_str();

	int nCompressedLength = MAX_SERIALIZED_CHAR_SIZE;

	int nRet = sc::CMinLzo::GetInstance().lzoCompress((const BYTE *)pBytes, nOriginalLength, bufferCompressed, nCompressedLength);

	if (nRet == sc::CMinLzo::MINLZO_SUCCESS)
	{
		sc::writeLogDebug(sc::string::format("<----- Serialization Source: Compressed size (%1% bytes) -----", nCompressedLength));

		return nCompressedLength;
	}
	else
	{
		sc::writeLogError(sc::string::format("GLGaeaServer::SerializeFromChar - Compress failure (%1%)", nRet));

		return 0;
	}
}

int GLGaeaServer::SerializeToChar(IN BYTE bufferCompressed[MAX_SERIALIZED_CHAR_SIZE], int nCompressedLength, OUT GLChar *pChar)
{
	GASSERT(pChar && bufferCompressed);
	GASSERT(nCompressedLength > 0);

	int nDecompressedLength = MAX_SERIALIZED_CHAR_SIZE;

	BYTE bufferDecompressed[MAX_SERIALIZED_CHAR_SIZE];

	sc::writeLogDebug(sc::string::format("-----> Serialization Target: Compressed size (%1% bytes)", nCompressedLength));

	int nRet = sc::CMinLzo::GetInstance().lzoDeCompress((const BYTE *)bufferCompressed, nCompressedLength, bufferDecompressed, nDecompressedLength);

	if (nRet == sc::CMinLzo::MINLZO_SUCCESS)
	{
		try
		{
			std::stringstream ss(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
			
			ss.write((char *)bufferDecompressed, nDecompressedLength);

			boost::archive::binary_iarchive ia(ss);

			ia >> (*pChar);

			ss.seekg(0, std::ios::end);

			int size = ss.tellg();

			ss.seekg(0, std::ios::beg);

			GASSERT(nDecompressedLength == size);

			sc::writeLogDebug(sc::string::format("-----> Serialization Target: Decompressed size (%1% bytes)", nDecompressedLength));
			sc::writeLogDebug(sc::string::format("-----> Serialization Target: Stream size (%1% bytes)", size));
		}
		catch (boost::exception &be)
		{
			std::string strWhat = boost::diagnostic_information(be);
			sc::writeLogError(std::string("An boost::exception has occurred during GLChar DeSerialization - ") + strWhat);
		}
		catch (std::exception &se)
		{
			sc::writeLogError(std::string("An std::exception has occurred during GLChar DeSerialization - ") + std::string(se.what()));
		}

		return nDecompressedLength;
	}
	else
	{
		sc::writeLogError(sc::string::format("GLGaeaServer::SerializeToChar - Decompress failure (%1%)", nRet));

		return 0;
	}
}

//
//mjeon.instance.transfer
//
BOOL GLGaeaServer::TransferTo(GLChar *pChar, DWORD dwInstanceServerNum, SNATIVEID idMap, DWORD dwGateID, BOOL bOnly4DBSave)
{
	/// Instance Server 를 사용하지 않기 때문에 현재는 사용하지 않는다;
	/// 이곳에 들어오면 안된다;
	sc::writeLogError ( "[ Move Map ] [ GLGaeaServer::TransferTo ]" );

	/*//
	//1.Direction
	//
	BOOL bToInstance = IsInstance() ? FALSE : TRUE;

	if (bOnly4DBSave)
	{
		//DB저장 용도는 InstanceServer -> FieldServer 방향으로만 가능하다.

		GASSERT(bToInstance == FALSE);
		if (bToInstance == TRUE)
		{
			sc::writeLogError(std::string("GLGaeaServer::TransferTo - Wrong direction of Transfer for DB save!!!"));

			return FALSE;
		}
	}

	//
	//2.Does the map exist?
	//
	if (idMap == SNATIVEID(false))
	{
		sc::writeLogError(std::string("GLGaeaServer::TransferTo - Map does not exist"));
		return FALSE;
	}

	//
	//3.Does MapNode exist?
	//
	const SMAPNODE *pMapNode = FindMapNode(idMap.dwID);

	if( pMapNode == NULL )
	{
		sc::writeLogError(std::string("GLGaeaServer::TransferTo - MapNode does not exist"));
		return FALSE;
	}

	//
	//4.Handle pet, vehicle and summons.
	//
	DropOutPET(pChar->m_dwPetGUID, true, true);

	if (bToInstance)
	{
		SaveVehicle(pChar->ClientSlot(), pChar->GetGaeaID(), true);
	}
	
	DropOutAllSummon(pChar);

	//
	//5.Transfer character to the target FieldServer or InstanceServer
	//
	DWORD dwNextFieldSvr = pMapNode->GetFieldServerNum();

	BYTE buffer[MAX_SERIALIZED_CHAR_SIZE];
	ZeroMemory(buffer, sizeof(buffer));

	int nLen = SerializeFromChar(pChar, buffer);

	if (nLen <= 0)
	{
		sc::writeLogError(std::string("GLGaeaServer::TransferTo() - Failed to serialize the char."));
		return FALSE;
	}

	WORD nLastSeq = nLen / GLMSG::SNET_TRANSFER_CHAR::BUF_SIZE + ((nLen % GLMSG::SNET_TRANSFER_CHAR::BUF_SIZE) > 0 ? 1:0);

	GLMSG::SNET_TRANSFER_CHAR msg;
	msg.dwSvrGroupNum	= pChar->SvrGroupNum();
	msg.dwChaNum		= pChar->CharDbNum();
	msg.dwInstanceServerNum	= dwInstanceServerNum;
	msg.dwMapID			= idMap.dwID;
	msg.dwGateID		= dwGateID;
	msg.wOnlye4DBSave	= (WORD)bOnly4DBSave;	//캐릭터의 DB저장을 위한 용도인가?

	for (WORD i=1; i<=nLastSeq; i++)	//Seq is 1-base
	{
		msg.SetData(buffer, nLen, i, nLastSeq);

		if (bToInstance)
			SENDTOAGENT(&msg);				//FieldServer	-> InstanceServer
		else
			m_pMsgServer->SendMatch(&msg);	//InstanceServer-> FieldServer
	}*/

	return TRUE;
}

GLChar* GLGaeaServer::CreateCharacterFail(GLChar* pChar, DWORD dwChaNum)
{
    if (pChar)
	{
        RELEASE_CHAR(pChar);
	}

	GLMSG::SNETLOBBY_CHARJOIN_FB NetMsgFB(EMCJOIN_FB_ERROR);
	NetMsgFB.dwChaNum		= dwChaNum;
	SENDTOAGENT(&NetMsgFB);
    
    return NULL;	//	케릭터 생성 실패.
}

void GLGaeaServer::UpdateUserLockerOption( DWORD UserNum, int Option )
{
	// Option 값을 1로 주면 잠그기, 0으로 주면 풀기
	AddGameAdoJob( db::DbActionPtr( new db::UpdateUserLockerOption( UserNum, Option ) ) );
}

GLPetField* GLGaeaServer::CreatePET( GLPET* pPetData, DWORD OwnerDbNum, DWORD PetDbNum, bool bValid /*= true*/ )
{
	// DB에서 팻정보를 가져와야 한다.

	GLMSG::SNETPET_REQ_USEPETCARD_FB NetMsg;

	if ( !pPetData )
        return NULL;

	// 요청 케릭터가 유효성 체크
	GLChar* pOwner = GetCharByDbNum( OwnerDbNum );
	if ( !pOwner )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on CreatePET ", OwnerDbNum ) );
        return NULL;
	}

	// 팻Full Check
	if ( pPetData->IsNotEnoughFull() )
	{
		NetMsg.emFB = EMPET_USECARD_FB_NOTENOUGHFULL;
		SENDTOCLIENT( pOwner->ClientSlot(), &NetMsg );
		return NULL;
	}
		
	GLLandMan* pLandMan( pOwner->GetLandMan() );
	if ( !pLandMan ) 
	{
		SENDTOCLIENT( pOwner->ClientSlot(), &NetMsg );
		return NULL;
	}

	GLPetField* pPet( NULL );

	// 다른 필드서버로 넘어가지 않았다면 GLPetField 가 존재한다.
	// 맵정보와 네비게이션만 변경해준다.
	pPet = GetPET ( pOwner->m_dwPetGUID );
	if ( pPet && m_bEmulator )
	{
		// 메모리에 존재하는 팻 제거
		DropOutPET( pPet->m_dwGUID, true, false );
		pPet = NULL;
	}

	if ( pPet )
	{
		HRESULT hr = pPet->SetPosition( pLandMan );
		if ( FAILED( hr ) )
		{
			// 메모리에 존재하는 팻 제거
			DropOutPET( pPet->m_dwGUID, true, false );
			SENDTOCLIENT( pOwner->ClientSlot(), &NetMsg );
			return NULL;
		}
	}
	else
	{
        // 신규로 생성해준다.
		pPet = NEW_PET();
		if ( NULL == pPet )
		{
			return NULL;
		}

		HRESULT hr = pPet->Create( pLandMan, pOwner, pPetData );
		if ( FAILED( hr ) )
		{
			// 생성실패 처리
			RELEASE_PET( pPet );
			SENDTOCLIENT( pOwner->ClientSlot(), &NetMsg );
			return NULL;
		}

		DWORD dwGUID = -1;
		if ( !m_FreePETGIDs.GetHead( dwGUID ) )
            return NULL;
		m_FreePETGIDs.DelHead();
		pPet->m_dwGUID = dwGUID;

		// 주인ID 할당
		pOwner->m_dwPetGUID = pPet->m_dwGUID;
		pOwner->SetPetDbNum( PetDbNum );

		// 팻의 고유ID(DB저장용) 설정
		pPet->SetDbNum( PetDbNum );
	}
	
	if ( !DropPET( pPet, pLandMan->GetGaeaMapID() ) )
	{
		// 생성실패 처리
		m_FreePETGIDs.AddTail( pPet->m_dwGUID );
		RELEASE_PET( pPet );
		SENDTOCLIENT( pOwner->ClientSlot(), &NetMsg );
		return NULL;
	}

	// 주인 포인터 할당
	pPet->m_OwnerDbNum = pOwner->CharDbNum();
	pPet->m_OwnerSvrGroupNum = pOwner->SvrGroupNum();

	// 활성화 유무 설정
	if ( bValid )
        pPet->SetValid();
	else
        pPet->ReSetValid();

	// 활성상태일때만
	if ( pPet->IsValid() )
	{
		WORD i = 0;

		NetMsg.emFB					= EMPET_USECARD_FB_OK;
		NetMsg.m_emTYPE				= pPet->m_emTYPE;
		NetMsg.m_dwGUID				= pPet->m_dwGUID;
		NetMsg.m_sPetID				= pPet->m_sPetID;
		NetMsg.m_dwOwner			= pPet->m_dwOwner;
		NetMsg.m_wStyle				= pPet->m_wStyle;
		NetMsg.m_wColor				= pPet->m_wColor;
		NetMsg.m_fWalkSpeed			= pPet->m_fWalkSpeed;
		NetMsg.m_fRunSpeed			= pPet->m_fRunSpeed;
		NetMsg.m_nFull				= pPet->Satiation();
		NetMsg.m_sMapID				= pPet->m_sMapID;
		NetMsg.m_dwCellID			= pPet->m_dwCellID;
		NetMsg.m_wSkillNum			= static_cast< WORD >( pPet->m_ExpSkills.size() );
		NetMsg.m_vPos				= pPet->m_vPos;
		NetMsg.m_vDir				= pPet->m_vDir;
		NetMsg.m_sPetSkinPackData	= pPet->m_sPetSkinPackData;
		NetMsg.m_wMaxSkillSlot		= pPet->m_wMaxSkillSlot;

		for ( i=0; i<MAX_PETSKILLSLOT; ++i )
		{
			NetMsg.m_sActiveSkillID[i] = pPet->m_sActiveSkillID[i];
		}

		StringCchCopy( NetMsg.m_szName, PETNAMESIZE+1, pPetData->GetName() );

		NetMsg.m_dwPetID = PetDbNum;

		PETSKILL_MAP_ITER iter = pPet->m_ExpSkills.begin();
		PETSKILL_MAP_ITER iter_end = pPet->m_ExpSkills.end();

		for ( i=0; iter != iter_end; ++iter )
		{
			NetMsg.m_Skills[i++] = iter->second;
		}

		const CTime cTIME_CUR = CTime::GetCurrentTime();

		for ( i=0; i < ACCETYPESIZE; ++i )
		{
			SITEMCUSTOM sPetItem = pPet->m_PutOnItems[i];
			if ( sPetItem.GetNativeID() == NATIVEID_NULL () ) continue;

			const SITEM* pITEM = GLogicData::GetInstance().GetItem( sPetItem.GetNativeID() );
			if ( !pITEM )	continue;

			// 시한부 아이템
			if ( sPetItem.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
			{
				CTimeSpan cSPAN( pITEM->sDrugOp.tTIME_LMT );
				CTime cTIME_LMT( sPetItem.tBORNTIME );
				cTIME_LMT += cSPAN + sPetItem.GetPeriodExBasic();

				if ( cTIME_CUR > cTIME_LMT )
				{
					//	시간 제한으로 아이템 삭제 로그 남김.
					// 통계 및 로그 수정
					m_pItemLimit->LogItemExchange( sPetItem, gamelog::ID_CHAR, pOwner->m_CharDbNum, gamelog::ID_CHAR, 0, ITEM_ROUTE_DELETE, 0, ( pOwner == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pOwner->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_TIMEOUT, false );

					// #item
					//	아이템 삭제.
					pPet->PetPutOnItemReset( static_cast< ACCESSORYTYPE >( i ), true );

					//	시간 제한으로 아이템 삭제 알림.
					GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
					NetMsgInvenDelTimeLmt.nidITEM = sPetItem.GetNativeID();
					SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgInvenDelTimeLmt );
				}
			}
			NetMsg.m_PutOnItems[i] = pPet->m_PutOnItems[i];
		}

		SENDTOCLIENT( pOwner->ClientSlot(), &NetMsg );

		// 주변에 알림
        GLMSG::SNETPET_DROP_PET MsgFC;
        pPet->ReqNetMsg_Drop( MsgFC );
		GLMSG::SNETPET_CREATE_ANYPET NetMsgBrd( MsgFC.Data );
		pOwner->SendMsgViewAround( &NetMsgBrd );

		// 스킬 자동 구동
		pPet->ActiveSkill();

        // 펫 소환
        m_pItemLimit->LogPetAction( 
            pPet->DbNum(), 
            SNATIVEID( 0, 0 ), 
            pet::EM_ACTION_SUMMONS, 
            pPet->Satiation() );
	}
	
	return pPet;	
}

BOOL GLGaeaServer::DropPET(GLPetField* pPet, SNATIVEID sMapID)
{
	if (!pPet)
        return FALSE;

	GLLandMan* pLandMan = GetLand ( sMapID );

	if (!pLandMan)
        return FALSE;

	// 글로벌에 등록
	//m_PETArray[pPet->m_dwGUID] = pPet;
    RegistPet(pPet->m_dwGUID, pPet);

	// 랜드에 등록
	pPet->m_pLandMan = pLandMan;
	pPet->m_pLandNode = pLandMan->m_GlobPETList.ADDHEAD(pPet);
	pLandMan->RegistPet(pPet);
	
	return TRUE;
}

// 이 함수는 주인이 맵을 이동하는 모든 행위에 대해서
// 호출된다. (주인이 게임을 종료하는 경우에도 호출됨)
// 따라서 소환여부를 고려해야 한다.
BOOL GLGaeaServer::DropOutPET(DWORD dwGUID, bool bLeaveFieldServer, bool bMoveMap)
{
	if (dwGUID >= m_dwMaxClient)
	{	
		if (dwGUID != UINT_MAX)
        {
            sc::writeLogError(
                sc::string::format(
                    "dwGUID>=m_dwMaxClient PetGUID : %1% dwMaxClient : %2%", dwGUID, m_dwMaxClient));
        }
		return FALSE;
	}

    //GLPetField* pPet = m_PETArray[dwGUID];
    GLPetField* pPet = GetPET(dwGUID);
	if (!pPet) 
	{
        /*
        sc::writeLogError(
            sc::string::format(
                "m_PETArray[%1%] == NULL",
                dwGUID));
        */
		return FALSE;
	}

    DWORD PetDbNum = pPet->DbNum();
    DWORD PetSatiation = pPet->Satiation();

	// 활동 여부
	bool bValid = pPet->IsValid();

	// 소환이 안된상태로 서버를 떠나지 않는다면
	if (!bValid && !bLeaveFieldServer)
	{
        sc::writeLogDebug(
            sc::string::format(
                "!bValid && !bLeaveFieldServer bValid %d bLeaveFieldServer %d", bValid, bLeaveFieldServer));
		return FALSE;
	}

	DWORD dwPetGUID = pPet->m_dwGUID;
	DWORD dwOwnerID = pPet->m_dwOwner;

	// 활동중이면
	if (bValid)
	{
		// Land 리스트에서 제거.
		GLLandMan* pLandMan = pPet->m_pLandMan;
		if (pLandMan)
		{
			pLandMan->RemovePet(pPet);
			pPet->m_pLandMan = NULL;

			// [shhan][2015.09.30] 지형이 지워지므로 m_actorMove 도 당연히 Reset 되야한다.
			pPet->ResetActorMove();
		}

		pPet->ReSetValid ();
		pPet->ReSetAllSTATE ();
		pPet->ReSetSkillDelay ();
	}

	// 클라이언트 팻 사라지게 메시지 발송 (PC가 게임을 종료하면 pOwner 없을 수 있다)
	GLChar* pOwner = GetChar ( dwOwnerID );
	if ( pOwner && bValid )
	{
		// 스킬의 보조능력치 제거

		for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
		{
			pOwner->m_sPETSKILLFACT[i].RESET ();
		}

		GLMSG::SNETPET_REQ_UNUSEPETCARD_FB NetMsgFB( dwPetGUID, bMoveMap );
		SENDTOCLIENT ( pOwner->ClientSlot(), &NetMsgFB );

		// 주변에 알림
		GLMSG::NET_PET_REQ_SKILL_CHANGE_FC NetMsgBRD;

		NetMsgBRD.dwGUID = pPet->m_dwGUID;

		for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
		{
			NetMsgBRD.sSKILL[i].sSkillID = NATIVEID_NULL ();
			NetMsgBRD.sSKILL[i].dwTarID  = pOwner->GetGaeaID();
		}

		pOwner->SendMsgViewAround(&NetMsgBRD);
	}

	// 소환해제시 펫데이터 저장 // 인벤토리에서 펫카드 이동시 팻 DB데이터를 받아옴. 
	if( pPet )
	{
		// 팻의 데이터 DB저장
		AddGameAdoJob(
			db::DbActionPtr(
			new db::PetFullSet(pPet->m_OwnerDbNum, pPet->DbNum (), pPet->Satiation())));		
	}

	// 현재 필드서버를 떠나거나 게임을 완전 종료하면
	if ( bLeaveFieldServer && pPet )
	{
		AddGameAdoJob(
            db::DbActionPtr(
                new db::PetInvenSet(pPet->m_OwnerDbNum, pPet->DbNum(), pPet)));		

		// Note : 스킬 슬롯 저장
		DWORD     dwActiveSkillSlotFlag = 0;
		SNATIVEID sPreSkillID = NATIVEID_NULL();

		for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
		{
			SNATIVEID& sSkillID = pPet->m_sActiveSkillID[i];

			if (sSkillID != NATIVEID_NULL())
			{		
                if (sSkillID.Mid() != GLPET::PET_SKILL_MAIN)
				{
                    sc::writeLogError(
                        sc::string::format(
                            "GLGaeaServer::DropOutPET, MID = %d, SID = %d",
                            pPet->m_sActiveSkillID[i].wMainID,
                            pPet->m_sActiveSkillID[i].wSubID));
				}

				if (sPreSkillID != sSkillID)
				{
					sPreSkillID = sSkillID;
					dwActiveSkillSlotFlag = 0;
				}

				dwActiveSkillSlotFlag |= (1<<i);

				PETSKILL sPetSkill( sSkillID, 0 );
				AddGameAdoJob(
                    db::DbActionPtr(
                        new db::PetSkillSet(pPet->m_OwnerDbNum, pPet->DbNum (), sPetSkill, dwActiveSkillSlotFlag)));				
			}
		}

		// 글로벌 리스트에서 제거, GUID 반환
		RELEASE_PET(pPet);        
		//m_PETArray[dwGUID] = NULL;
        UnRegistPet(dwGUID);
		m_FreePETGIDs.AddTail(dwGUID);
	}

    m_pItemLimit->LogPetAction(PetDbNum, SNATIVEID(0,0), pet::EM_ACTION_UNSUMMONS, PetSatiation);

	return TRUE;
}

HRESULT GLGaeaServer::ClearDropObj()
{
	/*
	if (m_PCArray)
	{
		// 플래이어 드롭 아웃 처리.
		for (DWORD i=0; i<m_dwMaxClient; i++)
		{
			if (m_PCArray[i])
                ReserveDropOutPC(i, db::DbActionNull::Null);
		}

		ClearReservedDropOutPC ();
	}
	*/

	UMAP_GLCHAR_ITER iter = m_mapCharacters.begin();
	for (; iter != m_mapCharacters.end(); ++iter)
	{
		sc::writeLogInfo( "[ Character ] [ Reserve Drop Out PC ( all ), GLGaeaServer::ClearDropObj ]" );

		ReserveDropOutPC(iter->first, db::DbActionNull::Null);
	}

	ClearReservedDropOutPC();

	// 지형 정보 제거.
	//
	/*GLLANDMANNODE* pLandManNode = m_LandManList.m_pHead;
	for ( ; pLandManNode; pLandManNode = pLandManNode->pNext )
	{
	pLandManNode->Data->ClearDropObj ();
	}*/

    size_t tSize = m_landManagerMap.size();
    for( size_t i=0; i<tSize; ++i )
    {
        m_landManagerMap[i].pLandManager->clearDropObject();
    }

	return S_OK;
}

BOOL GLGaeaServer::ValidCheckTarget ( GLLandMan* pLandMan, STARGETID &sTargetID )
{
	GASSERT(pLandMan);

	if ( sTargetID.GaeaId == EMTARGET_NULL )
		return FALSE;

	if ( sTargetID.emCrow == CROW_PC )
	{
		GLChar* pChar = GetChar ( sTargetID.GaeaId );
		if ( pChar && pChar->IsValidBody() )
		{
			if ( pChar->GetLandMan() != pLandMan )
				return FALSE;
			
			return TRUE;
		}
	}
	else if ( sTargetID.emCrow == CROW_NPC || sTargetID.emCrow == CROW_MOB )
	{
		PGLCROW pCrow = pLandMan->GetCrow ( sTargetID.GaeaId );
		if ( pCrow && pCrow->IsValidBody() )
		{
			if ( pCrow->GetLandMan() != pLandMan )
				return FALSE;
			return TRUE;
		}
	}
	else if ( sTargetID.emCrow == CROW_MATERIAL )
	{
		PGLMATERIAL pMaterial = pLandMan->GetMaterial ( sTargetID.GaeaId );
		if ( pMaterial && pMaterial->IsValidBody() )
		{
			if ( pMaterial->GetLandMan() != pLandMan )
				return FALSE;
			return TRUE;
		}
	}
	else if( sTargetID.emCrow == CROW_PET )	// PetData
	{
	}else if ( sTargetID.emCrow == CROW_SUMMON )
	{
		PGLSUMMONFIELD pSummon = pLandMan->GetSummon( sTargetID.GaeaId );		
		if ( pSummon && pSummon->IsValidBody() )
		{
			if ( pSummon->GetLandMan() != pLandMan )
				return FALSE;
			return TRUE;
		}
	}
	/*else
	{
		GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
	}
	*/

	sTargetID.GaeaId = EMTARGET_NULL;
	return FALSE;
}

ServerActor* GLGaeaServer::GetTarget ( const GLLandMan* pLandMan, const STARGETID &sTargetID )
{
	GASSERT(pLandMan);
	if (!pLandMan)
        return NULL;
	if (sTargetID.GaeaId == EMTARGET_NULL)
        return NULL;

	switch ( sTargetID.emCrow )
	{
	case CROW_PC:
		{
			// 같은 맵에 있는 PC만 검색
			GLChar* pChar(GetChar(sTargetID.GaeaId));
			if ( pChar == NULL )
				return NULL;

			GLLandMan* pLand(pChar->GetLandMan());
			if ( pLand == NULL )
				return NULL;

			if ( pLand->GetGaeaMapID() == pLandMan->GetGaeaMapID())
                return pChar;
		}
		break;

	case CROW_MOB:
	case CROW_NPC:
		{
			PGLCROW pCrow = pLandMan->GetCrow(sTargetID.GaeaId);
			if (pCrow)
                return pCrow;
		}
		break;
	case CROW_MATERIAL:
		{
			PGLMATERIAL pMaterial = pLandMan->GetMaterial(sTargetID.GaeaId);
			if (pMaterial)
                return pMaterial;
		}
		break;
	case CROW_SUMMON:
		{
			PGLSUMMONFIELD pSummon = pLandMan->GetSummon(sTargetID.GaeaId);
			if (pSummon)
                return pSummon;
		}
		break;

	//case CROW_PET:	break;	// PetData

	//default:	GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
	};

	return NULL;
}

ServerActor* GLGaeaServer::GetActor(const GLLandMan* pLandMan, const Faction::GLActorID& actorID)
{
    GASSERT(pLandMan);
    if (!pLandMan)
        return NULL;

    switch ( actorID.actorType )
    {
    case CROW_PC:
        {
            // 같은 맵에 있는 PC만 검색
			GLChar* pChar = NULL;
            pChar = GetCharByDbNum(actorID.actorID_Num);
            if ( pChar == NULL )
			{
				pChar = GetChar( actorID.actorID_Num );
				if ( NULL == pChar )
					return NULL;
			}

            GLLandMan* pLand(pChar->GetLandMan());
            if ( pLand == NULL )
                return NULL;

            if ( pLand->GetGaeaMapID() == pLandMan->GetGaeaMapID())
                return pChar;
        }
        break;

    case CROW_MOB:
    case CROW_NPC:
        {
            PGLCROW pCrow = pLandMan->GetCrow(actorID.actorID_Num);
            if (pCrow)
                return pCrow;
        }
        break;
    case CROW_MATERIAL:
        {
            PGLMATERIAL pMaterial = pLandMan->GetMaterial(actorID.actorID_Num);
            if (pMaterial)
                return pMaterial;
        }
        break;
    case CROW_SUMMON:
        {
            PGLSUMMONFIELD pSummon = pLandMan->GetSummon(actorID.actorID_Num);
            if (pSummon)
                return pSummon;
        }
        break;

        //case CROW_PET:	break;	// PetData

        //default:	GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
    };

    return NULL;
}

ServerActor* GLGaeaServer::GetActor(const GLLandManager* pLandMan, const Faction::GLActorID& actorID)
{
    GASSERT(pLandMan);
    if (!pLandMan)
        return NULL;

    switch ( actorID.actorType )
    {
    case CROW_PC:
        {
            // 같은 맵에 있는 PC만 검색
            GLChar* pChar = NULL;
            pChar = GetCharByDbNum(actorID.actorID_Num);
            if ( pChar == NULL )
			{
				pChar = GetChar( actorID.actorID_Num );
				if ( NULL == pChar )
					return NULL;
			}

            GLLandMan* pLand(pChar->GetLandMan());
            if ( pLand == NULL )
                return NULL;

            if ( pLand->GetGaeaMapID().Mid() == pLandMan->getInstanceMapMainID())
                return pChar;
        }
        break;

    case CROW_MOB:
    case CROW_NPC:
        {
            PGLCROW pCrow = pLandMan->getCrow(actorID.actorID_Num);
            if (pCrow)
                return pCrow;
        }
        break;
    case CROW_MATERIAL:
        {
            PGLMATERIAL pMaterial = pLandMan->getMaterial(actorID.actorID_Num);
            if (pMaterial)
                return pMaterial;
        }
        break;
    case CROW_SUMMON:
        {
            PGLSUMMONFIELD pSummon = pLandMan->getSummon(actorID.actorID_Num);
            if (pSummon)
                return pSummon;
        }
        break;

        //case CROW_PET:	break;	// PetData

        //default:	GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
    };

    return NULL;
}

const D3DXVECTOR3& GLGaeaServer::GetTargetPos ( const GLLandMan* pLandMan, const STARGETID &sTargetID )
{
	GASSERT(pLandMan);

	// 타겟의 위치 정보를 가져옴.
	if ( sTargetID.emCrow == CROW_PC )
	{
		GLChar* pChar = GetChar(sTargetID.GaeaId);
		if (pChar)
            return pChar->GetPosition();
	}
	else if ( sTargetID.emCrow == CROW_NPC || sTargetID.emCrow == CROW_MOB ) 
	{
		PGLCROW pCrow = pLandMan->GetCrow(sTargetID.GaeaId);
		if (pCrow)
            return pCrow->GetPosition();
	}
	else if ( sTargetID.emCrow == CROW_MATERIAL )
	{
		PGLMATERIAL pMaterial = pLandMan->GetMaterial ( sTargetID.GaeaId );
		if (pMaterial)
            return pMaterial->GetPosition();
	}
	else if (sTargetID.emCrow == CROW_PET)	// PetData
	{
	}
	else if (sTargetID.emCrow == CROW_SUMMON)
	{
		PGLSUMMONFIELD pSummon =  pLandMan->GetSummon(sTargetID.GaeaId);
		if (pSummon)
            return pSummon->GetPosition();
	}
	/*else
	{
		GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
	}
	*/

	static D3DXVECTOR3 vERROR(FLT_MAX,FLT_MAX,FLT_MAX);
	return vERROR;
}

WORD GLGaeaServer::GetTargetBodyRadius ( GLLandMan* pLandMan, STARGETID &sTargetID )
{
	GASSERT(pLandMan);

	// 타겟의 위치 정보를 가져옴.
	if (sTargetID.emCrow == CROW_PC)
	{
		GLChar* pChar = GetChar(sTargetID.GaeaId);
		if (pChar)
            return pChar->GETBODYRADIUS();
	}
	else if (sTargetID.emCrow == CROW_NPC || sTargetID.emCrow == CROW_MOB)
	{
		PGLCROW pCrow = pLandMan->GetCrow(sTargetID.GaeaId);
		if (pCrow)
            return pCrow->GETBODYRADIUS();
	}
	else if (sTargetID.emCrow == CROW_MATERIAL)
	{
		PGLMATERIAL pMaterial = pLandMan->GetMaterial(sTargetID.GaeaId);
		if (pMaterial)
            return pMaterial->GetBodyRadius();
	}
	else if( sTargetID.emCrow == CROW_PET )	// PetData
	{
	}
	else if ( sTargetID.emCrow == CROW_SUMMON )
	{
		PGLSUMMONFIELD pSummon =  pLandMan->GetSummon(sTargetID.GaeaId);
		if (pSummon)
            return pSummon->GETBODYRADIUS();
	}
	/*else
	{
		GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
	}
	*/

	return WORD(0xFFFF);
}

GLARoundSlot* GLGaeaServer::GetARoundSlot ( const STARGETID &sTargetID )
{
	if ( sTargetID.emCrow==CROW_PC )
	{
		GLChar* pChar = GetChar(sTargetID.GaeaId);
		if (!pChar)
            return NULL;
		return &pChar->GetARoundSlot();
	}
	return NULL;
}

BOOL GLGaeaServer::DropPC(GLLandMan *pLandMan, const D3DXVECTOR3& vPos, GLChar *pPC)
{
	if ( !pLandMan )
		return FALSE;

	if ( pLandMan->GetGaeaMapID() == SNATIVEID(false) )
		return FALSE;

	pPC->SetNavi(pLandMan->GetNavi(), vPos);

	RegisterCharacter(pPC->GetGaeaID(), pPC);

	pPC->m_pCharNodeInGaea = m_GaeaPCList.ADDHEAD(pPC);

	MoveSystem::CMoveManagerField::Instance()->OnJoinServer(
		pPC, pLandMan );

	/*//위치로 내비게이션 초기화
	pPC->SetNavi(pLandMan->GetNavi(), vPos);

	RegisterCharacter(pPC->GetGaeaID(), pPC);

	pPC->m_pCharNodeInGaea = m_GaeaPCList.ADDHEAD(pPC);


	
	//
	//mjeon.tbb
	//
	pLandMan->EnterLand(pPC);	
	
	// GLLandMan의 셀에 등록하는 작업.  
	//		GLLandMan::RegistChar(pPC) 작업이 GLGaeaServer::RequestLandIn()이 호출될 때까지 유보
	pPC->m_dwCeID = 0;
	pPC->m_pQuadNode = NULL;
	pPC->m_pCellNode = NULL;*/
	

	//PC NAME map 에 등록
	GASSERT(m_pCharMan->ChaNameExist(std::string(pPC->GetCharData2().m_szName)) == false);
	m_pCharMan->ChaNameAdd(std::string(pPC->GetCharData2().m_szName), pPC);

	
	//
	// 이 시점에는 아직 FieldServer <-> Client 간의 세션이 맺어지지 않았다 !!
	//

	// CID map 에 등록
	if (GetServerType() == net::SERVER_INSTANCE)	//InstanceServer에서는 ChaDbNum을 사용해서는 안된다!
	{
		m_pCharMan->ChaUniqueIDAdd(UNIQUEID(pPC->SvrGroupNum(), pPC->CharDbNum()).Id(), pPC->GetGaeaID());
	}
	else
	{
		m_pCharMan->ChaDbNumAdd(pPC->m_CharDbNum, pPC->GetGaeaID());
	}

	m_TransferMan.Clear(UNIQUEID(pPC->SvrGroupNum(), pPC->CharDbNum()).Id());

	return TRUE;
}

BOOL GLGaeaServer::DropOutPC(GLChar *pChar)
{
	if ( pChar && m_pNetMsgMgr )
	{
		m_pNetMsgMgr->DeleteCharacter( pChar->CharDbNum() );
	}

	if ( m_pCTFMan )
		m_pCTFMan->AddRanker( pChar->CharDbNum() );

	//mjeon.instance
	m_TransferMan.Clear(UNIQUEID(pChar->SvrGroupNum(), pChar->CharDbNum()).Id());

	// 만약 상점이 열려 있는 상태이면 상점을 닫아준다
	if (pChar->IsPrivateMarketOpen())
	{
		sc::writeLogInfo( sc::string::format("DropOutPC (%1%) IsPrivateMarketOpen",pChar->GetName()) );
		if ( pChar->GetPrivateMarket()->IsPremiumMarket() )
		{
			sc::writeLogInfo( sc::string::format("DropOutPC (%1%) IsPrivateMarketOpen IsPremiumMarket",pChar->GetName()) );
			if ( !pChar->m_bOffVend )
			{
				pChar->m_bOffVend = true;
				sc::writeLogInfo( sc::string::format("DropOutPC (%1%) Offline Vend",pChar->GetName()) );
				return TRUE;
			}
		}
		pChar->PrivateMarketClose();

		GLMSG::SNETPC_PMARKET_CLOSE_BRD	NetMsgBRD;
		NetMsgBRD.dwGaeaID = pChar->GetGaeaID();
		pChar->SendMsgViewAround(&NetMsgBRD);

		// cache 에 개인상점 닫는다는 메시지 전송
		GLMSG::SNETPC_PMARKET_CLOSE_FH NetMsgFH;
		NetMsgFH.dwChaDbNum = pChar->CharDbNum();
		NetMsgFH.nServerGroup = GetServerGroup();
		SENDTOINTEGRATIONCACHE( &NetMsgFH );
	}

	if ( pChar->m_sCONFTING.IsCONFRONTING() )
	{
		pChar->ReceiveLivingPoint ( GLCONST_CHAR::nCONFRONT_LOSS_LP );
	}

	pChar->ResetViewAround();

	UnregisterCharacter(pChar->GetGaeaID());
	
	if ( pChar->m_pCharNodeInGaea )
	{
		if ( m_GaeaPCList.DELNODE(pChar->m_pCharNodeInGaea) == false )
		{
			const MapID& _charMapID(pChar->GetCurrentMap());				
			sc::writeLogError(sc::string::format("GLGaeaServer::DropOutPC() : m_GaeaPCList.DELNODE() is FAIL, CharMapID(%1%/%2%, %3%/%4%)", 
				_charMapID.getBaseMapID().Mid(), _charMapID.getBaseMapID().Sid(), _charMapID.getGaeaMapID().Mid(), _charMapID.getGaeaMapID().Sid()));
		}
	}
	
	// Land 리스트에서 제거	
	GLLandMan* pLandMan(pChar->GetLandMan());
	if (pLandMan)
		pLandMan->RemoveChar(pChar);
	
	// PC NAME map 에서 제거
	GASSERT(m_pCharMan->ChaNameExist(std::string(pChar->GetCharData2().m_szName)) == true);
	if (m_pCharMan->ChaNameExist(std::string(pChar->GetCharData2().m_szName)) == true)
	{
		m_pCharMan->ChaNameDel(std::string(pChar->GetCharData2().m_szName));
	}

    m_pCharMan->ChaDbNumDel(pChar->m_CharDbNum);		

	//메모리 반환
	RELEASE_CHAR(pChar);

	return TRUE;
}

const bool GLGaeaServer::PreSaveCharDB ( DWORD _dwChaDBNum )
{
	GLChar* pChar = GetCharByDbNum( _dwChaDBNum );
	if ( NULL == pChar )
		return false;

	sc::writeLogInfo ( sc::string::format( "Start PreSaveCharDB %1%", _dwChaDBNum ) );

	pChar->SAVE_CURRENT_SLOT_INFO();

	sc::writeLogInfo ( sc::string::format( "End PreSaveCharDB %1%", _dwChaDBNum ) );

	return true;
}

BOOL GLGaeaServer::SaveCharDB( DWORD dwGaeaID, bool bSaveWorldBattle /*= false*/, bool bAutoSave /*= false*/ )
{
	if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

	GLChar* pChar = GetChar(dwGaeaID);
	if ( pChar == NULL )
		return FALSE;

	sc::writeLogInfo ( sc::string::format( "Start SaveCharDB %1%", pChar->CharDbNum() ) );
	
	GLLandMan* const pLand(pChar->GetLandMan());
	const MapID sBackMapID(pLand == 0 ? pChar->m_sStartMapID : pLand->GetMapID());
	const D3DXVECTOR3 vBackPos(pChar->GetPosition());

	// 구인던일 경우 인던 밖으로 이동한다.
	SMAPNODE* pMapNode(FindMapNode(sBackMapID.getBaseMapID()));
	const bool _bOldInstantMap((pMapNode != NULL) && (pMapNode->GetInstanceType() == SMAPNODE::EMINSTANCE_OLD_INSTANCE));
	if ( _bOldInstantMap == true )
		SaveCheckInstantMap(pChar);
	
	pChar->SavePosition ();

	if ( m_pDBMan )
	{
		// 케릭터 저장 요청.
		m_pCharMan->ReqSaveDbUserAdd( pChar->GetUserID() );

		GLMSG::SNETPC_MSG_CHA_SAVE_FA NetMsgFA( pChar->GetUserID() );
		SENDTOAGENT( &NetMsgFA );

		if( bSaveWorldBattle )
		{
			sc::writeLogInfo( sc::string::format(
				"[ Battle Server ] [ To start it that the database save a character for move to the battle server. < UserDBNum : %1%, ChaDBNum : %2% > ]",
				pChar->GetUserID(),
				pChar->CharDbNum() ) );

			AddGameAdoJob(db::DbActionPtr(new db::CharacterSave( pChar, dwGaeaID ) ) );	// W.B.
		}
		else
			AddGameAdoJob(db::DbActionPtr(new db::CharacterSave( pChar, bAutoSave ) ) );	//mjeon.ado
	}

	//	인던일 경우 현재좌표로 다시 돌려준다. 
	//	인던은 저장할때 인던 밖에 저장하기 때문에
	if ( _bOldInstantMap == true )
	{
		pChar->SetCurrentMap(sBackMapID);
		pChar->SetPosition(vBackPos);
	}

	sc::writeLogInfo ( sc::string::format( "End SaveCharDB %1%", pChar->CharDbNum() ) );

	return TRUE;
}

BOOL GLGaeaServer::SaveCharacterIntoDB( GLChar* pChar, db::DbActionPtr spAfterJob, bool bSaveToSqlite /*= false*/ )
{
	GASSERT(pChar);
	if (!pChar)
    {
        if (spAfterJob)
            AddGameAdoJob(spAfterJob);
		return FALSE;
    }

	sc::writeLogInfo ( sc::string::format( "Start SaveCharacterIntoDB %1%", pChar->CharDbNum() ) );

	// mjeon.CTF
	pChar->SaveExtraInfo();

	//
	//mjeon.activity
	//
	pChar->ActivitySave();
	
	//
	//mjeon.attendance
	//
	pChar->AttendanceSave();

	// PET
	// 맵이동시 Pet 삭제
	DropOutPET(pChar->m_dwPetGUID, true, true);
	
	// VEHICLE
	SaveVehicle(pChar->ClientSlot(), pChar, true);

	// SUMMON
	DropOutAllSummon(pChar);
	
	// 구인던일 경우 위치 백업 후 인던 밖으로 이동한다.
	const MapID _mapIDBackup(pChar->GetCurrentMap());
	const D3DXVECTOR3 _positionBackup(pChar->GetPosition());	
	const SMAPNODE* const pMapNode(FindMapNode(_mapIDBackup));
	const bool _bOldInstantMap((pMapNode != NULL) && (pMapNode->GetInstanceType() == SMAPNODE::EMINSTANCE_OLD_INSTANCE));
	if ( _bOldInstantMap == true )
		SaveCheckInstantMap( pChar );

	pChar->SavePosition();

	// 사망한 상태로 제거시
	if (pChar->IsActState(EM_ACT_DIE))
	{
		// 부활시 경험치 감소
		pChar->DoDeathPenaltyExp();

		//현재 위치를 마지막 귀환 위치로 저장
		pChar->SaveLastCall();
	}

	// 현재 음식버프 저장.
	pChar->SaveSkillFact();

	if ( bSaveToSqlite )
	{
		// 임시로 sqlite 에 저장할 경우
		SaveCharacterInSqlite( pChar, true );
	}
	else
	{
		// 정상적으로 db 에 저장하는 경우
		m_pCharMan->ReqSaveDbUserAdd( pChar->GetUserID() );

		GLMSG::SNETPC_MSG_CHA_SAVE_FA NetMsgFA( pChar->GetUserID() );
		SENDTOAGENT( &NetMsgFA );

		AddGameAdoJob(
			db::DbActionPtr(
			new db::CharacterSave( pChar ) ) );

		if (spAfterJob)
			AddGameAdoJob(spAfterJob);
	}

	if ( _bOldInstantMap == true )
	{
		pChar->SetCurrentMap(_mapIDBackup);
		pChar->SetPosition(_positionBackup);	
	}	

	sc::writeLogInfo ( sc::string::format( "End SaveCharacterIntoDB %1%", pChar->CharDbNum() ) );

	return TRUE;
}

BOOL GLGaeaServer::ClearReservedDropOutPC()
{
	sc::CriticalSectionOwnerLock OwnerLock(m_CSPCLock);
	{
		VPCID_ITER iter     = m_reqDropOutChar.begin();
		VPCID_ITER iter_end = m_reqDropOutChar.end();
		for (; iter != iter_end; ++iter)
		{
			DWORD dwGaeaID = iter->m_dwGaeaID;
			GLChar* pChar = GetChar(dwGaeaID);
			if (pChar)
			{
                pChar->Offline();

				switch ( m_dwError )
				{
				case SERVER_STOP_NOMAL:
				case SERVER_STOP_JUST_RESTART:
					PreSaveCharDB( pChar->CharDbNum() );
					SaveCharacterIntoDB( pChar, iter->m_spMsg );
					break;
				case SERVER_STOP_OUTOFMEMORY:
					PreSaveCharDB( pChar->CharDbNum() );
					SaveCharacterIntoDB( pChar, iter->m_spMsg, true );
					break;
				}

				DropOutPC(pChar);
			}
			else
			{
				m_pCharMan->ReqSaveDbUserDel(iter->m_dwUserID);
			}
		}
		
		m_reqDropOutChar.clear();
	}	

	return TRUE;
}

BOOL GLGaeaServer::ReserveDropOutPC(DWORD dwGaeaId, db::DbActionPtr spDbActToAgentMsg)
{
	// * 주의
	// pDbActToAgentMsg 의 Default 가 NULL 이다.
	// 여기서 NULL 체크를 해버리면 캐릭터가 정상적으로 DROP 되지 않아서
	// 서버안에 남아있게 된다.
	//GASSERT(dwGaeaId != GAEAID_NULL);
	if (dwGaeaId == GAEAID_NULL)
		return FALSE;

	sc::CriticalSectionOwnerLock OwnerLock(m_CSPCLock);
	GLChar* pChar = GetChar(dwGaeaId);
	if (pChar)
	{
		SDROPOUTINFO sDropOut( dwGaeaId, pChar->GetUserID(), spDbActToAgentMsg );
		VPCID_ITER it = std::find( m_reqDropOutChar.begin(), m_reqDropOutChar.end(), sDropOut );
		if ( it == m_reqDropOutChar.end() )
		{
			// 케릭터의 db 작업을 예약에 들어감.
			m_reqDropOutChar.push_back( sDropOut );
		}
	}
	return TRUE;
}

BOOL GLGaeaServer::SaveCheckInstantMap( GLChar* pChar )
{
	if( !pChar )
		return FALSE;

	GLLandMan* const pLand(pChar->GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	if( !pLand->IsInstantMap() )
		return FALSE;

	GLLandMan* pInLandMan = NULL;
	DxLandGateMan* pInGateMan = NULL;
	PDXLANDGATE pInGate = NULL;
	D3DXVECTOR3 vPos(0,0,0);

	DxLandGateMan* pOutGateMan = NULL;
	PDXLANDGATE pOutGate = NULL;				

	pOutGateMan = pLand->GetLandGateMan();
	if( !pOutGateMan )
		return FALSE;

	// 무조건 첫번째 게이트로 이동한다.
	DWORD dwGateID = 0;
	DWORD dwOutGateID = 0;
	SNATIVEID sMoveMapId;

	for( dwGateID = 0; dwGateID < pOutGateMan->GetNumLandGate(); dwGateID++ )
	{
		pOutGate = pOutGateMan->FindLandGate ( dwGateID );
		if( pOutGate )
		{

			pInLandMan = GetLand ( pOutGate->GetToMapID() );
			if ( !pInLandMan )		
			{
				pOutGate = NULL;
				continue;
			}

			break;
		}
	}

	// Gate를 못 찾았을경우
	if( !pOutGate )
	{
		dwOutGateID			  = GLCONST_CHAR::dwSTARTGATE[pChar->m_wSchool];
		SNATIVEID sStartMapID = GLCONST_CHAR::nidSTARTMAP[pChar->m_wSchool];
		pInLandMan = GetLand ( sStartMapID );
		if ( !pInLandMan )		return FALSE;

	}else{
		pInLandMan = GetLand ( pOutGate->GetToMapID() );
		if ( !pInLandMan )
			return FALSE;

		dwOutGateID = pOutGate->GetToGateID();			
	}

	// 목표 게이트 가져오기.					//

	pInGateMan = pInLandMan->GetLandGateMan ();
	if ( !pInGateMan )
		return FALSE;
	pInGate = pInGateMan->FindLandGate ( dwOutGateID );
	if ( !pInGate )
		return FALSE;

	const SNATIVEID _baseMapIDIn(pInLandMan->GetBaseMapID());
	SMAPNODE *pMapNode = FindMapNode(_baseMapIDIn);
	if ( !pMapNode )
		return FALSE;

	vPos = pInGate->GetGenPos ( DxLandGate::GEN_RENDUM );	

	const MapID& _mapIDIn(pInLandMan->GetMapID());
	pChar->SetCurrentMap(_mapIDIn);
	pChar->SetPosition(vPos);

	return TRUE;
}

BOOL GLGaeaServer::FindCreatingCharUserID(DWORD dwUserID)
{
    return m_pCharMan->CreatingCharFind(dwUserID);
}

void GLGaeaServer::SetCreatingCharUserID(DWORD dwUserID)
{
    m_pCharMan->CreatingCharAdd(dwUserID);
}

BOOL GLGaeaServer::ResetCreatingCharUserID(DWORD dwUserID)
{
    m_pCharMan->CreatingCharDel(dwUserID);
	return TRUE;
}

BOOL GLGaeaServer::FindSaveDBUserID ( DWORD dwUserID )
{
    return m_pCharMan->ReqSaveDbUserFind(dwUserID);
}

/*
BOOL GLGaeaServer::FindSaveDBUserID ( DWORD dwUserID )
{
    return m_pCharMan->ReqSaveDbUserFind(dwUserID);
}

/*
BOOL GLGaeaServer::FindSaveDBUserID ( DWORD dwUserID )
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CSPCLock);
	BOOL bFOUND(FALSE);
    PCID_ITER iter = m_reqSaveDBUserID.find(dwUserID);
    bFOUND = (iter != m_reqSaveDBUserID.end());
	return bFOUND;
}
*/
/*
void GLGaeaServer::SetSaveDBUserID ( DWORD dwUserID )
{
    sc::CriticalSectionOwnerLock OwnerLock(m_CSPCLock);
    m_reqSaveDBUserID.insert(dwUserID);
}
*/
BOOL GLGaeaServer::ResetSaveDBUserID(DWORD dwUserID)
{
	if ( m_pCharMan )
		m_pCharMan->ReqSaveDbUserDel(dwUserID);
	
	return TRUE;
}

/*BOOL GLGaeaServer::RequestReBirth(
    const DWORD dwGaeaID,
    GLLandMan *pTargetLandMan,
    const DWORD dwGenGate,
    const D3DXVECTOR3& _vPos)
{
	GLChar* pPC = GetChar(dwGaeaID);
	if ( pPC == NULL )
        return FALSE;

	if ( pTargetLandMan == NULL )
		return FALSE;

	if ( pTargetLandMan->GetGaeaMapID() == SNATIVEID(false) )
		return FALSE;
	
	DxLandGateMan* pGateMan = NULL;
	PDXLANDGATE pGate = NULL;
	D3DXVECTOR3 vStartPos;
	if (dwGenGate!=UINT_MAX)
	{
		pGateMan = pTargetLandMan->GetLandGateMan();
		pGate = pGateMan->FindLandGate(dwGenGate);
		if (!pGate)
			return FALSE;

		vStartPos = pGate->GetGenPos(DxLandGate::GEN_RENDUM);
	}
	else
	{
		vStartPos = _vPos;
	}

	if (!IsInstance())
	{
		pPC->SaveLastCall();
	}

	// 자신이 본 주변 셀을 정리
	pPC->ResetViewAround();

	GLLandMan* pCurrentLandMan(pPC->GetLandMan());
	const MapID& _landMapID(pCurrentLandMan == NULL ? SNATIVEID(false) : pCurrentLandMan->GetMapID());	
	// 종전 맵에 있던 관리 노드 삭제
	if ( pCurrentLandMan != NULL )
	{
		if ( pCurrentLandMan->AddGlobPcDelList(pPC->m_pCharNodeInLand) == false )
		{
			const MapID& _charMapID(pPC->GetCurrentMap());
			const MapID& _landMapID(pCurrentLandMan->GetMapID());
			sc::writeLogError(sc::string::format("RequestReBirth() : pPC->m_pCharNodeInLand is invalid, LandMapID(%1%/%2%, %3%/%4%)", 
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}

	if (pPC->m_pQuadNode && pPC->m_pQuadNode->pData)
	{
		if ( pPC->m_pQuadNode->pData->m_PCList.DELNODE(pPC->m_pCellNode) == false )
		{				
			sc::writeLogError(sc::string::format("RequestReBirth() : pPC->m_pCellNode is invalid, LandMapID(%1%/%2%, %3%/%4%)", 
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}
	else
	{
		sc::writeLogError(sc::string::format("RequestReBirth() : pPC->m_pQuadNode is invalid, LandMapID(%1%/%2%, %3%/%4%)",
			_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
	}

	// 새로운 맵 위치로 내비게이션 초기화
	pPC->SetNavi(pTargetLandMan->GetNavi(), vStartPos);
	

	//
	//mjeon.tbb
	//
	pTargetLandMan->EnterLand(pPC);


	// GLLandMan의 셀에 등록하는 작업
	//RegistChar ( pPC ); --> ( GLGaeaServer::RequestLandIn ()이 호출될 때까지 유보됨. )

	pPC->m_dwCeID = 0;
	pPC->m_pQuadNode = NULL;
	pPC->m_pCellNode = NULL;

	return TRUE;
}*/


//
//mjeon.tbb
//FrameMove for static Main Maps (not inserted or deleted during the game.)
//
void GLGaeaServer::FrameMoveLandMan(float fTime, float fElapsedTime, __time64_t &CurrentTime)
{
	g_fCurrentFrameTime = fTime;
	if ( m_pInstanceManager != NULL )
		m_pInstanceManager->frameMove(fTime, fElapsedTime);

#ifdef _USE_TBB

	int size = (int)m_landManagerMap.size();

#ifdef _TBB_LOOP_TYPE_FUNCTOR

	tbb::parallel_for_each( m_landManagerMap.begin(), m_landManagerMap.end(),
							 boost::bind(boost::mem_fn(&GLLandManager::frameMove), _1, fTime, fElapsedTime));

#else	//LoopBody
	
	/*
	parallel_for utilizes the idea of recursive splitting the overall iteration space in half.
	Thus, random access iterators (e.g. std::vector::iterator) could be efficiently used 
	 with parallel_for via blocked_range:

	A general forward iterator can be inherently serial, like in lists:
	 one can not access an element in the middle until all previous elements are traversed.
	 TBB's blocked range will not work with such an iterator.
	*/	

    // 	tbb::parallel_for_each( m_landManagerMap.begin(), m_landManagerMap.end(),
    // 		boost::bind(boost::mem_fn(&GLLandManager::frameMove), _1, fTime, fElapsedTime, boost::ref(CurrentTime)) );
    // 	tbb::parallel_for_each(m_landManagerMap.begin(), m_landManagerMap.end(), );

    tbb::parallel_for(tbb::blocked_range<int>(0, size), LoopBodyFrame(fTime, fElapsedTime, boost::ref(CurrentTime), m_landManagerMap), tbb::auto_partitioner());	

#endif		 

	
#else	//_USE_TBB

#ifdef USE_OPEN_MP // Open MP
    int LandSize = (int) m_landManagerMap.size();
#pragma omp parallel for
    for (int i=0; i<LandSize; ++i)
        m_landManagerMap[i].pLandManager->frameMove(fTime, fElapsedTime);

#else	//Open MP
	
	size_t i, size = m_landManagerMap.size();

	BOOST_FOREACH( LandManagerPair& rValue, m_landManagerMap )
	{
		rValue.pLandManager->frameMove(fTime, fElapsedTime, CurrentTime);
	}

#endif	//Open MP


#endif	//_USE_TBB

	// 인던 삭제 처리, 인던 생성과 진입을 처리 다른 곳으로 이동 -> ExecuteReservedTask -> FrameMoveInstanceMapExtra
}

void GLGaeaServer::FrameMoveInstanceMapExtra( float fElapsedTime )
{
	//mjeon.인던 삭제 처리, fElapsedTime 저 함수안에서 사용하지 않는다.
	DeleteInstantMaps( fElapsedTime );

	//mjeon.인던 생성과 진입을 처리
	CreateAndEnterInstantMaps();
}

HRESULT GLGaeaServer::FrameMove(float fTime, float fElapsedTime)
{
	m_fTime = fTime;
    CTime CurrentDate(CTime::GetCurrentTime());
    __time64_t CurrentTime = CurrentDate.GetTime();

    // 날씨 갱신.
    GLPeriod::GetInstance().UpdateWeather(fElapsedTime);
    GLPeriod::GetInstance().FrameMove(fTime, fElapsedTime);
	

	if (!m_bUpdate)
		return S_OK;

	//
	//mjeon.instance
	//
    //
    //mjeon.CaptureTheField
    //
    FrameMoveCTF(fElapsedTime);	

    // 선도 지역 갱신
    FrameMoveGuidance(fElapsedTime);

    // 클럽데스매치 갱신
    FrameMoveClubDeathMatch(fElapsedTime);

    // 파티 아이템 입찰 시스템 갱신;
    FrameMovePartyTender(fTime, fElapsedTime);

    // 미니게임 갱신;
    FrameMoveMiniGame(fTime, fElapsedTime);

    //
    //mjeon.tbb
    //
    FrameMoveLandMan(fTime, fElapsedTime, CurrentTime);	

	FrameMoveInstanceMapExtra( fElapsedTime );

    // [2010.04.12] LevelEdit 돌릴때 뻑나서 if 문 추가
    if (m_pClubMan)
    {
        m_pClubMan->FrameMoveField( fTime, fElapsedTime );
    }

	// 선도전 끝난직후 잔여 클럽원들 체크
	if (m_pGuidance->IsCheckExtraGuild())
		m_pGuidance->CheckExtraGuild(fElapsedTime);

	// Provide Reward System;
	if ( false == GLUseFeatures::GetInstance().IsUsingWorldBattle() )
	{
		ProvideRewardSystem::GLProvideRewardManager* pProvideRewardMan =
			ProvideRewardSystem::GLProvideRewardManager::Instance();
		if ( pProvideRewardMan )
			pProvideRewardMan->OnFrameMove( fElapsedTime );
	}

	// 예약 메시지들 처리
	ReserveMessageProcess();

	ClearReservedDropOutPC();	//PC
	ClearReserveDropOutPet();	//Pet

    m_pVehicleCache->FrameMove();

	// NULL 체크를 하지 않으면 LevelEdit 에서 Error 발생.
	if ( m_pCharMan )
	{
		m_pCharMan->FrameMove( fElapsedTime );
	}

	// CTF 강제 부활 처리
	ProcessCTFRebirth();

	// Move System;
	MoveSystem::CMoveManagerField* pMoveMan =
		MoveSystem::CMoveManagerField::Instance();
	if ( pMoveMan )
		pMoveMan->OnFrameMove( fElapsedTime );

	if ( m_pMoneyMan )
		m_pMoneyMan->OnUpdate( fElapsedTime );

	return S_OK;
}

HRESULT GLGaeaServer::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
    size_t tSize = m_landManagerMap.size();
    for( size_t i=0; i<tSize; ++i )
    {
        m_landManagerMap[i].pLandManager->render( pd3dDevice, cv );
    }

	return S_OK;
}

void GLGaeaServer::ChangeNameMap(GLChar* pChar, const char* pszOldName, const char* pszNewName)
{
    m_pCharMan->ChaNameDel(pChar->GetCharData2().m_szName);

    StringCchCopy(pChar->GetCharData2().m_szName, CHAR_SZNAME, pszNewName);
    m_pCharMan->ChaNameAdd(pChar->GetCharData2().m_szName, pChar);
}

void GLGaeaServer::ChangeNameMap(GLChar* pChar, const TCHAR* pszPhoneNumber)
{
    if(m_pCharMan->ChaNameExist(pChar->GetCharData2().m_szName))
    {
		StringCchCopy(pChar->GetCharData2().m_szPhoneNumber, SMS_RECEIVER, pszPhoneNumber);
	}
}

BOOL GLGaeaServer::ReserveServerStop(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if( dwClientID != 0 )
	{
		sc::writeLogError(sc::string::format("GLGaeaServer::ReserveServerStop() - ClientID(%1%) is weird.", dwClientID));
		return FALSE;
	}

	GLMSG::SNET_REQ_SERVERSTOP *msg = (GLMSG::SNET_REQ_SERVERSTOP *)nmg;
	if (msg->dwCode != GLMSG::SNET_REQ_SERVERSTOP::CODE_MAGIC_NUMBER)
	{
		sc::writeLogError(sc::string::format("GLGaeaServer::ReserveServerStop() - MagicNumber unmatched.(%1%/%2%)", msg->dwCode, GLMSG::SNET_REQ_SERVERSTOP::CODE_MAGIC_NUMBER));
		return FALSE;
	}

	m_bReservedStop = true;
	m_dwError = msg->dwError;

	if ( m_pMoneyMan )
		m_pMoneyMan->OnStop();

	// 주변을 정리한다.
	/*
	for (UINT k=0; k<m_dwMaxClient; k++ )
	{
		if(m_PCArray[k])
		{
			m_PCArray[k]->ResetViewAround ();
			m_PCArray[k]->SetSTATE(EM_GETVA_AFTER);
			m_PCArray[k]->ReSetSTATE(EM_ACT_WAITING);
		}
	}
	*/
	UMAP_GLCHAR_ITER iter = m_mapCharacters.begin();
	for (; iter != m_mapCharacters.end(); ++iter)
	{
		GLChar *pChar = iter->second;

		if (pChar)
		{
			pChar->ResetViewAround ();
			pChar->SetActState(EM_GETVA_AFTER);
			pChar->ReSetActState(EM_ACT_WAITING);
		}
	}

	// 모든 필드서버의 맵에 있는 오브젝트(캐릭터 제외)를 제거한다.	
    size_t tSize = m_landManagerMap.size();
    for( size_t i=0; i<tSize; ++i )
    {
        m_landManagerMap[i].pLandManager->clearObjectExceptCharacter();
    }

	if ( SERVER_STOP_JUST_RESTART != msg->dwError )
	{
		GLMSG::SNET_REQ_SERVERSTOP_FB NetMsgFB;
		NetMsgFB.dwError = msg->dwError;

		// http://crm.mincoms.com/Require/RequireView/1639
		// 대만은 SERVER_STOP_OUTOFMEMORY, SERVER_STOP_JUST_RESTART 에러일 경우 서버재시작 메시지 출력하지 않도록 함
		if ( SP_HONGKONG == m_ServiceProvider )
		{
			if ( SERVER_STOP_NOMAL == NetMsgFB.dwError )
			{
				// 클라이언트에게 서버테스트를 위한 멈춤을 알림
				SENDTOALLCLIENT(&NetMsgFB);
			}
		}
		else
		{
			// 클라이언트에게 서버테스트를 위한 멈춤을 알림
			SENDTOALLCLIENT(&NetMsgFB);
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::RequestUsePETCARD ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (IsReserveServerStop())
        return FALSE;

	GLMSG::SNETPET_REQ_USEPETCARD* pNetMsg = (GLMSG::SNETPET_REQ_USEPETCARD*)nmg;	
	GLMSG::SNETPET_REQ_USEPETCARD_FB NetMsgFB;

	GLChar* pOwner = GetChar ( dwGaeaID );
	if ( !pOwner )
	{
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return FALSE;
	}

    if ( pOwner->GetRemainTimeSummonPet() > 0.0f )
    {
        NetMsgFB.emFB = EMPET_USECARD_FB_ACTIONLIMIT;
        SENDTOCLIENT(dwClientID, &NetMsgFB);

        return FALSE;
    }

	// 수독 효과에 걸려 있다	
	if ( pOwner->IsCheckedSkillFlagSpecial(EMSPECA_RELEASE_PET) )
	{		
		NetMsgFB.emFB = EMPET_USECARD_FB_FAIL;
		SENDTOCLIENT(dwClientID, &NetMsgFB);
		return E_FAIL;
	}

    //	아이템을 사용할수 없는 상황이다.
    if ( pOwner->IsActionLimit( EMACTION_LIMIT_PET) )
    {
        NetMsgFB.emFB = EMPET_USECARD_FB_ACTIONLIMIT;
        SENDTOCLIENT(dwClientID, &NetMsgFB);
        return E_FAIL;
    }

    // 대련 중이면 펫 소환 할 수 없다.
    if ( pOwner->m_sCONFTING.IsCONFRONTING() )
    {
        NetMsgFB.emFB = EMPET_USECARD_FB_ACTIONLIMIT;
        SENDTOCLIENT(dwClientID, &NetMsgFB);
        return E_FAIL;
    }

	// 맵진입가능여부 체크
	const GLLandMan* const pLandMan(pOwner->GetLandMan());
	if ( (pLandMan == NULL) || (pLandMan->IsPetActivity() == false) )
	{
		NetMsgFB.emFB = EMPET_USECARD_FB_INVALIDZONE;
		SENDTOCLIENT(dwClientID, &NetMsgFB);
		return FALSE;
	}

	// 팻이 활성화 되어있으면
	GLPetField* pMyPet = GetPET(pOwner->m_dwPetGUID);
	// if ( pMyPet && pMyPet->IsValid () ) return FALSE;


	SINVENITEM* pInvenItem = pOwner->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
	{
		NetMsgFB.emFB = EMPET_USECARD_FB_NOITEM;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}

	/*if( pMyPet )
	{	
		sc::writeLogError ( "## MyPet Info PetGUID %d PetOwnerGUID %d CharGUID %d PetID %d OwnerPetID %d", 
								pMyPet->m_dwGUID, pMyPet->m_pOwner->m_dwPetGUID, pOwner->m_dwPetGUID, pMyPet->GetPetID(), pInvenItem->sItemCustom.dwPetID  );
	}*/

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD )
	{
		NetMsgFB.emFB = EMPET_USECARD_FB_INVALIDCARD;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
		return E_FAIL;
	}

	// Note : 레벨 조건 확인
	//
	if ( !pOwner->SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	// RM #840 아이템 구매조건에 특별활동 및 기여도를 없애서 일부 아이템이 비정상적으로 사용되어서 수정 (박스, 코스튬 등)
	if ( !pOwner->CHECK_ACTIVITY_POINT( *pItem ) ) 
    {
        //	활동 포인트가 부족합니다.
		NetMsgFB.emFB = EMPET_USECARD_FB_ERR_ACTIVITY;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
        return E_FAIL;
    }

    if ( !pOwner->CHECK_CONTRIBUTION_POINT( *pItem ) ) 
    {
        //	기여 포인트가 부족합니다.
		NetMsgFB.emFB = EMPET_USECARD_FB_ERR_CONTRIBUTION;
		SENDTOCLIENT ( dwClientID, &NetMsgFB );
        return E_FAIL;
    }

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pItem->sPet.sPetID );
	if ( !pCrowData )
        return E_FAIL;

	// 에뮬레이터 모드
	if ( m_bEmulator )
	{
		PGLPET pPet = GLCONST_PET::GetPetData ( pItem->sPet.emPetType );
		if ( !pPet )
		{
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return E_FAIL;
		}

		GLPET NewPet(*pPet);
		NewPet.m_sPetID = pItem->sPet.sPetID;

		// 팻을 생성해준다
		CreatePET ( &NewPet, pOwner->m_CharDbNum, 0 );

		if ( GLHIDESET::CheckHideSet(EMHIDE_PET_SUMMON) )
		{
			pOwner->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			pOwner->RestoreActStateByInvisible();
		}

		return S_OK;
	}

	// 신규생성
	if ( pInvenItem->sItemCustom.PetDbNum() == 0 )
	{
		// 혹시 팻이 있으면 (하나의 케릭터가 동시에 복수개의 팻을 만드는 것을 방지)
		if ( pMyPet ) 
		{
			if ( !DropOutPET ( pOwner->m_dwPetGUID, true, false ) )
			{
                sc::writeLogError(
                    sc::string::format(
                        "RequestUsePETCARD DropOutPET PetGUID:%1%",
                        pOwner->m_dwPetGUID));				
			}
		}

		PGLPET pPetData = GLCONST_PET::GetPetData ( pItem->sPet.emPetType );
		if ( !pPetData )
		{
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return E_FAIL;
		}

		AddGameAdoJob(
            db::DbActionPtr(
                new db::PetCreate(
                    dwClientID, 
                    (int) pOwner->m_CharDbNum, 
                    pCrowData->GetName(),
                    pPetData->m_emTYPE,
                    (int) pItem->sPet.sPetID.wMainID,
                    (int) pItem->sPet.sPetID.wSubID,
                    (int) pPetData->m_wStyle,
                    (int) pPetData->m_wColor,
                    pNetMsg->wPosX,
                    pNetMsg->wPosY, 
                    (int) pItem->sBasicOp.sNativeID.wMainID,
                    (int) pItem->sBasicOp.sNativeID.wSubID)));
		

		// 일단 생성중인 번호임을 알리기 위해
		pInvenItem->sItemCustom.PetDbNumSet(UINT_MAX);

		pOwner->m_bGetPetFromDB = true;
	}
	// DB에서 데이터 불러와서 팻소환
	else 
	{
		// 이미 팻을 부르고 있으면
		if (pOwner->m_bGetPetFromDB)
            return TRUE;


        // 팻이 정상적으로 DropOut 되지 않고 서버에 잔존하는 경우가 생겼다...(^^;;;)
		if (pMyPet && pMyPet->IsValid())
        {            
            sc::writeLogError(
                sc::string::format(
                    "RequestUsePETCARD() pMyPet->IsValid, PetGUID:%1%",
                    pMyPet->m_dwGUID));
            DropOutPET ( pMyPet->m_dwGUID, true, false );
            pMyPet = NULL;
        }

		// 팻이 로직에 남아 있으면
		if (pMyPet) 
        {
            //  여기는 DB에서 정보를 읽어오지 않고 로직상에 등록된 정보로 펫을 생성한다.
            //  단, 펫이 비활성화중일때만 사용가능하다.
            //  활성화중일때 접근하면 LandMan에 중복 등록이 되어 버린다.
            if (pMyPet->DbNum () == pInvenItem->sItemCustom.PetDbNum()) 
		    {
				/// CreatePET() : 답장 송신하는 부분, 위에 에뮬 모드 동작도 처리 해 줄 것;
			    GLPET* pPet = dynamic_cast<GLPET*> (pMyPet);
			    CreatePET(pPet, pOwner->m_CharDbNum, pMyPet->DbNum());

				if ( GLHIDESET::CheckHideSet(EMHIDE_PET_SUMMON) )
				{
					pOwner->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
					pOwner->RestoreActStateByInvisible();
				}

			    return TRUE;
		    }
            else 
            {
                if (!DropOutPET(pMyPet->m_dwGUID, true, false))
			    {				    
                    sc::writeLogError(
                        sc::string::format(
                            "RequestUsePETCARD() DropOutPET PetGUID:%1%",
                            pMyPet->m_dwGUID));
			    }
            }
        }

		PGLPET pPetData = GLCONST_PET::GetPetData ( pItem->sPet.emPetType );
		if ( !pPetData )
		{
			SENDTOCLIENT ( dwClientID, &NetMsgFB );
			return E_FAIL;
		}
		
		GLPetField* pNewPet = new GLPetField(this);
		pNewPet->ASSIGN ( *pPetData );

		// DB작업 실패시 혹은 생성후 메모리 해제해줘라
		AddGameAdoJob(
            db::DbActionPtr(
                new db::PetGet(
                    pNewPet,
                    pInvenItem->sItemCustom.PetDbNum(),
                    dwClientID,
                    pOwner->m_CharDbNum,
                    pNetMsg->wPosX,
                    pNetMsg->wPosY)));
		pOwner->m_bGetPetFromDB = true;
	}
	
    pOwner->ResetLimitTimeSummonPet();

	return TRUE;
}

BOOL GLGaeaServer::RequestUnUsePETCARD ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_UNUSEPETCARD* pNetMsg = ( GLMSG::SNETPET_REQ_UNUSEPETCARD* ) nmg;
	GLPetField* pPet = GetPET ( pNetMsg->dwGUID );
	if ( pPet && pPet->DbNum () == pNetMsg->dwPetID )
	{
		DropOutPET ( pNetMsg->dwGUID, false, false );
		GLChar* pOwner = GetChar(dwGaeaID);
		if ( GLHIDESET::CheckHideSet(EMHIDE_PET_SUMMON_RELEASE) )
		{
			pOwner->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
			pOwner->RestoreActStateByInvisible();
		}
	}
	return TRUE;
}

BOOL GLGaeaServer::RequestRevivePet ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_REVIVE* pNetMsg = (GLMSG::SNETPET_REQ_REVIVE*)nmg;
	GLChar* pOwner = GetChar(dwGaeaID);
	if ( !pOwner )
		return FALSE;

	// 아이템이 팻카드인지 체크
	SINVENITEM* pInvenItem = pOwner->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
		return FALSE;
	
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return FALSE;

	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD )
		return FALSE;

    SINVENITEM* pHoldInvenItem = pOwner->InvenGetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
    if ( !pHoldInvenItem )
        return E_FAIL;

	// 손에든 아이템이 팻부활카드인지 체크
	const SITEMCUSTOM sHold = pHoldInvenItem->CustomItem();

	const SITEM* pHoldItem = GLogicData::GetInstance().GetItem( sHold.GetNativeID() );
	if ( !pHoldItem )
		return FALSE;
	
	if ( pHoldItem->sBasicOp.emItemType != ITEM_PET_REVIVE )
		return FALSE;

	// 펫 액션 로그
	m_pItemLimit->LogPetAction(pNetMsg->dwPetID, pInvenItem->sItemCustom.GetNativeID(), pet::EM_ACTION_REVIVE, 0);

	// 삭제된 팻을 부활하고 새롭게 생성하는 코드 (반드시 Full 이 100.00% 여야 함!)
    AddGameAdoJob(
        db::DbActionPtr(
            new db::PetRestore(
                pNetMsg->dwPetID, 
                dwClientID,
                pOwner->m_CharDbNum, 
                pNetMsg->wPosX, 
                pNetMsg->wPosY,
                pNetMsg->wHoldPosX,
                pNetMsg->wHoldPosY )));
	return TRUE;
}

// #item
// 디비에서 오는 메시지이다. 이거도 예전에 만들어진 거 처럼 클라이언트 슬롯을 디비 액션할 때 넣어 보내 클라이언트가
// 보낸 메시지처럼 헤깔리게 되어있고, 실제 클라이언트가 저 메시지 만들어보내면 대책없다.
BOOL GLGaeaServer::RevivePet( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_REVIVE_FROMDB_FB* pNetMsg = ( GLMSG::SNETPET_REQ_REVIVE_FROMDB_FB* )nmg;
	GLChar* pOwner = GetChar( dwGaeaID );
	if ( !pOwner )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on RevivePet", dwGaeaID ) );
        return FALSE;
	}

	// 아이템이 팻카드인지 체크
	SINVENITEM* pInvenItem = pOwner->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pInvenItem )
        return FALSE;
	
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
        return FALSE;

	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD )
        return FALSE;

    SINVENITEM* pHoldInvenItem = pOwner->InvenGetItem( pNetMsg->wHoldPosX, pNetMsg->wHoldPosY );
    if ( !pHoldInvenItem )
        return E_FAIL;

	// 손에든 아이템이 팻부활카드인지 체크
	const SITEMCUSTOM& sHold = pHoldInvenItem->CustomItem();

	const SITEM* pHoldItem = GLogicData::GetInstance().GetItem( sHold.GetNativeID() );
	if ( !pHoldItem )
        return FALSE;
	
	if ( pHoldItem->sBasicOp.emItemType != ITEM_PET_REVIVE )
        return FALSE;

	// 기존의 팻 제거
	if ( pOwner->GetPetDbNum() == pInvenItem->sItemCustom.PetDbNum() )
	{
		DropOutPET( pOwner->m_dwPetGUID, true, false );
	}
	
	if ( pInvenItem->sItemCustom.PetDbNum() != 0 )
	{
		AddGameAdoJob(
            db::DbActionPtr(
                new db::PetDelete( pOwner->m_CharDbNum, pInvenItem->sItemCustom.PetDbNum() ) ) );
	}

	// 팻카드의 팻ID를 부활시킨 팻ID로 변경.
	pInvenItem->sItemCustom.PetDbNumSet( pNetMsg->m_PetDbNum );

	// 팻 부활카드 사용로그 남김
	// 통계 및 로그 수정
	m_pItemLimit->LogItemExchange( sHold, gamelog::ID_CHAR, 0, gamelog::ID_CHAR, pOwner->m_CharDbNum, ITEM_ROUTE_DELETE, sHold.wTurnNum, 
		(pOwner == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pOwner->GetCurrentMap(), LOG_ITEM_DEL_PET_REVIVE, 
		false );

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
    WORD wHoldPosY = sHold_BackUp.wPosY;

	// 인벤에 홀드된 팻부활 카드를 제거해준다.
	pOwner->InvenDeleteItem( wHoldPosX, wHoldPosY, true, sHold_BackUp.CustomItem().bVietnamGainItem, 209, true );

	// 팻 부활 성공을 알림
	GLMSG::SNETPET_REQ_REVIVE_FB NetMsg(EMPET_REQ_REVIVE_FB_OK,pNetMsg->m_PetDbNum);
	SENDTOCLIENT( dwClientID, &NetMsg );

	// 부활된 팻카드의 정보를 알려준다.	
	GLPetField* pPetInfo = new GLPetField( this );

	// DB작업 실패시 혹은 생성후 메모리 해제해줘라
    AddGameAdoJob(
        db::DbActionPtr(
            new db::PetGet(
                pPetInfo,
                pNetMsg->m_PetDbNum,
                dwClientID,
                pOwner->m_CharDbNum, 
                pNetMsg->wPosX,
                pNetMsg->wPosY,
                false,
                true)));

	return TRUE;
}

BOOL GLGaeaServer::CreatePETOnDB ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLMSG::SNETPET_CREATEPET_FROMDB_FB* pNetMsg = (GLMSG::SNETPET_CREATEPET_FROMDB_FB*)nmg;
    switch ( pNetMsg->emFB )
    {
    case EMPET_CREATEPET_FB_FAIL:
        {
		    // 실패하면 팻카드의 팻번호를 다시 0으로 되돌려 놓는다. (재생성 가능하게 하기 위해)
		    GLChar* pOwner = GetChar ( dwGaeaID );
	        if ( !pOwner ) return FALSE;

            SINVENITEM* pInvenItem = pOwner->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
            if (pInvenItem)
                pInvenItem->sItemCustom.PetDbNumSet(0);

			pOwner->m_bGetPetFromDB = false;  
        }
        break;
    case EMPET_CREATEPET_FB_OK:
        {
	        GLMSG::SNETPET_REQ_USEPETCARD_FB NetMsgFB;

	        GLChar* pOwner = GetChar ( dwGaeaID );
	        if ( !pOwner )
			{
				// 통계 및 로그 수정
				// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
				sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on CreatePETOnDB ", dwGaeaID));
				return FALSE;
			}
        	
	        pOwner->m_bGetPetFromDB = false;

	        SINVENITEM* pInvenItem = pOwner->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	        if ( !pInvenItem )
	        {
		        NetMsgFB.emFB = EMPET_USECARD_FB_NOITEM;
		        SENDTOCLIENT ( dwClientID, &NetMsgFB );
		        return FALSE;
	        }

	        const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	        if ( !pItem )
	        {
		        NetMsgFB.emFB = EMPET_USECARD_FB_FAIL;
		        SENDTOCLIENT ( dwClientID, &NetMsgFB );
		        return FALSE;
	        }

	        if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD )
	        {
		        NetMsgFB.emFB = EMPET_USECARD_FB_INVALIDCARD;
		        SENDTOCLIENT ( dwClientID, &NetMsgFB );
		        return FALSE;
	        }

            //	아이템을 사용할수 없는 상황이다.
            if ( pOwner->IsActionLimit(EMACTION_LIMIT_PET) )
            {
                NetMsgFB.emFB = EMPET_USECARD_FB_ACTIONLIMIT;
                SENDTOCLIENT(dwClientID, &NetMsgFB);
                return FALSE;
            }

	        // 맵진입가능여부 체크
	        const GLLandMan* const pLandMan(pOwner->GetLandMan());
	        if ( pLandMan == NULL )
                return FALSE;
	        if ( pLandMan->IsPetActivity() == false )
	        {
		        pInvenItem->sItemCustom.PetDbNumSet(pNetMsg->DbNum());

		        NetMsgFB.emFB = EMPET_USECARD_FB_INVALIDZONE;
		        SENDTOCLIENT ( dwClientID, &NetMsgFB );		
		        return FALSE;
	        }

	        const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pItem->sPet.sPetID );
	        if ( !pCrowData )
                return FALSE;
        	
	        // 팻 아이디 설정
	        pInvenItem->sItemCustom.m_PetDbNum = pNetMsg->DbNum();

	        PGLPET pPet = GLCONST_PET::GetPetData ( pItem->sPet.emPetType );
	        if ( !pPet )
                return FALSE;

	        // 신규생성
	        GLPET NewPet;
	        NewPet.ASSIGN ( *pPet );
	        NewPet.m_emTYPE = pItem->sPet.emPetType;
	        NewPet.m_sPetID = pItem->sPet.sPetID;
	        NewPet.m_sPetCardID = pInvenItem->sItemCustom.GetNativeID();
	        NewPet.SetName(pCrowData->GetName());
	        GLPetField* pMyPet = CreatePET(&NewPet, pOwner->m_CharDbNum, pNetMsg->DbNum());
	        if (!pMyPet)
                return FALSE;

	        /*	
	        // 만약 여기서 성공하면 소유권을 생성해준다.
	        CExchangePet* pDbAction = new CExchangePet ( pOwner->m_dwCharID, pNetMsg->dwPetID );
	        m_pDBMan->AddJob ( pDbAction );
	        */

	        // 팻카드의 정보도 수정해준다.
	        GLMSG::SNETPET_REQ_PETCARDINFO_FB NetMsg(
                pMyPet->GetName(),
                pMyPet->DbNum(),
                pMyPet->m_emTYPE,
                pMyPet->Satiation(),
                pMyPet->m_wMaxSkillSlot,
                static_cast<WORD>(pMyPet->m_ExpSkills.size()),
                false);

	        PETSKILL_MAP_ITER iter = pMyPet->m_ExpSkills.begin();
	        PETSKILL_MAP_ITER iter_end = pMyPet->m_ExpSkills.end();
	        WORD i(0);  
	        for ( ;iter != iter_end; ++iter )
	        {
		        NetMsg.Skills[i++] = iter->second;
	        }

	        for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	        {
		        NetMsg.PutOnItems[i] = pMyPet->m_PutOnItems[i];
	        }

	        for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
	        {
		        NetMsg.sActiveSkillID[i] = pMyPet->m_sActiveSkillID[i];
	        }

	        SENDTOCLIENT ( dwClientID, &NetMsg );

	        // 로그에 최초생성을 남김
			// 통계 및 로그 수정
	        m_pItemLimit->LogItemExchange ( pInvenItem->sItemCustom, gamelog::ID_CHAR, pOwner->m_CharDbNum, gamelog::ID_CHAR, 0, ITEM_ROUTE_PET_CARD, 1, (pOwner == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pOwner->GetCurrentMap(), LOG_ITEM_PETCARD_CREATE_PET, false );

        }
        break;
    }
	return TRUE;
}

BOOL GLGaeaServer::GetPETInfoFromDB ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    // DB에서 온 패킷이 아니라면 작업 할 수 없다.
    if ( DBACTION_CLIENT_ID != dwClientID )
        return FALSE;

	// [주의]DB에서 넣어주지 않아서 pNetMsg->pPet->m_dwPetID 값은 유효하지 않다.
	GLMSG::SNETPET_GETPET_FROMDB_FB* pNetMsg = ( GLMSG::SNETPET_GETPET_FROMDB_FB* )nmg;
	GLMSG::SNETPET_REQ_USEPETCARD_FB NetMsgFB;

	GLChar* pChar = GetCharByDbNum( pNetMsg->dwCharDBNum );
	if ( !pChar )
	{
		SAFE_DELETE ( pNetMsg->pPet );
		return FALSE;
	}

	// DB에 없으면 그냥 초기값으로 넘어오는 경우가 있으므로 체크해준다.
	if ( pNetMsg->pPet->m_emTYPE == PETTYPE_NONE )
	{
		SAFE_DELETE ( pNetMsg->pPet );
		return FALSE;
	}

	// 팻카드 정보 발신
	if ( pNetMsg->bTrade || pNetMsg->bCardInfo )
	{
		GLMSG::SNETPET_REQ_PETCARDINFO_FB NetMsgFB(
            pNetMsg->pPet->GetName(),
            pNetMsg->dwPetID,
            pNetMsg->pPet->m_emTYPE,
            pNetMsg->pPet->Satiation(),
            pNetMsg->pPet->m_wMaxSkillSlot,
            static_cast< WORD >( pNetMsg->pPet->m_ExpSkills.size() ),
            pNetMsg->bTrade );

		PETSKILL_MAP_ITER iter      = pNetMsg->pPet->m_ExpSkills.begin();
		PETSKILL_MAP_ITER iter_end  = pNetMsg->pPet->m_ExpSkills.end();
		WORD i( 0 );
		for ( ; iter != iter_end; ++iter )
            NetMsgFB.Skills[i++] = iter->second;
		for ( WORD i = 0; i < ACCETYPESIZE; ++i )
            NetMsgFB.PutOnItems[i] = pNetMsg->pPet->m_PutOnItems[i];
		for ( WORD i = 0; i < MAX_PETSKILLSLOT; ++i )
            NetMsgFB.sActiveSkillID[i] = pNetMsg->pPet->m_sActiveSkillID[i];

		SENDTOCLIENT( pNetMsg->dwClientID, &NetMsgFB );
	}
	// 팻 악세서리의 시효성 검사
	else if ( pNetMsg->bLMTItemCheck )
	{
		if ( pChar )
		{
			// 없앨꺼 없애고
			GLPetField* pPetField = pNetMsg->pPet;
			pPetField->UpdateTimeLmtItem( pChar );
			
			// 넣어줄꺼 넣어준다.
			for ( WORD i = 0; i < ACCETYPESIZE; ++i )
			{
				if ( pNetMsg->pPet->m_PutOnItems[i].GetNativeID() == NATIVEID_NULL() )
				{
					continue;
				}

				CItemDrop cDropItem;
				cDropItem.sItemCustom = pNetMsg->pPet->m_PutOnItems[i];
				if ( pChar->IsInsertToInven ( &cDropItem ) ) 
				{
					pChar->InsertToInven ( &cDropItem, false, false, false );
				}
				else
				{
					pChar->DropItem( cDropItem.sItemCustom, pChar->GetPosition() );
				}
			}
			
			AddGameAdoJob(
                db::DbActionPtr(
                    new db::PetDelete( pChar->m_CharDbNum, pNetMsg->dwPetID ) ) );
			
			// 팻이 삭제되면 부활정보를 클라이언트에 알려줌.
			AddGameAdoJob(
                db::DbActionPtr(
                    new db::PetListRestoreGet( pChar->m_CharDbNum, pChar->ClientSlot() ) ) );
		}
	}
	else
	{
        //	아이템을 사용할수 없는 상황이다.
        if ( pChar->IsActionLimit( EMACTION_LIMIT_PET ) )
        {
            SAFE_DELETE ( pNetMsg->pPet );
            pChar->m_bGetPetFromDB = false;

            NetMsgFB.emFB = EMPET_USECARD_FB_ACTIONLIMIT;
            SENDTOCLIENT( pNetMsg->dwClientID, &NetMsgFB );
            return FALSE;
        }

		// 맵진입가능여부 체크
		const GLLandMan* const pLandMan( pChar->GetLandMan() );
		if ( ( pLandMan == NULL ) || ( pLandMan->IsPetActivity() == false ) )
		{
			SAFE_DELETE ( pNetMsg->pPet );
			pChar->m_bGetPetFromDB = false;

			NetMsgFB.emFB = EMPET_USECARD_FB_INVALIDZONE;
			SENDTOCLIENT ( pNetMsg->dwClientID, &NetMsgFB );					
			return FALSE;
		}

		// 팻을 생성해준다
		CreatePET( pNetMsg->pPet, pChar->m_CharDbNum, pNetMsg->dwPetID );
		if ( pChar )
            pChar->m_bGetPetFromDB = false;	// ?
	}
	
	SAFE_DELETE ( pNetMsg->pPet );
	return TRUE;
}

BOOL GLGaeaServer::GetPETInfoFromDBError( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_GETPET_FROMDB_ERROR* pNetMsg = (GLMSG::SNETPET_GETPET_FROMDB_ERROR*)nmg;
    switch ( pNetMsg->emFB )
    {
    case EMPET_GETPET_FROMDB_DBFAIL:
        {
		    GLChar* pOwner = GetChar ( dwGaeaID );
		    if ( pOwner )
		    {
			    pOwner->m_bGetPetFromDB = false;
		    }
        }
        break;
    case EMPET_GETPET_FROMDB_NODATA:
        {
	        GLMSG::SNETPET_REQ_USEPETCARD_FB NetMsgFB;

	        GLChar* pChar = GetChar ( dwGaeaID );
	        if ( !pChar )
	        {
		        return FALSE;
	        }

	        // 팻카드 정보 발신
	        if ( pNetMsg->bTrade || pNetMsg->bCardInfo || pNetMsg->bLMTItemCheck )
	        {
		        return FALSE;
	        }

	        SINVENITEM* pInvenItem = pChar->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	        if (pInvenItem)
                pInvenItem->sItemCustom.PetDbNumSet(0);
        	
	        pChar->m_bGetPetFromDB = false;

	        NetMsgFB.emFB = EMPET_USECARD_FB_NODATA;
	        SENDTOCLIENT ( dwClientID, &NetMsgFB );		
        }
        break;
    }
	return TRUE;
}

BOOL GLGaeaServer::MsgPETInfoCardReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_PETCARDINFO* pNetMsg = ( GLMSG::SNETPET_REQ_PETCARDINFO* ) nmg;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar ) return FALSE;

	// 팻 생성여부 확인
	if ( pNetMsg->m_PetDbNum <= 0 || pChar->m_CharDbNum <= 0 )
	{
		sc::writeLogError(
            sc::string::format(
                "Invalid ID, PetID %d CharID %d dwClientID %d",
			    pNetMsg->m_PetDbNum,
                pChar->m_CharDbNum,
                dwClientID));
		return FALSE;
	}

	// 거래시 사용될 팻카드 정보이면
	if ( pNetMsg->bTrade )
	{
		GLPetField* pPetInfo = new GLPetField(this);

		// DB작업 실패시 혹은 생성후 메모리 해제해줘라
		AddGameAdoJob(
			db::DbActionPtr(
			    new db::PetGet(
			        pPetInfo,
			        pNetMsg->m_PetDbNum,
			        dwClientID,
			        pChar->m_CharDbNum, 
			        0,
			        0,
			        false,
			        false,
			        true)));
	}
	// 인벤토리,창고에 사용될 팻카드 정보이면
	else
	{
		GLPetField* pPetInfo = new GLPetField(this);

		// DB작업 실패시 혹은 생성후 메모리 해제해줘라
		AddGameAdoJob(
			db::DbActionPtr(
			    new db::PetGet(
			        pPetInfo,
			        pNetMsg->m_PetDbNum,
			        dwClientID,
			        pChar->m_CharDbNum, 
			        0,
			        0,
			        false,
			        true)));
	}
	return TRUE;
}

void GLGaeaServer::ReserveMessage( DWORD dwClientID, DWORD dwGaeaID, CTime time, LPVOID nmg )
{
	NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) nmg;	

	if( !pNmg )
		return;

	GLChar* pChar = GetChar(dwGaeaID);
	if( !pChar )
		return;

	DWORD dwSize = pNmg->Size();

	SReserveMSG reserveMsg;
	reserveMsg.sendTime   = time;
	reserveMsg.dwClientID = dwClientID;
	reserveMsg.dwGaeaID   = dwGaeaID;
	memcpy( reserveMsg.sendMsg, nmg, dwSize );
	m_listReserveMsg.push_back(reserveMsg);
}

void GLGaeaServer::ReserveMessage( DWORD dwClientID, DWORD dwGaeaID, DWORD dwLatterSec, LPVOID nmg )
{
	NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) nmg;	
	if( !pNmg ) return;
	GLChar* pChar = GetChar(dwGaeaID);
	if( !pChar ) return;

	DWORD dwSize = pNmg->Size();

	SReserveMSG reserveMsg;
	CTime curTime = CTime::GetCurrentTime();
	CTimeSpan timeSpan( 0, 0, 0, dwLatterSec );
	reserveMsg.sendTime	  = curTime + timeSpan;
	reserveMsg.dwClientID = dwClientID;
	reserveMsg.dwGaeaID   = dwGaeaID;
	memcpy( reserveMsg.sendMsg, nmg, dwSize );
	m_listReserveMsg.push_back(reserveMsg);
}

void GLGaeaServer::ReserveMessageProcess()
{
	if(m_listReserveMsg.size() == 0)
		return;

	RESERVEMSGLIST_ITER iter = m_listReserveMsg.begin();
	SReserveMSG reserveMsg;
	CTime curTime = CTime::GetCurrentTime();
	
	for(; iter != m_listReserveMsg.end();)
	{
		reserveMsg = *iter;
		if(reserveMsg.sendTime == curTime || reserveMsg.sendTime < curTime)
		{
			GLChar* pChar = GetChar(reserveMsg.dwGaeaID);
			if( pChar && pChar->ClientSlot() == reserveMsg.dwClientID )
			{
				SENDTOCLIENT( reserveMsg.dwClientID, (NET_MSG_GENERIC*) reserveMsg.sendMsg );
			}
			m_listReserveMsg.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}

void GLGaeaServer::ClearReserveDropOutPet ()
{
	if( m_reqDropOutPet.size() == 0 )
		return;

#ifdef _USE_TBB
	ITER_CVECPETDROPOUTINFO iter = m_reqDropOutPet.begin();
#else
	VPETID_ITER iter = m_reqDropOutPet.begin();
#endif

	SDROPOUTPETINFO DropoutPetInfo;

	for (; iter != m_reqDropOutPet.end(); ++iter)
	{
		DropoutPetInfo = *(iter);

		DropOutPET(DropoutPetInfo.dwPetGuid, DropoutPetInfo.bLeaveFieldServer, DropoutPetInfo.bMoveMap);
	}

	m_reqDropOutPet.clear();
}

BOOL GLGaeaServer::ReqQBoxEnable(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if ( IsValidGaeaID(dwGaeaID) == TRUE )
		return FALSE; // 클라이언트에서 이 메세지가 오면 안됨;

	GLMSG::SNET_QBOX_OPTION_REQ_FLD* pNetMsg = (GLMSG::SNET_QBOX_OPTION_REQ_FLD*)nmg;	
	
	const GLPartyID& PartyID = pNetMsg->PartyID;
	GLPartyField* const pParty = GetParty(PartyID);
	if ( pParty == NULL )
		return TRUE;

	GLMSG::SNET_QBOX_OPTION_MEMBER NetMsg(PartyID, pNetMsg->bQBoxEnable);
	pParty->SetQBox(pNetMsg->bQBoxEnable);

	// 자리 이동으로 인해 파티가 바뀐 경우를 대비해 모든 파티 그룹의 QBox 상태를 알고 있어야 함;
	m_pPartyFieldMan->SendMsgToMember(PartyID, &NetMsg);

	return TRUE;
}

float GLGaeaServer::GetExpGainRate( EMCHARINDEX CLASS, DWORD dwLevel )   
{ 
	if( ( dwLevel >= m_ClassEventData.dwExpMinLevel[CLASS] && dwLevel <= m_ClassEventData.dwExpMaxLevel[CLASS] ) || 
		m_ClassEventData.dwExpMinLevel[CLASS] == 0.0f || m_ClassEventData.dwExpMaxLevel[CLASS] == 0.0f )
	{
		return m_ClassEventData.fExpGainRate[CLASS]; 
	}else{
		return 1.0f;
	}
}
float GLGaeaServer::GetItemGainRate( EMCHARINDEX CLASS, DWORD dwLevel )  
{ 
	if( ( dwLevel >= m_ClassEventData.dwItemMinLevel[CLASS] && dwLevel <= m_ClassEventData.dwItemMaxLevel[CLASS] ) || 
		m_ClassEventData.dwItemMinLevel[CLASS] == 0.0f || m_ClassEventData.dwItemMaxLevel[CLASS] == 0.0f )
	{
		return m_ClassEventData.fItemGainRate[CLASS]; 
	}else{
		return 1.0f;
	}
}
float GLGaeaServer::GetMoneyGainRate( EMCHARINDEX CLASS, DWORD dwLevel ) 
{ 
	if ((dwLevel >= m_ClassEventData.dwMoneyMinLevel[CLASS] && dwLevel <= m_ClassEventData.dwMoneyMaxLevel[CLASS] ) || 
		m_ClassEventData.dwMoneyMinLevel[CLASS] == 0.0f || m_ClassEventData.dwMoneyMaxLevel[CLASS] == 0.0f)
	{
		return m_ClassEventData.fMoneyGainRate[CLASS]; 
	}
    else
    {
		return 1.0f;
	}
}

BOOL GLGaeaServer::ReqSearchResultShopItem ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLChar* pMyChar = GetChar(dwGaeaID);
	if (!pMyChar)
        return TRUE;

	if (pMyChar->GetPrivateSearchResultSize() == 0)
        return TRUE;

	GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT NetResultMsg;
	
    GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ* pPacket = (GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT_REQ*) nmg;
    DWORD PageNum = pPacket->dwPageNum;
	int resultNum = PageNum * MAX_SEARCH_RESULT;

	//
	//resultNum이 음수값이 되어서 crash가 발생하는 문제 해결을 위해 조건식 추가
	//
	if (resultNum < 0)
    {
        PageNum = 0;
		resultNum = 0;
    }

	int iMaxCount = (int)(pMyChar->GetPrivateSearchResultSize() - (PageNum * MAX_SEARCH_RESULT));
	if( iMaxCount > MAX_SEARCH_RESULT )
        iMaxCount = MAX_SEARCH_RESULT;

	for (int i = 0; i < iMaxCount; ++i)
	{
		if (resultNum >= (int) pMyChar->GetPrivateSearchResultSize())
            break;
		if (i >= MAX_SEARCH_RESULT)
            break;
		NetResultMsg.sSearchResult[i] = pMyChar->GetPrivateMarketSearchResult(resultNum);
		resultNum++;
	}

	NetResultMsg.dwSearchNum = (DWORD) pMyChar->GetPrivateSearchResultSize();
	NetResultMsg.dwPageNum   = PageNum;

	SENDTOCLIENT ( dwClientID, &NetResultMsg );

	return TRUE;
}

void GLGaeaServer::DelPlayHostileClubBattle(DWORD dwClub_P, DWORD dwClub_S)
{
    GASSERT(IsField());
	std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub(dwClub_P);
	if (!pCLUB)
        return;

    CLUBMEMBERS& Members = pCLUB->GetMembers();
	CLUBMEMBERS_ITER pos = Members.begin();
	CLUBMEMBERS_ITER end = Members.end();	
	GLChar* pCHAR = NULL;
	for ( ; pos!=end; ++pos)
	{
		pCHAR = GetCharByDbNum(pos->first);
		if (pCHAR)
			pCHAR->RemovePlayHostileClub(dwClub_S);
	}
}

void GLGaeaServer::DelPlayHostileAllianceBattle( DWORD dwClub_P, DWORD dwClub_S )
{
	std::tr1::shared_ptr<GLClubField> pCLUB_P = m_pClubMan->GetClub ( dwClub_P );
	if ( !pCLUB_P )
        return;

	std::tr1::shared_ptr<GLClubField> pCLUB_S = m_pClubMan->GetClub ( dwClub_S );
	if ( !pCLUB_S )
        return;

	// 자신 클럽
	CLUB_ALLIANCE_ITER pos = pCLUB_P->m_setAlliance.begin();
	CLUB_ALLIANCE_ITER end = pCLUB_P->m_setAlliance.end();
	for ( ; pos!=end; ++pos )
	{
		const GLCLUBALLIANCE &sALLIANCE_A = pos->second;
		std::tr1::shared_ptr<GLClubField> pCLUB_A = m_pClubMan->GetClub ( sALLIANCE_A.m_dwID );
		if ( !pCLUB_A )
            continue;	

		CLUB_ALLIANCE_ITER posTar = pCLUB_S->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER endTar = pCLUB_S->m_setAlliance.end();
		for ( ; posTar!=endTar; ++posTar )
		{
			const GLCLUBALLIANCE &sALLIANCE_B = pos->second;
			std::tr1::shared_ptr<GLClubField> pCLUB_B = m_pClubMan->GetClub ( sALLIANCE_B.m_dwID );
			if ( !pCLUB_B )
                continue;	
			
			DelPlayHostileClubBattle ( pCLUB_A->m_DbNum, pCLUB_B->m_DbNum );
		}
	}	
}

BOOL GLGaeaServer::SaveNpcCommission( DWORD dwCharID, DWORD dwUserID, LONGLONG lnCommission )
{
	GASSERT(IsField());

	if ( lnCommission <= 0 )
		return FALSE;

	GLChar* pChar = GetCharByDbNum( dwCharID );

	//  캐릭터가 같은 필드에 있을경우 
	if ( pChar ) 
	{
		pChar->UpdateNpcCommission( lnCommission );
	}
	else
	{
		GLMSG::SNET_INVEN_NPC_COMMISSION NetMsgAgt;	
		NetMsgAgt.nCHANNEL = GetServerChannel();
		NetMsgAgt.dwFieldID = GetFieldSvrID();
		NetMsgAgt.dwCharID = dwCharID;
		NetMsgAgt.dwUserID = dwUserID;
		NetMsgAgt.lnCommission = lnCommission;
		SENDTOAGENT( &NetMsgAgt );
	}

	return TRUE;
}

BOOL GLGaeaServer::SaveNpcCommissionDB( DWORD dwCharID, DWORD dwUserID, LONGLONG lnCommission )
{
	if ( lnCommission <= 0 )
        return FALSE;

	GLChar* pChar = GetCharByDbNum( dwCharID );
	if ( !pChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on SaveNpcCommissionDB ", dwCharID));
	}

	// 통계 및 로그 수정
	m_pItemLimit->LogMoneyExchange ( gamelog::ID_CHAR, dwCharID, gamelog::ID_USER, dwUserID, lnCommission, ITEM_ROUTE_NPCCOME, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_NPC_COMMISSION );

	AddGameDbJob(
        db::DbActionPtr(
            new db::UserMoneyAddUpdate(dwUserID, lnCommission)));	

//	UserLastInfo 업데이트
//#if defined ( TW_PARAM ) || defined ( HK_PARAM ) // || defined(_RELEASED)	
	if (m_ServiceProvider == SP_HONGKONG)
	{	
		AddGameDbJob(
			db::DbActionPtr(
				new db::UserLastInfoAddUpdate(dwUserID, lnCommission)));
//#endif
	}
	return TRUE;
}

BOOL GLGaeaServer::ReqClubDeathMatchRanking ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_CLUB_DEATHMATCH_RANKING_REQ* pNetMsg = (GLMSG::SNET_CLUB_DEATHMATCH_RANKING_REQ*)nmg;
	// 요청 케릭터가 유효성 체크
	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;	

	GLLandMan* const pLand(pChar->GetLandMan());
	if ( pLand == NULL )
		return FALSE;
	
	if ( pChar->m_ClubDbNum == CLUB_NULL )
		return FALSE;

	if ( pLand->GetBaseMapID().Id() != pNetMsg->dwMapID )
		return FALSE;

	if ( !pLand->m_bClubDeathMatchMap )
		return FALSE;

	GLClubDeathMatch* pCDM = m_pDeathMatchMan->Find( pLand->m_dwClubMapID );
	if (!pCDM || !pCDM->IsBattle())
        return FALSE;	
	
	CDM_RANK_INFO_MAP_ITER pos = pCDM->m_mapCdmScore.find( pChar->m_ClubDbNum );
	if( pos != pCDM->m_mapCdmScore.end() ) 
	{
		GLMSG::SNET_CLUB_DEATHMATCH_MYRANK_UPDATE NetMsgMy;

		NetMsgMy.sMyCdmRank.wClubRanking = pos->second.wClubRanking;
		NetMsgMy.sMyCdmRank.wKillNum = pos->second.wKillNum;
		NetMsgMy.sMyCdmRank.wDeathNum = pos->second.wDeathNum;
		
		SENDTOCLIENT( dwClientID, &NetMsgMy );
	}


	GLMSG::SNET_CLUB_DEATHMATCH_RANKING_UPDATE NetMsg;
	
	CDM_RANK_INFO_MAP_ITER pos_begin = pCDM->m_mapCdmScore.begin();
	CDM_RANK_INFO_MAP_ITER pos_end = pCDM->m_mapCdmScore.end();

	for ( ; pos_begin != pos_end; pos_begin++ )
	{
		SCDM_RANK_INFO& sCdmRankInfo = pos_begin->second;
		int nIndex = sCdmRankInfo.nIndex;

		if ( nIndex >= 0 && nIndex < RANKING_NUM )
		{
			SCDM_RANK sCdmRank = sCdmRankInfo;
			NetMsg.ADDCLUB( sCdmRank );
		}
	}	

	if ( NetMsg.wRankNum > 0 )
		SENDTOCLIENT( dwClientID, &NetMsg );

	return TRUE;
}

BOOL GLGaeaServer::IsValidTime(CTime &ctValue)
{
	return ((ctValue.GetTime() >= m_ctLowerLimit.GetTime()) && (ctValue.GetTime() < m_ctUpperLimit.GetTime()));		
}

//! 캐릭터 Action Log
// 통계 및 로그 수정
void GLGaeaServer::LogCharAction(
    DWORD dwCI, gamelog::EMACTION emACT, 
    gamelog::EMIDTYPE emTAR, DWORD dwTAR, 
    __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY, const MapID &sMapID)
{
    m_pItemLimit->LogCharAction(dwCI, emACT, emTAR, dwTAR, nEXP, nBRIGHT, nLIFE, nMONEY, sMapID);
}

void GLGaeaServer::TxtLogToDb()
{
    if (m_pItemLimit)
        m_pItemLimit->TxtLogToDB();
}

DWORD GLGaeaServer::CalcDamage(
    GLChar* pChar,
    int& rResultDAMAGE,
    const DWORD dwGaeaID,
    const STARGETID &cTargetID,
    const GLLandMan* pLandMan,
    const GLSKILL* pSkill,
    const DWORD dwskill_lev,
    const DWORD dwWeatherFlag,
    const DWORD dwDivCount)
{
    if (!pChar)
        return DAMAGE_TYPE_NONE;
    else
        return m_pDamageCalculator->CalcDamage(
            pChar,
            rResultDAMAGE,
            dwGaeaID,
            cTargetID,
            pLandMan,
            pSkill,
            dwskill_lev,
            dwWeatherFlag,
            dwDivCount);
}

//! 공격력 변화율 반영
void GLGaeaServer::AdjuctDamage(
    EMCROW CrowType,
	int& nDamageLow,
	int& nDamageHigh,
    float DamageRate,
    float OwnerDamagePer)
{
    m_pDamageCalculator->AdjuctDamage(CrowType,  nDamageLow, nDamageHigh, DamageRate, OwnerDamagePer);
}

void GLGaeaServer::AdjustLevel(int& ndxLvl, int& nExtFORCE, int TarLevel, WORD MyLevel)
{
    m_pDamageCalculator->AdjustLevel(ndxLvl, nExtFORCE, TarLevel, MyLevel);
}

int GLGaeaServer::CalcCriticalDamage(bool bCritical, bool bCrushingBlow, int Damage)
{
    return m_pDamageCalculator->CalcCriticalDamage(bCritical, bCrushingBlow, Damage);
}

//! Critical 발생 여부 판단
bool GLGaeaServer::CalcCritical(EMCROW CrowType, DWORD NowHp, DWORD MaxHp, int ndxLvl, int IncCritical)
{
    return m_pDamageCalculator->CalcCritical(CrowType, NowHp, MaxHp, ndxLvl, IncCritical);
}

//! Damage 최대 최소값 사이의 랜덤 Damage 결정
int GLGaeaServer::CalcRandomDamage(const int& nDamageLow, const int& nDamageHigh, int ExtForce)
{
    return m_pDamageCalculator->CalcRandomDamage(nDamageLow, nDamageHigh, ExtForce);
}

//! 최저 수용 Damage 산출
int GLGaeaServer::CalcLowDamage(EMCROW CrowType, int DamageOld, int Defense, float StateDamage)
{
    return m_pDamageCalculator->CalcLowDamage(CrowType, DamageOld, Defense, StateDamage);
}

//! 방어구 Damage 흡수율 반영
int GLGaeaServer::CalcDefenseRate(EMCROW CrowType, int DefaultDefense, int ItemDefense, int Damage, WORD Level)
{
    return m_pDamageCalculator->CalcDefenseRate(CrowType, DefaultDefense, ItemDefense, Damage, Level);
}

SMAPNODE* GLGaeaServer::FindMapNode(const MapID& mapID)
{
	return FindMapNode(mapID.getBaseMapID());
}
SMAPNODE* GLGaeaServer::FindMapNode(const SNATIVEID& sNativeID)
{
    return m_pMapList->FindMapNode(sNativeID);
}

const SLEVEL_REQUIRE* GLGaeaServer::GetLevelRequire(const SNATIVEID& sNativeID)
{
    return m_pMapList->GetLevelRequire(sNativeID);
}

const char* GLGaeaServer::GetMapName(const SNATIVEID& sNativeID)
{
    return m_pMapList->GetMapName(sNativeID);
}

GLMapList::FIELDMAP& GLGaeaServer::GetMapList()
{
    return m_pMapList->GetMapList();
}

HRESULT GLGaeaServer::LoadMapList(
    BOOL bTool,
    LPF_SERVER_INFO pServerInfo,
    int nServerNum)
{
    return m_pMapList->LoadMapList(bTool, pServerInfo, nServerNum, m_bEmulator);
}

void GLGaeaServer::ConvertWorldToMap(const MapID& mapID, const D3DXVECTOR3& vPos, int& nPosX, int& nPosY)
{
    m_pMapList->ConvertWorldToMap(mapID.getBaseMapID(), vPos, nPosX, nPosY);
}

void GLGaeaServer::ConvertMapToWorld(const MapID& mapID, int PosX, int PosY, D3DXVECTOR3& vPos)
{
    m_pMapList->ConvertMapToWorld(mapID.getBaseMapID(), PosX, PosY, vPos);
}

bool GLGaeaServer::MoveMap( GLChar* pChar, const MapID& mapID, DWORD dwGate, const D3DXVECTOR3& vPos )
{
	/*if( NULL == pChar )
		return false;

	const SMAPNODE* pMapNode = m_pMapList->FindMapNode( mapID.getBaseMapID() );
	if( NULL == pMapNode )
		return false;

	GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;

	// PET
	// 맵이동시 Pet 삭제
	DropOutPET( pChar->m_dwPetGUID, true, true );

	// VEHICLE
	SaveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), true );

	//SUMMON
	DropOutAllSummon( pChar );

	// 매크로가 켜져있다면 맵이동시 끈다;
	if ( pChar->IsActState(EM_ACT_MACROMODE) )
	{
		pChar->MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SENDTOCLIENT ( pChar->ClientSlot(), &msg );
	}

	if( pMapNode->GetFieldServerNum() != m_dwFieldSvrID )
	{
		// 다른 필드 서버일 경우
		GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.dwChaNum = pChar->CharDbNum();
		NetMsgAg.dwFieldServerIDTo = pMapNode->GetFieldServerNum();
		NetMsgAg.mapIDTo = mapID;
		NetMsgAg.dwGATEID = dwGate;
		NetMsgAg.vPosition = vPos;
		SENDTOAGENT( &NetMsgAg );
	}    
	else
	{
		// 같은 필드 서버일 경우;
        // 주의!: RequestInvenRecallThisSvr 함수 호출 후에는 케릭터의 현재 mapID가 이동하는 mapID로 변경되어버림;
        // 그래서 현재 맵 정보는 pChar->GetCurrentMapID() 를 사용하면 안된다. const MapID sCurMapID(pChar->GetCurrentMap()); 를 사용하여야 한다.;
        const MapID sCurMapID(pChar->GetCurrentMap());

		BOOL bOK = RequestInvenRecallThisSvr( pChar, mapID, dwGate, vPos );
		if( FALSE == bOK )
			return false;

		// 버프와 상태이상 제거
		if( sCurMapID != mapID )
		{
			for( int i = 0; i < EMBLOW_MULTI; ++i )
				pChar->DISABLEBLOW( i );
			for( int i = SKILLFACT_INDEX_NORMAL_START; i < SKILLFACT_INDEX_NORMAL_END; ++i )
				pChar->RESETSKEFF( i );

			// CP 초기화 맵인지 확인
			if( pMapNode->IsCombatPointReset() )
				pChar->ResetCP();
		}

		pChar->ResetAction();
		GLLandMan* const pLand = pChar->GetLandMan();
		if ( !pLand )
			return false;

		// 이동 성공을 알림
		NetMsgFB.dwChaNum = pChar->CharDbNum();
		NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
		NetMsgFB.sMAPID = mapID;
		NetMsgFB.vPOS = pChar->GetPosition();
		SENDTOAGENT( &NetMsgFB );
	}*/

	return true;
}

//
// GetCharByDbNum or GetCharByUniqueID에 대한 자동 분기 함수
//
GLChar* GLGaeaServer::GetChar64(DWORD dwChaNum, DWORD dwSvrGroupNum)
{
	if(GetServerType() == net::SERVER_INSTANCE)
	{
		GASSERT(dwSvrGroupNum != MAXDWORD);
		GASSERT(dwChaNum != MAXDWORD);

		return GetCharByUniqueID(UNIQUEID(dwSvrGroupNum, dwChaNum));
	}
	else
	{
		GASSERT(dwChaNum != MAXDWORD);

		return GetCharByDbNum(dwChaNum);
	}
}

std::tr1::shared_ptr<GLMapAxisInfo> GLGaeaServer::GetAxisInfo(const std::string& FileName)
{
    return m_pMapList->GetAxisInfo(FileName);
}

GLChar* GLGaeaServer::GetCharByDbNum(DWORD ChaDbNum)
{
	GASSERT(GetServerType() != net::SERVER_INSTANCE);

	DWORD GaeaId = m_pCharMan->GetGaeaId(ChaDbNum);

    if (GaeaId != GAEAID_NULL)
        return GetChar(GaeaId);
    else
        return NULL;
}

GLChar* GLGaeaServer::GetCharByUniqueID(UNIQUEID &dw64UniqueID)
{
	GASSERT(GetServerType() == net::SERVER_INSTANCE);

	DWORD GaeaId = m_pCharMan->GetGaeaId(dw64UniqueID.dwSvrGroup, dw64UniqueID.dwChaNum);

    if (GaeaId != GAEAID_NULL)
        return GetChar(GaeaId);
    else
        return NULL;
}


const std::map<DWORD, DWORD>& GLGaeaServer::GetCharIDMap()
{ 
    return m_pCharMan->GetCharIDMap();
}

const std::map<DWORD64, DWORD>& GLGaeaServer::GetCharIDMap64()
{ 
    return m_pCharMan->GetCharIDMap64(); 
}


BOOL GLGaeaServer::GmDbUnlockUserAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwClientID != m_dwAgentSlot)
        return FALSE;
    GLMSG::NET_GM_DB_UNLOCK_USER_AF* pPacket = (GLMSG::NET_GM_DB_UNLOCK_USER_AF*) nmg;
    ResetCreatingCharUserID(pPacket->m_UserDbNum);
    ResetSaveDBUserID(pPacket->m_UserDbNum);
    sc::writeInfo(
        sc::string::format(
            "GmDbUnlockUserAF User Db Num %1%", pPacket->m_UserDbNum));
    return TRUE;
}

void GLGaeaServer::ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType)
{
    m_pCharMan->ChaNameDbNumAdd(ChaName, ChaDbNum, UserDbNum, UserType);
}

void GLGaeaServer::ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType, int iClass, int iLv)
{
	m_pCharMan->ChaNameDbNumAdd(ChaName, ChaDbNum, UserDbNum, UserType, iClass, iLv);
}

DWORD GLGaeaServer::GetChaDbNumByChaName(const std::string& ChaName)
{
    return m_pCharMan->GetChaDbNumByChaName(ChaName);
}

std::string GLGaeaServer::GetChaNameByDbNum(DWORD ChaDbNum)
{
    return m_pCharMan->GetChaNameByDbNum(ChaDbNum);
}

std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> GLGaeaServer::GetCharCacheData(const std::string& ChaName)
{
    return m_pCharMan->GetCharCacheData(ChaName);
}

std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> GLGaeaServer::GetCharCacheData(DWORD ChaDbNum)
{
    return m_pCharMan->GetCharCacheData(ChaDbNum);
}

std::string GLGaeaServer::GetClientIP( DWORD dwClientID )
{
    GASSERT(dwClientID < m_dwMaxClient);

    if (m_pMsgServer)
        return m_pMsgServer->GetClientIP( dwClientID );

    return "0.0.0.0";
}

void GLGaeaServer::Online(const DWORD dwGaeaID)
{
}
void GLGaeaServer::Offline(const DWORD dwGaeaID)
{
	m_pMiniGameManager->endMiniGame(dwGaeaID);	
}

void GLGaeaServer::ReportFieldServerStateToAgent(const int _nUsedMemory)
{
	GLMSG::NET_FIELD_STATE_MEMORY fieldStateMessage(m_dwFieldSvrID, m_nServerChannel);
	
	fieldStateMessage.nUsedMemory = _nUsedMemory;

	SENDTOAGENT(&fieldStateMessage);
}

void GLGaeaServer::ReportUsedMemoryPerLand(void)
{
	GLMSG::NET_FIELD_STATE_MEMORY_PER_LAND _stateMemoryMessage;
	for ( DWORD _nMainID(MAX_LAND_MID); _nMainID--; )
	{
		GLLandManager* const pLandManager(GetLandManager(_nMainID));
		if ( pLandManager == NULL )
			continue;

		const unsigned int _sizeStage(pLandManager->getNStage());
		for ( unsigned int _index(_sizeStage); _index--; )
		{
			GLLandMan* const _pLand(pLandManager->getLand(_index));
			if ( _pLand == NULL )
				continue;
			
			const DWORD _nUsedMemory(_pLand->GetUsedMemory());
			if ( _stateMemoryMessage.addLand(_pLand->GetBaseMapID().Id(), _nUsedMemory) == false )
			{
				SENDTOAGENT(&_stateMemoryMessage);
				_stateMemoryMessage.reset();
			}
		}
	}	

	if ( _stateMemoryMessage.isValid() == true )
		SENDTOAGENT(&_stateMemoryMessage);
}

void GLGaeaServer::ReserveCTFRebirth( DWORD dwChaDbNum, const SNATIVEID& _moveMapID, const DWORD _dwGateID, const float _fRestore )
{
	CTF_REBIRTH CTFRebirth( dwChaDbNum, _moveMapID, _dwGateID, _fRestore );
	m_CTFRebirthReserved.push( CTFRebirth );
}

void GLGaeaServer::ProcessCTFRebirth()
{
	CTF_REBIRTH CTFRebirth;
#ifdef _USE_TBB
	while ( m_CTFRebirthReserved.try_pop( CTFRebirth ) )
	{
#else
	while ( m_CTFRebirthReserved.empty() == false )
	{
		CTFRebirth = m_CTFRebirthReserved.front();
		m_CTFRebirthReserved.pop();
#endif
		GLChar* pCha = NULL;
		pCha = GetCharByDbNum( CTFRebirth.dwChaDbNum );
		if ( NULL == pCha )
		{
			continue;
		}

		pCha->ResurrectMoveMap( CTFRebirth.moveMapID, CTFRebirth.dwGateID, CTFRebirth.fRestore );
	}
}

void GLGaeaServer::SaveCharacterInSqlite( SCHARDATA2* pChadata, bool bSendAgent /*= true*/ )
{
	if ( NULL == m_pCharacterBackup )
	{
		return;
	}

	if ( !m_pCharacterBackup->SaveCharacterInSqlite( pChadata ) )
	{
		if ( bSendAgent )
		{
			NET_BLOCK_USER msg;
			msg.dwUserNum = pChadata->GetUserID();
			msg.bBlock = true;
			SENDTOAGENT( &msg );
		}
	}
}

void GLGaeaServer::UserLockerLock( DWORD UserNum )
{
	if ( m_pUserLockerLockMgr )
	{
		m_pUserLockerLockMgr->Lock( UserNum );
	}
}

void GLGaeaServer::UserLockerUnLock( DWORD UserNum )
{
	if ( m_pUserLockerLockMgr )
	{
		m_pUserLockerLockMgr->UnLock( UserNum );
	}
}

void GLGaeaServer::UnblockForEmergencySavedUser()
{
	if ( m_pCharacterBackup )
	{
		std::vector< DWORD >& vecUserNum = m_pCharacterBackup->GetSavedUserNum();
		size_t nSize = vecUserNum.size();
		for ( size_t loop = 0; loop < nSize; loop++ )
		{
			NET_BLOCK_USER msg;
			msg.dwUserNum = vecUserNum[loop];
			msg.bBlock = false;
			SENDTOAGENT( &msg );
		}
	}
}

BOOL GLGaeaServer::SaveCharacterIntoSqlite( GLChar* pChar )
{
	// SaveCharacterIntoDB 로 통합함 20140925
	// sqlite 에 저장한다. 서버가 다시 켜지면서 실제 디비에 저장을 한다.
	// 현재는 캐릭터 기본 정보만 저장을 하는데, 기여도, 특별활동, 펫, 탈 것도 추가 예정
	if ( NULL == pChar )
	{
		return FALSE;
	}

	// mjeon.CTF
	pChar->SaveExtraInfo();

	//
	//mjeon.activity
	//
	pChar->ActivitySave();

	//
	//mjeon.attendance
	//
	pChar->AttendanceSave();

	// PET
	// 맵이동시 Pet 삭제
	DropOutPET( pChar->m_dwPetGUID, true, true );

	// VEHICLE
	SaveVehicle( pChar->ClientSlot(), pChar, true );

	// SUMMON
	DropOutAllSummon( pChar );

	const MapID _mapIDBackup(pChar->GetCurrentMap());
	const D3DXVECTOR3 _positionBackup(pChar->GetPosition());
	// 구인던일 경우 인던 밖으로 이동한다.
	const SMAPNODE* const pMapNode(FindMapNode(_mapIDBackup));
	const bool _bOldInstantMap((pMapNode != NULL) && (pMapNode->GetInstanceType() == SMAPNODE::EMINSTANCE_OLD_INSTANCE));
	if ( _bOldInstantMap == true )
		SaveCheckInstantMap( pChar );

	pChar->SavePosition();

	// 사망한 상태로 제거시
	if ( pChar->IsActState( EM_ACT_DIE ) )
	{
		// 부활시 경험치 감소
		pChar->DoDeathPenaltyExp();

		//현재 위치를 마지막 귀환 위치로 저장
		pChar->SaveLastCall();
	}

	if ( pChar->IsActState(EM_ACT_MACROMODE) )
	{
		pChar->MacroOnOff(false);
	}

	// 현재 음식버프 저장.
	pChar->SaveSkillFact();

	SaveCharacterInSqlite( pChar, true );

	if ( _bOldInstantMap == true )
	{
		pChar->SetCurrentMap(_mapIDBackup);
		pChar->SetPosition(_positionBackup);
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgReloadAIFSMProc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	AISystem::GLFSMContainer::Instance()->Reload();

	return true;
}

void GLGaeaServer::DisconnectAllCharacter()
{
	UMAP_GLCHAR_CITER iter = m_mapCharacters.begin();
	for ( ; iter != m_mapCharacters.end(); ++iter )
	{
		NET_DISCONNECT_CHARACTER msg;
		GLChar* pChar = iter->second;
		if ( pChar )
		{
			msg.ChaDbNum = pChar->CharDbNum();
			SENDTOAGENT( &msg );
		}
	}
}

BOOL GLGaeaServer::SendServerStopMsg( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_REQ_SERVERSTOP_MSG* msg = ( GLMSG::SNET_REQ_SERVERSTOP_MSG* )nmg;
	if ( msg->dwCode != GLMSG::SNET_REQ_SERVERSTOP_MSG::CODE_MAGIC_NUMBER )
	{
		sc::writeLogError( sc::string::format( "GLGaeaServer::SendServerStopMsg() - MagicNumber unmatched.(%1%/%2%)", msg->dwCode, GLMSG::SNET_REQ_SERVERSTOP_MSG::CODE_MAGIC_NUMBER ) );
		return FALSE;
	}

	GLMSG::SNET_REQ_SERVERSTOP_FB NetMsgFB;
	NetMsgFB.dwError = msg->dwError;

	// http://crm.mincoms.com/Require/RequireView/1639
	// 대만은 SERVER_STOP_OUTOFMEMORY, SERVER_STOP_JUST_RESTART 에러일 경우 서버재시작 메시지 출력하지 않도록 함
	if ( SP_HONGKONG == m_ServiceProvider )
	{
		if ( SERVER_STOP_NOMAL == NetMsgFB.dwError )
		{
			// 클라이언트에게 서버테스트를 위한 멈춤을 알림
			SENDTOALLCLIENT( &NetMsgFB );
		}
	}
	else
	{
		// 클라이언트에게 서버테스트를 위한 멈춤을 알림
		SENDTOALLCLIENT( &NetMsgFB );
	}
	return TRUE;
}

void GLGaeaServer::InsertErrorItem( sc::MGUID& guid, int CallType )
{
	// 디비 액션 만들어서 호출하자.
	__time64_t Date = CTime::GetCurrentTime().GetTime();

	AddGameAdoJob(
		db::DbActionPtr( 
		new db::ItemInsertError( guid, CallType, Date ) ) );
}

void GLGaeaServer::UpdateItemInvenType( DWORD Owner, const SITEMCUSTOM& sItemCustom, INVEN_TYPE InvenType, WORD PosX, WORD PosY, DWORD Channel /*= 0*/ )
{
	// db action 제작 필요
	// guid, 변경될 인벤타입, 좌표 필요하다.
	// sItemCustom 의 인벤타입enum INVEN_TYPE 참조
	// 아이템 세부 정보 변경없이, 단지 위치만 변경되는 경우에 사용된다. 예를 들어 창고에 있던 아이템이 인벤토리로 이동한 경우
	// 장착창에서 인벤토리로 이동한 경우, 인벤토리에서 인벤토리로 이동한 경우등
	// 디비에서 저장 위치와, 좌표값만 변경한다. 아이템 세부항목 전체 업데이트 비용을 줄이기 위한 용도니다.
	// 변경되는 저장위치가 창고라면 채널이랑 좌표값 처리가 필요하다.
	// Target 뭘로 해야하는지 다시 정리해야 한다. 지금 헤깔림
	// 인벤이나 이런건 캐릭터번호, 유저락커는 유저번호이다.

	AddGameAdoJob(
		db::DbActionPtr( 
		new db::ItemUpdatePos( Owner, InvenType, sItemCustom.GetGuid(), PosX, PosY, 9079, Channel ) ) );
}

void GLGaeaServer::OnChangeMoney ( EMGAME_MONEY_CHANGE _emMoneyChange, LONGLONG _llChangeMoney )
{
	if ( m_pMoneyMan )
		m_pMoneyMan->OnChangeMoney( _emMoneyChange, _llChangeMoney );
}

void GLGaeaServer::OnChangeMapMoney ( EMGAME_MONEY_CHANGE _emChangeType, int _nMapID, LONGLONG _llChangeMoney )
{
	if ( m_pMoneyMan )
		m_pMoneyMan->OnChangeMapMoney( _emChangeType, _nMapID, _llChangeMoney );
}

void GLGaeaServer::PushJackpotItem( JACKPOT_ITEM& JackpotItem )
{
	m_JackpotItem.push( JackpotItem );
}

bool GLGaeaServer::PopJackpotItem( JACKPOT_ITEM& JackpotItem )
{
	return m_JackpotItem.try_pop( JackpotItem );
}

BOOL GLGaeaServer::MsgJackpotPush( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwClientID != m_dwAgentSlot )
		return FALSE;

	GLMSG::NET_JACKPOT_PUSH* pNetMsg = ( GLMSG::NET_JACKPOT_PUSH* ) nmg;

	JACKPOT_ITEM JackpotItem;
	JackpotItem.ItemID = pNetMsg->ItemID;
	JackpotItem.RequiredLevel = pNetMsg->RequiredLevel;
	JackpotItem.GameMoney = pNetMsg->GameMoney;
	JackpotItem.bMain = pNetMsg->bMain;

	PushJackpotItem( JackpotItem );

	return TRUE;
}

//! 확률/아이템 리스트 제어 기능 : sckim Redmine #4544
BOOL GLGaeaServer::RandomChanceLoadScript() const
{
	//Load Script
	//
	std::string strScriptPath(sc::getAppPath());
	strScriptPath.append(m_pPath->GLogicServerPath()); //SUBPATH::GLogicServerPath());
	strScriptPath.append("scripts\\");

    std::string strRCScriptPath( strScriptPath );
    strRCScriptPath.append( GLogicData::GetInstance().GetRandomChanceFileName() );

    m_pRandomChanceCtrl->Reset();

    return m_pRandomChanceCtrl->LoadFile( strRCScriptPath.c_str() );
}

BOOL GLGaeaServer::ResetChaSkill( GLChar* pChar )
{
	if ( NULL == pChar )
	{
		return FALSE;
	}

	WORD wGRADE[EMGRADE];
	memset( wGRADE, 0, sizeof( WORD ) * EMGRADE );

	SKILL_MAP_ITER pos = pChar->m_ExpSkills.begin();
	SKILL_MAP_ITER end = pChar->m_ExpSkills.end();
	for ( ; pos != end; ++pos )
	{
		SNATIVEID sNID( ( *pos ).first );

		PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( sNID );
		if ( !pSKILL )									continue;
		if ( EMGRADE_MIN > pSKILL->m_sBASIC.dwGRADE )	continue;

		wGRADE[pSKILL->m_sBASIC.dwGRADE] += 1;
	}

	for ( int i = EMGRADE_MIN; i < EMGRADE; ++i )
	{
		if ( !wGRADE[i] )
		{
			continue;
		}

		SNATIVEID ItemID( EMMARBLE_MID, i );

		const SITEM* pITEMSKILL = GLogicData::GetInstance().GetItem( ItemID );
		if ( !pITEMSKILL )
		{
			continue;
		}

		SITEMCUSTOM sITEM_NEW( ItemID );
		CTime cTIME = CTime::GetCurrentTime();
		sITEM_NEW.tBORNTIME = cTIME.GetTime();
		sITEM_NEW.wTurnNum = wGRADE[i];
		sITEM_NEW.cGenType = EMGEN_INIT;
		sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
		sITEM_NEW.GenerateBasicStat( false );
		sITEM_NEW.GenerateLinkSkill();
		sITEM_NEW.GenerateAddOption();

		BOOL PostSendResult = PostSendSystemMail( 
			pChar->CharDbNum(), 
			std::string( pChar->GetName() ), 
			sITEM_NEW, 
			0, 
			ATTENDANCE_REWARD_POST_DESIGN, 
			ATTENDANCE_REWARD_POST_SENDER, 
			std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME", 0 ) ),
			std::string( ID2SERVERTEXT( "TEXT_SKILLREST_TITLE", 0 ) ),
			std::string( ID2SERVERTEXT( "TEXT_SKILLREST_CONTENTS", 0 ) ) );

		if ( FALSE == PostSendResult )
		{
			sc::writeLogError( sc::string::format( 
				"GLGaeaServer::ResetChaSkill() - postsend fail chanum: %d item(mid/sid/turn): %d/%d/%d", 
				pChar->CharDbNum(), ItemID.wMainID, ItemID.wSubID, sITEM_NEW.wTurnNum ) );
		}
		else
		{
			if ( m_pItemLimit )
			{
				m_pItemLimit->LogItemExchange( 
					sITEM_NEW, 
					gamelog::ID_CHAR, 0, 
					gamelog::ID_CHAR, pChar->CharDbNum(),
					ITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum, 
					MapID( SNATIVEID( false ), SNATIVEID( false ) ), LOG_ITEM_ADD_ITEM_BY_SKILL_STAT_RESET,
					false );
			}
		}
	}

	pChar->RESET_STATS_SKILL( NULL );

	return TRUE;
}

BOOL GLGaeaServer::ResetChaSkillNew( GLChar* pChar )
{
	if ( NULL == pChar )
	{
		return FALSE;
	}

	WORD wGRADE[EMGRADE];
	memset( wGRADE, 0, sizeof( WORD ) * EMGRADE );

	SKILL_MAP_ITER pos = pChar->m_ExpSkills.begin();
	SKILL_MAP_ITER end = pChar->m_ExpSkills.end();
	for ( ; pos != end; ++pos )
	{
		SNATIVEID sNID( ( *pos ).first );

		DWORD SkillGrade = GLSkillResetDefine::Instance()->GetSkillGrade( sNID );
		if ( SkillGrade == SNATIVEID::ID_NULL )
		{
			continue;
		}

		if ( SkillGrade >= EMGRADE )
		{
			continue;
		}
		
		if ( EMGRADE_MIN > SkillGrade )
		{
			continue;
		}

		wGRADE[SkillGrade] += 1;
	}

	for ( int i = EMGRADE_MIN; i < EMGRADE; ++i )
	{
		if ( !wGRADE[i] )
		{
			continue;
		}

		SNATIVEID ItemID( EMMARBLE_MID, i );

		const SITEM* pITEMSKILL = GLogicData::GetInstance().GetItem( ItemID );
		if ( !pITEMSKILL )
		{
			sc::writeLogError( sc::string::format( 
				"GLGaeaServer::ResetChaSkill() - pITEMSKILL is null chanum: %d item(mid/sid): %d/%d", 
				pChar->CharDbNum(), ItemID.wMainID, ItemID.wSubID ) );
			continue;
		}

		SITEMCUSTOM sITEM_NEW( ItemID );
		CTime cTIME = CTime::GetCurrentTime();
		sITEM_NEW.tBORNTIME = cTIME.GetTime();
		sITEM_NEW.wTurnNum = wGRADE[i];
		sITEM_NEW.cGenType = EMGEN_INIT;
		sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );
		sITEM_NEW.GenerateBasicStat( false );
		sITEM_NEW.GenerateLinkSkill();
		sITEM_NEW.GenerateAddOption();

		BOOL PostSendResult = PostSendSystemMail( 
			pChar->CharDbNum(), 
			std::string( pChar->GetName() ), 
			sITEM_NEW, 
			0, 
			ATTENDANCE_REWARD_POST_DESIGN, 
			ATTENDANCE_REWARD_POST_SENDER, 
			std::string( ID2SERVERTEXT( "SYSTEM_POST_SENDER_NAME", 0 ) ),
			std::string( ID2SERVERTEXT( "TEXT_SKILLREST_TITLE", 0 ) ),
			std::string( ID2SERVERTEXT( "TEXT_SKILLREST_CONTENTS", 0 ) ) );

		if ( FALSE == PostSendResult )
		{
			sc::writeLogError( sc::string::format( 
				"GLGaeaServer::ResetChaSkill() - postsend fail chanum: %d item(mid/sid/turn): %d/%d/%d", 
				pChar->CharDbNum(), ItemID.wMainID, ItemID.wSubID, sITEM_NEW.wTurnNum ) );
		}
		else
		{
			if ( m_pItemLimit )
			{
				m_pItemLimit->LogItemExchange( 
					sITEM_NEW, 
					gamelog::ID_CHAR, 0, 
					gamelog::ID_CHAR, pChar->CharDbNum(),
					ITEM_ROUTE_CHAR, sITEM_NEW.wTurnNum, 
					MapID( SNATIVEID( false ), SNATIVEID( false ) ), LOG_ITEM_ADD_ITEM_BY_SKILL_STAT_RESET,
					false );
			}
		}
	}

	pChar->RESET_STATS_SKILL( NULL );

	return TRUE;
}

BOOL GLGaeaServer::MsgReqResetSkillStatsBySkillUI( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
		return FALSE;
	
	pChar->MsgReqResetSkillStatsBySkillUI( nmg );

	return TRUE;
}

