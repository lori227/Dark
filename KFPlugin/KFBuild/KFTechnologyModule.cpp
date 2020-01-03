#include "KFTechnologyModule.hpp"

namespace KFrame
{
    void KFTechnologyModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ENTER_PLAYER__( &KFTechnologyModule::OnEnterTechnologyModule );
        __REGISTER_ADD_ELEMENT__( __STRING__( technology ), &KFTechnologyModule::AddTechnologyElement );

        __REGISTER_ADD_DATA_1__( __STRING__( technology ), &KFTechnologyModule::OnAddUnlockTechnology );
        __REGISTER_UPDATE_DATA_2__( __STRING__( technology ), __STRING__( status ), &KFTechnologyModule::OnUpdateTechnologyStatus );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_UPGRADE_TECHNOLOGY_REQ, &KFTechnologyModule::HandleUpgradeTechnologyReq );
    }

    void KFTechnologyModule::BeforeShut()
    {
        __UN_ENTER_PLAYER__();
        __UN_ADD_ELEMENT__( __STRING__( technology ) );
        __UN_ADD_DATA_1__( __STRING__( technology ) );
        __UN_UPDATE_DATA_2__( __STRING__( technology ), __STRING__( status ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_UPGRADE_TECHNOLOGY_REQ );
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFTechnologyModule::AddTechnologyElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", kfelement->_data_name );
            return false;
        }

        auto kfsetting = KFTechnologyConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "id=[{}] technologysetting = null", kfelementobject->_config_id );
            return false;
        }

        auto status = kfelementobject->CalcValue( kfparent->_data_setting, kfparent->_data_setting->_value_key_name, 1.0f );
        UpdateTechnologyData( player, kfparent, kfsetting->_id, status );
        return true;
    }

    __KF_ADD_DATA_FUNCTION__( KFTechnologyModule::OnAddUnlockTechnology )
    {
        OnUpgradeTechnology( player, kfdata );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTechnologyModule::OnUpdateTechnologyStatus )
    {
        OnUpgradeTechnology( player, kfdata->GetParent() );
    }

    void KFTechnologyModule::OnUpgradeTechnology( KFEntity* player, KFData* kftechnology )
    {
        auto status = kftechnology->Get<uint32>( kftechnology->_data_setting->_value_key_name );
        if ( status == 0u )
        {
            return;
        }

        auto kfsetting = KFTechnologyConfig::Instance()->FindSetting( kftechnology->GetKeyID() );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 执行科技效果逻辑
        _kf_execute->Execute( player, &kfsetting->_execute_data, __FUNC_LINE__ );

        // 建筑科技的数量
        auto kfbuild = player->Find( __STRING__( build ), kfsetting->_build_id );
        if ( kfbuild != nullptr )
        {
            player->UpdateData( kfbuild, __STRING__( technology ), KFEnum::Add, 1u );
        }

        // 解锁后置科技
        for ( auto unlockid : kfsetting->_unlock_technology )
        {
            UpdateTechnologyData( player, kftechnology->GetParent(), unlockid, 0u );
        }
    }

    void KFTechnologyModule::UpdateTechnologyData( KFEntity* player, KFData* kftechnologyrecord, uint32 technologyid, uint32 status )
    {
        auto kfsetting = KFTechnologyConfig::Instance()->FindSetting( technologyid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        auto newstatus = ( status == 0u ? kfsetting->_status : status );
        auto kftechnology = kftechnologyrecord->Find( technologyid );
        if ( kftechnology == nullptr )
        {
            kftechnology = player->CreateData( kftechnologyrecord );
            kftechnology->Set( __STRING__( type ), kfsetting->_type );
            kftechnology->Set( kftechnology->_data_setting->_value_key_name, newstatus );
            player->AddData( kftechnologyrecord, technologyid, kftechnology );
        }
        else
        {
            auto oldstatus = kftechnology->Get( kftechnology->_data_setting->_value_key_name );
            if ( oldstatus == 0u && newstatus != 0u )
            {
                player->UpdateData( kftechnology, kftechnology->_data_setting->_value_key_name, KFEnum::Set, newstatus );
            }
        }
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFTechnologyModule::OnEnterTechnologyModule )
    {
        // 设置科技类型
        auto kftechnologyrecord = player->Find( __STRING__( technology ) );
        for ( auto kftechnology = kftechnologyrecord->First(); kftechnology != nullptr; kftechnology = kftechnologyrecord->Next() )
        {
            auto kfsetting = KFTechnologyConfig::Instance()->FindSetting( kftechnology->GetKeyID() );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            kftechnology->Set( __STRING__( type ), kfsetting->_type );
        }

        auto kfeffect = player->Find( __STRING__( effect ) );
        auto lastversion = kfeffect->Get<std::string>( __STRING__( version ) );
        auto& newversion = KFTechnologyConfig::Instance()->GetVersion();
        if ( lastversion == newversion )
        {
            return;
        }

        kfeffect->Reset();
        kfeffect->Set( __STRING__( newversion ), 1u );
        kfeffect->Set( __STRING__( version ), newversion );

        // 清空所有的建筑的科技数量
        auto kfbuildrecord = player->Find( __STRING__( build ) );
        {
            for ( auto kfbuild = kfbuildrecord->First(); kfbuild != nullptr; kfbuild = kfbuildrecord->Next() )
            {
                kfbuild->Set( __STRING__( technology ), 0u );
            }
        }

        for ( auto kftechnology = kftechnologyrecord->First(); kftechnology != nullptr; kftechnology = kftechnologyrecord->Next() )
        {
            auto status = kftechnology->Get<uint32>( kftechnologyrecord->_data_setting->_value_key_name );
            if ( status == 0u )
            {
                continue;
            }

            auto kfsetting = KFTechnologyConfig::Instance()->FindSetting( kftechnology->GetKeyID() );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            // 建筑科技的数量
            _kf_execute->Execute( player, &kfsetting->_execute_data, __FUNC_LINE__ );

            // 建筑科技的数量
            auto kfbuild = kfbuildrecord->Find( kfsetting->_id );
            if ( kfbuild != nullptr )
            {
                kfbuild->Operate( __STRING__( technology ), KFEnum::Add, 1u );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTechnologyModule::HandleUpgradeTechnologyReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpgradeTechnologyReq );

        auto kftechsetting = KFTechnologyConfig::Instance()->FindSetting( kfmsg.id() );
        if ( kftechsetting == nullptr )
        {
            // 科技参数错误
            return _kf_display->SendToClient( player, KFMsg::TechnologySettingError );
        }

        auto kftechrecord = player->Find( __STRING__( technology ) );
        auto kftechnology = kftechrecord->Find( kfmsg.id() );
        if ( kftechnology == nullptr )
        {
            // 当前科技未解锁
            return _kf_display->SendToClient( player, KFMsg::TechnologyIslock );
        }

        auto status = kftechnology->Get<uint32>( kftechrecord->_data_setting->_value_key_name );
        if ( status != _invalid_int )
        {
            // 当前科技已解锁
            return _kf_display->SendToClient( player, KFMsg::TechnologyIsUnlock );
        }

        // 前置科技条件
        for ( auto id : kftechsetting->_pre_technology )
        {
            auto prestatus = kftechrecord->Get<uint32>( id, kftechrecord->_data_setting->_value_key_name );
            if ( prestatus == _invalid_int )
            {
                return _kf_display->SendToClient( player, KFMsg::TechnologyLackPreCondition );
            }
        }

        // 资源是否足够
        if ( !kftechsetting->_consume.IsEmpty() )
        {
            auto dataname = player->CheckRemoveElement( &kftechsetting->_consume, __FUNC_LINE__ );
            if ( !dataname.empty() )
            {
                return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
            }

            player->RemoveElement( &kftechsetting->_consume, __STRING__( technology ), __FUNC_LINE__ );
        }

        // 更新科技状态
        player->UpdateData( kftechnology, kftechrecord->_data_setting->_value_key_name, KFEnum::Set, 1u );
    }
}