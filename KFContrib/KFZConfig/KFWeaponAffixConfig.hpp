#ifndef __KF_WEAPON_AFFIX_CONFIG_H__
#define __KF_WEAPON_AFFIX_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFWeaponAffixSeting : public KFIntSetting
    {
    public:

    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFWeaponAffixConfig : public KFConfigT< KFWeaponAffixSeting >, public KFInstance< KFWeaponAffixConfig >
    {
    public:
        KFWeaponAffixConfig()
        {
            _file_name = "weaponaffix";
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFWeaponAffixSeting* kfsetting );
    };
}

#endif