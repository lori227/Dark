#include "KFEquipTypeConfig.hpp"

namespace KFrame
{
    void KFEquipTypeConfig::ReadSetting( KFNode& xmlnode, KFEquipTypeSeting* kfsetting )
    {
        kfsetting->_default_id = xmlnode.GetUInt32( "DefaultId", true );
    }
}