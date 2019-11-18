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
        kfsetting->_level_type = KFUtility::SplitValue<uint32>( strlevel, __DOMAIN_STRING__ );
        kfsetting->_level_value = KFUtility::SplitValue<int32>( strlevel, __DOMAIN_STRING__ );

        // character
        auto strcharacter = xmlnode.GetString( "RandCharacter", true );
        kfsetting->_character_pool_list = KFUtility::SplitList< VectorUInt32 >( strcharacter, __SPLIT_STRING__ );

        // skill
        auto strskillline = xmlnode.GetString( "RandSkill" );
        while ( !strskillline.empty() )
        {
            auto strskill = KFUtility::SplitString( strskillline, __SPLIT_STRING__ );
            if ( !strskill.empty() )
            {
                auto skillpoolid = KFUtility::SplitValue<uint32>( strskill, __DOMAIN_STRING__ );
                auto weight = KFUtility::SplitValue<uint32>( strskill, __DOMAIN_STRING__ );
                kfsetting->_rand_skill_list.Create( skillpoolid, weight );
            }
        }

        // san
        auto strsanline = xmlnode.GetString( "RandDip" );
        while ( !strsanline.empty() )
        {
            auto strsan = KFUtility::SplitString( strsanline, __SPLIT_STRING__ );
            if ( !strsan.empty() )
            {
                auto minvalue = KFUtility::SplitValue<uint32>( strsan, __RANGE_STRING__ );
                auto maxvalue = KFUtility::SplitValue<uint32>( strsan, __DOMAIN_STRING__ );
                auto weight = KFUtility::SplitValue<uint32>( strsan, __DOMAIN_STRING__ );
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