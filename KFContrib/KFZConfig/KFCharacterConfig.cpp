﻿#include "KFCharacterConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFCharacterConfig::ReadSetting( KFNode& xmlnode, KFCharacterSetting* kfsetting )
    {
        auto strrace = xmlnode.GetString( "RaceLimit", true );
        if ( !strrace.empty() )
        {
            KFUtility::SplitSet( kfsetting->_race_list, strrace, __SPLIT_STRING__ );
        }

        auto strbackground = xmlnode.GetString( "BackGroundLimit", true );
        if ( !strbackground.empty() )
        {
            KFUtility::SplitSet( kfsetting->_background_list, strbackground, __SPLIT_STRING__ );
        }

        auto strprofession = xmlnode.GetString( "ProfessionLimit", true );
        if ( !strprofession.empty() )
        {
            KFUtility::SplitSet( kfsetting->_profession_list, strprofession, __SPLIT_STRING__ );
        }
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