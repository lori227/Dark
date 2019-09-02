#ifndef __KF_EQUIP_TYPE_CONFIG_H__
#define __KF_EQUIP_TYPE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFEquipTypeSeting : public KFIntSetting
    {
    public:
        // 默认武器id
        uint32 _default_id;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFEquipTypeConfig : public KFIntConfigT< KFEquipTypeSeting >, public KFInstance< KFEquipTypeConfig >
    {
    public:
        KFEquipTypeConfig()
        {
            _file_name = "equiptype";
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFEquipTypeSeting* kfsetting );
    };
}

#endif