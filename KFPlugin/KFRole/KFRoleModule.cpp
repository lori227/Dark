#include "KFRoleModule.hpp"

namespace KFrame
{
    void KFRoleModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFRoleModule::OnEnterRoleModule );
        __REGISTER_UPDATE_DATA_1__( __STRING__( money ), &KFRoleModule::OnUpdateMoneyCallBack );
        __REGISTER_UPDATE_DATA_1__( __STRING__( mainstage ), &KFRoleModule::OnMainStageUpdate );
        __REGISTER_REMOVE_DATA_1__( __STRING__( story ), &KFRoleModule::OnRemoveStoryCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( story ), __STRING__( sequence ), &KFRoleModule::OnUpdateStoryCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( balance ), __STRING__( pveresult ), &KFRoleModule::OnUpdatePVECallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( balance ), __STRING__( realmresult ), &KFRoleModule::OnUpdateRealmCallBack );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_PLAYER_HEADICON_REQ, &KFRoleModule::HandleSetPlayerHeadIconReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_PLAYER_FACTION_REQ, &KFRoleModule::HandleSetPlayerFactionReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_MAIN_STAGE_REQ, &KFRoleModule::HandleUpdateMainStageReq );
    }

    void KFRoleModule::BeforeShut()
    {
        __UN_ENTER_PLAYER__();
        __UN_UPDATE_DATA_1__( __STRING__( money ) );
        __UN_UPDATE_DATA_1__( __STRING__( mainstage ) );
        __UN_REMOVE_DATA_1__( __STRING__( story ) );
        __UN_UPDATE_DATA_2__( __STRING__( story ), __STRING__( sequence ) );
        __UN_UPDATE_DATA_2__( __STRING__( balance ), __STRING__( pveresult ) );
        __UN_UPDATE_DATA_2__( __STRING__( balance ), __STRING__( realmresult ) );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SET_PLAYER_HEADICON_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_PLAYER_FACTION_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_MAIN_STAGE_REQ );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFRoleModule::OnEnterRoleModule )
    {
        auto kfbasic = player->Find( __STRING__( basic ) );

        // 默认头像
        if ( kfbasic->Get<uint32>( __STRING__( iconid ) ) == _invalid_int )
        {
            auto setting = KFIconConfig::Instance()->GetDedaultIconSetting();
            if ( setting != nullptr )
            {
                kfbasic->Set( __STRING__( iconid ), setting->_id );
            }
        }

        // 默认势力
        if ( kfbasic->Get<uint32>( __STRING__( factionid ) ) == _invalid_int )
        {
            auto setting = KFFactionConfig::Instance()->GetDedaultFactionSetting();
            if ( setting != nullptr )
            {
                kfbasic->Set( __STRING__( factionid ), setting->_id );
            }
        }

        // 进入初始化流程
        auto mainstage = player->Get( __STRING__( mainstage ) );
        OnExecuteInitialProcess( player, mainstage );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFRoleModule::OnUpdateMoneyCallBack )
    {
        if ( operate == KFEnum::Dec )
        {
            player->UpdateData( __STRING__( consumemoney ), KFEnum::Add, value );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRoleModule::HandleSetPlayerHeadIconReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetPlayerHeadIconReq );

        auto kfsetting = KFIconConfig::Instance()->FindSetting( kfmsg.iconid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerHeadIconNotExist );
        }

        // 非默认需要判断拥有
        if ( !kfsetting->_default )
        {
            auto kficon = player->Find( __STRING__( icon ), __STRING__( id ) );
            if ( kficon == nullptr )
            {
                return _kf_display->SendToClient( player, KFMsg::PlayerHeadIconNotHad );
            }
        }

        _kf_display->SendToClient( player, KFMsg::PlayerHeadIconSetOK );
        player->UpdateData( __STRING__( basic ), __STRING__( iconid ), KFEnum::Set, kfmsg.iconid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRoleModule::HandleSetPlayerFactionReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetPlayerFactionReq );

        // 是否符合配置
        auto kfsetting = KFFactionConfig::Instance()->FindSetting( kfmsg.factionid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::PlayerFactionNotExist );
        }

        _kf_display->SendToClient( player, KFMsg::PlayerFactionSetOK );
        player->UpdateData( __STRING__( basic ), __STRING__( factionid ), KFEnum::Set, kfmsg.factionid() );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRoleModule::OnMainStageUpdate )
    {
        OnExecuteInitialProcess( player, newvalue );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFRoleModule::OnRemoveStoryCallBack )
    {
        AddSequence( player, kfdata, KFMsg::ProcessStory );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRoleModule::OnUpdateStoryCallBack )
    {
        if ( newvalue != 0u )
        {
            return;
        }

        AddSequence( player, kfdata->GetParent(), KFMsg::ProcessStory );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRoleModule::OnUpdatePVECallBack )
    {
        if ( newvalue != KFMsg::Victory )
        {
            return;
        }

        AddSequence( player, kfdata->GetParent(), KFMsg::ProcessPVE );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRoleModule::OnUpdateRealmCallBack )
    {
        if ( newvalue != KFMsg::Victory )
        {
            return;
        }

        AddSequence( player, kfdata->GetParent(), KFMsg::ProcessExplore );
    }

    void KFRoleModule::AddSequence( KFEntity* player, KFData* kfdata, uint32 type )
    {
        auto mainstage = player->Get( __STRING__( mainstage ) );
        auto kfsetting = KFInitialProcessConfig::Instance()->FindSetting( mainstage );
        if ( kfsetting == nullptr )
        {
            return;
        }

        if ( kfsetting->_type != type )
        {
            return;
        }

        auto id = kfdata->Get<uint32>( __STRING__( id ) );
        if ( kfsetting->_parameter == id )
        {
            player->UpdateData( __STRING__( mainstage ), KFEnum::Add, 1u );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRoleModule::HandleUpdateMainStageReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateMainStageReq );

        auto mainstage = player->Get( __STRING__( mainstage ) );

        if ( kfmsg.stageid() != mainstage + 1u )
        {
            // 流程递增
            return;
        }

        auto kfsetting = KFInitialProcessConfig::Instance()->FindSetting( mainstage );
        if ( kfsetting == nullptr )
        {
            return;
        }

        if ( kfsetting->_type == KFMsg::ProcessPVE || kfsetting->_type == KFMsg::ProcessExplore
                || kfsetting->_type == KFMsg::ProcessTask || kfsetting->_type == KFMsg::ProcessStory )
        {
            return;
        }

        player->UpdateData( __STRING__( mainstage ), KFEnum::Add, 1u );
    }

    void KFRoleModule::OnExecuteInitialProcess( KFEntity* player, uint32 id )
    {
        auto kfsetting = KFInitialProcessConfig::Instance()->FindSetting( id );
        if ( kfsetting == nullptr )
        {
            return;
        }

        if ( kfsetting->_type == KFMsg::ProcessTask )
        {
            // 如果当前流程为任务，接取任务
            _kf_task->OpenTask( player, kfsetting->_parameter, KFMsg::ExecuteStatus, 0u );

            player->UpdateData( __STRING__( mainstage ), KFEnum::Add, 1u );
        }
        else if ( kfsetting->_type == KFMsg::ProcessStory )
        {
            _kf_story->AddStory( player, kfsetting->_parameter, _invalid_int, _invalid_string, _invalid_int );
        }
    }
}