#include "KFNpcConfig.hpp"

namespace KFrame
{
    void KFNpcConfig::ReadSetting( KFNode& xmlnode, KFNpcSetting* kfsetting )
    {
        kfsetting->_race_id = xmlnode.GetUInt32( "Race" );
        kfsetting->_profession_id = xmlnode.GetUInt32( "Profession" );
        kfsetting->_weapon_id = xmlnode.GetUInt32( "Weapon", true );
        kfsetting->_sex = xmlnode.GetUInt32( "Sex", true );
        kfsetting->_attr_id = xmlnode.GetUInt32( "RandAttr", true );

        // level
        auto strlevel = xmlnode.GetString( "Level" );
        kfsetting->_level_type = KFUtility::SplitValue<uint32>( strlevel, FUNCTION_LINK_STRING );
        kfsetting->_level_value = KFUtility::SplitValue<int32>( strlevel, FUNCTION_LINK_STRING );

        // character
        auto strcharacter = xmlnode.GetString( "RandCharacter", true );
        kfsetting->_character_pool_list = KFUtility::SplitList< std::list< uint32 > >( strcharacter, DEFAULT_SPLIT_STRING );

        // skill
        auto strskillline = xmlnode.GetString( "RandSkill" );
        while ( !strskillline.empty() )
        {
            auto strskill = KFUtility::SplitString( strskillline, DEFAULT_SPLIT_STRING );
            if ( !strskill.empty() )
            {
                auto skillpoolid = KFUtility::SplitValue<uint32>( strskill, FUNCTION_LINK_STRING );
                auto weight = KFUtility::SplitValue<uint32>( strskill, FUNCTION_LINK_STRING );
                kfsetting->_rand_skill_list.Create( skillpoolid, weight );
            }
        }

        // san
        auto strsanline = xmlnode.GetString( "RandDip" );
        while ( !strsanline.empty() )
        {
            auto strsan = KFUtility::SplitString( strsanline, DEFAULT_SPLIT_STRING );
            if ( !strsan.empty() )
            {
                auto minvalue = KFUtility::SplitValue<uint32>( strsan, FUNCTION_RANGE_STRING );
                auto maxvalue = KFUtility::SplitValue<uint32>( strsan, FUNCTION_LINK_STRING );
                auto weight = KFUtility::SplitValue<uint32>( strsan, FUNCTION_LINK_STRING );
                auto kfweight = kfsetting->_rand_dip_list.Create( 0, weight );
                kfweight->_min_value = minvalue;
                kfweight->_max_value = maxvalue;
            }
        }

        // 定制属性列表
        std::list< std::string > keylist;
        xmlnode.GetKeyList( keylist );
        for ( auto& key : keylist )
        {
            auto attrname = key;
            KFUtility::SplitString( attrname, "_" );
            if ( !attrname.empty() )
            {
                uint32 value = xmlnode.GetUInt32( key.c_str(), true );
                kfsetting->_attribute[ attrname ] = value;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    enum KFNpclevelEnum
    {
        FixedLevel = 1,
        TaskLevel = 2,
    };

    uint32 KFNpcSetting::CalcNpcLevel( uint32 level ) const
    {
        switch ( _level_type )
        {
        case KFNpclevelEnum::FixedLevel:
            level = _level_value;
            break;
        case KFNpclevelEnum::TaskLevel:
            if ( _level_value >= 0 )
            {
                level += _level_value;
            }
            else
            {
                uint32 levelvalue = abs( _level_value );
                level -= __MIN__( level, levelvalue );
            }
            break;
        default:
            break;
        }

        return level;
    }
}