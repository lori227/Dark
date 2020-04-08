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

        __REGISTER_ADD_ELEMENT__( __STRING__( build ), &KFBuildModule::AddBuildElement );
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

        __UN_ADD_ELEMENT__( __STRING__( build ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_START_UPGRADE_BUILD_REQ );
        __UN_MESSAGE__( KFMsg::MSG_ONEKEY_UPGRADE_BUILD_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPGRADE_BUILD_REQ );
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFBuildModule::AddBuildElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        auto level = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( level ), kfresult->_multiple );

#ifdef __KF_DEBUG__
        for ( auto& iter : KFBuildConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( level == 0u || kfsetting->_level <= level )
            {
                player->UpdateData( kfparent, kfsetting->_build_id, __STRING__( level ), KFEnum::Set, kfsetting->_level );
                player->UpdateData( kfparent, kfsetting->_build_id, __STRING__( unlock ), KFEnum::Set, KFGlobal::Instance()->_real_time );
            }
        }
#endif

        return true;
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

        if ( IsUpgradeListLimit( player ) )
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

        auto decconsume = player->Get<uint32>( __STRING__( effect ), __STRING__( campdecconsume ) );
        decconsume = __MIN__( decconsume, KFRandEnum::TenThousand );
        double multiple = 1.0 - static_cast<double>( decconsume ) / KFRandEnum::TenThousand;

        // 资源是否足够
        auto& dataname = player->RemoveElement( &kfsetting->_consume, multiple, __STRING__( buildupgrade ), kfsetting->_id, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        // 开始升级
        auto dectime = player->Get<uint32>( __STRING__( effect ), __STRING__( campdectime ) );
        dectime = __MIN__( dectime, KFRandEnum::TenThousand );
        double ratio = 1.0 - static_cast<double>( dectime ) / KFRandEnum::TenThousand;

        upgradetime = static_cast<uint64>( ratio * kfsetting->_upgrade_time + 0.5 );
        upgradetime += KFGlobal::Instance()->_real_time;
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
        auto& dataname = player->RemoveElement( &kfsetting->_onekey_consume, count, __STRING__( buildonekey ), kfsetting->_id, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

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

        _kf_display->SendToClient( player, KFMsg::BuildUpgradeSuc );
    }

    bool KFBuildModule::IsUpgradeListLimit( KFEntity* player )
    {
        auto upgradelist = player->Get<uint32>( __STRING__( effect ), __STRING__( campupgradelist ) );

        uint32 upgradenum = 0u;
        auto nowtime = KFGlobal::Instance()->_real_time;
        auto kfbuildrecord = player->Find( __STRING__( build ) );
        for ( auto kfbuild = kfbuildrecord->First(); kfbuild != nullptr; kfbuild = kfbuildrecord->Next() )
        {
            auto upgradetime = kfbuild->Get<uint64>( __STRING__( time ) );
            if ( upgradetime > nowtime )
            {
                upgradenum++;
                if ( upgradenum >= upgradelist )
                {
                    return true;
                }
            }
        }

        return false;
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

        auto kftechnologyrecord = player->Find( __STRING__( technology ) );
        for ( auto id : kfbuildsetting->_technology )
        {
            // 是否默认解锁激活
            auto kftechsetting = KFTechnologyConfig::Instance()->FindSetting( id );
            if ( kftechsetting == nullptr )
            {
                __LOG_ERROR__( "Technology id [{}] is not exist", id );
                continue;
            }

            auto kftechnology = kftechnologyrecord->Find( id );
            if ( kftechnology == nullptr )
            {
                kftechnology = player->CreateData( kftechnologyrecord );
                kftechnology->Set( __STRING__( status ), kftechsetting->_status );
                kftechnology->Set( __STRING__( type ), kftechsetting->_type );
                player->AddData( kftechnologyrecord, id, kftechnology );
            }
            else
            {
                auto status = kftechnology->Get( __STRING__( status ) );
                if ( status == 0u && kftechsetting->_status != 0u )
                {
                    player->UpdateData( kftechnology, __STRING__( status ), KFEnum::Set, kftechsetting->_status );
                }
            }
        }
    }

    void KFBuildModule::UpdateBuildCondition( KFEntity* player, KFData* kfdata )
    {
        // 清空当前的条件
        auto kfconditionobject = kfdata->Find( __STRING__( conditions ) );

        auto kfconditionrecord = kfconditionobject->Find( __STRING__( condition ) );
        player->CleanData( kfconditionrecord, false );

        auto id = kfdata->Get( __STRING__( id ) );
        auto level = kfdata->Get( __STRING__( level ) );
        auto kfsetting = KFBuildConfig::Instance()->FindBuildSetting( id, level + 1u );
        if ( kfsetting == nullptr || kfsetting->_condition.empty() )
        {
            return;
        }

        // 添加下一级的升级条件
        _kf_condition->AddCondition( player, kfconditionobject, kfsetting->_condition, kfsetting->_condition_type );

        // 初始化条件
        _kf_condition->InitCondition( player, kfconditionobject, KFConditionEnum::LimitNull, false );

        // 数据同步到客户端
        player->SynAddRecordData( kfconditionrecord );
    }

}