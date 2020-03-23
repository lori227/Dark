#include "KFProfessionConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFProfessionConfig::ReadSetting( KFNode& xmlnode, KFProfessionSetting* kfsetting )
    {
        auto strracelimit = xmlnode.GetString( "RaceLimit", true );
        KFUtility::SplitSet( kfsetting->_race_list, strracelimit, __SPLIT_STRING__ );

        kfsetting->_sex_limit = xmlnode.GetUInt32( "SexLimit" );

        kfsetting->_move_type = xmlnode.GetUInt32( "MoveType" );
        kfsetting->_max_level = xmlnode.GetUInt32( "MaxLevel" );
        kfsetting->_class_lv = xmlnode.GetUInt32( "ClassLv" );

        // 武器类型
        auto strweapon = xmlnode.GetString( "WeaponType" );
        KFUtility::SplitSet( kfsetting->_weapon_type_list, strweapon, __SPLIT_STRING__ );

        // 音色类型
        auto strvoice = xmlnode.GetString( "VoiceType" );
        KFUtility::SplitList( kfsetting->_voice_list, strvoice, __SPLIT_STRING__ );

        // 属性偏好率
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

        auto strdurability = xmlnode.GetString( "RoleDurability" );
        UInt32Vector durabilitylist;
        KFUtility::ParseArrayList( durabilitylist, strdurability );
        if ( durabilitylist.size() == 2u )
        {
            if ( durabilitylist[0] == 0u || durabilitylist[1] == 0u || durabilitylist[0] > durabilitylist[1] )
            {
                return __LOG_ERROR__( "Profession = [{}] RoleDurability is error", kfsetting->_id );
            }

            kfsetting->_min_durability = durabilitylist[0];
            kfsetting->_max_durability = durabilitylist[1];
        }
        else
        {
            return __LOG_ERROR__( "Profession = [{}] RoleDurability is error", kfsetting->_id );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////
    bool KFProfessionSetting::IsValid( uint32 race, uint32 sex, uint32 movetype, uint32 weapontype ) const
    {
        // 种族限制
        if ( race != 0u )
        {
            if ( !_race_list.empty() )
            {
                if ( _race_list.find( race ) == _race_list.end() )
                {
                    return false;
                }
            }
        }

        // 性别限制
        if ( sex != 0u )
        {
            if ( _sex_limit != 0u )
            {
                if ( _sex_limit != sex )
                {
                    return false;
                }
            }
        }

        // 行动方式
        if ( movetype != 0u )
        {
            if ( movetype != _move_type )
            {
                return false;
            }
        }

        // 武器类型
        if ( weapontype != 0u )
        {
            if ( _weapon_type_list.find( weapontype ) == _weapon_type_list.end() )
            {
                return false;
            }
        }

        return true;
    }

    uint32 KFProfessionSetting::GetAttributeValue( const std::string& name ) const
    {
        auto iter = _attribute.find( name );
        if ( iter == _attribute.end() )
        {
            return 0u;
        }

        return iter->second;
    }

    bool KFProfessionSetting::IsWeaponTypeValid( uint32 weapontype ) const
    {
        if ( _weapon_type_list.find( weapontype ) == _weapon_type_list.end() )
        {
            return false;
        }

        return true;
    }

    uint32 KFProfessionSetting::RandWeapontype() const
    {
        auto size = _weapon_type_list.size();
        if ( size == 0u )
        {
            return 0u;
        }

        auto index = KFGlobal::Instance()->RandRatio( ( uint32 )size );
        auto iter = _weapon_type_list.begin();
        std::advance( iter, index );

        return *iter;
    }

    uint32 KFProfessionSetting::RandRoleDurability() const
    {
        return KFGlobal::Instance()->RandRange( _min_durability, _max_durability, 1u );
    }
}