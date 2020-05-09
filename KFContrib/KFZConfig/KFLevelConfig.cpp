#include "KFLevelConfig.hpp"

namespace KFrame
{
    void KFLevelConfig::ReadSetting( KFNode& xmlnode, KFLevelSetting* kfsetting )
    {
        kfsetting->_exp = xmlnode.GetUInt32( "Exp", true );
        kfsetting->_floor_attribute = xmlnode.GetUInt32( "Floor", true );
        kfsetting->_upper_attribute = xmlnode.GetUInt32( "Upper", true );
        kfsetting->_active_pool_list = xmlnode.GetUInt32Vector( "ActivePool", true );
        kfsetting->_innate_pool_list = xmlnode.GetUInt32Vector( "InnatePool", true );
    }
}