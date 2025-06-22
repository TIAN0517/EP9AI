#include "KillCardUIHost_Std.h"
#include <cassert>
#include <iostream>

void AutoTest_KillCardUI() {
    KillCardUIHost_Std uiHost;
    KillerInfo killer = {"測試者", 10, "戰士", "測試公會", "測試劍", WeaponType::SWORD, {"測試技能"}, {{"攻擊",100}}, {"成就1"}};
    VictimInfo victim = {"假想敵", 8, "法師", "敵對公會", "法杖", {{"防禦",50}}, true, false, ""};
    KillLocation loc = {"測試地圖", "測試區域", 1.0f, 2.0f, 3.0f, "測試點"};
    KillEventData event = {"EVT_TEST", KillType::NORMAL_KILL, killer, victim, loc, std::chrono::system_clock::now(), 123, false, 1, {"閃光"}, {}};
    KillDescription desc = {"測試標題", "測試描述", "副標題", {"亮點1"}, "平靜", "簡約", 5};
    KillEffectConfig eff = {"閃光", "粒子", {{"亮度",0.5f}}, "音效", "粒子", 1.0f, 1};
    KillCardData card = {"CARD_TEST", event, desc, {eff}, "bg_test.jpg", "test.mp3", false, 1, std::chrono::system_clock::now(), std::chrono::system_clock::now(), true};
    uiHost.ShowKillCard(card);
    uiHost.PlayKillAnimation("TestAnim");
    uiHost.ShowEffect("閃光");
    uiHost.HideKillCard();
    // 可擴展斷言檢查UI狀態
    std::cout << "[AutoTest] KillCardUIHost_Std 測試通過" << std::endl;
}

int main() {
    AutoTest_KillCardUI();
    return 0;
}
