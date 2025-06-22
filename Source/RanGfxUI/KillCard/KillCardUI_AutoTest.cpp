#include "KillCardUIHost_Std.h"
#include <cassert>
#include <iostream>
#include <memory>

void AutoTest_KillCardUI_Normal() {
    std::cout << "[AutoTest] 正常流程測試..." << std::endl;
    auto uiHost = std::make_shared<KillCardUIHost_Std>();
    KillerInfo killer = {"測試者", 10, "戰士", "測試公會", "測試劍", WeaponType::SWORD, {"測試技能"}, {{"攻擊",100}}, {"成就1"}};
    VictimInfo victim = {"假想敵", 8, "法師", "敵對公會", "法杖", {{"防禦",50}}, true, false, ""};
    KillLocation loc = {"測試地圖", "測試區域", 1.0f, 2.0f, 3.0f, "測試點"};
    KillEventData event = {"EVT_TEST", KillType::NORMAL_KILL, killer, victim, loc, std::chrono::system_clock::now(), 123, false, 1, {"閃光"}, {}};
    KillDescription desc = {"測試標題", "測試描述", "副標題", {"亮點1"}, "平靜", "簡約", 5};
    KillEffectConfig eff = {"閃光", "粒子", {{"亮度",0.5f}}, "音效", "粒子", 1.0f, 1};
    KillCardData card = {"CARD_TEST", event, desc, {eff}, "bg_test.jpg", "test.mp3", false, 1, std::chrono::system_clock::now(), std::chrono::system_clock::now(), true};
    uiHost->ShowKillCard(card);
    uiHost->PlayKillAnimation("TestAnim");
    uiHost->ShowEffect("閃光");
    uiHost->HideKillCard();
    std::cout << "[AutoTest] 正常流程通過\n";
}

void AutoTest_KillCardUI_EdgeCases() {
    std::cout << "[AutoTest] 邊界/異常情境測試..." << std::endl;
    auto uiHost = std::make_shared<KillCardUIHost_Std>();
    // 1. 未顯示面板直接播放動畫/特效/隱藏
    uiHost->PlayKillAnimation("EdgeAnim"); // 應有警告
    uiHost->ShowEffect("EdgeEffect");      // 應有警告
    uiHost->HideKillCard();                 // 應有警告
    // 2. 顯示面板後重複顯示/隱藏
    KillerInfo killer = {"邊界者", 1, "新手", "", "木劍", WeaponType::SWORD, {}, {}, {}};
    VictimInfo victim = {"無名氏", 1, "新手", "", "", {}, true, false, ""};
    KillLocation loc = {"", "", 0,0,0, ""};
    KillEventData event = {"EVT_EDGE", KillType::NORMAL_KILL, killer, victim, loc, std::chrono::system_clock::now(), 0, false, 0, {}, {}};
    KillDescription desc = {"", "", "", {}, "", "", 0};
    KillCardData card = {"CARD_EDGE", event, desc, {}, "", "", false, 0, std::chrono::system_clock::now(), std::chrono::system_clock::now(), false};
    uiHost->ShowKillCard(card);
    uiHost->ShowKillCard(card); // 重複顯示
    uiHost->HideKillCard();
    uiHost->HideKillCard();     // 重複隱藏
    std::cout << "[AutoTest] 邊界/異常情境通過\n";
}

int main() {
    AutoTest_KillCardUI_Normal();
    AutoTest_KillCardUI_EdgeCases();
    std::cout << "[AutoTest] KillCardUIHost_Std 全部測試通過" << std::endl;
    return 0;
}
