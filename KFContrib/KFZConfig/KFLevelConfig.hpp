#ifndef __KF_LEVEL_CONFIG_H__
#define __KF_LEVEL_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFLevelSetting : public KFIntSetting
    {
    public:
        // 总经验
        uint32 _exp = 0u;

        // 属性下限偏移
        uint32 _floor_attribute = 0u;

        // 属性上限偏移
        uint32 _upper_attribute = 0u;

        // 主动技能
        UInt32Vector _active_pool_list;

        // 天赋
        UInt32Vector _innate_pool_list;
    };

    class KFLevelConfig : public KFConfigT< KFLevelSetting >, public KFInstance< KFLevelConfig >
    {
    public:
        KFLevelConfig()
        {
            _file_name = "herolevel";
        }

    protected:

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFLevelSetting* kfsetting );
    };
}

#endif