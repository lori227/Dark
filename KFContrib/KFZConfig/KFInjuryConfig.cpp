#include "KFInjuryConfig.hpp"

namespace KFrame
{
    void KFInjuryConfig::ReadSetting( KFNode& xmlnode, KFInjurySetting* kfsetting )
    {
        auto level = xmlnode.GetUInt32( "Level", true );
        if ( level > kfsetting->_max_level )
        {
            kfsetting->_max_level = level;
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}