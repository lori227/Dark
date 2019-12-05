﻿#include "KFGenerateConfig.hpp"

namespace KFrame
{
    void KFGenerateConfig::ReadSetting( KFNode& xmlnode, KFGenerateSetting* kfsetting )
    {
        kfsetting->_race_pool_id = xmlnode.GetUInt32( "RacePool" );
        kfsetting->_sex_pool_id = xmlnode.GetUInt32( "SexPool" );
        kfsetting->_profession_pool_id = xmlnode.GetUInt32( "ProfessionPool" );
        kfsetting->_background_pool_id = xmlnode.GetUInt32( "BackGroundPool" );
        kfsetting->_attr_id = xmlnode.GetUInt32( "AttrId" );
        kfsetting->_growth_id = xmlnode.GetUInt32( "GrowthId" );

        auto strcharacter = xmlnode.GetString( "CharacterPool" );
        kfsetting->_character_pool_list = KFUtility::SplitList< VectorUInt32>( strcharacter, __SPLIT_STRING__ );

        auto strinnate = xmlnode.GetString( "InnatePool" );
        kfsetting->_innate_pool_list = KFUtility::SplitList< VectorUInt32 >( strinnate, __SPLIT_STRING__ );

        auto stractive = xmlnode.GetString( "ActivePool" );
        kfsetting->_active_pool_list = KFUtility::SplitList< VectorUInt32 >( stractive, __SPLIT_STRING__ );
    }
}