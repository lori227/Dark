#ifndef __KF_GRANARY_CONFIG_H__
#define __KF_GRANARY_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFGranarySetting : public KFIntSetting
    {
    public:
        // 间隔时间
        uint64 _cd_time = 0u;

        // 增加数量
        uint32 _add_num = 0u;

        // 最大数量
        uint32 _max_num = 0u;
    };

    class KFGranaryConfig : public KFConfigT< KFGranarySetting >, public KFInstance< KFGranaryConfig >
    {
    public:
        KFGranaryConfig()
        {
            _file_name = "granary";
        }

    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFGranarySetting* kfsetting );
    };
}

#endif