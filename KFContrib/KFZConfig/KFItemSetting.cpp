﻿#include "KFItemSetting.hpp"
#include "KFItemTypeConfig.hpp"

namespace KFrame
{
    bool KFItemSetting::CheckCanUse() const
    {
        return _use_count > 0;
    }

    bool KFItemSetting::IsRealItem() const
    {
        switch ( _type )
        {
        case KFItemEnum::Script:
            return false;
            break;
        default:
            break;
        }

        return true;
    }

    bool KFItemSetting::IsAutoUse() const
    {
        return _auto_use != 0u;
    }

    const std::string& KFItemSetting::GetFunction( uint32 type ) const
    {
        if ( type >= KFItemEnum::MaxFunction )
        {
            return _invalid_string;
        }

        return _function[ type ];
    }

    bool KFItemSetting::IsOverlay() const
    {
        return _overlay_count > 1u;
    }

    uint32 KFItemSetting::GetOverlayCount( const std::string& bagname ) const
    {
        auto iter = _overlay_count_list.find( bagname );
        if ( iter == _overlay_count_list.end() )
        {
            return _overlay_count;
        }

        return ( uint32 )iter->second;
    }
}
