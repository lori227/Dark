#ifndef __KF_RECRUIT_REFRESH_CONFIG_H__
#define __KF_RECRUIT_REFRESH_CONFIG_H__

#include "KFCore/KFElement.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFRefreshSetting : public KFIntSetting
    {
    public:
        // 刷新英雄个数
        uint32 _count = 0u;

        // 是否清空
        bool _is_clear = false;

        // 显示通知
        uint32 _display_id = 0u;

        // 刷新花费
        KFElements _cost_elements;

        // 英雄生成
        KFElements _hero_generate;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFRefreshConfig : public KFConfigT< KFRefreshSetting >, public KFInstance< KFRefreshConfig >
    {
    public:
        KFRefreshConfig()
        {
            _file_name = "recruit";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRefreshSetting* kfsetting );
    };
}

#endif