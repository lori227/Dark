#include "KFWeaponTypeConfig.hpp"

namespace KFrame
{
    void KFWeaponTypeConfig::ReadSetting( KFNode& xmlnode, KFWeaponTypeSeting* kfsetting )
    {
        kfsetting->_default_weapon_id = xmlnode.GetUInt32( "WeaponDefault" );
    }
}