﻿#include "KFTechnologyConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFTechnologyConfig::ReadSetting( KFNode& xmlnode, KFTechnologySetting* kfsetting )
    {
        kfsetting->_status = xmlnode.GetUInt32( "Status" );
        kfsetting->_build_id = xmlnode.GetUInt32( "BuildingId" );
        kfsetting->_type = xmlnode.GetUInt32( "Type" );

        kfsetting->_pre_technology = xmlnode.GetUInt32Vector( "Condition", true );
        kfsetting->_unlock_technology = xmlnode.GetUInt32Vector( "UnlockId", true );

        kfsetting->_consume._str_parse = xmlnode.GetString( "Consume" );
        KFReadSetting::ReadExecuteData( xmlnode, &kfsetting->_execute_data );
    }

    void KFTechnologyConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->ParseElement( kfsetting->_consume, __FILE__, kfsetting->_id );
        }
    }
}