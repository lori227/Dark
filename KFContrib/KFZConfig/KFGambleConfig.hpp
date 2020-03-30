#ifndef __KF_GAMBLE_CONFIG_H__
#define __KF_GAMBLE_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFGambleSetting : public KFIntSetting
    {
    public:
        // 消耗道具
        uint32 _cost_item_id = 0u;
        KFElements _cost_item;

        // 初始数量
        uint32 _init_count = 0u;

        //  最大数量
        uint32 _max_count = 0u;

        // 参数列表
        UInt32Vector _params;

        // 显示道具的数量
        uint32 _show_count = 0u;

        // 掉落类型
        uint32 _drop_type = 0u;

        // 掉落
        uint32 _normal_drop_id = 0u;
        uint32 _inner_drop_id = 0u;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFGambleConfig : public KFConfigT< KFGambleSetting >, public KFInstance< KFGambleConfig >
    {
    public:
        KFGambleConfig()
        {
            _file_name = "gamble";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFGambleSetting* kfsetting );
    };
}

#endif