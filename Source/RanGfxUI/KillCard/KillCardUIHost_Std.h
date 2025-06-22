#pragma once
#include "IKillCardUIHost.h"
#include <memory>
#include <string>
#include <vector>

// 擴展：抽象渲染/動畫/特效子系統接口
class IKillCardRenderer {
public:
    virtual ~IKillCardRenderer() = default;
    virtual void RenderPanel(const KillCardData& data) = 0;
    virtual void PlayAnimation(const std::string& animName) = 0;
    virtual void PlayEffect(const std::string& effectName) = 0;
    virtual void ClearPanel() = 0;
};

// 預設渲染/動畫/特效子系統（可替換為DX/OpenGL/ImGui等）
class KillCardRenderer_Default : public IKillCardRenderer {
public:
    void RenderPanel(const KillCardData& data) override;
    void PlayAnimation(const std::string& animName) override;
    void PlayEffect(const std::string& effectName) override;
    void ClearPanel() override;
};

// 正式UI實現：可連接渲染層、動畫、特效等
class KillCardUIHost_Std : public IKillCardUIHost {
public:
    explicit KillCardUIHost_Std(std::shared_ptr<IKillCardRenderer> renderer = std::make_shared<KillCardRenderer_Default>());
    ~KillCardUIHost_Std();
    void ShowKillCard(const KillCardData& data) override;
    void PlayKillAnimation(const std::string& animName) override;
    void ShowEffect(const std::string& effectName) override;
    void HideKillCard() override;
private:
    std::shared_ptr<IKillCardRenderer> m_renderer;
    KillCardData m_lastCard;
    bool m_panelVisible = false;
};
