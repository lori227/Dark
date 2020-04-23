#include "KFBackGroundConfig.hpp"

namespace KFrame
{
    void KFBackGroundConfig::ReadSetting( KFNode& xmlnode, KFBackGroundSetting* kfsetting )
    {
        kfsetting->_race_list = xmlnode.GetUInt32Set( "RaceLimit", true );
        kfsetting->_sex_list = xmlnode.GetUInt32Set( "SexLimit", true );
    }
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    bool KFBackGroundSetting::IsValid( uint32 race, uint32 sex ) const
    {
        if ( !_race_list.empty() )
        {
            if ( _race_list.find( race ) == _race_list.end() )
            {
                return false;
            }
        }

        if ( !_sex_list.empty() )
        {
            if ( _sex_list.find( sex ) == _sex_list.end() )
            {
                return false;
            }
        }

        return true;
    }
}