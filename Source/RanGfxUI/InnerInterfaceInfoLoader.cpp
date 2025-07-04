#include "StdAfx.h"

#include "../SigmaCore/String/StringFormat.h"

#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "../RanLogicClient/GLGaeaClient.h"
#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/Crow/GLCrowDataMan.h"
#include "../RanLogic/GLogicDataMan.h"

#include "../RanLogicClient/Land/GLLandManClient.h"
#include "../RanLogicClient/Pet/GLPetClient.h"

#include "./OldUI/Display/ItemInfoTooltip.h"
#include "./OldUI/Display/SkillInforTooltip.h"
#include "./OldUI/Display/SkillLinkInfoTooltip.h"
#include "InnerInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------//
// ITEMINFO

void CInnerInterface::INFO_DISPLAY_ITEM_RESET ()
{
	m_INFO_DISPLAY_ITEM_sItemCustomBACK = SITEMCUSTOM( NATIVEID_NULL() );
	m_INFO_DISPLAY_ITEM_bShopOpenBACK				= FALSE;
	m_INFO_DISPLAY_ITEM_bInMarketBACK				= FALSE;
	m_INFO_DISPLAY_ITEM_bInPrivateMarketBACK		= FALSE;
    m_INFO_DISPLAY_ITEM_bUsePreviewBACK			= FALSE;
	m_INFO_DISPLAY_ITEM_bIsWEAR_ITEMBACK			= FALSE;
	m_INFO_DISPLAY_ITEM_bControlBACK					= FALSE;
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddTextNoSplit ( CString strText, D3DCOLOR dwColor )
{
	ADDTEXT_NOSPLIT ( strText, dwColor );
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddTextLongestLineSplit ( CString strText, D3DCOLOR dwColor )
{
	ADDTEXT_LONGESTLINE_SPLIT ( strText, dwColor );
}

void CInnerInterface::INFO_DISPLAY_ITEM_RemoveAllInfo ()
{
	CLEAR_TEXT ();
}

CString CInnerInterface::INFO_DISPLAY_ITEM_GetNumberWithSign ( int nValue )
{
	CString strNumber;

	if ( 0 < nValue )
	{
		strNumber.Format( "(+%d)" , nValue );
	}
	else if ( nValue < 0 )
	{
		strNumber.Format( "(%d)" , nValue );
	}

	return strNumber;
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddInfoItemAddon ( int nBasic, int nAddon, CString strFormat )
{
	CString Text;
	if ( nAddon )
	{
		Text.Format ( "%s:%d%s", strFormat, nBasic, INFO_DISPLAY_ITEM_GetNumberWithSign ( nAddon ) );
		INFO_DISPLAY_ITEM_AddTextNoSplit ( Text, NS_UITEXTCOLOR::PRIVATE );
	}
	else if ( nBasic )
	{
		Text.Format ( "%s:%d", strFormat, nBasic );
		INFO_DISPLAY_ITEM_AddTextNoSplit ( Text, NS_UITEXTCOLOR::DEFAULT );
	}
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddInfoItemAddon ( int nBasic, int nAddon, BYTE uGRADE, CString strFormat )
{
	CString Text;
	if ( nAddon )
	{
		Text.Format ( "%s:%d%s", strFormat, nBasic, INFO_DISPLAY_ITEM_GetNumberWithSign( nAddon ) );
		INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( Text, uGRADE );
		INFO_DISPLAY_ITEM_AddTextNoSplit ( Text, NS_UITEXTCOLOR::PRIVATE );
	}
	else if ( nBasic )
	{
		Text.Format ( "%s:%d", strFormat, nBasic );
		INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( Text, uGRADE );
		INFO_DISPLAY_ITEM_AddTextNoSplit ( Text, NS_UITEXTCOLOR::DEFAULT );
	}
}

void CInnerInterface::INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( CString& strOrigin, BYTE uGRADE )
{
	if ( !uGRADE ) return ;

	CString strGRADE;
	strGRADE.Format ( "[+%d]", uGRADE );
	strOrigin += strGRADE;
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddInfoItemAddonRange ( int nBasicMin, int nBasicMax, int nAddon, BYTE uGRADE, CString strFormat )
{
	CString Text;

	if ( nAddon )
	{
		Text.Format ( "%s:%d%s~%d%s", strFormat, nBasicMin, INFO_DISPLAY_ITEM_GetNumberWithSign( nAddon ), nBasicMax, INFO_DISPLAY_ITEM_GetNumberWithSign( nAddon ) );
		INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( Text, uGRADE );
		INFO_DISPLAY_ITEM_AddTextNoSplit ( Text, NS_UITEXTCOLOR::PRIVATE );
	}
	else if ( nBasicMin || nBasicMax )
	{
		Text.Format ( "%s:%d~%d", strFormat, nBasicMin, nBasicMax );
		INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( Text, uGRADE );
		INFO_DISPLAY_ITEM_AddTextNoSplit ( Text, NS_UITEXTCOLOR::DEFAULT );
	}
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddItemTurnInfo ( const SITEMCUSTOM &sItemCustom, const BOOL bInMarket, const BOOL bInPrivateMarket )
{
	const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );

	CString strText;

	//switch ( pItemData->sBasicOp.emItemType )
	//{
	//case ITEM_CHARM:
	//case ITEM_ARROW:
	//case ITEM_CURE:
	//case ITEM_GRINDING:
	if ( pItemData->sDrugOp.wPileNum > 1 )
	{
		WORD wPileNum = pItemData->sDrugOp.wPileNum;
		WORD wTurnNum = sItemCustom.wTurnNum;				
		if ( bInPrivateMarket )	//	°³ÀÎ »óÁ¡
		{
			strText.Format("%s:%d", ID2GAMEWORD("ITEM_TURN_INFO", 0 ), wTurnNum);
			INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::CHARTREUSE);

			return ;
		}

		if ( bInMarket )	wTurnNum = pItemData->GETAPPLYNUM();
		strText.Format("%s:%d/%d", ID2GAMEWORD("ITEM_TURN_INFO", 0 ), wTurnNum, wPileNum);
		INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);
	}
	//break;
	//}
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddInfoPetSkillItem( const SITEMCUSTOM &sItemCustom )
{
	CString strText;

	SNATIVEID sNativeID = sItemCustom.GetNativeID();
	GLPetClient* pPetClient = m_pGaeaClient->GetPetClient();
	const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sNativeID );
	SNATIVEID sSkillID = pItemData->sSkillBookOp.sSkill_ID;

	//	Note : ½ºÅ³ Á¤º¸ °¡Á®¿È.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	if ( pSkill )
	{
		//	±âº» Á¤º¸
		//{
		//	//	3. ¼Ó¼º
		//	strText.Format("%s:%s",ID2GAMEWORD("SKILL_BASIC_INFO", 2), COMMENT::BRIGHT[pSkill->m_sLEARN.emBRIGHT].c_str());
		//	INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PALEGREEN);
		//}

		//{
		//	//	¸ñÇ¥ °¡´É°Å¸®
		//	const SKILL::SBASIC& sBASIC = pSkill->m_sBASIC;		

		//	if ( sBASIC.wTARRANGE )
		//	{
		//		strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_TARGET_RANGE", 0), sBASIC.wTARRANGE);
		//		INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
		//	}
		//}

		//	ÀÌ¹Ì ¹è¿î ½ºÅ³
		if ( pPetClient->ISLEARNED_SKILL( sSkillID ) )
		{
			INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_SKILL_CONDITION", 0 ), NS_UITEXTCOLOR::RED );	
			return ;
		}
	}
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddInfoSkillItem ( const SITEMCUSTOM &sItemCustom )
{
	CString strText;

	SNATIVEID sNativeID = sItemCustom.GetNativeID();
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();	
	const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sNativeID );
	SNATIVEID sSkillID = pItemData->sSkillBookOp.sSkill_ID;

	//	Note : ½ºÅ³ Á¤º¸ °¡Á®¿È.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	if ( pSkill )
	{
		//	±âº» Á¤º¸
		{
			//	2. µî±Þ
			strText.Format("%s:%d",ID2GAMEWORD("SKILL_BASIC_INFO", 1), pSkill->m_sBASIC.dwGRADE);
			INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PALEGREEN);

			// Info : 착용 클래스
			{
				std::string strClass;
				DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
				if( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
				{
					const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
					if ( !pItemDisguise )   return;

					DWORD dwReqCharClass_Disguise = pItemDisguise->sBasicOp.dwReqCharClass;
					DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

					if ( (dwReqCharClass&m_pGaeaClient->GetCharacter()->m_emClass) && (dwReqCharClass_Disguise&m_pGaeaClient->GetCharacter()->m_emClass) )
						dwColor = ITEM_INFOR_COLOR::dwDefault;
					else
						dwColor = ITEM_INFOR_COLOR::dwReqWrong;

					if( dwReqCharClass )
					{
						if ( (dwReqCharClass==GLCC_ALL_TRICKER) && (dwReqCharClass_Disguise==GLCC_ALL_TRICKER)
							|| (dwReqCharClass==GLCC_ALL_SCIENTIST) && (dwReqCharClass_Disguise==GLCC_ALL_SCIENTIST)
							|| (dwReqCharClass==GLCC_ALL_ASSASSIN) && (dwReqCharClass_Disguise==GLCC_ALL_ASSASSIN)
							|| (dwReqCharClass==GLCC_ALL_ACTOR) && (dwReqCharClass_Disguise==GLCC_ALL_ACTOR)
							|| (dwReqCharClass == GLCC_MAN) && (dwReqCharClass_Disguise == GLCC_MAN)
							|| (dwReqCharClass == GLCC_WOMAN) && (dwReqCharClass_Disguise == GLCC_WOMAN) )
						{
							strClass = ID2GAMEWORD("ITEM_ALL_CLASS");
						}
						else
						{
							if ( (dwReqCharClass&GLCC_FIGHTER_M) && (dwReqCharClass_Disguise&GLCC_FIGHTER_M)
								|| (dwReqCharClass&GLCC_FIGHTER_W) && (dwReqCharClass_Disguise&GLCC_FIGHTER_W) )
							{
								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_FIGHTER_M ) );
								strClass += " ";
							}

							if ( (dwReqCharClass&GLCC_ARMS_M)  && (dwReqCharClass_Disguise&GLCC_ARMS_M)
								|| (dwReqCharClass&GLCC_ARMS_W)  && (dwReqCharClass_Disguise&GLCC_ARMS_W) )
							{
								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARMS_M ) );
								strClass += " ";
							}

							if ( (dwReqCharClass&GLCC_ARCHER_M)  && (dwReqCharClass_Disguise&GLCC_ARCHER_M)
								|| (dwReqCharClass&GLCC_ARCHER_W)  && (dwReqCharClass_Disguise&GLCC_ARCHER_W) )
							{
								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS",
									CharClassToClassIndex( GLCC_ARCHER_M ) );
								strClass += " ";
							}

							if ( (dwReqCharClass&GLCC_SPIRIT_M)  && (dwReqCharClass_Disguise&GLCC_SPIRIT_M)
								|| (dwReqCharClass&GLCC_SPIRIT_W)  && (dwReqCharClass_Disguise&GLCC_SPIRIT_W) )
							{

								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SPIRIT_M ));
								strClass += " ";
							}

							if ( (dwReqCharClass&GLCC_EXTREME_M)  && (dwReqCharClass_Disguise&GLCC_EXTREME_M)
								|| (dwReqCharClass&GLCC_EXTREME_W)  && (dwReqCharClass_Disguise&GLCC_EXTREME_W) )
							{

								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_EXTREME_M ) );
								strClass += " ";
							}

							if ( (dwReqCharClass&GLCC_SCIENTIST_M)  && (dwReqCharClass_Disguise&GLCC_SCIENTIST_M)
								|| (dwReqCharClass&GLCC_SCIENTIST_W)  && (dwReqCharClass_Disguise&GLCC_SCIENTIST_W) )
							{

								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SCIENTIST_M ) );
								strClass += " ";
							}

							/*if ( (dwReqCharClass&GLCC_ASSASSIN_M)  && (dwReqCharClass_Disguise&GLCC_ASSASSIN_M)
								|| (dwReqCharClass&GLCC_ASSASSIN_W)  && (dwReqCharClass_Disguise&GLCC_ASSASSIN_W) )
							{

								INFO_DISPLAY_ITEM_AddTextNoSplit( 
									ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ASSASSIN_M ) ),
									dwColor );
							}

							if ( (dwReqCharClass&GLCC_TRICKER_M)  && (dwReqCharClass_Disguise&GLCC_TRICKER_M)
								|| (dwReqCharClass&GLCC_TRICKER_W)  && (dwReqCharClass_Disguise&GLCC_TRICKER_W) )
							{

								INFO_DISPLAY_ITEM_AddTextNoSplit( 
									ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_TRICKER_M ) ),
									dwColor );
							}*/

							/*if ( (dwReqCharClass&GLCC_ETC_M)  && (dwReqCharClass_Disguise&GLCC_ETC_M)
								|| (dwReqCharClass&GLCC_ETC_W)  && (dwReqCharClass_Disguise&GLCC_ETC_W) )
							{

								INFO_DISPLAY_ITEM_AddTextNoSplit(
									ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ETC_M ) ),
									dwColor );
							}*/
						}
						INFO_DISPLAY_ITEM_AddTextNoSplit(strClass.c_str(), dwColor );
					}
				}
				else
				{
					std::string strClass;
					DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

					if ( (dwReqCharClass&m_pGaeaClient->GetCharacter()->m_emClass) )
						dwColor = ITEM_INFOR_COLOR::dwDefault;
					else
						dwColor = ITEM_INFOR_COLOR::dwReqWrong;

					if( dwReqCharClass )
					{
						if ( dwReqCharClass==GLCC_ALL_TRICKER
							|| dwReqCharClass==GLCC_ALL_ASSASSIN
							|| dwReqCharClass==GLCC_ALL_SCIENTIST
							|| dwReqCharClass==GLCC_ALL_ACTOR
							|| (dwReqCharClass == GLCC_MAN)
							|| (dwReqCharClass == GLCC_WOMAN) )
						{
							strClass = ID2GAMEWORD("ITEM_ALL_CLASS");
						}
						else
						{
							bool bDistinction = false;
							if ( (dwReqCharClass & GLCC_FIGHTER_M) 
								|| (dwReqCharClass & GLCC_FIGHTER_W) )
							{

								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_FIGHTER_M ));
								strClass += " ";
							}

							if ( (dwReqCharClass & GLCC_ARMS_M) 
								|| (dwReqCharClass & GLCC_ARMS_W) )
							{
								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARMS_M ) );
								strClass += " ";
							}

							if ( (dwReqCharClass & GLCC_ARCHER_M)
								|| (dwReqCharClass & GLCC_ARCHER_W) )
							{

								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARCHER_M ) );
								strClass += " ";
							}

							if ( (dwReqCharClass & GLCC_SPIRIT_M)
								|| (dwReqCharClass & GLCC_SPIRIT_W) )
							{

								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SPIRIT_M ) );
								strClass += " ";
							}
							if ( (dwReqCharClass & GLCC_EXTREME_M)
								|| (dwReqCharClass & GLCC_EXTREME_W) )
							{

								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_EXTREME_M ) );
								strClass += " ";
							}

							if ( (dwReqCharClass & GLCC_SCIENTIST_M)
								|| (dwReqCharClass & GLCC_SCIENTIST_W) )
							{

								strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SCIENTIST_M ) );
								strClass += " ";
							}

							/*if ( (dwReqCharClass & GLCC_ASSASSIN_M)
								|| (dwReqCharClass & GLCC_ASSASSIN_W) )
							{

								INFO_DISPLAY_ITEM_AddTextNoSplit( 
									ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ASSASSIN_M ) ),
									dwColor );
							}

							if ( (dwReqCharClass & GLCC_TRICKER_M)
								|| (dwReqCharClass & GLCC_TRICKER_W) )
							{

								INFO_DISPLAY_ITEM_AddTextNoSplit( 
									ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_TRICKER_M ) ),
									dwColor );
							}

							if ( (dwReqCharClass & GLCC_ETC_M)
								|| (dwReqCharClass & GLCC_ETC_W) )
							{

								INFO_DISPLAY_ITEM_AddTextNoSplit(
									ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ETC_M ) ),
									dwColor );
							}*/
						}
					}
					INFO_DISPLAY_ITEM_AddTextNoSplit(strClass.c_str(), dwColor );
				}
			}

			// Info : 성별
			{
				std::string strClass;
				DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
				if( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
				{
					const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
					if ( !pItemDisguise )   return;

					DWORD dwReqCharClass_Disguise = pItemDisguise->sBasicOp.dwReqCharClass;
					DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

					if ( ( (dwReqCharClass!=GLCC_ALL_SCIENTIST) || (dwReqCharClass_Disguise!=GLCC_ALL_SCIENTIST) ) &&
						 (dwReqCharClass!=GLCC_ALL_ASSASSIN) || (dwReqCharClass_Disguise!=GLCC_ALL_ASSASSIN) &&
						 (dwReqCharClass!=GLCC_ALL_ACTOR) || (dwReqCharClass_Disguise!=GLCC_ALL_ACTOR) &&
						 (dwReqCharClass!=GLCC_ALL_TRICKER) || (dwReqCharClass_Disguise!=GLCC_ALL_TRICKER) )
					{
						bool bMan = false;
						bool bWoman = false;

						if ( (dwReqCharClass&GLCC_FIGHTER_M) && (dwReqCharClass_Disguise&GLCC_FIGHTER_M)
							|| (dwReqCharClass&GLCC_ARMS_M) && (dwReqCharClass_Disguise&GLCC_ARMS_M)
							|| (dwReqCharClass&GLCC_ARCHER_M) && (dwReqCharClass_Disguise&GLCC_ARCHER_M)
							|| (dwReqCharClass&GLCC_SPIRIT_M) && (dwReqCharClass_Disguise&GLCC_SPIRIT_M)
							|| (dwReqCharClass&GLCC_EXTREME_M) && (dwReqCharClass_Disguise&GLCC_EXTREME_M)
							|| (dwReqCharClass&GLCC_SCIENTIST_M) && (dwReqCharClass_Disguise&GLCC_SCIENTIST_M)
							|| (dwReqCharClass&GLCC_ASSASSIN_M) && (dwReqCharClass_Disguise&GLCC_ASSASSIN_M)
							|| (dwReqCharClass&GLCC_TRICKER_M) && (dwReqCharClass_Disguise&GLCC_TRICKER_M)
							|| (dwReqCharClass&GLCC_ETC_M) && (dwReqCharClass_Disguise&GLCC_ETC_M) )
						{
							bMan = true;
						}

						if ( (dwReqCharClass&GLCC_FIGHTER_W) && (dwReqCharClass_Disguise&GLCC_FIGHTER_W)
							|| (dwReqCharClass&GLCC_ARMS_W) && (dwReqCharClass_Disguise&GLCC_ARMS_W)
							|| (dwReqCharClass&GLCC_ARCHER_W) && (dwReqCharClass_Disguise&GLCC_ARCHER_W)
							|| (dwReqCharClass&GLCC_SPIRIT_W) && (dwReqCharClass_Disguise&GLCC_SPIRIT_W)
							|| (dwReqCharClass&GLCC_EXTREME_W) && (dwReqCharClass_Disguise&GLCC_EXTREME_W)
							|| (dwReqCharClass&GLCC_SCIENTIST_W) && (dwReqCharClass_Disguise&GLCC_SCIENTIST_W)
							|| (dwReqCharClass&GLCC_ASSASSIN_W) && (dwReqCharClass_Disguise&GLCC_ASSASSIN_W)
							|| (dwReqCharClass&GLCC_TRICKER_W) && (dwReqCharClass_Disguise&GLCC_TRICKER_W)
							|| (dwReqCharClass&GLCC_ETC_W) && (dwReqCharClass_Disguise&GLCC_ETC_W) )
						{
							bWoman = true;
						}

						if ( !bMan || !bWoman )
						{
							// 남, 여 모두 착용 가능하지 않을 때.
							if ( bMan )
							{
								// 남자 착용 가능 할 때.
								if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_M) 
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_M) )
									dwColor = ITEM_INFOR_COLOR::dwDefault;
								else
									dwColor = ITEM_INFOR_COLOR::dwReqWrong;

								//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 0 ), dwColor );
								strClass += ID2GAMEWORD( "BASIC_SEX", 0 );
								strClass += " ";
							}
							else
							{
								// 여자 착용 가능 할 때.
								if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_W) 
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_W) )
									dwColor = ITEM_INFOR_COLOR::dwDefault;
								else
									dwColor = ITEM_INFOR_COLOR::dwReqWrong;

								//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 1 ), dwColor );
								strClass += ID2GAMEWORD( "BASIC_SEX", 1 );
								strClass += " ";
							}
						}
					}
				}
				else
				{
					DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

					if ( (dwReqCharClass!=GLCC_ALL_SCIENTIST) || (dwReqCharClass!=GLCC_ALL_ASSASSIN) || (dwReqCharClass!=GLCC_ALL_ACTOR) || (dwReqCharClass!=GLCC_ALL_TRICKER) )
					{
						bool bMan = false;
						bool bWoman = false;

						if ( (dwReqCharClass & GLCC_FIGHTER_M) 
							|| (dwReqCharClass & GLCC_ARMS_M)
							|| (dwReqCharClass & GLCC_ARCHER_M)
							|| (dwReqCharClass & GLCC_SPIRIT_M)
							|| (dwReqCharClass & GLCC_EXTREME_M)
							|| (dwReqCharClass & GLCC_SCIENTIST_M)
							|| (dwReqCharClass & GLCC_ASSASSIN_M)
							|| (dwReqCharClass & GLCC_TRICKER_M)
							|| (dwReqCharClass & GLCC_ETC_M) )
						{
							bMan = true;
						}

						if ( (dwReqCharClass & GLCC_FIGHTER_W) 
							|| (dwReqCharClass & GLCC_ARMS_W)
							|| (dwReqCharClass & GLCC_ARCHER_W)
							|| (dwReqCharClass & GLCC_SPIRIT_W)
							|| (dwReqCharClass & GLCC_EXTREME_W)
							|| (dwReqCharClass & GLCC_SCIENTIST_W)
							|| (dwReqCharClass & GLCC_ASSASSIN_W)
							|| (dwReqCharClass & GLCC_TRICKER_W)
							|| (dwReqCharClass & GLCC_ETC_W) )
						{
							bWoman = true;
						}

						if ( !bMan || !bWoman )
						{
							// 남, 여 모두 착용 가능하지 않을 때.
							if ( bMan )
							{
								// 남자 착용 가능 할 때.
								if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_M) 
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_M)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_M) )
									dwColor = ITEM_INFOR_COLOR::dwDefault;
								else
									dwColor = ITEM_INFOR_COLOR::dwReqWrong;

								//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 0 ), dwColor );
								strClass += ID2GAMEWORD( "BASIC_SEX", 0 );
								strClass += " ";
							}
							else
							{
								// 여자 착용 가능 할 때.
								if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_W) 
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_W)
									|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_W) )
									dwColor = ITEM_INFOR_COLOR::dwDefault;
								else
									dwColor = ITEM_INFOR_COLOR::dwReqWrong;

								//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 1 ), dwColor );
								strClass += ID2GAMEWORD( "BASIC_SEX", 1 );
								strClass += " ";
							}
						}
					}
				}
			}
		}

		{
			//	¸ñÇ¥ °¡´É°Å¸®, ¿Þ¼Õ, ¿À¸¥¼Õ µµ±¸
			const SKILL::SBASIC& sBASIC = pSkill->m_sBASIC;		

			if ( sBASIC.wTARRANGE )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_TARGET_RANGE", 0), sBASIC.wTARRANGE);
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			}

			if ( (sBASIC.emUSE_LITEM && (sBASIC.emUSE_LITEM != SKILL::SKILLATT_NOCARE)) || 
				(sBASIC.emUSE_RITEM && (sBASIC.emUSE_RITEM != SKILL::SKILLATT_NOCARE)) )
			{
				INFO_DISPLAY_ITEM_AddTextNoSplit(ID2GAMEWORD("SKILL_CATEGORY", 1),NS_UITEXTCOLOR::LIGHTSKYBLUE);
			}

			CString strUSEITEM;
			if ( sBASIC.emUSE_LITEM )
			{
				if ( sBASIC.emUSE_LITEM != SKILL::SKILLATT_NOCARE )
				{
					strText.Format( "%s:%s", 
						ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM_LR_HAND", 0), 
						ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM", sBASIC.emUSE_LITEM - 1 ) );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
				}
			}

			if ( sBASIC.emUSE_RITEM )
			{
				if ( sBASIC.emUSE_RITEM != SKILL::SKILLATT_NOCARE )
				{
					strText.Format ( "%s:%s", 
						ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM_LR_HAND", 1), 
						ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM", sBASIC.emUSE_RITEM - 1 ) );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
				}
			}
		}

		//	ÀÌ¹Ì ¹è¿î ½ºÅ³
		if ( pCharacter->GETLEARNED_SKILL ( sSkillID ) )
		{
			INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_SKILL_CONDITION", 0 ), NS_UITEXTCOLOR::RED );	
			return ;
		}

		//	¿ä±¸Ä¡ Á¤º¸
		{
			INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD("SKILL_CATEGORY", 7), NS_UITEXTCOLOR::LIGHTSKYBLUE);

			const WORD wLevel = 0;
			SKILL::SLEARN& sLEARN = pSkill->m_sLEARN;
			SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[wLevel];

			//	1. ¿ä±¸º¸À¯½ºÅ³
			SNATIVEID NeedSkillID = pSkill->m_sLEARN.sSKILL;			
			if ( NeedSkillID != NATIVEID_NULL() )
			{
				BOOL bVALID = FALSE;
				BOOL bNeedSkillLevel = FALSE;

				CString strNeedSkillName;
				CString strNeedSkillLevel;

				PGLSKILL pNeedSkill = GLSkillMan::GetInstance().GetData ( NeedSkillID.wMainID, NeedSkillID.wSubID );
				if ( pNeedSkill ) strNeedSkillName.Format("%s:%s", ID2GAMEWORD( "SKILL_ADVANCED_INFO", 0), pNeedSkill->GetName() );
				bVALID = pCharacter->ISLEARNED_SKILL ( NeedSkillID );			

				//	2. ¿ä±¸º¸À¯½ºÅ³·¹º§
				if ( 0 < sLVL.dwSKILL_LVL )
				{
					strNeedSkillLevel.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO", 1), (sLVL.dwSKILL_LVL + 1) );
					bNeedSkillLevel = TRUE;

					SKILL_MAP& map = pCharacter->m_ExpSkills;				
					SKILL_MAP_ITER iter = map.find ( NeedSkillID.dwID );
					if ( iter != map.end() )
					{
						SCHARSKILL& rCharSkill = (*iter).second;

						//	»ö Á¶Àý
						bVALID = rCharSkill.wLevel >= sLVL.dwSKILL_LVL;						
					}
				}

				INFO_DISPLAY_ITEM_AddTextNoSplit(strNeedSkillName,NS_UITEXTCONTROL::GetEvaluateColor ( bVALID ));

				if ( bNeedSkillLevel )
					INFO_DISPLAY_ITEM_AddTextNoSplit(strNeedSkillLevel, NS_UITEXTCONTROL::GetEvaluateColor ( bVALID ) );
			}

			//	3. ¿ä±¸°æÇèÄ¡
			if ( 0 < sLVL.dwSKP )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO", 2), sLVL.dwSKP);				
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_dwSkillPoint >= sLVL.dwSKP ) );
			}

			//	4. ¿ä±¸·¹º§
			if ( 0 < sLVL.dwLEVEL )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO", 3), sLVL.dwLEVEL);
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->GETLEVEL () >= int(sLVL.dwLEVEL) ) );
			}

			//	5. Á¶°Ç - ¾Ï±¤
			BOOL bValue = TRUE;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				strText.Format ( "%s", CGameTextMan::GetInstance().GetCommentText("BRIGHT", pItemData->sBasicOp.emReqBright).GetString() );
			else
				strText.Format ( "%s", COMMENT::BRIGHT[pItemData->sBasicOp.emReqBright].c_str() );
			if ( pItemData->sBasicOp.emReqBright != BRIGHT_BOTH )
			{
				if ( pCharacter->GETBRIGHT() != pItemData->sBasicOp.emReqBright )
				{
					bValue = FALSE;
				}
			}
			INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCONTROL::GetEvaluateColor ( bValue ) );


			//	Stats
			//	1. ¿ä±¸Èû
			if ( 0 < sLVL.sSTATS.wPow )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 0), sLVL.sSTATS.wPow);
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wPow >= sLVL.sSTATS.wPow ) );
			}

			//	2. ¿ä±¸Ã¼·Â
			if ( 0 < sLVL.sSTATS.wStr )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 1), sLVL.sSTATS.wStr);
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wStr >= sLVL.sSTATS.wStr ) );
			}

			//	3. ¿ä±¸Á¤½Å
			if ( 0 < sLVL.sSTATS.wSpi )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 2), sLVL.sSTATS.wSpi);
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wSpi >= sLVL.sSTATS.wSpi ));
			}

			//	4. ¿ä±¸¹ÎÃ¸
			if ( 0 < sLVL.sSTATS.wDex )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 3), sLVL.sSTATS.wDex);
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wDex >= sLVL.sSTATS.wDex ) );
			}

			//	5. ¿ä±¸Áö·Â
			if ( 0 < sLVL.sSTATS.wInt )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 4), sLVL.sSTATS.wInt);
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wInt >= sLVL.sSTATS.wInt ) );
			}

			//	6. ¿ä±¸±Ù·Â
			if ( 0 < sLVL.sSTATS.wSta )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 5), sLVL.sSTATS.wSta);
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wPow >= sLVL.sSTATS.wSta ) );
			}
		}
	}
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddTextSpecialVaule( const SITEMCUSTOM & sItemCustom, const SITEM* pItemData )
{
	if ( !pItemData ) return;

	EMITEM_VAR emITEM_VAR = pItemData->sSuitOp.sVARIATE.emTYPE;
	EMITEM_VAR emITEM_VOL = pItemData->sSuitOp.sVOLUME.emTYPE;

	bool bPrintHead = false;
	CString strText;

	if ( ( emITEM_VAR != EMVAR_NONE ) || ( emITEM_VOL != EMVAR_NONE ) )
	{
		if ( !bPrintHead ) 
		{
			INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_CATEGORY", 5 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			bPrintHead = true;
		}
	}

	float arrVALUE[EMVAR_SIZE];
	SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );

	//	Æ¯¼ö±â´É ( º¯È­À² )
	//
	if ( emITEM_VAR != EMVAR_NONE )
	{
		switch ( emITEM_VAR )
		{
		case EMVAR_HP:
		case EMVAR_MP:
		case EMVAR_SP:
		case EMVAR_AP:
			break;
		default:
			arrVALUE[emITEM_VAR] = pItemData->sSuitOp.sVARIATE.fVariate;
			break;
		};
	}

	arrVALUE[EMVAR_HP] = sItemCustom.GETINCHP();
	arrVALUE[EMVAR_MP] = sItemCustom.GETINCMP();
	arrVALUE[EMVAR_SP] = sItemCustom.GETINCSP();
	arrVALUE[EMVAR_AP] = sItemCustom.GETINCAP();

	for ( int i = 1; i < EMVAR_SIZE; ++i )
	{
		if ( arrVALUE[i] != 0.0f ) 
		{
			if ( !bPrintHead ) 
			{
				INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_CATEGORY", 5 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );
				bPrintHead = true;
			}

			arrVALUE[i] *= COMMENT::ITEMVAR_SCALE[i];
			strText.Format("%s %2.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", i ), arrVALUE[i] );
			if ( COMMENT::IsITEMVAR_SCALE(emITEM_VAR) )		strText += "%";
			INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
		}
	}


	// °æÇèÄ¡ ¹èÀ²
	float fExpMultiple = pItemData->GetExpMultiple();
	if( fExpMultiple != 1.0f )
	{
		strText.Format("%s %2.2f%s", 
			ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_EX", 0 ), 
			fExpMultiple, 
			ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_EX", 1 ) );

		INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
	}

	//	Æ¯¼ö±â´É ( º¯È­·® )
	//
	if ( emITEM_VOL != EMVAR_NONE)
	{
		if ( emITEM_VOL == EMVAR_MOVE_SPEED )
		{
			float fVOLUME = sItemCustom.GETMOVESPEED();
			strText.Format("%s %.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ), fVOLUME );
			INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
		}
		else
		{
			float fVOLUME = pItemData->sSuitOp.sVOLUME.fVolume;
			strText.Format("%s %.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ), fVOLUME );
			INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
		}			
	}


	//	ÄÚ½ºÆ¬ ´É·ÂÄ¡
	if ( sItemCustom.nidDISGUISE == NATIVEID_NULL() )	return;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
	//if ( !pItem || !pItem->sBasicOp.IsDISGUISE() ) return;
	if ( !pItem || !( sItemCustom.IsDisguise() != NULL ) ) return;

	EMITEM_VAR emITEM_VAR_COS = pItem->sSuitOp.sVARIATE.emTYPE;
	EMITEM_VAR emITEM_VOL_COS = pItem->sSuitOp.sVOLUME.emTYPE;

	if ( ( emITEM_VAR_COS != EMVAR_NONE ) || ( emITEM_VOL_COS != EMVAR_NONE ) )
	{
		if ( !bPrintHead ) 
		{
			INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_CATEGORY", 5 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			bPrintHead = true;
		}
	}

	if ( emITEM_VAR_COS != EMVAR_NONE )
	{
		float fVariate = pItem->sSuitOp.sVARIATE.fVariate;		
		fVariate *= COMMENT::ITEMVAR_SCALE[emITEM_VAR_COS];
		strText.Format("%s %2.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", emITEM_VAR_COS ), fVariate );
		if ( COMMENT::IsITEMVAR_SCALE(emITEM_VAR_COS) )		strText += "%";
		INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::HOTPINK);
	}

	if ( emITEM_VOL_COS != EMVAR_NONE)
	{
		float fVOLUME = pItem->sSuitOp.sVOLUME.fVolume;
		strText.Format("%s %.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL_COS ), fVOLUME );
		INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::HOTPINK);
	}
}

EMITEM_VAR	ConvertAddon2Var ( EMITEM_ADDON emAddOn )
{
	switch ( emAddOn )
	{
	case EMADDEX_INCR_HP:		return EMVAR_HP;
	case EMADDEX_INCR_MP:		return EMVAR_MP;
	case EMADDEX_INCR_SP:		return EMVAR_SP;
	case EMADDEX_INCR_AP:		return EMVAR_AP;

	case EMADDEX_INCR_MOVE_SPEED:		return EMVAR_MOVE_SPEED;
	case EMADDEX_INCR_ATTACK_SPEED:		return EMVAR_ATTACK_SPEED;
	case EMADDEX_INCR_CRITICAL_RATE:	return EMVAR_CRITICAL_RATE;
	case EMADDEX_INCR_CRUSHING_BLOW:		return EMVAR_CRUSHING_BLOW;
	default :
		return EMVAR_SIZE;
		break;
	}
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddTextAddValue( const SITEMCUSTOM & sItemCustom, const ITEM::SSUIT & _sSUIT )
{
	CString strText;
	BOOL bLEAST_HAVE_ONE(TRUE);
	INT arrVALUE[EMADD_SIZE];
	SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );

	ITEM::SSUIT sSUIT = _sSUIT;

	for ( int i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
	{
		EMITEM_ADDON emTYPE = sSUIT.sADDON[i].emTYPE;

		if ( emTYPE != EMADD_NONE )
		{
			int nVALUE = sSUIT.sADDON[i].nVALUE;
			float fVALUE = sSUIT.sADDON[i].fVALUE;

			switch ( emTYPE )
			{
				//	¹Ø¿¡¼­ ÇÑ¹ø¿¡ °è»ê
			case EMADD_HP:
			case EMADD_MP:
			case EMADD_SP:
			case EMADD_MA:
				break;
			case EMADD_HITRATE:		arrVALUE[EMADD_HITRATE] += nVALUE;		break;
			case EMADD_AVOIDRATE:	arrVALUE[EMADD_AVOIDRATE] += nVALUE;	break;
			case EMADD_DAMAGE:		arrVALUE[EMADD_DAMAGE] += nVALUE;		break;
			case EMADD_DEFENSE:		arrVALUE[EMADD_DEFENSE] += nVALUE;		break;
			case EMADD_STATS_POW:	arrVALUE[EMADD_STATS_POW] += nVALUE;	break;
			case EMADD_STATS_STR:	arrVALUE[EMADD_STATS_STR] += nVALUE;	break;
			case EMADD_STATS_SPI:	arrVALUE[EMADD_STATS_SPI] += nVALUE;	break;
			case EMADD_STATS_DEX:	arrVALUE[EMADD_STATS_DEX] += nVALUE;	break;
			case EMADD_STATS_INT:	arrVALUE[EMADD_STATS_INT] += nVALUE;	break;
			case EMADD_STATS_STA:	arrVALUE[EMADD_STATS_STA] += nVALUE;	break;
			case EMADD_PA:			arrVALUE[EMADD_PA] += nVALUE;			break;
			case EMADD_SA:			arrVALUE[EMADD_SA] += nVALUE;			break;
			};
		}
	}

	//	ÀüÃ¼ ¿É¼ÇÀ» µû·Î °è»ê
	arrVALUE[EMADD_HP] = sItemCustom.GETADDHP();
	arrVALUE[EMADD_MP] = sItemCustom.GETADDMP();
	arrVALUE[EMADD_SP] = sItemCustom.GETADDSP();
	arrVALUE[EMADD_MA] = sItemCustom.GETADDMA();

	for( int i=1; i<EMADD_SIZE; ++i)
	{
		if( arrVALUE[i] != 0 )
		{
			//	NOTE
			//		°¡»êÈ¿°ú°¡ Á¸ÀçÇÒ°æ¿ì¿¡¸¸ Å¸ÀÌÆ²À»
			//		Ãâ·ÂÇÏ±â À§ÇØ, Àû¾îµµ ÇÏ³ª°¡ Á¸ÀçÇÒ¶§
			//		»Ñ¸°´Ù´Â °ÍÀ» Ã¼Å©ÇÑ´Ù.
			if ( bLEAST_HAVE_ONE )
			{
				INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD ( "ITEM_CATEGORY", 10 ),NS_UITEXTCOLOR::LIGHTSKYBLUE);
				bLEAST_HAVE_ONE = FALSE;
			}

			strText.Format( "%s:%s", ID2GAMEWORD( "ITEM_ADDON_INFO", i ), INFO_DISPLAY_ITEM_GetNumberWithSign( arrVALUE[i] ) );
			INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::ORANGE );
		}
	}

	{
		SITEM* pItemData = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
		if ( pItemData )
		{
			// °æÇèÄ¡ ¹èÀ²
			float fExpMultiple = pItemData->GetExpMultiple();
			if( fExpMultiple != 1.0f )
			{
				strText.Format("%s %2.2f%s", 
					ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_EX", 0 ), 
					fExpMultiple, 
					ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_EX", 1 ) );

				INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
			}
		}
	}

	// special function
	bLEAST_HAVE_ONE = TRUE;
	for ( int i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
	{
		EMITEM_ADDON emTYPE = sSUIT.sADDON[i].emTYPE;
		if ( emTYPE < EMADDEX_INCR_NONE ) continue;

		if ( !sSUIT.sADDON[i].IsNoneType() )
		{
			if ( bLEAST_HAVE_ONE )
			{
				INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD ( "ITEM_CATEGORY", 5 ),NS_UITEXTCOLOR::LIGHTSKYBLUE);
				bLEAST_HAVE_ONE = FALSE;
			}

			if ( ITEM::bAddonValueInteger[emTYPE] )
			{
				int nVALUE = sSUIT.GetAddonValue_CompareExchange< const int >( i, ITEM::bAddonValueInteger[emTYPE] );

				strText.Format( "%s:%s", ID2GAMEWORD( "ITEM_ADDON_INFO", emTYPE ), INFO_DISPLAY_ITEM_GetNumberWithSign( nVALUE ) );
				INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::PRIVATE );
			}
			else
			{
				EMITEM_VAR emVarType = ConvertAddon2Var ( emTYPE );
				float fVALUE = sSUIT.GetAddonValue_CompareExchange< const float >( i, ITEM::bAddonValueInteger[emTYPE] );
				if ( COMMENT::IsITEMVAR_SCALE(emVarType) ) fVALUE *= COMMENT::ITEMVAR_SCALE[emVarType];

				strText.Format( "%s:%2.2f", ID2GAMEWORD( "ITEM_ADDON_INFO", emTYPE ), fVALUE );
				if ( COMMENT::IsITEMVAR_SCALE(emVarType) )		strText += "%";
				INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::PRIVATE );
			}
		}
	}

	//	ÄÚ½ºÆ¬ ´É·ÂÄ¡
	if ( sItemCustom.nidDISGUISE == NATIVEID_NULL() )	return;
	SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
	if ( pItem )
	{
		bLEAST_HAVE_ONE = TRUE;
		for ( int i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
		{
			EMITEM_ADDON emTYPE = pItem->sSuitOp.sADDON[i].emTYPE;

			if ( emTYPE > EMADDEX_INCR_NONE ) continue;

			if ( !pItem->sSuitOp.sADDON[i].IsNoneType() )
			{
				int nVALUE = pItem->sSuitOp.sADDON[i].nVALUE;

				if ( bLEAST_HAVE_ONE )
				{
					INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD ( "ITEM_CATEGORY", 10 ),NS_UITEXTCOLOR::LIGHTSKYBLUE);
					bLEAST_HAVE_ONE = FALSE;
				}

				strText.Format( "%s:%s", ID2GAMEWORD( "ITEM_ADDON_INFO", emTYPE ), INFO_DISPLAY_ITEM_GetNumberWithSign( nVALUE ) );
				INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::HOTPINK );

			}
		}

		// special function
		bLEAST_HAVE_ONE = TRUE;
		for ( int i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
		{
			EMITEM_ADDON emTYPE = pItem->sSuitOp.sADDON[i].emTYPE;
			if ( emTYPE < EMADDEX_INCR_NONE ) continue;

			if ( !pItem->sSuitOp.sADDON[i].IsNoneType() )
			{
				if ( bLEAST_HAVE_ONE )
				{
					INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD ( "ITEM_CATEGORY", 5 ),NS_UITEXTCOLOR::LIGHTSKYBLUE);
					bLEAST_HAVE_ONE = FALSE;
				}

				if ( ITEM::bAddonValueInteger[emTYPE] )
				{
					int nVALUE = pItem->sSuitOp.GetAddonValue_CompareExchange< const int >( i, ITEM::bAddonValueInteger[emTYPE] );

					strText.Format( "%s:%s", ID2GAMEWORD( "ITEM_ADDON_INFO", emTYPE ), INFO_DISPLAY_ITEM_GetNumberWithSign( nVALUE ) );
					INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::HOTPINK );
				}
				else
				{
					EMITEM_VAR emVarType = ConvertAddon2Var ( emTYPE );
					float fVALUE = pItem->sSuitOp.GetAddonValue_CompareExchange< const float >( i, ITEM::bAddonValueInteger[emTYPE] );
					if ( COMMENT::IsITEMVAR_SCALE(emVarType) )fVALUE *= COMMENT::ITEMVAR_SCALE[emVarType];

					strText.Format( "%s:%2.2f", ID2GAMEWORD( "ITEM_ADDON_INFO", emTYPE ), fVALUE );
					if ( COMMENT::IsITEMVAR_SCALE(emVarType) )		strText += "%";
					INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::HOTPINK );
				}
			}
		}
	}
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddInfoLunchBoxItem ( const SITEMCUSTOM &sItemCustom )
{
	CString strText;

	SNATIVEID    sNativeID   = sItemCustom.GetNativeID();
	GLCharacter* pCharacter  = m_pGaeaClient->GetCharacter();	
	const SITEM*       pItemData   = GLogicData::GetInstance().GetItem ( sNativeID );
	SNATIVEID    sSkillID    = pItemData->sSkillBookOp.sSkill_ID;
	WORD         wSkillLevel = pItemData->sSkillBookOp.wSkill_Level;

	//	Note : ½ºÅ³ Á¤º¸ °¡Á®¿È.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	if ( !pSkill )
	{
		return;
	}

	// Note : È¿°úÁ¤º¸ Ãâ·Â
	pSkill->GetDescFormatByLevel( strText, wSkillLevel, GLSKILL::DESC_FORMAT_IMPACT | GLSKILL::DESC_FORMAT_SPEC );

	INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "ITEM_CATEGORY", 12 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );
	INFO_DISPLAY_SKILL_AddTextLongestLineSplit(strText, NS_UITEXTCOLOR::DEFAULT);
}

void CInnerInterface::INFO_DISPLAY_ITEM_AddInfoPetDualSkillItem ( const SITEMCUSTOM &sItemCustom )
{
	GLPetClient* pPetClient = m_pGaeaClient->GetPetClient();

	//	ÀÌ¹Ì µà¾ó ½ºÅ³À» ¹è¿ü´Ù¸é
	if ( 2 <= pPetClient->m_wMaxSkillSlot )
	{
		INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_SKILL_CONDITION", 0 ), NS_UITEXTCOLOR::RED );	
		return ;
	}	
}

void CInnerInterface::INFO_DISPLAY_ITEM_LOAD_SIMPLE ( const SITEMCUSTOM &sItemCustom )
{
	if ( m_INFO_DISPLAY_ITEM_sItemCustomBACK == sItemCustom ) return ;

	m_INFO_DISPLAY_ITEM_sItemCustomBACK = sItemCustom;

	INFO_DISPLAY_ITEM_RemoveAllInfo();

	const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
	if ( !pItemData ) return ;

	DWORD dwLevel = pItemData->sBasicOp.emLevel;
	//	ÀÌ¸§
	INFO_DISPLAY_ITEM_AddTextNoSplit ( sItemCustom.GETNAME().c_str(), COMMENT::ITEMCOLOR[dwLevel] );
}

bool CInnerInterface::DisplayItemInformation_Durability(const SITEMCUSTOM& sItemCustom, const BOOL bShopOpen, const BOOL bInMarket, const BOOL bInPrivateMarket, const BOOL bUsePreview, const BOOL bIsWEAR_ITEM, const BOOL bIsDurable, WORD wPosX, WORD wPosY, SNATIVEID sNpcNativeID, int iPageIndex )
{
	const BOOL _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? TRUE : FALSE);
	if ( m_INFO_DISPLAY_ITEM_sItemCustomBACK == sItemCustom && m_INFO_DISPLAY_ITEM_bShopOpenBACK == bShopOpen
		&& m_INFO_DISPLAY_ITEM_bInMarketBACK == bInMarket && m_INFO_DISPLAY_ITEM_bInPrivateMarketBACK == bInPrivateMarket && m_INFO_DISPLAY_ITEM_bUsePreviewBACK == bUsePreview
		&& m_INFO_DISPLAY_ITEM_bIsWEAR_ITEMBACK == bIsWEAR_ITEM && m_INFO_DISPLAY_ITEM_wPosXBACK == wPosX && m_INFO_DISPLAY_ITEM_wPosYBACK == wPosY
		&& m_INFO_DISPLAY_ITEM_bControlBACK == _bShowMinMax )
		return false;

	m_INFO_DISPLAY_ITEM_sItemCustomBACK = sItemCustom;
	m_INFO_DISPLAY_ITEM_bShopOpenBACK = bShopOpen;
	m_INFO_DISPLAY_ITEM_bInMarketBACK = bInMarket;
	m_INFO_DISPLAY_ITEM_bInPrivateMarketBACK = bInPrivateMarket;
	m_INFO_DISPLAY_ITEM_bUsePreviewBACK = bUsePreview;
	m_INFO_DISPLAY_ITEM_bIsWEAR_ITEMBACK = bIsWEAR_ITEM;
	m_INFO_DISPLAY_ITEM_wPosXBACK = wPosX;
	m_INFO_DISPLAY_ITEM_wPosYBACK = wPosY;
	m_INFO_DISPLAY_ITEM_bControlBACK = _bShowMinMax;

	if ( bIsDurable )
	{
		m_pItemInforTooltipDurability ->ResetInformation();
		m_pItemInforTooltipDurability->DisplayItemDurability( sItemCustom, bShopOpen, bInMarket,
			bInPrivateMarket, bUsePreview, FALSE, TRUE, wPosX, wPosY, sNpcNativeID );
	}
	else
	{
		m_pItemInforTooltip->ResetInformation();
		m_pItemInforTooltip->DisplayItemDurability( sItemCustom, bShopOpen, bInMarket,
			bInPrivateMarket, bUsePreview, FALSE, FALSE, wPosX, wPosY, sNpcNativeID, iPageIndex );
	}	

	return true;
}

bool CInnerInterface::DisplayItemInformation(const SITEMCUSTOM& sItemCustom, const BOOL bShopOpen, const BOOL bInMarket, const BOOL bInPrivateMarket, const BOOL bUsePreview, const BOOL bIsWEAR_ITEM, WORD wPosX, WORD wPosY, SNATIVEID sNpcNativeID, int iPageIndex )
{
	const BOOL _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? TRUE : FALSE);
    if ( m_INFO_DISPLAY_ITEM_sItemCustomBACK == sItemCustom && m_INFO_DISPLAY_ITEM_bShopOpenBACK == bShopOpen
        && m_INFO_DISPLAY_ITEM_bInMarketBACK == bInMarket && m_INFO_DISPLAY_ITEM_bInPrivateMarketBACK == bInPrivateMarket && m_INFO_DISPLAY_ITEM_bUsePreviewBACK == bUsePreview
        && m_INFO_DISPLAY_ITEM_bIsWEAR_ITEMBACK == bIsWEAR_ITEM && m_INFO_DISPLAY_ITEM_wPosXBACK == wPosX && m_INFO_DISPLAY_ITEM_wPosYBACK == wPosY
		&& m_INFO_DISPLAY_ITEM_bControlBACK == _bShowMinMax )
		return false;

    m_INFO_DISPLAY_ITEM_sItemCustomBACK = sItemCustom;
    m_INFO_DISPLAY_ITEM_bShopOpenBACK = bShopOpen;
    m_INFO_DISPLAY_ITEM_bInMarketBACK = bInMarket;
    m_INFO_DISPLAY_ITEM_bInPrivateMarketBACK = bInPrivateMarket;
    m_INFO_DISPLAY_ITEM_bUsePreviewBACK = bUsePreview;
    m_INFO_DISPLAY_ITEM_bIsWEAR_ITEMBACK = bIsWEAR_ITEM;
    m_INFO_DISPLAY_ITEM_wPosXBACK = wPosX;
    m_INFO_DISPLAY_ITEM_wPosYBACK = wPosY;
	m_INFO_DISPLAY_ITEM_bControlBACK = _bShowMinMax;

    if ( bIsWEAR_ITEM )
    {
        m_pItemInforTooltipWear->ResetInformation();
        m_pItemInforTooltipWear->DisplayItemInformation( sItemCustom, bShopOpen, bInMarket,
            bInPrivateMarket, bUsePreview, TRUE, wPosX, wPosY, sNpcNativeID );
    }
    else
    {
        m_pItemInforTooltip->ResetInformation();
        m_pItemInforTooltip->DisplayItemInformation( sItemCustom, bShopOpen, bInMarket,
            bInPrivateMarket, bUsePreview, FALSE, wPosX, wPosY, sNpcNativeID, iPageIndex );
    }	

	return true;
}

void CInnerInterface::DisplayItemInformationLink(const SITEMCUSTOM& sItemCustom, const BOOL bShopOpen, const BOOL bInMarket, const BOOL bInPrivateMarket, const BOOL bUsePreview, const BOOL bIsWEAR_ITEM, WORD wPosX, WORD wPosY, SNATIVEID sNpcNativeID )
{
	const BOOL _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? TRUE : FALSE);
	if ( m_INFO_DISPLAY_ITEM_sItemCustomBACK == sItemCustom && m_INFO_DISPLAY_ITEM_bShopOpenBACK == bShopOpen
		&& m_INFO_DISPLAY_ITEM_bInMarketBACK == bInMarket && m_INFO_DISPLAY_ITEM_bInPrivateMarketBACK == bInPrivateMarket && m_INFO_DISPLAY_ITEM_bUsePreviewBACK == bUsePreview
		&& m_INFO_DISPLAY_ITEM_bIsWEAR_ITEMBACK == bIsWEAR_ITEM && m_INFO_DISPLAY_ITEM_wPosXBACK == wPosX && m_INFO_DISPLAY_ITEM_wPosYBACK == wPosY
		&& m_INFO_DISPLAY_ITEM_bControlBACK == _bShowMinMax )
		return;

	m_INFO_DISPLAY_ITEM_sItemCustomBACK = sItemCustom;
	m_INFO_DISPLAY_ITEM_bShopOpenBACK = bShopOpen;
	m_INFO_DISPLAY_ITEM_bInMarketBACK = bInMarket;
	m_INFO_DISPLAY_ITEM_bInPrivateMarketBACK = bInPrivateMarket;
	m_INFO_DISPLAY_ITEM_bUsePreviewBACK = bUsePreview;
	m_INFO_DISPLAY_ITEM_bIsWEAR_ITEMBACK = bIsWEAR_ITEM;
	m_INFO_DISPLAY_ITEM_wPosXBACK = wPosX;
	m_INFO_DISPLAY_ITEM_wPosYBACK = wPosY;
	m_INFO_DISPLAY_ITEM_bControlBACK = _bShowMinMax;

	m_pItemInforTooltipLink->ResetInformation();
	m_pItemInforTooltipLink->DisplayItemInformation( sItemCustom, bShopOpen, bInMarket,
		bInPrivateMarket, bUsePreview, FALSE, wPosX, wPosY, sNpcNativeID );
}

void CInnerInterface::DisplayWishListItemInfo( WishList::ItemSPtr pWishItem )
{
	if( m_INFO_DISPLAY_ITEM_sItemCustomBACK == pWishItem->m_sItem )
		return;

    m_INFO_DISPLAY_ITEM_sItemCustomBACK = pWishItem->m_sItem;

	m_pItemInforTooltip->ResetInformation();
	m_pItemInforTooltip->DisplayWishListItemInfo( pWishItem );
}

void CInnerInterface::INFO_DISPLAY_ITEM_LOAD ( const SITEMCUSTOM &sItemCustom, const BOOL bShopOpen, const BOOL bInMarket, const BOOL bInPrivateMarket,  const BOOL bUsePreview, const BOOL bIsWEAR_ITEM, WORD wPosX, WORD wPosY, SNATIVEID sNpcNativeID, int  iPageIndex )
{	
	if ( m_INFO_DISPLAY_ITEM_sItemCustomBACK == sItemCustom && m_INFO_DISPLAY_ITEM_bShopOpenBACK == bShopOpen
		&& m_INFO_DISPLAY_ITEM_bInMarketBACK == bInMarket && m_INFO_DISPLAY_ITEM_bInPrivateMarketBACK == bInPrivateMarket && m_INFO_DISPLAY_ITEM_bUsePreviewBACK == bUsePreview
		&& m_INFO_DISPLAY_ITEM_bIsWEAR_ITEMBACK == bIsWEAR_ITEM && m_INFO_DISPLAY_ITEM_wPosXBACK == wPosX && m_INFO_DISPLAY_ITEM_wPosYBACK == wPosY)	return ;

	m_INFO_DISPLAY_ITEM_sItemCustomBACK = sItemCustom;
	m_INFO_DISPLAY_ITEM_bShopOpenBACK = bShopOpen;
	m_INFO_DISPLAY_ITEM_bInMarketBACK = bInMarket;
	m_INFO_DISPLAY_ITEM_bInPrivateMarketBACK = bInPrivateMarket;
    m_INFO_DISPLAY_ITEM_bUsePreviewBACK = bUsePreview;
	m_INFO_DISPLAY_ITEM_bIsWEAR_ITEMBACK = bIsWEAR_ITEM;
	m_INFO_DISPLAY_ITEM_wPosXBACK = wPosX;
	m_INFO_DISPLAY_ITEM_wPosYBACK = wPosY;

	INFO_DISPLAY_ITEM_RemoveAllInfo();

	if ( bIsWEAR_ITEM )
	{
		INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ("WEAR_ITEM" ), NS_UITEXTCOLOR::RED );
	}

	CString strText, strText2;
	BOOL bValue = FALSE;
	int nExtraValue = 0;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	SITEM* pItemData = GLogicData::GetInstance().GetItem ( sItemCustom.GetBoxWrappedItemID() );

	if ( !pItemData ) return;
	{
		INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_CATEGORY", 0 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//	BasicInfo
		//	1.¾ÆÀÌÅÛ ÀÌ¸§		
		BOOL bInsert = FALSE;

		strText.Format ( "%s", pItemData->GetName() );
		BYTE uGRADE = sItemCustom.GETGRADE(EMGRINDING_NUNE);
		INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( strText, uGRADE );
		//			INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::PALEGREEN );

		DWORD dwLevel = (DWORD)pItemData->sBasicOp.emLevel;
		INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, COMMENT::ITEMCOLOR[dwLevel] );
		

		//	ÄÚ½ºÃã
		if ( sItemCustom.nidDISGUISE!=SNATIVEID(false) )
		{
			const SITEM* pDisguiseData = GLogicData::GetInstance().GetItem ( sItemCustom.nidDISGUISE );

			strText.Format ( "%s:%s", ID2GAMEWORD ( "ITEM_BASIC_INFO", 3 ), pDisguiseData->GetName() );			
			INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::GREENYELLOW );

			//	ÄÚ½ºÆ¬ »ö±ò Á¤º¸. by luxes.
			strText.Format ( "%s", ID2GAMEWORD ( "ITEM_BASIC_INFO", 10 ) );			
			INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::LIGHTSKYBLUE );

			if ( pDisguiseData->IsColorChange() )
			{
				strText.Format ( "%s", ID2GAMEWORD ( "ITEM_SUIT_COLOR_CHANGE", 0 ) );
				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::GREENYELLOW );
			}
			else
			{
				strText.Format ( "%s", ID2GAMEWORD ( "ITEM_SUIT_COLOR_CHANGE", 1 ) );
				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::RED );
			}
		}

		//	ÄÚ½ºÆ¬ »ö±ò Á¤º¸. by luxes.
		else
		{
			//if ( pItemData->sBasicOp.IsDISGUISE() )
			if ( sItemCustom.IsDisguise() != NULL )
			{
				strText.Format ( "%s", ID2GAMEWORD ( "ITEM_BASIC_INFO", 10 ) );			
				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::LIGHTSKYBLUE );

				if ( pItemData->IsColorChange() )
				{
					strText.Format ( "%s", ID2GAMEWORD ( "ITEM_SUIT_COLOR_CHANGE", 0 ) );
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::GREENYELLOW );
				}
				else
				{
					strText.Format ( "%s", ID2GAMEWORD ( "ITEM_SUIT_COLOR_CHANGE", 1 ) );
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::RED );
				}
			}
		}

		//	ÄÚ½ºÃõ ±â°£ Ç¥½Ã
		if ( sItemCustom.tDISGUISE!=0 )
		{				
			CTime cTime(sItemCustom.tDISGUISE + sItemCustom.GetPeriodExDisguise());

			//	¼­¹ö Å¸ÀÓÀ¸·Î º¯È¯( TimeZone °è»ê )
			m_pGaeaClient->GetConvertServerTime( cTime );

			if ( cTime.GetYear() > 1970 )
			{
				CString strExpireDate;

				strExpireDate = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
					(cTime.GetYear ()%2000),  cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() ).c_str();

				strText.Format ( "%s:%s", ID2GAMEWORD ( "ITEM_BASIC_INFO", 7 ), strExpireDate.GetString() );			
				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::DARKORANGE );
			}
		}

		//	ÄÚ½ºÃõ ±â°£ Ç¥½Ã
		if ( sItemCustom.IsTempEnchant() )
		{				
			CTime cTime( sItemCustom.sTLGrind.tFireTime );

			if ( cTime.GetYear() > 1970 )
			{
				CString strExpireDate;

				BYTE uGRADE = sItemCustom.GETGRADE(EMGRINDING_NUNE);
				INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( strExpireDate, uGRADE );
				strExpireDate += " ";

				strExpireDate += sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
					(cTime.GetYear ()%2000),  cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() ).c_str();

				strText.Format ( "%s: %s", ID2GAMEWORD ( "ITEM_BASIC_INFO", 22 ), strExpireDate.GetString() );			
				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::DARKORANGE );
			}
		}

		LONGLONG dwCOMMISSION_MONEY = 0;

		//	2.°¡°Ý
		if ( pItemData->sBasicOp.dwBuyPrice || bInPrivateMarket )
		{
			if ( bInPrivateMarket )
			{
				bool bOPENER;
				DWORD dwPrivateMarketID;
				GetPrivateMarketInfo ( bOPENER, dwPrivateMarketID );
				if ( bOPENER )	//	ÆÄ´Â »ç¶÷
				{
					GLPrivateMarket &sPMarket = m_pGaeaClient->GetCharacter()->m_sPMarket;

					const SSALEITEM *pSALE = sPMarket.GetItem ( SNATIVEID(wPosX,wPosY) ); 
					if ( pSALE )
					{
						CString strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( pSALE->llPRICE, 3, "," );
						strText.Format ( "%s:%s", ID2GAMEWORD ( "ITEM_BASIC_INFO", 5 ), strMoney );			
                        INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, MONEYCOLOR::GetMoneyColor(pSALE->llPRICE) );
					}						
				}
				else		//	»ç´Â »ç¶÷
				{
					std::tr1::shared_ptr<GLCharClient> pCLIENT = m_pGaeaClient->GetChar ( dwPrivateMarketID );
					if ( !pCLIENT )
                        return;

					GLPrivateMarket &sPMarket = pCLIENT->m_sPMarket;

					const SSALEITEM *pSALE = sPMarket.GetItem ( SNATIVEID(wPosX,wPosY) ); 
					if ( pSALE )
					{
						CString strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( pSALE->llPRICE, 3, "," );
						strText.Format ( "%s:%s", ID2GAMEWORD ( "ITEM_BASIC_INFO", 5 ), strMoney );			
						INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, MONEYCOLOR::GetMoneyColor(pSALE->llPRICE) );
					}
				}
			}				
			else if ( bShopOpen && bInMarket )	// »ì¶§ °¡°Ý
			{
				LONGLONG dwNpcSellPrice = 0;

				volatile LONGLONG dwPrice = 0;
				volatile double fSHOP_RATE = m_pGaeaClient->GetCharacter()->GetBuyRate();

				if( sNpcNativeID.wMainID != 0 && sNpcNativeID.wSubID != 0 )
				{
					const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sNpcNativeID );
					if( pCrowData != NULL )
					{
						LONGLONG dwNpcPrice = pCrowData->GetNpcSellPrice( pItemData->sBasicOp.sNativeID.dwID );
						if( dwNpcPrice == 0 )
						{								
							dwNpcSellPrice = pItemData->sBasicOp.dwBuyPrice;
                            dwPrice = dwNpcSellPrice * static_cast<LONGLONG>(fSHOP_RATE) / 100;
						}else{
							dwNpcSellPrice = dwNpcPrice;
							dwPrice = dwNpcSellPrice;								
						}
					}

				}				

				CString strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( dwPrice, 3, "," );

				strText.Format( "%s:%s", ID2GAMEWORD("ITEM_BASIC_INFO", 1), strMoney );

				//	Ä¿¹Ì¼Ç ¾×¼ö
				dwCOMMISSION_MONEY = static_cast<LONGLONG>(dwPrice - dwNpcSellPrice);
                
                LONGLONG lMoney  = m_pGaeaClient->GetCharacterLogic().GetInvenMoney();
				D3DCOLOR dwColor = NS_UITEXTCOLOR::RED;

				if( dwPrice <= lMoney )
				{
					dwColor = MONEYCOLOR::GetMoneyColor((LONGLONG)dwPrice);//NS_UITEXTCOLOR::PALEGREEN;
				}
				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, dwColor );
			}
			else if ( bShopOpen ) // ÆÈ¶§ °¡°Ý
			{
				volatile float fSHOP_RATE = m_pGaeaClient->GetCharacter()->GetSaleRate();
				volatile float fSALE_DISCOUNT = fSHOP_RATE * 0.01f;

				volatile LONGLONG dwPrice = pItemData->GETSELLPRICE ( sItemCustom.wTurnNum );
				volatile LONGLONG dwSALE_PRICE = DWORD ( dwPrice * fSALE_DISCOUNT );					

				//	Ä¿¹Ì¼Ç ¾×¼ö
				volatile LONGLONG dwDISPRICE = pItemData->GETSELLPRICE ( sItemCustom.wTurnNum );
				dwCOMMISSION_MONEY = dwDISPRICE - dwSALE_PRICE;

				CString strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( dwSALE_PRICE, 3, "," );
				strText.Format( "%s:%s", ID2GAMEWORD("ITEM_BASIC_INFO", 1), strMoney );

				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, MONEYCOLOR::GetMoneyColor((LONGLONG)dwSALE_PRICE));
			}
		}

		//	¼ö¼ö·á Ç¥½Ã
		if ( dwCOMMISSION_MONEY )
		{
			CString strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT ( dwCOMMISSION_MONEY, 3, "," );
			strText.Format( "%s:%s", ID2GAMEWORD("ITEM_BASIC_INFO", 6), strMoney );

			INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::PALEGREEN );
		}

		//	¼ö·®Ç¥½Ã
		INFO_DISPLAY_ITEM_AddItemTurnInfo ( sItemCustom, bInMarket, bInPrivateMarket );

		//	±â°£Ç¥½Ã
		//if ( pItemData->IsTIMELMT () )
		if ( sItemCustom.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL )  )
		{
			CTime cTime(sItemCustom.tBORNTIME);

			//	¼­¹ö Å¸ÀÓÀ¸·Î º¯È¯( TimeZone °è»ê )
			m_pGaeaClient->GetConvertServerTime( cTime );
			if ( cTime.GetYear() > 1970 )
			{
				CTimeSpan sLMT(pItemData->sDrugOp.tTIME_LMT);
				cTime += sLMT + sItemCustom.GetPeriodExBasic();

				CString strExpireDate, strPeriodEx;

				strExpireDate = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
					(cTime.GetYear ()%2000), cTime.GetMonth (), cTime.GetDay (), cTime.GetHour (), cTime.GetMinute () ).c_str();

				strText.Format ( "%s:%s", ID2GAMEWORD ( "ITEM_BASIC_INFO", 4 ), strExpireDate.GetString() );			
				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::DARKORANGE );
			}
		}

		//	Âø¿ëÁ¶°Ç->¿ä±¸ ·¹º§
		if ( pItemData->sBasicOp.wReqLevelDW || pItemData->sBasicOp.wReqLevelUP )
		{
			bool bReqLevel = true;
            strText.Format( "%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 0), 
                pItemData->sBasicOp.wReqLevelDW );

			if ( pItemData->sBasicOp.wReqLevelUP )
			{
				strText2.Format ( "~%d",pItemData->sBasicOp.wReqLevelUP );
				strText += strText2; 
				bReqLevel = (pItemData->sBasicOp.wReqLevelUP >= pCharacter->m_wLevel);
			}

			INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pItemData->sBasicOp.wReqLevelDW <= pCharacter->m_wLevel && bReqLevel ) );
		}

		//	Àç»ç¿ë ´ë±â½Ã°£
		if ( pItemData->sBasicOp.IsCoolTime() )
		{
			CString strTime = "";
			CTimeSpan cCoolTime( pItemData->sBasicOp.dwCoolTime );

			if ( cCoolTime.GetHours() > 0 )	
				strTime += MakeString( "%d%s ", cCoolTime.GetHours(), ID2GAMEWORD ( "ITEM_BASIC_TIME", 0 ) );
			if ( cCoolTime.GetMinutes() > 0 )	
				strTime += MakeString( "%d%s ", cCoolTime.GetMinutes(), ID2GAMEWORD ( "ITEM_BASIC_TIME", 1 ) );
			if ( cCoolTime.GetSeconds() > 0 )	
				strTime += MakeString( "%d%s ", cCoolTime.GetSeconds(), ID2GAMEWORD ( "ITEM_BASIC_TIME", 2 ) );

			strText.Format ( "%s : %s", ID2GAMEWORD ( "ITEM_BASIC_INFO", 8 ), strTime );			
			INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::DARKORANGE );	
		}

		//	³²Àº½Ã°£


		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if ( pCharacter && pCharacter->IsCoolTime( pItemData->sBasicOp.sNativeID ) )
		{
			CString strTime = "";
			__time64_t tCurTime =  m_pGaeaClient->GetCurrentTime().GetTime();
			__time64_t tCoolTime = pCharacter->GetMaxCoolTime( pItemData->sBasicOp.sNativeID );				

			CTimeSpan cReTime( tCoolTime - tCurTime );

			if ( cReTime.GetHours() > 0 )	
				strTime += MakeString( "%d%s ", cReTime.GetHours(), ID2GAMEWORD ( "ITEM_BASIC_TIME", 0 ) );
			if ( cReTime.GetMinutes() > 0 )	
				strTime += MakeString( "%d%s ", cReTime.GetMinutes(), ID2GAMEWORD ( "ITEM_BASIC_TIME", 1 ) );
			if ( cReTime.GetSeconds() > 0 )	
				strTime += MakeString( "%d%s ", cReTime.GetSeconds(), ID2GAMEWORD ( "ITEM_BASIC_TIME", 2 ) );

			strText.Format ( "%s : %s", ID2GAMEWORD ( "ITEM_BASIC_INFO", 9 ), strTime );			
			INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::DARKORANGE );
		}

		if( pItemData->sDrugOp.dwERList > 0 )
		{
			CString str = "";
			CString str1 = "";

			for( int i = 0; i < ITEM_DRUG_ER_SIZE; ++i )
			{
				DWORD dwValue = 1 << i;

				if( dwValue & pItemData->sDrugOp.dwERList )
				{
					if( i > 0 )
						str += " + ";

					EMITEM_DRUG_ER eType = (EMITEM_DRUG_ER)i;

					switch( eType )
					{
					case ITEM_DRUG_ER_STUN        : str += MakeString ( "%s", ID2GAMEWORD ( "ITEM_INFO_SET_BONUS", 26)); break;
					case ITEM_DRUG_ER_HALLUCINATE : str += MakeString ( "%s", ID2GAMEWORD ( "ITEM_INFO_SET_BONUS", 49)); break;
					case ITEM_DRUG_ER_NON_DRUG    : str += MakeString ( "%s", ID2GAMEWORD ( "ITEM_INFO_SET_BONUS", 34)); break;
					}
				}
			}

			if( !str.IsEmpty() )
			{
				str1.Format ( "%s : %s", ID2GAMEWORD ( "ITEM_BASIC_INFO", 21 ), str );
				INFO_DISPLAY_ITEM_AddTextNoSplit( str1, ITEM_INFOR_COLOR::dwDefault );
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//	AdvInfo
		const SCHARSTATS& rItemStats = pItemData->sBasicOp.sReqStats;
		const SCHARSTATS& rCharStats = pCharacter->m_sSUMSTATS;
		EMITEM_TYPE emItemType = pItemData->sBasicOp.emItemType;

		switch ( emItemType )
		{
		case ITEM_SUIT:
		case ITEM_REVIVE:
		case ITEM_ANTI_DISAPPEAR:
		case ITEM_INC_GRINDINGRATE:
		case ITEM_ANTI_RESET:
			{
				INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_CATEGORY", 1 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );

				BYTE uGRADE = 0;

				//	°ø°Ý·Â
				GLPADATA &sDamage = sItemCustom.getdamage();
				nExtraValue = sItemCustom.GETGRADE_DAMAGE();
				uGRADE = sItemCustom.GETGRADE(EMGRINDING_DAMAGE);
				INFO_DISPLAY_ITEM_AddInfoItemAddonRange ( sDamage.wLow, sDamage.wHigh, nExtraValue, uGRADE, ID2GAMEWORD("ITEM_ADVANCED_INFO", 0) );

				//	±â·ÂÄ¡? (¾Æ·¡¿¡ º¸¸é EMGRINDING_DAMAGE °ªÀ» °¡Á®¿Â´Ù. ¿Ö?)
				nExtraValue = sItemCustom.GETGRADE_DAMAGE();
				if ( nExtraValue )
				{
					uGRADE = sItemCustom.GETGRADE(EMGRINDING_DAMAGE);
					strText.Format ( "%s:+%d", ID2GAMEWORD("ITEM_ADVANCED_INFO", 8), nExtraValue );
					INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( strText, uGRADE );
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::PRIVATE );
				}

				EMCHARINDEX emCharIdx = GLCI_NUM_ETC;
				int nLevel = -1;
				if ( pCharacter )
				{
					emCharIdx = pCharacter->GetCharIndex();
					nLevel = pCharacter->GETLEVEL();
				}

				//	¹æ¾î·Â
				short nDefense = sItemCustom.getdefense(emCharIdx, nLevel);
				nExtraValue = sItemCustom.GETGRADE_DEFENSE();
				uGRADE = sItemCustom.GETGRADE(EMGRINDING_DEFENSE);
				INFO_DISPLAY_ITEM_AddInfoItemAddon ( nDefense, nExtraValue, uGRADE, ID2GAMEWORD("ITEM_ADVANCED_INFO", 1) );

				//	°ø°Ý °Å¸®
				if ( sItemCustom.GETATTRANGE() )
				{
					strText.Format("%s:%d",ID2GAMEWORD("ITEM_ADVANCED_INFO", 2), sItemCustom.GETATTRANGE() );
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::DEFAULT );
				}

				//	¸íÁß·ü
				if ( sItemCustom.GETHITRATE() )
				{
					nExtraValue = 0;
					INFO_DISPLAY_ITEM_AddInfoItemAddon ( sItemCustom.GETHITRATE(), nExtraValue, ID2GAMEWORD("ITEM_ADVANCED_INFO", 3) );		
				}

				//	È¸ÇÇÀ²
				if ( sItemCustom.GETAVOIDRATE() )
				{
					nExtraValue = 0;
					INFO_DISPLAY_ITEM_AddInfoItemAddon ( sItemCustom.GETAVOIDRATE(), nExtraValue, ID2GAMEWORD("ITEM_ADVANCED_INFO", 4) );
				}

				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					//	Âø¿ëÀ§Ä¡
					strText.Format("%s:%s ", ID2GAMEWORD("ITEM_ADVANCED_INFO", 5), CGameTextMan::GetInstance().GetCommentText("ITEMSUIT", pItemData->sSuitOp.emSuit).GetString() );
					INFO_DISPLAY_ITEM_AddTextNoSplit (strText, NS_UITEXTCOLOR::DEFAULT );

					//	°ø°Ý ¼Ó¼º
					if ( pItemData->sSuitOp.emAttack != ITEMATT_NOTHING )
					{
						strText.Format("%s:%s", ID2GAMEWORD("ITEM_ADVANCED_INFO", 6), CGameTextMan::GetInstance().GetCommentText("ITEMATTACK", pItemData->sSuitOp.emAttack).GetString() );
						INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::DEFAULT );
					}
				}
				else
				{
					//	Âø¿ëÀ§Ä¡
					strText.Format("%s:%s ", ID2GAMEWORD("ITEM_ADVANCED_INFO", 5), COMMENT::ITEMSUIT[pItemData->sSuitOp.emSuit].c_str() );
					INFO_DISPLAY_ITEM_AddTextNoSplit (strText, NS_UITEXTCOLOR::DEFAULT );

					//	°ø°Ý ¼Ó¼º
					if ( pItemData->sSuitOp.emAttack != ITEMATT_NOTHING )
					{
						strText.Format("%s:%s", ID2GAMEWORD("ITEM_ADVANCED_INFO", 6), COMMENT::ITEMATTACK[pItemData->sSuitOp.emAttack].c_str() );
						INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::DEFAULT );
					}
				}

				//	SP ¼Ò¸ð
				const WORD wReqSP = sItemCustom.GETREQ_SP();
				if ( 0 < wReqSP )
				{
					strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO", 7), wReqSP );
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::ORANGE );
				}

				//	Âø¿ëÁ¶°Ç-------------------------------------------------------------------------------------
				//	Âø¿ëÁ¶°Ç-Âø¿ë°¡´É Å¬·¡½º
				INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD ( "ITEM_CATEGORY", 2 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );

				if ( pItemData->sBasicOp.dwReqSchool!=GLSCHOOL_ALL )
				{
					CString strSchool;
					if ( pItemData->sBasicOp.dwReqSchool & GLSCHOOL_00)
					{
						strSchool+=GLCONST_CHAR::strSCHOOLNAME[school2index(GLSCHOOL_00)].c_str();
						strSchool+=" ";
					}
					if ( pItemData->sBasicOp.dwReqSchool & GLSCHOOL_01)
					{
						strSchool+=GLCONST_CHAR::strSCHOOLNAME[school2index(GLSCHOOL_01)].c_str();
						strSchool+=" ";
					}
					if ( pItemData->sBasicOp.dwReqSchool & GLSCHOOL_02)
					{
						strSchool+=GLCONST_CHAR::strSCHOOLNAME[school2index(GLSCHOOL_02)].c_str();
						strSchool+=" ";
					}						

					INFO_DISPLAY_ITEM_AddTextNoSplit ( strSchool, NS_UITEXTCONTROL::GetEvaluateColor ( pItemData->sBasicOp.dwReqSchool & index2school(pCharacter->m_wSchool) ) );
				}
				// Info : 착용 클래스
				{
					std::string strClass;
					DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
					if( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
					{
						const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
						if ( !pItemDisguise )   return;

						DWORD dwReqCharClass_Disguise = pItemDisguise->sBasicOp.dwReqCharClass;
						DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

						if ( (dwReqCharClass&m_pGaeaClient->GetCharacter()->m_emClass) && (dwReqCharClass_Disguise&m_pGaeaClient->GetCharacter()->m_emClass) )
							dwColor = ITEM_INFOR_COLOR::dwDefault;
						else
							dwColor = ITEM_INFOR_COLOR::dwReqWrong;

						if( dwReqCharClass )
						{
							if ( (dwReqCharClass==GLCC_ALL_TRICKER) && (dwReqCharClass_Disguise==GLCC_ALL_TRICKER)
								|| (dwReqCharClass==GLCC_ALL_SCIENTIST) && (dwReqCharClass_Disguise==GLCC_ALL_SCIENTIST)
								|| (dwReqCharClass==GLCC_ALL_ASSASSIN) && (dwReqCharClass_Disguise==GLCC_ALL_ASSASSIN)
								|| (dwReqCharClass==GLCC_ALL_ACTOR) && (dwReqCharClass_Disguise==GLCC_ALL_ACTOR)
								|| (dwReqCharClass == GLCC_MAN) && (dwReqCharClass_Disguise == GLCC_MAN)
								|| (dwReqCharClass == GLCC_WOMAN) && (dwReqCharClass_Disguise == GLCC_WOMAN) )
							{
								strClass = ID2GAMEWORD("ITEM_ALL_CLASS");
							}
							else
							{
								if ( (dwReqCharClass&GLCC_FIGHTER_M) && (dwReqCharClass_Disguise&GLCC_FIGHTER_M)
									|| (dwReqCharClass&GLCC_FIGHTER_W) && (dwReqCharClass_Disguise&GLCC_FIGHTER_W) )
								{
									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_FIGHTER_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass&GLCC_ARMS_M)  && (dwReqCharClass_Disguise&GLCC_ARMS_M)
									|| (dwReqCharClass&GLCC_ARMS_W)  && (dwReqCharClass_Disguise&GLCC_ARMS_W) )
								{
									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARMS_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass&GLCC_ARCHER_M)  && (dwReqCharClass_Disguise&GLCC_ARCHER_M)
									|| (dwReqCharClass&GLCC_ARCHER_W)  && (dwReqCharClass_Disguise&GLCC_ARCHER_W) )
								{
									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS",
										CharClassToClassIndex( GLCC_ARCHER_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass&GLCC_SPIRIT_M)  && (dwReqCharClass_Disguise&GLCC_SPIRIT_M)
									|| (dwReqCharClass&GLCC_SPIRIT_W)  && (dwReqCharClass_Disguise&GLCC_SPIRIT_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SPIRIT_M ));
									strClass += " ";
								}

								if ( (dwReqCharClass&GLCC_EXTREME_M)  && (dwReqCharClass_Disguise&GLCC_EXTREME_M)
									|| (dwReqCharClass&GLCC_EXTREME_W)  && (dwReqCharClass_Disguise&GLCC_EXTREME_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_EXTREME_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass&GLCC_SCIENTIST_M)  && (dwReqCharClass_Disguise&GLCC_SCIENTIST_M)
									|| (dwReqCharClass&GLCC_SCIENTIST_W)  && (dwReqCharClass_Disguise&GLCC_SCIENTIST_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SCIENTIST_M ) );
									strClass += " ";
								}

								/*if ( (dwReqCharClass&GLCC_ASSASSIN_M)  && (dwReqCharClass_Disguise&GLCC_ASSASSIN_M)
									|| (dwReqCharClass&GLCC_ASSASSIN_W)  && (dwReqCharClass_Disguise&GLCC_ASSASSIN_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit( 
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ASSASSIN_M ) ),
										dwColor );
								}

								if ( (dwReqCharClass&GLCC_TRICKER_M)  && (dwReqCharClass_Disguise&GLCC_TRICKER_M)
									|| (dwReqCharClass&GLCC_TRICKER_W)  && (dwReqCharClass_Disguise&GLCC_TRICKER_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit( 
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_TRICKER_M ) ),
										dwColor );
								}*/

								/*if ( (dwReqCharClass&GLCC_ETC_M)  && (dwReqCharClass_Disguise&GLCC_ETC_M)
									|| (dwReqCharClass&GLCC_ETC_W)  && (dwReqCharClass_Disguise&GLCC_ETC_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit(
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ETC_M ) ),
										dwColor );
								}*/
							}
							INFO_DISPLAY_ITEM_AddTextNoSplit(strClass.c_str(), dwColor );
						}
					}
					else
					{
						std::string strClass;
						DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

						if ( (dwReqCharClass&m_pGaeaClient->GetCharacter()->m_emClass) )
							dwColor = ITEM_INFOR_COLOR::dwDefault;
						else
							dwColor = ITEM_INFOR_COLOR::dwReqWrong;

						if( dwReqCharClass )
						{
							if ( dwReqCharClass==GLCC_ALL_TRICKER
								|| dwReqCharClass==GLCC_ALL_ASSASSIN
								|| dwReqCharClass==GLCC_ALL_SCIENTIST
								|| dwReqCharClass==GLCC_ALL_ACTOR
								|| (dwReqCharClass == GLCC_MAN)
								|| (dwReqCharClass == GLCC_WOMAN) )
							{
								strClass = ID2GAMEWORD("ITEM_ALL_CLASS");
							}
							else
							{
								bool bDistinction = false;
								if ( (dwReqCharClass & GLCC_FIGHTER_M) 
									|| (dwReqCharClass & GLCC_FIGHTER_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_FIGHTER_M ));
									strClass += " ";
								}

								if ( (dwReqCharClass & GLCC_ARMS_M) 
									|| (dwReqCharClass & GLCC_ARMS_W) )
								{
									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARMS_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass & GLCC_ARCHER_M)
									|| (dwReqCharClass & GLCC_ARCHER_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARCHER_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass & GLCC_SPIRIT_M)
									|| (dwReqCharClass & GLCC_SPIRIT_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SPIRIT_M ) );
									strClass += " ";
								}
								if ( (dwReqCharClass & GLCC_EXTREME_M)
									|| (dwReqCharClass & GLCC_EXTREME_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_EXTREME_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass & GLCC_SCIENTIST_M)
									|| (dwReqCharClass & GLCC_SCIENTIST_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SCIENTIST_M ) );
									strClass += " ";
								}

								/*if ( (dwReqCharClass & GLCC_ASSASSIN_M)
									|| (dwReqCharClass & GLCC_ASSASSIN_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit( 
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ASSASSIN_M ) ),
										dwColor );
								}

								if ( (dwReqCharClass & GLCC_TRICKER_M)
									|| (dwReqCharClass & GLCC_TRICKER_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit( 
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_TRICKER_M ) ),
										dwColor );
								}

								if ( (dwReqCharClass & GLCC_ETC_M)
									|| (dwReqCharClass & GLCC_ETC_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit(
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ETC_M ) ),
										dwColor );
								}*/
							}
						}
						INFO_DISPLAY_ITEM_AddTextNoSplit(strClass.c_str(), dwColor );
					}
				}

				// Info : 성별
				{
					std::string strClass;
					DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
					if( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
					{
						const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
						if ( !pItemDisguise )   return;

						DWORD dwReqCharClass_Disguise = pItemDisguise->sBasicOp.dwReqCharClass;
						DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

						if ( ( (dwReqCharClass!=GLCC_ALL_SCIENTIST) || (dwReqCharClass_Disguise!=GLCC_ALL_SCIENTIST) ) &&
							 (dwReqCharClass!=GLCC_ALL_ASSASSIN) || (dwReqCharClass_Disguise!=GLCC_ALL_ASSASSIN) &&
							 (dwReqCharClass!=GLCC_ALL_ACTOR) || (dwReqCharClass_Disguise!=GLCC_ALL_ACTOR) &&
							 (dwReqCharClass!=GLCC_ALL_TRICKER) || (dwReqCharClass_Disguise!=GLCC_ALL_TRICKER) )
						{
							bool bMan = false;
							bool bWoman = false;

							if ( (dwReqCharClass&GLCC_FIGHTER_M) && (dwReqCharClass_Disguise&GLCC_FIGHTER_M)
								|| (dwReqCharClass&GLCC_ARMS_M) && (dwReqCharClass_Disguise&GLCC_ARMS_M)
								|| (dwReqCharClass&GLCC_ARCHER_M) && (dwReqCharClass_Disguise&GLCC_ARCHER_M)
								|| (dwReqCharClass&GLCC_SPIRIT_M) && (dwReqCharClass_Disguise&GLCC_SPIRIT_M)
								|| (dwReqCharClass&GLCC_EXTREME_M) && (dwReqCharClass_Disguise&GLCC_EXTREME_M)
								|| (dwReqCharClass&GLCC_SCIENTIST_M) && (dwReqCharClass_Disguise&GLCC_SCIENTIST_M)
								|| (dwReqCharClass&GLCC_ASSASSIN_M) && (dwReqCharClass_Disguise&GLCC_ASSASSIN_M)
								|| (dwReqCharClass&GLCC_TRICKER_M) && (dwReqCharClass_Disguise&GLCC_TRICKER_M)
								|| (dwReqCharClass&GLCC_ETC_M) && (dwReqCharClass_Disguise&GLCC_ETC_M) )
							{
								bMan = true;
							}

							if ( (dwReqCharClass&GLCC_FIGHTER_W) && (dwReqCharClass_Disguise&GLCC_FIGHTER_W)
								|| (dwReqCharClass&GLCC_ARMS_W) && (dwReqCharClass_Disguise&GLCC_ARMS_W)
								|| (dwReqCharClass&GLCC_ARCHER_W) && (dwReqCharClass_Disguise&GLCC_ARCHER_W)
								|| (dwReqCharClass&GLCC_SPIRIT_W) && (dwReqCharClass_Disguise&GLCC_SPIRIT_W)
								|| (dwReqCharClass&GLCC_EXTREME_W) && (dwReqCharClass_Disguise&GLCC_EXTREME_W)
								|| (dwReqCharClass&GLCC_SCIENTIST_W) && (dwReqCharClass_Disguise&GLCC_SCIENTIST_W)
								|| (dwReqCharClass&GLCC_ASSASSIN_W) && (dwReqCharClass_Disguise&GLCC_ASSASSIN_W)
								|| (dwReqCharClass&GLCC_TRICKER_W) && (dwReqCharClass_Disguise&GLCC_TRICKER_W)
								|| (dwReqCharClass&GLCC_ETC_W) && (dwReqCharClass_Disguise&GLCC_ETC_W) )
							{
								bWoman = true;
							}

							if ( !bMan || !bWoman )
							{
								// 남, 여 모두 착용 가능하지 않을 때.
								if ( bMan )
								{
									// 남자 착용 가능 할 때.
									if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_M) 
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_M) )
										dwColor = ITEM_INFOR_COLOR::dwDefault;
									else
										dwColor = ITEM_INFOR_COLOR::dwReqWrong;

									//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 0 ), dwColor );
									strClass += ID2GAMEWORD( "BASIC_SEX", 0 );
									strClass += " ";
								}
								else
								{
									// 여자 착용 가능 할 때.
									if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_W) 
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_W) )
										dwColor = ITEM_INFOR_COLOR::dwDefault;
									else
										dwColor = ITEM_INFOR_COLOR::dwReqWrong;

									//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 1 ), dwColor );
									strClass += ID2GAMEWORD( "BASIC_SEX", 1 );
									strClass += " ";
								}
								INFO_DISPLAY_ITEM_AddTextNoSplit(strClass.c_str(), dwColor );
							}
						}
					}
					else
					{
						DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

						if ( (dwReqCharClass!=GLCC_ALL_SCIENTIST) || (dwReqCharClass!=GLCC_ALL_ASSASSIN) || (dwReqCharClass!=GLCC_ALL_ACTOR) || (dwReqCharClass!=GLCC_ALL_TRICKER) )
						{
							bool bMan = false;
							bool bWoman = false;

							if ( (dwReqCharClass & GLCC_FIGHTER_M) 
								|| (dwReqCharClass & GLCC_ARMS_M)
								|| (dwReqCharClass & GLCC_ARCHER_M)
								|| (dwReqCharClass & GLCC_SPIRIT_M)
								|| (dwReqCharClass & GLCC_EXTREME_M)
								|| (dwReqCharClass & GLCC_SCIENTIST_M)
								|| (dwReqCharClass & GLCC_ASSASSIN_M)
								|| (dwReqCharClass & GLCC_TRICKER_M)
								|| (dwReqCharClass & GLCC_ETC_M) )
							{
								bMan = true;
							}

							if ( (dwReqCharClass & GLCC_FIGHTER_W) 
								|| (dwReqCharClass & GLCC_ARMS_W)
								|| (dwReqCharClass & GLCC_ARCHER_W)
								|| (dwReqCharClass & GLCC_SPIRIT_W)
								|| (dwReqCharClass & GLCC_EXTREME_W)
								|| (dwReqCharClass & GLCC_SCIENTIST_W)
								|| (dwReqCharClass & GLCC_ASSASSIN_W)
								|| (dwReqCharClass & GLCC_TRICKER_W)
								|| (dwReqCharClass & GLCC_ETC_W) )
							{
								bWoman = true;
							}

							if ( !bMan || !bWoman )
							{
								// 남, 여 모두 착용 가능하지 않을 때.
								if ( bMan )
								{
									// 남자 착용 가능 할 때.
									if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_M) 
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_M) )
										dwColor = ITEM_INFOR_COLOR::dwDefault;
									else
										dwColor = ITEM_INFOR_COLOR::dwReqWrong;

									//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 0 ), dwColor );
									strClass += ID2GAMEWORD( "BASIC_SEX", 0 );
									strClass += " ";
								}
								else
								{
									// 여자 착용 가능 할 때.
									if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_W) 
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_W) )
										dwColor = ITEM_INFOR_COLOR::dwDefault;
									else
										dwColor = ITEM_INFOR_COLOR::dwReqWrong;

									//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 1 ), dwColor );
									strClass += ID2GAMEWORD( "BASIC_SEX", 1 );
									strClass += " ";
								}
								INFO_DISPLAY_ITEM_AddTextNoSplit(strClass.c_str(), dwColor );
							}
						}
					}
				}

				//	Á¶°Ç - ¾Ï±¤
				bValue = TRUE;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strText.Format("%s",CGameTextMan::GetInstance().GetCommentText("BRIGHT", pItemData->sBasicOp.emReqBright).GetString());
				else
					strText.Format ( "%s", COMMENT::BRIGHT[pItemData->sBasicOp.emReqBright].c_str() );

				if ( pItemData->sBasicOp.emReqBright != BRIGHT_BOTH )
				{
					if ( pCharacter->GETBRIGHT() != pItemData->sBasicOp.emReqBright )
					{
						bValue = FALSE;
					}
				}
				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCONTROL::GetEvaluateColor ( bValue ) );					

				//	Âø¿ëÁ¶°Ç->°ÝÅõÄ¡
				if ( pItemData->sBasicOp.wReqPA )
				{
					strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 1), pItemData->sBasicOp.wReqPA );
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCONTROL::GetEvaluateColor ( int(pItemData->sBasicOp.wReqPA) <= pCharacter->m_powerAttribute[SKILL::EMAPPLY_MELEE] ) );
				}
				//	Âø¿ëÁ¶°Ç->»ç°ÝÄ¡
				if ( pItemData->sBasicOp.wReqSA )
				{
					strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 2 ), pItemData->sBasicOp.wReqSA );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCONTROL::GetEvaluateColor ( int(pItemData->sBasicOp.wReqSA) <= pCharacter->m_powerAttribute[SKILL::EMAPPLY_RANGE] ));
				}
				//	Âø¿ëÁ¶°Ç->¸¶·ÂÄ¡
// 				if ( pItemData->sBasicOp.wReqMA )
// 				{
// 					strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 3 ), pItemData->sBasicOp.wReqMA );
// 					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCONTROL::GetEvaluateColor ( int(pItemData->sBasicOp.wReqMA) <= pCharacter->m_powerAttribute[SKILL::EMAPPLY_MAGIC] ));
// 				}

				//	Stats
				//	Âø¿ëÁ¶°Ç->Èû
				if ( rItemStats.wPow )
				{
					strText.Format( "%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 0 ), rItemStats.wPow );
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCONTROL::GetEvaluateColor ( rItemStats.wPow <= rCharStats.wPow ) );
				}
				//	Âø¿ëÁ¶°Ç->Ã¼·Â
				if ( rItemStats.wStr )
				{
					strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 1 ), rItemStats.wStr );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( rItemStats.wStr <= rCharStats.wStr ) );
				}
				//	Âø¿ëÁ¶°Ç->Á¤½Å·Â
				if ( rItemStats.wSpi)
				{
					strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 2 ), rItemStats.wSpi );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( rItemStats.wSpi <= rCharStats.wSpi ) );
				}
				//	Âø¿ëÁ¶°Ç->¹ÎÃ¸¼º
				if ( rItemStats.wDex )
				{
					strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 3 ), rItemStats.wDex );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( rItemStats.wDex <= rCharStats.wDex ) );
				}
				//	Âø¿ëÁ¶°Ç->Áö·Â
				if ( rItemStats.wInt  )
				{
					strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 4 ), rItemStats.wInt );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( rItemStats.wInt <= rCharStats.wInt ) );
				}
				//	Âø¿ëÁ¶°Ç->±Ù·Â
				if ( rItemStats.wSta )
				{
					strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION_STATS", 5 ), rItemStats.wSta );
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCONTROL::GetEvaluateColor ( rItemStats.wSta <= rCharStats.wSta ) );
				}

                //  Âø¿ëÁ¶°Ç->¿ä±¸È°µ¿
		        if ( pItemData->sBasicOp.wReqActPointDW || pItemData->sBasicOp.wReqActPointUP )
		        {
                    int emType = pItemData->sBasicOp.emReqActivityType;
			        bool bReqActPoint = true;
					// ¾Æ·¡ÀÇ COMMENT´Â µ¥ÀÌÅÍ¿¡¼­ ÀÐ¾î¿ÀÁö¾Ê´Â´Ù;
			        strText.Format( "%s:%s(%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 3), 
                        COMMENT::ACTIVITY_CLASS[emType].c_str(), pItemData->sBasicOp.wReqActPointDW );

			        if ( pItemData->sBasicOp.wReqActPointUP )
			        {
				        strText2.Format ( "~%d)",pItemData->sBasicOp.wReqActPointUP );
				        strText += strText2; 
				        bReqActPoint = (pItemData->sBasicOp.wReqActPointUP >= pCharacter->m_llActivityPoint[emType] );
			        }
                    else
                    {
                        strText += ")";
                    }

			        INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( 
                        pItemData->sBasicOp.wReqActPointDW <= pCharacter->m_llActivityPoint[emType] && bReqActPoint ) );
		        }


				//  Âø¿ëÁ¶°Ç->±â¿©µµ
				if ( pItemData->sBasicOp.dwReqContribution )
				{
					bool bReqPoint = true;

					strText.Format( "%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 4), pItemData->sBasicOp.dwReqContribution );

					INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( 
						pItemData->sBasicOp.dwReqContribution <= static_cast< DWORD >( pCharacter->m_nContributionPoint ) ) );
				}

				//-----------------------------------------------------------------------------------------------
				//	ÀúÇ×°ª
				const int nELEC   = sItemCustom.GETRESIST_ELEC();
				const int nFIRE   = sItemCustom.GETRESIST_FIRE();
				const int nICE    = sItemCustom.GETRESIST_ICE();
				const int nPOISON = sItemCustom.GETRESIST_POISON();
				const int nSPIRIT = sItemCustom.GETRESIST_SPIRIT();

				if ( nELEC || nFIRE || nICE || nPOISON || nSPIRIT )
				{
					INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_CATEGORY", 3 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );

					if ( nELEC )
					{
						strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_RESIST", 0 ), nELEC );	
						BYTE uGRADE = sItemCustom.GETGRADE(EMGRINDING_RESIST_ELEC);
						INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( strText, uGRADE );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
					}
					if ( nFIRE )
					{
						strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_RESIST", 1 ), nFIRE );
						BYTE uGRADE = sItemCustom.GETGRADE(EMGRINDING_RESIST_FIRE);
						INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( strText, uGRADE );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
					}
					if ( nICE )
					{
						strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_RESIST", 2 ), nICE );
						BYTE uGRADE = sItemCustom.GETGRADE(EMGRINDING_RESIST_ICE);
						INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( strText, uGRADE );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
					}
					if ( nPOISON )
					{
						strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_RESIST", 3 ), nPOISON );
						BYTE uGRADE = sItemCustom.GETGRADE(EMGRINDING_RESIST_POISON);
						INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( strText, uGRADE );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
					}
					if ( nSPIRIT )
					{
						strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_RESIST", 4 ), nSPIRIT );
						BYTE uGRADE = sItemCustom.GETGRADE(EMGRINDING_RESIST_SPIRIT);
						INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( strText, uGRADE );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
					}
				}


				//	»óÅÂÀÌ»ó
				EMSTATE_BLOW emBLOW = pItemData->sSuitOp.sBLOW.emTYPE;
				if ( emBLOW !=EMBLOW_NONE )
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						std::string strBLOW, strBLOW_VAR1, strBLOW_VAR2;
						strBLOW = CGameTextMan::GetInstance().GetCommentText("BLOW", emBLOW).GetString();
						strBLOW_VAR1 = CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1", emBLOW).GetString();
						strBLOW_VAR2 = CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2", emBLOW).GetString();
						INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_CATEGORY", 4 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );

						strText.Format("%s:%s", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 0 ), strBLOW.c_str() );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);

						strText.Format("%s:%.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 1 ),pItemData->sSuitOp.sBLOW.fLIFE );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);

						strText.Format("%s:%.2f%%", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 2 ),pItemData->sSuitOp.sBLOW.fRATE );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);

						{
							float fVAR1 = pItemData->sSuitOp.sBLOW.fVAR1 * COMMENT::BLOW_VAR1_SCALE[emBLOW];

							if( fVAR1 != 0.0f )
							{
								if ( COMMENT::IsBLOW1_PER(emBLOW) )
									strText.Format("%s:%.2f%%", strBLOW_VAR1.c_str(), fVAR1 );
								else
									strText.Format("%s:%.2f", strBLOW_VAR1.c_str(), fVAR1 );

								INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
							}
						}

						{
							float fVAR2 = pItemData->sSuitOp.sBLOW.fVAR2 * COMMENT::BLOW_VAR2_SCALE[emBLOW];

							if( fVAR2 != 0.0f )
							{
								if ( COMMENT::IsBLOW2_PER(emBLOW)  )
									strText.Format("%s:%.2f%%", strBLOW_VAR2.c_str(), fVAR2 );
								else
									strText.Format("%s:%.2f", strBLOW_VAR2.c_str(), fVAR2 );

								INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
							}
						}
					}
					else
					{
						INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_CATEGORY", 4 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );

						strText.Format("%s:%s", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 0 ), COMMENT::BLOW[emBLOW].c_str() );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);

						strText.Format("%s:%.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 1 ),pItemData->sSuitOp.sBLOW.fLIFE );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);

						strText.Format("%s:%.2f%%", ID2GAMEWORD("ITEM_ADVANCED_INFO_ABNORMAL", 2 ),pItemData->sSuitOp.sBLOW.fRATE );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);

						{
							float fVAR1 = pItemData->sSuitOp.sBLOW.fVAR1 * COMMENT::BLOW_VAR1_SCALE[emBLOW];

							if( fVAR1 != 0.0f )
							{
								if ( COMMENT::IsBLOW1_PER(emBLOW) )
									strText.Format("%s:%.2f%%", COMMENT::BLOW_VAR1[emBLOW].c_str(), fVAR1 );
								else
									strText.Format("%s:%.2f", COMMENT::BLOW_VAR1[emBLOW].c_str(), fVAR1 );

								INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
							}
						}

						{
							float fVAR2 = pItemData->sSuitOp.sBLOW.fVAR2 * COMMENT::BLOW_VAR2_SCALE[emBLOW];

							if( fVAR2 != 0.0f )
							{
								if ( COMMENT::IsBLOW2_PER(emBLOW) )
									strText.Format("%s:%.2f%%", COMMENT::BLOW_VAR2[emBLOW].c_str(), fVAR2 );
								else
									strText.Format("%s:%.2f", COMMENT::BLOW_VAR2[emBLOW].c_str(), fVAR2 );

								INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
							}
						}
					}
				}


				//	NOTE
				//		°¡»ê È¿°ú
				INFO_DISPLAY_ITEM_AddTextAddValue( sItemCustom, pItemData->sSuitOp );

				//	NOTE
				//	Æ¯¼ö±â´É ( º¯È­À² ¹× º¯È­·®) 
				//INFO_DISPLAY_ITEM_AddTextSpecialVaule( sItemCustom, pItemData );						

				// Note : ·£´ý ¼öÄ¡
				/*if( sItemCustom.IsSetRandOpt() )
				{
					CString strDescText;

					INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD ( "ITEM_CATEGORY", 11 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );

					INT nRandOptType = sItemCustom.GETOptTYPE1();
					if( (RandomOption::RANDOM_OPTION_NULL < nRandOptType) && (nRandOptType < RandomOption::RANDOM_OPTION_NSIZE) )
					{
						strDescText.Format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

						float fVal = sItemCustom.GETOptVALUE1();
						if( fVal != 0.0f )
						{
							if( fVal > 0.0f) strDescText += _T('+');

							strDescText.AppendFormat( "%.2f", fVal );
							if( sItemCustom.IsPerRandOpt( nRandOptType ) ) strDescText += _T('%');

							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText, NS_UITEXTCOLOR::PRIVATE );
						}
					}

					nRandOptType = sItemCustom.GETOptTYPE2();
					if( (RandomOption::RANDOM_OPTION_NULL < nRandOptType) && (nRandOptType < RandomOption::RANDOM_OPTION_NSIZE) )
					{
						strDescText.Format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

						float fVal = sItemCustom.GETOptVALUE2();
						if( fVal != 0.0f )
						{
							if( fVal > 0.0f) strDescText += _T('+');

							strDescText.AppendFormat( "%.2f", fVal );
							if( sItemCustom.IsPerRandOpt( nRandOptType ) ) strDescText += _T('%');

							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText, NS_UITEXTCOLOR::PRIVATE );
						}
					}

					nRandOptType = sItemCustom.GETOptTYPE3();
					if( (RandomOption::RANDOM_OPTION_NULL < nRandOptType) && (nRandOptType < RandomOption::RANDOM_OPTION_NSIZE) )
					{
						strDescText.Format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

						float fVal = sItemCustom.GETOptVALUE3();
						if( fVal != 0.0f )
						{
							if( fVal > 0.0f) strDescText += _T('+');

							strDescText.AppendFormat( "%.2f", fVal );
							if( sItemCustom.IsPerRandOpt( nRandOptType ) ) strDescText += _T('%');

							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText, NS_UITEXTCOLOR::PRIVATE );
						}
					}

					nRandOptType = sItemCustom.GETOptTYPE4();
					if( (RandomOption::RANDOM_OPTION_NULL < nRandOptType) && (nRandOptType < RandomOption::RANDOM_OPTION_NSIZE) )
					{
						strDescText.Format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

						float fVal = sItemCustom.GETOptVALUE4();
						if( fVal != 0.0f )
						{
							if( fVal > 0.0f) strDescText += _T('+');

							strDescText.AppendFormat( "%.2f", fVal );
							if( sItemCustom.IsPerRandOpt( nRandOptType ) ) strDescText += _T('%');

							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText, NS_UITEXTCOLOR::PRIVATE );
						}
					}
				}*/

				
				// Info : ·£´ý ¼öÄ¡
				{
					if( sItemCustom.IsSetRandOpt() )
					{
						AddTextNoSplit( ID2GAMEWORD ( "ITEM_CATEGORY", 11 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );
						DWORD dwRandomAbilityColor = ITEM_INFOR_COLOR::dwRandomAbility;

						if( (sItemCustom.GETOptTYPE1() > RandomOption::RANDOM_OPTION_NULL) && (sItemCustom.GETOptTYPE1() < RandomOption::RANDOM_OPTION_NSIZE) )
						{
							int nRandOptType = sItemCustom.GETOptTYPE1();
							float fVal = sItemCustom.GETOptVALUE1() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
							bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

							/*SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );
							if( fVal != 0.0f )
							{
								if( fVal > 0.0f) strDescText += _T('+');

								strDescText.AppendFormat( "%.2f", fVal );
								if( sItemCustom.IsPerRandOpt( nRandOptType1 ) ) strDescText += _T('%');

								AddTextNoSplit( strDescText, NS_UITEXTCOLOR::PRIVATE );
							}*/

							std::string strDescText = "";

							//if( (nRandOptType <= RandomOption::RANDOM_OPTION_NULL) || (nRandOptType >= RandomOption::RANDOM_OPTION_NSIZE) )
							//	return;

							//if ( fVal == 0.0f )
							//	return;

							strDescText = sc::string::format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

							if ( fVal > 0.0f) 
								strDescText += _T('+');
							if ( fVal < 0.0f) 
								strDescText += _T('-');

							if ( bIsPerRandOpt )
							{
								CString strTemp = sc::string::format( "%.2f", fVal ).c_str();
								CutZero(strTemp,2);
								strDescText += sc::string::format("%s", strTemp);

								strDescText += _T('%');
							}
							else
								strDescText += sc::string::format( "%.2f", fVal );

							const bool _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? true : false);
							if ( _bShowMinMax == true )
							{
								const RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
								const RandomOption::Entity* const _pEntity(_managerRandomOption.find(pItemData->sRandomOpt.szNAME));		
								if ( (_pEntity != NULL) && (GLUseFeatures::GetInstance().IsUsingRandomOptionClientView() == true) )
								{
									RandomOption::Information _information;
									if ( _pEntity->getInformation(RandomOption::GENERATE_REMODEL, RandomOption::RANDOM_OPTION(nRandOptType), _information) == true )
									{
										if ( bIsPerRandOpt == true )
											strDescText += sc::string::format( "(Min %.2f ~ Max %.2f)", _information.valueFloatLow, _information.valueFloatHigh);
										else
											strDescText += sc::string::format( "(Min %d ~ Max %d)", _information.valueIntegerLow, _information.valueIntegerHigh);
									}			
								}
							}	
							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText.c_str(), NS_UITEXTCOLOR::PRIVATE );

							//m_pItemLabel2->AddString( nIndex, strDescText.c_str(), dwAddAbility );
						}
						if( (sItemCustom.GETOptTYPE2() > RandomOption::RANDOM_OPTION_NULL) && (sItemCustom.GETOptTYPE2() < RandomOption::RANDOM_OPTION_NSIZE) )
						{
							int nRandOptType = sItemCustom.GETOptTYPE2();
							float fVal = sItemCustom.GETOptVALUE2() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
							bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

							//SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );

							std::string strDescText = "";

							//if( (nRandOptType <= RandomOption::RANDOM_OPTION_NULL) || (nRandOptType >= RandomOption::RANDOM_OPTION_NSIZE) )
							//	return;

							//if ( fVal == 0.0f )
							//	return;

							strDescText = sc::string::format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

							if ( fVal > 0.0f) 
								strDescText += _T('+');
							if ( fVal < 0.0f) 
								strDescText += _T('-');

							if ( bIsPerRandOpt )
							{
								CString strTemp = sc::string::format( "%.2f", fVal ).c_str();
								CutZero(strTemp,2);
								strDescText += sc::string::format("%s", strTemp);

								strDescText += _T('%');
							}
							else
								strDescText += sc::string::format( "%.2f", fVal );

							const bool _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? true : false);
							if ( _bShowMinMax == true )
							{
								const RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
								const RandomOption::Entity* const _pEntity(_managerRandomOption.find(pItemData->sRandomOpt.szNAME));		
								if ( (_pEntity != NULL) && (GLUseFeatures::GetInstance().IsUsingRandomOptionClientView() == true) )
								{
									RandomOption::Information _information;
									if ( _pEntity->getInformation(RandomOption::GENERATE_REMODEL, RandomOption::RANDOM_OPTION(nRandOptType), _information) == true )
									{
										if ( bIsPerRandOpt == true )
											strDescText += sc::string::format( "(Min %.2f ~ Max %.2f)", _information.valueFloatLow, _information.valueFloatHigh);
										else
											strDescText += sc::string::format( "(Min %d ~ Max %d)", _information.valueIntegerLow, _information.valueIntegerHigh);
									}			
								}
							}	
							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText.c_str(), NS_UITEXTCOLOR::PRIVATE );
						}
						if( (sItemCustom.GETOptTYPE3() > RandomOption::RANDOM_OPTION_NULL) && (sItemCustom.GETOptTYPE3() < RandomOption::RANDOM_OPTION_NSIZE) )
						{
							int nRandOptType = sItemCustom.GETOptTYPE3();
							float fVal = sItemCustom.GETOptVALUE3() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
							bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

							//SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );

							std::string strDescText = "";

							//if( (nRandOptType <= RandomOption::RANDOM_OPTION_NULL) || (nRandOptType >= RandomOption::RANDOM_OPTION_NSIZE) )
							//	return;

							//if ( fVal == 0.0f )
							//	return;

							strDescText = sc::string::format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

							if ( fVal > 0.0f) 
								strDescText += _T('+');
							if ( fVal < 0.0f) 
								strDescText += _T('-');

							if ( bIsPerRandOpt )
							{
								CString strTemp = sc::string::format( "%.2f", fVal ).c_str();
								CutZero(strTemp,2);
								strDescText += sc::string::format("%s", strTemp);

								strDescText += _T('%');
							}
							else
								strDescText += sc::string::format( "%.2f", fVal );

							const bool _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? true : false);
							if ( _bShowMinMax == true )
							{
								const RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
								const RandomOption::Entity* const _pEntity(_managerRandomOption.find(pItemData->sRandomOpt.szNAME));		
								if ( (_pEntity != NULL) && (GLUseFeatures::GetInstance().IsUsingRandomOptionClientView() == true) )
								{
									RandomOption::Information _information;
									if ( _pEntity->getInformation(RandomOption::GENERATE_REMODEL, RandomOption::RANDOM_OPTION(nRandOptType), _information) == true )
									{
										if ( bIsPerRandOpt == true )
											strDescText += sc::string::format( "(Min %.2f ~ Max %.2f)", _information.valueFloatLow, _information.valueFloatHigh);
										else
											strDescText += sc::string::format( "(Min %d ~ Max %d)", _information.valueIntegerLow, _information.valueIntegerHigh);
									}			
								}
							}	
							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText.c_str(), NS_UITEXTCOLOR::PRIVATE );
						}
						if( (sItemCustom.GETOptTYPE4() > RandomOption::RANDOM_OPTION_NULL) && (sItemCustom.GETOptTYPE4() < RandomOption::RANDOM_OPTION_NSIZE) )
						{
							int nRandOptType = sItemCustom.GETOptTYPE4();
							float fVal = sItemCustom.GETOptVALUE4() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
							bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

							//SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );

							std::string strDescText = "";

							//if( (nRandOptType <= RandomOption::RANDOM_OPTION_NULL) || (nRandOptType >= RandomOption::RANDOM_OPTION_NSIZE) )
							//	return;

							//if ( fVal == 0.0f )
							//	return;

							strDescText = sc::string::format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

							if ( fVal > 0.0f) 
								strDescText += _T('+');
							if ( fVal < 0.0f) 
								strDescText += _T('-');

							if ( bIsPerRandOpt )
							{
								CString strTemp = sc::string::format( "%.2f", fVal ).c_str();
								CutZero(strTemp,2);
								strDescText += sc::string::format("%s", strTemp);

								strDescText += _T('%');
							}
							else
								strDescText += sc::string::format( "%.2f", fVal );

							const bool _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? true : false);
							if ( _bShowMinMax == true )
							{
								const RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
								const RandomOption::Entity* const _pEntity(_managerRandomOption.find(pItemData->sRandomOpt.szNAME));		
								if ( (_pEntity != NULL) && (GLUseFeatures::GetInstance().IsUsingRandomOptionClientView() == true) )
								{
									RandomOption::Information _information;
									if ( _pEntity->getInformation(RandomOption::GENERATE_REMODEL, RandomOption::RANDOM_OPTION(nRandOptType), _information) == true )
									{
										if ( bIsPerRandOpt == true )
											strDescText += sc::string::format( "(Min %.2f ~ Max %.2f)", _information.valueFloatLow, _information.valueFloatHigh);
										else
											strDescText += sc::string::format( "(Min %d ~ Max %d)", _information.valueIntegerLow, _information.valueIntegerHigh);
									}			
								}
							}	
							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText.c_str(), NS_UITEXTCOLOR::PRIVATE );
						}
					}
				}
			}
			break;

		case ITEM_BOX:
		case ITEM_RANDOMITEM:
		case ITEM_SELECTIVEFORM_BOX:
			{
				//  Âø¿ëÁ¶°Ç->¿ä±¸È°µ¿
				if ( pItemData->sBasicOp.wReqActPointDW || pItemData->sBasicOp.wReqActPointUP )
				{
					int emType = pItemData->sBasicOp.emReqActivityType;
					bool bReqActPoint = true;
					// ¾Æ·¡ÀÇ COMMENT´Â µ¥ÀÌÅÍ¿¡¼­ ÀÐ¾î¿ÀÁö¾Ê´Â´Ù;
					strText.Format( "%s:%s(%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 3), 
						COMMENT::ACTIVITY_CLASS[emType].c_str(), pItemData->sBasicOp.wReqActPointDW );

					if ( pItemData->sBasicOp.wReqActPointUP )
					{
						strText2.Format ( "~%d)",pItemData->sBasicOp.wReqActPointUP );
						strText += strText2; 
						bReqActPoint = (pItemData->sBasicOp.wReqActPointUP >= pCharacter->m_llActivityPoint[emType] );
					}
					else
					{
						strText += ")";
					}

					INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( 
						pItemData->sBasicOp.wReqActPointDW <= pCharacter->m_llActivityPoint[emType] && bReqActPoint ) );
				}


				//  Âø¿ëÁ¶°Ç->±â¿©µµ
				if ( pItemData->sBasicOp.dwReqContribution )
				{
					bool bReqPoint = true;

					strText.Format( "%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 4), pItemData->sBasicOp.dwReqContribution );

					INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( 
						pItemData->sBasicOp.dwReqContribution <= static_cast< DWORD >( pCharacter->m_nContributionPoint ) ) );
				}
			}
			break;

		case ITEM_CHARM:
		case ITEM_ARROW:
		case ITEM_BULLET:
			{
				INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_CATEGORY", 1 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );

				//	°ø°Ý·Â
				GLDWDATA &sDAMAGE(sItemCustom.GETDAMAGE());
				if ( sDAMAGE.nNow || sDAMAGE.nMax )
				{
					strText.Format("%s:%s~%s ", ID2GAMEWORD("ITEM_ARROW_INFO", 0 ), 
						INFO_DISPLAY_ITEM_GetNumberWithSign ( sDAMAGE.nNow ), 
						INFO_DISPLAY_ITEM_GetNumberWithSign ( sDAMAGE.nMax ) );

					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);
				}


				//	NOTE
				//		°¡»ê È¿°ú
				INFO_DISPLAY_ITEM_AddTextAddValue( sItemCustom, pItemData->sSuitOp );					
			}
			break;

		case ITEM_CURE:
			{
				INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_CATEGORY", 7 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );

				//pItemData->sDrugOp.bRatio
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					if( pItemData->sDrugOp.dwCureVolume == 0 )
						strText.Format("%s",CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", pItemData->sDrugOp.emDrug).GetString());
					else
						strText.Format("%s:%d",CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", pItemData->sDrugOp.emDrug).GetString(), pItemData->sDrugOp.dwCureVolume);

					if ( pItemData->sDrugOp.bRatio )
						strText.Append("%");
				}
				else
				{
					if( pItemData->sDrugOp.dwCureVolume == 0 )
						strText.Format("%s",COMMENT::ITEMDRUG[pItemData->sDrugOp.emDrug].c_str());
					else
						strText.Format("%s:%d",COMMENT::ITEMDRUG[pItemData->sDrugOp.emDrug].c_str(), pItemData->sDrugOp.dwCureVolume);

					if ( pItemData->sDrugOp.bRatio )
						strText.Append("%");
				}
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT);
			}
			break;

		case ITEM_SKILL:
			{
				INFO_DISPLAY_ITEM_AddInfoSkillItem ( sItemCustom );
			}
			break;

		case ITEM_PET_SKILL:
			{
				INFO_DISPLAY_ITEM_AddInfoPetSkillItem( sItemCustom );
			}
			break;

		case ITEM_GRINDING:
			{
				INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD ( "ITEM_CATEGORY_EX", 0 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );
			}
			break;

			// °æÇèÄ¡ È¹µæ Ä«µå Á¤º¸
		case ITEM_VIETNAM_EXPGET:
			{
				if ( pItemData->sDrugOp.bRatio )
				{
					strText = sc::string::format( ID2GAMEWORD ("ITEM_CATEGORY_VIETNAM_EXP", 1) , pItemData->sDrugOp.dwCureVolume ).c_str();
				}
				else
				{
					strText = sc::string::format( ID2GAMEWORD ("ITEM_CATEGORY_VIETNAM_EXP", 0), pItemData->sDrugOp.dwCureVolume ).c_str();
				}

				INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			}
			break;
			// ¾ÆÀÌÅÛ È¹µæ Ä«µå Á¤º¸
		case ITEM_VIETNAM_ITEMGET:
			{
				strText = sc::string::format( ID2GAMEWORD ("ITEM_CATEGORY_VIETNAM_ITEM", 0) , pItemData->sDrugOp.dwCureVolume ).c_str();
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::LIGHTSKYBLUE);
			}
			break;

		case ITEM_VEHICLE:
			{
				BYTE uGRADE = 0;

				//	°ø°Ý·Â
				GLPADATA &sDamage = sItemCustom.getdamage();
				nExtraValue = sItemCustom.GETGRADE_DAMAGE();
				uGRADE = sItemCustom.GETGRADE(EMGRINDING_DAMAGE);
				INFO_DISPLAY_ITEM_AddInfoItemAddonRange ( sDamage.wLow, sDamage.wHigh, nExtraValue, uGRADE, ID2GAMEWORD("ITEM_ADVANCED_INFO", 0) );

				//	±â·ÂÄ¡? (¾Æ·¡¿¡ º¸¸é EMGRINDING_DAMAGE °ªÀ» °¡Á®¿Â´Ù. ¿Ö?)
				nExtraValue = sItemCustom.GETGRADE_DAMAGE();
				if ( nExtraValue )
				{
					uGRADE = sItemCustom.GETGRADE(EMGRINDING_DAMAGE);
					strText.Format ( "%s:+%d", ID2GAMEWORD("ITEM_ADVANCED_INFO", 8), nExtraValue );
					INFO_DISPLAY_ITEM_APPEND_ITEM_GRADE ( strText, uGRADE );
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::PRIVATE );
				}

				EMCHARINDEX emCharIdx = GLCI_NUM_ETC;
				int nLevel = -1;
				if ( pCharacter )
				{
					emCharIdx = pCharacter->GetCharIndex();
					nLevel = pCharacter->GETLEVEL();
				}

				//	¹æ¾î·Â
				short nDefense = sItemCustom.getdefense(emCharIdx, nLevel);
				nExtraValue = sItemCustom.GETGRADE_DEFENSE();
				uGRADE = sItemCustom.GETGRADE(EMGRINDING_DEFENSE);
				INFO_DISPLAY_ITEM_AddInfoItemAddon ( nDefense, nExtraValue, uGRADE, ID2GAMEWORD("ITEM_ADVANCED_INFO", 1) );

				//	°ø°Ý °Å¸®
				if ( sItemCustom.GETATTRANGE() )
				{
					strText.Format("%s:%d",ID2GAMEWORD("ITEM_ADVANCED_INFO", 2), sItemCustom.GETATTRANGE() );
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::DEFAULT );
				}

				//	¸íÁß·ü
				if ( sItemCustom.GETHITRATE() )
				{
					nExtraValue = 0;
					INFO_DISPLAY_ITEM_AddInfoItemAddon ( sItemCustom.GETHITRATE(), nExtraValue, ID2GAMEWORD("ITEM_ADVANCED_INFO", 3) );		
				}

				//	È¸ÇÇÀ²
				if ( sItemCustom.GETAVOIDRATE() )
				{
					nExtraValue = 0;
					INFO_DISPLAY_ITEM_AddInfoItemAddon ( sItemCustom.GETAVOIDRATE(), nExtraValue, ID2GAMEWORD("ITEM_ADVANCED_INFO", 4) );
				}

				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					//	Âø¿ëÀ§Ä¡
					strText.Format("%s:%s ", ID2GAMEWORD("ITEM_ADVANCED_INFO", 5), CGameTextMan::GetInstance().GetCommentText("ITEMSUIT", pItemData->sSuitOp.emSuit).GetString() );
					INFO_DISPLAY_ITEM_AddTextNoSplit (strText, NS_UITEXTCOLOR::DEFAULT );

					//	°ø°Ý ¼Ó¼º
					if ( pItemData->sSuitOp.emAttack != ITEMATT_NOTHING )
					{
						strText.Format("%s:%s", ID2GAMEWORD("ITEM_ADVANCED_INFO", 6), CGameTextMan::GetInstance().GetCommentText("ITEMATTACK", pItemData->sSuitOp.emAttack).GetString() );
						INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::DEFAULT );
					}
				}
				else
				{
					//	Âø¿ëÀ§Ä¡
					strText.Format("%s:%s ", ID2GAMEWORD("ITEM_ADVANCED_INFO", 5), COMMENT::ITEMSUIT[pItemData->sSuitOp.emSuit].c_str() );
					INFO_DISPLAY_ITEM_AddTextNoSplit (strText, NS_UITEXTCOLOR::DEFAULT );

					//	°ø°Ý ¼Ó¼º
					if ( pItemData->sSuitOp.emAttack != ITEMATT_NOTHING )
					{
						strText.Format("%s:%s", ID2GAMEWORD("ITEM_ADVANCED_INFO", 6), COMMENT::ITEMATTACK[pItemData->sSuitOp.emAttack].c_str() );
						INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::DEFAULT );
					}
				}

				//	SP ¼Ò¸ð
				const WORD wReqSP = sItemCustom.GETREQ_SP();
				if ( 0 < wReqSP )
				{
					strText.Format("%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO", 7), wReqSP );
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText, NS_UITEXTCOLOR::ORANGE );
				}

				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					strText = sc::string::format( ID2GAMEWORD ("ITEM_VEHICLE_TYPE_INFO", 0) , CGameTextMan::GetInstance().GetCommentText("VEHICLE_TYPE", pItemData->sVehicle.emVehicleType).GetString() ).c_str();
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::LIGHTSKYBLUE);
					strText.Format( "%s %s", ID2GAMEWORD ("ITEM_VEHICLE_NAME",0), CGameTextMan::GetInstance().GetCommentText("VEHICLE_TYPE", pItemData->sVehicle.emVehicleType).GetString() );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);
				}
				else
				{
					strText = sc::string::format( ID2GAMEWORD ("ITEM_VEHICLE_TYPE_INFO", 0) , COMMENT::VEHICLE_TYPE[pItemData->sVehicle.emVehicleType].c_str() ).c_str();
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::LIGHTSKYBLUE);

					strText.Format( "%s %s", ID2GAMEWORD ("ITEM_VEHICLE_NAME",0), COMMENT::VEHICLE_TYPE[pItemData->sVehicle.emVehicleType].c_str() );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);
				}
				bool bInfo(true);


				VEHICLEITEMINFO_MAP_ITER iter = m_pGaeaClient->GetCharacter()->m_mapVEHICLEItemInfo.find ( sItemCustom.m_VehicleDbNum );
				if ( iter == m_pGaeaClient->GetCharacter()->m_mapVEHICLEItemInfo.end() )
				{
					iter = m_pGaeaClient->GetCharacter()->m_mapVEHICLEItemInfoTemp.find ( sItemCustom.m_VehicleDbNum );
					if (iter == m_pGaeaClient->GetCharacter()->m_mapVEHICLEItemInfoTemp.end())
                        bInfo = false;
				}


				float fAddOnSpeed = 0.0f;

				if ( bInfo ) 
				{
					SVEHICLEITEMINFO sVehicleItemInfo = (*iter).second;

					for ( int i = 0; i < ACCE_TYPE_SIZE; ++i ) 
					{							
						fAddOnSpeed += sVehicleItemInfo.m_PutOnItems[i].GETMOVESPEED();
					}
				}

				EMITEM_VAR emITEM_VOL = pItemData->sSuitOp.sVOLUME.emTYPE;
				//	ÀÌµ¿¼Óµµ º¯È­·®
				//
				if ( emITEM_VOL != EMVAR_NONE)
				{
					if ( emITEM_VOL == EMVAR_MOVE_SPEED )
					{
						float fVOLUME = sItemCustom.GETMOVESPEED();

						if ( fAddOnSpeed != 0.0f )
							strText.Format("%s %.2f(+%.2f)", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ), fVOLUME, fAddOnSpeed);
						else 
							strText.Format("%s %.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ), fVOLUME );

						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
					}
					else
					{
						float fVOLUME = pItemData->sSuitOp.sVOLUME.fVolume;
						strText.Format("%s %.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ), fVOLUME );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
					}
				}


				if ( bInfo ) 
				{
					SVEHICLEITEMINFO sVehicleItemInfo = (*iter).second;

					for ( int i = 0; i < ACCE_TYPE_SIZE; ++i ) 
					{							
						const SITEM* pItem = GLogicData::GetInstance().GetItem ( sVehicleItemInfo.m_PutOnItems[i].GetNativeID() );
						if ( pItem )
						{
							strText.Format("%s %s", ID2GAMEWORD ("ITEM_VEHICLE_SLOT",i), pItem->GetName () );
							INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);
						}
					}

					strText.Format( "%s", ID2GAMEWORD ("ITEM_VEHICLE_BOOST_LEARN",sVehicleItemInfo.m_dwBoosterID ) );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);

					strText.Format( "%s %.2f%%", ID2GAMEWORD ("ITEM_VEHICLE_BATTERY",0), sVehicleItemInfo.m_nFull/10.0f );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);
				}


				INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD ( "ITEM_CATEGORY", 2 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );



				// Info : 착용 클래스
				{
					std::string strClass;
					DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
					if( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
					{
						const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
						if ( !pItemDisguise )   return;

						DWORD dwReqCharClass_Disguise = pItemDisguise->sBasicOp.dwReqCharClass;
						DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

						if ( (dwReqCharClass&m_pGaeaClient->GetCharacter()->m_emClass) && (dwReqCharClass_Disguise&m_pGaeaClient->GetCharacter()->m_emClass) )
							dwColor = ITEM_INFOR_COLOR::dwDefault;
						else
							dwColor = ITEM_INFOR_COLOR::dwReqWrong;

						if( dwReqCharClass )
						{
							if ( (dwReqCharClass==GLCC_ALL_TRICKER) && (dwReqCharClass_Disguise==GLCC_ALL_TRICKER)
								|| (dwReqCharClass==GLCC_ALL_SCIENTIST) && (dwReqCharClass_Disguise==GLCC_ALL_SCIENTIST)
								|| (dwReqCharClass==GLCC_ALL_ASSASSIN) && (dwReqCharClass_Disguise==GLCC_ALL_ASSASSIN)
								|| (dwReqCharClass==GLCC_ALL_ACTOR) && (dwReqCharClass_Disguise==GLCC_ALL_ACTOR)
								|| (dwReqCharClass == GLCC_MAN) && (dwReqCharClass_Disguise == GLCC_MAN)
								|| (dwReqCharClass == GLCC_WOMAN) && (dwReqCharClass_Disguise == GLCC_WOMAN) )
							{
								strClass = ID2GAMEWORD("ITEM_ALL_CLASS");
							}
							else
							{
								if ( (dwReqCharClass&GLCC_FIGHTER_M) && (dwReqCharClass_Disguise&GLCC_FIGHTER_M)
									|| (dwReqCharClass&GLCC_FIGHTER_W) && (dwReqCharClass_Disguise&GLCC_FIGHTER_W) )
								{
									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_FIGHTER_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass&GLCC_ARMS_M)  && (dwReqCharClass_Disguise&GLCC_ARMS_M)
									|| (dwReqCharClass&GLCC_ARMS_W)  && (dwReqCharClass_Disguise&GLCC_ARMS_W) )
								{
									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARMS_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass&GLCC_ARCHER_M)  && (dwReqCharClass_Disguise&GLCC_ARCHER_M)
									|| (dwReqCharClass&GLCC_ARCHER_W)  && (dwReqCharClass_Disguise&GLCC_ARCHER_W) )
								{
									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS",
										CharClassToClassIndex( GLCC_ARCHER_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass&GLCC_SPIRIT_M)  && (dwReqCharClass_Disguise&GLCC_SPIRIT_M)
									|| (dwReqCharClass&GLCC_SPIRIT_W)  && (dwReqCharClass_Disguise&GLCC_SPIRIT_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SPIRIT_M ));
									strClass += " ";
								}

								if ( (dwReqCharClass&GLCC_EXTREME_M)  && (dwReqCharClass_Disguise&GLCC_EXTREME_M)
									|| (dwReqCharClass&GLCC_EXTREME_W)  && (dwReqCharClass_Disguise&GLCC_EXTREME_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_EXTREME_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass&GLCC_SCIENTIST_M)  && (dwReqCharClass_Disguise&GLCC_SCIENTIST_M)
									|| (dwReqCharClass&GLCC_SCIENTIST_W)  && (dwReqCharClass_Disguise&GLCC_SCIENTIST_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SCIENTIST_M ) );
									strClass += " ";
								}

								/*if ( (dwReqCharClass&GLCC_ASSASSIN_M)  && (dwReqCharClass_Disguise&GLCC_ASSASSIN_M)
									|| (dwReqCharClass&GLCC_ASSASSIN_W)  && (dwReqCharClass_Disguise&GLCC_ASSASSIN_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit( 
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ASSASSIN_M ) ),
										dwColor );
								}

								if ( (dwReqCharClass&GLCC_TRICKER_M)  && (dwReqCharClass_Disguise&GLCC_TRICKER_M)
									|| (dwReqCharClass&GLCC_TRICKER_W)  && (dwReqCharClass_Disguise&GLCC_TRICKER_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit( 
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_TRICKER_M ) ),
										dwColor );
								}*/

								/*if ( (dwReqCharClass&GLCC_ETC_M)  && (dwReqCharClass_Disguise&GLCC_ETC_M)
									|| (dwReqCharClass&GLCC_ETC_W)  && (dwReqCharClass_Disguise&GLCC_ETC_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit(
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ETC_M ) ),
										dwColor );
								}*/
							}
							INFO_DISPLAY_ITEM_AddTextNoSplit(strClass.c_str(), dwColor );
						}
					}
					else
					{
						std::string strClass;
						DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

						if ( (dwReqCharClass&m_pGaeaClient->GetCharacter()->m_emClass) )
							dwColor = ITEM_INFOR_COLOR::dwDefault;
						else
							dwColor = ITEM_INFOR_COLOR::dwReqWrong;

						if( dwReqCharClass )
						{
							if ( dwReqCharClass==GLCC_ALL_TRICKER
								|| dwReqCharClass==GLCC_ALL_ASSASSIN
								|| dwReqCharClass==GLCC_ALL_SCIENTIST
								|| dwReqCharClass==GLCC_ALL_ACTOR
								|| (dwReqCharClass == GLCC_MAN)
								|| (dwReqCharClass == GLCC_WOMAN) )
							{
								strClass = ID2GAMEWORD("ITEM_ALL_CLASS");
							}
							else
							{
								bool bDistinction = false;
								if ( (dwReqCharClass & GLCC_FIGHTER_M) 
									|| (dwReqCharClass & GLCC_FIGHTER_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_FIGHTER_M ));
									strClass += " ";
								}

								if ( (dwReqCharClass & GLCC_ARMS_M) 
									|| (dwReqCharClass & GLCC_ARMS_W) )
								{
									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARMS_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass & GLCC_ARCHER_M)
									|| (dwReqCharClass & GLCC_ARCHER_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ARCHER_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass & GLCC_SPIRIT_M)
									|| (dwReqCharClass & GLCC_SPIRIT_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SPIRIT_M ) );
									strClass += " ";
								}
								if ( (dwReqCharClass & GLCC_EXTREME_M)
									|| (dwReqCharClass & GLCC_EXTREME_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_EXTREME_M ) );
									strClass += " ";
								}

								if ( (dwReqCharClass & GLCC_SCIENTIST_M)
									|| (dwReqCharClass & GLCC_SCIENTIST_W) )
								{

									strClass += ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_SCIENTIST_M ) );
									strClass += " ";
								}

								/*if ( (dwReqCharClass & GLCC_ASSASSIN_M)
									|| (dwReqCharClass & GLCC_ASSASSIN_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit( 
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ASSASSIN_M ) ),
										dwColor );
								}

								if ( (dwReqCharClass & GLCC_TRICKER_M)
									|| (dwReqCharClass & GLCC_TRICKER_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit( 
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_TRICKER_M ) ),
										dwColor );
								}

								if ( (dwReqCharClass & GLCC_ETC_M)
									|| (dwReqCharClass & GLCC_ETC_W) )
								{

									INFO_DISPLAY_ITEM_AddTextNoSplit(
										ID2GAMEWORD( "ITEM_SHOP_SEARCH_CLASS", CharClassToClassIndex( GLCC_ETC_M ) ),
										dwColor );
								}*/
							}
						}
						INFO_DISPLAY_ITEM_AddTextNoSplit(strClass.c_str(), dwColor );
					}
				}

				// Info : 성별
				{
					std::string strClass;
					DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
					if( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
					{
						const SITEM* pItemDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
						if ( !pItemDisguise )   return;

						DWORD dwReqCharClass_Disguise = pItemDisguise->sBasicOp.dwReqCharClass;
						DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

						if ( ( (dwReqCharClass!=GLCC_ALL_SCIENTIST) || (dwReqCharClass_Disguise!=GLCC_ALL_SCIENTIST) ) &&
							 (dwReqCharClass!=GLCC_ALL_ASSASSIN) || (dwReqCharClass_Disguise!=GLCC_ALL_ASSASSIN) &&
							 (dwReqCharClass!=GLCC_ALL_ACTOR) || (dwReqCharClass_Disguise!=GLCC_ALL_ACTOR) &&
							 (dwReqCharClass!=GLCC_ALL_TRICKER) || (dwReqCharClass_Disguise!=GLCC_ALL_TRICKER) )
						{
							bool bMan = false;
							bool bWoman = false;

							if ( (dwReqCharClass&GLCC_FIGHTER_M) && (dwReqCharClass_Disguise&GLCC_FIGHTER_M)
								|| (dwReqCharClass&GLCC_ARMS_M) && (dwReqCharClass_Disguise&GLCC_ARMS_M)
								|| (dwReqCharClass&GLCC_ARCHER_M) && (dwReqCharClass_Disguise&GLCC_ARCHER_M)
								|| (dwReqCharClass&GLCC_SPIRIT_M) && (dwReqCharClass_Disguise&GLCC_SPIRIT_M)
								|| (dwReqCharClass&GLCC_EXTREME_M) && (dwReqCharClass_Disguise&GLCC_EXTREME_M)
								|| (dwReqCharClass&GLCC_SCIENTIST_M) && (dwReqCharClass_Disguise&GLCC_SCIENTIST_M)
								|| (dwReqCharClass&GLCC_ASSASSIN_M) && (dwReqCharClass_Disguise&GLCC_ASSASSIN_M)
								|| (dwReqCharClass&GLCC_TRICKER_M) && (dwReqCharClass_Disguise&GLCC_TRICKER_M)
								|| (dwReqCharClass&GLCC_ETC_M) && (dwReqCharClass_Disguise&GLCC_ETC_M) )
							{
								bMan = true;
							}

							if ( (dwReqCharClass&GLCC_FIGHTER_W) && (dwReqCharClass_Disguise&GLCC_FIGHTER_W)
								|| (dwReqCharClass&GLCC_ARMS_W) && (dwReqCharClass_Disguise&GLCC_ARMS_W)
								|| (dwReqCharClass&GLCC_ARCHER_W) && (dwReqCharClass_Disguise&GLCC_ARCHER_W)
								|| (dwReqCharClass&GLCC_SPIRIT_W) && (dwReqCharClass_Disguise&GLCC_SPIRIT_W)
								|| (dwReqCharClass&GLCC_EXTREME_W) && (dwReqCharClass_Disguise&GLCC_EXTREME_W)
								|| (dwReqCharClass&GLCC_SCIENTIST_W) && (dwReqCharClass_Disguise&GLCC_SCIENTIST_W)
								|| (dwReqCharClass&GLCC_ASSASSIN_W) && (dwReqCharClass_Disguise&GLCC_ASSASSIN_W)
								|| (dwReqCharClass&GLCC_TRICKER_W) && (dwReqCharClass_Disguise&GLCC_TRICKER_W)
								|| (dwReqCharClass&GLCC_ETC_W) && (dwReqCharClass_Disguise&GLCC_ETC_W) )
							{
								bWoman = true;
							}

							if ( !bMan || !bWoman )
							{
								// 남, 여 모두 착용 가능하지 않을 때.
								if ( bMan )
								{
									// 남자 착용 가능 할 때.
									if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_M) 
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_M) )
										dwColor = ITEM_INFOR_COLOR::dwDefault;
									else
										dwColor = ITEM_INFOR_COLOR::dwReqWrong;

									//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 0 ), dwColor );
									strClass += ID2GAMEWORD( "BASIC_SEX", 0 );
									strClass += " ";
								}
								else
								{
									// 여자 착용 가능 할 때.
									if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_W) 
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_W) )
										dwColor = ITEM_INFOR_COLOR::dwDefault;
									else
										dwColor = ITEM_INFOR_COLOR::dwReqWrong;

									//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 1 ), dwColor );
									strClass += ID2GAMEWORD( "BASIC_SEX", 1 );
									strClass += " ";
								}
							}
						}
					}
					else
					{
						DWORD dwReqCharClass = pItemData->sBasicOp.dwReqCharClass;

						if ( (dwReqCharClass!=GLCC_ALL_SCIENTIST) || (dwReqCharClass!=GLCC_ALL_ASSASSIN) || (dwReqCharClass!=GLCC_ALL_ACTOR) || (dwReqCharClass!=GLCC_ALL_TRICKER) )
						{
							bool bMan = false;
							bool bWoman = false;

							if ( (dwReqCharClass & GLCC_FIGHTER_M) 
								|| (dwReqCharClass & GLCC_ARMS_M)
								|| (dwReqCharClass & GLCC_ARCHER_M)
								|| (dwReqCharClass & GLCC_SPIRIT_M)
								|| (dwReqCharClass & GLCC_EXTREME_M)
								|| (dwReqCharClass & GLCC_SCIENTIST_M)
								|| (dwReqCharClass & GLCC_ASSASSIN_M)
								|| (dwReqCharClass & GLCC_TRICKER_M)
								|| (dwReqCharClass & GLCC_ETC_M) )
							{
								bMan = true;
							}

							if ( (dwReqCharClass & GLCC_FIGHTER_W) 
								|| (dwReqCharClass & GLCC_ARMS_W)
								|| (dwReqCharClass & GLCC_ARCHER_W)
								|| (dwReqCharClass & GLCC_SPIRIT_W)
								|| (dwReqCharClass & GLCC_EXTREME_W)
								|| (dwReqCharClass & GLCC_SCIENTIST_W)
								|| (dwReqCharClass & GLCC_ASSASSIN_W)
								|| (dwReqCharClass & GLCC_TRICKER_W)
								|| (dwReqCharClass & GLCC_ETC_W) )
							{
								bWoman = true;
							}

							if ( !bMan || !bWoman )
							{
								// 남, 여 모두 착용 가능하지 않을 때.
								if ( bMan )
								{
									// 남자 착용 가능 할 때.
									if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_M) 
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_M)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_M) )
										dwColor = ITEM_INFOR_COLOR::dwDefault;
									else
										dwColor = ITEM_INFOR_COLOR::dwReqWrong;

									//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 0 ), dwColor );
									strClass += ID2GAMEWORD( "BASIC_SEX", 0 );
									strClass += " ";
								}
								else
								{
									// 여자 착용 가능 할 때.
									if ( (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_FIGHTER_W) 
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARMS_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ARCHER_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SPIRIT_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_EXTREME_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_SCIENTIST_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ASSASSIN_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_TRICKER_W)
										|| (m_pGaeaClient->GetCharacter()->m_emClass & GLCC_ETC_W) )
										dwColor = ITEM_INFOR_COLOR::dwDefault;
									else
										dwColor = ITEM_INFOR_COLOR::dwReqWrong;

									//INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "BASIC_SEX", 1 ), dwColor );
									strClass += ID2GAMEWORD( "BASIC_SEX", 1 );
									strClass += " ";
								}
							}
						}
					}
				}

				if ( pItemData->sBasicOp.wReqActPointDW || pItemData->sBasicOp.wReqActPointUP )
				{
					int emType = pItemData->sBasicOp.emReqActivityType;
					bool bReqActPoint = true;
					// ¾Æ·¡ÀÇ COMMENT´Â µ¥ÀÌÅÍ¿¡¼­ ÀÐ¾î¿ÀÁö¾Ê´Â´Ù;
					strText.Format( "%s:%s(%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 3), 
						COMMENT::ACTIVITY_CLASS[emType].c_str(), pItemData->sBasicOp.wReqActPointDW );

					if ( pItemData->sBasicOp.wReqActPointUP )
					{
						strText2.Format ( "~%d)",pItemData->sBasicOp.wReqActPointUP );
						strText += strText2; 
						bReqActPoint = (pItemData->sBasicOp.wReqActPointUP >= pCharacter->m_llActivityPoint[emType] );
					}
					else
					{
						strText += ")";
					}

					INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( 
						pItemData->sBasicOp.wReqActPointDW <= pCharacter->m_llActivityPoint[emType] && bReqActPoint ) );
				}


				//  Âø¿ëÁ¶°Ç->±â¿©µµ
				if ( pItemData->sBasicOp.dwReqContribution )
				{
					bool bReqPoint = true;

					strText.Format( "%s:%d", ID2GAMEWORD("ITEM_ADVANCED_INFO_CONDITION", 4), pItemData->sBasicOp.dwReqContribution );

					INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( 
						pItemData->sBasicOp.dwReqContribution <= static_cast< DWORD >( pCharacter->m_nContributionPoint ) ) );
				}

				//INFO_DISPLAY_ITEM_AddTextAddValue ( sItemCustom, pItemData->sSuitOp );

				{
					BOOL bLEAST_HAVE_ONE(TRUE);
					for ( int i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
					{
						EMITEM_ADDON emTYPE = pItemData->sSuitOp.sADDON[i].emTYPE;

						if ( emTYPE > EMADDEX_INCR_NONE ) continue;

						if ( !pItemData->sSuitOp.sADDON[i].IsNoneType() )
						{
							int nVALUE = pItemData->sSuitOp.sADDON[i].nVALUE;

							if ( bLEAST_HAVE_ONE )
							{
								INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD ( "ITEM_CATEGORY", 10 ),NS_UITEXTCOLOR::LIGHTSKYBLUE);
								bLEAST_HAVE_ONE = FALSE;
							}

							strText.Format( "%s:%s", ID2GAMEWORD( "ITEM_ADDON_INFO", emTYPE ), INFO_DISPLAY_ITEM_GetNumberWithSign( nVALUE ) );
							INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::ORANGE );

						}
					}

					// special function
					bLEAST_HAVE_ONE = TRUE;
					for ( int i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
					{
						EMITEM_ADDON emTYPE = pItemData->sSuitOp.sADDON[i].emTYPE;
						if ( emTYPE < EMADDEX_INCR_NONE ) continue;

						if ( !pItemData->sSuitOp.sADDON[i].IsNoneType() )
						{
							if ( bLEAST_HAVE_ONE )
							{
								INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD ( "ITEM_CATEGORY", 5 ),NS_UITEXTCOLOR::LIGHTSKYBLUE);
								bLEAST_HAVE_ONE = FALSE;
							}

							if ( ITEM::bAddonValueInteger[emTYPE] )
							{
								int nVALUE = pItemData->sSuitOp.GetAddonValue_CompareExchange< const int >( i, ITEM::bAddonValueInteger[emTYPE] );

								strText.Format( "%s:%s", ID2GAMEWORD( "ITEM_ADDON_INFO", emTYPE ), INFO_DISPLAY_ITEM_GetNumberWithSign( nVALUE ) );
								INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::PRIVATE );
							}
							else
							{
								EMITEM_VAR emVarType = ConvertAddon2Var ( emTYPE );
								if ( emVarType == EMVAR_MOVE_SPEED )
								{
									float fVOLUME = sItemCustom.GETMOVESPEED();

									if ( fAddOnSpeed != 0.0f )
										strText.Format("%s %.2f(+%.2f)", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emVarType ), fVOLUME, fAddOnSpeed);
									else 
										strText.Format("%s %.2f", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emVarType ), fVOLUME );

									INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::PRIVATE);
								}
								else
								{
									float fVALUE = pItemData->sSuitOp.GetAddonValue_CompareExchange< const float >( i, ITEM::bAddonValueInteger[emTYPE] );
									if ( COMMENT::IsITEMVAR_SCALE(emVarType) ) fVALUE *= COMMENT::ITEMVAR_SCALE[emVarType];

									strText.Format( "%s:%2.2f", ID2GAMEWORD( "ITEM_ADDON_INFO", emTYPE ), fVALUE );
									if ( COMMENT::IsITEMVAR_SCALE(emVarType) )		strText += "%";
									INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::PRIVATE );
								}
							}
						}
					}
				}

				{
					if( sItemCustom.IsSetRandOpt() )
					{
						AddTextNoSplit( ID2GAMEWORD ( "ITEM_CATEGORY", 11 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );
						DWORD dwRandomAbilityColor = ITEM_INFOR_COLOR::dwRandomAbility;

						if( (sItemCustom.GETOptTYPE1() > RandomOption::RANDOM_OPTION_NULL) && (sItemCustom.GETOptTYPE1() < RandomOption::RANDOM_OPTION_NSIZE) )
						{
							int nRandOptType = sItemCustom.GETOptTYPE1();
							float fVal = sItemCustom.GETOptVALUE1() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
							bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

							/*SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );
							if( fVal != 0.0f )
							{
								if( fVal > 0.0f) strDescText += _T('+');

								strDescText.AppendFormat( "%.2f", fVal );
								if( sItemCustom.IsPerRandOpt( nRandOptType1 ) ) strDescText += _T('%');

								AddTextNoSplit( strDescText, NS_UITEXTCOLOR::PRIVATE );
							}*/

							std::string strDescText = "";

							//if( (nRandOptType <= RandomOption::RANDOM_OPTION_NULL) || (nRandOptType >= RandomOption::RANDOM_OPTION_NSIZE) )
							//	return;

							//if ( fVal == 0.0f )
							//	return;

							strDescText = sc::string::format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

							if ( fVal > 0.0f) 
								strDescText += _T('+');
							if ( fVal < 0.0f) 
								strDescText += _T('-');

							if ( bIsPerRandOpt )
							{
								CString strTemp = sc::string::format( "%.2f", fVal ).c_str();
								CutZero(strTemp,2);
								strDescText += sc::string::format("%s", strTemp);

								strDescText += _T('%');
							}
							else
								strDescText += sc::string::format( "%.2f", fVal );

							const bool _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? true : false);
							if ( _bShowMinMax == true )
							{
								const RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
								const RandomOption::Entity* const _pEntity(_managerRandomOption.find(pItemData->sRandomOpt.szNAME));		
								if ( (_pEntity != NULL) && (GLUseFeatures::GetInstance().IsUsingRandomOptionClientView() == true) )
								{
									RandomOption::Information _information;
									if ( _pEntity->getInformation(RandomOption::GENERATE_REMODEL, RandomOption::RANDOM_OPTION(nRandOptType), _information) == true )
									{
										if ( bIsPerRandOpt == true )
											strDescText += sc::string::format( "(Min %.2f ~ Max %.2f)", _information.valueFloatLow, _information.valueFloatHigh);
										else
											strDescText += sc::string::format( "(Min %d ~ Max %d)", _information.valueIntegerLow, _information.valueIntegerHigh);
									}			
								}
							}	
							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText.c_str(), NS_UITEXTCOLOR::PRIVATE );

							//m_pItemLabel2->AddString( nIndex, strDescText.c_str(), dwAddAbility );
						}
						if( (sItemCustom.GETOptTYPE2() > RandomOption::RANDOM_OPTION_NULL) && (sItemCustom.GETOptTYPE2() < RandomOption::RANDOM_OPTION_NSIZE) )
						{
							int nRandOptType = sItemCustom.GETOptTYPE2();
							float fVal = sItemCustom.GETOptVALUE2() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
							bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

							//SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );

							std::string strDescText = "";

							//if( (nRandOptType <= RandomOption::RANDOM_OPTION_NULL) || (nRandOptType >= RandomOption::RANDOM_OPTION_NSIZE) )
							//	return;

							//if ( fVal == 0.0f )
							//	return;

							strDescText = sc::string::format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

							if ( fVal > 0.0f) 
								strDescText += _T('+');
							if ( fVal < 0.0f) 
								strDescText += _T('-');

							if ( bIsPerRandOpt )
							{
								CString strTemp = sc::string::format( "%.2f", fVal ).c_str();
								CutZero(strTemp,2);
								strDescText += sc::string::format("%s", strTemp);

								strDescText += _T('%');
							}
							else
								strDescText += sc::string::format( "%.2f", fVal );

							const bool _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? true : false);
							if ( _bShowMinMax == true )
							{
								const RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
								const RandomOption::Entity* const _pEntity(_managerRandomOption.find(pItemData->sRandomOpt.szNAME));		
								if ( (_pEntity != NULL) && (GLUseFeatures::GetInstance().IsUsingRandomOptionClientView() == true) )
								{
									RandomOption::Information _information;
									if ( _pEntity->getInformation(RandomOption::GENERATE_REMODEL, RandomOption::RANDOM_OPTION(nRandOptType), _information) == true )
									{
										if ( bIsPerRandOpt == true )
											strDescText += sc::string::format( "(Min %.2f ~ Max %.2f)", _information.valueFloatLow, _information.valueFloatHigh);
										else
											strDescText += sc::string::format( "(Min %d ~ Max %d)", _information.valueIntegerLow, _information.valueIntegerHigh);
									}			
								}
							}	
							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText.c_str(), NS_UITEXTCOLOR::PRIVATE );
						}
						if( (sItemCustom.GETOptTYPE3() > RandomOption::RANDOM_OPTION_NULL) && (sItemCustom.GETOptTYPE3() < RandomOption::RANDOM_OPTION_NSIZE) )
						{
							int nRandOptType = sItemCustom.GETOptTYPE3();
							float fVal = sItemCustom.GETOptVALUE3() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
							bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

							//SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );

							std::string strDescText = "";

							//if( (nRandOptType <= RandomOption::RANDOM_OPTION_NULL) || (nRandOptType >= RandomOption::RANDOM_OPTION_NSIZE) )
							//	return;

							//if ( fVal == 0.0f )
							//	return;

							strDescText = sc::string::format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

							if ( fVal > 0.0f) 
								strDescText += _T('+');
							if ( fVal < 0.0f) 
								strDescText += _T('-');

							if ( bIsPerRandOpt )
							{
								CString strTemp = sc::string::format( "%.2f", fVal ).c_str();
								CutZero(strTemp,2);
								strDescText += sc::string::format("%s", strTemp);

								strDescText += _T('%');
							}
							else
								strDescText += sc::string::format( "%.2f", fVal );

							const bool _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? true : false);
							if ( _bShowMinMax == true )
							{
								const RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
								const RandomOption::Entity* const _pEntity(_managerRandomOption.find(pItemData->sRandomOpt.szNAME));		
								if ( (_pEntity != NULL) && (GLUseFeatures::GetInstance().IsUsingRandomOptionClientView() == true) )
								{
									RandomOption::Information _information;
									if ( _pEntity->getInformation(RandomOption::GENERATE_REMODEL, RandomOption::RANDOM_OPTION(nRandOptType), _information) == true )
									{
										if ( bIsPerRandOpt == true )
											strDescText += sc::string::format( "(Min %.2f ~ Max %.2f)", _information.valueFloatLow, _information.valueFloatHigh);
										else
											strDescText += sc::string::format( "(Min %d ~ Max %d)", _information.valueIntegerLow, _information.valueIntegerHigh);
									}			
								}
							}	
							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText.c_str(), NS_UITEXTCOLOR::PRIVATE );
						}
						if( (sItemCustom.GETOptTYPE4() > RandomOption::RANDOM_OPTION_NULL) && (sItemCustom.GETOptTYPE4() < RandomOption::RANDOM_OPTION_NSIZE) )
						{
							int nRandOptType = sItemCustom.GETOptTYPE4();
							float fVal = sItemCustom.GETOptVALUE4() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
							bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

							//SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );

							std::string strDescText = "";

							//if( (nRandOptType <= RandomOption::RANDOM_OPTION_NULL) || (nRandOptType >= RandomOption::RANDOM_OPTION_NSIZE) )
							//	return;

							//if ( fVal == 0.0f )
							//	return;

							strDescText = sc::string::format( "%s:", ID2GAMEWORD("ITEM_RANDOM_OPTION", nRandOptType ) );

							if ( fVal > 0.0f) 
								strDescText += _T('+');
							if ( fVal < 0.0f) 
								strDescText += _T('-');

							if ( bIsPerRandOpt )
							{
								CString strTemp = sc::string::format( "%.2f", fVal ).c_str();
								CutZero(strTemp,2);
								strDescText += sc::string::format("%s", strTemp);

								strDescText += _T('%');
							}
							else
								strDescText += sc::string::format( "%.2f", fVal );

							const bool _bShowMinMax(GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_DOMINATE_CONTROL) & DXKEY_DOWNED ? true : false);
							if ( _bShowMinMax == true )
							{
								const RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
								const RandomOption::Entity* const _pEntity(_managerRandomOption.find(pItemData->sRandomOpt.szNAME));		
								if ( (_pEntity != NULL) && (GLUseFeatures::GetInstance().IsUsingRandomOptionClientView() == true) )
								{
									RandomOption::Information _information;
									if ( _pEntity->getInformation(RandomOption::GENERATE_REMODEL, RandomOption::RANDOM_OPTION(nRandOptType), _information) == true )
									{
										if ( bIsPerRandOpt == true )
											strDescText += sc::string::format( "(Min %.2f ~ Max %.2f)", _information.valueFloatLow, _information.valueFloatHigh);
										else
											strDescText += sc::string::format( "(Min %d ~ Max %d)", _information.valueIntegerLow, _information.valueIntegerHigh);
									}			
								}
							}	
							INFO_DISPLAY_ITEM_AddTextNoSplit( strDescText.c_str(), NS_UITEXTCOLOR::PRIVATE );
						}
					}
				}
			}
			break;

		case ITEM_PET_CARD:
			{
				if (sItemCustom.PetDbNum() == 0)
                    break;
				PETCARDINFO_MAP_ITER iter = m_pGaeaClient->GetCharacter()->m_mapPETCardInfo.find ( sItemCustom.PetDbNum() );
				if ( iter == m_pGaeaClient->GetCharacter()->m_mapPETCardInfo.end() )
				{
					iter = m_pGaeaClient->GetCharacter()->m_mapPETCardInfoTemp.find ( sItemCustom.PetDbNum() );
					if ( iter == m_pGaeaClient->GetCharacter()->m_mapPETCardInfoTemp.end() ) break;
				}

				SPETCARDINFO sPetCardInfo = (*iter).second;

				strText.Format(ID2GAMEWORD ("ITEM_PET_INFO", 0 ));
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::LIGHTSKYBLUE);

				strText.Format( "%s %s", ID2GAMEWORD ("ITEM_PET_NAME",0), sPetCardInfo.m_szName );
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);

				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strText.Format( "%s %s", ID2GAMEWORD ("ITEM_PET_TYPE",0), CGameTextMan::GetInstance().GetText("PET_TYPE", sPetCardInfo.m_emTYPE, CGameTextMan::EM_COMMENT_TEXT).GetString());
				else
					strText.Format( "%s %s", ID2GAMEWORD ("ITEM_PET_TYPE",0), COMMENT::PET_TYPE[sPetCardInfo.m_emTYPE].c_str() );
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);

				strText.Format( "%s %.2f%%", ID2GAMEWORD ("ITEM_PET_FULL",0), sPetCardInfo.m_nFull/10.0f );
				INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);

				if ( 2 <= sPetCardInfo.m_wMaxSkillSlot )
				{
					strText.Format( "%s", ID2GAMEWORD ("ITEM_PET_DUALSKILL",1) );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);
				}
				else
				{
					strText.Format( "%s", ID2GAMEWORD ("ITEM_PET_DUALSKILL",0) );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);
				}

				const SITEM* pItem = GLogicData::GetInstance().GetItem ( sPetCardInfo.m_PutOnItems[ACCETYPEA].GetNativeID() );
				if ( pItem )
				{
					strText.Format("%s %s", ID2GAMEWORD ("ITEM_PET_SLOTA",0), pItem->GetName () );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);
				}

				pItem = GLogicData::GetInstance().GetItem ( sPetCardInfo.m_PutOnItems[ACCETYPEB].GetNativeID() );
				if ( pItem )
				{
					strText.Format("%s %s", ID2GAMEWORD ("ITEM_PET_SLOTB",0), pItem->GetName () );
					INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);
				}

				if ( sItemCustom.tDISGUISE != 0 )
				{
					const CTime& currentTime = m_pGaeaClient->GetCurrentTime();
					CTime startTime   = sItemCustom.tBORNTIME;
					CTimeSpan timeSpan = currentTime - startTime;
					if( timeSpan.GetTotalSeconds() < (sItemCustom.tDISGUISE + sItemCustom.GetPeriodExDisguise()) )
					{
						strText.Format("[%s]", ID2GAMEWORD ("ITEM_PET_USE_SKINPACK",0) );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::GOLD);
						strText.Format("%s : %dsec", ID2GAMEWORD ("CLUB_BATTLE_TIME",0), sItemCustom.tDISGUISE - timeSpan.GetTotalSeconds() );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::GOLD);
					}
				}

				INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD ("ITEM_PET_SKILLS", 0 ), NS_UITEXTCOLOR::DEFAULT );

				PETSKILL_MAP_CITER pos = sPetCardInfo.m_ExpSkills.begin();
				PETSKILL_MAP_CITER pos_end = sPetCardInfo.m_ExpSkills.end();
				for ( ;pos != pos_end; ++pos )
				{
					const PETSKILL& sPetSkill = (*pos).second;
					PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData( sPetSkill.sNativeID );
					if ( pSKILL )
					{
						strText.Format("%s", pSKILL->GetName() );
						INFO_DISPLAY_ITEM_AddTextNoSplit(strText,NS_UITEXTCOLOR::DEFAULT);
					}
				}
			}
			break;

		case ITEM_LUNCHBOX:
			{
				INFO_DISPLAY_ITEM_AddInfoLunchBoxItem ( sItemCustom );
			}
			break;

		case ITEM_PET_DUALSKILL:
			{
				INFO_DISPLAY_ITEM_AddInfoPetDualSkillItem ( sItemCustom );
			}
			break;

        case ITEM_POINT_CARD_REFUND:
        case ITEM_POINT_CARD_NOT_REFUND:
            {
                // Note : Æ÷ÀÎÆ® ¾ç Ç¥½Ã.
                strText = sc::string::format( ID2GAMEWORD("POINT_CARD_POINT_VOLUME"), pItemData->sDrugOp.dwCureVolume ).c_str();
                INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::YELLOW );
            }
            break;
		}

		if( emItemType != ITEM_PET_SKILL && emItemType != ITEM_PET_DUALSKILL
            && emItemType != ITEM_POINT_CARD_REFUND && emItemType != ITEM_POINT_CARD_NOT_REFUND )
		{
			INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD( "ITEM_CATEGORY", 8 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );

			// Note : ½Ã±Í/Á÷±Í Ä«µå¿¡ ¸ÊÀÌ¸§ ÁÂÇ¥ Ãâ·Â
			if( pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_REGEN )
			{
				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();	

				CString strName( m_pGaeaClient->GetMapName( pCharacter->m_sStartMapID ) );
				if( strName == _T("(null)") ) strName.Empty();

				strText.Format( "%s:%s", ID2GAMEWORD("ITEM_CALL_MAP", 0 ), strName );
				INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::DEFAULT);

				SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode ( pCharacter->m_sStartMapID );
				if( pMapNode )
				{
					GLLevelFileClient cLevelFile(m_pGaeaClient);
					BOOL bOk = cLevelFile.LoadFile( pMapNode->m_LevelFile.c_str(), NULL );
					if( bOk )
					{	
						D3DXVECTOR3 vStartPos;
						PDXLANDGATE pGate = cLevelFile.GetLandGateMan()->FindLandGate( pCharacter->m_dwStartGate );
						if( pGate )
						{
							vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
						}
						else
						{
							pGate = cLevelFile.GetLandGateMan()->FindLandGate( DWORD(0) );
							if( pGate ) vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
						}

						int nPosX(0), nPosY(0);
						cLevelFile.GetMapAxisInfo().Convert2MapPos( vStartPos.x, vStartPos.z, nPosX, nPosY );

						strText.Format( "%s:%d,%d", ID2GAMEWORD("ITEM_CALL_MAP", 1 ), nPosX, nPosY );
						INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::DEFAULT);
					}
				}
			}
			else if( pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_LASTCALL )
			{
				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

				if( pCharacter->m_sLastCallMapID.IsValidNativeID() )
				{
					CString strName( m_pGaeaClient->GetMapName( pCharacter->m_sLastCallMapID ) );
					if( strName == _T("(null)") ) strName.Empty();

					strText.Format( "%s:%s", ID2GAMEWORD("ITEM_CALL_MAP", 0 ), strName );
					INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::DEFAULT);

					SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode ( pCharacter->m_sLastCallMapID );
					if( pMapNode )
					{
						GLLevelFileClient cLevelFile(m_pGaeaClient);
						BOOL bOk = cLevelFile.LoadFile( pMapNode->m_LevelFile.c_str(), NULL );
						if( bOk )
						{	
							int nPosX(0), nPosY(0);
							cLevelFile.GetMapAxisInfo().Convert2MapPos( pCharacter->m_vLastCallPos.x, pCharacter->m_vLastCallPos.z, nPosX, nPosY );

							strText.Format( "%s:%d,%d", ID2GAMEWORD("ITEM_CALL_MAP", 1 ), nPosX, nPosY );
							INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::DEFAULT);
						}
					}
				}
				else
				{
					INFO_DISPLAY_ITEM_AddTextNoSplit( ID2GAMEWORD("ITEM_CALL_MAP", 2 ), NS_UITEXTCOLOR::RED);
				}
			}
			else if ( pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_TELEPORT )
			{
				CString strName( m_pGaeaClient->GetMapName( pItemData->sBasicOp.sSubID ) );
				if( strName == _T("(null)") ) strName.Empty();

				strText.Format( "%s:%s", ID2GAMEWORD("ITEM_TELEPORT_MAP", 0 ), strName );
				INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::DEFAULT);

				strText.Format( "%s:%d,%d", ID2GAMEWORD("ITEM_TELEPORT_MAP", 1 ), pItemData->sBasicOp.wPosX, pItemData->sBasicOp.wPosY );
				INFO_DISPLAY_ITEM_AddTextNoSplit( strText, NS_UITEXTCOLOR::DEFAULT);
			}

            if ( emItemType != ITEM_POINT_CARD_REFUND && emItemType != ITEM_POINT_CARD_NOT_REFUND )
            {
				if ( pCharacter->ACCEPT_ITEM( sItemCustom ) )
			    {
				    strText.Format ( "[%s]", ID2GAMEWORD("ITEM_USABLE", 0 ) );
				    INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCOLOR::ENABLE );

				    WORD wNeedSP = pCharacter->CALC_ACCEPTP ( sItemCustom.GetNativeID() );
				    if ( 0 < wNeedSP )
				    {
					    strText.Format ( "%s:%d", ID2GAMEWORD("ITEM_NEEDSP" ), wNeedSP );
					    INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCOLOR::PALEGREEN );
				    }
			    }
			    else
			    {
				    strText.Format ( "[%s]", ID2GAMEWORD("ITEM_USABLE", 1 ) );
				    INFO_DISPLAY_ITEM_AddTextNoSplit(strText, NS_UITEXTCOLOR::RED );
			    }
            }
		}

		// Info : 거래 및 버리기
		{
			std::string strText = "";
			INFO_DISPLAY_ITEM_AddTextNoSplit ( "", ITEM_INFOR_COLOR::dwDefault );

			//DWORD dwItemFlag = pItemData->sBasicOp.dwFlags;
			//DWORD dwItemFlag = sItemCustom.IsFlags();

			DWORD dwTradeColor = ITEM_INFOR_COLOR::dwDefault;

			//bool bAllTradeOption = false;
			//if ( (dwItemFlag & TRADE_SALE) && (dwItemFlag & TRADE_THROW) && (dwItemFlag & TRADE_EXCHANGE) )
			//if ( sItemCustom.IsSale() && sItemCustom.IsThrow() && sItemCustom.IsExchange() )
			//	bAllTradeOption = true;

			//if ( !bAllTradeOption )
			{
				strText = sc::string::format( "%s ", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 0) );
				//if ( dwItemFlag & TRADE_SALE )
				if ( sItemCustom.IsSale() )
				{
					strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0);
					dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
				}
				else
				{
					strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1);
					dwTradeColor = ITEM_INFOR_COLOR::dwTrade;
				}
				INFO_DISPLAY_ITEM_AddTextNoSplit (strText.c_str(), dwTradeColor );

				strText = sc::string::format( "%s ", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 1) );
				//if ( dwItemFlag & TRADE_THROW )
				if ( sItemCustom.IsThrow() )
				{
					strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0);
					dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
				}
				else
				{
					strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1);
					dwTradeColor = ITEM_INFOR_COLOR::dwTrade;
				}
				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText.c_str(), dwTradeColor );

				strText = sc::string::format( "%s ", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 6) );
				//if ( dwItemFlag & TRADE_THROW )
				if ( sItemCustom.IsNotStorage() )
				{
					strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1);
					dwTradeColor = ITEM_INFOR_COLOR::dwTrade;
				}
				else
				{
					strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0);
					dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
				}
				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText.c_str(), dwTradeColor );

				strText = sc::string::format( "%s ", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 3) );
				//if ( dwItemFlag & TRADE_EXCHANGE )
				if ( sItemCustom.IsExchange() )
				{
					strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0);
					dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
				}
				else
				{
					strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1);
					dwTradeColor = ITEM_INFOR_COLOR::dwTrade;
				}

				INFO_DISPLAY_ITEM_AddTextNoSplit ( strText.c_str(), dwTradeColor );


				// 란 포인트 거래
				if ( GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
				{
					strText = sc::string::format( "%s ", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 4) );
					int RPTradeCountMax = pItemData->sBasicOp.RanPointTradeCount();
					int RPTradeCountNow = sItemCustom.wRanPointTradeCount;

					//if ( dwItemFlag & TRADE_EXCHANGE || 0 < RPTradeCountMax - RPTradeCountNow )
					if ( sItemCustom.IsExchange() && 0 < RPTradeCountMax - RPTradeCountNow )
					{
						strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 0);
						dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
					}
					else
					{
						strText += ID2GAMEWORD("ITEM_INFO_TRADE_OPTION", 1);
						dwTradeColor = ITEM_INFOR_COLOR::dwTrade;
					}

					if( 0 < RPTradeCountMax )
						strText += sc::string::format( " (%1%/%2%)", RPTradeCountMax-RPTradeCountNow, RPTradeCountMax );

					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText.c_str(), dwTradeColor );
				}

				// 포장 가능 여부 표시 : 포장지 포장 가능 여부는 기존 거래 및 버리기 옵션과 다르게 포장 가능인 아이템에 한하여 메시지 출력;
				if ( sItemCustom.IsBoxWrappable() )
				{
					strText = sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 5) );
					dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
					INFO_DISPLAY_ITEM_AddTextNoSplit ( strText.c_str(), dwTradeColor );
				}
			}
			//else
			//{
			//	// 포장 가능 여부 표시 : 포장지 포장 가능 여부는 기존 거래 및 버리기 옵션과 다르게 포장 가능인 아이템에 한하여 메시지 출력;
			//	if ( sItemCustom.IsBoxWrappable() )
			//	{
			//		strText = sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_TRADE_RN", 5) );
			//		dwTradeColor = ITEM_INFOR_COLOR::dwDefault;
			//		INFO_DISPLAY_ITEM_AddTextNoSplit (strText.c_str(), dwTradeColor );
			//	}
			//}
		}

        // ¾ÆÀÌÅÛ ¹Ì¸®º¸±â
        if ( bUsePreview )
        {
            if ( ItemPreviewWindowIsPreviewItem( sItemCustom ) )
            {
                INFO_DISPLAY_ITEM_AddTextLongestLineSplit( ID2GAMEWORD("ITEM_USEPREVIEW"), NS_UITEXTCOLOR::GOLD );
            }
		}

		switch ( emItemType )
		{
		case ITEM_RANDOMITEM:
		case ITEM_BOX:
		case ITEM_SELECTIVEFORM_BOX:
			INFO_DISPLAY_ITEM_AddTextLongestLineSplit ( ID2GAMEINTEXT("HELP_SHOW_ITEM_TOOLTIP"), ITEM_INFOR_COLOR::dwDefault );
			break;
		}

		{ // ¼³¸í
			const TCHAR * pszComment(NULL);

			if ( (emItemType != ITEM_SKILL) || (emItemType != ITEM_PET_SKILL) )
			{
				pszComment = pItemData->GetComment();
			}
			// Note : ½ÇÇà µÇÁö ¾Ê´Â ÄÚµå ÀÔ´Ï´Ù.
			else
			{
				SNATIVEID sNativeID = sItemCustom.GetNativeID();
				const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sNativeID );
				SNATIVEID sSkillID = pItemData->sSkillBookOp.sSkill_ID;

				//	Note : ½ºÅ³ Á¤º¸ °¡Á®¿È.
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
				if ( pSkill )
				{
					pszComment = pSkill->GetDesc();
				}
			}

			if( pszComment )
			{
				int StrLength = static_cast<int>( strlen( pszComment ) );
				if( StrLength )
				{
					INFO_DISPLAY_ITEM_AddTextNoSplit ( ID2GAMEWORD( "ITEM_CATEGORY", 9 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );
					INFO_DISPLAY_SKILL_AddTextLongestLineSplit( pszComment, NS_UITEXTCOLOR::DEFAULT );
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------//
// SKILLINFO

void CInnerInterface::INFO_DISPLAY_SKILL_RESET ()
{
	m_INFO_DISPLAY_SKILL_sNativeID_Previous		= NATIVEID_NULL();
	m_INFO_DISPLAY_SKILL_bNextLevel_Previous	= FALSE;
    m_INFO_DISPLAY_SKILL_wLevel_Previous        = 0;
}

void CInnerInterface::INFO_DISPLAY_SKILL_AddTextNoSplit ( CString strText, D3DCOLOR dwColor )
{
	ADDTEXT_NOSPLIT ( strText, dwColor );
}

void CInnerInterface::INFO_DISPLAY_SKILL_AddTextLongestLineSplit ( CString strText, D3DCOLOR dwColor )
{
	ADDTEXT_LONGESTLINE_SPLIT ( strText, dwColor );
}

void CInnerInterface::INFO_DISPLAY_SKILL_RemoveAllInfo ()
{
	CLEAR_TEXT ();
}

void CInnerInterface::INFO_DISPLAY_SKILL_LOAD ( GLSKILL* const pSkill, SCHARSKILL* const pCharSkill, BOOL bNextLevel )
{
	CString	strText;

	//	½ºÅ³ Á¤º¸ ÀÔ·Â

	WORD wDispLevel = 0;			
	if ( pCharSkill )	//	¹è¿î ½ºÅ³
	{
		wDispLevel = pCharSkill->wLevel + 1;				
	}

	BOOL bMASTER = m_pGaeaClient->GetCharacter()->ISMASTER_SKILL ( pSkill->m_sBASIC.sNATIVEID );
	if ( bNextLevel )
	{
		if ( bMASTER )
		{
			//	NOTE
			//		¸¸¾à, ¸¶½ºÅÍ¶ó¸é
			//		´ÙÀ½ ·¹º§À» »Ñ¸®´Â°ÍÀÌ ¾Æ´Ï¶ó,
			//		ÇöÀç ·¹º§ÀÇ °ªµéÀ» »Ñ¸®µµ·Ï ÇÑ´Ù.
			bNextLevel = FALSE;
		}
		else
		{
			if ( wDispLevel )
			{				
				strText.Format ("%s:%d", ID2GAMEWORD("SKILL_NEXTLEVEL"), wDispLevel + 1 );
				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::RED);
			}
		}
	}

	strText = ID2GAMEWORD("SKILL_CATEGORY", 0);
	INFO_DISPLAY_SKILL_AddTextNoSplit(strText,NS_UITEXTCOLOR::LIGHTSKYBLUE);

	//	±âº» Á¤º¸
	{
		//	1. ÀÌ¸§			
		strText.Format("%s:%s", ID2GAMEWORD("SKILL_BASIC_INFO", 0), pSkill->GetName());
		if ( pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
		{
			CString strDescText;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				strDescText.Format( "(%s)",CGameTextMan::GetInstance().GetCommentText("SKILL_ROLE", SKILL::EMROLE_PASSIVE).GetString() );
			else
				strDescText.Format( "(%s)",COMMENT::SKILL_ROLE[SKILL::EMROLE_PASSIVE].c_str() );
			strText += strDescText;
		}
		INFO_DISPLAY_SKILL_AddTextNoSplit(strText,NS_UITEXTCOLOR::PALEGREEN);

		//	2. µî±Þ
		strText.Format("%s:%d",ID2GAMEWORD("SKILL_BASIC_INFO", 1), pSkill->m_sBASIC.dwGRADE);
		INFO_DISPLAY_SKILL_AddTextNoSplit(strText,NS_UITEXTCOLOR::PALEGREEN);

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			//	5. Å¸ÀÔ;
			strText.Format("%s:%s",ID2GAMEWORD("SKILL_BASIC_INFO", 5), CGameTextMan::GetInstance().GetCommentText("SKILL_ACTION_TYPE", pSkill->m_sBASIC.emACTION_TYPE).GetString());
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText,NS_UITEXTCOLOR::PALEGREEN);

			//	3. ¼Ó¼º;
			strText.Format("%s:%s",ID2GAMEWORD("SKILL_BASIC_INFO", 2), CGameTextMan::GetInstance().GetCommentText("BRIGHT", pSkill->m_sLEARN.emBRIGHT).GetString());
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText,NS_UITEXTCOLOR::PALEGREEN);

			//	4. Á÷¾÷;
			strText.Format ("%s:", ID2GAMEWORD("SKILL_BASIC_INFO", 3) );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_FIGHTER_M )		strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_FIGHTER_M).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_FIGHTER_W )		strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_FIGHTER_W).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ARMS_M )			strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_ARMS_M).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ARMS_W )			strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_ARMS_W).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ARCHER_M )		strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_ARCHER_M).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ARCHER_W )		strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_ARCHER_W).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_SPIRIT_M )		strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_SPIRIT_M).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_SPIRIT_W )		strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_SPIRIT_W).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_EXTREME_M )	strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_EXTREME_M).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_EXTREME_W )	strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_EXTREME_W).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_SCIENTIST_M )	strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_SCIENTIST_M).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_SCIENTIST_W )	strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_SCIENTIST_W).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ASSASSIN_M )	strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_ASSASSIN_M).GetString() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ASSASSIN_W )	strText.AppendFormat ( " %s", CGameTextMan::GetInstance().GetCommentText("CHARCLASS",GLCI_ASSASSIN_W).GetString() );
		}
		else
		{
			//	5. Å¸ÀÔ
			strText.Format("%s:%s",ID2GAMEWORD("SKILL_BASIC_INFO", 5), COMMENT::SKILL_ACTION_TYPE[pSkill->m_sBASIC.emACTION_TYPE].c_str());
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText,NS_UITEXTCOLOR::PALEGREEN);


			//	3. ¼Ó¼º
			strText.Format("%s:%s",ID2GAMEWORD("SKILL_BASIC_INFO", 2), COMMENT::BRIGHT[pSkill->m_sLEARN.emBRIGHT].c_str());
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText,NS_UITEXTCOLOR::PALEGREEN);

			//	4. Á÷¾÷
			strText.Format ("%s:", ID2GAMEWORD("SKILL_BASIC_INFO", 3) );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_FIGHTER_M )		strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_FIGHTER_M].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_FIGHTER_W )		strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_FIGHTER_W].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ARMS_M )			strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_ARMS_M].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ARMS_W )			strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_ARMS_W].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ARCHER_M )		strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_ARCHER_M].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ARCHER_W )		strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_ARCHER_W].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_SPIRIT_M )		strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_SPIRIT_M].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_SPIRIT_W )		strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_SPIRIT_W].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_EXTREME_M )	strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_EXTREME_M].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_EXTREME_W )	strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_EXTREME_W].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_SCIENTIST_M )	strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_SCIENTIST_M].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_SCIENTIST_W )	strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_SCIENTIST_W].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ASSASSIN_M )	strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_ASSASSIN_M].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ASSASSIN_W )	strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_ASSASSIN_W].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_TRICKER_M )	strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCC_TRICKER_M].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCI_TRICKER_W )	strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_TRICKER_W].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCC_ETC_M )	strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCC_ETC_M].c_str() );
			if ( pSkill->m_sLEARN.dwCLASS & GLCI_ETC_W )	strText.AppendFormat ( " %s", COMMENT::CHARCLASS[GLCI_ETC_W].c_str() );
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText,NS_UITEXTCOLOR::PALEGREEN);
		}
	}

	//	¸¶½ºÅÍ
	BOOL bMaster = FALSE;
	if ( bMASTER )
	{
		INFO_DISPLAY_SKILL_AddTextNoSplit(ID2GAMEWORD("SKILL_ETC_MASTER"), NS_UITEXTCOLOR::ENABLE);
		bMaster = TRUE;
	}
	else
	{
		if ( wDispLevel )
		{			
			if( !bNextLevel ) 
				strText.Format ("%s:%d", ID2GAMEWORD("SKILL_BASIC_INFO", 4), wDispLevel );

			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::ENABLE);
		}
	}


	//	¸ñÇ¥ °¡´É°Å¸®, ¿Þ¼Õ, ¿À¸¥¼Õ µµ±¸
	{
		const SKILL::SBASIC& sBASIC = pSkill->m_sBASIC;		

		if ( sBASIC.wTARRANGE )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_TARGET_RANGE", 0), sBASIC.wTARRANGE);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
		}

		if ( (sBASIC.emUSE_LITEM && (sBASIC.emUSE_LITEM != SKILL::SKILLATT_NOCARE)) || 
			(sBASIC.emUSE_RITEM && (sBASIC.emUSE_RITEM != SKILL::SKILLATT_NOCARE)) )
		{
			INFO_DISPLAY_SKILL_AddTextNoSplit(ID2GAMEWORD("SKILL_CATEGORY", 1),NS_UITEXTCOLOR::LIGHTSKYBLUE);
		}

		if ( sBASIC.emUSE_LITEM )
		{
			if ( sBASIC.emUSE_LITEM != SKILL::SKILLATT_NOCARE )
			{
				strText.Format ( "%s:%s",	ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM_LR_HAND", 0), 
					ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM", sBASIC.emUSE_LITEM - 1 ) );

				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			}
		}

		if ( sBASIC.emUSE_RITEM )
		{
			if ( sBASIC.emUSE_RITEM != SKILL::SKILLATT_NOCARE )
			{
				strText.Format ( "%s:%s",	ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM_LR_HAND", 1), 
					ID2GAMEWORD( "SKILL_ADVANCED_USE_ITEM", sBASIC.emUSE_RITEM - 1 ) );

				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			}
		}
	}


	const SKILL::SAPPLY& sAPPLY = pSkill->m_sAPPLY;

	//	Æ¯¼öÈ¿°ú
	WORD wLevel = 0;
	if ( pCharSkill )	//	¹è¿î ½ºÅ³ÀÏ °æ¿ì¿¡...
	{
		wLevel = pCharSkill->wLevel;
		if ( bNextLevel ) wLevel = pCharSkill->wLevel + 1;
	}

	//	±âº» ¼Ó¼º
	if ( pCharSkill )	//	¹è¿î ½ºÅ³ÀÏ °æ¿ì¿¡...
	{		
		INFO_DISPLAY_SKILL_AddTextNoSplit(ID2GAMEWORD("SKILL_CATEGORY", 2),NS_UITEXTCOLOR::LIGHTSKYBLUE);

		const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[wLevel];

		//	µô·¹ÀÌ Å¸ÀÓ
		if ( 0 < sDATA_LVL.fDELAYTIME )
		{
			strText.Format("%s:%2.1f",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC", 0), sDATA_LVL.fDELAYTIME);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
		}

		//	Áö¼Ó ½Ã°£
		if ( 0 < sDATA_LVL.fLIFE )
		{
			strText.Format("%s:%2.1f",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC", 1), sDATA_LVL.fLIFE);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
		}

		//	Àû¿ë ¿µ¿ª
		if ( 0 < sDATA_LVL.wAPPLYRANGE )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC", 2), sDATA_LVL.wAPPLYRANGE);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
		}

		//	Àû¿ë °¹¼ö
		if ( 0 < sDATA_LVL.wAPPLYNUM )
		{ // »ç¿ëÇÏÁö ¾Ê´Â °ªÀÌ°í ¸ðµç ½ºÅ³µéÀÌ 1·Î ¼³Á¤ µÇ¾î ÀÖÀ½;
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC", 3), sDATA_LVL.wAPPLYNUM);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
		}

		//	Àû¿ë °¢µµ
		if ( 0 < sDATA_LVL.wAPPLYANGLE )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC", 4), sDATA_LVL.wAPPLYANGLE);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
		}

		//	°üÅë Á¤µµ
		if ( 0 < sDATA_LVL.wPIERCENUM )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC", 5), sDATA_LVL.wPIERCENUM);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
		}

		//	º¯È­°ª
		if ( 0 != sDATA_LVL.fBASIC_VAR )
		{
			float fBASIC_VAR_SCALE = sDATA_LVL.fBASIC_VAR * COMMENT::SKILL_TYPES_SCALE[sAPPLY.emBASIC_TYPE];

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				if ( COMMENT::IsSKILL_PER(sAPPLY.emBASIC_TYPE) )
					strText.Format("%s:%2.1f%%",CGameTextMan::GetInstance().GetCommentText("SKILL_TYPES", sAPPLY.emBASIC_TYPE).GetString(), fBASIC_VAR_SCALE );
				else
					strText.Format("%s:%2.1f",CGameTextMan::GetInstance().GetCommentText("SKILL_TYPES", sAPPLY.emBASIC_TYPE).GetString(), fBASIC_VAR_SCALE );
			}
			else
			{
				if ( COMMENT::IsSKILL_PER(sAPPLY.emBASIC_TYPE) )
					strText.Format("%s:%2.1f%%",COMMENT::SKILL_TYPES[sAPPLY.emBASIC_TYPE].c_str(), fBASIC_VAR_SCALE );
				else
					strText.Format("%s:%2.1f",COMMENT::SKILL_TYPES[sAPPLY.emBASIC_TYPE].c_str(), fBASIC_VAR_SCALE );
			}
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
		}

		//	Å¸°Ù °¹¼ö
		if ( 0 < sDATA_LVL.wTARNUM )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC", 6), sDATA_LVL.wTARNUM);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
		}


		///////////////////////////////////////////////////////////////////////
		//	»ç¿ë½Ã ¼ÒÁø·®
		if ( pSkill->m_sBASIC.emROLE != SKILL::EMROLE_PASSIVE )
		{
			INFO_DISPLAY_SKILL_AddTextNoSplit(ID2GAMEWORD("SKILL_CATEGORY", 3),NS_UITEXTCOLOR::LIGHTSKYBLUE);

			//	È­»ì °³¼ö
			if ( 0 < sDATA_LVL.wUSE_ARROWNUM )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 0), sDATA_LVL.wUSE_ARROWNUM);
				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			}

			//	ºÎÀû °³¼ö
			if ( 0 < sDATA_LVL.wUSE_CHARMNUM )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 1), sDATA_LVL.wUSE_CHARMNUM);
				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			}

			//	ÃÑ¾Ë °³¼ö
			if ( 0 < sDATA_LVL.wUSE_BULLETNUM )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 7), sDATA_LVL.wUSE_BULLETNUM);
				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			}

			// ¹èÅÍ¸® ¼Ò¸ð·®
			if ( 0 < sDATA_LVL.nUSE_BATTERY )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 7), sDATA_LVL.nUSE_BATTERY);
				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			}

			//	EXP ¼ÒÁø·®
			//if ( 0 < sDATA_LVL.wUSE_EXP )
			//{
			//	strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 2), sDATA_LVL.wUSE_EXP);
			//	INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			//}

			//	HP ¼ÒÁø·®
			if ( 0 < sDATA_LVL.wUSE_HP )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 3), sDATA_LVL.wUSE_HP);
				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			}

			//	MP ¼ÒÁø·®
			if ( 0 < sDATA_LVL.wUSE_MP )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 4), sDATA_LVL.wUSE_MP);
				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			}

			//	SP ¼ÒÁø·®
			if ( 0 < sDATA_LVL.wUSE_SP )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 5), sDATA_LVL.wUSE_SP);
				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			}

			//	CP ¼ÒÁø·®
			if ( 0 < sDATA_LVL.wUSE_CP )
			{
				strText.Format("%s:%d",ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC_USE_VALUE", 6), sDATA_LVL.wUSE_CP);
				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::DEFAULT );
			}
		}
	}

	{	
		if ( sAPPLY.emSTATE_BLOW != EMBLOW_NONE )
		{
			INFO_DISPLAY_SKILL_AddTextNoSplit(ID2GAMEWORD("SKILL_CATEGORY", 4),NS_UITEXTCOLOR::LIGHTSKYBLUE);

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				INFO_DISPLAY_SKILL_AddTextNoSplit(CGameTextMan::GetInstance().GetCommentText("BLOW", sAPPLY.emSTATE_BLOW).GetString(), NS_UITEXTCOLOR::PRIVATE );

				const SKILL::SSTATE_BLOW& sSTATE_BLOW = sAPPLY.sSTATE_BLOW[wLevel];
				if ( 0 < sSTATE_BLOW.fRATE )
				{			
					strText.Format ( "%s:%2.1f", ID2GAMEWORD( "SKILL_STATE_BLOW", 0), sSTATE_BLOW.fRATE );
					INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
				}

				if ( 0 < sSTATE_BLOW.fVAR1 )
				{
					strText.Format ( "%s:%2.1f", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR1", sAPPLY.emSTATE_BLOW).GetString(), sSTATE_BLOW.fVAR1 );
					INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
				}

				if ( 0 < sSTATE_BLOW.fVAR2 )
				{
					strText.Format ( "%s:%2.1f", CGameTextMan::GetInstance().GetCommentText("BLOW_VAR2", sAPPLY.emSTATE_BLOW).GetString(), sSTATE_BLOW.fVAR2 );
					INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
				}
			}
			else
			{
				INFO_DISPLAY_SKILL_AddTextNoSplit(COMMENT::BLOW [ sAPPLY.emSTATE_BLOW ].c_str(), NS_UITEXTCOLOR::PRIVATE );

				const SKILL::SSTATE_BLOW& sSTATE_BLOW = sAPPLY.sSTATE_BLOW[wLevel];
				if ( 0 < sSTATE_BLOW.fRATE )
				{			
					strText.Format ( "%s:%2.1f", ID2GAMEWORD( "SKILL_STATE_BLOW", 0), sSTATE_BLOW.fRATE );
					INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
				}

				if ( 0 < sSTATE_BLOW.fVAR1 )
				{
					strText.Format ( "%s:%2.1f", COMMENT::BLOW_VAR1[sAPPLY.emSTATE_BLOW].c_str(), sSTATE_BLOW.fVAR1 );
					INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
				}

				if ( 0 < sSTATE_BLOW.fVAR2 )
				{
					strText.Format ( "%s:%2.1f", COMMENT::BLOW_VAR2[sAPPLY.emSTATE_BLOW].c_str(), sSTATE_BLOW.fVAR2 );
					INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
				}
			}
		}
	}

	//	ºÎ°¡È¿°ú
	//if ( pCharSkill )	//	¹è¿î ½ºÅ³ÀÏ °æ¿ì¿¡...
	{
		if ( !sAPPLY.vecADDON.empty() )
		{
			INFO_DISPLAY_SKILL_AddTextNoSplit(ID2GAMEWORD("SKILL_CATEGORY", 5),NS_UITEXTCOLOR::LIGHTSKYBLUE);

			for ( DWORD i = 0; i < sAPPLY.vecADDON.size(); ++i )
			{
				const float fADDON_VAR = sAPPLY.vecADDON[i].fADDON_VAR[wLevel];
				if ( 0.0f != fADDON_VAR )
				{
					float fADDON_VAR_SCALE = fADDON_VAR * COMMENT::IMPACT_ADDON_SCALE[sAPPLY.vecADDON[i].emADDON];

					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						if ( COMMENT::IsIMPACT_ADDON_PER(sAPPLY.vecADDON[i].emADDON) )
						{
							strText.Format ( "%s:%2.2f%%", CGameTextMan::GetInstance().GetCommentText("IMPACT_ADDON", sAPPLY.vecADDON[i].emADDON).GetString(), fADDON_VAR_SCALE );
						}
						else
						{
							strText.Format ( "%s:%2.2f", CGameTextMan::GetInstance().GetCommentText("IMPACT_ADDON", sAPPLY.vecADDON[i].emADDON).GetString(), fADDON_VAR_SCALE );
						}
					}
					else
					{
						if ( COMMENT::IsIMPACT_ADDON_PER(sAPPLY.vecADDON[i].emADDON) )
						{
							strText.Format ( "%s:%2.2f%%", COMMENT::IMPACT_ADDON[sAPPLY.vecADDON[i].emADDON].c_str(), fADDON_VAR_SCALE );
						}
						else
						{
							strText.Format ( "%s:%2.2f", COMMENT::IMPACT_ADDON[sAPPLY.vecADDON[i].emADDON].c_str(), fADDON_VAR_SCALE );
						}
					}

					INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
				}
			}
		}
	}

	{
		if ( !sAPPLY.vecSPEC.empty() )
		{
			INFO_DISPLAY_SKILL_AddTextNoSplit(ID2GAMEWORD("SKILL_CATEGORY", 6),NS_UITEXTCOLOR::LIGHTSKYBLUE);
			

			for ( DWORD i = 0; i <  sAPPLY.vecSPEC.size(); ++i )
			{
				const SKILL::SSPEC& sSPEC = sAPPLY.vecSPEC[i].sSPEC[wLevel];
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strText.Format ( "%s:%s", ID2GAMEWORD("SKILL_SPEC_TYPE", 0), CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON", sAPPLY.vecSPEC[i].emSPEC).GetString() );
				else
					strText.Format ( "%s:%s", ID2GAMEWORD("SKILL_SPEC_TYPE", 0), CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON", sAPPLY.vecSPEC[i].emSPEC).GetString() );
				INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );

				strText.Empty();
				CString strTemp;
				if ( sAPPLY.vecSPEC[i].emSPEC == EMSPECA_NONBLOW || sAPPLY.vecSPEC[i].emSPEC == EMSPECA_RECBLOW )
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						if ( sSPEC.dwFLAG & DIS_NUMB )
						{
							strTemp.Format ( "%s ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_NUMB).GetString() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_STUN )
						{
							strTemp.Format ( "%s ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_STUN).GetString() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_STONE )
						{
							strTemp.Format ( "%s ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_STONE).GetString() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_BURN )
						{
							strTemp.Format ( "%s ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_BURN).GetString() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_FROZEN )
						{
							strTemp.Format ( "%s ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_FROZEN).GetString() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_MAD )
						{
							strTemp.Format ( "%s ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_MAD).GetString() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_POISON )
						{
							strTemp.Format ( "%s ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_POISON).GetString() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_CURSE )
						{
							strTemp.Format ( "%s ", CGameTextMan::GetInstance().GetCommentText("BLOW",EMBLOW_CURSE).GetString() );
							strText += strTemp;
						}
					}
					else
					{
						if ( sSPEC.dwFLAG & DIS_NUMB )
						{
							strTemp.Format ( "%s ", COMMENT::BLOW[EMBLOW_NUMB].c_str() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_STUN )
						{
							strTemp.Format ( "%s ", COMMENT::BLOW[EMBLOW_STUN].c_str() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_STONE )
						{
							strTemp.Format ( "%s ", COMMENT::BLOW[EMBLOW_STONE].c_str() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_BURN )
						{
							strTemp.Format ( "%s ", COMMENT::BLOW[EMBLOW_BURN].c_str() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_FROZEN )
						{
							strTemp.Format ( "%s ", COMMENT::BLOW[EMBLOW_FROZEN].c_str() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_MAD )
						{
							strTemp.Format ( "%s ", COMMENT::BLOW[EMBLOW_MAD].c_str() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_POISON )
						{
							strTemp.Format ( "%s ", COMMENT::BLOW[EMBLOW_POISON].c_str() );
							strText += strTemp;
						}
						if ( sSPEC.dwFLAG & DIS_CURSE )
						{
							strTemp.Format ( "%s ", COMMENT::BLOW[EMBLOW_CURSE].c_str() );
							strText += strTemp;
						}
					}
				}
				// ¹ßµ¿ ½ºÅ³
				else if ( sAPPLY.vecSPEC[i].emSPEC == EMSPECA_DEFENSE_SKILL_ACTIVE )
				{
					GLSKILL* pSkillTemp = GLSkillMan::GetInstance().GetData( sSPEC.dwNativeID );
					if ( pSkillTemp )
					{
						strTemp.Format ( "%s Lv%d", pSkillTemp->GetName(),sSPEC.dwFLAG );
						strText += strTemp;
					}
				}

				if ( strText.GetLength () )
				{
					INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
				}

				// Æ¯¼öÈ¿°ú°¡ ¾øÀ½ ÀÏ¶§´Â È­¸é¿¡ º¸¿©ÁÖÁö ¾Êµµ·Ï ÀÓ½Ã ¼öÁ¤~
				// Æ¯¼öÈ¿°ú°¡ ¾øÀ½ ÀÏ¶§´Â Åø¿¡¼­ Á÷Á¢ °ª¿¡ 0À» ³Öµµ·Ï ¹Ù²Ù´Â°Ô ÁÁ´Ù
				if ( 0 != sSPEC.fVAR1 )
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						std::string fSPEC_ADDON_VAR1_NULL = CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR1", sAPPLY.vecSPEC[i].emSPEC*EMSPECA_NULL).GetString();
						std::string fSPEC_ADDON_VAR1 = CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR1", sAPPLY.vecSPEC[i].emSPEC).GetString();
						
						if( fSPEC_ADDON_VAR1_NULL != fSPEC_ADDON_VAR1 )
						{
							float fVAR1_SCALE = sSPEC.fVAR1 * COMMENT::SPEC_ADDON_VAR1_SCALE[sAPPLY.vecSPEC[i].emSPEC];;

							if ( COMMENT::IsSPEC_ADDON1_PER(sAPPLY.vecSPEC[i].emSPEC) )
								strText.Format ( "%s:%2.2f%%", fSPEC_ADDON_VAR1.c_str(), fVAR1_SCALE );
							else
								strText.Format ( "%s:%2.2f", fSPEC_ADDON_VAR1.c_str(), fVAR1_SCALE );

							INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
						}
					}
					else
					{
						if( COMMENT::SPEC_ADDON_VAR1[EMSPECA_NULL] != COMMENT::SPEC_ADDON_VAR1[sAPPLY.vecSPEC[i].emSPEC] )
						{
							float fVAR1_SCALE = sSPEC.fVAR1 * COMMENT::SPEC_ADDON_VAR1_SCALE[sAPPLY.vecSPEC[i].emSPEC];

							if ( COMMENT::IsSPEC_ADDON1_PER(sAPPLY.vecSPEC[i].emSPEC) )
								strText.Format ( "%s:%2.2f%%", COMMENT::SPEC_ADDON_VAR1[sAPPLY.vecSPEC[i].emSPEC].c_str(), fVAR1_SCALE );
							else
								strText.Format ( "%s:%2.2f", COMMENT::SPEC_ADDON_VAR1[sAPPLY.vecSPEC[i].emSPEC].c_str(), fVAR1_SCALE );

							INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
						}
					}
				}

				if ( 0 != sSPEC.fVAR2 )
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						std::string fSPEC_ADDON_VAR2_NULL = CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR2", sAPPLY.vecSPEC[i].emSPEC*EMSPECA_NULL).GetString();
						std::string fSPEC_ADDON_VAR2 = CGameTextMan::GetInstance().GetCommentText("SPEC_ADDON_VAR2", sAPPLY.vecSPEC[i].emSPEC).GetString();

						if( fSPEC_ADDON_VAR2_NULL != fSPEC_ADDON_VAR2 &&
							sAPPLY.vecSPEC[i].emSPEC != EMSPECA_PULLIN &&  sAPPLY.vecSPEC[i].emSPEC != EMSPECA_ONWARD )
						{
							float fVAR2_SCALE = sSPEC.fVAR2 * COMMENT::SPEC_ADDON_VAR2_SCALE[sAPPLY.vecSPEC[i].emSPEC];

							if ( COMMENT::IsSPEC_ADDON2_PER(sAPPLY.vecSPEC[i].emSPEC) )
								strText.Format ( "%s:%2.1f%%", fSPEC_ADDON_VAR2.c_str(), fVAR2_SCALE );
							else
								strText.Format ( "%s:%2.1f", fSPEC_ADDON_VAR2.c_str(), fVAR2_SCALE );

							INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
						}
					}
					else
					{
						if( COMMENT::SPEC_ADDON_VAR2[EMSPECA_NULL] != COMMENT::SPEC_ADDON_VAR2[sAPPLY.vecSPEC[i].emSPEC] &&
							sAPPLY.vecSPEC[i].emSPEC != EMSPECA_PULLIN &&  sAPPLY.vecSPEC[i].emSPEC != EMSPECA_ONWARD )
						{
							float fVAR2_SCALE = sSPEC.fVAR2 * COMMENT::SPEC_ADDON_VAR2_SCALE[sAPPLY.vecSPEC[i].emSPEC];

							if ( COMMENT::IsSPEC_ADDON2_PER(sAPPLY.vecSPEC[i].emSPEC) )
								strText.Format ( "%s:%2.1f%%", COMMENT::SPEC_ADDON_VAR2[sAPPLY.vecSPEC[i].emSPEC].c_str(), fVAR2_SCALE );
							else
								strText.Format ( "%s:%2.1f", COMMENT::SPEC_ADDON_VAR2[sAPPLY.vecSPEC[i].emSPEC].c_str(), fVAR2_SCALE );

							INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCOLOR::PRIVATE );
						}
					}
				}
			}
		}
	}

	BOOL bNOT_LEARN = wDispLevel==0;

	//	¿ä±¸Ä¡ Á¤º¸
	if ( (!bMaster && bNextLevel) || bNOT_LEARN )
	{
		INFO_DISPLAY_SKILL_AddTextNoSplit(ID2GAMEWORD("SKILL_CATEGORY", 7),NS_UITEXTCOLOR::LIGHTSKYBLUE);

		WORD wNextLevel = wDispLevel;
		GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
		SKILL::SLEARN& sLEARN = pSkill->m_sLEARN;
		SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[wNextLevel];			


		//	1. ¿ä±¸º¸À¯½ºÅ³
		SNATIVEID NeedSkillID = pSkill->m_sLEARN.sSKILL;			
		if ( NeedSkillID != NATIVEID_NULL() )
		{
			BOOL bVALID = FALSE;
			BOOL bNeedSkillLevel = FALSE;

			CString strNeedSkillName;
			CString strNeedSkillLevel;

			PGLSKILL pNeedSkill = GLSkillMan::GetInstance().GetData ( NeedSkillID.wMainID, NeedSkillID.wSubID );
			if( pNeedSkill ) strNeedSkillName.Format("%s:%s", ID2GAMEWORD( "SKILL_ADVANCED_INFO", 0), pNeedSkill->GetName() );
			bVALID = pCharacter->ISLEARNED_SKILL ( NeedSkillID );			

			//	2. ¿ä±¸º¸À¯½ºÅ³·¹º§
			if ( 0 < sLVL.dwSKILL_LVL )
			{
				strNeedSkillLevel.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO", 1), (sLVL.dwSKILL_LVL + 1) );
				bNeedSkillLevel = TRUE;

				SKILL_MAP& map = pCharacter->m_ExpSkills;				
				SKILL_MAP_ITER iter = map.find ( NeedSkillID.dwID );
				if ( iter != map.end() )
				{
					SCHARSKILL& rCharSkill = (*iter).second;

					//	»ö Á¶Àý
					bVALID = rCharSkill.wLevel >= sLVL.dwSKILL_LVL;						
				}
			}

			INFO_DISPLAY_SKILL_AddTextNoSplit(strNeedSkillName,NS_UITEXTCONTROL::GetEvaluateColor ( bVALID ));

			if ( bNeedSkillLevel )
				INFO_DISPLAY_SKILL_AddTextNoSplit(strNeedSkillLevel, NS_UITEXTCONTROL::GetEvaluateColor ( bVALID ) );
		}

		//	3. ¿ä±¸°æÇèÄ¡
		if ( 0 < sLVL.dwSKP )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO", 2), sLVL.dwSKP);				
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_dwSkillPoint >= sLVL.dwSKP ) );
		}

		//	4. ¿ä±¸·¹º§
		if ( 0 < sLVL.dwLEVEL )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO", 3), sLVL.dwLEVEL);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->GETLEVEL () >= int(sLVL.dwLEVEL) ) );
		}

		//	Á¶°Ç - ¾Ï±¤
		//BOOL bValue = TRUE;
		//strText.Format ( "%s", COMMENT::BRIGHT[sLEARN.emBRIGHT].c_str() );
		//if ( sLEARN.emBRIGHT != BRIGHT_BOTH )
		//{
		//	if ( pCharacter->GETBRIGHT() != sLEARN.emBRIGHT )
		//	{
		//		bValue = FALSE;
		//	}
		//}
		//INFO_DISPLAY_SKILL_AddTextNoSplit ( strText, NS_UITEXTCONTROL::GetEvaluateColor ( bValue ) );


		//	Stats
		//	1. ¿ä±¸Èû
		if ( 0 < sLVL.sSTATS.wPow )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 0), sLVL.sSTATS.wPow);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wPow >= sLVL.sSTATS.wPow ) );
		}

		//	2. ¿ä±¸Ã¼·Â
		if ( 0 < sLVL.sSTATS.wStr )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 1), sLVL.sSTATS.wStr);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wStr >= sLVL.sSTATS.wStr ) );
		}

		//	3. ¿ä±¸Á¤½Å
		if ( 0 < sLVL.sSTATS.wSpi )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 2), sLVL.sSTATS.wSpi);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wSpi >= sLVL.sSTATS.wSpi ));
		}

		//	4. ¿ä±¸¹ÎÃ¸
		if ( 0 < sLVL.sSTATS.wDex )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 3), sLVL.sSTATS.wDex);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wDex >= sLVL.sSTATS.wDex ) );
		}

		//	5. ¿ä±¸Áö·Â
		if ( 0 < sLVL.sSTATS.wInt )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 4), sLVL.sSTATS.wInt);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wInt >= sLVL.sSTATS.wInt ) );
		}

		//	6. ¿ä±¸±Ù·Â
		if ( 0 < sLVL.sSTATS.wSta )
		{
			strText.Format("%s:%d",ID2GAMEWORD( "SKILL_ADVANCED_INFO_STATS", 5), sLVL.sSTATS.wSta);
			INFO_DISPLAY_SKILL_AddTextNoSplit(strText, NS_UITEXTCONTROL::GetEvaluateColor ( pCharacter->m_sSUMSTATS.wPow >= sLVL.sSTATS.wSta ) );
		}
	}

	if ( bNextLevel ) return ;

	//	°£´ÜÇÑ ¼³¸í
	if( !pSkill->GetDesc() )	return;

	int StrLength = static_cast<int>( strlen( pSkill->GetDesc() ) );
	if ( StrLength )
	{
		INFO_DISPLAY_SKILL_AddTextNoSplit ( ID2GAMEWORD ( "SKILL_CATEGORY", 8 ), NS_UITEXTCOLOR::LIGHTSKYBLUE );

		INFO_DISPLAY_SKILL_AddTextLongestLineSplit ( pSkill->GetDesc(), NS_UITEXTCOLOR::DEFAULT );
	}
}

void CInnerInterface::ShowSkillTooltip( const SNATIVEID& sNativeID, const BOOL bNextLevel, const WORD wAddTempLevel /*= 0*/ )
{
    if ( sNativeID != NATIVEID_NULL() )
    {
        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sNativeID.wMainID, sNativeID.wSubID );
        if ( !pSkill )
            return;
        const bool bMultiSkill = pSkill->m_sEXT_DATA.idMultiTargetSkill != NATIVEID_NULL();
		bool bLinkSkill = FALSE;
		bool bDelayActionSkill = FALSE;
		SNATIVEID sDelayActionSkillID;

		SKILL::SAPPLY sApply = pSkill->m_sAPPLY;
		for ( DWORD i = 0; i < sApply.vecSPEC.size(); ++i )
		{
			if( sApply.vecSPEC[i].emSPEC == EMSPECA_SKILL_LINK )
			{
				bLinkSkill = TRUE;
			}

			if( sApply.vecSPEC[i].emSPEC == EMSPECA_DELAY_ACTION_SKILL )
			{
				bDelayActionSkill = TRUE;
				WORD wMid;
				WORD wSid;
				wMid = static_cast<WORD>(sApply.vecSPEC[i].sSPEC[ 0 ].fVAR1);
				wSid = static_cast<WORD>(sApply.vecSPEC[i].sSPEC[ 0 ].fVAR2);
				sDelayActionSkillID.SetMid(wMid);
				sDelayActionSkillID.SetSid(wSid);
			}
		}

        if( ( sNativeID != m_INFO_DISPLAY_SKILL_sNativeID_Previous)  ||
            ( wAddTempLevel != m_INFO_DISPLAY_SKILL_wLevel_Previous ) )
        {
            m_INFO_DISPLAY_SKILL_sNativeID_Previous = sNativeID;
            m_INFO_DISPLAY_SKILL_wLevel_Previous = wAddTempLevel;

            SCHARSKILL* pCharSkill = m_pGaeaClient->GetCharacter()->GETLEARNED_SKILL( pSkill->GetId() );
            WORD wShowLevel = wAddTempLevel;
            if ( pCharSkill )
            {
                wShowLevel += pCharSkill->wLevel + 1;
            }
            if ( bNextLevel )
                wShowLevel++;
			
			const bool bShowReq = ( bNextLevel || ( wShowLevel < pSkill->m_sBASIC.dwMAXLEVEL + 1 ) ); //+¹öÆ°¿¡ ¸¶¿ì½º ¿À¹ö »óÅÂÀÌ°Å³ª ¸¶½ºÅÍÇÑ ½ºÅ³ÀÌ ¾Æ´Ï¶ó¸é ½Àµæ¿ä±¸Á¶°ÇÀ» Ãâ·Â

			if ( !bMultiSkill && !bLinkSkill && !bDelayActionSkill )
			{
				m_pSkillTooltip->DisplaySkillInformation(
					m_pGaeaClient->GetCharacter(),
					pSkill,
					wShowLevel,
					CSkillInfoTooltip::BASIC_TOOLTIP,
					bShowReq );
			}
            else
			{
				m_pSkillTooltip->DisplaySkillInformation(
					m_pGaeaClient->GetCharacter(),
					pSkill,
					wShowLevel,
					CSkillInfoTooltip::MULTISKILL_MAIN_TOOLTIP,
					bShowReq );

				if ( bMultiSkill ) //¸ÖÆ¼ ½ºÅ³ÀÎ °æ¿ì;
				{
					PGLSKILL pSubSkill = GLSkillMan::GetInstance().GetData( pSkill->m_sEXT_DATA.idMultiTargetSkill.wMainID, pSkill->m_sEXT_DATA.idMultiTargetSkill.wSubID );
					m_pSkillTooltipSubSkill->DisplaySkillInformation(
						m_pGaeaClient->GetCharacter(),
						pSubSkill ,
						wShowLevel,
						CSkillInfoTooltip::MULTISKILL_SUB_TOOLTIP );
				}

				// ¸µÅ© ¹ßµ¿ ½ºÅ³;
				if( bLinkSkill )
				{
					m_pSkillTooltipLinkSkill->Reset();
					WORD wSkillLevel = max( wShowLevel - 1 , 0 );
					for ( DWORD i = 0; i < sApply.vecSPEC.size(); ++i )
					{
						if( sApply.vecSPEC[i].emSPEC != EMSPECA_SKILL_LINK )
							continue;

						if( m_pSkillTooltipLinkSkill->IsHaveSkill( sApply.vecSPEC[i].sSPEC[wSkillLevel].dwNativeID ) )
							continue;

						PGLSKILL pLinkSkill = GLSkillMan::GetInstance().GetData( sApply.vecSPEC[i].sSPEC[wSkillLevel].dwNativeID.wMainID, sApply.vecSPEC[i].sSPEC[wSkillLevel].dwNativeID.wSubID );
						if( !pLinkSkill )
							continue;

						m_pSkillTooltipLinkSkill->DisplaySkillInformation(
							m_pGaeaClient->GetCharacter(),
							pLinkSkill,
							wShowLevel,
							CSkillInfoTooltip::MULTISKILL_LINK_TOOLTIP );
					}
				}

				if ( bDelayActionSkill )
				{
					PGLSKILL pSubSkill = GLSkillMan::GetInstance().GetData( sDelayActionSkillID );
					m_pSkillTooltipDelayActionSkill->DisplaySkillInformation(
						m_pGaeaClient->GetCharacter(),
						pSubSkill ,
						wShowLevel,
						CSkillInfoTooltip::MULTISKILL_DELAY_ACTION_TOOLTIP );
				}

			}
        }
        
        UiShowGroupTop( SKILL_INFOR_TOOLTIP );
        if ( bMultiSkill )
        {
            UiShowGroupTop( SKILL_INFOR_TOOLTIP_MULTI );
        }else
		{
			UiHideGroup( SKILL_INFOR_TOOLTIP_MULTI );
		}

		if( bLinkSkill )
		{
			UiShowGroupTop( SKILL_INFOR_TOOLTIP_LINK );
		}else
		{
			UiHideGroup( SKILL_INFOR_TOOLTIP_LINK );
		}

		if( bDelayActionSkill )
		{
			UiShowGroupTop( SKILL_INFOR_TOOLTIP_DELAY_ACTION );
		}else
		{
			UiHideGroup( SKILL_INFOR_TOOLTIP_DELAY_ACTION );
		}

    }   
}

void CInnerInterface::INFO_DISPLAY_SKILL_LOAD ( const SNATIVEID& sNativeID, const BOOL bNextLevel )
{
	if ( sNativeID != NATIVEID_NULL() )
	{
		BOOL bUPDATE = FALSE;
		if (( bNextLevel != m_INFO_DISPLAY_SKILL_bNextLevel_Previous ) ||
			( sNativeID != m_INFO_DISPLAY_SKILL_sNativeID_Previous) )
		{			
			bUPDATE = TRUE;
		}

		if ( bUPDATE )
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID.wMainID, sNativeID.wSubID );
			if ( pSkill )	//	½ºÅ³ÀÌ Á¸ÀçÇÒ °æ¿ì
			{
				INFO_DISPLAY_SKILL_RemoveAllInfo ();

				SCHARSKILL* pCharSkill = m_pGaeaClient->GetCharacter()->GETLEARNED_SKILL ( sNativeID );					
				INFO_DISPLAY_SKILL_LOAD ( pSkill, pCharSkill, bNextLevel );
			}

			m_INFO_DISPLAY_SKILL_sNativeID_Previous = sNativeID;
			m_INFO_DISPLAY_SKILL_bNextLevel_Previous = bNextLevel;
		}		
	}
}

void CInnerInterface::ShowSkillToolTipMulti(const SNATIVEID& sNativeID, WORD wLevel)
{
	if( sNativeID == NATIVEID_NULL() || !m_pSkillTooltipSubSkill ) return;

	PGLSKILL pSubSkill = GLSkillMan::GetInstance().GetData( sNativeID.wMainID, sNativeID.wSubID );
	m_pSkillTooltipSubSkill->DisplaySkillInformation(
		m_pGaeaClient->GetCharacter(),
		pSubSkill ,
		wLevel,
		CSkillInfoTooltip::MULTISKILL_SUB_TOOLTIP, false, false );	
}