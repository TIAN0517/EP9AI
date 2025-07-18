#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"

#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../GLGaeaClient.h"
#include "../Effect/GLFactEffect.h"
#include "../Char/GLCharClient.h"
#include "../Land/GLLandManClient.h"
#include "./GLPetClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLPetClient::MsgGetRightOfItem(NET_MSG_GENERIC* nmg)
{
/*
	GLMSG::SNETPET_REQ_GETRIGHTOFITEM_FB* pNetMsg = ( GLMSG::SNETPET_REQ_GETRIGHTOFITEM_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_GETRIGHTOFITEM_FB_FAIL:
		break;

	case EMPET_REQ_GETRIGHTOFITEM_FB_OK:
		{
			m_vecDroppedItems.clear();
			for ( WORD i = 0; i < pNetMsg->wSum; ++i )
			{
				m_vecDroppedItems.push_back ( pNetMsg->DroppedItems[i] );
			}
		}
		break;
	};

//	m_ReqDropItems = false;
*/
}

void GLPetClient::MsgSkillChange(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_ADD_SKILLFACT* pNetMsg = ( GLMSG::SNETPET_ADD_SKILLFACT* ) nmg;

	for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
	{
		GLMSG::SNETPET_ADD_SKILLFACT::SPETSKILLFACT& sSKILLFACT = pNetMsg->sPETSKILLFACT[i];
		
		if ( sSKILLFACT.sNATIVEID == NATIVEID_NULL() ) continue;

		m_pOwner->m_sPETSKILLFACT[i].sNATIVEID	= sSKILLFACT.sNATIVEID;
		m_pOwner->m_sPETSKILLFACT[i].emTYPE		= sSKILLFACT.emTYPE;
		m_pOwner->m_sPETSKILLFACT[i].fMVAR		= sSKILLFACT.fMVAR;
		m_pOwner->m_sPETSKILLFACT[i].fAGE		= sSKILLFACT.fAge;

		// 이전 이펙트 제거
		FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_pOwner->GetGaeaID(),m_pOwner->GetPosition() ), m_pOwner->GetSkinChar (), m_sActiveSkillID[i] );

		m_sActiveSkillID[i] = sSKILLFACT.sNATIVEID;

		//	지속형 스킬의 경우 스킬 팩터 추가됨.
		if ( m_sActiveSkillID[i] != SNATIVEID(false) )
		{
			// 주인이 타겟이된다.
			STARGETID sTargetID( CROW_PC, m_pOwner->GetGaeaID(), m_pOwner->GetPosition () );
			FACTEFF::NewSkillFactEffect ( sTargetID, m_pOwner->GetSkinChar (), m_sActiveSkillID[i], m_matTrans, sTargetID.vPos );
		}
	}

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );

	// 스킬콤보박스 리프레쉬
	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );
}

void GLPetClient::MsgLearnSkill(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_LEARNSKILL_FB* pNetMsg = ( GLMSG::SNETPET_REQ_LEARNSKILL_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_LEARNSKILL_FB_OK:
		{
			SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( !pInvenItem )	return;

			SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
			if ( !pItem )		return;

			// 스킬습득
			LEARN_SKILL ( pItem->sSkillBookOp.sSkill_ID );

			// 팻카드의 정보도 갱신해준다.
			PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find ( m_DbNum );
			if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
			{
				SPETCARDINFO& sPet = (*iter).second;
				PETSKILL sPetSkill( pItem->sSkillBookOp.sSkill_ID, 0 );
				sPet.m_ExpSkills.insert ( std::make_pair(sPetSkill.sNativeID.dwID,sPetSkill) );
			}

			// 스킬 배움 이팩트 추가
			STARGETID sTargetID(CROW_PET, m_dwGUID, m_vPos);
			DxEffGroupPlayer::GetInstance().NewEffGroup
			(
				GLCONST_CHAR::strSKILL_LEARN_EFFECT.c_str(),
				m_matTrans,
				&sTargetID,
				FALSE,
				FALSE
			);

			// 스킬콤보박스 리프레쉬
			//GLWidgetScript::GetInstance().LuaCallFunc< void >(
			//	NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );

			m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::PALEGREEN,
                sc::string::format(ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_OK"), m_szName));
		}
		break;
	case EMPET_REQ_LEARNSKILL_FB_FAIL:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_FAIL") );
		break;
	case EMPET_REQ_LEARNSKILL_FB_NOITEM:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_NOITEM") );
		break;
	case EMPET_REQ_LEARNSKILL_FB_BADITEM:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_BADITEM") );
		break;
	case EMPET_REQ_LEARNSKILL_FB_ALREADY:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_LEARNSKILL_FB_ALREADY") );
		break;
	};

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );
}

void GLPetClient::MsgSkillSlotExpansion(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_SKILLSLOT_EXPANSION_FB* pNetMsg = ( GLMSG::SNETPET_REQ_SKILLSLOT_EXPANSION_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_SKILLSLOT_EXPANSION_FB_OK:
		{
			SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindPosItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( !pInvenItem )	return;

			SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
			if ( !pItem )		return;

			// Note : 펫 스킬 슬롯 확장
			m_wMaxSkillSlot = pNetMsg->wMaxSlot;

			if ( MAX_PETSKILLSLOT < m_wMaxSkillSlot )
			{
				m_wMaxSkillSlot = MAX_PETSKILLSLOT;
			}

			// Note : 펫카드 정보 갱신
			PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find ( m_DbNum );
			if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
			{
				SPETCARDINFO& sPet = (*iter).second;
				sPet.m_wMaxSkillSlot = m_wMaxSkillSlot;
			}

			// Note : 스킬 확장 이팩트 추가
			STARGETID sTargetID(CROW_PET, m_dwGUID, m_vPos);
			DxEffGroupPlayer::GetInstance().NewEffGroup
				(
				GLCONST_CHAR::strSKILL_LEARN_EFFECT.c_str(),
				m_matTrans,
				&sTargetID,
				FALSE,
				FALSE
				);

			// Note : 스킬콤보박스 리프레쉬
			//GLWidgetScript::GetInstance().LuaCallFunc< void >(
			//	NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );

			m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::PALEGREEN,
                sc::string::format(ID2GAMEINTEXT("EMPET_REQ_SKILLSLOT_EXPANSION_FB_OK", pNetMsg->emType), m_szName));
		}
		break;
	case EMPET_REQ_SKILLSLOT_EXPANSION_FB_FAIL:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SKILLSLOT_EXPANSION_FB_FAIL", pNetMsg->emType) );
		break;
	case EMPET_REQ_SKILLSLOT_EXPANSION_FB_NOITEM:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SKILLSLOT_EXPANSION_FB_NOITEM", pNetMsg->emType) );
		break;
	case EMPET_REQ_SKILLSLOT_EXPANSION_FB_BADITEM:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SKILLSLOT_EXPANSION_FB_BADITEM", pNetMsg->emType) );
		break;
	case EMPET_REQ_SKILLSLOT_EXPANSION_FB_ALREADY:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SKILLSLOT_EXPANSION_FB_ALREADY", pNetMsg->emType) );
		break;

	case EMPET_REQ_SKILLSLOT_EXPANSION_FB_MAX:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SKILLSLOT_EXPANSION_FB_MAX", pNetMsg->emType) );
		break;
	};

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );
}

void GLPetClient::MsgUpdateFull(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_UPDATE_CLIENT_PETFULL* pNetMsg = ( GLMSG::SNETPET_UPDATE_CLIENT_PETFULL* ) nmg;
	m_nFull = pNetMsg->nFull;

	// 팻카드의 정보도 갱신해준다.
	PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find ( m_DbNum );
	if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
	{
		SPETCARDINFO& sPet = (*iter).second;
		sPet.m_nFull = pNetMsg->nFull;

		//GLWidgetScript::GetInstance().LuaCallFunc< void >(
		//	NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );
	}

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );
}


void GLPetClient::MsgRename(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_RENAME_FB* pNetMsg = ( GLMSG::SNETPET_REQ_RENAME_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_RENAME_FB_FB_OK:
		{		
			std::string strPetName = pNetMsg->szName;
			//if ( m_pGaeaClient->GetServiceProvider() == SP_GS )
			//	strPetName = sc::string::utf8_to_ansi( strPetName );

			m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::PALEGREEN,
                sc::string::format(ID2GAMEINTEXT("EMPET_REQ_RENAME_FB_FB_OK"), m_szName, strPetName));
			SetName(strPetName);

			// 팻카드의 정보도 갱신해준다.
			PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find ( m_DbNum );
			if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
			{
				SPETCARDINFO& sPet = (*iter).second;
				StringCchCopy ( sPet.m_szName, PETNAMESIZE+1, pNetMsg->szName );
			}
		}
		break;

	case EMPET_REQ_RENAME_FB_FB_FAIL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_FAIL") );
		break;

	case EMPET_REQ_RENAME_FB_FB_NOITEM:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_NOITEM") );
		break;

	case EMPET_REQ_RENAME_FB_FB_BADITEM:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_BADITEM") );
		break;

	case EMPET_REQ_RENAME_FB_FB_LENGTH:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("NEWCHAR_NAME_TOO_SHORT") );	
		break;

	case EMPET_REQ_RENAME_FB_BAD_NAME:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("CHARACTER_BADNAME") );
		break;

	case EMPET_REQ_RENAME_FB_FROM_DB_FAIL:
		{			
			m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::NEGATIVE,
                sc::string::format(ID2GAMEINTEXT("EMINVEN_RENAME_FROM_DB_FAIL"), pNetMsg->szName));
		}
		break;
	case EMPET_REQ_RENAME_FB_FB_THAICHAR_ERROR:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_THAICHAR_ERROR"));
		break;
	case EMPET_REQ_RENAME_FB_FB_VNCHAR_ERROR:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_VNCHAR_ERROR"));
		break;
	};

//	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );
}

void GLPetClient::MsgChangeColor(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_CHANGE_COLOR_FB* pNetMsg = ( GLMSG::SNETPET_REQ_CHANGE_COLOR_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_COLOR_CHANGE_FB_OK:
		{
			m_wColor = pNetMsg->wColor;
			m_wColorTEMP = m_wColor;
			UpdateSuit ( FALSE );

			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMPET_REQ_COLOR_CHANGE_FB_OK") );
		}
		break;

	case EMPET_REQ_STYLECOLOR_CHANGE_FB_FAIL:
		ColorChange( m_wColor );
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_FAIL") );
		break;

	case EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM:
		ColorChange( m_wColor );
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM") );
		break;

	case EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM:
		ColorChange( m_wColor );
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM") );
		break;
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_UNCHANGEABLE:
		ColorChange( m_wColor );
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_UNCHANGEABLE") );
		break;
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_COOLTIME:		
		ColorChange( m_wColor );
		break;
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK:
		ColorChange( m_wColor );
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK") );
		break;
	};

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );
}

void GLPetClient::MsgChangeStyle(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_CHANGE_STYLE_FB* pNetMsg = ( GLMSG::SNETPET_REQ_CHANGE_STYLE_FB* ) nmg;

	switch( pNetMsg->emFB )
	{
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_FAIL :			// 일반오류.
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM :		// 아이템이 없음
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM :		// 사용 불가능 아이템
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_UNCHANGEABLE :	// 컬러변경이 불가능한 팻
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_COOLTIME :		// 컬러변경이 불가능한 팻
	case EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK :		// 스킨팩 사용중
		{
			StyleChange( m_wStyle );
			ColorChange( m_wColor );
		}
		break;

	case EMPET_REQ_STYLE_CHANGE_FB_OK :		// 성공
	case EMPET_REQ_COLOR_CHANGE_FB_OK :		// 성공
		{
			m_wStyle = pNetMsg->wStyle;
			m_wColor = pNetMsg->wColor;

			StyleChange( m_wStyle );
			ColorChange( m_wColor );

			UpdateSuit( FALSE );
		}
		break;
	}

	StyleEnd();
	ColorEnd();

	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_StylePetError, "-n, -n, -n",
	//	(int) pNetMsg->emFB, pNetMsg->wStyle, pNetMsg->wColor );

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );
}

void GLPetClient::MsgSlotExHold(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_FB* pNetMsg = ( GLMSG::SNETPET_REQ_SLOT_EX_HOLD_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_SLOT_EX_HOLD_FB_FAIL:

		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_OK:
		{
			// 손에든 아이템을 팻에게 장착시키고
			SetSlotItem( pNetMsg->emSuit, pNetMsg->sHoldtoSlotItemCustom );

			// 팻카드의 정보도 갱신해준다.
			PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find( m_DbNum );
			if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
			{
				SPETCARDINFO& sPet = (*iter).second;
				sPet.m_PutOnItems[(WORD)pNetMsg->emSuit-(WORD)SUIT_PET_A] = pNetMsg->sHoldtoSlotItemCustom;
			}

			UpdateSuit( FALSE );
		}
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH:
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
		break;
	};

//	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI);
}

void GLPetClient::MsgHoldToSlot(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_HOLD_TO_SLOT_FB* pNetMsg = ( GLMSG::SNETPET_REQ_HOLD_TO_SLOT_FB* ) nmg;

	switch ( pNetMsg->emFB )
	{
	case EMPET_REQ_SLOT_EX_HOLD_FB_FAIL:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_FAIL") );
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_OK:
		{
			// 손에든 아이템을 팻에게 장착시키고
			SetSlotItem( pNetMsg->emSuit, pNetMsg->sItemCustom );

			// 팻카드의 정보도 갱신해준다.
			PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find( m_DbNum );
			if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
			{
				SPETCARDINFO& sPet = (*iter).second;
				sPet.m_PutOnItems[(WORD)pNetMsg->emSuit-(WORD)SUIT_PET_A] = pNetMsg->sItemCustom;	
			}

			UpdateSuit( FALSE );
		}
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH") );
		break;

	case EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
		m_pGaeaClient->PrintMsgText ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM") );
		break;
	};

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI);
}

void GLPetClient::MsgSlotToHold(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_SLOT_TO_HOLD_FB* pNetMsg = ( GLMSG::SNETPET_REQ_SLOT_TO_HOLD_FB* ) nmg;

    // TODO : Hold Item 없애기.

	//switch ( pNetMsg->emFB )
	//{
	//case EMPET_REQ_SLOT_EX_HOLD_FB_FAIL:
	//	m_pGaeaClient->PrintMsgText ( 
	//		NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_FAIL") );
	//	break;

	//case EMPET_REQ_SLOT_EX_HOLD_FB_OK:
	//	{
	//		// 장착된 아이템 BackUp
	//		SITEMCUSTOM sSlotItemCustom = GetSlotitembySuittype( pNetMsg->emSuit );

	//		// 장착된 아이템 제거
	//		ReSetSlotItem( pNetMsg->emSuit );

	//		// 팻카드의 정보도 갱신해준다.
	//		PETCARDINFO_MAP_ITER iter = m_pOwner->m_mapPETCardInfo.find( m_DbNum );
	//		if ( iter!=m_pOwner->m_mapPETCardInfo.end() )
	//		{
	//			SPETCARDINFO& sPet = (*iter).second;
	//			WORD i = (WORD)pNetMsg->emSuit-(WORD)SUIT_PET_A;
	//			if ( i < ACCETYPESIZE )
 //                   sPet.m_PutOnItems[i] = SITEMCUSTOM( NATIVEID_NULL() );
	//		}

	//		// 장착됐던 아이템을 손에 든다
	//		m_pOwner->HOLD_ITEM( sSlotItemCustom );

 //           SkinLoad( m_pd3dDevice );
	//		UpdateSuit( TRUE );
	//	}
	//	break;

	//case EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH:
	//	m_pGaeaClient->PrintMsgText ( 
	//		NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH") );
	//	break;

	//case EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
	//	m_pGaeaClient->PrintMsgText ( 
	//		NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM") );
	//	break;
	//};
}

void GLPetClient::MsgRemoveSlotItem(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_FB* pNetMsg = ( GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_FB* ) nmg;
	
	WORD wPosX(0), wPosY(0);

	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype( pNetMsg->emSuit );

	SITEM* pSlotItem = GLogicData::GetInstance().GetItem( sSlotItemCustom.GetNativeID() );
	if ( !pSlotItem ) 
	{
		// 일반 오류
		return;
	}

	//BOOL bOk = m_pOwner->m_cInventory.FindInsertable( wPosX, wPosY );
	//if ( !bOk )
	//{
	//	//	인밴이 가득찻음.
	//	return;
	//}

	//// 인벤에 넣기
	//m_pOwner->m_cInventory.InsertItem( sSlotItemCustom, wPosX, wPosY );

	// 슬롯아이템 제거
	ReSetSlotItem( pNetMsg->emSuit );

    SkinLoad( m_pd3dDevice );
	UpdateSuit( TRUE );

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI);	
}

void GLPetClient::MsgAttack(NET_MSG_GENERIC* nmg)
{
	// 이동중이면 이동을 마친후 공격 모션을 취한다.
	if ( m_actorMove.PathIsActive () ) return;

	GLMSG::SNETPET_ATTACK* pNetMsg = ( GLMSG::SNETPET_ATTACK* ) nmg;

	D3DXVECTOR3 vTargetPos;

	if ( pNetMsg->sTarID.emCrow == CROW_PC )
	{
		GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
		std::tr1::shared_ptr<GLCharClient> pChar = pLandMan->GetChar ( pNetMsg->sTarID.GaeaId );
		if( NULL == pChar )
			return;

		vTargetPos = pChar->GetPosition();			
	}
	else if ( pNetMsg->sTarID.emCrow == CROW_MOB )
	{
		GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
		std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(pNetMsg->sTarID.GaeaId);
		if( NULL == spCrow )
			return;

		vTargetPos = spCrow->GetPosition();
	}
	else if ( pNetMsg->sTarID.emCrow == CROW_SUMMON )
	{
		GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
		std::tr1::shared_ptr<GLSummonClient> spSummon = pLandMan->GetSummon(pNetMsg->sTarID.GaeaId);
		if( NULL == spSummon )
			return;

		vTargetPos = spSummon->GetPosition();
	}

	SetAttackState( vTargetPos );
}

void GLPetClient::MsgSad(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_SAD* pPetNetMsg = (GLMSG::SNETPET_SAD*)nmg;
	ReSetAllSTATE ();

	// 플레이어가 죽어서 전송된 메세지다;
	if ( pPetNetMsg->bReset == false)
	{
		SetSTATE ( EM_PETACT_SAD );

		// SAD 애니메이션
		if( IsUsePetSkinPack() && m_sPetSkinPackState.bNotFoundSad )
		{
			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		}else{
			m_pSkinChar->SELECTANI ( AN_GESTURE, AN_SUB_04 );
		}

		// 방향 전환
		D3DXVECTOR3 vDirection = m_pOwner->GetPosition () - m_vPos;
		D3DXVec3Normalize ( &m_vDir, &vDirection );
		m_vDir = vDirection;
	}
	// 플레이어가 부활하여 전송된 메세지다;
	else
	{
		ReSetSTATE(EM_PETACT_SAD);
		SetSTATE (EM_PETACT_STOP);
		m_actorMove.Stop ();
		m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
	}
}

void GLPetClient::MsgAccessoryDelete(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_ACCESSORY_DELETE* pNetMsg = (GLMSG::SNETPET_ACCESSORY_DELETE*) nmg;
	m_PutOnItems[pNetMsg->accetype] = SITEMCUSTOM();

    SkinLoad( m_pd3dDevice );
	UpdateSuit( TRUE );
}



void GLPetClient::MsgRemoveSkillFact(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REMOVE_SKILLFACT* pNetMsg = (GLMSG::SNETPET_REMOVE_SKILLFACT*) nmg;

	WORD wSLOT = pNetMsg->wSlot;

	if ( MAX_PETSKILLSLOT <= wSLOT )
	{
		return;
	}

	// 이펙트 제거
	FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_pOwner->GetGaeaID(),m_pOwner->GetPosition() ), m_pOwner->GetSkinChar (), m_sActiveSkillID[wSLOT] );

	m_pOwner->m_sPETSKILLFACT[wSLOT].RESET ();
	m_sActiveSkillID[wSLOT] = NATIVEID_NULL();

	// 스킬콤보박스 리프레쉬
	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );
}

void GLPetClient::MsgJumpPos(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_JUMP* pNetMsg = (GLMSG::SNETPET_REQ_JUMP*)nmg;

	m_vPos = pNetMsg->vJumpPos;

	m_pOwner = m_pGaeaClient->GetCharacter ();

	GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

	m_actorMove.Create ( pLandMan->GetNaviMesh(), m_vPos );
	m_actorMove.Stop ();

	ReSetSTATE ( EM_PETACT_MOVE );
	SetSTATE ( EM_PETACT_STOP );
	
}

void GLPetClient::MsgProcess(NET_MSG_GENERIC* nmg)
{
	switch (nmg->Type())
	{
	case NET_MSG_GCTRL_PET_JUMP_POS :
		MsgJumpPos(nmg);
		break;

	case NET_MSG_PET_GETRIGHTOFITEM_FB:
        MsgGetRightOfItem(nmg);
		break;

	case NET_MSG_PET_REQ_SKILLCHANGE_FB:
        MsgSkillChange(nmg);
		break;

	case NET_MSG_PET_REQ_RENAME_FB:
        MsgRename(nmg);
		break;

	case NET_MSG_PET_UPDATECLIENT_FULL:
        MsgUpdateFull(nmg);
		break;

	case NET_MSG_PET_REQ_CHANGE_COLOR_FB:
        MsgChangeColor(nmg);
		break;

	case NET_MSG_PET_REQ_CHANGE_STYLE_FB:
        MsgChangeStyle(nmg);
		break;

	case NET_MSG_PET_REQ_SLOT_EX_HOLD_FB:
        MsgSlotExHold(nmg);
		break;

	case NET_MSG_PET_REQ_HOLD_TO_SLOT_FB:
        MsgHoldToSlot(nmg);
		break;

	case NET_MSG_PET_REQ_SLOT_TO_HOLD_FB:
        MsgSlotToHold(nmg);
		break;

	case NET_MSG_PET_ADD_SKILLFACT:
        MsgSkillChange(nmg);
		break;

	case NET_MSG_PET_REMOVE_SKILLFACT:
        MsgRemoveSkillFact(nmg);
		break;

	case NET_MSG_PET_REQ_LEARNSKILL_FB:
        MsgLearnSkill(nmg);
		break;

	case NET_MSG_PET_REQ_SKILLSLOT_EXPANSION_FB:
        MsgSkillSlotExpansion(nmg);
		break;

	case NET_MSG_PET_REMOVE_SLOTITEM_FB:
        MsgRemoveSlotItem(nmg);
		break;

	case NET_MSG_PET_ATTACK:
        MsgAttack(nmg);
		break;

	case NET_MSG_PET_SAD:
        MsgSad(nmg);
		break;

	case NET_MSG_PET_ACCESSORY_DELETE:
        MsgAccessoryDelete(nmg);
		break;

	default:
        sc::writeLogWarn(
            sc::string::format(
                "GLPetField::MsgProcess unknown message type %1%", nmg->Type()));
	  	break;
	};
}
