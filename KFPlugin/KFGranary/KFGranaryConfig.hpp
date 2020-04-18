#ifndef __KF_GRANARY_CONFIG_H__
#define __KF_GRANARY_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFSetting.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFGranarySetting : public KFIntSetting
    {
    public:
        // 消耗
        KFElements _consume;
    };

    class KFGranaryConfig : public KFConfigT< KFGranarySetting >, public KFInstance< KFGranaryConfig >
    {
    public:
        KFGranaryConfig()
        {
            _file_name = "granarybuy";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFGranarySetting* kfsetting );
    };
}

#endif