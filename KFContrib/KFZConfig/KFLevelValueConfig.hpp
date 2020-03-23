#ifndef __KF_LEVEL_VALUE_CONFIG_H__
#define __KF_LEVEL_VALUE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFLevelValueSetting : public KFIntSetting
    {
    public:
        // 最大等级
        uint32 _max_level = 0u;
    };

    class KFLevelValueConfig : public KFConfigT< KFLevelValueSetting >, public KFInstance< KFLevelValueConfig >
    {
    public:
        KFLevelValueConfig()
        {
            _key_name = "Skillid";
            _file_name = "levelvalue";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFLevelValueSetting* kfsetting );
    };
}

#endif