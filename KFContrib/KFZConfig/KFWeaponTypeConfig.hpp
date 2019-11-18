#ifndef __KF_WEAPON_TYPE_CONFIG_H__
#define __KF_WEAPON_TYPE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFWeaponTypeSeting : public KFIntSetting
    {
    public:
        // 默认武器id
        uint32 _default_weapon_id = 0u;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFWeaponTypeConfig : public KFConfigT< KFWeaponTypeSeting >, public KFInstance< KFWeaponTypeConfig >
    {
    public:
        KFWeaponTypeConfig()
        {
            _file_name = "weapontype";
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFWeaponTypeSeting* kfsetting );
    };
}

#endif