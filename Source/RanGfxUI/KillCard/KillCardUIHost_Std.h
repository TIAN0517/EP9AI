#pragma once
#include "IKillCardUIHost.h"
#include <memory>

// 正式UI實現：可連接渲染層、動畫、特效等
class KillCardUIHost_Std : public IKillCardUIHost {
public:
    KillCardUIHost_Std();
    ~KillCardUIHost_Std();
    void ShowKillCard(const KillCardData& data) override;
    void PlayKillAnimation(const std::string& animName) override;
    void ShowEffect(const std::string& effectName) override;
    void HideKillCard() override;
private:
    // 這裡可持有渲染/動畫/特效等子系統指針
    void RenderPanel(const KillCardData& data);
    void PlayAnimation(const std::string& animName);
    void PlayEffect(const std::string& effectName);
    void ClearPanel();
};
