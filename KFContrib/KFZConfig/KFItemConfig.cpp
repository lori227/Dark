﻿#include "KFItemConfig.hpp"
#include "KFFieldEx.h"

namespace KFrame
{
    void KFItemConfig::ClearSetting()
    {
        _rune_type_level.clear();
        _rune_compound.clear();
        KFConfigT< KFItemSetting >::ClearSetting();
    }

    void KFItemConfig::ReadSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {
        switch ( kfsetting->_type )
        {
        case KFItemEnum::Normal:
            break;
        case KFItemEnum::Gift:
            ReadGiftSetting( xmlnode, kfsetting );
            break;
        case KFItemEnum::Drug:
            ReadDrugSetting( xmlnode, kfsetting );
            break;
        case KFItemEnum::Weapon:
            ReadWeaponSetting( xmlnode, kfsetting );
            break;
        case KFItemEnum::Material:
            ReadMaterialSetting( xmlnode, kfsetting );
            break;
        case KFItemEnum::Rune:
            ReadRuneSetting( xmlnode, kfsetting );
            break;
        case KFItemEnum::Other:
            ReadOtherSetting( xmlnode, kfsetting );
            break;
        case KFItemEnum::Food:
            ReadFoodSetting( xmlnode, kfsetting );
            break;
        default:
            ReadCommonSetting( xmlnode, kfsetting );
            break;
        }
    }

    void KFItemConfig::LoadAllComplete()
    {
        _rune_compound.clear();
        for ( auto iter : _rune_type_level )
        {
            auto nextkey = iter.first + 1u;
            auto nextiter = _rune_type_level.find( nextkey );
            if ( nextiter != _rune_type_level.end() )
            {
                _rune_compound[iter.second] = nextiter->second;
            }
        }

        _rune_type_level.clear();
    }

    void KFItemConfig::ReadCommonSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_quality = xmlnode.GetUInt32( "Quality" );
        kfsetting->_use_count = xmlnode.GetUInt32( "UseCount", true );
        kfsetting->_use_limit = xmlnode.GetUInt32( "UseLimit", true, KFItemEnum::UseInAll );
        kfsetting->_overlay_type = xmlnode.GetUInt32( "OverlayType" );
        kfsetting->_overlay_count = xmlnode.GetUInt32( "OverlayCount" );
        if ( kfsetting->_overlay_count == 0u )
        {
            kfsetting->_overlay_count = __MAX_UINT32__;
        }

        kfsetting->_overlay_count_list.clear();
        auto exploreoverlaycount = xmlnode.GetUInt32( "ExploreOverlayCount" );
        if ( exploreoverlaycount != 0u )
        {
            kfsetting->_overlay_count_list[ __STRING__( explore ) ] = exploreoverlaycount;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////

        auto strsell = xmlnode.GetString( "Sell", true );
        kfsetting->_sell_elements.Parse( strsell, __FUNC_LINE__ );

        kfsetting->_lua_file = xmlnode.GetString( "LuaFile", true );
        auto addfunction = xmlnode.GetString( "AddFunction", true );
        if ( !addfunction.empty() )
        {
            kfsetting->_function[ KFItemEnum::AddFunction ] = addfunction;
        }

        auto usefunction = xmlnode.GetString( "UseFunction", true );
        if ( !usefunction.empty() )
        {
            kfsetting->_function[ KFItemEnum::UseFunction ] = usefunction;
        }

        auto removefunction = xmlnode.GetString( "RemoveFunction", true );
        if ( !removefunction.empty() )
        {
            kfsetting->_function[ KFItemEnum::RemoveFunction ] = removefunction;
        }
    }

    void KFItemConfig::ReadGiftSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {
        kfsetting->_drop_id = xmlnode.GetUInt32( "DropId" );
        kfsetting->_auto_use = xmlnode.GetUInt32( "AutoUse" );

        auto strreward = xmlnode.GetString( "Reward", true );
        if ( !strreward.empty() )
        {
            kfsetting->_reward.Parse( strreward, __FUNC_LINE__ );
        }
    }

    void KFItemConfig::ReadDrugSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {
        kfsetting->_drug_type = xmlnode.GetUInt32( "DrugType" );
        kfsetting->_use_target = xmlnode.GetUInt32( "UseTarget" );

        auto strrestore = xmlnode.GetString( "Restore" );
        __JSON_PARSE_STRING__( kfjson, strrestore );
        for ( auto iter = kfjson.MemberBegin(); iter != kfjson.MemberEnd(); ++iter )
        {
            if ( iter->value.IsNumber() )
            {
                kfsetting->_drug_values[ iter->name.GetString() ] = iter->value.GetInt();
            }
            else
            {
                __LOG_ERROR__( "drug=[{}] values=[{}] not int", kfsetting->_id, iter->name.GetString() );
            }
        }
    }

    void KFItemConfig::ReadWeaponSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {
        kfsetting->_weapon_type = xmlnode.GetUInt32( "WeaponType" );
        kfsetting->_weapon_level = xmlnode.GetUInt32( "WeaponLeve" );
        kfsetting->_level_limit = xmlnode.GetUInt32( "LevelLimit", true );
        kfsetting->_durability = xmlnode.GetUInt32( "Durability", true );
        kfsetting->_durability_cost = xmlnode.GetUInt32( "Cost", true );

        auto straffix = xmlnode.GetString( "AffixPool" );
        KFUtility::SplitList( kfsetting->_affix_pool_list, straffix, __SPLIT_STRING__ );

        auto strracelimit = xmlnode.GetString( "RaceLimit" );
        KFUtility::SplitSet( kfsetting->_race_limit_list, strracelimit, __SPLIT_STRING__ );
    }

    void KFItemConfig::ReadMaterialSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {

    }

    void KFItemConfig::ReadOtherSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {

    }

    void KFItemConfig::ReadRuneSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {
        if ( xmlnode.GetUInt32( "AutoSet", true ) == 1u )
        {
            kfsetting->_auto_set = true;
        }

        kfsetting->_rune_type = xmlnode.GetUInt32( "RuneType", true );
        kfsetting->_rune_level = xmlnode.GetUInt32( "RuneLevel" );

        if ( kfsetting->_rune_type == 0u )
        {
            return;
        }

        auto key = kfsetting->_rune_type * 10000u + kfsetting->_rune_level;
        _rune_type_level[key] = static_cast<uint32>( kfsetting->_id );
    }

    void KFItemConfig::ReadFoodSetting( KFNode& xmlnode, KFItemSetting* kfsetting )
    {

    }

    uint32 KFItemConfig::GetRuneCompoundId( uint32 id )
    {
        auto iter = _rune_compound.find( id );
        if ( iter != _rune_compound.end() )
        {
            return iter->second;
        }

        return _invalid_int;
    }
}