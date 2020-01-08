#ifndef __KF_SMITHY_WEAPON_CONFIG_H__
#define __KF_SMITHY_WEAPON_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFSmithyWeaponSetting : public KFIntSetting
    {
    public:
        // 武器
        KFElements _item;

        // 需要材料数量
        uint32 _need_num = 0u;

        // 打造消耗
        std::string _str_consume;
        KFElements _consume;
    };

    class KFSmithyWeaponConfig : public KFConfigT< KFSmithyWeaponSetting >, public KFInstance< KFSmithyWeaponConfig >
    {
    public:
        KFSmithyWeaponConfig()
        {
            _file_name = "smithyweapon";
        }

        virtual void LoadAllComplete();

    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFSmithyWeaponSetting* kfsetting );
    };
}

#endif