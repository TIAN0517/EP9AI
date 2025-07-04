#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxSound/DxSoundLib.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"

#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/SNS/GLSNSData.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/TraceMsg.h"

#include "../Tournament/GLTournamentClient.h"
#include "../Party/GLPartyManagerClient.h"
#include "../Party/GLPartyClient.h"
#include "../Crow/GLCrowRenList.h"
#include "../SNS/GLSNSClient.h"
//#include "../NpcTalk/GLNpcTalkClient.h"
#include "../GLGaeaClient.h"
#include "../Land/GLLandManClient.h"
#include "./GLCharacter.h"
#include "../PVE/GLPVEClient.h"

#include "../../RanGfxUI/InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! 인벤토리 남은공간
DWORD GLCharacter::InvenAmountEmpty(bool bAllLine)
{
    return m_cInventory.GetAmountEmpty(bAllLine);
}

void GLCharacter::MsgInvenTLGrinding(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_INVEN_TL_GRINDING *pNetMsg  = (GLMSG::SNET_INVEN_TL_GRINDING *)pMsg;

	EMREQ_TEMP_ENCHANT_RESULT eResult = pNetMsg->eResult;

	switch(eResult)
	{
	case EMREQ_TEMP_ENCHANT_SUCCESS :
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("TEMP_ENCHANT_SUCCESS") );
		DxSoundLib::GetInstance()->PlaySound ( "GRINDING_SUCCEED" );
		break;
	case EMREQ_TEMP_ENCHANT_FAIL :
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEMP_ENCHANT_FAIL") );
		break;
	case EMREQ_TEMP_ENCHANT_NOT_ITEM :
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEMP_ENCHANT_NOT_ITEM") );
		break;
	case EMREQ_TEMP_ENCHANT_FIRETIME :
		{
			const SITEM* pItem = GLogicData::GetInstance().GetItem( pNetMsg->fireItemID );

			if( pItem ) {
				std::string strFormat = sc::string::format(ID2GAMEINTEXT("TEMP_ENCHANT_FIRETIME"), pItem->GetName() );

				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, strFormat.c_str() );
			}			
		}
		break;
	case EMREQ_TEMP_ENCHANT_NOT :
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEMP_ENCHANT_NOT") );
		break;
	case EMREQ_TEMP_ENCHANT_EXIST :
		{
			m_wInvenPosX1 = pNetMsg->wPosX;
			m_wInvenPosY1 = pNetMsg->wPosY;
			m_wInvenPosX2 = pNetMsg->wHoldPosX;
			m_wInvenPosY2 = pNetMsg->wHoldPosX;

			//!! SF_TODO
			m_pGaeaClient->GetInterface()->DoModal( ID2GAMEINTEXT("TEMP_ENCHANT_EXIST"), UI::MODAL_QUESTION, UI::OKCANCEL, UI::MODAL_TEMP_ENCHANT );			
			/*GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_TempEnchant,
				"-n, -n", m_wInvenPosX1, m_wInvenPosY1 );*/
		}
		break;
	case EMREQ_TEMP_ENCHANT_GRADE_NOT :
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEMP_GRADE_NOT") );
		break;
	}
}
void GLCharacter::MsgInvenGrindingFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_GRINDING_FB *pNetMsg = (GLMSG::SNET_INVEN_GRINDING_FB *)pMsg;

    DWORD dwGrindingResult = pNetMsg->dwGrindResult;

    switch ( pNetMsg->emGrindFB )
    {
    case EMGRINDING_FAIL:

        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMGRINDING_FAIL") );

        if ( pNetMsg->bTERMINATE )
        {
            m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMGRINDING_TERMINATE") );
            DxSoundLib::GetInstance()->PlaySound ( "GRINDING_BROKEN" );
        }
        else if ( pNetMsg->bRESET )
        {
            // 소방주 완화주 동시사용
            if ( (dwGrindingResult&EMGRINDINGRESULT_ANTIRESET) && (dwGrindingResult&EMGRINDINGRESULT_ANTIDISAPPEAR) )
            {
                m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMGRINDING_ANTIDISAPPER_AND_ANTIRESET") );

				//  [12/17/2015 gbgim];
				// 메세지 박스가 출력됨으로 들어올린 아이템은 내려놓자;
				holditem::ReleaseHoldItem();
				//m_pGaeaClient->PrintMessageBox( NSWIDGET_SCRIPT::SFMSG_BOX_TYPE_OK, ID2GAMEINTEXT("EMGRINDING_ANTIDISAPPER_AND_ANTIRESET"));
            }
            // 소방주
            else if ( dwGrindingResult&EMGRINDINGRESULT_ANTIDISAPPEAR )
            {
                m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMGRINDING_ANTIDISAPPEAR") );

				//  [12/17/2015 gbgim];
				// 메세지 박스가 출력됨으로 들어올린 아이템은 내려놓자;
				holditem::ReleaseHoldItem();
				//m_pGaeaClient->PrintMessageBox( NSWIDGET_SCRIPT::SFMSG_BOX_TYPE_OK, ID2GAMEINTEXT("EMGRINDING_ANTIDISAPPEAR"));
            }
            // 완화주
            else if ( dwGrindingResult&EMGRINDINGRESULT_ANTIRESET )
            {
                // Note : 감소 수치 반영
                int nDecGrade = int ( pNetMsg->cGradeCurrent ) - int ( pNetMsg->cGradePrev );

                std::string strText = sc::string::format( ID2GAMEINTEXT("EMGRINDING_ANTIRESET"),nDecGrade ).c_str();
                m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::PALEGREEN, strText );

				//  [12/17/2015 gbgim];
				// 메세지 박스가 출력됨으로 들어올린 아이템은 내려놓자;
				holditem::ReleaseHoldItem();
				//m_pGaeaClient->PrintMessageBox( NSWIDGET_SCRIPT::SFMSG_BOX_TYPE_OK, ID2GAMEINTEXT("EMGRINDING_ANTIRESET"));
            }
            else
            {
                m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMGRINDING_RESET") );
            }

            DxSoundLib::GetInstance()->PlaySound ( "GRINDING_RESET" );
        }
        else
        {
            DxSoundLib::GetInstance()->PlaySound ( "GRINDING_FAIL" );
        }
        break;

    case EMGRINDING_SUCCEED:
        {
			//GfxInterfaceBase::GetInstance()->UpdateHoldSlot();

			if ( pNetMsg->cGradePrev < pNetMsg->cGradeCurrent )
			{
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::LIGHTSKYBLUE, ID2GAMEINTEXT("EMGRINDING_SUCCEED") );
				DxSoundLib::GetInstance()->PlaySound ( "GRINDING_SUCCEED" );

				//
				//mjeon.sns
				//

				char buffer[32]={0,};

				_itoa_s( (int)pNetMsg->cGradeCurrent, buffer, 10);

				std::string strUpgrade(buffer);


				RANPARAM::SNSFACEBOOK_SET& sOptFaceBook = RANPARAM::sSNSFaceBook;
				RANPARAM::SNSTWITTER_SET&  sOptTwitter  = RANPARAM::sSNSTwitter;

				if ( sOptFaceBook.bGRINDING && GLCONST_SNS::dw_REGISTER_GRINDING_LEVEL <= pNetMsg->cGradeCurrent )
				{
					m_pGaeaClient->GetSNSClient()->FBWallPost( strUpgrade, EMSNS_ENCHANT_SUCCESS, EMSNSREQTYPE_AUTOREQ );
				}

				if ( sOptTwitter.bGRINDING && GLCONST_SNS::dw_REGISTER_GRINDING_LEVEL <= pNetMsg->cGradeCurrent )
				{
					std::string msg = strUpgrade + ID2GAMEINTEXT("EMSNS_DESC_ENCHANTED");

					m_pGaeaClient->GetSNSClient()->TTTweetMsg( msg, EMSNSREQTYPE_AUTOREQ );
				}
			}
			else if ( pNetMsg->cGradePrev == pNetMsg->cGradeCurrent )
			{
				std::string strFormat = sc::string::format(ID2GAMEINTEXT("EMGRINDING_SUCCEED_SAME_GRADE"), static_cast<UINT>(pNetMsg->cGradeCurrent) );
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::LIGHTSKYBLUE, strFormat.c_str() );
				DxSoundLib::GetInstance()->PlaySound ( "GRINDING_RESET" );
			}
			else
			{
				std::string strFormat = sc::string::format(ID2GAMEINTEXT("EMGRINDING_SUCCEED_DEC_GRADE"), static_cast<UINT>(pNetMsg->cGradePrev), static_cast<UINT>(pNetMsg->cGradeCurrent) );
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::LIGHTSKYBLUE, strFormat.c_str() );
				DxSoundLib::GetInstance()->PlaySound ( "GRINDING_FAIL" );
			}

        }
        break;
    };
}

void GLCharacter::MsgInvenPeriodExtensionFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_INVEN_PERIODEXTEND_FB* pNetMsg = static_cast<GLMSG::SNET_INVEN_PERIODEXTEND_FB*>(pMsg);

	switch ( pNetMsg->emResult )
	{
	case EMREQ_PERIODEX_FB_FAIL:
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_PERIODEX_FAIL") );
		}
		break;
	case EMREQ_PERIODEX_FB_FAIL_DUPLICATION:
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_PERIODEX_FAIL_DUPLICATION") );
		}
		break;
	case EMREQ_PERIODEX_FB_FAIL_LIMITCOUNT:
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_PERIODEX_FAIL_LIMITCOUNT") );
		}
		break;
	case EMREQ_PERIODEX_FB_CHECK_EXPIRATIONDATE:
		{
// 			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PeriodExReconfirmOpenMsgBox
// 				, "-n, -n, -n, -n", pNetMsg->wHoldPosX, pNetMsg->wHoldPosY, pNetMsg->wPosX, pNetMsg->wPosY );
		}
		break;
	case EMREQ_PERIODEX_FB_SUCCESS:
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("ITEM_PERIODEX_TOOLTIP", 0) );
		}
		break;
	case EMREQ_PERIODEX_FB_SUCCESS_BASE:
	case EMREQ_PERIODEX_FB_SUCCESS_COSTUME:
		{
			SINVENITEM* pInvenItem = m_cInventory.FindPosItem(pNetMsg->wPosX, pNetMsg->wPosY);
			if ( !pInvenItem )
				return;

			SITEM* pItem = GLogicData::GetInstance().GetItem( pNetMsg->emResult == EMREQ_PERIODEX_FB_SUCCESS_BASE ? pInvenItem->sItemCustom.GetNativeID() : pInvenItem->sItemCustom.nidDISGUISE );
			if ( pItem == NULL )
				return;

			if ( pNetMsg->wRemainCount == (WORD)-1 )
			{
				std::string strMsg = sc::string::format( ID2GAMEINTEXT("ITEM_PERIODEX_SUCCESS"), pItem->GetName() );
				m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, strMsg );
			}
			else
			{
				std::string strMsg = sc::string::format( ID2GAMEINTEXT("ITEM_PERIODEX_SUCCESS_LIMIT"), pItem->GetName(), pNetMsg->wRemainCount );
				m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, strMsg );
			}
		}
		break;
	}
}

HRESULT GLCharacter::MsgReqInvenItemSort( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNETPC_REQ_ITEM_SORT* pNetMsg = (GLMSG::SNETPC_REQ_ITEM_SORT*)nmg;

    SetEnableSendItemSortMsg();

    if ( ( pNetMsg->nMoveCount <= 0 && pNetMsg->bHasPile == false ) || pNetMsg->nMoveCount >= EInvenTotalSlot )
        return E_FAIL;

    GLInventory* pInvenOrigin = NULL;

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		pInvenOrigin = &m_cVietnamInventory;
	}
	else
	{
//#else
		pInvenOrigin = &m_cInventory;
	}
//#endif

    if ( !pInvenOrigin )
        return E_FAIL;

    GLInventory cInvenClone;
    cInvenClone.Assign( *pInvenOrigin );

    GLInventory cInvenTemp;
    cInvenTemp.SetAddLine( pInvenOrigin->GETAddLine(), pInvenOrigin->IsLimit() );
    cInvenTemp.SetState( pInvenOrigin->GetSizeX(), pInvenOrigin->GetSizeY() );

    SINVENITEM* pInvenItem = NULL;
    SITEM*  pItem = NULL;
    BOOL bInsertItemOK = FALSE;
    BOOL bDeleteItemOK = FALSE;

    for ( WORD i = 0; i < pNetMsg->nMoveCount; i++ )
    {
		SINVEN_MOVE_ITEM_SORT& sInvenSort = pNetMsg->sMoveItem[ i ];

		WORD nOriginX = sInvenSort.nOrigin % EInvenX;
		WORD nOriginY = sInvenSort.nOrigin / EInvenX;

        pInvenItem = cInvenClone.GetItem( nOriginX, nOriginY );
        if ( !pInvenItem )
            return E_FAIL;

        pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
        if ( !pItem )
            return E_FAIL;

		WORD nTargetX = sInvenSort.nTarget % EInvenX;
		WORD nTargetY = sInvenSort.nTarget / EInvenX;

        bInsertItemOK = cInvenTemp.InsertItem( pInvenItem->sItemCustom, nTargetX, nTargetY, true );
        if ( !bInsertItemOK )
            return E_FAIL;

        bDeleteItemOK = cInvenClone.DeleteItem( nOriginX,  nOriginY );
        if ( !bDeleteItemOK )
            return E_FAIL;
    }

    const GLInventory::CELL_MAP* pMapInvenItem = cInvenClone.GetItemList();
    if ( !pMapInvenItem->empty() )
    {
        GLInventory::CELL_MAP_CITER itr = pMapInvenItem->begin();
        GLInventory::CELL_MAP_CITER endItr = pMapInvenItem->end();

        GLInventory::CELL_KEY keyClone;

        for ( ; itr != endItr; itr++ )
        {
            keyClone = itr->first;

            pInvenItem = itr->second;
            if( !pInvenItem )
                return E_FAIL;

            bInsertItemOK = cInvenTemp.InsertItem(
				pInvenItem->sItemCustom,
                keyClone.first, keyClone.second, true );

			if ( !bInsertItemOK )
                return E_FAIL;
        }
    }

    if (pNetMsg->bHasPile)
    {
        cInvenTemp.OptimalPile();
    }

    pInvenOrigin->Assign( cInvenTemp );

	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_Inventory_InvenAllSlotUpdate );

    return S_OK;
}

HRESULT GLCharacter::MsgReqInvenItemSortStart( NET_MSG_GENERIC* nmg )
{
	return S_OK;
}

HRESULT GLCharacter::MsgReqInvenItemSortData( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_ITEM_SORT_DATA* pNetMsg = (GLMSG::SNETPC_REQ_ITEM_SORT_DATA*)nmg;

	if ( pNetMsg->nCurCount <= 0 || pNetMsg->nStartIdx < 0 || pNetMsg->nStartIdx >= EInvenTotalSlot )
		return E_FAIL;

	for( int i = 0; i < pNetMsg->nCurCount; i++ )
	{
		m_sSwapInventory[pNetMsg->nStartIdx + i] = pNetMsg->sMoveItem[i];
	}

	return S_OK;
}

HRESULT GLCharacter::MsgReqInvenItemSortEnd( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_ITEM_SORT_END* pNetMsg = (GLMSG::SNETPC_REQ_ITEM_SORT_END*)nmg;

    if ( (pNetMsg->nTotalCount <= 0 && pNetMsg->bHasPile == false ) || pNetMsg->nTotalCount >= EInvenTotalSlot )
        return E_FAIL;

	GLInventory* pInvenOrigin = NULL;

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		pInvenOrigin = &m_cVietnamInventory;
	}
	else
	{
//#else
		pInvenOrigin = &m_cInventory;
	}
//#endif
    if ( !pInvenOrigin )
        return E_FAIL;

    GLInventory cInvenClone;
    cInvenClone.Assign( *pInvenOrigin );

    GLInventory cInvenTemp;
    cInvenTemp.SetAddLine( pInvenOrigin->GETAddLine(), pInvenOrigin->IsLimit() );
    cInvenTemp.SetState( pInvenOrigin->GetSizeX(), pInvenOrigin->GetSizeY() );

    SINVENITEM* pInvenItem = NULL;
    SITEM*  pItem = NULL;
    BOOL bInsertItemOK = FALSE;
    BOOL bDeleteItemOK = FALSE;


	for ( WORD i = 0; i < pNetMsg->nTotalCount; i++ )
    {
		SINVEN_MOVE_ITEM_SORT& sInvenSort = m_sSwapInventory[ i ];

		WORD nOriginX = sInvenSort.nOrigin % EInvenX;
		WORD nOriginY = sInvenSort.nOrigin / EInvenX;

        pInvenItem = cInvenClone.GetItem( nOriginX, nOriginY );
        if ( !pInvenItem )
            return E_FAIL;

        pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
        if ( !pItem )
            return E_FAIL;

		WORD nTargetX = sInvenSort.nTarget % EInvenX;
		WORD nTargetY = sInvenSort.nTarget / EInvenX;

        bInsertItemOK = cInvenTemp.InsertItem( pInvenItem->sItemCustom, nTargetX, nTargetY, true );
        if ( !bInsertItemOK )
            return E_FAIL;

        bDeleteItemOK = cInvenClone.DeleteItem( nOriginX, nOriginY );
        if ( !bDeleteItemOK )
            return E_FAIL;
    }

    const GLInventory::CELL_MAP* pMapInvenItem = cInvenClone.GetItemList();
    if ( !pMapInvenItem->empty() )
    {
        GLInventory::CELL_MAP_CITER itr = pMapInvenItem->begin();
        GLInventory::CELL_MAP_CITER endItr = pMapInvenItem->end();

        for ( ; itr != endItr; itr++ )
        {
            const GLInventory::CELL_KEY& keyClone = itr->first;

            pInvenItem = itr->second;
            if( !pInvenItem )
                return E_FAIL;

            bInsertItemOK = cInvenTemp.InsertItem( pInvenItem->sItemCustom, keyClone.first, keyClone.second, true );

			if ( !bInsertItemOK )
                return E_FAIL;
        }
    }

    if (pNetMsg->bHasPile)
    {
        cInvenTemp.OptimalPile();
    }

    pInvenOrigin->Assign( cInvenTemp );

	ClearSwapInventory();
	SetEnableSendItemSortMsg();

	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_Inventory_InvenAllSlotUpdate );

	return S_OK;
}

void GLCharacter::MsgInvenBoxOpenFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_BOXOPEN_FB *pNetMsg = (GLMSG::SNET_INVEN_BOXOPEN_FB *)pMsg;
    switch ( pNetMsg->emFB )
    {
    case EMREQ_BOXOPEN_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_FAIL") );
        break;
    case EMREQ_BOXOPEN_FB_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_OK") );
        break;
    case EMREQ_BOXOPEN_FB_NOITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_NOITEM") );
        break;
    case EMREQ_BOXOPEN_FB_NOBOX:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_NOBOX") );
        break;
    case EMREQ_BOXOPEN_FB_EMPTY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_EMPTY") );
        break;
    case EMREQ_BOXOPEN_FB_NOTINVEN:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_NOTINVEN") );
        break;
    case EMREQ_BOXOPEN_FB_INVALIDITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_INVALIDITEM") );
        break;
    case EMREQ_BOXOPEN_FB_ERR_ACTIVITY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_ERR_ACTIVITY") );
        break;
    case EMREQ_BOXOPEN_FB_ERR_CONTRIBUTION:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_ERR_CONTRIBUTION") );
        break;
    };
}

void GLCharacter::MsgInvenRandomBoxOpenFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_RANDOMBOXOPEN_FB *pNetMsg = (GLMSG::SNET_INVEN_RANDOMBOXOPEN_FB *)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_RANDOMBOXOPEN_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_FAIL") );
        break;

    case EMINVEN_RANDOMBOXOPEN_FB_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_OK") );
        break;

    case EMINVEN_RANDOMBOXOPEN_FB_EMPTY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_EMPTY") );
        break;

    case EMINVEN_RANDOMBOXOPEN_FB_BADITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_BADITEM") );
        break;

    case EMINVEN_RANDOMBOXOPEN_FB_NOINVEN:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_NOINVEN") );
        break;

    case EMINVEN_RANDOMBOXOPEN_FB_MISS:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_MISS") );
        break;
    };
}

void GLCharacter::MsgInvenLockBoxFB(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_INVEN_LOCKBOX_FB* pNetMsg = (GLMSG::SNET_INVEN_LOCKBOX_FB*)pMsg;
	switch ( pNetMsg->emFB )
	{
	case EMINVEN_LOCKBOX_FB_UNLOCK_FAIL:	//	일반오류;
	case EMINVEN_LOCKBOX_FB_UNLOCK_OK:			//	성공;
	case EMINVEN_LOCKBOX_FB_UNLOCK_ALREADY:		//	이미 열려있음;
	case EMINVEN_LOCKBOX_FB_UNLOCK_INCORRECT:	//	해당 열쇠로 열 수 없음;
	case EMINVEN_LOCKBOX_FB_UNLOCK_KEYINSTANCE:	//	키가 소모품이 아니어서 오류;
	case EMINVEN_LOCKBOX_FB_OPEN_FAIL:			//	일반오류;
	case EMINVEN_LOCKBOX_FB_OPEN_NOINVEN:		//	인벤에 여유공간 없음;
	case EMINVEN_LOCKBOX_FB_OPEN_LOCK:			//  잠겨있음;
	case EMINVEN_LOCKBOX_FB_FAIL:
	case EMINVEN_LOCKBOX_FB_SHORT_KEY:			// 개수가 모지람;
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("EMINVEN_LOCKBOX_FB", pNetMsg->emFB) );
		break;
	case EMINVEN_LOCKBOX_FB_OPEN_OK:			//	성공;
		{
			std::string strText( sc::string::format(
				ID2GAMEWORD("EMINVEN_LOCKBOX_FB", EMINVEN_LOCKBOX_FB_OPEN_OK)
				, GLItemMan::GetInstance().GetItemName(pNetMsg->nidITEM).c_str() ));
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, strText );
		}
		break;
	};
}

void GLCharacter::MsgSelformBoxSelectOpenFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_INVEN_SELFORMBOX_SELECTOPEN_FB* pNetMsgFB = (GLMSG::SNET_INVEN_SELFORMBOX_SELECTOPEN_FB*)pMsg;
	switch ( pNetMsgFB->emFB )
	{
	case EMINVEN_SELFORMBOX_FB_OK:
	case EMINVEN_SELFORMBOX_FB_INVENNOSPACE:
	case EMINVEN_SELFORMBOX_FB_FAIL:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("SELFORMBOX_MSG", (int)pNetMsgFB->emFB));
		break;
	}

	m_pGaeaClient->GetInterface()->UiHideGroup( SELECTIVEFORMBOX_WINDOW );
}

void GLCharacter::MsgInvenDisjunctionFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_DISJUNCTION_FB *pNetMsg = (GLMSG::SNET_INVEN_DISJUNCTION_FB *)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_DISJUNCTION_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_FAIL") );
        break;

    case EMINVEN_DISJUNCTION_FB_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_OK") );		
        break;

    case EMINVEN_DISJUNCTION_FB_BADITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_BADITEM") );
        break;

    case EMINVEN_DISJUNCTION_FB_NOINVEN:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_NOINVEN") );
        break;

    case EMINVEN_DISJUNCTION_FB_NONEED:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_NONEED") );
        break;
    };
}

void GLCharacter::MsgInvenDisguiseFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_DISGUISE_FB *pNetMsg = (GLMSG::SNET_INVEN_DISGUISE_FB *)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREQ_DISGUISE_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_FAIL") );
        break;
    case EMREQ_DISGUISE_FB_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_OK") );
        break;
    case EMREQ_DISGUISE_FB_NOITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_NOITEM") );
        break;
    case EMREQ_DISGUISE_FB_NODISGUISE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_NODISGUISE") );
        break;
    case EMREQ_DISGUISE_FB_NOTSUIT:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_NOTSUIT") );
        break;
    case EMREQ_DISGUISE_FB_DEFSUIT:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_DEFSUIT") );
        break;
    case EMREQ_DISGUISE_FB_ALREADY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_ALREADY") );
		break;
	case EMREQ_DISGUISE_FB_PILED:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_PILED") );
        break;
	case EMREQ_DISGUISE_FB_ERR_ACTIVITY:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_ERR_ACTIVITY") );
        break;
	case EMREQ_DISGUISE_FB_ERR_CONTRIBUTION:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_ERR_CONTRIBUTION") );
		break;
	case EMREQ_DISGUISE_FB_ERR_CANNOT:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_ERR_CANNOT") );
		break;
    };
}

void GLCharacter::MsgInvenCleanserFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_CLEANSER_FB *pNetMsg = (GLMSG::SNET_INVEN_CLEANSER_FB *)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREQ_CLEANSER_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_FAIL") );
        break;
    case EMREQ_CLEANSER_FB_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_OK") );
		break;
    case EMREQ_CLEANSER_FB_NOITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_NOITEM") );
        break;
    case EMREQ_CLEANSER_FB_NOCLEANSER:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_NOCLEANSER") );
        break;
    case EMREQ_CLEANSER_FB_NONEED:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_NONEED") );
        break;
    };
}

void GLCharacter::MsgInvenBoxWrappingFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_BOX_WRAPPING_FB *pNetMsg = (GLMSG::SNET_INVEN_BOX_WRAPPING_FB *)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREQ_BOX_WRAPPING_FB_FAILED:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXWRAPPING_FB_FAIL") );
        break;
    case EMREQ_BOX_WRAPPING_FB_SUCCESS:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_BOXWRAPPING_FB_OK") );
        break;
    case EMREQ_BOX_WRAPPING_FB_NON_ALLOWED_ITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXWRAPPING_FB_NOTALLOWED") );
        break;
    };
}

void GLCharacter::MsgInvenBoxUnwrappingFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_BOX_UNWRAPPING_FB *pNetMsg = (GLMSG::SNET_INVEN_BOX_UNWRAPPING_FB *)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREQ_BOX_UNWRAPPING_FB_FAILED:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXUNWRAPPING_FB_FAIL") );
        break;
    case EMREQ_BOX_UNWRAPPING_FB_SUCCESS:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_BOXUNWRAPPING_FB_OK") );
        break;
    };
}

void GLCharacter::MsgInvenDelItemTimeLimit(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT *pNetMsg = (GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT *)pMsg;

    SITEM *pITEM = GLogicData::GetInstance().GetItem ( pNetMsg->nidITEM );
    if ( !pITEM )
        return;

    std::string strText = sc::string::format( ID2GAMEINTEXT("INVEN_DEL_ITEM_TIMELMT"), pITEM->GetName() ).c_str();
    m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, strText );
}


void GLCharacter::MsgInvenVietnamItemGetNumUpdate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_VIETNAM_ITEMGETNUM_UPDATE *pNetMsg = (GLMSG::SNETPC_INVEN_VIETNAM_ITEMGETNUM_UPDATE *) pMsg;

    m_dwVietnamInvenCount = pNetMsg->dwVietnamInvenCount;
}

void GLCharacter::MsgInvenVietnamItemGetFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_VIETNAM_ITEMGET_FB *pNetMsg = (GLMSG::SNETPC_INVEN_VIETNAM_ITEMGET_FB *) pMsg;

    m_dwVietnamInvenCount = pNetMsg->dwVietnamInvenCount;
    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("VIETNAM_USE_ITEMGET_ITEM") );
}

void GLCharacter::MsgInvenVietnamExpGetFb(NET_MSG_GENERIC* pMsg)
{
    m_lVNGainSysMoney = 0;
    //			m_lnMoney		  = 0;
    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("VIETNAM_USE_EXPGET_ITEM") );
}

void GLCharacter::MsgVietnamAllInitTime(NET_MSG_GENERIC* pMsg)
{
//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		GLMSG::SNETPC_VIETNAM_ALLINITTIME *pNetMsg = (GLMSG::SNETPC_VIETNAM_ALLINITTIME *)pMsg;
		m_dwVietnamGainType = GAINTYPE_MAX;
		m_sVietnamSystem.Init();
		m_sVietnamSystem.loginTime = pNetMsg->initTime;
		m_tLoginTime = m_sVietnamSystem.loginTime;
		m_pGaeaClient->SetCurrentTime( m_tLoginTime );
		m_sVietnamSystem.gameTime  = 0;
		//			m_pInterface->SET_VNGAINTYPE_GAUGE( 0, 300 );
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("VIETNAM_TIME_ALLINIT") );
	}
//#endif
}

void GLCharacter::MsgInvenResetSkstFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_RESET_SKST_FB *pNetMsg = (GLMSG::SNET_INVEN_RESET_SKST_FB *) pMsg;

	switch( pNetMsg->emFB )
	{
	case EMREQ_RESET_SKST_FB_OK :
	case EMREQ_RESET_SKST_FB_OK2:
		{
			if( TRUE == pNetMsg->bRESETALL )
				RESET_STATS_SKILL( NULL );
			else
				RESET_STATS_SKILL( pNetMsg->wResetStats );

			//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Character_ApplyStatPoint );
			//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Skill_UpdateList );
			//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_IQS_UpdateSlot );
			//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_SQS_UpdateSlot );

			m_pGaeaClient->GetInterface()->REFRESH_SKILLWINDOW();
		}
		break;
	}

	// 루아에 알림
	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_StatsReset_Result,
	//	"-n, -n, -n, -n",
	//	pNetMsg->emFB, pNetMsg->wSTATS_NUM, pNetMsg->wSKILL_NUM, pNetMsg->llGameMoney );

	// 스킬창을 업데이트 해야한다.
	if( GLUseFeatures::GetInstance().IsUsingRenewSkillWindow() )
	{
		//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Skill_UpdateList );
		m_pGaeaClient->GetInterface()->REFRESH_SKILLWINDOW();
	}
}


void GLCharacter::MsgInvenInvenLineFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_INVENLINE_FB *pNetMsg = (GLMSG::SNET_INVEN_INVENLINE_FB *) pMsg;
    switch ( pNetMsg->emFB )
    {
    case EMREQ_INVENLINE_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_FAIL") );
        break;

    case EMREQ_INVENLINE_FB_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_OK") );

        //	Note : 인벤 줄수 설정.
        m_wINVENLINE = pNetMsg->wINVENLINE;

        //	Note : 현제 활성화된 인벤 라인 설정.
        //
        m_cInventory.SetAddLine( GetOnINVENLINE(), true );

		//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Inventory_AddInvenLine );
        break;

    case EMREQ_INVENLINE_FB_NOITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_NOITEM") );
        break;

    case EMREQ_INVENLINE_FB_MAXLINE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_MAXLINE") );
        break;
    };
}

void GLCharacter::MsgInvenReModelOpenFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_REMODELOPEN_FB *pNetMsg = (GLMSG::SNET_INVEN_REMODELOPEN_FB *) pMsg;

    switch( pNetMsg->emFB )
    {
    case EMREQ_REMODELOPEN_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_REMODELOPEN_FB_FAIL") );
        break;

    case EMREQ_REMODELOPEN_FB_OK:
		{
			m_emTypeItemRemodelState = EMTYPE_ITEM_REMODEL_RANDOMOPTION;
			//m_bUseRemodelCard = true;
			//RebuildCard::GetInstance().OpenRebuildUI( m_emTypeItemRemodelState );
			m_pGaeaClient->GetInterface()->SetItemRebuildWindowCardOpen();
		}
        break;

    case EMREQ_REMODELOPEN_FB_NOITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_REMODELOPEN_FB_NOITEM") );
        break;
	case EMREQ_REMODELOPEN_FB_STAMP_ERROR:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_REMODELOPEN_FB_STAMP_ERROR") );
		break;
    };

	if( EMREQ_REMODELOPEN_FB_OK != pNetMsg->emFB )
	{
		//GLWidgetScript::GetInstance().LuaCallFunc< void >(
		//	NSWIDGET_SCRIPT::g_strFunc_RebuildCard_StampUncheck );
		m_pGaeaClient->GetInterface()->SetItemRebuildResetCheckBox();
	}
}

void GLCharacter::MsgInvenGarbageOpenFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_GARBAGEOPEN_FB *pNetMsg = (GLMSG::SNET_INVEN_GARBAGEOPEN_FB *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREQ_GARBAGEOPEN_FB_FAIL:
		m_ItemGarbagePosX = USHRT_MAX;
		m_ItemGarbagePosY = USHRT_MAX;
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GARBAGEOPEN_FB_FAIL") );
        break;

    case EMREQ_GARBAGEOPEN_FB_OK:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_GARBAGEOPEN_FB_OK") );
		//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_InvenTrash_Open );
		m_pGaeaClient->GetInterface()->OpenItemGarbageWindow();
        break;

    case EMREQ_GARBAGEOPEN_FB_NOITEM:
		m_ItemGarbagePosX = USHRT_MAX;
		m_ItemGarbagePosY = USHRT_MAX;
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GARBAGEOPEN_FB_NOITEM") );
        break;

	default:
		m_ItemGarbagePosX = USHRT_MAX;
		m_ItemGarbagePosY = USHRT_MAX;
		break;
    };
}

void GLCharacter::MsgInvenStorageOpenFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_STORAGEOPEN_FB *pNetMsg = (GLMSG::SNET_INVEN_STORAGEOPEN_FB *) pMsg;
    //SINVENITEM sItem( pNetMsg->wPosX, pNetMsg->wPosY );
    switch ( pNetMsg->emFB )
    {
    case EMREQ_STORAGEOPEN_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGEOPEN_FB_FAIL") );
        break;

    case EMREQ_STORAGEOPEN_FB_OK:
        m_bUseStorageCard = true;
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_STORAGEOPEN_FB_OK") );

		//GLNpcTalkClient::GetInstance().SetNpcID( SNATIVEID( false ) );
		//GLNpcTalkClient::GetInstance().SetNpcGlobalID( GAEAID_NULL );

		//GLWidgetScript::GetInstance().LuaCallFunc< void >(
		//	NSWIDGET_SCRIPT::g_strFunc_Locker_UseOpenCard );
		m_pGaeaClient->GetInterface()->SetStorageWindowCardOpen();

        break;

    case EMREQ_STORAGEOPEN_FB_NOITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGEOPEN_FB_NOITEM") );
        break;
    };
}

void GLCharacter::MsgInvenPMarketSearchOpenFb(NET_MSG_GENERIC* pMsg)
{
    const GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_FB *pNetMsg = (GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_FB *) pMsg;
    //SINVENITEM sItem( pNetMsg->wPosX, pNetMsg->wPosY );
    switch ( pNetMsg->emFB )
    {
    case EMREQ_PMARKET_SEARCH_OPEN_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("EMREQ_PMARKET_SEARCH_CARD_FB", EMREQ_PMARKET_SEARCH_OPEN_FB_FAIL) );
        break;

    case EMREQ_PMARKET_SEARCH_OPEN_FB_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEWORD("EMREQ_PMARKET_SEARCH_CARD_FB", EMREQ_PMARKET_SEARCH_OPEN_FB_OK) );
		//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_Open );
		m_pGaeaClient->GetInterface()->PrivateMarketWindowOpen(true);
        break;

    case EMREQ_PMARKET_SEARCH_OPEN_FB_NOITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("EMREQ_PMARKET_SEARCH_CARD_FB", EMREQ_PMARKET_SEARCH_OPEN_FB_NOITEM) );
        break;

    case EMREQ_PMARKET_SEARCH_OPEN_FB_ALREADY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("EMREQ_PMARKET_SEARCH_CARD_FB", EMREQ_PMARKET_SEARCH_OPEN_FB_ALREADY) );
		//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_Open );
		m_pGaeaClient->GetInterface()->PrivateMarketWindowOpen(true);
        break;

    case EMREQ_PMARKET_SEARCH_OPEN_FB_BLOCK:
        //클라에서 이미 개인상점 가능지여인지를 검사하고 보냈기때문에 여기로 들어오면 안됨
        m_pGaeaClient->PrivateMarketError( private_market::eNotPurchaseMap );
        break;
    };
}

void GLCharacter::MsgInvenPremiumSetFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_PREMIUMSET_FB *pNetMsg = (GLMSG::SNET_INVEN_PREMIUMSET_FB *) pMsg;
    switch ( pNetMsg->emFB )
    {
    case EMREQ_PREMIUMSET_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_PREMIUMSET_FB_FAIL") );
        break;

    case EMREQ_PREMIUMSET_FB_OK:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_PREMIUMSET_FB_OK") );

            CTime tLMT(pNetMsg->tLMT);

            //	서버 타임으로 변환( TimeZone 계산 )
            m_pGaeaClient->GetConvertServerTime( tLMT );
            CTimeSpan tSPAN(pNetMsg->tSPAN);

            std::string strText = sc::string::format( ID2GAMEINTEXT("PREMIUMSET_EX_STATE"), tLMT.GetYear(), tLMT.GetMonth(), 
                tLMT.GetDay(), tLMT.GetHour(), tSPAN.GetDays() ).c_str();

            m_pGaeaClient->PrintConsoleTextDlg ( strText );

            m_tPREMIUM = pNetMsg->tLMT;
            m_bPREMIUM = true;

            //	Note : 현제 활성화된 인벤 라인 설정.
            //
            m_cInventory.SetAddLine( GetOnINVENLINE(), true );

			//GLWidgetScript::GetInstance().LuaCallFunc< void >(
				//NSWIDGET_SCRIPT::g_strFunc_UpdatePremium );

        }
        break;

    case EMREQ_PREMIUMSET_FB_NOITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_PREMIUMSET_FB_NOITEM") );
        break;

    case EMREQ_PREMIUMSET_FB_NOTINVEN:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_PREMIUMSET_FB_NOTINVEN") );
        break;
    };
}


void GLCharacter::MsgInvenCharCardFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_CHARCARD_FB *pNetMsg = (GLMSG::SNET_INVEN_CHARCARD_FB *) pMsg;
    switch ( pNetMsg->emFB )
    {
    case EMREQ_CHARCARD_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CHARCARD_FB_FAIL") );
        break;

    case EMREQ_CHARCARD_FB_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_CHARCARD_FB_OK") );
        break;

    case EMREQ_CHARCARD_FB_NOITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CHARCARD_FB_NOITEM") );
        break;
    };
}

void GLCharacter::MsgInvenStorageCardFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_STORAGECARD_FB *pNetMsg = (GLMSG::SNET_INVEN_STORAGECARD_FB *) pMsg;
    switch ( pNetMsg->emFB )
    {
    case EMREQ_STORAGECARD_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGECARD_FB_FAIL") );
        break;

    case EMREQ_STORAGECARD_FB_OK:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_STORAGECARD_FB_OK") );

            CTime tLMT(pNetMsg->tSTORAGE_LIMIT);

            //	서버 타임으로 변환( TimeZone 계산 )
            m_pGaeaClient->GetConvertServerTime( tLMT );
            CTimeSpan tSPAN(pNetMsg->tSPAN);

            std::string strText = sc::string::format( ID2GAMEINTEXT("STORAGE_EX_STATE"), pNetMsg->wSTORAGE+1, tLMT.GetYear(),
                tLMT.GetMonth(), tLMT.GetDay(), tLMT.GetHour(),	tSPAN.GetDays() ).c_str();
            m_pGaeaClient->PrintConsoleTextDlg ( strText );

            int nINDEX = pNetMsg->wSTORAGE-EMSTORAGE_CHANNEL_SPAN;
            m_bSTORAGE[nINDEX] = true;
            m_tSTORAGE[nINDEX] = pNetMsg->tSTORAGE_LIMIT;

			//GLWidgetScript::GetInstance().LuaCallFunc< void >(
			//	NSWIDGET_SCRIPT::g_strFunc_Locker_Update );
        }
        break;

    case EMREQ_STORAGECARD_FB_NOITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGECARD_FB_NOITEM") );
        break;

    case EMREQ_STORAGECARD_FB_INVNUM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGECARD_FB_INVNUM") );
        break;
    };
}

void GLCharacter::MsgInvenItemMixFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_INVEN_ITEM_MIX_FC* pNetMsg = ( GLMSG::NET_INVEN_ITEM_MIX_FC* ) pMsg;
    std::string strMsg;

    bool bSucc = false;
    bool bFail = false;

    DWORD dwColor = NS_UITEXTCOLOR::DISABLE;

    switch( pNetMsg->emFB )
    {
    case EMITEM_MIX_FB_FAIL:
        {				
            strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_FAIL");
            bFail = true;
        }
        break;
    case EMITEM_MIX_FB_OK:
        {				
            SITEM* pItem = GLogicData::GetInstance().GetItem( pNetMsg->sNativeID );
            if ( pItem ) 
            {
                strMsg = sc::string::format( ID2GAMEINTEXT("EMITEM_MIX_FB_OK") , pItem->GetName(), pNetMsg->wTurnNum ).c_str();
                dwColor = NS_UITEXTCOLOR::PALEGREEN;
                bSucc = true;
            }					
        }				
        break;
    case EMITEM_MIX_FB_ERROR:
        {				
            strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_ERROR");
        }
        break;				
    case EMITEM_MIX_FB_NOMIX:
        strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NOMIX");
        break;
    case EMITEM_MIX_FB_NOMIXNUM:
        strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NOMIXNUM");
        break;
    case EMITEM_MIX_FB_NOITEM:
        strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NOITEM");
        break;
    case EMITEM_MIX_FB_NOMONEY:
        strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NOMONEY");
        break;
    case EMITEM_MIX_FB_NOINVEN:
        strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NOINVEN");
        break;
    case EMITEM_MIX_FB_NONPC:
        strMsg = ID2GAMEINTEXT("EMITEM_MIX_FB_NONPC");
        break;
    }

    m_pGaeaClient->PrintMsgText ( dwColor, strMsg );
	//!! SF_TODO
    //m_pInterface->SetItemMixResult(CString(strMsg.c_str()), bSucc, bFail );

    m_pGaeaClient->GetCharacter()->ResetItemMix();
}

void GLCharacter::MsgReqActionQFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_ACTIONQUICK_FB *pNetMsg = (GLMSG::SNETPC_REQ_ACTIONQUICK_FB *)pMsg;
    m_sACTIONQUICK[pNetMsg->wSLOT] = pNetMsg->sACT;

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_IQS_UpdateSlot );
}

void GLCharacter::MsgInvenItemUpdate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INVEN_ITEM_UPDATE *pNetMsg = (GLMSG::SNET_INVEN_ITEM_UPDATE *)pMsg;
    SINVENITEM* pInvenItem = NULL;
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if( pNetMsg->bVietnamInven )
			pInvenItem = m_cVietnamInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
        else
			pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	}
	else
	{
		pInvenItem = m_cInventory.GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
	}
    if ( !pInvenItem )
        return;

    pInvenItem->sItemCustom = pNetMsg->sItemCustom;

	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_Inventory_InvenSlotUpdate,
	//	"-n, -n", pNetMsg->wPosX, pNetMsg->wPosY);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );
}

void GLCharacter::MsgPushPullBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_PUSHPULL_BRD *pNetMsg = (GLMSG::SNET_PUSHPULL_BRD *)pMsg;
    const D3DXVECTOR3 &vMovePos = pNetMsg->vMovePos;

    //	Note : 밀려날 위치로 이동 시도.
    //	
    const BOOL bSucceed = ClientActor::GotoLocation(vMovePos, 5.0f);

    if ( bSucceed )
    {
        //	Note : 밀려나는 엑션 시작.
        //
        m_sActionID.vPos = vMovePos;
        m_dwANISUBSELECT = pNetMsg->dwAniType;

		TurnAction ( GLAT_PUSHPULL );

        //	Note : 밀리는 속도 설정.
        //
		ClientActor::SetMaxSpeed(pNetMsg->fPUSH_SPEED);
    }
}

void GLCharacter::MsgRegenGateFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_REGEN_GATE_FB *pNetMsg = (GLMSG::SNETPC_REQ_REGEN_GATE_FB *)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREGEN_GATE_FAIL:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREGEN_GATE_FAIL") );
        }
        break;

    case EMREGEN_GATE_SUCCEED:
        {
            // Note : 시작귀환카드에서 사용될 정보를 받습니다.
            m_sStartMapID = pNetMsg->sMapID;
            m_dwStartGate = pNetMsg->dwGateID;
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREGEN_GATE_SUCCEED") );
        }
        break;
    };
}

void GLCharacter::MsgCharResetFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_CHARRESET_FB *pNetMsg = (GLMSG::SNETPC_REQ_CHARRESET_FB *)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREGEN_CHARRESET_SUCCEED:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREGEN_CHARRESET_SUCCEED") );

            RESET_STATS_SKILL();

            m_wStatsPoint = (WORD) pNetMsg->dwSTATS_P;
            m_dwSkillPoint = pNetMsg->dwSKILL_P;

			m_sRunSkill = SNATIVEID(false);
			m_sActiveSkill = SNATIVEID(false);
			m_sScopeSkill = SKILLID_NULL;
            SetDefenseSkill( false );
            ResetReservationAction();

			//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_SQS_LastSkillID,
			//	"-dw", m_sRunSkill.dwID ); 

            //	Note : 만약 SKILL 사용중이라면 모두 리셋.
            //
            if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
            {
                TurnAction(GLAT_IDLE);
            }
        }
        break;

    case EMREGEN_CHARRESET_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREGEN_CHARRESET_FAIL") );
        break;

    case EMREGEN_CHARRESET_ITEM_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREGEN_CHARRESET_ITEM_FAIL") );
        break;
    };
}

void GLCharacter::MsgFireCrackerFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_FIRECRACKER_FB *pNetMsg = (GLMSG::SNETPC_REQ_FIRECRACKER_FB *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREQ_FIRECRACKER_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_FIRECRACKER_FB_FAIL") );
        break;

    case EMREQ_FIRECRACKER_FB_OK:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_FIRECRACKER_FB_OK") );
        break;

    case EMREQ_FIRECRACKER_FB_NOITEM:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_FIRECRACKER_FB_NOITEM") );
        break;
    };
}

void GLCharacter::MsgFireCrackerBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_FIRECRACKER_BRD *pNetMsg = (GLMSG::SNETPC_REQ_FIRECRACKER_BRD *) pMsg;

    SITEM *pITEM = GLogicData::GetInstance().GetItem ( pNetMsg->nidITEM );
    if ( !pITEM )
        return;

    D3DXMATRIX matEffect;
    D3DXMatrixTranslation ( &matEffect, pNetMsg->vPOS.x, pNetMsg->vPOS.y, pNetMsg->vPOS.z );

    //	보이지 않는 타갯일 경우 타격 이팩트는 생략됨.
    DxEffGroupPlayer::GetInstance().NewEffGroup
        (
        pITEM->GetTargetEffect(),
        matEffect,
        &STARGETID(CROW_PC,0,pNetMsg->vPOS),
		FALSE, 
		TRUE
        );
}

void GLCharacter::MsgGmMoveToGateFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_MOVE2GATE_FB *pNetMsg = (GLMSG::SNET_GM_MOVE2GATE_FB *) pMsg;
    ClientActor::InitializeActorMove ( pNetMsg->vPOS );

	if( pNetMsg->dwFlags & MOVE_OPTION_SHOW_EFF &&
		m_idIllusionSKILL.IsValidNativeID() )
	{
		PGLSKILL	pSkill = GLSkillMan::GetInstance().GetData( m_idIllusionSKILL.wMainID, m_idIllusionSKILL.wSubID );
		if( pSkill )
		{
			EMELEMENT				emELMT		= ( pSkill->m_sAPPLY.emELEMENT == EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
            const char*				szZONE_EFF	= pSkill->m_sEXT_DATA.GETTARGBODY01( emELMT );
            D3DXMATRIX				matTrans;
			D3DXMatrixIdentity( &matTrans );
            matTrans._41 = pNetMsg->vPOS.x;
            matTrans._42 = pNetMsg->vPOS.y;
            matTrans._43 = pNetMsg->vPOS.z;

			DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, &TARGETID_NULL, TRUE, TRUE );
		} //if
	} //if
    
	if( !(pNetMsg->dwFlags & MOVE_OPTION_NOT_TURN_IDLE) ) 
		TurnAction( GLAT_IDLE );
}

void GLCharacter::MsgCureFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_CURE_FB *pNetMsg = (GLMSG::SNETPC_REQ_CURE_FB *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREGEN_CURE_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREGEN_CURE_FAIL") );
        break;

    case EMREGEN_CURE_SUCCEED:
        m_sHP = pNetMsg->sHP;
        m_sMP = pNetMsg->sMP;
        m_sSP = pNetMsg->sSP;

		//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );

        for ( int i=0; i<EMBLOW_MULTI; ++i )
        {
            if ( m_sSTATEBLOWS[i].emBLOW==EMBLOW_NONE )		continue;

            EMDISORDER emDIS = STATE_TO_DISORDER(m_sSTATEBLOWS[i].emBLOW);
            if ( !(pNetMsg->dwCUREFLAG&emDIS) )				continue;

            m_sSTATEBLOWS[i].fAGE = 0.0f;
        }
        break;
    };
}

void GLCharacter::MsgReqGatheringResult(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_GATHERING_RESULT* pNetMsg = ( GLMSG::SNETPC_REQ_GATHERING_RESULT* ) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREQ_GATHER_RESULT_SUCCESS:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, 
                ID2GAMEINTEXT("EMREQ_GATHER_RESULT_SUCCESS") );

            m_dwANISUBGESTURE = 0;	//	성공
        }
        break;
    case EMREQ_GATHER_RESULT_SUCCESS_EX:
        {
            m_pGaeaClient->PrintMsgText 
                ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_RESULT_SUCCESS_EX") );	

            m_dwANISUBGESTURE = 0;	//	성공
        }
        break;
    case EMREQ_GATHER_RESULT_FAIL:
        {
            m_pGaeaClient->PrintMsgText 
                ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_RESULT_FAIL") );	

            m_dwANISUBGESTURE = 1;	//	실패
        }
        break;
    case EMREQ_GATHER_RESULT_ITEMFAIL:
        {
            m_pGaeaClient->PrintMsgText 
                ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_RESULT_ITEMFAIL") );					

            m_dwANISUBGESTURE = 1;	//	실패
        }
        break;
    }
}

void GLCharacter::MsgReqGatheringFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_GATHERING_FB* pNetMsg = ( GLMSG::SNETPC_REQ_GATHERING_FB* ) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREQ_GATHER_FB_OK:
        {					
            m_pGaeaClient->PrintMsgText 
                ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_GATHER_FB_OK") );

            m_dwANISUBGESTURE = pNetMsg->dwAniSel;
            TurnAction( GLAT_GATHERING );
        }
        break;
    case EMREQ_GATHER_FB_FAIL:
        {
            m_pGaeaClient->PrintMsgText 
                ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_FAIL") );
        }
        break;
    case EMREQ_GATHER_FB_DISTANCE:
        {
            m_pGaeaClient->PrintMsgText 
                ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_DISTANCE") );
        }
        break;
    case EMREQ_GATHER_FB_NOTTYPE:
        {
            m_pGaeaClient->PrintMsgText 
                ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_NOTTYPE") );
        }
        break;
    case EMREQ_GATHER_FB_NOCROW:
        {
            m_pGaeaClient->PrintMsgText 
                ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_NOCROW") );
        }
        break;
    case EMREQ_GATHER_FB_USE:
        {
            m_pGaeaClient->PrintMsgText 
                ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_USE") );
        }
        break;
    case EMREQ_GATHER_FB_GATHERING:
        {
            m_pGaeaClient->PrintMsgText 
                ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_GATHERING") );
        }
        break;
    }
}

void GLCharacter::MsgReqAttendRewardClt(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_ATTEND_REWARD_CLT* pNetMsg = ( GLMSG::SNETPC_REQ_ATTEND_REWARD_CLT* ) pMsg;

    SITEM* pItem = GLogicData::GetInstance().GetItem( pNetMsg->idAttendReward );			

    if ( pItem )
    {
        std::string strText = sc::string::format( ID2GAMEINTEXT("ATTEND_REWARD_ITEM"), pItem->GetName() ).c_str();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
    }
    else
    {
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ATTEND_REWARD_ITEM") );
    }
}

void GLCharacter::MsgReqAttendanceFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_ATTENDANCE_FB* pNetMsg = ( GLMSG::SNETPC_REQ_ATTENDANCE_FB* ) pMsg;

    switch( pNetMsg->emFB )
    {
    case EMREQ_ATTEND_FB_OK:
        {
            m_dwComboAttend = pNetMsg->dwComboAttend;
            USER_ATTEND_INFO sAttend;
            sAttend.tAttendTime = pNetMsg->tAttendTime;
            sAttend.bAttendReward = pNetMsg->bAttendReward;
            m_vecAttend.push_back( sAttend );

            std::string strText = sc::string::format( ID2GAMEINTEXT("EMREQ_ATTEND_FB_OK"), m_dwComboAttend ).c_str();
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
        }
        break;
    case EMREQ_ATTEND_FB_FAIL:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_ATTEND_FB_FAIL") );
        }
        break;
    case EMREQ_ATTEND_FB_ALREADY:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_ATTEND_FB_ALREADY") );
        }
        break;
    case EMREQ_ATTEND_FB_ATTENTIME:
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT("EMREQ_ATTEND_FB_ATTENTIME"),m_dwAttendTime ).c_str();
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
        }
        break;
    case EMREQ_ATTEND_FB_NOTUSE:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_ATTEND_FB_NOTUSE"));			
			//GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			//	NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Close );
        }
    }

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( 
	//	NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Attendance );
}

void GLCharacter::MsgReqAttendListFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_ATTENDLIST_FB* pNetMsg = ( GLMSG::SNETPC_REQ_ATTENDLIST_FB* ) pMsg;

    m_vecAttend.clear();

    for ( DWORD i = 0; i < pNetMsg->dwAttenNum; ++i )
    {
        m_vecAttend.push_back( pNetMsg->sAttend[i] );	
    }

    m_dwComboAttend = pNetMsg->dwAttendCombo;

    m_tAttendLogin = pNetMsg->tLoginTime;
    m_dwAttendTime = pNetMsg->dwAttendTime;
	
	//GLWidgetScript::GetInstance().LuaCallFunc< void >( 
	//	NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Attendance );
}

void GLCharacter::MsgUpdateTracingCharacter(NET_MSG_GENERIC* pMsg)
{
	GLMSG::NET_UPDATE_TRACINGCHAR* pNetMsg = ( GLMSG::NET_UPDATE_TRACINGCHAR* ) pMsg;
    if( pNetMsg->updateNum == 0 )
        m_bTracingUser = FALSE;
    else
        m_bTracingUser = TRUE;

	    m_bTracingUser = FALSE;
}

void GLCharacter::MsgQboxOptionMember(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_QBOX_OPTION_MEMBER* pNetMsg = ( GLMSG::SNET_QBOX_OPTION_MEMBER* ) pMsg;	
	GLPartyClient* const pParty = m_pGaeaClient->GetPartyManager()->GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return;

	pParty->m_bQBoxEnable = pNetMsg->bQBoxEnable;    

	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
	if ( pMyParty == NULL )
		return;

	// 원정대에 속한 상태에서 파티ID만을 비교 하는 경우 원정대ID가 같으면;
	// 같은 소속으로 보기 때문에 파티ID 만을 따로 비교 함;
	if ( pMyParty->GetPartyID().GetPartyID() == pNetMsg->PartyID.GetPartyID() )
	{
		//GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		//	NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetQBoxLock,
		//	"-b", pNetMsg->bQBoxEnable );

		m_pGaeaClient->GetInterface()->QBoxButtonSetEnable(pNetMsg->bQBoxEnable);
	}
}

void GLCharacter::MsgMapEffectUpdateBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_MAPEFFECT_UPDATE_BRD* pNetMsg = ( GLMSG::SNETPC_MAPEFFECT_UPDATE_BRD* ) pMsg;

    if ( pNetMsg->bADD ) 
    {
        ADDMAPEFF( pNetMsg->sMapEffect );
    }
    else
    {
        DELMAPEFF( pNetMsg->sMapEffect );
    }
}

void GLCharacter::MsgLandEffectUpdateBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_LANDEFFECT_UPDATE_BRD* pNetMsg = ( GLMSG::SNETPC_LANDEFFECT_UPDATE_BRD* ) pMsg;
    PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
    if ( !pLand )
        return;
    const SLEVEL_ETC_FUNC *pLevelEtcFunc = pLand->GetLevelEtcFunc();
    if( !pLevelEtcFunc )
        return;

    if ( pNetMsg->bADD ) 
    {
        ADDLANDEFF( pNetMsg->landEffect );
    }
    else
    {
        DELLANDEFF( pNetMsg->landEffect );
    }
}

void GLCharacter::MsgGmLimitEventTimeReqFb(NET_MSG_GENERIC* pMsg)
{
#ifndef CH_PARAM_USEGAIN //** Add EventTime
    GLMSG::SNET_GM_LIMIT_EVENT_TIME_REQ_FB *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_TIME_REQ_FB *)pMsg;

    m_bEventStart  = pNetMsg->bEventStart;
    if( m_bEventStart )
    {
        m_tLoginTime   = pNetMsg->loginTime;
        m_pGaeaClient->SetCurrentTime( m_tLoginTime );
        m_EventStartLv = pNetMsg->start_Lv;
        m_EventEndLv   = pNetMsg->end_Lv;
        m_EventBusterTime = pNetMsg->buster_Time;
		m_EventStartTime  = pNetMsg->play_Time;
		m_EventBusterExpGain = pNetMsg->expGain_Rate;
		m_EventBusterItemGain = pNetMsg->itemGain_Rate;

		if( m_wLevel >= m_EventStartLv && m_wLevel <= m_EventEndLv )
		{
			m_bEventApply  = true;

			int nTime = 0;
			int nRemainTime = 0;
			if( false == m_bEventBuster )
			{
				nTime = m_EventStartTime;
				nRemainTime = m_RemainEventTime;
			}
			else
			{
				nTime = m_EventBusterTime;
				nRemainTime = m_RemainBusterTime;
			}

			//GLWidgetScript::GetInstance().LuaCallFunc< void >(
			//	NSWIDGET_SCRIPT::g_strFunc_BonusTime_StartEvent,
			//	"-b, -n, -n, -f, -f",
			//	m_bEventBuster, nTime, nRemainTime, m_EventBusterExpGain, m_EventBusterItemGain );
			m_pGaeaClient->GetInterface()->BONUS_TIME_EVENT_START( !m_bEventBuster );

		}
		else
		{
			m_bEventApply  = false;

			//GLWidgetScript::GetInstance().LuaCallFunc< void >(
			//	NSWIDGET_SCRIPT::g_strFunc_BonusTime_EndEvent );
			m_pGaeaClient->GetInterface()->BONUS_TIME_EVENT_END();
		}
	}
#endif
}

void GLCharacter::MsgGmLimitEventEndFb(NET_MSG_GENERIC* pMsg)
{
#ifndef CH_PARAM_USEGAIN //** Add EventTime
    //m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, "이벤트 종료!!" );

    m_bEventApply  = FALSE;
    m_bEventStart  = FALSE;
    m_bEventBuster = FALSE;
	
	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_BonusTime_EndEvent );
	m_pGaeaClient->GetInterface()->BONUS_TIME_EVENT_END();

#endif
}

void GLCharacter::MsgGmLimitEventBeginFb(NET_MSG_GENERIC* pMsg)
{
#ifndef CH_PARAM_USEGAIN //** Add EventTime
    GLMSG::SNET_GM_LIMIT_EVENT_BEGIN_FB *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_BEGIN_FB *) pMsg;
    /*m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, 
    "%d 레벨 부터 %d 레벨 까지 %d분 플레이 %d 부스터 %.2f 경험치 %.2f 드랍율",
    pNetMsg->start_Lv, pNetMsg->end_Lv, pNetMsg->play_Time, pNetMsg->buster_Time,
    pNetMsg->expGain_Rate, pNetMsg->itemGain_Rate );*/

    m_bEventStart = TRUE;
    m_bEventBuster = FALSE;

    m_EventStartLv = pNetMsg->start_Lv;
    m_EventEndLv   = pNetMsg->end_Lv;

    m_EventBusterTime = pNetMsg->buster_Time;
    m_EventStartTime  = pNetMsg->play_Time;

    m_RemainEventTime  = m_EventStartTime;
	m_RemainBusterTime = m_EventBusterTime;

	m_EventBusterExpGain = pNetMsg->expGain_Rate;
	m_EventBusterItemGain = pNetMsg->itemGain_Rate;

	if( m_wLevel >= m_EventStartLv && m_wLevel <= m_EventEndLv )
	{
		const CTime& crtTime = m_pGaeaClient->GetCurrentTime();
		m_tLoginTime  = crtTime.GetTime();
		m_bEventApply = true;

		int nTime = 0;
		int nRemainTime = 0;
		if( false == m_bEventBuster )
		{
			nTime = m_EventStartTime;
			nRemainTime = m_RemainEventTime;
			m_pGaeaClient->GetInterface()->BONUS_TIME_EVENT_START( !m_bEventBuster );
		}
		else
		{
			nTime = m_EventBusterTime;
			nRemainTime = m_RemainBusterTime;
			m_pGaeaClient->GetInterface()->BONUS_TIME_EVENT_END();
		}

		//GLWidgetScript::GetInstance().LuaCallFunc< void >(
		//	NSWIDGET_SCRIPT::g_strFunc_BonusTime_StartEvent,
		//	"-b, -n, -n, -f, -f",
		//	m_bEventBuster, nTime, nRemainTime, m_EventBusterExpGain, m_E
	}
	else
	{
		m_bEventApply = false;

		/*GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_BonusTime_EndEvent );*/
		m_pGaeaClient->GetInterface()->BONUS_TIME_EVENT_END();
	}
#endif
}

void GLCharacter::MsgGmLimitEventApplyEnd(NET_MSG_GENERIC* pMsg)
{
#ifndef CH_PARAM_USEGAIN //** Add EventTime
    GLMSG::SNET_GM_LIMIT_EVENT_APPLY_END *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_APPLY_END *) pMsg;
    m_bEventBuster    = FALSE;
    m_tLoginTime      = pNetMsg->loginTime;
    m_pGaeaClient->SetCurrentTime( m_tLoginTime );
    m_RemainEventTime  = m_EventStartTime;
    m_RemainBusterTime = m_EventBusterTime;
	
	/*GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_BonusTime_StartEvent,
		"-b, -n, -n, -f, -f",
		false, m_EventStartTime, m_RemainEventTime, m_EventBusterExpGain, m_EventBusterItemGain );*/
	m_pGaeaClient->GetInterface()->BONUS_TIME_BUSTER_END();

#endif
}

void GLCharacter::MsgGmLimitEventApplyStart(NET_MSG_GENERIC* pMsg)
{
#ifndef CH_PARAM_USEGAIN //** Add EventTime
    GLMSG::SNET_GM_LIMIT_EVENT_APPLY_START *pNetMsg = (GLMSG::SNET_GM_LIMIT_EVENT_APPLY_START *) pMsg;
    m_bEventBuster    = TRUE;
    m_tLoginTime      = pNetMsg->loginTime;
    m_RemainEventTime = 0;
    m_RemainBusterTime = m_EventBusterTime;
	
	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_BonusTime_StartEvent,
	//	"-b, -n, -n, -f, -f",
	//	true, m_EventBusterTime, m_RemainBusterTime, m_EventBusterExpGain, m_EventBusterItemGain );
	m_pGaeaClient->GetInterface()->BONUS_TIME_BUSTER_START();

#endif
}

void GLCharacter::MsgGmShowMeTheMoney(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_SHOWMETHEMONEY* pNetMsg = (GLMSG::SNET_GM_SHOWMETHEMONEY*)pMsg;
	SetInvenMoney( pNetMsg->llMoney );
    std::string Msg(sc::string::format("Character money %1%", GetInvenMoney()));
    PrintMsgDlg(NS_UITEXTCOLOR::DEFAULT, Msg);

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Inventory_UpdateMoney,
	//	"-l", pNetMsg->llMoney );
}
/*
void GLCharacter::MsgSmsPhoneNumberFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PHONE_NUMBER_FB *pNetMsg = (GLMSG::SNETPC_PHONE_NUMBER_FB *) pMsg;

    std::string strText;
    switch ( pNetMsg->emFB )
    {
    case EMSMS_PHONE_NUMBER_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_FAIL") );
        break;
    case EMSMS_PHONE_NUMBER_FB_OK:
        {
            StringCchCopy ( m_szPhoneNumber, SMS_RECEIVER, pNetMsg->szPhoneNumber );

            // 캐릭터 정보창에 폰 번호 변경
            m_pInterface->CharacterSetPhoneNumber(CString(pNetMsg->szPhoneNumber));					
            m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::PALEGREEN,
                sc::string::format(ID2GAMEINTEXT("EMSMS_PHONE_NUMBER_FB_OK"), pNetMsg->szPhoneNumber));
        }
        break;
    case EMSMS_PHONE_NUMBER_FROM_DB_FAIL:				
        m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(ID2GAMEINTEXT("EMSMS_PHONE_NUMBER_FROM_DB_FAIL"), pNetMsg->szPhoneNumber));
        break;
    };
}
*/
void GLCharacter::MsgSmsSendFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SEND_SMS_FB *pNetMsg = (GLMSG::SNETPC_SEND_SMS_FB *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMSMS_SEND_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSMS_SEND_FB_FAIL") );
        break;
    case EMSMS_SEND_FB_OK:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMSMS_SEND_FB_OK") );
        break;
    case EMSMS_SEND_FROM_DB_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSMS_SEND_FROM_DB_FAIL") );
        break;
    case EMSMS_SEND_FB_NOITEM:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSMS_SEND_FB_NOITEM") );
        break;
    case EMSMS_SEND_FB_BADITEM:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMSMS_SEND_FB_BADITEM") );
        break;
    };
}

void GLCharacter::MsgUpdateStartCall(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_STARTCALL *pNetMsg = (GLMSG::SNETPC_UPDATE_STARTCALL *)pMsg;

    // 시작위치 변경
    m_sStartMapID = pNetMsg->sStartMapID;
    m_dwStartGate = pNetMsg->dwStartGateID;
    m_vSavePos	  = pNetMsg->vStartPos;
}

void GLCharacter::MsgUpdateLastCall(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_LASTCALL *pNetMsg = (GLMSG::SNETPC_UPDATE_LASTCALL *)pMsg;

    // Note : 직전귀환카드에서 사용될 정보를 받습니다.
    m_sLastCallMapID = pNetMsg->sLastCallMapID;
    m_vLastCallPos = pNetMsg->vLastCallPos;
}

void GLCharacter::MsgReBuildInputMoney(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_REBUILD_INPUT_MONEY* pNetMsg = (GLMSG::SNET_REBUILD_INPUT_MONEY*)pMsg;
    m_i64RebuildInput = pNetMsg->i64InputMoney;
}

void GLCharacter::MsgReBuildCostMoeny(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_REBUILD_COST_MONEY* pNetMsg = (GLMSG::SNET_REBUILD_COST_MONEY*)pMsg;
    m_i64RebuildCost = pNetMsg->i64CostMoney;

    if (GetInvenMoney() < m_i64RebuildCost)
    {
		//RebuildCard::GetInstance().InitRebuild();
		InitRebuildData();
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREBUILD_RESULT_MONEY" ) );        
    }
    else
    {
        ReqRebuildInputMoney( m_i64RebuildCost );
    }
}

void GLCharacter::MsgRebuildMoveItem(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_REBUILD_MOVE_ITEM* pNetMsg = (GLMSG::SNET_REBUILD_MOVE_ITEM*)pMsg;
	switch ( pNetMsg->emItemType )
	{
	case GLMSG::SNET_REBUILD_MOVE_ITEM::EMITEM_EQUIPMENT:
		{
			m_sRebuildItem.SET( pNetMsg->wPosX, pNetMsg->wPosY );
			// m_sBackUpRebuildInvenItem.sItemCustom = GET_REBUILD_ITEM();
			m_sBackUpRebuildInvenItem = SINVENITEM();

			//RebuildCard::GetInstance().ReceiveRebuildMoveItem( pNetMsg->wPosX, pNetMsg->wPosY );		
			break;
		}
		
	case GLMSG::SNET_REBUILD_MOVE_ITEM::EMITEM_STAMP:
		{
			m_sStampItem.SET( pNetMsg->wPosX, pNetMsg->wPosY );

			//RebuildCard::GetInstance().ReceiveRebuildMoveStemp();
			break;
		}
	}    
}

void GLCharacter::MsgGarbageResultFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GARBAGE_RESULT_FB* pNetMsg = (GLMSG::SNET_GARBAGE_RESULT_FB*)pMsg;

	m_ItemGarbagePosX = USHRT_MAX;
	m_ItemGarbagePosY = USHRT_MAX;

	switch( pNetMsg->emResult )
	{
	case EMGARBAGE_RESULT_FB_FAIL:
	case EMGARBAGE_RESULT_FB_ITEMTYPE:
	case EMGARBAGE_RESULT_FB_MONEY:
		InitGarbageData();
		break;
	}

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_InvenTrash_TrashResult,
	//	"-n", (int) pNetMsg->emResult );

	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Inventory_InvenAllSlotUpdate );
}

void GLCharacter::MsgRebuildResult(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_REBUILD_RESULT* pNetMsg = (GLMSG::SNET_REBUILD_RESULT*)pMsg;

	unsigned int nFixedOp = -1;

	// 결과값 정보를 백업한다;
	m_sBackUpRebuildInvenItem.sItemCustom = pNetMsg->sItemCustom;

	switch( pNetMsg->emResult )
	{
	case EMREBUILD_RESULT_MONEY:	// 확인 버튼 눌렀고, 금액이 맞지 않아서 오류 전송
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREBUILD_RESULT_MONEY" ) );
		return;

	case EMREBUILD_RESULT_DESTROY:	// 확인 버튼 눌렀고, 개조 실패, 아이템 소멸
		InitRebuildData();

		/*if ( false == RebuildCard::GetInstance().CheckItemRebuildWindowClose(
			pNetMsg->bUseStampCard,
			pNetMsg->fixOption,
			m_emTypeItemRemodelState ) )
		{
			m_emTypeItemRemodelState = EMTYPE_ITEM_REMODEL_NONE;
		}*/

		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "EMREBUILD_RESULT_DESTROY" ) );
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMREQ_REMODELOPEN_FB_OK") );
		break;

		/*
	case EMREBUILD_RESULT_TRY:
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_RebuildCard_FixPopupUI );
		break;

	case EMREBUILD_RESULT_SUCCESS:	// 확인 버튼 눌렀고, 개조 성공
		{
			if ( true == RebuildCard::GetInstance().CheckItemRebuildWindowClose(
				pNetMsg->bUseStampCard,
				pNetMsg->fixOption,
				m_emTypeItemRemodelState ) )
			{
				if ( pNetMsg->bUseStampCard == true )
				{
					m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE,
						ID2GAMEINTEXT( "EMREBUILD_RESULT_SUCCESS_WITH_STAMPCARD" ) );
					m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN,
						ID2GAMEINTEXT("EMREQ_REMODELOPEN_FB_OK_WITH_STAMPCARD") );

					RebuildCard::GetInstance().SetItemRebuildSetCheckBox(pNetMsg->sItemCustom, pNetMsg->fixOption);

					nFixedOp = RebuildCard::GetInstance().GetFixedOptionIndex( pNetMsg->fixOption );
					RebuildCard::GetInstance().SetFixedOption( nFixedOp );
				}
				else
				{
					m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, 
						ID2GAMEINTEXT( "EMREBUILD_RESULT_SUCCESS" ) );
					m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, 
						ID2GAMEINTEXT("EMREQ_REMODELOPEN_FB_OK") );
				}
			}
			else
			{
				m_emTypeItemRemodelState = EMTYPE_ITEM_REMODEL_NONE;
			}
		}
		break;
		*/
	case EMREBUILD_UPDATE:
		break;
		// 현재는 할말없음;
	case EMREBUILD_RESULT_SELECT_INTO_CACHE_OK:
	case EMREBUILD_RESULT_SELECT_INTO_CACHE_FAIL:
		break;
	case EMREBUILD_CHANGE_REBUILDTYPE_KEEPORIGIN_CACHERESULT:
		m_wRebuildType = EMREBUILDTYPE::EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT;
		//GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		//	NSWIDGET_SCRIPT::g_strFunc_RebuildCard_ChangedRebuildType,
		//	"-b", true );
		break;
	case EMREBUILD_CHANGE_REBUILDTYPE_APPLYRESULT_CACHEORIGINAL:
		m_wRebuildType = EMREBUILDTYPE::EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL;
		//GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		//	NSWIDGET_SCRIPT::g_strFunc_RebuildCard_ChangedRebuildType,
		//	"-b", false );
		break;
	}

	//GLWidgetCallFuncValue args[ 3 ];

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(m_sRebuildItem.wPosX, m_sRebuildItem.wPosY);
	if ( NULL == pInvenItem )
		return;

	if ( pNetMsg->bUseStampCard ) m_pGaeaClient->GetInterface()->SetItemRebuildSetCheckBox( pInvenItem->sItemCustom, pNetMsg->fixOption );
	m_pGaeaClient->GetInterface()->SetItemPreviousInformation( pNetMsg->sItemCustom );
}


void GLCharacter::MsgInvenHairChangeFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_HAIR_CHANGE_FB *pNetMsg = (GLMSG::SNETPC_INVEN_HAIR_CHANGE_FB *) pMsg;

	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_StyleHairError, "-n, -n", (int) pNetMsg->emFB, (WORD) pNetMsg->dwID  );
}

void GLCharacter::MsgInvenHairColorChangeFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE_FB *pNetMsg = (GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE_FB*)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_HAIR_CHANGE_FB_FAIL:
        HairColorChange( m_wHairColor );
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_FAIL") );
        break;
    case EMINVEN_HAIR_CHANGE_FB_OK:
        {
            m_wHairColor = pNetMsg->wHairColor;
			HairColorChange( m_wHairColor );
            //UpdateSuit( TRUE );

            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_OK") );
        }
        break;
    case EMINVEN_HAIR_CHANGE_FB_NOITEM:
        HairColorChange( m_wHairColor );
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_NOITEM") );
        break;
    case EMINVEN_HAIR_CHANGE_FB_BADITEM:
        HairColorChange( m_wHairColor );
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_BADITEM") );
        break;
    case EMINVEN_HAIR_CHANGE_FB_BADCLASS:
        HairColorChange( m_wHairColor );
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_BADCLASS") );
        break;
    case EMINVEN_HAIR_CHANGE_FB_COOLTIME:
        HairColorChange( m_wHairColor );
        break;
    };
}

void GLCharacter::MsgCostumeColorChangeFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_COSTUM_COLOR_CHANGE_FB *pNetMsg = (GLMSG::SNETPC_INVEN_COSTUM_COLOR_CHANGE_FB*)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_COSTUM_COLOR_CHANGE_FB_FAIL:
        UpdateSuit();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_COSTUM_COLOR_CHANGE_FB_FAIL") );
        break;
    case EMINVEN_COSTUM_COLOR_CHANGE_FB_OK:
        {
            EMSLOT	emSlot = pNetMsg->emSlot;

            if ( m_PutOnItems[ emSlot ].GetNativeID() != NATIVEID_NULL() )
            {
                m_PutOnItems[ emSlot ].dwMainColor = pNetMsg->dwMainColor;
                m_PutOnItems[ emSlot ].dwSubColor = pNetMsg->dwSubColor;
            }

            UpdateSuit();

            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMINVEN_COSTUM_COLOR_CHANGE_FB_OK") );
			//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Character_UpdateSlot );
        }
        break;
    case EMINVEN_COSTUM_COLOR_CHANGE_FB_NOITEM:
        UpdateSuit();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_COSTUM_COLOR_CHANGE_FB_NOITEM") );
        break;
    case EMINVEN_COSTUM_COLOR_CHANGE_FB_BADITEM:
        UpdateSuit();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_COSTUM_COLOR_CHANGE_FB_BADITEM") );
        break;
    case EMINVEN_COSTUM_COLOR_CHANGE_FB_BADCLASS:
        UpdateSuit();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_COSTUM_COLOR_CHANGE_FB_BADCLASS") );
        break;
    case EMINVEN_COSTUM_COLOR_CHANGE_FB_RESTRICT:
        UpdateSuit();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_COSTUM_COLOR_CHANGE_FB_RESTRICT") );
        break;
    case EMINVEN_COSTUM_COLOR_CHANGE_FB_COOLTIME:
        UpdateSuit();
        break;
    };
}


void GLCharacter::MsgInvenFaceChangeFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_FACE_CHANGE_FB *pNetMsg = (GLMSG::SNETPC_INVEN_FACE_CHANGE_FB *) pMsg;

	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_StyleFaceError, "-n, -n", (int) pNetMsg->emFB, (WORD) pNetMsg->dwID  );
}

void GLCharacter::MsgInvenGenderChangeFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_GENDER_CHANGE_FB *pNetMsg = (GLMSG::SNETPC_INVEN_GENDER_CHANGE_FB *) pMsg;

   // GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_StyleGenderError, "-n", (int) pNetMsg->emFB );

	switch ( pNetMsg->emFB )
	{
	case EMINVEN_GENDER_CHANGE_FB_OK:
		m_pGaeaClient->GetInterface()->DoModal( ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_OK"), UI::MODAL_NOTITLE, UI::OK, UI::MODAL_GENDER_CHANGE_END );
		break;
	case EMINVEN_GENDER_CHANGE_FB_NOTVALUE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_NOTVALUE") );
		break;
	case EMINVEN_GENDER_CHANGE_FB_FAIL:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_FAIL") );
		break;
	case EMINVEN_GENDER_CHANGE_FB_ITEMTYPE:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_ITEMTYPE") );
		break;
	case EMINVEN_GENDER_CHANGE_FB_NOITEM:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_NOITEM") );
		break;
	case EMINVEN_GENDER_CHANGE_FB_BADCLASS:
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_BADCLASS") );
		break;
	}
}

void GLCharacter::MsgInvenSchoolChangeFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_SCHOOL_CHANGE_FB *pNetMsg = (GLMSG::SNETPC_INVEN_SCHOOL_CHANGE_FB *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_SCHOOL_CHANGE_FB_OK:
		//GLWidgetScript::GetInstance().LuaCallFunc< void >(
		//	NSWIDGET_SCRIPT::g_strFunc_SchoolChangeOK );
		m_wSchool = pNetMsg->wSchool;
		m_pGaeaClient->GetInterface()->DoModal( ID2GAMEINTEXT("EMINVEN_SCHOOL_CHANGE_FB_OK"), UI::MODAL_NOTITLE, UI::OK, UI::MODAL_CLOSEGAME );
        break;

    case EMINVEN_SCHOOL_CHANGE_FB_NOITEM:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_SCHOOL_CHANGE_FB_NOITEM") );
        }
        break;
    case EMINVEN_SCHOOL_CHANGE_FB_ITEMTYPE:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_SCHOOL_CHANGE_FB_ITEMTYPE") );
        }
        break;

    case EMINVEN_SCHOOL_CHANGE_FB_NOTVALUE:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_SCHOOL_CHANGE_FB_NOTVALUE") );
        }
		break;

	case EMINVEN_SCHOOL_CHANGE_FB_MAP_ERROR:
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_SCHOOL_CHANGE_FB_MAP_ERROR") );
		}
		break;

	case EMINVEN_SCHOOL_CHANGE_FB_SCHOOL_ERROR:
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_SCHOOL_CHANGE_FB_SCHOOL_ERROR") );
		}
		break;

	case EMINVEN_SCHOOL_CHANGE_FB_CLUB_ERROR:
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_SCHOOL_CHANGE_FB_CLUB_ERROR") );
		}
		break;

    default:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_SCHOOL_CHANGE_FB_FAIL") );
        }
        break;
    }

}

void GLCharacter::MsgInvenRenameFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_RENAME_FB *pNetMsg = (GLMSG::SNETPC_INVEN_RENAME_FB *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_RENAME_FB_OK:
        {
			// [shhan][2015.10.02] rm #1948 - 캐릭터가 이름 변경카드 사용 후 게임에 재접속 할 때 개인정보 보호 옵션이 저장이 안되는 문제
			//						RANPARAM::SAVE_GAMEOPT 에서 변경된 이름파일로 옵션을 저장하여야 되는데, 이전 이름으로 옵션을 저장하여서 문제가 생김.
			//						LOAD_GAMEOPT 시 백업한 strENC_CHARID 값을 직접 변경하도록 함.
			RANPARAM::strENC_CHARID = pNetMsg->szName;

            SetName(pNetMsg->szName);

            // 이름표 변경					
            CROWREN sDISP_NAME;
            sDISP_NAME.SetData(m_pGaeaClient, this);

			//GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME );
			m_pGaeaClient->GetInterface()->NameDisplayAddName(sDISP_NAME);

            if ( m_pMyClub->IsMember ( m_CharDbNum ) )
            {
                GLCLUBMEMBER* pCMember = m_pMyClub->GetMember( m_CharDbNum );
                if (pCMember)
                    pCMember->SetName(pNetMsg->szName);
            }

            if ( m_pMyClub->IsMaster ( m_CharDbNum ) )
            {
                m_pMyClub->SetMasterName(pNetMsg->szName);
            }
			
            GLPartySlotClient* const pPMember = m_pGaeaClient->GetMyPartyClient()->GetMemberByGaeaID(m_dwGaeaID);
            if ( pPMember != NULL )
				pPMember->SetName(pNetMsg->szName);

            //	Note : 인터페이스에 변경 알림.
			// GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Community_FriendStateUpdate );
			//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CharInfo_Update );

            std::string strText = sc::string::format( ID2GAMEINTEXT("EMINVEN_RENAME_FB_OK"), pNetMsg->szName ).c_str();
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
        }
        break;
    case EMINVEN_RENAME_FB_LENGTH:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("NEWCHAR_NAME_TOO_SHORT") );	
        break;
    case EMINVEN_RENAME_FROM_DB_BAD:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("CHARACTER_BADNAME") );
        break;
    case EMINVEN_RENAME_FROM_DB_FAIL:
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT("EMINVEN_RENAME_FROM_DB_FAIL"), pNetMsg->szName ).c_str();
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, strText );
        }
        break;
    case EMINVEN_RENAME_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_FAIL") );
        break;
    case EMINVEN_RENAME_FB_NOITEM:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_NOITEM") );
        break;
    case EMINVEN_RENAME_FB_BADITEM:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_BADITEM") );
        break;
    case EMINVEN_RENAME_FB_THAICHAR_ERROR:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_THAICHAR_ERROR") );
        break;
    case EMINVEN_RENAME_FB_VNCHAR_ERROR:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_VNCHAR_ERROR") );
        break;
    };
}



void GLCharacter::MsgReqEnterNumChargeFb( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FC *pNetMsg = (GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FC *) pMsg;

	SNATIVEID sMapID;
	sMapID.dwID = pNetMsg->dwMapID;

	// Sub ID 가 1 이면 Matching System 용이다;
	// 신규인던으로 모두 넘어올 때 까지 임시로 사용한다;
	if ( 1 == sMapID.Sid() )
	{
		DWORD dwMatchingBaseID = static_cast< DWORD >( sMapID.Mid() );
		MatchingSystem::GLTouranmentClient* pMatching = m_pGaeaClient->GetTournamentClient();
		if ( NULL == pMatching )
		{
			// 잘못됨;
			return;
		}

		if ( 0 <= dwMatchingBaseID &&
			pMatching->m_VecMatchingInfo.size() > dwMatchingBaseID )
		{
			const MatchingSystem::GLMatchingInfo* pMatchingInfo =
				pMatching->m_VecMatchingInfo.at( dwMatchingBaseID );
			if ( NULL == pMatchingInfo )
			{
				// 잘못됨;
				return;
			}

			std::string strContentName = pMatchingInfo->m_strContentsName;

			std::string strText = sc::string::format(
				ID2GAMEINTEXT("COMPETITION_PVE_ENREANCE_CHARGE_TEXT"),
				ID2GAMEINTEXT( strContentName.c_str() ) ).c_str();
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );	
		}

		return;
	}

	GLPVEClient* pPVEClient = m_pGaeaClient->GetPVEClient();

	TriggerMapInfo* info;
	pPVEClient->FindInfo( pNetMsg->dwMapID, info);
	std::string strText = sc::string::format( ID2GAMEINTEXT("COMPETITION_PVE_ENREANCE_CHARGE_TEXT"), ID2GAMEWORD(info->mapname.c_str()) ).c_str();
	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );

}

void GLCharacter::MsgInvenItemDecomposeFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_REQ_INVEN_ITEMDECOMPOSE_FB* pMsgFB = (GLMSG::SNETPC_REQ_INVEN_ITEMDECOMPOSE_FB*)pMsg;

	switch ( pMsgFB->emFeedback )
	{
	case EMREQ_ITEMDECOMPOSE_FB_FAILED:
	case EMREQ_ITEMDECOMPOSE_FB_NOT_SPLIT:
	case EMREQ_ITEMDECOMPOSE_FB_NOT_ENOUGH_INVEN:
	case EMREQ_ITEMDECOMPOSE_FB_NOT_ENOUGH_MONEY:
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("EMREQ_DECOMPOSE_MESSAGE", pMsgFB->emFeedback)  );
		break;
	case EMREQ_ITEMDECOMPOSE_FB_SUCCESS:
		{
			SITEM* const pItem = GLItemMan::GetInstance().GetItem(pMsgFB->sDecomposeItem);
			if ( !pItem )
				break;

			SINVENITEM* pInvenItem = NULL;
			const std::string strDecomposeItem(pItem->GetNameStr());
			const std::string strDecomposeResultMsg(ID2GAMEWORD("DECOMPOSE_RESULTITEM_MSG"));
			for ( int i = 0; i < GLMSG::SNETPC_REQ_INVEN_ITEMDECOMPOSE_FB::MAX_ITEM_COUNT && (pMsgFB->sGenIteminfo[i].sNID != NATIVEID_NULL()); ++i )
			{
				if (NULL != (pInvenItem = m_cInventory.FindItem( pMsgFB->sGenIteminfo[i].sNID )))
				{
					m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN
						, sc::string::format(
							strDecomposeResultMsg.c_str()
							, strDecomposeItem.c_str()
							, GLItemMan::GetInstance().GetItemName(pInvenItem->GetNativeID())
							, pMsgFB->sGenIteminfo[i].wTurnNum) );
				}
			}
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEWORD("EMREQ_DECOMPOSE_MESSAGE", EMREQ_ITEMDECOMPOSE_FB_SUCCESS) );
		}
		break;

	default:
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEWORD("EMREQ_DECOMPOSE_MESSAGE", EMREQ_ITEMDECOMPOSE_FB_SUCCESS) );
		break;
	}
}

void GLCharacter::MsgCostumeStatsFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_INVEN_COSTUME_STATS_FB *pNetMsg = (GLMSG::SNET_INVEN_COSTUME_STATS_FB *)pMsg;

	switch ( pNetMsg->emFB )
	{
	case EMREQ_CS_FB_FAIL1:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG2") );
		break;
	case EMREQ_CS_FB_FAIL2:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG3") );
		break;
	case EMREQ_CS_FB_FAIL3:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG1") );
		break;
	case EMREQ_CS_FB_FAIL4:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG4") );
		break;
	case EMREQ_CS_FB_FAIL5:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG5") );
		break;
	case EMREQ_CS_FB_FAIL6:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG6") );
		break;
	case EMREQ_CS_FB_FAIL7:		
		break;
	case EMREQ_CS_FB_SUCCESS :
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SUCCESS_COSTUME_MSG") );
		break;
	case ENREQ_CS_FB_NOTICE1 :
		{
			SITEM* pItem = GLogicData::GetInstance().GetItem(pNetMsg->sItemID);

			if( pItem ) {
				std::string msg = sc::string::format(ID2GAMEINTEXT("NOTICE_COSTUME_MSG1"), pItem->GetName());
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, msg.c_str() );
			}
		}
		break;
	};

	//!! SF_TODO
	if( pNetMsg->emFB == EMREQ_CS_FB_SUCCESS || pNetMsg->emFB == EMREQ_CS_FB_FAIL7 )
	{
		m_pGaeaClient->GetInterface()->OpenCostumeStatWindow();
	}
}

void GLCharacter::MsgDrugFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_DRUG_FB *pNetMsg = (GLMSG::SNET_DRUG_FB *)pMsg;

	SITEM* pItem = NULL;

	switch( pNetMsg->emFB )
	{
	case EMREQ_DRUG_CONSUME_SUCCESS :
		{
			pItem = GLogicData::GetInstance().GetItem(pNetMsg->sItemID);
		}
		break;
	}

	if( pItem == NULL ) return;

	switch( pItem->sDrugOp.emDrug )
	{
	case ITEM_DRUG_EFFECT_RELEASE :
		{
			ReleaseCureEffect( pItem->sDrugOp.dwERList );
		}
		break;
	}
}

void GLCharacter::MsgCostumeStatsReleaseFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_INVEN_COSTUME_RELEASE_FB *pNetMsg = (GLMSG::SNET_INVEN_COSTUME_RELEASE_FB *)pMsg;

	if( pNetMsg == NULL ) return;

	switch ( pNetMsg->emFB )
	{
	case EMREQ_CS_RELEASE_FB_SUCCESS :
		{
			SITEM* pItem = GLogicData::GetInstance().GetItem(pNetMsg->sItemID);

			if( pItem )
			{
				std::string msg = sc::string::format(ID2GAMEINTEXT("SUCCESS_RELEASE_COSTUME_MSG"), pItem->GetName());
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, msg.c_str() );
			}
		}
		break;
	case EMREQ_CS_RELEASE_FB_FAIL2 :
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG7") );
		}
		break;
	}
}


void GLCharacter::MsgJumpingLevelUpFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_JUMPINGLEVELUP_FB *pNetMsg = (GLMSG::SNETPC_REQ_JUMPINGLEVELUP_FB *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_EXPE_FB_OK:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CARD_OK") );
        }
        break;

    case EMINVEN_EXP_FB_NOITEM:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CARD_NOITEM") );
        }
        break;
    case EMINVEN_EXP_FB_ITEMTYPE:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CARD_TYPEERROR") );
        }
        break;
    default:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CARD_ERROR") );
        }
        break;
    }

}

void GLCharacter::MsgExpCompressorAns(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_EXP_COMPRESSOR_ANS *pNetMsg = (GLMSG::SNETPC_EXP_COMPRESSOR_ANS *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_EXP_COMPRESSOR_OK:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_COMPRESSOR_OK") );
        }
        break;

    case EMINVEN_EXP_COMPRESSOR_NOTEXIST:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_COMPRESSOR_NOTEXIST") );
        }
        break;
    case EMINVEN_EXP_COMPRESSOR_ITEMTYPE:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_COMORESSOR_ITEMTYPE") );
        }
        break;
	case EMINVEN_EXP_COMPRESSOR_NOTENOUGH:
		{
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_COMPRESSOR_NOTENOUGH") );
        }
		break;
    default:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_COMPRESSOR_ERROR") );
        }
        break;
    }

}

void GLCharacter::MsgExpCapsuleAns(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_EXP_CAPSULE_ANS *pNetMsg = (GLMSG::SNETPC_EXP_CAPSULE_ANS *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_EXP_CAPSULE_OK:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CAPSULE_OK") );
        }
        break;

    case EMINVEN_EXP_CAPSULE_NOTEXIST:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CAPSULE_NOTEXIST") );
        }
        break;
    case EMINVEN_EXP_CAPSULE_ITEMTYPE:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CAPSULE_ITEMTYPE") );
        }
        break;
    default:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CAPSULE_ERROR") );
        }
        break;
    }

}

void GLCharacter::MsgInvenChangeItemSkillFB ( NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNET_INVEN_CHANGE_ITEMSKILL_FB* pNetMsg =
		static_cast< GLMSG::SNET_INVEN_CHANGE_ITEMSKILL_FB* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return;

	switch( pNetMsg->emFlag )
	{
	case EMCHANGE_ITEMSKILL_FB_FAIL:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_FAIL" ) );
		}
		break;

	case EMCHANGE_ITEMSKILL_FB_OK:
		{
			m_emTypeItemRemodelState = EMTYPE_ITEM_REMODEL_ITEMSKILL;
			//RebuildCard::GetInstance().OpenRebuildUI( m_emTypeItemRemodelState );
		}
		break;

	case EMCHANGE_ITEMSKILL_FB_NOITEM:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOITEM" ) );
		}
		break;

	case EMCHANGE_ITEMSKILL_FB_COOLTIME:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_COOLTIME" ) );
		}
		break;

	case EMCHANGE_ITEMSKILL_FB_NOTITEM:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOTITEM" ) );
		}
		break;

	case EMCHANGE_ITEMSKILL_FB_DONTCHANGE:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_DONTCHANGE" ) );
		}
		break;
	};
}

void GLCharacter::MsgInvenChangeBasicStatsFB ( NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNET_INVEN_CHANGE_BASICSTATS_FB* pNetMsg =
		static_cast< GLMSG::SNET_INVEN_CHANGE_BASICSTATS_FB* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return;

	switch( pNetMsg->emFlag )
	{
	case EMCHANGE_BASICSTATS_FB_FAIL:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_FAIL" ) );
		}
		break;

	case EMCHANGE_BASICSTATS_FB_OK:
		{
			m_emTypeItemRemodelState = EMTYPE_ITEM_REMODEL_BASICSTATS;
			//RebuildCard::GetInstance().OpenRebuildUI( m_emTypeItemRemodelState );
		}
		break;

	case EMCHANGE_BASICSTATS_FB_NOITEM:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOITEM" ) );
		}
		break;

	case EMCHANGE_BASICSTATS_FB_STAMP_ERROR:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_STAMP_ERROR" ) );
		}
		break;

	case EMCHANGE_BASICSTATS_FB_COOLTIME:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_COOLTIME" ) );
		}
		break;

	case EMCHANGE_BASICSTATS_FB_NOTITEM:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOTITEM" ) );
		}
		break;

	case EMCHANGE_BASICSTATS_FB_DONTCHANGE:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_DONTCHANGE" ) );
		}
		break;
	};

	if ( EMCHANGE_BASICSTATS_FB_OK != pNetMsg->emFlag )
	{
		//GLWidgetScript::GetInstance().LuaCallFunc< void >(
			//NSWIDGET_SCRIPT::g_strFunc_RebuildCard_StampUncheck );
		m_pGaeaClient->GetInterface()->SetItemRebuildResetCheckBox();
	}
}

void GLCharacter::MsgInvenChangeAddStatsFB ( NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNET_INVEN_CHANGE_ADDSTATS_FB* pNetMsg =
		static_cast< GLMSG::SNET_INVEN_CHANGE_ADDSTATS_FB* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return;

	switch( pNetMsg->emFlag )
	{
	case EMCHANGE_ADDSTATS_FB_FAIL:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_FAIL" ) );
		}
		break;

	case EMCHANGE_ADDSTATS_FB_OK:
		{
			m_emTypeItemRemodelState = EMTYPE_ITEM_REMODEL_ADDSTATS;
			//RebuildCard::GetInstance().OpenRebuildUI( m_emTypeItemRemodelState );
		}
		break;

	case EMCHANGE_ADDSTATS_FB_NOITEM:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOITEM" ) );
		}
		break;

	case EMCHANGE_ADDSTATS_FB_STAMP_ERROR:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_STAMP_ERROR" ) );
		}
		break;

	case EMCHANGE_ADDSTATS_FB_COOLTIME:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_COOLTIME" ) );
		}
		break;

	case EMCHANGE_ADDSTATS_FB_NOTITEM:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOTITEM" ) );
		}
		break;

	case EMCHANGE_ADDSTATS_FB_DONTCHANGE:
		{
			m_pGaeaClient->PrintMsgTextDlg(
				NS_UITEXTCOLOR::DISABLE,
				ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_DONTCHANGE" ) );
		}
		break;
	};

	if ( EMCHANGE_ADDSTATS_FB_OK != pNetMsg->emFlag )
	{
		//GLWidgetScript::GetInstance().LuaCallFunc< void >(
		//	NSWIDGET_SCRIPT::g_strFunc_RebuildCard_StampUncheck );
		m_pGaeaClient->GetInterface()->SetItemRebuildResetCheckBox();
	}
}