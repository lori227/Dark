#ifndef __KF_INNER_WORLD_CONFIG_H__
#define __KF_INNER_WORLD_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFInnerWorldSetting : public KFIntSetting
    {
    public:
        // 减少回复效果(万分比)
        uint32 _reduced_treatment = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFInnerWorldConfig : public KFConfigT< KFInnerWorldSetting >, public KFInstance< KFInnerWorldConfig >
    {
    public:
        KFInnerWorldConfig()
        {
            _file_name = "innerworld";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFInnerWorldSetting* kfsetting );
    };
}

#endif