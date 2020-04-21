#ifndef __KF_INJURY_RANDOM_CONFIG_H__
#define __KF_INJURY_RANDOM_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFInjuryRandomSetting : public KFIntSetting
    {
    public:
        // 伤病随机池
        uint32 _pool = 0u;

        // 死亡概率(万分比)
        uint32 _dead_prob = 0u;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFInjuryRandomConfig : public KFConfigT< KFInjuryRandomSetting >, public KFInstance< KFInjuryRandomConfig >
    {
    public:
        KFInjuryRandomConfig()
        {
            _file_name = "injuryrandom";
        }

        // 获取伤病随机配置
        const KFInjuryRandomSetting* FindInjureRandomSetting( uint32 id );

    protected:
        // 清空配置
        virtual void ClearSetting();

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFInjuryRandomSetting* kfsetting );

    private:
        // 伤病列表
        std::map<uint32, KFInjuryRandomSetting*> _injure_map;
    };
}

#endif