#include "KFDivisorConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFDivisorConfig::ReadSetting( KFNode& xmlnode, KFDivisorSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_value = xmlnode.GetUInt32( "Value" );
        kfsetting->_max_count = xmlnode.GetUInt32( "MaxCount" );
        kfsetting->_probability = xmlnode.GetUInt32( "Probability" );
    }
}