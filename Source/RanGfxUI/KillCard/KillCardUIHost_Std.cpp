#include "KillCardUIHost_Std.h"
#include <iostream>

KillCardUIHost_Std::KillCardUIHost_Std() {}
KillCardUIHost_Std::~KillCardUIHost_Std() {}

void KillCardUIHost_Std::ShowKillCard(const KillCardData& data) {
    std::cout << "[UI] 顯示擊殺面板: " << data.cardId << std::endl;
    RenderPanel(data);
}
void KillCardUIHost_Std::PlayKillAnimation(const std::string& animName) {
    std::cout << "[UI] 播放動畫: " << animName << std::endl;
    PlayAnimation(animName);
}
void KillCardUIHost_Std::ShowEffect(const std::string& effectName) {
    std::cout << "[UI] 顯示特效: " << effectName << std::endl;
    PlayEffect(effectName);
}
void KillCardUIHost_Std::HideKillCard() {
    std::cout << "[UI] 隱藏擊殺面板" << std::endl;
    ClearPanel();
}
void KillCardUIHost_Std::RenderPanel(const KillCardData& data) {
    // TODO: 連接實際渲染層（DX/OpenGL/ImGui等）
    std::cout << "  [渲染] 卡片標題: " << data.description.title << std::endl;
    std::cout << "  [渲染] 擊殺者: " << data.eventData.killer.name << " 受害者: " << data.eventData.victim.name << std::endl;
    // ...可擴展更多UI細節...
}
void KillCardUIHost_Std::PlayAnimation(const std::string& animName) {
    // TODO: 調用動畫系統
    std::cout << "  [動畫] 執行動畫: " << animName << std::endl;
}
void KillCardUIHost_Std::PlayEffect(const std::string& effectName) {
    // TODO: 調用特效系統
    std::cout << "  [特效] 執行特效: " << effectName << std::endl;
}
void KillCardUIHost_Std::ClearPanel() {
    // TODO: 清理UI面板
    std::cout << "  [UI] 清理面板" << std::endl;
}
