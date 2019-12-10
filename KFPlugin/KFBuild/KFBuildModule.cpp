#include "KFBuildModule.hpp"

namespace KFrame
{
    void KFBuildModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ADD_DATA__( &KFBuildModule::OnAddDataBuildModule );
        __REGISTER_REMOVE_DATA__( &KFBuildModule::OnRemoveDataBuildModule );
        __REGISTER_UPDATE_DATA__( &KFBuildModule::OnUpdateDataBuildModule );

        __REGISTER_ADD_DATA_1__( __STRING__( build ), &KFBuildModule::OnAddUnlockBuild );
        __REGISTER_UPDATE_DATA_2__( __STRING__( build ), __STRING__( level ), &KFBuildModule::OnUpdateBuildLevel );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_START_UPGRADE_BUILD_REQ, &KFBuildModule::HandleStartUpgradeBuildReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_ONEKEY_UPGRADE_BUILD_REQ, &KFBuildModule::HandleOnekeyUpgradeBuildReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPGRADE_BUILD_REQ, &KFBuildModule::HandleUpgradeBuildReq );
    }

    void KFBuildModule::BeforeShut()
    {
        __UN_ADD_DATA__();
        __UN_REMOVE_DATA__();
        __UN_UPDATE_DATA__();

        __UN_ADD_DATA_1__( __STRING__( build ) );
        __UN_UPDATE_DATA_2__( __STRING__( build ), __STRING__( level ) );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_START_UPGRADE_BUILD_REQ );
        __UN_MESSAGE__( KFMsg::MSG_ONEKEY_UPGRADE_BUILD_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPGRADE_BUILD_REQ );
    }

#define __UPDATE_CONDITION_LIST__( updatefunction )\
    auto kfbuildrecord = player->Find( __STRING__( build ) );\
    for ( auto kfbuild = kfbuildrecord->First(); kfbuild != nullptr; kfbuild = kfbuildrecord->Next() )\
    {\
        if ( kfbuild->Get<uint64>( __STRING__( time ) ) > 0u )\
        {\
            continue;\
        }\
        auto kfconditionobject = kfbuild->Find( __STRING__( conditions ) );\
        auto kfconditionrecord = kfconditionobject->Find(__STRING__(condition));\
        if (kfconditionrecord->Size() == 0u)\
        {\
            continue;\
        }\
        updatefunction;\
    }\


    __KF_ADD_DATA_FUNCTION__( KFBuildModule::OnAddDataBuildModule )
    {
        __UPDATE_CONDITION_LIST__( _kf_condition->UpdateAddCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata ) );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFBuildModule::OnRemoveDataBuildModule )
    {
        __UPDATE_CONDITION_LIST__( _kf_condition->UpdateRemoveCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata ) );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFBuildModule::OnUpdateDataBuildModule )
    {
        __UPDATE_CONDITION_LIST__( _kf_condition->UpdateUpdateCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata, operate, value, newvalue ) );
    }

    __KF_ADD_DATA_FUNCTION__( KFBuildModule::OnAddUnlockBuild )
    {
        // 解锁科技等级
        UnlockTechnologyLevel( player, kfdata );

        // 更新建筑升级条件
        UpdateBuildCondition( player, kfdata );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFBuildModule::OnUpdateBuildLevel )
    {
        // 解锁科技等级
        UnlockTechnologyLevel( player, kfdata->GetParent() );

        // 更新建筑升级条件
        UpdateBuildCondition( player, kfdata->GetParent() );
    }

    __KF_MESSAGE_FUNCTION__( KFBuildModule::HandleStartUpgradeBuildReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgStartUpgradeBuildReq );

        auto kfbuild = player->Find( __STRING__( build ), kfmsg.id() );
        if ( kfbuild == nullptr )
        {
            // 建筑未激活
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        auto level = kfbuild->Get( __STRING__( level ) );
        auto kfsetting = KFBuildConfig::Instance()->FindBuildSetting( kfmsg.id(), level + 1u );
        if ( kfsetting == nullptr )
        {
            // 建筑已满级
            return _kf_display->SendToClient( player, KFMsg::BuildLevelIsMax );
        }

        auto upgradetime = kfbuild->Get<uint64>( __STRING__( time ) );
        if ( upgradetime > 0u )
        {
            // 建筑正在升级
            return _kf_display->SendToClient( player, KFMsg::BuildInUpgradeTime );
        }

        auto upgradebuildnum = GetUpgradeBuildNum( player );

        static auto _option = _kf_option->FindOption( __STRING__( buildupgrademaxnum ) );
        if ( upgradebuildnum >= _option->_uint32_value )
        {
            // 没有空闲的升级队列
            return _kf_display->SendToClient( player, KFMsg::BuildNoFreeUpgradeList );
        }

        auto kfconditionobject = kfbuild->Find( __STRING__( conditions ) );
        if ( !_kf_condition->CheckCondition( player, kfconditionobject ) )
        {
            // 升级条件不满足
            return _kf_display->SendToClient( player, KFMsg::BuildUpgradeLackCondition );
        }

        // 资源是否足够
        if ( !kfsetting->_consume.IsEmpty() )
        {
            auto dataname = player->CheckRemoveElement( &kfsetting->_consume, __FUNC_LINE__ );
            if ( !dataname.empty() )
            {
                return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
            }

            player->RemoveElement( &kfsetting->_consume, __FUNC_LINE__ );
        }

        // 开始升级
        upgradetime = KFGlobal::Instance()->_real_time + kfsetting->_upgrade_time;
        player->UpdateData( kfbuild, __STRING__( time ), KFEnum::Set, upgradetime );
    }

    __KF_MESSAGE_FUNCTION__( KFBuildModule::HandleOnekeyUpgradeBuildReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgOnekeyUpgradeBuildReq );

        auto kfbuild = player->Find( __STRING__( build ), kfmsg.id() );
        if ( kfbuild == nullptr )
        {
            // 建筑未激活
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        auto upgradetime = kfbuild->Get<uint64>( __STRING__( time ) );
        auto nowtime = KFGlobal::Instance()->_real_time;
        if ( upgradetime <= nowtime )
        {
            // 建筑不在升级中
            return _kf_display->SendToClient( player, KFMsg::BuildNotInUpgradeTime );
        }

        auto level = kfbuild->Get<uint64>( __STRING__( level ) );
        auto kfsetting = KFBuildConfig::Instance()->FindBuildSetting( kfmsg.id(), level + 1u );
        if ( kfsetting == nullptr )
        {
            // 建筑等级配置错误
            return _kf_display->SendToClient( player, KFMsg::BuildLevelSettingError );
        }

        auto count = ( upgradetime - nowtime - 1u ) / kfsetting->_unit_time + 1u;

        auto dataname = player->CheckRemoveElement( &kfsetting->_onekey_consume, __FUNC_LINE__, count );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }
        player->RemoveElement( &kfsetting->_onekey_consume, __FUNC_LINE__, count );

        player->UpdateData( kfbuild, __STRING__( time ), KFEnum::Set, nowtime );
    }

    __KF_MESSAGE_FUNCTION__( KFBuildModule::HandleUpgradeBuildReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpgradeBuildReq );

        auto kfbuild = player->Find( __STRING__( build ), kfmsg.id() );
        if ( kfbuild == nullptr )
        {
            // 建筑未激活
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        auto level = kfbuild->Get( __STRING__( level ) );
        auto kfsetting = KFBuildConfig::Instance()->FindBuildSetting( kfmsg.id(), level + 1u );
        if ( kfsetting == nullptr )
        {
            // 建筑已满级
            return _kf_display->SendToClient( player, KFMsg::BuildLevelIsMax );
        }

        auto upgradetime = kfbuild->Get<uint64>( __STRING__( time ) );
        auto nowtime = KFGlobal::Instance()->_real_time;
        if ( upgradetime == 0u || upgradetime > nowtime )
        {
            // 建筑正在升级
            return _kf_display->SendToClient( player, KFMsg::BuildInUpgradeTime );
        }

        // 开始升级
        player->UpdateData( kfbuild, __STRING__( time ), KFEnum::Set, 0u );
        player->UpdateData( kfbuild, __STRING__( level ), KFEnum::Add, 1u );
    }

    uint32 KFBuildModule::GetUpgradeBuildNum( KFEntity* player )
    {
        uint32 upgradenum = 0u;
        auto kfbuildrecord = player->Find( __STRING__( build ) );
        for ( auto kfbuild = kfbuildrecord->First(); kfbuild != nullptr; kfbuild = kfbuildrecord->Next() )
        {
            if ( kfbuild->Get<uint64>( __STRING__( time ) ) > 0u )
            {
                upgradenum++;
            }
        }

        return upgradenum;
    }

    void KFBuildModule::UnlockTechnologyLevel( KFEntity* player, KFData* kfdata )
    {
        auto id = kfdata->Get( __STRING__( id ) );
        auto level = kfdata->Get( __STRING__( level ) );
        auto kfbuildsetting = KFBuildConfig::Instance()->FindBuildSetting( id, level );
        if ( kfbuildsetting == nullptr )
        {
            return;
        }

        auto kftechnohogyrecord = player->Find( __STRING__( technology ) );
        for ( auto id : kfbuildsetting->_technology )
        {
            // 是否默认解锁激活
            auto kftechsetting = KFTechnologyConfig::Instance()->FindSetting( id );
            if ( kftechsetting == nullptr )
            {
                continue;
            }

            player->UpdateData( kftechnohogyrecord, id, __STRING__( status ), KFEnum::Set, kftechsetting->_status );
        }
    }

    void KFBuildModule::UpdateBuildCondition( KFEntity* player, KFData* kfdata )
    {
        // 清空当前的条件
        auto kfconditionobject = kfdata->Find( __STRING__( conditions ) );

        auto kfconditionrecord = kfconditionobject->Find( __STRING__( condition ) );
        player->CleanData( kfconditionrecord );

        auto id = kfdata->Get( __STRING__( id ) );
        auto level = kfdata->Get( __STRING__( level ) );
        auto kfsetting = KFBuildConfig::Instance()->FindBuildSetting( id, level + 1u );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 更换同步顺序, 先删除条件，再添加条件，再更新
        player->SyncDataSequence( KFEnum::Dec, KFEnum::Add, KFEnum::Set );

        // 添加下一级的升级条件
        _kf_condition->AddCondition( kfconditionobject, kfsetting->_condition, kfsetting->_condition_type );

        // 初始化条件
        _kf_condition->InitCondition( player, kfconditionobject, KFConditionEnum::LimitNull, false );

        // 数据同步到客户端
        player->SynAddRecordData( kfconditionrecord );
    }

}