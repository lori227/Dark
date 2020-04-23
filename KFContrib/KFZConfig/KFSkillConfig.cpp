#include "KFSkillConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFSkillConfig::ReadSetting( KFNode& xmlnode, KFSkillSetting* kfsetting )
    {
        kfsetting->_race_list = xmlnode.GetUInt32Set( "RaceLimit", true );
        kfsetting->_profession_list = xmlnode.GetUInt32Set( "ProfessionLimit", true );
        kfsetting->_background_list = xmlnode.GetUInt32Set( "BackGroundLimit", true );
        kfsetting->_weapon_type_list = xmlnode.GetUInt32Set( "WeaponLimit", true );
        kfsetting->_quality = xmlnode.GetUInt32( "Quality", true );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFSkillSetting::IsValid( uint32 race, uint32 profession, uint32 background, uint32 weapontype, uint32 minquality, uint32 maxquality ) const
    {
        if ( !_race_list.empty() )
        {
            if ( _race_list.find( race ) == _race_list.end() )
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

        if ( !_background_list.empty() && background != 0u )
        {
            if ( _background_list.find( background ) == _background_list.end() )
            {
                return false;
            }
        }

        if ( !_weapon_type_list.empty() && weapontype != 0u )
        {
            if ( _weapon_type_list.find( weapontype ) == _weapon_type_list.end() )
            {
                return false;
            }
        }

        if ( minquality != 0 && maxquality != 0 )
        {
            if ( _quality < minquality || _quality > maxquality )
            {
                return false;
            }
        }

        return true;
    }
}