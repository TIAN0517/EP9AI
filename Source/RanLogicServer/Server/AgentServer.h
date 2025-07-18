#ifndef _LS_AGENT_SERVER_H_
#define _LS_AGENT_SERVER_H_

#pragma once

#include "../../SigmaCore/Json/MinJson.h"
#include "../../SigmaCore/Util/minTea.h"
#include "../../EngineLib/G-Logic/DxMsgServer.h"

#include "../../RanLogic/Util/s_CMinMd5.h"
#include "../../RanLogic/Util/s_Util.h"
#include "../../RanLogic/Util/ApexCloseList.h"
#include "../../RanLogic/Util/s_CWhiteRock.h"
#include "../../RanLogic/Util/ApexProxyLib.h" // 신규 APEX
#include "../../RanLogic/Util/CPLManager.h"

#include "../Country/GLCountryManServer.h"

#include "../DxServerInstance.h"
#include "../Character/GLChar.h"
#include "../AgentServer/GLAgentServer.h"
#include "../Database/DBAction/DbExecuter.h"
#include "./AgentServerApex.h"
#include "./NetServer.h"

#include <stdio.h>

#include "../../=cURL/include/curl/curl.h"

#ifndef ERROR_MSG_LENGTH 
#define ERROR_MSG_LENGTH 256
#endif

/*
#if defined ( PH_PARAM ) || defined ( VN_PARAM )
	#import "../../=DLL/EGameEncrypt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#else
	#import "../../=DLL/MyRossoEncrypt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#endif
*/

//#import "../../=DLL/EGameEncrypt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids
//#import "../../=DLL/MyRossoEncrypt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids

#include "EGameEncrypt.tlh"
#include "MyRossoEncrypt.tlh"

//
//mjeon.indonesia
//
//Live Server
#define IDN_VTC_AUTH_URL				"http://103.14.108.2:8088/GameService/BillingService.asmx"
#define IDN_MAKERCODE					"RANONLINE"
#define IDN_SERVICEID					"790019"
#define IDN_SERVICEKEY					"e379390d150409a0f76aa0d7cf53d1a0"
#define IDN_MD5KEY						"b5f97c184034cd03f1c8ed703949f017"
#define IDN_VTC_ACCOUNT_FUNCTION_URL	"http://103.14.108.2:8088/GameService/AccountFunction.asmx"
/* 
//Test Server
#define IDN_VTC_URL		"http://sandbox2.vtcebank.vn/GameServiceAPI/BillingService.asmx"
#define IDN_MAKERCODE	"RANONLINE"
#define IDN_SERVICEID	"880136"
#define IDN_SERVICEKEY	"32874be2a8f1e9722edff1ab442613a8"
#define IDN_MD5KEY		"123456"
*/

typedef std::vector<char> VECBUFFER;
typedef std::vector<char>::iterator VECBUFFER_ITER;

size_t wrapper_curl_write_callback( void *ptr, size_t size, size_t nmemb, void *userp );


class NetAgentClientMan;
class AgentServerConfigXml;

#define MAX_RANDOM_PASSWORD_NUM 30
#define	HTTP_STATUS_ALREADY_LOGIN 533

struct NET_LOGIN_FEEDBACK_DATA2;

struct DAUM_NET_LOGIN_FEEDBACK_DATA2;
struct DAUM_NET_PASSCHECK_FEEDBACK_DATA2;

struct GSP_NET_LOGIN_FEEDBACK_DATA2;

struct TERRA_NET_LOGIN_FEEDBACK_DATA2;
struct TERRA_NET_PASSCHECK_FEEDBACK_DATA2;

struct GS_NET_LOGIN_FEEDBACK_DATA2;

struct EXCITE_NET_LOGIN_FEEDBACK_DATA2;
struct EXCITE_NET_PASSCHECK_FEEDBACK_DATA2;

struct JAPAN_NET_LOGIN_FEEDBACK_DATA2;
struct NET_LOGIN_FEEDBACK_DATA;
struct NET_LOGIN_FEEDBACK_DATA2;

struct GM_NET_LOGIN_FEEDBACK_DATA2;


struct NET_CP_FEEDBACK_DATA2;
struct NET_RPN_FEEDBACK_DATA2;
struct NET_RPS_FEEDBACK_DATA2;
struct NET_RPS2_FEEDBACK_DATA2;
struct NET_CPN_FEEDBACK_DATA2;
struct NET_CE_FEEDBACK_DATA2;
struct NET_TOPUP_FEEDBACK_DATA2;
struct NET_GTCVT_FEEDBACK_DATA2;
struct NET_PPCVT_FEEDBACK_DATA2;

class GLObservePacketMgr;

namespace sc {
	class CGlobalAuthClientLogic;
}

//
//mjeon.ServiceProvider
//
//  moved below objects into the CAgenServer as its member variables.
//
/*
#if defined ( PH_PARAM ) || defined ( VN_PARAM )
	extern IEGameEncryptPtr    m_pMyRossoEncrypto; //! Terra Decrypt Module
#else
	extern IMyRossoEncryptPtr    m_pMyRossoEncrypto; //! Terra Decrypt Module
#endif
*/


typedef std::vector<char> VECBUFFER;
typedef std::vector<char>::iterator VECBUFFER_ITER;


size_t wrapper_curl_write_callback( void *ptr, size_t size, size_t nmemb, void *userp );


//! 2002-05-30 Jgkim
//! 2011-04-08 Jgkim 설정파일 xml 로 교체 AgentServerConfigXml
class CAgentServer : public NetServer, public DxMsgServer
{
public :
	CAgentServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo, SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvier, const bool bEmulator = false);
	virtual ~CAgentServer();

protected :
	// SOCKET m_sSession; //! Socket for session server
	NetAgentClientMan* m_pClientManager; //! Client 관리용
	int m_ChannelUser[MAX_CHANNEL_NUMBER]; //! 채널별 사용자 숫자 관리
	AgentServerConfigXml* m_pConfig;

	//CDaumGameAuth m_DaumAuth; //! Daum Game Decrypt Module
	SERVER_UTIL::CMinMd5 m_MinMd5; //! Excite Japan MD5
    sc::minTea m_Tea; //! Encrypt algorithm
	bool m_bHeartBeat;
	bool m_bEmulator;

	sc::CGlobalAuthClientLogic* m_pAuthManager; // for global auth

	BOOL	m_bTriggerPostCleanUp;				// mjeon.Post
	BOOL	m_bTriggerAttendanceMaintenance;	// mjeon.attendance
	BOOL	m_bCooledAttendanceTrigger;			// mjeon.attendance - 특정 시간이 지나기 전에는 연속된 maintenance 요청은 무시한다.
    UINT    m_uCntFieldResponse;                // kykim.attendance - AttendanceMaintananceStart에 대해 Ready 응답한 필드서버 카운트.
    WORD    m_wAttendanceResetTimeHour;         // kykim.attendance - 마지막에 Maintanance 요청한 시
    WORD    m_wAttendanceResetTimeMinute;       // kykim.attendance - 마지막에 Maintanance 요청한 분
	

	// typedef	std::pair<DWORD,DWORD>		APEX_ID_TIME;
	// typedef	std::list<APEX_ID_TIME>		APEX_CLOSE_LIST;
	// typedef	APEX_CLOSE_LIST::iterator	APEX_CLOSE_LIST_ITER;

	ApexCloseList m_ApexClose;

	IEGameEncryptPtr	m_pIEGameEncrypto; //Terra Decrypt Module for Phillipines and Vietnam
	IMyRossoEncryptPtr	m_pMyRossoEncrypto; //! Terra Decrypt Module for Malaysia

    int m_nServerChannelNumber; //! Server Channel Number (Agent 가 가진 서버 채널 갯수)

	// APEX_CLOSE_LIST m_listApexClose;

	// INFO:Reached Max Client Number 메시지가 뜰때 UpdateProc가 제대로 작동하는지를 알아보기 위한 변수
	bool m_bReachedMAX;

	CTime m_CurrentTime; //! 현재 시간
    CTime m_KorShutdownCheckTime;

	CCPLManager	m_CPLManager;

    //  CloseClientList
    std::vector<DWORD> m_vecCloseList;

    GLAgentServer* m_pGLAgentServer;

    DWORD m_SessionServerSlot; //! Session Server 연결 slot
    DWORD m_CacheServerSlot; //! Cahce Server 연결 Slot
	DWORD m_MatchServerSlot;

    F_SERVER_INFO m_FieldServerInfo[MAX_CHANNEL_NUMBER][FIELDSERVER_MAX]; //! Field Server Information
	A_SERVER_INFO m_AgentServerInfo[AGENTSERVER_MAX];

	//CURL		*m_curl;
	//CURLcode	m_result;

	GLObservePacketMgr* m_pObservePacketMgr;

	// 일시적으로 유저 블럭(캐릭터 복구 시스템에서 현재 사용한다. 다른데서 사용해도 좋을 듯 일시 블럭 기능)
	// 유저번호 / 삽입될 때 시간(tickcount)
	std::map< DWORD, DWORD > m_BlockUser;

public:
    GLAgentServer* GetGLAgentServer() { return m_pGLAgentServer; }
    F_SERVER_INFO* GetFieldServerInfo(DWORD ServerID, DWORD Channel);

public:
    //! -----------------------------------------------------------------------
    //! NetServer override functions 
    //! -----------------------------------------------------------------------
    virtual int Start() override;
    virtual int Stop() override;
	virtual void ForceStop( DWORD Reason ) override;
    
    virtual int StartClientManager() override;
    virtual int	WorkProc() override;
    virtual int ListenProc() override;
    virtual int	UpdateProc() override;
	virtual int	RegularScheduleProc() override;

    // I/O operation memory
    virtual PER_IO_OPERATION_DATA* GetFreeOverIO(EMMSG_IO_TYPE nType=NET_SEND_POSTED) override;
    virtual void ReleaseOperationData(PER_IO_OPERATION_DATA* pData) override;

    //! -----------------------------------------------------------------------
    //! DxMsgServer override functions
    //! -----------------------------------------------------------------------
	virtual int GetServerGroup() const override { return m_nServerGroup; }
	virtual int GetServerNum() const override { return m_nServerNum; }

	virtual void SendMySelf(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;
	virtual void SendMySelf(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;

	virtual int SendClient(NET_MSG_GENERIC* pBuffer, DWORD dwChaNum) override; //mjeon.AF
    virtual int SendClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, DWORD dwChaNum, bool UseCrc=false) override;

    virtual int SendClient(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendClient(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	virtual int	SendField(DWORD dwChannel, DWORD dwFieldSvrNum, NET_MSG_GENERIC* pBuffer) override;
    virtual int SendField(DWORD dwChannel, DWORD dwFieldSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	virtual int SendField(DWORD dwClient, NET_MSG_GENERIC* pBuffer) override;	//mjeon.AF
    virtual int SendField(DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    virtual int SendFieldSvr(int nSvrNum, NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual int SendFieldSvr(int nSvrNum, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    virtual void SendAllField(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual void SendAllField(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    //! 에이젼트에서는 SendAgent()를 호출할수 없습니다. (X)
	//! 원래는 호출할 수 없었지만 전장서버가 생기면서 다른 Agent와 통신 가능하도록 고친다;
	//! Agent는 여러개가 연결되지만 무조건 모든 연결된 Agent로 Broadcasting 하도록 한다;
    virtual int SendAgent(NET_MSG_GENERIC* pBuffer) override;
    virtual int SendAgent(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

	//! 특정 Agent;
	virtual int SendAgent ( DWORD dwClient, NET_MSG_GENERIC* pBuffer ) override;
	virtual int SendAgent ( DWORD dwClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, NET_MSG_GENERIC* pBuffer ) override;
	virtual int SendAgent_ServerGroup ( DWORD dwServerGroup, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;

    virtual void SendChannel(NET_MSG_GENERIC* pBuffer, int nServerChannel=0) override;
    virtual void SendChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false) override;

    virtual int SendAllChannel(NET_MSG_GENERIC* pBuffer) override;
    virtual int SendAllChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;
    
	//mjeon.AF
	//virtual int ConnectFieldSvr(DWORD dwClient, int nFieldServer, DWORD dwGaeaID, int nServerChannel=0) override;
	virtual int SetUpFieldInfo(DWORD dwClient, int nFieldServer, DWORD dwGaeaID, int nServerChannel=0) override;
	virtual int SetUpInstanceInfo(DWORD dwClient, int nInstanceServer, int nServerChannel);
	
	//virtual BOOL IsBoundByInstance(DWORD dwClient, DWORD &dwInstanceSvrNum);

    virtual void SendAllClient(NET_MSG_GENERIC* pBuffer) override;
    virtual void SendAllClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;
    
    virtual void CloseClient(DWORD dwClient, net::CLOSE_TYPE emCloseType = net::UNDEFINE, int nLastError = 0) override; //! Close Client Connection

private:
	void CloseClientAgentAgent( DWORD dwClient, net::CLOSE_TYPE emCloseType );
    void CloseClientClient( DWORD dwClient, net::CLOSE_TYPE emCloseType );
    void CloseClientAgentField( DWORD dwClient, net::CLOSE_TYPE emCloseType );
    void CloseClientSession( DWORD dwClient, net::CLOSE_TYPE emCloseType );
    void CloseClientCache( DWORD dwClient, net::CLOSE_TYPE emCloseType );

public:    
    virtual bool IsSlotOnline(DWORD dwClient) override;
    int GetOnlineFieldCount();

    //! -----------------------------------------------------------------------
    int	StartDbManager();
    void WriteNprotectLog( DWORD dwClient, DWORD dwResult );

    void SendAllClientChannel(NET_MSG_GENERIC* pBuffer, int nServerChannel=0);
    void SendAllClientChannel(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, int nServerChannel=0, bool UseCrc=false);
    
    //void CloseField (DWORD dwClient);
    void CloseAllClient(); 	// close all client connections

    //!  CloseClient 예약 처리
    void ReserveCloseClient(DWORD dwClient);
    void CloseClientFinal();
    bool IsReserveCloseClient(DWORD dwClient);

	Country::SCOUNTRY_INFO GetCountry ( DWORD dwDbNum ) const;

	net::EMSERVERTYPE GetServerType();

	std::string GetIP();
	int	GetPort();

	const bool IsSubServer ();

	// 어떤 Client Slot 인가?;
	const bool IsAgentClientSlot ( DWORD _dwClientSlotID );
	const bool IsFieldClientSlot ( DWORD _dwClientSlotID );

	// Agent 서버군에 따른 Client Slot ID 정보;
	const DWORD GetServerGroup_ClientSlotID ( DWORD _dwClientSlotID );
	const DWORD GetClientSlotID_ServerGroup ( DWORD _dwServerGroup );

	const DWORD GetWorldBattleServerNumberInfo ();

	//void MsgVersion			(MSG_LIST* pMsg);
	void MsgCreateCharacter	(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);    
    void MsgCreateCharacterBack(int nChaNum, DWORD dwClient, DWORD dwUserNum, const char* szUserIP, USHORT uPort=0);
	void MsgSndPing			(DWORD dwClient);
    void MsgSndPingAnswer	(MSG_LIST* pMsg);

    void ClearBlockIp();

    //! 캐릭터 이름/캐릭터 DB 번호, 캐릭터 DB 번호/캐릭터 이름
    void ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType);
	void ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType, int iClass, int iLv);

	void ExecuteReservedTask();

	DWORD GetRandomFieldSlot();

public:
	struct AgentPacketFunc
	{
		(void) (CAgentServer::*Msgfunc)(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
		AgentPacketFunc()
		{
			Msgfunc = NULL;
		}
	};
	AgentPacketFunc m_MsgFunc[NET_MSG_GCTRL_MAX];

	void initPacketFunc();

public:
	//
	//mjeon.Post
	//
	void	PostCleanUp(SYSTEMTIME &st);

	//
	//mjeon.attendance.maintenance
	//
	void	DailyMaintenance();
	BOOL	AttendanceDailyMaintenance(SYSTEMTIME &st, BOOL bManually = FALSE);
	void	MsgAttendanceDailyMaintenanceReadyFA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgAttendanceMaintenanceManualStartCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	

	//
	//not a message handler
	//
	// 출석부 정리 요청이 연속으로 들어오는 것을 방지하기 위한 flag를 관리.
	//
	
	void	Ready4AttendanceMaintenance(BOOL bReady)
	{
		m_bTriggerAttendanceMaintenance = bReady;			// daily request		
	}
	

	void	MsgReqChaNum(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgReqChaNumBack(DWORD SenderClientSlot, DWORD SenderGaeaId, int ReceiverChaDbNum, int ReceiverUserDbNum, int nReqID);
	void	MsgPostGet(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void	MsgPostGetState(DWORD dwClient,  DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void	MsgPostGetSendState(DWORD dwClient,  DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void	MsgPostGetRecvState(DWORD dwClient,  DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgPostNotify(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgPostOpen(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgPostOpenAndTakeCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgPostDelete(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgPostReturn(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	

	//
	//mjeon.Activity
	//	
	void	MsgActivityCompletedFA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgActivityClosedFA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);


	//
	//mjeon.SNS
	//

	//Get FB-info from DB and keep in AgentServer. Then sends it to the client.
	void	MsgSNSGetInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	void	MsgSNSFBAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgSNSFBUnAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	void	MsgSNSFBGetUID(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);


	//Get TW-info from DB and keep in AgentServer. Then sends it to the client.
	void	MsgSNSTWGetInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgSNSTWAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	void	MsgSNSTWUnAuthed(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	void	MsgSNSTWGetUID(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);


	//-------------------------------------------------------------------------------------
    void    MsgProtocolInfoCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	void	MsgSndChatGlobal(const char* szChatMsg);
	void	MsgSndChatNormal(DWORD dwClient, const char* szName, const char* szMsg);
	void	MsgSndCryptKey	(DWORD dwClient);
	
	void	MsgSndLoginInfoToField(DWORD dwClient);

	void	MsgSndChaBasicBAInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	
	void	MsgSndChaBasicInfo  (DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void	MsgSndChaBasicInfo  (DWORD dwClient, int nChaNum);
	void	MsgSndChaBasicInfoEnd  (DWORD dwClient);
	void	MsgSndChaBasicInfoBack(NET_MSG_GENERIC* pMsg);
	void	MsgSndChaBasicInfoBackEnd(NET_MSG_GENERIC* pMsg);
	
	void	MsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg); // Agent->Client:캐릭터 삭제 결과 전송	
	void	MsgSndChaDelInfoBack( int nChaNum, 
		                          int nUserNum, 
								  int nResult, 
								  DWORD dwClient, 
								  const TCHAR* szUserIP, 
								  USHORT uPort=0 ); // 게임서버->클라이언트 : 캐릭터 삭제 결과 전송	

	//! Agent->Client:캐릭터 삭제
	void DaumMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! Agent->Client:캐릭터 삭제 결과 전송
	void DaumMsgSndChaDelInfoBack(int nChaNum, int nUserNum, int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort=0);

	//
	//mjeon.indonesia
	//
	void IdnMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void IdnMsgSndChaDelInfoBack(int nChaNum, int nUserNum, int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
	    
	//! Client->Agent:캐릭터 삭제요청
	void TerraMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    //! Client->Agent:캐릭터 삭제요청
	void GspMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! Client->Agent:캐릭터 삭제요청
	void ExciteMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! \param nChaNum 
	//! \param nUserNum 
	//! \param nResult 
	//! \param dwClient 
	//! \param szUserIP 
	//! \param uPort 
	void ExciteMsgSndChaDelInfoBack(
        int nChaNum, 
        int nUserNum, 
        int nResult, 
        DWORD dwClient, 
        const char* szUserIP, 
        USHORT uPort=0);

	//! Client->Agent:캐릭터 삭제요청
	void JapanMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! \param nChaNum 
	//! \param nUserNum 
	//! \param nResult 
	//! \param dwClient 
	//! \param szUserIP 
	//! \param uPort 
	void JapanMsgSndChaDelInfoBack(
        int nChaNum, 
        int nUserNum, 
        int nResult, 
        DWORD dwClient, 
        const char* szUserIP, 
        USHORT uPort=0);

	//! Agent->Client:캐릭터 삭제결과전송
	//! \param nChaNum 캐릭터번호
	//! \param nUserNum 유저번호
	//! \param nResult 결과
	//! \param dwClient 클라이언트번호
	//! \param szUserIP IP
	//! \param uPort PORT
	void TerraMsgSndChaDelInfoBack(
        int nChaNum, 
        int nUserNum, 
        int nResult, 
        DWORD dwClient, 
        const char* szUserIP, 
        USHORT uPort=0);

	/**
	* Agent->Client:캐릭터 삭제결과전송
	* \param nChaNum 캐릭터번호
	* \param nUserNum 유저번호
	* \param nResult 결과
	* \param dwClient 클라이언트번호
	* \param szUserIP IP
	* \param uPort PORT
	*/
	void GspMsgSndChaDelInfoBack(
			int nChaNum, 
	        int nUserNum, 
			int nResult, 
			DWORD dwClient, 
			const TCHAR* szUserIP, 
			USHORT uPort=0 );


	/**
	* Client->Agent:캐릭터 삭제요청
	* \param pMsg 
	*/
	void	GsMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg); // Agent->Client:캐릭터 삭제 결과 전송	

	/**
	* Agent->Client:캐릭터 삭제결과전송
	* \param nChaNum 캐릭터번호
	* \param nUserNum 유저번호
	* \param nResult 결과
	* \param dwClient 클라이언트번호
	* \param szUserIP IP
	* \param uPort PORT
	*/
	void	GsMsgSndChaDelInfoBack( int nChaNum, 
		                          int nUserNum, 
								  int nResult, 
								  DWORD dwClient, 
								  const TCHAR* szUserIP, 
								  USHORT uPort=0 ); // 게임서버->클라이언트 : 캐릭터 삭제 결과 전송	
	
	//! Client->Agent : 캐릭터 선택후 DB에서 읽어오기 요청.
	void MsgGameJoinCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg); 
	//! DB->Agent : DB에서 읽어진 정보로 케릭터 접속 시도.
	void MsgAgentReqJoinDA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg);
	//void MsgAgentReqJoin( DWORD dwClientID, GLCHARAG_DATA * pCHAR_DATA );
	//! Field->Agent : 캐릭터가 게임에 조인했음
	void MsgLobbyCharJoinFA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg);

	void MsgChaJoin2ndPassword(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg); // 2차비번

	void MsgJoinInstance(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	

	void MsgFieldSvrOutFb(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgResurrectMoveMapFb(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgReCallFb(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgPremiumSetFb(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);    	
	void MsgTeleportFb(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	void MsgServerLoginMaintenanceAGT(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	void MsgUserSystemInfoCA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	
	void SetUserInfo(DWORD nClient, const char* strUserId, const char* strPassword); 	

	BOOL CheckConnectionIP(const std::string& strIP) { return m_CPLManager.CheckConnectionIP(strIP); }

	//! Execute Command Text
	//! @param strCmd 컴멘드
	int	ExecuteCommand(char* strCmd);

	//////////////////////////////////////////////////////////////////////////
	/// All Connection Agent Server
	//////////////////////////////////////////////////////////////////////////
	void AgentConnectAll ();

	void FindAgentSlot ( DWORD dwClient, DWORD& dwAgentNum );

	int AgentConnect ( DWORD dwAgentNum );
	int	AgentConnect ( unsigned long ulServerIP, int nPort, DWORD dwAgentNum = 0 );

	int	AgentCloseConnect ( DWORD dwSlotNum );
	void AgentCloseConnectAll ();

	DWORD GetAgentSlotNum ( DWORD dwAgentNum );
	void AgentSlotSet ( DWORD dwAgentNum, DWORD dwSlotNum );
	void AgentSlotReset ( DWORD dwAgentNum );

	BOOL AgentMsgProcess ( MSG_LIST* pMsg );

	///////////////////////////////////////////////////////////////////////////
	/// Field Server Communication Member Function
	/// If you do not use Field Server, this functions are unnecessary
	///////////////////////////////////////////////////////////////////////////
	void FieldConnectAll();
	void FindFieldSlot( DWORD dwClient, DWORD& dwChannel, DWORD& dwFieldNum );
	
	int FieldConnect(DWORD FieldNum=0, DWORD Channel=0);	
	// int FieldConnect(const char* szServerIP, int nPort, int nFieldNum=0, int nChannel=0);
	int	FieldConnect(unsigned long ulServerIP, int nPort, DWORD FieldNum=0, DWORD Channel=0);

	int	FieldCloseConnect(DWORD SlotNum);
	void FieldCloseConnectAll();
    
    void FieldSlotSet(DWORD Channel, DWORD FieldNum, DWORD SlotNum);
    void FieldSlotReset(DWORD Channel, DWORD FieldNum);
    void FieldSlotReset(DWORD SlotNum);

	// bool CheckFieldConnection();

	///////////////////////////////////////////////////////////////////////////
	/// 메시지 처리 관련 함수들
	///////////////////////////////////////////////////////////////////////////	
	//! Local Message process function ( gaeaserver로 직접 내부 메시지 전송. )
    int	InsertMsg(int nClient, void* pMsg);
	int	InsertMsg(int nClient, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);
	
	//! 모든 메시지는 일단 이곳을 거친다.
	void MsgProcess(MSG_LIST* pMsg);

	void MsgAutoRelay(DWORD dwClient, NET_MSG_GENERIC *nmg);	//mjeon.AF
	//! 서버끼리의 통신일때는 이곳을 거친다.
	//BOOL ServerMsgProcess(MSG_LIST* pMsg);
	//! 서버끼리의 통신중 session 서버가 아닐때는 이곳을 거친다.
	BOOL FieldMsgProcess(MSG_LIST* pMsg);	

	// 받은 메시지들을 처리한다.
	void RecvMsgProcess();
	// 하트비트 체크를 한다.
	void AgentSrvHeartBeatCheck( DWORD dwUdpateTime );

    void ClientNprotectCheck();
    void ClientHeartBeatCheck();
    void ServerHeartBeatCheck();
    void ServerHeartBeatRequest();

    // 서버 업데이트 
	void UpdateClientState();
	// 현재 서버의 상태를 프린트 한다.
	void PrintDebugMsg(DWORD dwUdpateTime);
	// UpdateThread 종료
	void EndUpdateThread();
	// 게임가드 관련 스레드 종료
	void EndRegularScheduleThread();

	///////////////////////////////////////////////////////////////////////////
	/// 채널 관련 함수들
	///////////////////////////////////////////////////////////////////////////
	int  IncreaseChannelUser(int nChannel);
	int  DecreaseChannelUser(int nChannel);
	int  GetChannelUser     (int nChannel);
	bool IsChannelFull      (int nChannel);
	void ChangeChannelUser ( DWORD dwClient, int nPreChannel, int nAfterChannel );

	//////////////////////////////////////////////////////////////////////////
	/// Agent Msg Process
	//////////////////////////////////////////////////////////////////////////
	void MsgHeartbeatServerRequest ( DWORD dwClient, NET_MSG_GENERIC* pMsg );
	void MsgHeartbeatServerAnswer ( DWORD dwClient, NET_MSG_GENERIC* pMsg );

	void MsgAgentRelayAC ( DWORD _dwClient, NET_MSG_GENERIC_RELAY_AGENT* _pMsg );
	void MsgAgentRelayAF ( DWORD _dwClient, NET_MSG_GENERIC_RELAY_AGENT* _pMsg );
	void MsgAgentRelayAB ( DWORD _dwClient, NET_MSG_GENERIC_RELAY_AGENT* _pMsg );
	
	///////////////////////////////////////////////////////////////////////////
	/// 로그인 관련 함수들
	///////////////////////////////////////////////////////////////////////////	
	//! 로그인 message ( 대만, 홍콩, 내부 ) 
	void MsgLogIn(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgLogInBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 );
	
	//! 로그인 message ( 대만, 홍콩, 내부 ) 
	void MsgRegister(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgRegisterBack( NET_REGISTER_FEEDBACK_DATA2* nlfd2 );


	//userpanel
	void MsgChangePass ( MSG_LIST* pMsg );
	void MsgChangePassBack ( NET_CP_FEEDBACK_DATA2* nlfd2 );
	void MsgResetPin ( MSG_LIST* pMsg );
	void MsgResetPinBack ( NET_RPN_FEEDBACK_DATA2* nlfd2 );
	void MsgResetPass ( MSG_LIST* pMsg );
	void MsgResetPassBack ( NET_RPS_FEEDBACK_DATA2* nlfd2 );
	void MsgResetPass2 ( MSG_LIST* pMsg );
	void MsgResetPassBack2 ( NET_RPS2_FEEDBACK_DATA2* nlfd2 );
	void MsgChangePin ( MSG_LIST* pMsg );
	void MsgChangePinBack ( NET_CPN_FEEDBACK_DATA2* nlfd2 );
	void MsgChangeEmail ( MSG_LIST* pMsg );
	void MsgChangeEmailBack ( NET_CE_FEEDBACK_DATA2* nlfd2 );
	void MsgTopUp ( MSG_LIST* pMsg );
	void MsgTopUpBack ( NET_TOPUP_FEEDBACK_DATA2* nlfd2 );
	void MsgGameTimeCVT ( MSG_LIST* pMsg );
	void MsgGameTimeCVTBack ( NET_GTCVT_FEEDBACK_DATA2* nlfd2 );
	void MsgPremiumPointCVT ( MSG_LIST* pMsg );
	void MsgPremiumPointCVTBack ( NET_PPCVT_FEEDBACK_DATA2* nlfd2 );

	//
	//mjeon.indonesia
	//
	void IdnMsgLogIn(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	int  IdnAuthVTC(std::string &strID, std::string &strPW);
	int  IdnAuthVTC_2ndPass(std::string &strID, std::string &str2ndPass);
	void IdnMsgLogInBack(NET_LOGIN_FEEDBACK_DATA2* nlfd2);	

    //! World Battle
    void MsgLoginWbCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg);
    //void MsgWorldBattleLogin(DWORD ClientSlot, DWORD GaeaID, const json_spirit::mObject& Obj);
    void MsgWorldBattleLoginBack(DWORD ClientSlot, NET_MSG_GENERIC* pMsg);
    void WorldBattleLoginFail(DWORD ClientSlot, NET_LOGIN_FEEDBACK_DATA2* pPacket, NET_LOGIN_FEEDBACK_DATA &MsgAC);
    void WorldBattleLoginSuccess(NET_LOGIN_FEEDBACK_DATA& MsgAC, NET_LOGIN_FEEDBACK_DATA2* pPacket, DWORD dwClientSlot);
   

	//! China 로그인 메시지 ( 중국 ) 
	void ChinaMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void ChinaMsgLoginBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Daum 로그인 메시지 ( 한국 ) 
	void DaumMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void DaumMsgLoginBack(DAUM_NET_LOGIN_FEEDBACK_DATA2* pMsg);

	//! Daum 패스워드 체크 메시지
	void DaumMsgPassCheck(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void DaumMsgPassCheckBack(DAUM_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2);

	//! GSP 로그인 메시지
	void GspMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void GspMsgLoginBack( GSP_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Terra 로그인 메시지 ( 말레이시아, 필리핀, 베트남 )
	void TerraMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void TerraMsgLoginBack( TERRA_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Terra 패스워드 체크 메시지
	void TerraMsgPassCheck(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void TerraMsgPassCheckBack( TERRA_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2 );

	//! Excite Japan 로그인 메시지 ( 일본 )
	void ExciteMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void ExciteMsgLoginBack(EXCITE_NET_LOGIN_FEEDBACK_DATA2* nlfd2);

	//! Excite Japna 패스워드 체크 메시지
	void ExciteMsgPassCheck(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void ExciteMsgPassCheckBack( EXCITE_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2 );

	//! 일본 Gonzo Japan 로그인 메시지
	void JapanMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void JapanMsgLoginBack(JAPAN_NET_LOGIN_FEEDBACK_DATA2* nlfd2);

	// 일본 Gonzo Japan 로그인 관련 함수
	void JapanWRFailed( DWORD dwClient, int nResult );
	void JapanWRSuccess( DWORD dwClient, CString strUserID, int nServerGroup, int nServerNum, CString strUUID, int nUserNum );

	//! 로그인 메시지 (Thailand) ( 태국 ) 
	//! \param pMsg 
	void ThaiMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void ThaiMsgLogInBack(NET_LOGIN_FEEDBACK_DATA2* nlfd2);	

    //! Encoding 된 TID 에서 Decoding 된 TID 를 얻는다.
	//! @param szEncodedTID Encoding 된 TID
	//! @param szDecodedTID Decoding 된 TID
	//! @return	
	int TerraGetDecodedTID(char* szEncodedTID, char* szDecodedTID);
	
	// GS 접속전에 WB 접속 여부 확인
	void GsMsgWBLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void GsMsgWBLoginAW(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void GsMsgWBLoginWA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! 로그인 message ( 글로벌 서비스 ) 
	void GsMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void GsMsgLoginBack( GS_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! 로그인 message ( EU ) 
	void GmMsgLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	BOOL GmHTTPAuth( CString &strID, CString &strPW, std::string &strGUID, int &nBlock );
	void GmMsgLoginBack( GM_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! 로그인 message ( US ) 
	void UsMsgLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void UsMsgLoginBack( US_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! 랜덤 암호를 위한 번호를 보낸다.
	void MsgSndRandomNumber(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! Apex Data 메세지	// Apex 적용
	void MsgApexData(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgApexReturn(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);	// Apex 적용( 홍콩 )

	//! Apex 가 사용자를 블럭 시킬때 호출됨 Block
	void ApexUserBlock( DWORD dwClient, WORD dwHour );

	//! Apex 가 사용자 채팅을 금지 시킬때 호출됨 
	void ApexUserChatBlock( DWORD dwClient, WORD dwHour );

	//! Apex 가 사용자 로그를 남길때 호출됨
	void ApexUserHackLog( DWORD dwClient, TCHAR* szComment );
	
	// APEX에 이상 사용자 로그아웃 통지
	void ApexUserLogout(DWORD dwClient);

	//! Apex 가 경고메시지를 클라이언트에게 보낸다.
	void ApexSendMessageToClient( DWORD dwClient, WORD wAction, WORD wHour );

	void ApexCloseClient( DWORD dwClient );

	void ApexCloseClientFinal();

	///////////////////////////////////////////////////////////////////////////
	/// nPortect 관련 함수들
	///////////////////////////////////////////////////////////////////////////	
	
	//! nProtect 인증용 메시지 전송
	void MsgSndGameGuard( DWORD dwClient, NET_GAMEGUARD_AUTH& nga );

	//! nProtect 클라이언트 인증 First
	void MsgSndGameGuardFirstCheck( DWORD dwClient );

	//! nProtect 클라이언트 인증 Second
	void MsgSndGameGuardSecondCheck( DWORD dwClient );

	//! nProtect 인증용 메세지 결과
	void MsgGameGuardAnswer(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! nProtect 인증용 메세지 결과
	void MsgGameGuardAnswerFirst(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! nProtect 인증용 메세지 결과
	void MsgGameGuardAnswerSecond(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	//! 필드서버와 클라이언트에서 받은 추적 로그를 찍는다.
	void MsgTracingLogPrint(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// 추적 ID인지 아닌지 체크
	int CheckTracingUser( const char *pUserAccount, const int nUserNum );
	
	///////////////////////////////////////////////////////////////////////////
	/// GM 명령용 메시지 함수들
	///////////////////////////////////////////////////////////////////////////	
	void MsgGmPkMode(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
	void MsgGmwherePcMap(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

	// 베트남 타임 리셋 임시 함수
	void MsgVietTimeReset(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg); //vietnamtest%%% && vietnamtest2

    void MsgGmLogItemReloadCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);
    void MsgGmGameDataUpdateCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);
    void MsgGmGameDataUpdateSS( NET_MSG_GENERIC* nmg );

    void MsgUserChangeCountry(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);

    void MsgCloseClient(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);

	//! -----------------------------------------------------------------------
	//! Integration Cache Server
	//! -----------------------------------------------------------------------
	virtual void SendIntegrationCache( NET_MSG_GENERIC* pMsg ) override;
	virtual void SendIntegrationCache( EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc = false ) override;


    //! -----------------------------------------------------------------------
    //! Cache Server
    //! -----------------------------------------------------------------------
    int CacheConnect();
	int CacheCloseConnect();
    void CacheMsgProcess(MSG_LIST* pMsg);    
    virtual void SendCache(NET_MSG_GENERIC* pMsg) override;
    virtual void SendCache(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    void CacheSendHeartbeat();

	//mjeon.instance.match
	int MatchConnect();
	int MatchCloseConnect();
    BOOL MatchMsgProcess(MSG_LIST* pMsg);
    void MatchSendHeartbeat();
	virtual int SendMatch(NET_MSG_GENERIC* pMsg) override;
    virtual int SendMatch(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false) override;

    //! Point Shop
    void MsgPointShopDataStart(NET_MSG_GENERIC* pMsg);
    void MsgPointShopDataEnd(NET_MSG_GENERIC* pMsg);
    void MsgPointShopData(NET_MSG_GENERIC* pMsg);
    void MsgPointShopGmReloadCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg);

    //! Game
    //! Agent->Client
    void MsgSndChaBasicBAInfoBack(NET_MSG_GENERIC* pMsg);

    //! -----------------------------------------------------------------------
	//! Session Server Communication Member Function	
	//! -----------------------------------------------------------------------
	int	SessionConnect(unsigned long ulServerIP, int nPort);
	int SessionConnect(const char* szServerIP, int nPort);

	//! Session 서버와 통신을 체크하고 이상이 있으면 다시 연결한다.
	int SessionReconnect();
	
	//! Session 서버에 접속하고 서버의 정보를 전송한다.
	int SessionConnectSndSvrInfo(const char* szServerIP, int nPort);
	int SessionConnectSndSvrInfo();

	int SessionCloseConnect();	
    int	SendSession(NET_MSG_GENERIC* pMsg);
	void SessionMsgProcess(MSG_LIST* pMsg);

	void SessionMsgHeartbeatServerAns();
	
	//! 세션서버->게임서버 : 채팅메시지
	void SessionMsgChatProcess(NET_MSG_GENERIC* nmg);
	void SessionMsgSvrCmd(NET_MSG_GENERIC* nmg);

	//  로그인 키
	void SessionMsgEncryptKey(NET_MSG_GENERIC* nmg);

    void SessionMsgVersionSS(NET_MSG_GENERIC* nmg);

    void DoNothing(NET_MSG_GENERIC* pMsg);
	
	//! Ping Answer	
	void SessionMsgPingAnswer(NET_MSG_GENERIC* nmg);
	//! Send ping
	// void SessionSndPing();
	//! Agent->Session:서버의 모든 정보를 보낸다.
	int SessionSndSvrInfo();
	//! Send Heartbeat
    void SessionSndHeartbeat(); 
	//! Agent->Session:서버의 현재 상태를 보낸다.
	int SessionSndSvrCurState();
	//! Agent->Session:서버의 현재 채널 상태를 보낸다.
	int SessionSndChannelState(int nChannel);

	//
	//mjeon.maintenance
	//
	//! 서버가 갑작스럽게 멈췄거나, 예상치못한 시간에(attendanceMaintenance 등과 같은 점검 시간과 겹쳐서) 멈췄을 경우
	//! 다음 서버가 구동할 때, 해당 작업들을 일괄 수행해준다.
	//! 이 때, 이러한 작업들은 이전에 이미 처리되지 않았는지 신중히 검사한 이후에 진행되어야 할 것이다.
	void CheckMaintenanceJobs();

	//! 서버 다운시 UserLoginState값이 제대로 초기화 되지 않는 문제가 있다.
	//! Agent가 처음 시작할 때 UserDB의 UserLoginState값을 0으로 초기화 시켜준다.
	void InitUserLoginState();

	int SessionSndCreateCharacter(int nUserNum);
	int	DaumSessionSndCreateCharacter(int nUserNum);

	int	SessionSndTestCreateCharacter(int nUserNum);
	int	DaumSessionSndTestCreateCharacter(int nUserNum);

	int	SessionSndDelCharacter     (int nUserNum);
	int	DaumSessionSndDelCharacter (int nUserNum);

	int SessionSndTestDelCharacter (int nUserNum);
	int DaumSessionSndTestDelCharacter(int nUserNum);

	BOOL UpdateTracingCharacter(NET_MSG_GENERIC* nmg );

	void MsgHeartbeatClientAns(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    
    void MsgCreateClub(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    void MsgJson(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    bool MsgJsonParser(DWORD ClientSlot, DWORD GaeaID, const std::string& JsonStr);

    void MsgClubLogCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void MsgClubLogHA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* nmg);
    void MsgClubLogHA_End(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* nmg);
    void MsgClubMemberGradeChangeCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);
    void MsgClubMemberCdCertifyCA(DWORD ClientSlot, DWORD GaeaID, const json_spirit::mObject& Obj);
    
    void MsgClubGradeNameChangeCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);

    void MsgClubMemberLobbyCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);
    void MsgClubNewLobbyCA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    void MsgGmDbUnlockUser(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    void MsgLogAttendanceDS(NET_MSG_GENERIC* pMsg);
    void MsgLogAttendanceTaskDS(NET_MSG_GENERIC* pMsg);

protected:
	void BroadcastToField(NET_MSG_GENERIC* pBuffer, DWORD ServerChannel=0);
	int	BroadcastToField(DWORD FieldNum, NET_MSG_GENERIC* pBuffer, DWORD Channel);
	int	BroadcastToField(DWORD SlotNum, NET_MSG_GENERIC* pBuffer);

public:
    void AddGameDbJob(db::DbActionPtr spJob);
    void AddGameAdoJob(db::DbActionPtr spJob);

	//개인상점검색 시스템 TEST
	void MsgPmarketSearchReqCA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);
	void MsgSearchResultHA(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* nmg);

	void MsgPrivateMarketItemNumTotalHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemNumHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketItemEndHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketLogNumHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketLogHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketLogEndHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgPrivateMarketStateHA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

    //개인상점 검색카드 사용
    void MsgPmarketSearchOpenFA(DWORD dwClient, NET_MSG_GENERIC* nmg);
    void MsgPmarketSearchClose(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg);
public:
    std::string GetClientIP(DWORD dwClient) const override;

public:
	void MsgGmReferenceCountCA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
    void MsgGmLoadIPEventCA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:
	int SendAuthInfoToSessionServer(); // 세션서버로 인정 정보 전송. 세션에서 인증서버로 릴레이
	int SendEventInfoToSessionServer(); // 세션서버로 이벤트 정보 전송. 세션서버는 인증서버로 해장 정보 릴레이
	void MsgAuthCertificationResponse(NET_MSG_GENERIC* pMsg);
	void MsgAuthEventInfoRequest(NET_MSG_GENERIC* pMsg);

public:
	// TexasHoldem
	void MsgTexasHolem( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:
	void MsgBlockUserFA( NET_MSG_GENERIC* nmg );
	bool IsBlockUser( DWORD dwUserNum );

public:
	void MsgGmCommandLogCA( DWORD ClientSlot, DWORD GaeaID, const json_spirit::mObject& Obj );

public:
	// Country
	void MsgUpdateUserCountryFA ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgUpdateUserCountryCA ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgUpdateUserCountryDA ( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );

public:
	void MsgDisconnectCharacterFA( NET_MSG_GENERIC* nmg );
	
/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.21
 * modified : 
 */
public:
    // 등록 요청
    void MsgGmSetRandomboxChanceReqHA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    // 조회 요청
    void MsgGmGetRandomboxChanceListHA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

    // 삭제 요청
    void MsgGmDelRandomboxChanceHA(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg);

public :
	void MsgRanMobileCommandRequest( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	void MsgRanMobileCommandResponse( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
};

extern CAgentServer* g_pAgentServer;

#endif // _LS_AGENT_SERVER_H_

