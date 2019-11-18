#ifndef __KF_SMITHY_CONFIG_H__
#define __KF_SMITHY_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFSmithySetting : public KFIntSetting
    {
    public:
        // 材料id
        uint32 _item_id = 0u;

        // 间隔时间
        uint64 _cd_time = 0u;

        // 增加数量
        uint32 _add_num = 0u;

        // 收集上限
        uint32 _collect_max = 0u;

        // 储存上限
        uint32 _store_max = 0u;

        // 打造上限
        uint32 _make_max = 0u;
    };

    class KFSmithyConfig : public KFConfigT< KFSmithySetting >, public KFInstance< KFSmithyConfig >
    {
    public:
        KFSmithyConfig()
        {
            _file_name = "smithy";
        }

    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFSmithySetting* kfsetting );
    };
}

#endif