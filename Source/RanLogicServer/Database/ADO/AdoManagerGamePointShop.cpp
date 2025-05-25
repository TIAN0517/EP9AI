#include "../../pch.h"
#include "./AdoManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

//! Character �� PointShop Point �� �����´�
//! ChaDbNum : ĳ���� DB ��ȣ
//! PointRefundable : ȯ�Ұ��� ����Ʈ
//! PointNotRefundable : ȯ�ҺҰ� ����Ʈ
int AdoManager::GetChaPoint(int ChaDbNum, __int64& PointRefundable, __int64& PointNotRefundable, __int64& PointBattle)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_IPARAM_INT(Ado, "@ChaNum", ChaDbNum);
    if (!Ado.Execute4Cmd("dbo.sp_GetChaPoint", adCmdStoredProc))
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())
    {
        Ado.PRINT("AdoManager::GetChaPoint no data. RecordSet is Empty.");
        return sc::db::DB_ERROR;
    }

    do
    {            
        Ado.GetCollect(0, PointRefundable);
		Ado.GetCollect(1, PointNotRefundable);  
		Ado.GetCollect(2, PointBattle);        
    } while (Ado.Next());

    return sc::db::DB_OK;
}

//! Character �� PointShop Point �� �����Ѵ�
//! ChaDbNum : ĳ���� DB ��ȣ
//! PointRefundable : ȯ�Ұ��� ����Ʈ
//! PointNotRefundable : ȯ�ҺҰ� ����Ʈ
int AdoManager::SetChaPoint(int ChaDbNum, const __int64& PointRefundable, const __int64& PointNotRefundable, const __int64& PointBattle)
{   
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    AdoGame.AppendIParamInteger   ("@ChaNum", ChaDbNum);
    AdoGame.AppendIParamBigint("@ChaPointRefundable", PointRefundable);
	AdoGame.AppendIParamBigint("@ChaPointNotRefundable", PointNotRefundable);
	AdoGame.AppendIParamBigint("@ChaPointBattle", PointBattle);

    return AdoGame.ExecuteStoredProcedure("dbo.sp_SetChaPoint");
}

}