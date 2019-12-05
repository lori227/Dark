﻿#include "KFTaskConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFTaskConfig::ReadSetting( KFNode& xmlnode, KFTaskSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_active_status = xmlnode.GetUInt32( "Active" );
        kfsetting->_complete_type = xmlnode.GetUInt32( "CompleteMode" );

        auto strprecondition = xmlnode.GetString( "PreCondition" );
        kfsetting->_pre_condition_type = KFReadSetting::ParseConditionList( strprecondition, kfsetting->_pre_condition );

        auto strplacecondition = xmlnode.GetString( "PlaceCondition" );
        kfsetting->_place_condition.Parse( strplacecondition, 0, __FUNC_LINE__ );

        auto strcompletecondition = xmlnode.GetString( "CompleteCondition" );
        kfsetting->_complete_condition_type = KFReadSetting::ParseConditionList( strcompletecondition, kfsetting->_complete_condition );

        KFReadSetting::ReadExecuteData( xmlnode, &kfsetting->_execute_data );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_execute_data._name != __STRING__( data ) )
            {
                continue;
            }

            if ( kfsetting->_execute_data._param_list._params.size() < 3u )
            {
                continue;
            }

            auto& dataname = kfsetting->_execute_data._param_list._params[ 0 ]->_str_value;
            auto& datavalue = kfsetting->_execute_data._param_list._params[ 1 ]->_str_value;
            auto& datakey = kfsetting->_execute_data._param_list._params[ 2 ]->_str_value;
            auto kfelementsetting = KFElementConfig::Instance()->FindElementSetting( dataname );
            auto strelement = __FORMAT__( kfelementsetting->_element_template, dataname, datavalue, datakey );
            kfsetting->_execute_data._param_list._elements.Parse( strelement, __FUNC_LINE__ );
        }
    }
}