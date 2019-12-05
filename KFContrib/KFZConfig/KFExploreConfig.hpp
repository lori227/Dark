﻿#ifndef __KF_EXPLORE_CONFIG_H__
#define __KF_EXPLORE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFExploreSeting : public KFIntSetting
    {
    public:
        // 成功道具掉落
        VectorUInt32 _victory_drop_list;

        // 失败道具掉落
        VectorUInt32 _fail_drop_list;
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
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFExploreSeting* kfsetting );
    };
}

#endif