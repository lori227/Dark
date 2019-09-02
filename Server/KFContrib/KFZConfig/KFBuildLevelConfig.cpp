#include "KFBuildLevelConfig.hpp"

namespace KFrame
{
    uint32 KFBuildLevelConfig::GetBuildUnlockLevel( uint32 id, uint32 level )
    {
        auto setting = FindSetting( id );
        if ( setting->_unlock_level == 0u )
        {
            return 1u;
        }

        return setting->_unlock_level > level ? 0u : 1u;
    }

    void KFBuildLevelConfig::ReadSetting( KFNode& xmlnode, KFBuildLevelSetting* kfsetting )
    {
        kfsetting->_unlock_level = xmlnode.GetUInt32( "UnlockLevel", true );
    }
}