#ifndef __KF_SELECT_CONFIG_H__
#define __KF_SELECT_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFSelectSetting : public KFIntSetting
    {
    public:
        // 选择数量
        uint32 _count = 0u;

        // 掉落
        uint32 _normal_drop_id = 0u;
        uint32 _inner_drop_id = 0u;
    };

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFSelectConfig : public KFConfigT< KFSelectSetting >, public KFInstance< KFSelectConfig >
    {
    public:
        KFSelectConfig()
        {
            _file_name = "select";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFSelectSetting* kfsetting );
    };
}

#endif