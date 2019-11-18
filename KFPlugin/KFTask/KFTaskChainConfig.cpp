#include "KFTaskChainConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    const KFTaskData* KFTaskChainSetting::FindTaskData( uint32 order, uint32 taskid ) const
    {
        auto eventlist = _task_data_list.Find( order );
        if ( eventlist == nullptr )
        {
            return nullptr;
        }

        for ( auto eventdata : eventlist->_weight_data )
        {
            if ( eventdata->_id == taskid )
            {
                return eventdata;
            }
        }

        return nullptr;
    }

    void KFTaskChainConfig::ReadSetting( KFNode& xmlnode, KFTaskChainSetting* kfsetting )
    {
        auto order = xmlnode.GetUInt32( "Order" );
        auto taskdatalist = kfsetting->_task_data_list.Find( order );
        if ( taskdatalist == nullptr )
        {
            taskdatalist = __KF_NEW__( KFWeightList< KFTaskData > );
            kfsetting->_task_data_list.Insert( order, taskdatalist );
        }

        auto taskid = xmlnode.GetUInt32( "Task" );
        auto weight = xmlnode.GetUInt32( "Weight", true );
        auto taskdata = taskdatalist->Create( taskid, ( weight == 0u ? 100u : weight ) );

        taskdata->_task_status = xmlnode.GetUInt32( "TaskStatus" );
        taskdata->_logic_name = xmlnode.GetString( "LogicName" );
        taskdata->_logic_status = xmlnode.GetUInt32( "LogicStatus" );
        taskdata->_finish_status = xmlnode.GetUInt32( "FinishStatus" );

        auto strextend = xmlnode.GetString( "ExtendChain" );
        KFReadSetting::ParseMapUInt32( strextend, taskdata->_extend_task_chain_list );

        auto strlogicid = xmlnode.GetString( "LogicId" );
        taskdata->_logic_type = KFReadSetting::ParseConditionList( strlogicid, taskdata->_logic_id_list );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskChainRefreshConfig::ClearSetting()
    {
        _refresh_data_list.Clear();
        KFConfigT< KFTaskChainRefreshSetting >::ClearSetting();
    }

    void KFTaskChainRefreshConfig::ReadSetting( KFNode& xmlnode, KFTaskChainRefreshSetting* kfsetting )
    {
        kfsetting->_refresh_rate = xmlnode.GetUInt32( "Rate" );
        kfsetting->_task_chain_id = xmlnode.GetUInt32( "ChainId" );
        kfsetting->_receive_time = xmlnode.GetUInt32( "PickupTime" );
        kfsetting->_done_time = xmlnode.GetUInt32( "CompleteTime" );
        kfsetting->_str_cost = xmlnode.GetString( "PickupCost" );

        auto timeid = xmlnode.GetUInt32( "RefreshTime" );
        auto kfrefreshdata = _refresh_data_list.Create( timeid );
        kfrefreshdata->_refresh_list.push_back( kfsetting );
    }

    void KFTaskChainRefreshConfig::LoadAllComplete()
    {
        for ( auto& iter : _refresh_data_list._objects )
        {
            auto kfsetting = iter.second;
            kfsetting->_time_setting = KFTimeConfig::Instance()->FindSetting( iter.first );
            if ( kfsetting->_time_setting == nullptr )
            {
                __LOG_ERROR__( "timeid=[{}] can't find setting!", iter.first );
            }
        }

        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFRewardConfig::Instance()->ParseRewards( kfsetting->_str_cost, kfsetting->_receive_cost );
        }
    }
}