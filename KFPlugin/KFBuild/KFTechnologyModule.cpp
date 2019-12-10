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

        auto kfsetting = KFTechnologyConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "id=[{}] technologysetting = null!", kfelementobject->_config_id );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        auto kftechnology = kfparent->Find( kfsetting->_id );
        if ( kftechnology != nullptr )
        {
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        // 更新科技状态
        auto status = kfelementobject->CalcValue( kfparent->_class_setting, kfparent->_data_setting->_value_key_name, 1.0f );
        player->UpdateData( kfparent, kfsetting->_id, kfparent->_data_setting->_value_key_name, KFEnum::Set, status );

        return std::make_tuple( KFDataDefine::Show_None, nullptr );
    }

    __KF_ADD_DATA_FUNCTION__( KFTechnologyModule::OnAddUnlockTechnology )
    {
        auto status = kfdata->Get( kfdata->_data_setting->_value_key_name );
        if ( status != 0u )
        {
            OnUpgradeTechnology( player, kfparent, key );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTechnologyModule::OnUpdateTechnologyStatus )
    {
        if ( newvalue != 0u )
        {
            OnUpgradeTechnology( player, kfdata->GetParent()->GetParent(), key );
        }
    }

    void KFTechnologyModule::OnUpgradeTechnology( KFEntity* player, KFData* kftechnologyrecord, uint32 technologyid )
    {
        auto kfsetting = KFTechnologyConfig::Instance()->FindSetting( technologyid );
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
            auto kftechnologysetting = KFTechnologyConfig::Instance()->FindSetting( unlockid );
            if ( kftechnologysetting == nullptr )
            {
                continue;
            }

            player->UpdateData( kftechnologyrecord, kftechnologysetting->_id, kftechnologyrecord->_data_setting->_value_key_name, KFEnum::Set, kftechnologysetting->_status );
        }
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFTechnologyModule::OnEnterTechnologyModule )
    {
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

        auto kftechnologyrecord = player->Find( __STRING__( technology ) );
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

            player->RemoveElement( &kftechsetting->_consume, __FUNC_LINE__ );
        }

        // 更新科技状态
        player->UpdateData( kftechnology, kftechrecord->_data_setting->_value_key_name, KFEnum::Set, 1u );
    }
}