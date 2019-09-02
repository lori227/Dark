#ifndef __KF_EXPLORE_CONFIG_H__
#define __KF_EXPLORE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFExploreSeting : public KFStrSetting
    {
    public:
        // 成功道具掉落
        uint32 _victory_item_drop_id = 0u;

        // 失败道具掉落
        uint32 _fail_item_drop_id = 0u;

        // 成功英雄经验掉落
        uint32 _victory_exp_drop_id = 0u;

        // 失败英雄经验掉落
        uint32 _fail_exp_drop_id = 0u;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFExploreConfig : public KFStrConfigT< KFExploreSeting >, public KFInstance< KFExploreConfig >
    {
    public:
        KFExploreConfig()
        {
            _file_name = "explore";
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFExploreSeting* kfsetting );
    };
}

#endif