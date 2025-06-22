#include "KillCardUIHost_Std.h"
#include <iostream>

// 預設渲染/動畫/特效子系統實作
void KillCardRenderer_Default::RenderPanel(const KillCardData& data) {
    std::cout << "[DefaultRenderer] 渲染擊殺卡片: " << data.cardId << std::endl;
    std::cout << "  標題: " << data.description.title << "  擊殺者: " << data.eventData.killer.name << " 受害者: " << data.eventData.victim.name << std::endl;
    std::cout << "  稀有度: " << data.rarity << "  動畫: " << (data.isAnimated ? "是" : "否") << std::endl;
}
void KillCardRenderer_Default::PlayAnimation(const std::string& animName) {
    std::cout << "[DefaultRenderer] 播放動畫: " << animName << std::endl;
}
void KillCardRenderer_Default::PlayEffect(const std::string& effectName) {
    std::cout << "[DefaultRenderer] 播放特效: " << effectName << std::endl;
}
void KillCardRenderer_Default::ClearPanel() {
    std::cout << "[DefaultRenderer] 清理面板" << std::endl;
}

// KillCardUIHost_Std
KillCardUIHost_Std::KillCardUIHost_Std(std::shared_ptr<IKillCardRenderer> renderer)
    : m_renderer(std::move(renderer)), m_panelVisible(false) {}
KillCardUIHost_Std::~KillCardUIHost_Std() {}

void KillCardUIHost_Std::ShowKillCard(const KillCardData& data) {
    m_lastCard = data;
    m_panelVisible = true;
    std::cout << "[UI] 顯示擊殺面板: " << data.cardId << std::endl;
    m_renderer->RenderPanel(data);
}
void KillCardUIHost_Std::PlayKillAnimation(const std::string& animName) {
    if (!m_panelVisible) {
        std::cerr << "[UI][警告] 播放動畫前請先顯示面板！" << std::endl;
        return;
    }
    m_renderer->PlayAnimation(animName);
}
void KillCardUIHost_Std::ShowEffect(const std::string& effectName) {
    if (!m_panelVisible) {
        std::cerr << "[UI][警告] 顯示特效前請先顯示面板！" << std::endl;
        return;
    }
    m_renderer->PlayEffect(effectName);
}
void KillCardUIHost_Std::HideKillCard() {
    if (!m_panelVisible) {
        std::cerr << "[UI][警告] 面板已隱藏！" << std::endl;
        return;
    }
    m_panelVisible = false;
    m_renderer->ClearPanel();
    std::cout << "[UI] 隱藏擊殺面板" << std::endl;
}
