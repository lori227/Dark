#include "KFPinchFaceConfig.hpp"

namespace KFrame
{
    void KFPinchFaceConfig::ReadSetting( KFNode& xmlnode, KFPinchFaceSetting* kfsetting )
    {
        kfsetting->_name = xmlnode.GetString( "Name", true );
        kfsetting->_range.SetValue( xmlnode.GetDouble( "MinValue" ), xmlnode.GetDouble( "MaxValue" ) );
    }
    /////////////////////////////////////////////////////////////////////////////////

}