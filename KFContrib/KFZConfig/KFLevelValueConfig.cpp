#include "KFLevelValueConfig.hpp"

namespace KFrame
{
    void KFLevelValueConfig::ReadSetting( KFNode& xmlnode, KFLevelValueSetting* kfsetting )
    {
        auto maxlevel = xmlnode.GetUInt32( "Level" );
        if ( maxlevel > kfsetting->_max_level )
        {
            kfsetting->_max_level = maxlevel;
        }
    }
}