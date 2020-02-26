#include "KFSelectConfig.hpp"

namespace KFrame
{
    void KFSelectConfig::ReadSetting( KFNode& xmlnode, KFSelectSetting* kfsetting )
    {
        kfsetting->_count = xmlnode.GetUInt32( "Count" );
        kfsetting->_normal_drop_id = xmlnode.GetUInt32( "NormalDrop" );
        kfsetting->_inner_drop_id = xmlnode.GetUInt32( "InnerDrop" );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}