#include "KFProfessionConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFProfessionConfig::ReadSetting( KFNode& xmlnode, KFProfessionSetting* kfsetting )
    {
        auto strracelimit = xmlnode.GetString( "RaceLimit", true );
        kfsetting->_race_list = KFUtility::SplitSet< std::set<uint32> >( strracelimit, DEFAULT_SPLIT_STRING );

        kfsetting->_move_type = xmlnode.GetUInt32( "MoveType" );
        kfsetting->_max_level = xmlnode.GetUInt32( "MaxLevel" );

        auto strcost = xmlnode.GetString( "RecruitCost" );
        kfsetting->_recruit_cost.Parse( strcost, __FUNC_LINE__ );

        // 武器类型
        auto strweapon = xmlnode.GetString( "WeaponType" );
        kfsetting->_weapon_type_list = KFUtility::SplitSet< std::set< uint32 > >( strweapon, DEFAULT_SPLIT_STRING );

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
    }
    /////////////////////////////////////////////////////////////////////////////////
    bool KFProfessionSetting::IsValid( uint32 race, uint32 movetype, uint32 weapontype ) const
    {
        // 种族限制
        if ( !_race_list.empty() )
        {
            if ( _race_list.find( race ) == _race_list.end() )
            {
                return false;
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
}