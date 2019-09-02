#include "KFPinchFaceConfig.hpp"

namespace KFrame
{
    void KFPinchFaceConfig::ReadSetting( KFNode& xmlnode, KFPinchFaceSetting* kfsetting )
    {
        kfsetting->_name = xmlnode.GetString( "Name", true );
        kfsetting->_min_value = xmlnode.GetDouble( "MinValue", true );
        kfsetting->_max_value = xmlnode.GetDouble( "MaxValue", true );
    }
    /////////////////////////////////////////////////////////////////////////////////

}