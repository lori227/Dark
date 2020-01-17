#include "KFDialogueBranchConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFDialogueBranchConfig::ReadSetting( KFNode& xmlnode, KFDialogueBranchSetting* kfsetting )
    {
        auto conditionstr = xmlnode.GetString( "Condition" );
        kfsetting->_condition_type = KFReadSetting::ParseConditionList( conditionstr, kfsetting->_condition );

        kfsetting->_str_cost = xmlnode.GetString( "Cost" );

        KFReadSetting::ReadExecuteData( xmlnode, &kfsetting->_execute_data );
    }

    void KFDialogueBranchConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_str_cost.empty() )
            {
                KFElementConfig::Instance()->ParseElement( kfsetting->_cost, kfsetting->_str_cost, __FILE__, kfsetting->_id );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}