#include "KFCharacterConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFCharacterConfig::ReadSetting( KFNode& xmlnode, KFCharacterSetting* kfsetting )
    {
        kfsetting->_race_list = xmlnode.GetUInt32Set( "RaceLimit", true );
        kfsetting->_background_list = xmlnode.GetUInt32Set( "BackGroundLimit", true );
        kfsetting->_profession_list = xmlnode.GetUInt32Set( "ProfessionLimit", true );
    }
    /////////////////////////////////////////////////////////////////////////////////
    bool KFCharacterSetting::IsValid( uint32 race, uint32 background, uint32 profession ) const
    {
        if ( !_race_list.empty() )
        {
            if ( _race_list.find( race ) == _race_list.end() )
            {
                return false;
            }
        }

        if ( !_background_list.empty() && background != 0u )
        {
            if ( _background_list.find( background ) == _background_list.end() )
            {
                return false;
            }
        }

        if ( !_profession_list.empty() )
        {
            if ( _profession_list.find( profession ) == _profession_list.end() )
            {
                return false;
            }
        }

        return true;
    }
}