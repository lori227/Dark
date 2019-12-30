#include "KFInitialProcessConfig.hpp"

namespace KFrame
{
    void KFInitialProcessConfig::ReadSetting( KFNode& xmlnode, KFInitialProcessSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_parameter = xmlnode.GetUInt32( "parameter1" );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}