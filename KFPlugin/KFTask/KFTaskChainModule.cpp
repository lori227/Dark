#include "KFTaskChainModule.hpp"

namespace KFrame
{
    void KFTaskChainModule::BeforeRun()
    {
        __REGISTER_RESET__( 0u, &KFTaskChainModule::OnResetRefreshTaskChain );

        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_ELEMENT__( __STRING__( taskchain ), &KFTaskChainModule::AddTaskChainElement );
        __REGISTER_COMMAND_FUNCTION__( __STRING__( refreshtaskchain ), &KFTaskChainModule::OnCommandRefreshTaskChain );

    }

    void KFTaskChainModule::BeforeShut()
    {
        __UN_RESET__();
        __UN_ADD_ELEMENT__( __STRING__( taskchain ) );
        __UN_COMMAND_FUNCTION__( __STRING__( refreshtaskchain ) );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTaskChainModule::BindOpenTaskChainFunction( const std::string& name, KFOpenTaskChainFunction& function )
    {
        auto kffucntion = _open_task_chain_function.Create( name );
        kffucntion->_function = function;
    }

    void KFTaskChainModule::UnBindOpenTaskChainFunction( const std::string& name )
    {
        _open_task_chain_function.Remove( name );
    }

    void KFTaskChainModule::BindFinishTaskChainFunction( const std::string& name, KFFinishTaskChainFunction& function )
    {
        auto kffucntion = _finish_task_chain_function.Create( name );
        kffucntion->_function = function;
    }

    void KFTaskChainModule::UnBindFinishTaskChainFunction( const std::string& name )
    {
        _finish_task_chain_function.Remove( name );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFTaskChainModule::AddTaskChainElement )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_data_name );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        auto order = kfelementobject->CalcValue( kfparent->_class_setting, __STRING__( order ), 1.0f );
        OpenTaskChain( player, kfelementobject->_config_id, order, 0u, 0u, __FUNC_LINE__ );
        return std::make_tuple( KFDataDefine::Show_None, nullptr );
    }

    bool KFTaskChainModule::OpenTaskChain( KFEntity* player, uint32 taskchainid, uint32 order, uint32 time, uint32 refreshid, const char* function, uint32 line )
    {
        auto kftaskchainsetting = KFTaskChainConfig::Instance()->FindSetting( taskchainid );
        if ( kftaskchainsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "taskchain=[{}] can't find setting!", taskchainid );
            return false;
        }

        if ( order == 0u )
        {
            order = 1u;
        }

        auto kftaskdatalist = kftaskchainsetting->_task_data_list.Find( order );
        if ( kftaskdatalist == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "taskchain=[{}] order=[{}] is error!", taskchainid, order );
            return false;
        }

        uint64 timeout = 0u;
        if ( time != 0u )
        {
            timeout = KFGlobal::Instance()->_real_time + time;
        }

        return OpenTaskLogicDataList( player, kftaskdatalist, taskchainid, order, timeout, refreshid, function, line );
    }

    bool KFTaskChainModule::OpenTaskLogicDataList( KFEntity* player, const KFWeightList<KFTaskData>* taskdatalist, uint32 taskchainid, uint32 order, uint64 time, uint32 refreshid, const char* function, uint32 line )
    {
        auto taskdata = taskdatalist->Rand();
        if ( taskdata == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "taskchain=[{}] order=[{}] can't rand taskdata!", taskchainid, order );
            return false;
        }

        // 开启逻辑功能
        std::list<uint32> logicids;
        switch ( taskdata->_logic_type )
        {
        case KFEnum::Or:
        {
            auto count = taskdata->_logic_id_list.size();
            if ( count != 0u )
            {
                auto index = KFGlobal::Instance()->RandRatio( count );
                auto logicid = taskdata->_logic_id_list[ index ];
                auto ok = OpenTaskLogicData( player, taskchainid, order, taskdata, logicid, function, line );
                if ( ok )
                {
                    logicids.push_back( logicid );
                }
            }
        }
        break;
        case KFEnum::And:
        {
            for ( auto logicid : taskdata->_logic_id_list )
            {
                auto ok = OpenTaskLogicData( player, taskchainid, order, taskdata, logicid, function, line );
                if ( ok )
                {
                    logicids.push_back( logicid );
                }
            }
        }
        break;
        }

        // 开启任务
        auto kftask = _kf_task->OpenTask( player, taskdata->_id, taskdata->_task_status, time, refreshid, taskchainid, order, logicids );
        return kftask != nullptr;
    }

    void KFTaskChainModule::CleanTaskChain( KFEntity* player, uint32 taskchainid )
    {
        auto kftaskrecord = player->Find( __STRING__( task ) );

        std::list< KFData* > tasklist;
        kftaskrecord->Find( __STRING__( chain ), taskchainid, tasklist, true );

        for ( auto kftask : tasklist )
        {
            auto taskid = kftask->Get<uint32>( __STRING__( id ) );
            player->RemoveData( kftaskrecord, taskid );
        }
    }

    void KFTaskChainModule::RemoveTaskChain( KFEntity* player, KFData* kftask )
    {
        auto taskchainid = kftask->Get<uint32>( __STRING__( chain ) );
        auto order = kftask->Get<uint32>( __STRING__( order ) );
        if ( taskchainid == 0u || order == 0u )
        {
            return;
        }

        auto kftaskchainsetting = KFTaskChainConfig::Instance()->FindSetting( taskchainid );
        if ( kftaskchainsetting == nullptr )
        {
            return;
        }

        // 附加的掉落组id
        auto taskid = kftask->Get<uint32>( __STRING__( id ) );
        auto taskdata = kftaskchainsetting->FindTaskData( order, taskid );
        if ( taskdata == nullptr )
        {
            return;
        }

        RemoveTaskLogicData( player, kftask, taskdata );
    }

    void KFTaskChainModule::FinishTaskChain( KFEntity* player, KFData* kftask, const char* function, uint32 line )
    {
        auto taskchainid = kftask->Get<uint32>( __STRING__( chain ) );
        auto order = kftask->Get<uint32>( __STRING__( order ) );
        auto time = kftask->Get( __STRING__( time ) );
        auto refreshid = kftask->Get( __STRING__( refresh ) );
        if ( taskchainid == 0u || order == 0u )
        {
            return;
        }

        auto kftaskchainsetting = KFTaskChainConfig::Instance()->FindSetting( taskchainid );
        if ( kftaskchainsetting == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "player=[{}] taskchain=[{}] can't find setting!", player->GetKeyID(), taskchainid );
        }

        // 附加的掉落组id
        auto taskid = kftask->Get<uint32>( __STRING__( id ) );
        auto taskdata = kftaskchainsetting->FindTaskData( order, taskid );
        if ( taskdata != nullptr )
        {
            for ( auto& iter : taskdata->_extend_task_chain_list )
            {
                auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
                if ( rand < iter.second )
                {
                    OpenTaskChain( player, iter.first, 1u, 0u, 0u, function, line );
                }
            }

            FinishTaskLogicData( player, kftask, taskdata );
        }

        ++order;
        auto kftaskdatalist = kftaskchainsetting->_task_data_list.Find( order );
        if ( kftaskdatalist == nullptr )
        {
            return;
        }

        OpenTaskLogicDataList( player, kftaskdatalist, taskchainid, order, time, refreshid, function, line );
    }

    bool KFTaskChainModule::OpenTaskLogicData( KFEntity* player, uint32 taskchainid, uint32 order, const KFTaskData* taskdata, uint32 logicid, const char* function, uint32 line )
    {
        if ( taskdata->_logic_name.empty() )
        {
            return false;
        }

        // 开启逻辑属性点
        auto kffunction = _open_task_chain_function.Find( taskdata->_logic_name );
        if ( kffunction != nullptr )
        {
            return kffunction->_function( player, taskchainid, order, taskdata->_id, logicid, taskdata->_logic_status, function, line );
        }

        auto kfrecord = player->Find( taskdata->_logic_name );
        if ( kfrecord == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "taskchain=[{}] order=[{}] task=[{}] dataname=[{}] no class data!", taskchainid, order, taskdata->_id, taskdata->_logic_name );
            return false;
        }

        player->UpdateData( kfrecord, logicid, kfrecord->_data_setting->_value_key_name, KFEnum::Set, taskdata->_logic_status );
        return true;
    }

    void KFTaskChainModule::FinishTaskLogicData( KFEntity* player, KFData* kftask, const KFTaskData* taskdata )
    {
        if ( taskdata->_logic_name.empty() )
        {
            return;
        }

        ListUInt32 logicids;
        auto kflogic = kftask->Find( __STRING__( logicids ) );
        for ( auto kfchild = kflogic->First(); kfchild != nullptr; kfchild = kflogic->Next() )
        {
            logicids.push_back( kfchild->Get<uint32>() );
        }

        // 结束逻辑属性点
        auto kffunction = _finish_task_chain_function.Find( taskdata->_logic_name );
        if ( kffunction != nullptr )
        {
            return kffunction->_function( player, logicids, taskdata->_finish_status );
        }

        auto kfrecord = player->Find( taskdata->_logic_name );
        if ( kfrecord == nullptr )
        {
            return;
        }

        for ( auto logicid : logicids )
        {
            auto kfdata = kfrecord->Find( logicid );
            if ( kfdata != nullptr )
            {
                player->UpdateData( kfdata, kfrecord->_data_setting->_value_key_name, KFEnum::Set, taskdata->_finish_status );
            }
        }
    }

    void KFTaskChainModule::RemoveTaskLogicData( KFEntity* player, KFData* kftask, const KFTaskData* taskdata )
    {
        FinishTaskLogicData( player, kftask, taskdata );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_RESET_FUNCTION__( KFTaskChainModule::OnResetRefreshTaskChain )
    {
        for ( auto& iter : KFTaskChainRefreshConfig::Instance()->_refresh_data_list._objects )
        {
            auto kfrefreshdata = iter.second;
            if ( !_kf_reset->CheckResetTime( player, iter.first ) )
            {
                continue;
            }

            for ( auto kfsetting : kfrefreshdata->_refresh_list )
            {
                // 判断条件
                if ( !_kf_condition->CheckCondition( player, &kfsetting->_conditions ) )
                {
                    continue;
                }

                auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
                if ( rand < kfsetting->_refresh_rate )
                {
                    // 清除原来的任务链
                    CleanTaskChain( player, kfsetting->_task_chain_id );

                    //开启新的任务链
                    OpenTaskChain( player, kfsetting->_task_chain_id, 1u, kfsetting->_receive_time, kfsetting->_id, __FUNC_LINE__ );
                }
            }
        }
    }

    __KF_COMMAND_FUNCTION__( KFTaskChainModule::OnCommandRefreshTaskChain )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        auto refreshid = KFUtility::ToValue<uint32>( params[ 0 ] );
        auto kfsetting = KFTaskChainRefreshConfig::Instance()->FindSetting( refreshid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 清除原来的任务链
        CleanTaskChain( player, kfsetting->_task_chain_id );

        //开启新的任务链
        OpenTaskChain( player, kfsetting->_task_chain_id, 1u, kfsetting->_receive_time, kfsetting->_id, __FUNC_LINE__ );
    }



}