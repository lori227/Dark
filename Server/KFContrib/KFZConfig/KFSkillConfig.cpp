#include "KFSkillConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFSkillConfig::ReadSetting( KFNode& xmlnode, KFSkillSetting* kfsetting )
    {
        auto strrace = xmlnode.GetString( "RaceLimit", true );
        if ( !strrace.empty() )
        {
            kfsetting->_race_list = KFUtility::SplitSet< std::set<uint32> >( strrace, DEFAULT_SPLIT_STRING );
        }

        auto strprofession = xmlnode.GetString( "ProfessionLimit", true );
        if ( !strprofession.empty() )
        {
            kfsetting->_profession_list = KFUtility::SplitSet< std::set<uint32> >( strprofession, DEFAULT_SPLIT_STRING );
        }

        auto strbackground = xmlnode.GetString( "BackGroundLimit", true );
        if ( !strbackground.empty() )
        {
            kfsetting->_background_list = KFUtility::SplitSet< std::set<uint32> >( strbackground, DEFAULT_SPLIT_STRING );
        }

        auto strweapon = xmlnode.GetString( "WeaponLimit", true );
        if ( !strweapon.empty() )
        {
            kfsetting->_weapon_type_list = KFUtility::SplitSet< std::set<uint32> >( strweapon, DEFAULT_SPLIT_STRING );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFSkillSetting::IsValid( uint32 race, uint32 profession, uint32 background, uint32 weapontype ) const
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

        if ( !_weapon_type_list.empty() )
        {
            if ( _weapon_type_list.find( weapontype ) == _weapon_type_list.end() )
            {
                return false;
            }
        }

        return true;
    }
}