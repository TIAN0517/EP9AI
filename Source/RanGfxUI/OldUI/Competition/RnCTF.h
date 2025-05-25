#pragma once
#include "RnCompetitionObject.h"
#include "../Util/UIPage.h"
#include "../../../RanLogic/GLCompetition.h"
#include "../../../RanLogic/Character/GLCharDefine.h"
#include <boost/function.hpp>

//-----------------------------------------------------------------------------------------------//

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CSwapImage;
class CCTFBattle;
class CCTFHistory;
class CBasicTextButton;
class CBasicLineBoxSmart;
class CBasicButton;
class CSwapImage;
class CSkillImage;

class RnCTFPage : public RnCompetitionObject
{
private:
	struct SKILLIMAGE
	{
		CSkillImage*		pImage;
		CBasicLineBoxSmart* pLine;

		SKILLIMAGE()
			: pImage(NULL)
			, pLine(NULL)
		{
		}

		SKILLIMAGE(CSkillImage* _pImage, CBasicLineBoxSmart* _pLine)
			: pImage(_pImage)
			, pLine(_pLine)
		{
		}
	};

	typedef std::vector<SKILLIMAGE>	 VEC_SKILLIMAGE;
	typedef VEC_SKILLIMAGE::iterator VEC_SKILLIMAGE_ITER;
protected:
	enum
	{
		CTF_PAGE = RNCOMPETITION_NEXT,
		CTF_BATTLE_BUTTON,
		CTF_HISTORY_BUFF_BEGIN,
	};

public:
	RnCTFPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~RnCTFPage();

public:
	void	             CreateSubControl ();

public:
	virtual void         Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void         TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void		 SetVisibleSingle( BOOL bVisible );

public:
	void				 Update();
	void                 UpdateInfo	   ();
	void                 UpdateBattle  ();
	void                 UpdateHistory ();

public:
	virtual void         BeginSubControl();
	virtual void         EndSubControl();

private:
	CBasicTextBox*		 m_pText_Title;

	CUIControl*			 m_pComp_SchoolImage[SCHOOL_NUM];

	VEC_SKILLIMAGE	 m_vecSkillImage;

private:
	PVP::ENUM_CTF_TYPE m_emCTFType;

public:
	void            BattleUpdate ();

public:
	void			SetCTFType( const PVP::ENUM_CTF_TYPE emType ) { m_emCTFType = emType; }

private:
	void			EnableJoinButton( const std::string& strLabel, const bool bEnable );

private:
	void            ReqJoin();
	void            ReqCancel();
	void            ReqExit();

private:
	CBasicTextBox*  CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign );
	void HistoryUpdate();

private:
	CBasicTextButton* m_pJoinButton;

private:
	boost::function<void(void)> m_fnCallbackButton;

protected:
	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;

private:
	std::string		 m_strTimeText;
	CBasicTextBox*	 m_pText_Time;
	CBasicTextBox*	 m_pText_Buff;
	CBasicTextBox*	 m_pText_Ranking;

	CSwapImage*		 m_pImage_AuthSchool;

	CUIControl*		 m_pAuthSchoolControl;
	CUIControl*		 m_pBuffControl;
	CUIControl*		 m_pBuffGapControl;
	CUIControl*		 m_pBuffItemControl;

	CBasicButton*	 m_pButton_Ranking;

	float           m_fJoinButton_Delay; // ��û��ư�� ���������� ���������ϵ��� �����̸� ��;

protected:
	LPDIRECT3DDEVICEQ* m_ppd3dDevice;
};
