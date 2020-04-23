#ifndef __KF_QUALITY_CONFIG_H__
#define __KF_QUALITY_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFQualitySetting : public KFIntSetting
    {
    public:
        // 天赋上限
        uint32 _innate_num = 0u;

        // 天赋品质区间
        KFRange<uint32> _innate_quality;

        // 主动技能品质区间
        KFRange<uint32> _active_quality;

        // 契约时长
        KFRange<uint32> _role_durability;

        // 属性成长率
        KFHashMap< std::string, const std::string&, KFRange<uint32> > _growth;
    public:
        // 是否有效
        bool IsValid() const
        {
            return true;
        }
    };

    class KFQualityConfig : public KFConfigT< KFQualitySetting >, public KFInstance< KFQualityConfig >
    {
    public:
        KFQualityConfig()
        {
            _file_name = "heroquality";
        }

    protected:

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFQualitySetting* kfsetting );
    };
}

#endif