#ifndef __KF_EXPLORE_CONFIG_H__
#define __KF_EXPLORE_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFExploreSeting : public KFIntSetting
    {
    public:
        // 成功道具掉落
        UInt32Vector _victory_drop_list;

        // 失败道具掉落
        UInt32Vector _fail_drop_list;

        // 进入消耗
        std::string _str_consume;
        KFElements _consume;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFExploreConfig : public KFConfigT< KFExploreSeting >, public KFInstance< KFExploreConfig >
    {
    public:
        KFExploreConfig()
        {
            _file_name = "explore";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFExploreSeting* kfsetting );
    };
}

#endif