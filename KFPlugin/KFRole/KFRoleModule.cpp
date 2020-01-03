#include "KFRoleModule.hpp"

namespace KFrame
{
    void KFRoleModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFRoleModule::OnEnterRoleModule );
        __REGISTER_UPDATE_DATA_1__( __STRING__( money ), &KFRoleModule::OnUpdateMoneyCallBack );
        __REGISTER_UPDATE_DATA_1__( __STRING__( mainstage ), &KFRoleModule::OnMainStageUpdate );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_SEX_REQ, &KFRoleModule::HandleSetSexReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_NAME_REQ, &KFRoleModule::HandleSetNameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYERNAME_TO_GAME_ACK, &KFRoleModule::HandleSetPlayerNameToGameAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_PLAYER_HEADICON_REQ, &KFRoleModule::HandleSetPlayerHeadIconReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_PLAYER_FACTION_REQ, &KFRoleModule::HandleSetPlayerFactionReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_MAIN_STAGE_REQ, &KFRoleModule::HandleUpdateMainStageReq );
    }

    void KFRoleModule::BeforeShut()
    {
        __UN_ENTER_PLAYER__();
        __UN_UPDATE_DATA_1__( __STRING__( money ) );
        __UN_UPDATE_DATA_1__( __STRING__( mainstage ) );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SET_NAME_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_SEX_REQ );
        __UN_MESSAGE__( KFMsg::S2S_SET_PLAYERNAME_TO_GAME_ACK );
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
    uint32 KFRoleModule::CheckNameValid( const std::string& name, uint32 maxlength )
    {
        if ( name.size() > maxlength )
        {
            return KFMsg::NameLengthError;
        }

        auto ok = _kf_filter->CheckFilter( name );
        if ( ok )
        {
            return KFMsg::NameFilterError;
        }

        return KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFRoleModule::HandleSetNameReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetNameReq );

        if ( kfmsg.name().empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::NameEmpty );
        }

        auto kfname = player->Find( __STRING__( basic ), __STRING__( name ) );
        auto name = kfname->Get<std::string>();
        if ( !name.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::NameAlreadySet );
        }

        // 检查名字的有效性
        auto result = CheckNameValid( kfmsg.name(), kfname->_data_setting->_int_max_value );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }

        // 修改名字
        KFMsg::S2SSetPlayerNameToDataReq req;
        req.set_playerid( playerid );
        req.set_oldname( name );
        req.set_newname( kfmsg.name() );
        req.set_costdata( _invalid_string );
        auto ok = _kf_route->SendToRand( playerid, __STRING__( logic ), KFMsg::S2S_SET_PLAYERNAME_TO_DATA_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::PublicServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRoleModule::HandleSetPlayerNameToGameAck )
    {
        __PROTO_PARSE__( KFMsg::S2SSetPlayerNameToGameAck );

        auto player = _kf_player->FindPlayer( kfmsg.playerid() );
        if ( player == nullptr )
        {
            return __LOG_ERROR__( "player[{}] set name[{}] item[{}] failed", kfmsg.playerid(), kfmsg.name(), kfmsg.costdata() );
        }

        _kf_display->SendToClient( player, kfmsg.result(), kfmsg.name() );
        if ( kfmsg.result() != KFMsg::NameSetOk )
        {
            return;
        }

        player->UpdateData( __STRING__( basic ), __STRING__( name ), kfmsg.name() );
        if ( kfmsg.costdata() != _invalid_string )
        {
            KFElements kfelements;
            auto ok = kfelements.Parse( kfmsg.costdata(), __FUNC_LINE__ );
            if ( ok )
            {
                player->RemoveElement( &kfelements, __STRING__( name ), __FUNC_LINE__ );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRoleModule::HandleSetSexReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetSexReq );

        _kf_display->SendToClient( player, KFMsg::SexSetOK );
        player->UpdateData( __STRING__( basic ), __STRING__( sex ), KFEnum::Set, kfmsg.sex() );
    }

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

    __KF_MESSAGE_FUNCTION__( KFRoleModule::HandleUpdateMainStageReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateMainStageReq );

        auto mainstage = player->Get( __STRING__( mainstage ) );
        if ( kfmsg.stageid() != mainstage + 1u )
        {
            // 流程递增
            return;
        }

        player->UpdateData( __STRING__( mainstage ), KFEnum::Set, kfmsg.stageid() );
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
    }
}