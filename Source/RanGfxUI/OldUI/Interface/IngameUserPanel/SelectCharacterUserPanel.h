//	�⺻ ���� ǥ��
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.11.21]
//			@ �ۼ�
//
#include "../../Util/UIGroupHelper.h"

#pragma	once

class   COuterInterface;
class	CBasicTextBox;
class	CBasicTextButton;


const DWORD UIMSG_MOUSEIN_SELECT_USERPANEL = UIMSG_USER1;

class CSelectCharacterUserPanel : public CUIGroupHelper
{
private:
	enum
	{
		SELECT_CHARACTER_USERPANEL_CP = NO_ID + 1,
		SELECT_CHARACTER_USERPANEL_CPN,
		SELECT_CHARACTER_USERPANEL_CE,
		SELECT_CHARACTER_USERPANEL_RN,
		SELECT_CHARACTER_USERPANEL_GTCVT,
		SELECT_CHARACTER_USERPANEL_PPCVT,
		SELECT_CHARACTER_USERPANEL_TOPUP,
	};
public:
	CSelectCharacterUserPanel (GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSelectCharacterUserPanel ();

public:
	void	CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
private:
	GLGaeaClient* m_pGaeaClient;
	COuterInterface*	m_pInterface;
    CBasicTextBox* m_pEmailTextBox;
    CBasicTextBox* m_pBattlePointTextBox;
    CBasicTextBox* m_pGameTimeTextBox;
    CBasicTextBox* m_pUserPointTextBox;
    CBasicTextBox* m_pPasswordTextBox;
    CBasicTextBox* m_pPinTextBox;
	
	CBasicTextButton* m_pButtonCE;
	CBasicTextButton* m_pButtonCP;
	CBasicTextButton* m_pButtonCPN;
	CBasicTextButton* m_pButtonRP;
	CBasicTextButton* m_pButtonGTCVT;
	CBasicTextButton* m_pButtonPPCVT;
	CBasicTextButton* m_pButtonTOPUP;

private:
	DWORD m_uUserPoint;
	DWORD m_dwGameTime;
	DWORD m_uBattlePoint;
	CString m_strEmail;
};