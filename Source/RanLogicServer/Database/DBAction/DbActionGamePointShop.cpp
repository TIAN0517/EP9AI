#include "../../pch.h"
#include "../DbManager.h"
#include "./DbActionGamePointShop.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

CharacterPointSet::CharacterPointSet(
    DWORD ChaDbNum,
    __int64 PointRefundable,
	__int64 PointNotRefundable,
	__int64 PointBattle)
    : DbAction(GAME_DB)
    , m_ChaDbNum(ChaDbNum)
    , m_PointRefundable(PointRefundable)
    , m_PointNotRefundable(PointNotRefundable)
	, m_PointBattle(PointBattle)
{   
}

CharacterPointSet::~CharacterPointSet()
{
}

int CharacterPointSet::Execute(NetServer* pServer)
{
    return m_pDbManager->SetChaPoint(m_ChaDbNum, m_PointRefundable, m_PointNotRefundable, m_PointBattle );
}


} // namespace db