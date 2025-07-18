#ifndef GLOGIXEX_H_
#define GLOGIXEX_H_

#pragma once

// GLCHARLOGIC, GLCROWLOGIC
//	두 클래스가 공통으로 가지는 함수들은 데이터의 근원 "케릭터 데이터", "NPC 데이터" 에서
//	각각 받아서 같은 내용을 함수로 제작하게됨. 실제 함수 부분은 각각 따로 있음. ( 동작 효율성 고려. )
//	단, 함수 내용은 동일하게 유지. ( 다른 데이터 근원에서 가져오더라도 인라인 함수 정의를 동일하게하여 맞춤. )
//
// 현재(2012/07/03) 위의 취지대로 사용되고 있지 않음;
// GLogic, GLogicCrow 및 ClientActor, ServerActor의 존재로 액션(이동, 스킬 등) 관련 작업 시,
// 동일한 코드를 두세군데 이상 작성해야 하는 번거로움이 존재;

#include <boost/tr1/unordered_map.hpp>
#include <list>

//#include "../InternalCommonLib/Interface/PlayerInterface.h"

#include "./Skill/GLSKILL.h"
#include "./Character/GLCharData.h"
#include "./Crow/GLCrowData.h"
#include "./GLogicData.h"
#include "./GLogicExDefine.h"
#include "./Msg/GLContrlMsg.h"

#include "./Skill/GLCountAttackSkill.h"
#include "./Skill/GLSkillEffVarSet.h"

using namespace NSSKILL_EFFSET;

#ifndef GLSKILL
	struct GLSKILL;
#endif //GLSKILL

struct SProductRecipe;

#define MOVE_OPTION_SHOW_EFF			0x00000001
#define MOVE_OPTION_NOT_TURN_IDLE		0x00000002

#define PK_COMBO_TIMER 10.0f

// 결국 같이 연산되어야하는 값들이 있지만 합치면 안된다;
// ex) fIncR_AtkSpeed, fIncR_AtkSpeed_SoloPlay는 결국 하나의 공속으로 치환되나;
// fIncR_AtkSpeed_SoloPlay는 다른 값에 의해 최대값이 제어가되어야함;
struct SSUM_ITEM
{
	GLDWDATA	gdDamage;
	int			nDefense;

	int			nAvoidRate;
	int			nHitRate;

	int			nHP;
	int			nMP;
	int			nSP;

	float		fIncR_HP;
	float		fIncR_MP;
	float		fIncR_SP;

	float		fInc_HP;
	float		fInc_MP;
	float		fInc_SP;

	SCHARSTATS	sStats;
	SRESIST		sResist;

	int			nPA;
	int			nSA;
	int			nMA;

	float		fIncR_MoveSpeed;
	float		fIncR_AtkSpeed;
	float		fIncR_Critical;
	float		fIncR_CrushingBlow;
	float		fIncR_Ignore_BossDamage;		// 보스몹에게 피격시 데미지 차감;
	float		fIncR_Critical_EmergencyMob;	// 위급 상태의 몹에게 크리티컬 발생률 증가;
	float		fIncR_Critical_EmergencyBoss;

	float		fInc_MoveSpeed;
	float		fInc_AtkSpeed;
	float		fInc_Critical;
	float		fInc_CrushingBlow;

	float		fIncR_ExpAddEffect;	// 경험치 증가율 효과'를 증가시킴;
	float		fIncR_DropGenMoney;	// 몹 사냥시 드랍되는 돈 증가율;
	float		fDecR_Damage_MeleeSkill; // 물리 근접 스킬 피격시 경우 데미지 감소;
	float		fDecR_Damage_RangeSkill; // 사격 스킬 피격시 " ;
	float		fDecR_Damage_MagicSkill; // 마법 스킬 피격시 " ;
	float		fIncR_AtkSpeed_SoloPlay; // 솔로플레이시 공격속도 증가;
	float		fIncR_AtkSpeed_PartyPlay; // 솔로플레이시 공격속도 증가;

	int			nPotion_HP_Inc;
	int			nPotion_MP_Inc;
	int			nPotion_SP_Inc;
	int			nReceive_CP_Inc;
	
	float		fExpMultiple;		//! 경험치 배수

	INT			nInc_BossFixedDamage;		// 보스몹 공격시 추가 고정 데미지;
	float		fDecR_Damage_EmergencyState;	// 위급 상태일 경우 데미지 감소;
	INT			nInc_HP_SoloPlay;			// 솔로플레이시 HP증가 (파티가 아닐시);
	INT			nInc_HP_PartyPlay;			// 파티플레이시 HP증가;

	float		fInc_SkillDurationTAdd[SKILL::MAX_GRADE];	// 등급별 스킬 지속시간 증가;
	float		fIncR_SkillDamageAdd[SKILL::MAX_GRADE];		// 등급별 스킬 데미지 증가;
	float		fDecR_SkillDelayT[SKILL::MAX_GRADE];			// 등급별 스킬 딜레이 감소;
	float		fIncR_CriticalDamage;						// 크리티컬 데미지 증가율;
	float		fIncR_CrushingBlowDamage;							// 강한타격 데미지 증가율
	float		fIncR_LifePerKill;							// 처치시 생명력 회복률;
	float		fIncR_MPPerKill;							// 처치시 MP회복률;
	float		fIncR_SPPerKill;							// 처치시 SP회복률;
	float		fIncR_HPMPSPPerKill;						// 처치시 HP,MP,SP회복률;

	float		fIncR_AllMPRatioToAttack;					// 전체 MP 비율만큼 공격력증가;
	float		fIncR_AllSPRatioToHp;						// 전체 SP 비율만큼 체력증가;
	float		fIncR_AllHitRatioToMelee;					// 전체 명중 비율만큼 격투치 증가;
	float		fIncR_AllHitRatioToRange;					// 전체 명중 비율만큼 사격치 증가;
	float		fIncR_AllHitRatioToMagic;					// 전체 명중 비율만큼 기력치 증가;

	SSUM_ITEM ()
		: nDefense(0)

		, nAvoidRate(0)
		, nHitRate(0)
		, nHP(0)
		, nMP(0)
		, nSP(0)
		, fIncR_HP(0.0f)
		, fIncR_MP(0.0f)
		, fIncR_SP(0.0f)

		, fInc_HP(0.0f)
		, fInc_MP(0.0f)
		, fInc_SP(0.0f)

		, nPA(0)
		, nSA(0)
		, nMA(0)
		, fIncR_MoveSpeed(0.0f)
		, fIncR_AtkSpeed(0.0f)
		, fIncR_Critical(0.0f)
		, fIncR_CrushingBlow(0.0f)
		, fIncR_Ignore_BossDamage(0.0f)
		, fIncR_Critical_EmergencyMob(0.0f)
		, fIncR_Critical_EmergencyBoss(0.0f)
		, fIncR_ExpAddEffect(0.0f)
		, fIncR_DropGenMoney(0.0f)
		, fDecR_Damage_MeleeSkill(0.0f)
		, fDecR_Damage_RangeSkill(0.0f)
		, fDecR_Damage_MagicSkill(0.0f)
		, fIncR_AtkSpeed_SoloPlay(0.0f)
		, fIncR_AtkSpeed_PartyPlay(0.0f)

		, fInc_MoveSpeed(0.0f)
		, fInc_AtkSpeed(0.0f)
		, fInc_Critical(0.0f)
		, fInc_CrushingBlow(0.0f)
		, nPotion_HP_Inc(0)
		, nPotion_MP_Inc(0)
		, nPotion_SP_Inc(0)
		, nReceive_CP_Inc(0)

		, fExpMultiple(1.0f)
		, nInc_BossFixedDamage(0)
		, fDecR_Damage_EmergencyState(0.0f)
		, nInc_HP_SoloPlay(0)
		, nInc_HP_PartyPlay(0)
		, fIncR_CriticalDamage(0.0f)
		, fIncR_CrushingBlowDamage(0.0f)
		, fIncR_LifePerKill(0.0f)
		, fIncR_MPPerKill(0.0f)
		, fIncR_SPPerKill(0.0f)
		, fIncR_HPMPSPPerKill(0.0f)
		, fIncR_AllMPRatioToAttack(0.0f)
		, fIncR_AllSPRatioToHp(0.0f)	
		, fIncR_AllHitRatioToMelee(0.0f)
		, fIncR_AllHitRatioToRange(0.0f)
		, fIncR_AllHitRatioToMagic(0.0f)
	{
		memset(fInc_SkillDurationTAdd, 0.0f, sizeof(float) * SKILL::MAX_GRADE);
		memset(fIncR_SkillDamageAdd, 0.0f, sizeof(float) * SKILL::MAX_GRADE);
		memset(fDecR_SkillDelayT, 0.0f, sizeof(float) * SKILL::MAX_GRADE);
	}

	void RESET ()
	{
		*this = SSUM_ITEM();
	}
};

namespace GLOGICEX
{
	EM_BRIGHT_FB GLSPACEGAP ( const EMBRIGHT emACTOR, const EMBRIGHT emRECEP, const EMBRIGHT emSPACE );

	//	공격 성공율 계산.
	int GLHITRATE ( const int nHit, const int nAvoid, const EM_BRIGHT_FB bFB );
	int GLHITRATE_RENEW(const int nHit, const int nAvoid, const INT nAtLv, const INT nDfLv);

	//	완전 쇼크 확율.
	BOOL CHECKSHOCK ( const int AttackerLev, const int DefenserLev, const int nDamage, const bool bCritical );

	//	방어력 계산.
	int GLDEFENSE ( const int nDEFENSE, const EMBRIGHT emACTOR, const EMBRIGHT emRecep, const EMBRIGHT emSPACE );

	//	경험치 획득 ( 타격시 )
	int GLATTACKEXP ( int AttackerLev, int DefenserLev, DWORD dwDamage, DWORD wMaxHP, DWORD dwBonusExp=0 );

	//	경험치 획득 ( 제거시 )
	int	GLKILLEXP ( int AttackerLev, int DefenserLev, DWORD dwBonusExp=0 );	
	
	//	죽었을때 경험치 감소.
	float GLDIE_DECEXP ( WORD wACTLEV );

	//	경험치 복구 비율
	float GLDIE_RECOVERYEXP ( WORD wACTLEV );

	//  경험치1당 돈
	float GLDIE_EXPMONEY ( WORD wACTLEV );

	//	LevelUp을 하기 위한 경험치
	LONGLONG GLNEEDEXP ( WORD wLev );
	LONGLONG GLNEEDEXP2 ( WORD wLev );

	BOOL GLCALCULATEEXP ( WORD wCurLev, LONGLONG llCurExp, LONGLONG llCalExp, WORD &wExceptedLev, LONGLONG &llRemainExp, LONGLONG &llRemainMaxExp );
	BOOL GLCALCULATEEXP2 ( WORD wCurLev, LONGLONG llCurExp, LONGLONG llCalExp, WORD &wExceptedLev, LONGLONG &llRemainExp, LONGLONG &llRemainMaxExp );

	// 렙업을 위한 누적 경험치
	LONGLONG GLNEEDACCUMULATEDEXP ( WORD wCurLev, WORD wNeedLev );
	LONGLONG GLNEEDACCUMULATEDEXP2 ( WORD wCurLev, WORD wNeedLev );

	//	상태이상 발생할 수 있는 확율 산출.
	BOOL CHECKSTATEBLOW ( float fACTRATE, WORD wACTLEVEL, WORD wLEVEL, WORD wRESIST );

	//	스킬 딜레이 산출.
	float SKILLDELAY ( DWORD dwSKILL_GRADE, WORD wSKILL_LEV, WORD wCHAR_LEVEL, float fDelay );

	//	Note : 파라메타 변경.
	//		계산시에는 int 형으로 계산을 수행하여 연산시 손실을 막는다.
	//		값 적용시에는 WORD 형 값의 영역 이내로 제한.
	inline WORD VARIATION ( WORD &wNow, const WORD wMax, const int nValue )
	{
		int nOld = int(wNow);

		int nNew = nOld + nValue;

		//	WORD 형 값의 영역 이내로 제한.
		if ( nNew < 0 )					nNew = 0;
		if ( nNew > USHRT_MAX )			nNew = USHRT_MAX;

		wNow = nNew;
		if ( wNow >= wMax )				wNow = wMax;

		WORD wDX = (WORD) abs(nOld-int(wNow));
		return wDX;
	}

	//	Note : 파라메타 변경.
	//		계산시에는 int 형으로 계산을 수행하여 연산시 손실을 막는다.
	//		값 적용시에는 WORD 형 값의 영역 이내로 제한.
	inline DWORD VARIATION ( DWORD &dwNow, const DWORD dwMax, const int nValue )
	{
		int nOld = int(dwNow);

		int nNew = nOld + nValue;

		//	WORD 형 값의 영역 이내로 제한.
		if ( nNew < 0 )					nNew = 0;

		dwNow = nNew;
		if ( dwNow >= dwMax )			dwNow = dwMax;

		DWORD dwDX = (DWORD) abs(nOld-int(dwNow));
		return dwDX;
	}

	inline const int VARIATION(int& valueTarget, const int maxValue, const int valueSource)
	{
		const int valueOld(valueTarget);
		int valueNew(valueTarget + valueSource);

		if ( valueNew < 0 )
			valueNew = 0;

		if ( valueNew > maxValue )
			valueNew = maxValue;

		valueTarget = valueNew;

		return ::abs(valueOld - valueNew);
	}

	//	날씨와 속성간 증폭율.
	float WEATHER_ELEMENT_POW ( EMELEMENT emElement, DWORD dwWeather, BOOL bWeatherActive );

	float WEATHER_BLOW_POW ( EMSTATE_BLOW emBlow, DWORD dwWeather, BOOL bWeatherActive );

	//	Note : 체력 같은 수치들을 변화율에 따라 갱신해준다.
	//		계산시에는 int 형으로 계산을 수행하여 연산시 손실을 막는다.
	//		값 적용시에는 WORD 형 값의 영역 이내로 제한.
	inline void UPDATE_POINT ( GLPADATA &sPOINT, float &fELP_VAR, const float fVAR, const WORD wLOW_LMT )
	{
		fELP_VAR += fVAR;
		int nNEWP = int(sPOINT.wNow) + int(fELP_VAR);
		fELP_VAR -= int(fELP_VAR);

		//	WORD 형 값의 영역 이내로 제한.
		if ( nNEWP < 0 )			nNEWP = 0;
		if ( nNEWP > USHRT_MAX )	nNEWP = USHRT_MAX;

		if ( nNEWP < wLOW_LMT )		nNEWP = wLOW_LMT;

		sPOINT.wNow = nNEWP;
		sPOINT.LIMIT ();
	}

	inline void UPDATE_POINT ( GLDWDATA &sPOINT, float &fELP_VAR, const float fVAR, const int nLOW_LMT )
	{
		fELP_VAR += fVAR;
		int nNEWP = int(sPOINT.nNow) + int(fELP_VAR);
		fELP_VAR -= int(fELP_VAR);

		//	int 형 값의 영역 이내로 제한.
		if ( nNEWP < 0 )
			nNEWP = 0;

		if ( nNEWP < nLOW_LMT )
			nNEWP = nLOW_LMT;

		sPOINT.nNow = nNEWP;
		sPOINT.LIMIT ();
	}

	//	Note : 체력 같은 수치들을 변화율에 따라 갱신해준다.
	//		계산시에는 int 형으로 계산을 수행하여 연산시 손실을 막는다.
	//		값 적용시에는 WORD 형 값의 영역 이내로 제한.
	inline void UPDATE_POINT ( DWORD &dwNowP, float &fELP_VAR, const DWORD MaxP, const float fVAR, const DWORD dwLOW_LMT )
	{
		fELP_VAR += fVAR;
		int nNEWP = int(dwNowP) + int(fELP_VAR);
		fELP_VAR -= int(fELP_VAR);

		//	WORD 형 값의 영역 이내로 제한.
		if ( nNEWP < 0 )				nNEWP = 0;
		if ( nNEWP < (int)dwLOW_LMT )	nNEWP = dwLOW_LMT;

		dwNowP = nNEWP;
		if ( dwNowP > MaxP )		dwNowP = MaxP;
	}

	inline void UPDATE_POINT ( WORD &wNowP, float &fELP_VAR, const WORD MaxP, const float fVAR, const WORD wLOW_LMT )
	{
		fELP_VAR += fVAR;
		int nNEWP = int(wNowP) + int(fELP_VAR);
		fELP_VAR -= int(fELP_VAR);

		//	WORD 형 값의 영역 이내로 제한.
		if ( nNEWP < 0 )			nNEWP = 0;
		if ( nNEWP > USHRT_MAX )	nNEWP = USHRT_MAX;

		if ( nNEWP < wLOW_LMT )		nNEWP = wLOW_LMT;

		wNowP = nNEWP;
		if ( wNowP > MaxP )			wNowP = MaxP;
	}

    //! 이동속도
    float GetMoveBaseVelocity(float StateMove, float SkillMove, float ItemMove, float OptionMove);
    float GetMoveVelocity(
        BOOL bRun, // 상태
        float RunVelo,  // 뛰기 속도
        float WalkVelo, // 걷기 속도
        bool bUseBoost,  // Boost 사용여부
        float MoveVelo, // +이동속도
        float ItemVelo, // +아이템 이동속도
        float MoveVeloLimitMin,  // 최소 이동속도;
        float MoveVeloLimitMax,  // 최대 이동속도;
        bool bCdmSafe = false,  // Club Death Match
        float CdmSpeed = 1.0f // Club Death Match safe time speed
    ); 

    //! 공격속도
    float GetAttackVelocity(float AttackVelo, float fAddAttackVelo);

}; // namespace GLOGICEX

enum EMBEGINATTACK_FB
{
	EMBEGINA_OK		= 0,
	EMBEGINA_SP		= 1,
	EMBEGINA_ARROW	= 2,
	EMBEGINA_BULLET = 3,
};

typedef std::list<DWORD>	            SUMMONIDLIST;
typedef SUMMONIDLIST::iterator		    SUMMONID_ITER;

typedef std::list<DWORD>	            LANDEVENTIDLIST;
typedef LANDEVENTIDLIST::iterator		LANDEVENTID_ITER;

//Serialization에 포함되어야 할 항목 체크용 매크로
#define SERIALIZE

//Serialization - 일단 다 넣자

//	Note : 변수 추가시에. RESET_DATA() 함수에 변수 초기화 넣어야됨.
//
struct GLCHARLOGIC : public SCHARDATA2
//struct GLCHARLOGIC : public ic::CRttiObject< GLCHARLOGIC, SCHARDATA2, 1, ic::EInternal >
{
private:
	typedef std::map< DWORD, EMSKILL_VALIDATION >		SKILL_VALIDATION_MAP;
	typedef SKILL_VALIDATION_MAP::iterator				SKILL_VALIDATION_MAP_ITER;
	typedef SKILL_VALIDATION_MAP::const_iterator		SKILL_VALIDATION_MAP_CITER;

public:
	DWORD		m_dwGlobalID;						//해당 캐릭터의 서버군에서 유효한 고유ID

	SNATIVEID	m_idACTIVESKILL;				//	현제 액티브된 스킬.
	SNATIVEID	m_idIllusionSKILL;

	EMCHARINDEX		m_CHARINDEX;			// 직업+성별
	SKILL::DELAY_MAP			m_SKILLDELAY;			// 사용된 스킬의 딜래이.

	SSKILLFACT	m_sSKILLFACT[SKILLFACT_SIZE];	// 스킬효과들
	SSKILLFACT	m_sPETSKILLFACT[MAX_PETSKILLSLOT]; //  팻의 보조스킬효과

	VEC_LANDEFF_VALUE	m_vecLandEffect;		//  지형 효과들
	VEC_MAP_EFFECT		m_vecMapEffect;			//	맵에 적용되는 효과들

	DWORD				m_dwActionLimit;					//	행동 액션 제한 사항	
	DWORD				m_dwTransformSkill;				//  변신 스킬 넘버
	SNATIVEID			m_IdSkillTransform;				// 변신 주체 스킬 인덱스

    //SUMMONIDLIST        m_dwSummonGUIDList;		// 소환수 GUID 리스트
    LANDEVENTIDLIST     m_dwLandEventList;		// 랜드 이벤트 GUID 리스트

	DWORD				m_dwHOLDBLOW;					//	상태이상이 방지됨.
	bool					m_bSTATEBLOW;					//	상태이상이 하나 이상 발생했을때.
	SSTATEBLOW		m_sSTATEBLOWS[EMBLOW_MULTI];	//	상태이상.	

	SQITEM_FACT			m_sQITEMFACT;				//	? 아이템 적용값.
	SEVENT_FACT			m_sEVENTFACT;				//	이벤트 ? 아이템 적용값.
	SPKCOMBO_COUNT		m_sPKCOMBOCOUNT;

	STAUNT					m_sTaunt;						// 도발 효과;
	SHALLUCINATE			m_sHALLUCINATE;			// 환독 효과.
	SRELEASE_ENDURE	m_sRELEASE_ENDURE;		// 피격시 효과 해제.
	SINVISIBLE				m_sINVISIBLE;					// 은신 및 은신감지.
	SRECVISIBLE			m_sRECVISIBLE;				// 은신 감지.
	SDOMINATE				m_sDOMINATE;				// 지배 효과.
	SSTIGMA				m_sSTIGMA_USER;			// 낙인 효과 - 사용.
	SSTIGMA				m_sSTIGMA_TAKER;			// 낙인 효과 - 적용.
	SLINKHP					m_sLINKHP;						// 고리 효과	
	SINCREASEEFF			m_sIncreaseEff;				// 강화 효과;
	SIGNORED_DAMAGE	m_sIgnoredDamage;			// 데미지 무시(무시);
	SEXPERIENCERATE	m_ExperienceRate;			// 랜덤 경험치 증가율;
	SVIEWRANGERATE	m_sRateViewRange;			// 시야 제한;
	SAIRBORNE				m_sAirborne;					// 에어본;
	SCLONE				m_sClone; //복제 효과

	CSkillCAIdx			m_SkillCAIndex;			// 반격리스트

	float		m_fHP_TOTALRATE;				// 총 증가율
	float		m_fMP_TOTALRATE;
	float		m_fSP_TOTALRATE;

	float		m_fHP_RATE;						// active 스킬의 HP 증폭율.
	float		m_fMP_RATE;						// active 스킬의 MP 증폭율.
	float		m_fSP_RATE;						// active 스킬의 SP 증폭율.

	float		m_fSTATE_MOVE_RATE;				// 상태 이상 이동속도. ( 1.0f == 100% )
	float		m_fSTATE_MOVE_VALUE;			// 이동 속도 (탈것과 동일한 수치 로직)
	float		m_fSTATE_DELAY;					// 상태 이상 딜래이. ( 1.0f == 100% ) (+)
	float		m_fSTATE_DAMAGE;				// 상태 이상 대미지 증가.	

	float		m_fSKILL_LIFE_INC;				//  스킬시간 증감
	float		m_fSKILL_MOVE;					// 스킬 이동속도.
	float		m_fOPTION_MOVE;					// 추가적인 이동속도. 월드에디터에서 사용.

	float		m_fSUM_RANGE_PSY_LONG;			// 물리 사격형 공격 가능거리 합산. ( 사정거리, 적용영역 ) 
	float		m_fSUM_TARRANGE_ALL;			// 사정거리 증가 옵션 ( 전체 스킬 타입 ) 
	float		m_fSUM_APPLYRANGE_ALL;			// 적용영역 증가 옵션 ( 전체 스킬 타입 )

	float		m_fDamageRate;					// 공격력 변화율 (스킬 데미지 + 아이템). EMSPECA
	float		m_fDefenseRate;					// 방어력 변화율. EMSPECA
	float		m_fHitRate;						// 명중률 변화율  EMSPECA
	float		m_fAvoidRate;					// 회피율 변화율  EMSPECA

	float		m_fBasicRateHP;					// HP 변화율 (펫스킬효과 제외 수치);
	float		m_fVariationRateHP;             // HP 변화율 (전체 수치에서 %로 적용);
	float		m_fVariationRateMP;             // MP 변화율;
	float		m_fVariationRateSP;             // SP 변화율;
	
	float		m_fATTVELO;						// 스킬 공격속도 변화.

	float		m_fSKILLDELAY;

	float		m_fEXP_RATE;					// 경험치 수신율.
	float		m_fQItem_EXP_RATE;				// QItem 경험치 수신율.
	float		m_fSkill_EXP_RATE;				// 스킬 경험치 수신율.
	float		m_fRANDOM_BOX_EXP_RATE;			// 랜덤박스 경험치 수신율.
	float		m_fGENITEM_RATE;				//	아이템 발생율.
	float		m_fGENMONEY_RATE;				//	금액 발생율.
	
	int			m_nHP_ADD_SKILL;		//  HP 증감. EMSPECA
	int			m_nMP_ADD_SKILL;		//  MP 증감. EMSPECA
	int			m_nSP_ADD_SKILL;		//  SP 증감. EMSPECA
	int			m_nCP_ADD_SKILL;		//  CP 증감. EMSPECA

	int			m_nPotion_HP_Inc_SKILL; // HP 포션 회복량 EMSPECA
	int			m_nPotion_MP_Inc_SKILL; // MP 포션 회복량 EMSPECA
	int			m_nPotion_SP_Inc_SKILL; // SP 포션 회복량 EMSPECA
	
	int			m_nPA_ADD;				// PA 증감 EMSPECA
	int			m_nMA_ADD;				// MA 증감 EMSPECA
	int			m_nSA_ADD;				// SA 증감 EMSPECA

	float		m_fVariationApplyRateHP;		// HP 변화량 적용률 EMSPECA
	float		m_fVariationApplyRateMP;		// MP 변화량 적용률 EMSPECA
	float		m_fVariationApplyRateSP;		// SP 변화량 적용률 EMSPECA
	
	int			m_nGET_CP_VALUE;					// 획득 CP 증가량
	
    // 주의: 변화량 계산후에 나온 소수점이하의 값들을 임시로 보관하기 위한 변수입니다.
	float		m_fTempVarHP;
	float		m_fTempVarMP;
	float		m_fTempVarSP;
	float		m_fTempVarCP;
	float		m_fCP_Timer;

	float		m_fJumpRange;

	float		m_fSUMMONACTIVETIME;		// 소환시간 증가.

	int			m_sumPowerAttribute[SKILL::EMAPPLY_NSIZE]; // 격투/사격/마력;
	int			m_powerAttack;			    	//	기본 공격력.
	int			m_powerDefence;					//	기본 방어력.
	
	int			m_nHP_ADD;				//  HP 증감.  EMIMPACTA
	int			m_nMP_ADD;				//  MP 증감.  EMIMPACTA
	int			m_nSP_ADD;				//  SP 증감.  EMIMPACTA
	int			m_nCP_ADD;				//  CP 증감.  EMIMPACTA
	
	int			m_nSUM_PIERCE;			//	관통 정도 합산.

	int			m_nHIT;					//	명중율.
	int			m_nSUM_HIT;				//	명중율 + 스킬효과. EMSPECA, EMFOR

	int			m_nAVOID;					//	회피율.
	int			m_nSUM_AVOID;			//	회피율 + 스킬효과. EMSPECA, EMFOR

	int			m_nDEFENSE_BODY;		//	방어력 ( DP + DEX+계수 ).
	int			m_nDEFENSE;				//	방어력 ( DP + DEX*계수 + ITEM_DEF ) * 공간적응 보정.
	int			m_nDEFENSE_SKILL;		//	방어력 ( DP + DEX*계수 + ITEM_DEF ) * 공간적응 보정 + 스킬. EMSPECA

	bool		m_bSTATE_PANT;                  //	상태이상 헐떡임.
	bool		m_bSTATE_STUN;                  //	상태이상 기절.
	bool		m_bSTATE_CONTROL;				//	캐릭터 컨트롤 상태.

	bool		m_bUseArmSub;					// 극강부를 위한 보조 무기 사용여부	

	bool		m_bActionType[SKILL::EMACTION_TYPE_NSIZE];	// 버프효과 UI 확인
	bool		m_bSkillFactsSpecial[EMSPECA_NSIZE];	// 걸려있는 스페셜 효과;
	bool		m_bSafeZone;

	DEFENSE_SKILL		m_sDefenseSkill;				//  발동형 스킬
	SCHARSTATS          m_sSUMSTATS;                    //	기본스텟 + 장비옵션 Stats.
	SCHARSTATS          m_sSUMSTATS_SKILL;              //	기본스텟 + 장비옵션 +스킬 버프 Stats.

	SRESIST				m_sSUMRESIST;					//	저항값. ( 아이템 + 패시브 )
	SRESIST				m_sSUMRESIST_SKILL;			// 스킬 저항값 ( 아이템 + 패시브 + 버프 ) 
	SSUM_ITEM			m_sSUMITEM;						// 아이템 가산값. 
	SPASSIVE_SKILL_DATA	m_sSUM_PASSIVE;		// 패시브 가산값.	

	WORD				m_wACCEPTP;						// 무기 아이템에 능력제한 허용 수치.
	WORD				m_wACCEPTP_SKILL;				// 무기 아이템에 능력제한 허용 수치 스킬 보너스. EMSPECA
	WORD				m_wSUM_DisSP;					// 소모 SP 합산.
	WORD				m_wATTRANGE;					// 공격 가능 거리.
	WORD				m_wATTRANGE_SKILL;				// 공격 가능 거리 스킬 보너스. EMSPECA
	WORD				m_wSUM_ATTRANGE;				// 공격 가능 거리.

	GLDWDATA			m_gdDAMAGE;					//	기본 공격력. ( AP + 페시브 스킬로 가산된 공업 )
	GLDWDATA			m_gdDAMAGE_SKILL;			//	기본 공격력 + temp 스킬효과. EMSPECA
	GLDWDATA			m_gdDAMAGE_BASIC;			//	기본 공격력 + 무기공업 + (격투치or사격치or마력치).	

	DAMAGE_SPEC			m_sDamageSpec;					//	데미지 흡수 및 반사
	
	SDURATION_DAMAGE	m_sDurDamage[EMELEMENT_MAXNUM];	//	지속 데미지
	
	GLITEM_ATT	m_emITEM_ATT;					//	아이탬 공격 속성.
	EMANI_SUBTYPE		m_emANISUBTYPE;					//	Item 착용에 따른 에니메이션 종류.
	
	EMANI_MAINTYPE		m_emANIMAINSKILL;				//	스킬에 따른 에니메이션.
	EMANI_SUBTYPE		m_emANISUBSKILL;				//	스킬에 따른 에니메이션.
	EMANI_MAINTYPE		m_emANIVehicleSKILL;	    	//	스킬에 따른 에니메이션.

	SITEM*				m_pITEMS[SLOT_TSIZE];

	

	MAPPLAYHOSTILE		m_mapPlayHostile;				//	적대 행위자. ( 쌍방 )

	EMPOSTBOXID_TYPE	m_emPOSTBOXIDTYPE;				//  사용중인 우편함 ID 타입
	SNATIVEID			m_sPOSTBOXID;					//  사용중인 우편함 ID
	

	// 실제로 타이틀은 TitleManager 클래스에서 관리함
	// 캐릭터 조회 시 잠깐 넣어두는 공간으로 사용 중인데,
	// 현재 선택된 타이틀을 여기에 넣어두는 것도 무방할듯..
	char				m_szTitle[ EM_TITLEID_MAXLENGTH ];

	CSkillEffSet		m_EffSkillVarSet;

	BOOL				m_bParty;

protected:
	EMVEHICLE_WHETHER	m_emVehicle;					//! 탈것 활성화 여부
    float				m_fVehicleSpeedRate;
    float				m_fVehicleSpeedVol;

	CHARACTER_POINT		m_PointShopPoint;		//! PointShop point

	WORD				m_ItemGarbagePosX;		//	휴지통 사용시 아이템 위치 저장
	WORD				m_ItemGarbagePosY;

    float               m_fMoveVeloLimit_Min;   // 최저 이동속도 제한;
    float               m_fMoveVeloLimit_Max;   // 최고 이동속도 제한;


private:
	// 스킬 유효성 맵;
	SKILL_VALIDATION_MAP	m_mapValidSkill;


public:
	GLCHARLOGIC();
	virtual ~GLCHARLOGIC();

    void RESET_DATA();
    VOID RegistProperty();
	void RESET_SKILL_DATA();

public:

	CSkillCAIdx* GetSkillCA() { return &m_SkillCAIndex; }

	void		 GetEffList( DWORD dwSlot, BYTE* pSSPECA, BYTE* pSIMA );

	inline CHARACTER_POINT PointShopPointGet() const { return m_PointShopPoint; }
	__int64 PointShopPoint() const { return m_PointShopPoint.Total(); }
	__int64 PointShopBattlePoint() const { return m_PointShopPoint.BattlePoint(); }

    inline bool IsCheckedSkillFlagSpecial(const DWORD dwFlag) const { return m_bSkillFactsSpecial[dwFlag]; }

	float GetQuestionTime()			{ return m_sQITEMFACT.fTime; }
	float GetPKComboCountTime()		{ return m_sPKCOMBOCOUNT.fTime; }
    
    //! 탈것 활성화 여부
    EMVEHICLE_WHETHER VehicleState() const { return m_emVehicle; }
    float VehicleSpeedRate() const { return m_fVehicleSpeedRate; }
    void VehicleSpeedRateSet(float Rate) { m_fVehicleSpeedRate=Rate; }
    float VehicleSpeedVol() const { return m_fVehicleSpeedVol; }
    void VehicleSpeedVolSet(float Vol) { m_fVehicleSpeedVol=Vol; }

	inline bool IsEntranceExitSkillApply() { return RF_ENTRANCE_EXIT(m_EffSkillVarSet).IsOn(); }

public:
	EMCROW GETCROW () const			{ return CROW_PC; }
	WORD GETLEVEL () const			{ return m_wLevel; }

	WORD GetPow() const				{ return m_sSUMSTATS.wPow; }
	WORD GetDex() const				{ return m_sSUMSTATS.wDex; }
	WORD GetSpi() const				{ return m_sSUMSTATS.wSpi; }
	WORD GetStr() const				{ return m_sSUMSTATS.wStr; }
	WORD GetSta() const				{ return m_sSUMSTATS.wSta; }

	// 스킬 버프가 포함된 스텟
	WORD GetPow_Skill() const		{ return m_sSUMSTATS_SKILL.wPow; }
	WORD GetDex_Skill() const				{ return m_sSUMSTATS_SKILL.wDex; }
	WORD GetSpi_Skill() const				{ return m_sSUMSTATS_SKILL.wSpi; }
	WORD GetStr_Skill() const				{ return m_sSUMSTATS_SKILL.wStr; }
	WORD GetSta_Skill() const				{ return m_sSUMSTATS_SKILL.wSta; }

	WORD GetAddedPow() const		{ return m_sStats.wPow; }
	WORD GetAddedDex() const		{ return m_sStats.wDex; }
	WORD GetAddedSpi() const		{ return m_sStats.wSpi; }
	WORD GetAddedStr() const		{ return m_sStats.wStr; }
	WORD GetAddedSta() const		{ return m_sStats.wSta; }
	
	const int GETHP () const			{ return m_sHP.nNow; }
	const int GETMAXHP () const			{ return m_sHP.nMax; }
    float GETMAXHPRATE() const      { return m_sLINKHP.fRef; }
    void  SETMAXHPRATE( float fRate ) { m_sLINKHP.fRef = fRate; }

	int GETMP () const				{ return m_sMP.nNow; }
	int GETMAXMP () const			{ return m_sMP.nMax; }

	int GETSP () const				{ return m_sSP.nNow; }
	int GETMAXSP () const			{ return m_sSP.nMax; }

	int GETCP () const				{ return m_sCP.wNow; }
	int GETMAXCP () const			{ return m_sCP.wMax; }

	LONGLONG GETEXP() const			{ return m_sExperience.lnNow; }
	LONGLONG GETMAXEXP() const		{ return m_sExperience.lnMax; }

	int GETHIT () const				{ return m_nSUM_HIT; }
	int GETAVOID () const			{ return m_nSUM_AVOID; }
	// RM# 1873 [9/14/2015 gbgim];
	// 현재 아래의 두 함수는 UI 출력을 위해서만 사용중;
	float GetHitRate() const		{ return static_cast<float>(m_nSUM_HIT)/GLCONST_CHAR::fHitRatePercentUnit; }
	float GetAvoidRate() const		{ return static_cast<float>(m_nSUM_AVOID)/GLCONST_CHAR::fAvoidRatePercentUnit; }
	EMBRIGHT GETBRIGHT () const		{ if(m_nBright<0) return BRIGHT_DARK; return BRIGHT_LIGHT; }
	int GETBRIGHTPER () const		{ return m_nBright; }
	int GETFORCE_LOW () const		{ return m_gdDAMAGE_BASIC.nNow; }
	int GETFORCE_HIGH () const		{ return m_gdDAMAGE_BASIC.nMax; }
	int GETDEFENSE () const			{ return m_nDEFENSE_SKILL; }
	
	float GETSUM_RANGE_PSY_LONG () const { return m_fSUM_RANGE_PSY_LONG; }		//	일반공격일때만 가산.
	
	float GETSUM_TARRANGE_ALL () const { return m_fSUM_TARRANGE_ALL; }
	float GETSUM_APPLYRANGE_ALL () const { return m_fSUM_APPLYRANGE_ALL; }

	int GETSUM_PIERCE () const		{ return m_nSUM_PIERCE; }
	
	WORD GETBODYRADIUS () const		{ return GLCONST_CHAR::wBODYRADIUS; }
	WORD GETATTACKRANGE () const	{ return m_wSUM_ATTRANGE; }	

	const int GETVISIBLESIGHT(void) const { return int(m_sRateViewRange * MAX_VIEWRANGE); }
	const float GETVISIBLESIGHT_RATE(void) const { return m_sRateViewRange.fRate; }

	FLOAT GetExpMultiple() const	{ return m_sSUMITEM.fExpMultiple; }

	const int GETSUMMONDAMGE ();

	const SCHARSTATS& GETSTATS_ADD () const;
	const SCHARSTATS& GETSTATS_ITEM () const;

	void OptionMoveUp();
	void OptionMoveDown();

	void SetUseArmSub( BOOL bSub )	{ m_bUseArmSub = bSub; }
	const bool IsUseArmSub(void) const		{ return m_bUseArmSub; }

	const EMSLOT GetCurRHand(void) const;
	const EMSLOT GetCurLHand(void) const;

	BOOL IsCurUseArm( EMSLOT emSlot );

	bool IsDefenseSkill()				{ return m_sDefenseSkill.m_bActive; }
	bool IsDefenseSkill( const SNATIVEID& sSkillID ) { return m_sDefenseSkill.m_bActive ? (m_sDefenseSkill.m_dwSkillID == sSkillID) : false; }	
	void SetDefenseSkill(bool bDefense)	{ m_sDefenseSkill.m_bActive = bDefense; }

	bool ReleaseCureEffect( DWORD dwCheckList );
	DWORD SkillEffectType( DWORD iReleaseType );

public:
	void SLOT_ITEM ( const SITEMCUSTOM &sItemCustom, EMSLOT _slot );
	void RELEASE_SLOT_ITEM ( EMSLOT _slot );

	const SITEMCUSTOM& GET_SLOT_ITEM(EMSLOT _slot) const { return m_PutOnItems[_slot]; }
	const SNATIVEID GET_SLOT_NID(EMSLOT _slot) const { return m_PutOnItems[_slot].GetNativeID(); }
	SITEM* GET_SLOT_ITEMDATA(EMSLOT _slot) { return m_pITEMS[_slot]; }
	BOOL VALID_SLOT_ITEM(EMSLOT _slot);
	BOOL VALID_SLOT_ITEM_ALLWEAPON ( EMSLOT _emSlot );

	virtual void HOLD_ITEM( const SITEMCUSTOM& sItemCustom );
	void RELEASE_HOLD_ITEM();
	const SITEMCUSTOM& GET_HOLD_ITEM() const { return m_PutOnItems[SLOT_HOLD]; }

    //! 마우스에 들고 있는 아이템이 있는가?
	BOOL HaveHoldItem() const { return GET_HOLD_ITEM().GetNativeID()!=NATIVEID_NULL(); }

	const GLITEM_ATT CONVERT_ITEMATT ( GLITEM_ATT emAttack );

	BOOL ISLONGRANGE_ARMS ();
	BOOL ISLONGRANGE_ARMS ( GLITEM_ATT emAttack );
	SKILL::EMAPPLY GetStatType();

	BOOL ISHAVEITEM ( SNATIVEID sNID, DWORD dwNum );
    DWORD GETHAVEITEM( SNATIVEID sNID );
	SITEM* FindRevive( EMSLOT* pemSlot = NULL );

	SITEM* GET_ELMT_ITEM ();	//	왼손, 오른손 아이템중에서 'BLOW' 속성이 있는 아이템을 반환.

	WORD GETSKILLRANGE_TAR ( const GLSKILL &sSKILL ) const;
	WORD GETSKILLRANGE_APPLY ( const GLSKILL &sSKILL, const WORD dwLEVEL ) const;

public:
	EMBEGINATTACK_FB BEGIN_ATTACK ( WORD wStrikeNum );
	BOOL VALID_LEVELUP () const;
	LONGLONG GET_LEVELUP_EXP () const;
	void LEVLEUP ( bool bInitNowExp = TRUE, bool bJumping = false, bool bCalExp = true );
	void STATSUP ( EMSTATS emStats );
	void STATSUP_CMD ( EMSTATS emStats, DWORD value);		//add addstats cmd

	bool RESET_STATS_SKILL ( const WORD* pResetStats=NULL );
	bool RESET_SKILL ();

	bool RESET_STATS ( const WORD* pResetStats=NULL );
	
	// Note : 미사용 (망각의 약 개편 이전)
	bool RESET_STATS_OLD ( const WORD wDIS=USHRT_MAX ); 

	void GET_EVENLY_DECREASE_STATS ( WORD* pOutResetStats, WORD wAmount );

protected:
	void SUM_ITEM ();
	void SUM_ITEM_BASIC ();
	bool SUM_ITEM_BASIC_WEAPON( EMSLOT emSLOT, float fDurability, const SITEMCUSTOM& sItemCustom );

	void SUM_ITEM_COSTUM ();
	void SUM_ITEM_SET ();
	void SUM_ITEM_ADDOP_LIMIT();
	void GLCHARLOGIC::SUM_PASSIVE_BASIC( PGLSKILL pSkill, WORD wLev );
	void GLCHARLOGIC::SUM_PASSIVE_ADDON( PGLSKILL pSkill, WORD wLev );
	void GLCHARLOGIC::SUM_PASSIVE_SPEC( PGLSKILL pSkill, WORD wLev );
	void SUM_PASSIVE ();
	void SUM_ADDITION ( float fCONFT_POINT_RATE );

	void RESET_PASSIVE ();

	bool SUM_ITEM_BASIC_CS( const SITEMCUSTOM& rItemCustom, float fDurability, EMSLOT eSlot);

public:
	void INIT_NEW_CHAR ( const EMCHARINDEX _emCIndex, const DWORD dwUserID, const DWORD dwServerID, const char* szCharName,
		const WORD _wSchool=0, const WORD wHair=0, const WORD wFace=0, const WORD _wHairColor=0, const WORD _wSex=0, const BOOL bJumping = false );

	BOOL INIT_DATA ( BOOL bNEW, BOOL bReGen, float fCONFT_POINT_RATE = 1.0f, bool bInitNowExp = TRUE, bool bJumping = false );

	void INIT_RECOVER ( int nRECOVER=30 );

public:
	void			PrevUPDATE_DATA(const float fElapsedTime);
	
	void			UPDATE_DATA_SetItemBonus( );
	void			UPDATE_DATA_SkillAddon( const SKILL::SIMPACT_ADDON_LEVEL& addon, int& nSUM_PA, int& nSUM_SA, int& nSUM_MA );
	void			UPDATE_DATA_SkillSpecial( const SSKILLFACT& sSKEFF, const SKILL::SSPEC_ADDON_LEVEL& spec, SDURATION_DAMAGE* sDurDamage, const DWORD dwSkillFactSlot, BOOL bClient = FALSE );
	void 			UPDATE_DATA_SkillFacts( float	fElapsedTime, BOOL bClient, int& nSUM_PA, int& nSUM_SA, int& nSUM_MA, SDURATION_DAMAGE* sDurDamage );
	void 			UPDATE_DATA_BlowMulti( float fElapsedTime, BOOL bClient );
	void 			UPDATE_DATA_QItemFact();
	void 			UPDATE_DATA_LandEffects();
	void 			UPDATE_DATA_MapEffects();
	void 			UPDATE_DATA_PetSkill( float fElapsedTime );
	void 			UPDATE_DATA_Stats( int& nSUM_PA, int& nSUM_SA, int& nSUM_MA );
	void 			UPDATE_DATA ( float fTime, float fElapsedTime, BOOL bClient=FALSE, float fCONFT_POINT_RATE=1.0f );
	void 			UPDATE_MAX_POINT ( float fCONFT_POINT_RATE, const bool bRATE_MAINTENANCE=true );
	bool 			UPDATE_QITEMFACT ( float fElapsedTime );
	bool			UPDATE_PKCOMBO ( float fElapsedTime );
	virtual void	UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel = FALSE, BOOL bForceEnd = FALSE, BOOL bOverLab = FALSE ){;} // 지속효과 만료 시 호출;
	virtual void	UPDATE_DATA_Additional(const float fElapsedTime){;} // Update_data 종료 후 호출;

	virtual void	Play_ScreenThrow( DWORD dwIndex, const STARGETID& sID ) {};
	virtual void	Stop_ScreenThrow( DWORD dwIndex ) {};

	virtual void	Play_CameraHit( WORD wEffNum ) {};
	virtual void	Stop_CameraHit( WORD wEffNum ) {};

	virtual void	End_FeignDeath( BOOL bForce ) {};

	virtual void	Play_Disguise( int iIdx, BOOL bOn ) {};
	virtual void	Stop_Disguise() {};

	virtual void	Play_ActiveDamage() {};
	virtual void	Stop_ActiveDamage() {};
	
	void Update_AccumulateHit( DWORD dwDamage, const EMCROW eCrow, const DWORD dwID );

public:
	EMSLOTCHECK_RESULT	CHECKSLOT_ITEM ( SNATIVEID sNativeID, EMSLOT emSlot );
	BOOL			ACCEPT_ITEM( const SITEMCUSTOM& sItemCustom );
	BOOL 			ISEMPTY_SLOT ( SNATIVEID sNativeID, EMSLOT emSlot );
	WORD			CALC_ACCEPTP ( const SITEMCUSTOM& sItemCustom );
	WORD 			CALC_ACCEPTP_EX ( const SITEMCUSTOM& sItemCustom );
	BOOL 			SIMPLE_CHECK_ITEM ( const SITEMCUSTOM& sItemCustom );
	BOOL			SIMPLE_CHECK_ITEM ( const SITEMCUSTOM& sItemCustom, EMINVEN_USE_ITEM& emMsg ); //아이템 조건확인 : 예외경우 enum값 emMsg에 대입 후 FALSE 리턴 
	BOOL 			SIMPLE_CHECK_ITEM ( const SITEMCUSTOM& sItemCustom, SNATIVEID sDisguiseID );
	BOOL 			SIMPLE_CHECK_ITEM ( const SITEMCUSTOM& sItemCustom, SNATIVEID sDisguiseID, EMINVEN_USE_ITEM& emMsg );
	virtual BOOL 	CHECK_ACTIVITY_POINT ( const SITEM& sItem ) { return TRUE; }
	virtual BOOL 	CHECK_CONTRIBUTION_POINT ( const SITEM& sItem ) { return TRUE; }

	// 모든 배운 스킬의 유효성을 체크하여 세팅한다;
	void						SET_VALIDATION_EXPSKILL_MAP ();

	// 특정 스킬의 유효성을 체크한다 ( 세팅된 맵에서 찾는다 );
	const EMSKILL_VALIDATION	VALIDATION_EXPSKILL ( const DWORD _dwSkillID );

private:
	// 실제로 유효성을 체크하는 함수이다 ( Private );
	const EMSKILL_VALIDATION	_ValidationSkill ( const SNATIVEID& _sSkillID );

public:	
	WORD RECEIVE_DAMAGE(const int nDamage);
		virtual void RECEIVE_DAMAGE_AFTER(void){;}
		void RELEASE_ENDURE_PROC(void);
	WORD RECEIVE_HEAL(const int nHeal);

public:
	BOOL RECEIVE_QITEMFACT ( const SNATIVEID &nidITEM );
	//	Memo :	이벤트로 인해 받는 ?아이템 효과를 설정한다.
	void RECEIVE_EVENTFACT_BEGIN ( const EMGM_EVENT_TYPE emType, const WORD wValue );
	//	Memo :	이벤트로 인해 받는 ?아이템 효과를 해제한다.
	void RECEIVE_EVENTFACT_END ( const EMGM_EVENT_TYPE emType );
	BOOL RECEIVE_PKCOMBO ( int nCount );

public:
	BOOL ISLEARNED_SKILL( SNATIVEID skill_id, WORD wLEVEL=0 );
	BOOL ISMASTER_SKILL ( SNATIVEID skill_id );
	SCHARSKILL* GETLEARNED_SKILL ( SNATIVEID skill_id );

	EMSKILL_LEARNCHECK CHECKLEARNABLE_SKILL ( SNATIVEID skill_id );
	EMSKILL_LEARNCHECK CHECKLEARNABLE_SKILL ( SNATIVEID skill_id, WORD wLevel );
	void LEARN_SKILL ( SNATIVEID skill_id );
	void LEARN_SKILL_SIMPLE( SNATIVEID skill_id );
	VOID LVLUP_SKILL ( SNATIVEID skill_id, WORD wToLevel );

	bool LEARN_SKILL_QUEST ( SNATIVEID skill_id );

	void LEARN_CHANGE_SKILL( const GLSKILL* pOrgSkill, WORD wLv );
	BOOL ISLEARNED_SKILLEX( SNATIVEID skill_id, SCHARSKILL& charSkill );
	void ChangeToOrgSkill(const SNATIVEID& rID, SNATIVEID& sOrgSkill );
	void CHANGE_CHARSKILL( const SCHARSKILL& OrgValue, SCHARSKILL& ChangeValue );
	void UPDATE_CHANGE_CHARSKILL( const SCHARSKILL& OrgValue );

	void SAVE_CURRENT_SLOT_INFO ( void );
	const bool CHANGE_CHARACTER_SLOT ( const WORD _wSlotIndex, const bool _bForce = false );

	const bool GET_CHARACTER_SLOT_LOCKSTATE ( const WORD _wSlotIndex ) const;
	const WORD GET_CURRENT_CHARACTER_SLOT ( void ) const;

	const bool IS_USEABLE_SKILL ( const SNATIVEID& sSkillID, const WORD wSkillLevel ) const;

public:
	virtual float GETCRITICALVELO();
	// 아래는 공속을 구하는건데, Update_data에서 매번 구하고 추가적으로 해당 함수를 콜하면서;
	// 추가 연산을 한다, 의도가뭐지;
	// 결국에 Update_data에서 구해낸 데이터는 다른곳에서 사용하지않고 아래서만한다;
	virtual float GETATTVELO();
	virtual float GETMOVEVELO();
	virtual float GETEXPVELO();	// 부가정보창 보여주기 위한 함수
	virtual float GETCRITICAL_ITEM();
	virtual float GETATT_ITEM(); // 절대치 증가
	virtual float GETMOVE_ITEM(); // 절대치 증가 % 

	virtual float GETCRITICAL_TOTAL(); // GETCRITICALVELO() + GETCRITICAL
	virtual float GETATT_TOTAL(); // GETATTVELO() + GETATT_ITEM()
	virtual float GETMOVE_TOTAL(); // GETMOVEVELO() + GETMOVE_ITEM() 

	virtual float	GetInc_SkillDurationTAdd(WORD wGrade)	{ if ( wGrade < 0 || SKILL::MAX_GRADE <= wGrade ) { return 0.f; } return m_sSUMITEM.fInc_SkillDurationTAdd[wGrade]; }
	virtual float	GetIncR_SkillDamageAdd(WORD wGrade)	{ if ( wGrade < 0 || SKILL::MAX_GRADE <= wGrade ) { return 0.f; } return m_sSUMITEM.fIncR_SkillDamageAdd[wGrade]; }
	virtual float	GetDecR_SkillDelayT(WORD wGrade)	{ if ( wGrade < 0 || SKILL::MAX_GRADE <= wGrade ) { return 0.f; } return m_sSUMITEM.fDecR_SkillDelayT[wGrade]; }
	virtual float	GetIncR_CriticalDamage()	{ return m_sSUMITEM.fIncR_CriticalDamage; }
	virtual float	GetIncR_CrushingBlow()		{ return m_sSUMITEM.fIncR_CrushingBlowDamage; }
	virtual float	GetIncR_LifePerKill()		{ return m_sSUMITEM.fIncR_LifePerKill; }
	virtual float	GetIncR_MPPerKill()		{ return m_sSUMITEM.fIncR_MPPerKill; }
	virtual float	GetIncR_SPPerKill()		{ return m_sSUMITEM.fIncR_SPPerKill; }
	virtual float	GetIncR_HPMPSPPerKill()	{ return m_sSUMITEM.fIncR_HPMPSPPerKill; }
	virtual float	GetIncR_AllMPRatioToAttack()		{ return m_sSUMITEM.fIncR_AllMPRatioToAttack; }
	virtual float	GetIncR_AllSPRatioToHp()			{ return m_sSUMITEM.fIncR_AllSPRatioToHp; }
	virtual float	GetIncR_AllHitRatioToMelee()		{ return m_sSUMITEM.fIncR_AllHitRatioToMelee; }
	virtual float	GetIncR_AllHitRatioToRange()		{ return m_sSUMITEM.fIncR_AllHitRatioToRange; }
	virtual float	GetIncR_AllHitRatioToMagic()		{ return m_sSUMITEM.fIncR_AllHitRatioToMagic; }

public:
	//	Note : 적대자 등록. ( pk 관련 ),	true : 신규 등록시, false : 기존 등록 갱신시.
	bool ADD_PLAYHOSTILE ( const DWORD dwCHARID, const BOOL bBAD, const float fTime );
	bool DEL_PLAYHOSTILE ( const DWORD dwCHARID );
	void DEL_PLAYHOSTILE_ALL ();

public:
	//	Note : 적대 리스트에 등록 되어 있는가?
	bool IS_PLAYHOSTILE ( DWORD dwCHARID );
	//	Note : 적대행위자?
	bool IS_HOSTILE_ACTOR ( DWORD dwCHARID );
	//	Note : 자신이 범죄자인가?
	bool ISOFFENDER ();

	bool ISPLAYKILLING ();

public:
	// Note : 남아있는 SKILLFACT 공간이 있는가
	bool IS_REMAIND_SKILLFACTARRAY ( EMSKILLFACT_TYPE emType );
	// Note : 해당 스킬이 중복되는가
	bool IS_DUPLICATE_SKILLFACT (const GLSKILL* pSkill, EMSKILLFACT_TYPE emType, WORD &wOutSlot );
    // Note : 해당 스킬을 가지고 있는가
	bool IS_HAVE_SKILLFACT ( const SNATIVEID skill_id, OUT WORD &wOutSlot );
    bool IS_HAVE_SKILLFACT (  const SNATIVEID skill_id, const WORD wLevel, OUT WORD &wOutSlot, const bool bOnlyEnable=true );

	const bool IS_HAVE_TYPES_IN_SKILLFACT(const EMIMPACT_ADDON emIMPACT_ADDON);
	const bool IS_HAVE_SPEC_IN_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON);
	const bool IS_HAVE_STYPES_IN_SKILLFACT(const SKILL::EMSPECIALSKILLTYPE emSSTYPE_ADDON);
	const bool IS_HAVE_SKILL_IN_SKILLFACT( const SNATIVEID& sNativeID );

public:
	enum
	{
		EGarbageCard,	// 휴지통 카드 사용
		EGarbageMoney,	// 게임 머니 사용
		EGarbagePoint,	// 인게임 캐시 사용
		EGarbageFail,	// 휴지통 이용 불가능
	};
	bool IsValidGarbageUse( const SNATIVEID& sDeleteItem, BYTE& nType, LONGLONG& nCost );

public:
	//	Note : 현제 pk 레벨을 알아본다. ( UINT_MAX 일경우 pk 해당사항 없음. )
	DWORD GET_PK_LEVEL ();

	//	Note : 죽을때 경험치 감소 pk 배수.
	float GET_PK_DECEXP_RATE ();
	std::string GET_PK_NAME ();
	DWORD GET_PK_COLOR ();
	float GET_PK_SHOP2BUY ();
	float GET_PK_SHOP2SALE ();

	DWORD GET_PK_ITEMDROP_NUM ();	

public:
	DWORD DOGRINDING(
        SITEMCUSTOM &sCusItem,
        const SITEM *pHold,
        SITEMCUSTOM& sHoldItemCustom,
        DWORD dwGrindingOption,
        BYTE& cRESULTGRADE);
	EMGRINDINGCONDITION GRINDING_CONDITION_CHECK ( const SITEMCUSTOM &sCusItem, const SITEM* pHold, const SITEMCUSTOM& sHoldItemCustom );
	WORD GRINDING_CONSUME_NUM( const SITEMCUSTOM &sCusItem, const SITEM* pHold, const SITEMCUSTOM& sHoldItemCustom );

	EMGRINDINGCONDITION GRINDING_TL_CONDITION_CHECK ( const SITEMCUSTOM &sCusItem, const SITEM* pHold, const SITEMCUSTOM& sHoldItemCustom, CTime time = 0 );
	void DOTLGRINDING( SITEMCUSTOM &sCusItem, const SITEM *pHold, BYTE& cRESULTGRADE, bool bReEnchant = false);

	BOOL	DoItemSkill( const PGLSKILL pSkill, WORD wEMLinkType, std::vector<sItemSkill>& vinfo );

public:
	BOOL ISSTATEBLOW() const  { return m_bSTATEBLOW; }
	
    //! Skill fact ------------------------------------------------------------
    void 				RESETSKEFF(size_t i);
    void 				RESETAGESKEFF(size_t i);
    void 				SetSkillFact(size_t i, const SSKILLFACT& Fact);
    void 				SetSkillFact(size_t i, BOOL Enable);
    const SSKILLFACT&	GetSkillFact(size_t i) const;
    SSKILLFACT&			GetSkillFact(size_t i);
    const SNATIVEID&	GetSkillFactId(size_t i) const;
    SNATIVEID&			GetSkillFactId(size_t i);
    float				GetSkillFactAge(size_t i) const;
    void				RemoveSkillFactBySkillID(const SNATIVEID& skill_id);
	void				RemoveSkillFactBySpecialForce(const EMSPEC_ADDON emSPEC_ADDON);
	void				RemoveSkillFactBySpecial(const EMSPEC_ADDON emSPEC_ADDON);
	void				RemoveSkillFactBySpecial(const EMSPEC_ADDON emSPEC_ADDON, int iSlotNum);
	void				RemoveSkillFactofSpecial(const SNATIVEID& skill_id, const EMSPEC_ADDON* emSPEC_ADDON);
	void				RemoveSkillFactofSimpact(const SNATIVEID& skill_id, const EMIMPACT_ADDON* emSPEC_ADDON);
	void				RemoveSkillFactofNormalBuf();
	void				RemoveSkillFactofNormalAll();

    void				SetFactTime(const float fVAR, const DWORD dwFLAG); // 봉주
    float               CheckEndureVaryInSPEC( const SKILL::EMACTION_TYPE emACTION_TYPE); //

	void 				DISABLEBLOW(int i)	  { m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE; }	
	void 				RESETAGEBLOW(int i)  { m_sSTATEBLOWS[i].fAGE = 0.0f; }

	void 				DISABLEALLLANDEFF();
	void 				SETENABLEALLLUNCHBOX( const BOOL bEnable );
	void 				ADDLANDEFF( SLANDEFFECT_VALUE& landEffect );
	void 				DELLANDEFF( SLANDEFFECT_VALUE& landEffect );

	void 				ADDMAPEFF( SMAP_EFFECT& mapEffect );
	void 				DELMAPEFF( SMAP_EFFECT& mapEffect );
	void 				DISABLEALLMAPEFF();

	void 				UPDATESKILLDELAY ( float fElapsedTime );
    bool				SET_SKILLDELAY_INC ( float fINCR, float fChance, DWORD dwFLAG, const SNATIVEID& skill_id );
	void				SET_IGNORED_DAMAGE(const int nIgnoreCount, const int nIgnoreDamage);
    void				SET_MAXHP_RATE ( const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF, float fMaxHpRate );
    void				SET_CP_DUR ( GLPADATA& sCP, float fValue, float& fTime );

	void				SETACTIVESKILL ( SNATIVEID skill_id );
	SNATIVEID			GETACTIVESKILL ()					{ return m_idACTIVESKILL; }
	EMSKILLCHECK		CHECHSKILL ( SNATIVEID skill_id, WORD wStrikeNum, EMVEHICLE_WHETHER emVehicle, DWORD dwCheckOption=0 );
	bool				CHECKPROBABILITYSKILL(const PGLSKILL pSkill, const WORD wLevel);
	BOOL				GetSkillLevel( const SNATIVEID& idSkill, SCHARSKILL& sSkill );


	virtual void		ACCOUNTSKILL ( const SCHARSKILL& sSkill, WORD wStrikeNum, bool bServer=false );
	void				ACCOUNTBIKESKILL( SNATIVEID skill_id, WORD wStrikeNum, bool bServer=false );
	
public:
	virtual void ReSelectAnimation() {}

public:
        // 아이템 내구도 , 수리
        virtual void DurabilityDropHit( unsigned int nDamage, BOOL bPvp/* = FALSE  */);
        virtual void DurabilityDropUseSkill();
        virtual void DurabilityDropDeath( BOOL bPvp = FALSE );
        virtual void ItemRepairing( EMSLOT _slot );
        virtual void Durabilitiy_INIT_DATA( EMSLOT slot );   // 성능이 하락하면 장비 정보를 갱신한다.
protected:
	virtual void SummonStateVisibleUpdate(void){}
private:
        float m_DurabilitySlotItemPerformance[SLOT_TSIZE];

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<SCHARDATA2>(*this);

		ar & m_dwGlobalID;
		ar & m_CHARINDEX;
	}
};

//	Note : 변수 추가시에. RESET_DATA() 함수에 변수 초기화 넣어야됨.
//
struct GLCROWLOGIC
{
	enum EMCONTROL
	{
		EMTARGET		= 0x00000001,
		EMATTACKABLE	= 0x00000002,
		EMTRACING		= 0x00000004,
		EMSTAY			= 0x00000008,
		EMJUMPTO		= 0x00000010,

		EMPREORDER		= 0x00000100,
	};

	DWORD				m_dwGlobalID;				//	생성 메모리 인덱스용 ( 해당 GLLandMan내에서 유니크함 )
	SNATIVEID			m_sNativeID;				//	Crow NativeID.
	SCROWDATA*		m_pCrowData;				//	Crow 기본 정보.

	

	SSKILLFACT	m_sSKILLFACT[CROW_SKILLFACT_SIZE];	//	스킬효과들.
	DWORD			m_dwHOLDBLOW;					//	상태이상이 방지됨.

	SSTATEBLOW	m_sSTATEBLOWS[EMBLOW_MULTI];	//	상태이상.

	float				m_fACTIONDELAY[SCROWDATA::EMMAXATTACK];	//	동작 cool time;
	float				m_fSTATE_MOVE_RATE;						//	상태 이상 이동속도. ( 1.0f == 100% )
	float				m_fSTATE_DELAY;							//	상태 이상 딜래이. ( 1.0f == 100% ) (+)
	float				m_fSTATE_DAMAGE;						//	상태 이상 대미지 증가.

	float				m_fDamageRate;							//	공격력 변화율.
	float				m_fDefenseRate;							//	방어력 변화율.

    // 주의: 변화량 계산후에 나온 소수점이하의 값들을 임시로 보관하기 위한 변수입니다.
	float				m_fTempVarHP;
	float				m_fTempVarMP;
	float				m_fTempVarSP;

	float               m_fMultipleHP;	                // HP 배수

	float				m_fATTVELO;						//	공격속도.
	float				m_fSKILL_MOVE;					//	스킬 이동속도.
	
	bool				m_bSkillFactsSpecial[EMSPECA_NSIZE];
	bool				m_bSTATE_PANT;                  //	상태이상 헐떡임.
	bool				m_bSTATE_STUN;                  //	상태이상 기절.	
	SINVISIBLE				m_sINVISIBLE;				// 은신.
	SRECVISIBLE			m_sRECVISIBLE;			// 은신감지.
	SHALLUCINATE			m_sHALLUCINATE;		// 환독 효과.
	SRELEASE_ENDURE	m_sRELEASE_ENDURE;	// 피격시 효과 해제.
	SDOMINATE				m_sDOMINATION;			// 지배 효과.
	SSTIGMA				m_sSTIGMA_USER;		// 낙인 효과 - 사용.
	SSTIGMA				m_sSTIGMA_TAKER;		// 낙인 효과 - 적용.
	SLINKHP					m_sLINKHP;					// 고리 효과.
	SINCREASEEFF			m_sIncreaseEff;			// 강화 효과;
	SIGNORED_DAMAGE	m_sIgnoredDamage;		// 데미지 흡수;
	SVIEWRANGERATE	m_sRateViewRange;		// 시야 제한;
	SAIRBORNE				m_sAirborne;				// 에어본;
	SCLONE				m_sClone;// 복제효과

	CSkillCAIdx			m_SkillCAIndex;			// 반격리스트
	
	float				m_fHPRestorationRate;			// HP 회복률;
	float				m_fMPRestorationRate;			// MP 회복률;
	float				m_fSPRestorationRate;			// SP 회복률;

	float				m_fHPRestorationRatePlus;		// HP 회복률 변화량;
	float				m_fMPRestorationRatePlus;		// MP 회복률 변화량;
	float				m_fSPRestorationRatePlus;		// SP 회복률 변화량;

	int					m_nSUM_HIT;						//	명중율 + 스킬효과.
	int					m_nSUM_AVOID;					//	회피율 + 스킬효과.

	int					m_nSUM_DEFENSE;				//	방어력 ( DP + POW*계수 + ITEM_DEF ) * 공간적응 보정 + 스킬.
	int					m_nDX_DAMAGE;					//	기본 공격력. + 스킬효과.

	int					m_nSUM_PIERCE;					//	추가 관통 정도.
	
	SRESIST				m_sSUMRESIST;				//	저항값.	
	
	DAMAGE_SPEC			m_sDamageSpec;					//	데미지 흡수 및 반사
	DEFENSE_SKILL		m_sDefenseSkill;				//  발동형 스킬

	SNATIVEID			m_idIllusionSKILL;

	SDURATION_DAMAGE	m_sDurDamage[EMELEMENT_MAXNUM];	//	지속 데미지 관리

	DWORD				m_dwActionLimit;				//	행동 액션 제한 사항

	DWORD				m_dwAType;						//	공격 속성 순번.
	
	DWORD				m_dwNowHP;						//	생명량.
	WORD				m_wNowMP;						//	정신량.
	WORD				m_wNowSP;						//	정신량.

	//
	//mjeon.CaptureTheField
	//
	DWORD				m_arrHP4School[SCHOOL_NUM];		//  CTF에서 각 학원별로 관리되는 인증기의 HP
	float					m_arrIncHp4School[SCHOOL_NUM];  //  CTF에서 각 학원별로 관리되는 인증기의 회복율    
	WORD				m_wSchoolAuthed;				//  CTF에서 현재 이 인증기를 인증한 학원 번호

	SNATIVEID			m_idACTIVESKILL;				//	활성 스킬.
	WORD				m_wACTIVESKILL_LVL;				//	활성 스킬  래벨.

    float               m_fMoveVeloLimit_Min;   // 최저 이동속도 제한;
    float               m_fMoveVeloLimit_Max;   // 최고 이동속도 제한;

	float				m_fInfluenceY;

	CSkillEffCrowSet	m_EffSkillVarSet;

	//	Note : 변수 추가시에. RESET_DATA() 함수에 변수 초기화 넣어야됨.
	//

	GLCROWLOGIC(void);
    virtual ~GLCROWLOGIC() {}

	void RESET_DATA();

	bool VALIDACTION();
	bool VALIDSKILL();
	bool VALIDATTACK();
	EMCROW GETCROW() const;
	WORD GETLEVEL() const;
	const char* GETNAME();
    const std::string GetNameStr() const;
	
	DWORD GETHP() const;
	DWORD GETMAXHP() const;
	float GETMAXHPRATE() const;
    void  SETMAXHPRATE( float fRate );
    void  SetMultipleHP( float fMultiple );
    float GetMultipleHP() const;

	int GETMP() const;
	int GETMAXMP() const;

	int GETSP() const;
	int GETMAXSP() const;

	int GETHIT() const;
	int GETAVOID() const;

	float GETHPRESTORATIONRATE() const;
	float GETMPRESTORATIONRATE() const;
	float GETSPRESTORATIONRATE() const;

	float GetHPRestorationRatePlus () const { return m_fHPRestorationRatePlus; }
	float GetMPRestorationRatePlus () const { return m_fMPRestorationRatePlus; }
	float GetSPRestorationRatePlus () const { return m_fSPRestorationRatePlus; }

	void SetHPRestorationRatePlus( float fRate );
	void SetMPRestorationRatePlus( float fRate );
	void SetSPRestorationRatePlus( float fRate );

	EMBRIGHT GETBRIGHT() const;

	int GETFORCE_LOW () const;
	int GETFORCE_HIGH () const;
	int GETDEFENSE () const;

	int GETSUM_PIERCE () const;

	CSkillCAIdx* GetSkillCA() { return &m_SkillCAIndex; }

	void		 GetEffList( DWORD dwSlot, BYTE* pSSPECA, BYTE* pSIMA );

	const int GETVISIBLESIGHT(void) const { return int(m_sRateViewRange * MAX_VIEWRANGE); }
	const float GETVISIBLESIGHT_RATE(void) const { return m_sRateViewRange.fRate; }

	WORD GETBODYRADIUS () const;
	WORD GETATTACKRANGE () const;

	WORD GETSKILLRANGE_TAR ( const GLSKILL &sSKILL ) const;
	WORD GETSKILLRANGE_APPLY ( const GLSKILL &sSKILL, const WORD dwLEVEL ) const;

	BOOL INIT_DATA ();	
	DWORD RECEIVE_DAMAGE(const int nDamage);
		virtual void RECEIVE_DAMAGE_AFTER(void){;}
		void RELEASE_ENDURE_PROC(void);
	DWORD RECEIVE_HEAL(const WORD wHeal);
	DWORD RECEIVE_DAMAGE_BY_SCHOOL(const int nDamage, const WORD wSchool);
	DWORD RECEIVE_HEAL_BY_SCHOOL(const int nHeal, const WORD wSchool){return 0;}

	void RESETSKEFF(size_t i );
	void DISABLEBLOW ( int i );

	void PrevUPDATE_DATA(const float fElapsedTime);
	void UPDATE_DATA(float fTime, float fElapsedTime, BOOL bClient=FALSE);		
	virtual void	UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel = FALSE ){;} // 지속효과 만료 시 호출; // 가상 함수 안됨; RanLogicClient/ReferChar에서 GLogic 사용;
	virtual void	UPDATE_DATA_Additional(const float fElapsedTime){;} // Update_data 종료 후 호출;

	void Update_AccumulateHit( DWORD dwDamage, const EMCROW eCrow, const DWORD dwID );

	void SET_IGNORED_DAMAGE(const int nIgnoreCount, const int nIgnoreDamage);

	void SETACTIVESKILL ( SNATIVEID skill_id, WORD wLevel );
	SNATIVEID GETACTIVESKILL ();
	EMSKILLCHECK CHECHSKILL ( DWORD dwAType, bool bNotLearn = false );
	void ACCOUNTSKILL ( WORD wStrikeNum );

	virtual float GETATTVELO();
	virtual float GETMOVEVELO();
    virtual float GETATT_TOTAL(); // GETATTVELO() + GETATT_ITEM()
    virtual float GETMOVE_TOTAL(); // GETMOVEVELO() + GETMOVE_ITEM() 

	bool IsDefenseSkill();
	void SetDefenseSkill(bool bDefense);

	inline bool IsCheckedSkillFlagSpecial(const DWORD dwFlag) const { return m_bSkillFactsSpecial[dwFlag]; }	
	inline bool IsEntranceExitSkillApply() { return RF_ENTRANCE_EXIT(m_EffSkillVarSet).IsOn(); }

    void RemoveSkillFactBySkillID(const SNATIVEID& skill_id);
	void RemoveSkillFactBySpecial(const EMSPEC_ADDON emSPEC_ADDON);
	void RemoveSkillFactofSpecial(const SNATIVEID& skill_id, const EMSPEC_ADDON* emSPEC_ADDON);

	void RemoveSkillFactofSimpact(const SNATIVEID& skill_id, const EMIMPACT_ADDON* emSPEC_ADDON);

    const bool IS_HAVE_SKILLFACT(const SNATIVEID& skill_id, OUT WORD& wOutSlot);
	const bool IS_HAVE_TYPES_IN_SKILLFACT(const EMIMPACT_ADDON emIMPACT_ADDON);
	const bool IS_HAVE_SPEC_IN_SKILLFACT(const EMSPEC_ADDON emSPEC_ADDON);
	const bool IS_HAVE_STYPES_IN_SKILLFACT(const SKILL::EMSPECIALSKILLTYPE emSSTYPE_ADDON);
	const bool IS_HAVE_SKILL_IN_SKILLFACT( const SNATIVEID& sNativeID );

    //! NPC에게 행동 가능한 거리인지
    bool ISNPC_INTERACTIONABLE(
        const D3DXVECTOR3& vReqPos,
        const D3DXVECTOR3& vCurPos,
        const float fReqRadius,
        int nNpcAction) const;

    const float GET_INTERACTION_RANGE() const { return (m_pCrowData->m_fRefenceActionableRange > 0.0f) ? m_pCrowData->m_fRefenceActionableRange : GLCONST_CHAR::fNPC_INTERACTION_RANGE; }

    //! Crow NativeID
    const SNATIVEID& GetNativeId(void) const { return m_sNativeID; }

	inline bool IsBoss()    { if(!m_pCrowData) return false; return m_pCrowData->IsBoss(); }
	inline bool IsPosHold() { if(!m_pCrowData) return false; return m_pCrowData->IsPosHold(); }
};

EMANI_SUBTYPE CHECK_ANISUB (const SITEM* pRHAND, const SITEM* pLHAND, const EMCHARINDEX _emClass );
EMANI_SUBTYPE CHECK_ATTACK_ANISUB (const SITEM* pRHAND, const SITEM* pLHAND, const EMCHARINDEX _emClass );

//! 아이템 획득 권한을 주기위한...
//! 데미지 로그를 기록하기 위한 구조체
struct SDAMAGELOG
{
	DWORD dwUserID; ///< User ID
	DWORD dwGaeaID; ///< 가이아 ID
	DWORD dwDamage; ///< 준 데미지	

	SDAMAGELOG () 
		: dwUserID(0)
		, dwGaeaID(0)
		, dwDamage(0)
	{
	}

	SDAMAGELOG ( DWORD dwuserid, DWORD dwgaeaid, DWORD dwdamage ) 
		: dwUserID(dwuserid)
		, dwGaeaID(dwgaeaid)
		, dwDamage(dwdamage)
	{
	}
};

typedef std::map<DWORD, SDAMAGELOG> DAMAGELOG;
typedef DAMAGELOG::iterator	        DAMAGELOG_ITER;

inline void AddDamageLog ( DAMAGELOG &cDamageLog, DWORD dwGaeaID, DWORD dwUserID, DWORD dwDamage )
{
	DAMAGELOG_ITER iter = cDamageLog.find ( dwUserID );
	DAMAGELOG_ITER iter_end = cDamageLog.end();
	
	if ( iter != iter_end )
	{
		SDAMAGELOG &sDamageLog = (*iter).second;
		if ( sDamageLog.dwUserID == dwUserID )	dwDamage += sDamageLog.dwDamage;
	}
	
	cDamageLog[dwUserID] = SDAMAGELOG(dwUserID,dwGaeaID,dwDamage);
}

//! 추가:2006-04-12 Jgkim
//! 파티 데미지 로그기록을 위한 구조체
struct SDAMAGELOGPARTY
{
	GLPartyID m_PartyID; ///< 파티 고유번호
	DWORD m_dwDamage; ///< 파티가 준 데미지

	SDAMAGELOGPARTY()
		: m_dwDamage(0)
	{
	}

	SDAMAGELOGPARTY(const GLPartyID& PartyID, DWORD dwDamage)
		: m_PartyID(PartyID)
		, m_dwDamage(dwDamage)
	{
	}
};
typedef std::map<GLPartyID, SDAMAGELOGPARTY> DAMAGELOGPARTY;
typedef DAMAGELOGPARTY::iterator		 DAMAGELOGPARTY_ITER;
inline void AddDamageLogParty(DAMAGELOGPARTY &cDamageLogParty, const GLPartyID& PartyID, DWORD dwDamage)
{
	if ( PartyID.isValidParty() == false )
		return;

	DAMAGELOGPARTY_ITER iter = cDamageLogParty.find(PartyID);
	if ( iter == cDamageLogParty.end() )
	{
		cDamageLogParty[PartyID] = SDAMAGELOGPARTY(PartyID, dwDamage);
		return;
	}

	SDAMAGELOGPARTY& sDamageLogParty = iter->second;
	if ( sDamageLogParty.m_PartyID != PartyID )
		return;

	sDamageLogParty.m_dwDamage += dwDamage;
}

inline bool CHECHSKILL_ITEM ( SKILL::GLSKILL_ATT emSKILL, GLITEM_ATT emITEM, const bool bHiddenWeapon )
{
	// 극강부 글러브 착용 시 스킬 사용 가능 유무;
	if ( emITEM == ITEMATT_EXTREME_GLOVE && bHiddenWeapon )
	{
		return true;
	}

	switch ( emSKILL ) 
	{
	case SKILL::SKILLATT_NOTHING:
		return ( emITEM==ITEMATT_NOTHING );
	case SKILL::SKILLATT_SWORD:
		return ( emITEM==ITEMATT_SWORD || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_SABER:
		return ( emITEM==ITEMATT_SABER || emITEM==ITEMATT_WEAPON_STR );	
	case SKILL::SKILLATT_SWORDSABER:
		return ( emITEM==ITEMATT_SWORD || emITEM==ITEMATT_SABER || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_DAGGER:
		return ( emITEM==ITEMATT_DAGGER );
	case SKILL::SKILLATT_SPEAR:
		return ( emITEM==ITEMATT_SPEAR || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_STICK:
		return ( emITEM==ITEMATT_STICK );
	case SKILL::SKILLATT_GWON:
		return ( emITEM==ITEMATT_GWON || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_BOW:
		return ( emITEM==ITEMATT_BOW || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_THROW:
		return ( emITEM==ITEMATT_THROW );	
	case SKILL::SKILLATT_PISTOL:
		return ( emITEM==ITEMATT_PISTOL || emITEM==ITEMATT_WEAPON_DEX );
	case SKILL::SKILLATT_RAILGUN:
		return ( emITEM==ITEMATT_RAILGUN || emITEM==ITEMATT_WEAPON_STR );
	case SKILL::SKILLATT_PORTALGUN:
		return ( emITEM==ITEMATT_PORTALGUN || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_GUN:
		return ( emITEM==ITEMATT_PISTOL || emITEM==ITEMATT_RAILGUN || emITEM==ITEMATT_PORTALGUN || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_RIFLE:
		return ( emITEM==ITEMATT_RAILGUN || emITEM==ITEMATT_PORTALGUN || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_SPI );
    case SKILL::SKILLATT_SIDE:
        return ( emITEM==ITEMATT_SIDE || emITEM==ITEMATT_WEAPON_STR );
    case SKILL::SKILLATT_DUALSPEAR:
        return ( emITEM==ITEMATT_DUALSPEAR || emITEM==ITEMATT_WEAPON_DEX );
    case SKILL::SKILLATT_THROWING_KNIFE:
        return ( emITEM==ITEMATT_THROWING_KNIFE || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_EXTREME_GLOVE:
		return ( emITEM==ITEMATT_EXTREME_GLOVE || emITEM==ITEMATT_WEAPON_STR || emITEM==ITEMATT_WEAPON_DEX || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_TRICK_STICK:
		return ( emITEM==ITEMATT_TRICK_STICK || emITEM==ITEMATT_WEAPON_SPI );
	case SKILL::SKILLATT_TRICK_BOX:
		return ( emITEM==ITEMATT_TRICK_BOX || emITEM==ITEMATT_WEAPON_DEX );
	case SKILL::SKILLATT_TRICK_WHIP:
		return ( emITEM==ITEMATT_TRICK_WHIP || emITEM==ITEMATT_WEAPON_STR );
	case SKILL::SKILLATT_ACTOR_SHIELD:
		return ( emITEM==ITEMATT_ACTOR_SHIELD || emITEM==ITEMATT_WEAPON_DEX );
	case SKILL::SKILLATT_ACTOR_HAMMER:
		return ( emITEM==ITEMATT_ACTOR_HAMMER || emITEM==ITEMATT_WEAPON_STR );
	case SKILL::SKILLATT_ACTOR_UMBRELLA:
		return ( emITEM==ITEMATT_ACTOR_UMBRELLA || emITEM==ITEMATT_WEAPON_SPI );
	};

	return false;
}

inline SKILL::GLSKILL_ATT ItemAtt2SkillAtt( GLITEM_ATT emItem, const DWORD _dwClass )
{
	switch( emItem ) 
	{
	case ITEMATT_NOTHING :
		return SKILL::SKILLATT_NOTHING;

	case ITEMATT_SWORD :
		return SKILL::SKILLATT_SWORD;

	case ITEMATT_SABER :
		return SKILL::SKILLATT_SABER;

	case ITEMATT_DAGGER :
		return SKILL::SKILLATT_DAGGER;

	case ITEMATT_SPEAR :
		return SKILL::SKILLATT_SPEAR;

	case ITEMATT_STICK :
		return SKILL::SKILLATT_STICK;

	case ITEMATT_GWON :
		return SKILL::SKILLATT_GWON;

	case ITEMATT_BOW :
		return SKILL::SKILLATT_BOW;

	case ITEMATT_THROW :
		return SKILL::SKILLATT_THROW;

	case ITEMATT_PISTOL :
		return SKILL::SKILLATT_PISTOL;

	case ITEMATT_RAILGUN :
		return SKILL::SKILLATT_RAILGUN;

	case ITEMATT_PORTALGUN :
		return SKILL::SKILLATT_PORTALGUN;

	case ITEMATT_SIDE :
		return SKILL::SKILLATT_SIDE;

	case ITEMATT_DUALSPEAR :
		return SKILL::SKILLATT_DUALSPEAR;

	case ITEMATT_THROWING_KNIFE :
		return SKILL::SKILLATT_THROWING_KNIFE;

	case ITEMATT_EXTREME_GLOVE :
		return SKILL::SKILLATT_EXTREME_GLOVE;

	case ITEMATT_TRICK_STICK :
		return SKILL::SKILLATT_TRICK_STICK;

	case ITEMATT_TRICK_BOX :
		return SKILL::SKILLATT_TRICK_BOX;

	case ITEMATT_TRICK_WHIP :
		return SKILL::SKILLATT_TRICK_WHIP;

	case ITEMATT_ACTOR_SHIELD :
		return SKILL::SKILLATT_ACTOR_SHIELD;

	case ITEMATT_ACTOR_HAMMER :
		return SKILL::SKILLATT_ACTOR_HAMMER;

	case ITEMATT_ACTOR_UMBRELLA :
		return SKILL::SKILLATT_ACTOR_UMBRELLA;

	case ITEMATT_WEAPON_STR:
		{
			if ( _dwClass & GLCC_FIGHTER )
				return SKILL::SKILLATT_GWON;
			else if ( _dwClass & GLCC_ARMS )
				return SKILL::SKILLATT_SABER;
			else if ( _dwClass & GLCC_ARCHER )
				return SKILL::SKILLATT_BOW;
			else if ( _dwClass & GLCC_SPIRIT )
				return SKILL::SKILLATT_SPEAR;
			else if ( _dwClass & GLCC_EXTREME )
				return SKILL::SKILLATT_EXTREME_GLOVE;
			else if ( _dwClass & GLCC_SCIENTIST )
				return SKILL::SKILLATT_RAILGUN;
			else if ( _dwClass & GLCC_ASSASSIN )
				return SKILL::SKILLATT_SIDE;
			else if ( _dwClass & GLCC_TRICKER )
				return SKILL::SKILLATT_TRICK_WHIP;
			else if ( _dwClass & GLCC_ETC )
				return SKILL::SKILLATT_NOTHING;
			else if ( _dwClass & GLCC_ACTOR )
				return SKILL::SKILLATT_ACTOR_HAMMER;
		}
		return SKILL::SKILLATT_NOTHING;

	case ITEMATT_WEAPON_DEX:
		{
			if ( _dwClass & GLCC_FIGHTER )
				return SKILL::SKILLATT_GWON;
			else if ( _dwClass & GLCC_ARMS )
				return SKILL::SKILLATT_SWORD;
			else if ( _dwClass & GLCC_ARCHER )
				return SKILL::SKILLATT_BOW;
			else if ( _dwClass & GLCC_SPIRIT )
				return SKILL::SKILLATT_SPEAR;
			else if ( _dwClass & GLCC_EXTREME )
				return SKILL::SKILLATT_EXTREME_GLOVE;
			else if ( _dwClass & GLCC_SCIENTIST )
				return SKILL::SKILLATT_PISTOL;
			else if ( _dwClass & GLCC_ASSASSIN )
				return SKILL::SKILLATT_DUALSPEAR;
			else if ( _dwClass & GLCC_TRICKER )
				return SKILL::SKILLATT_TRICK_BOX;
			else if ( _dwClass & GLCC_ETC )
				return SKILL::SKILLATT_NOTHING;
			else if ( _dwClass & GLCC_ACTOR )
				return SKILL::SKILLATT_ACTOR_SHIELD;
		}
		return SKILL::SKILLATT_NOTHING;

	case ITEMATT_WEAPON_SPI:
		{
			if ( _dwClass & GLCC_FIGHTER )
				return SKILL::SKILLATT_GWON;
			else if ( _dwClass & GLCC_ARMS )
				return SKILL::SKILLATT_SWORD;
			else if ( _dwClass & GLCC_ARCHER )
				return SKILL::SKILLATT_BOW;
			else if ( _dwClass & GLCC_SPIRIT )
				return SKILL::SKILLATT_SPEAR;
			else if ( _dwClass & GLCC_EXTREME )
				return SKILL::SKILLATT_EXTREME_GLOVE;
			else if ( _dwClass & GLCC_SCIENTIST )
				return SKILL::SKILLATT_PORTALGUN;
			else if ( _dwClass & GLCC_ASSASSIN )
				return SKILL::SKILLATT_THROWING_KNIFE;
			else if ( _dwClass & GLCC_TRICKER )
				return SKILL::SKILLATT_TRICK_STICK;
			else if ( _dwClass & GLCC_ETC )
				return SKILL::SKILLATT_NOTHING;
			else if ( _dwClass & GLCC_ACTOR )
				return SKILL::SKILLATT_ACTOR_UMBRELLA;
		}
		return SKILL::SKILLATT_NOTHING;

	default :
		return SKILL::SKILLATT_NOTHING;
	}
}

inline void GetRandomPos( D3DXVECTOR3* pDestPos, D3DXVECTOR3* pOrgPos, float fDist )
{
	D3DXVECTOR3			vDir;
	vDir.x = (((((float) rand())/RAND_MAX)*2.f)-1.f);
	vDir.y = (((((float) rand())/RAND_MAX)*2.f)-1.f);
	vDir.z = (((((float) rand())/RAND_MAX)*2.f)-1.f);

	D3DXVec3Normalize( &vDir, &vDir );

	pDestPos->x = pOrgPos->x + ( vDir.x * fDist );
	pDestPos->y = pOrgPos->y + ( vDir.y * fDist );
	pDestPos->z = pOrgPos->z + ( vDir.z * fDist );
} //GetRandomPos

#endif // GLOGIXEX_H_
