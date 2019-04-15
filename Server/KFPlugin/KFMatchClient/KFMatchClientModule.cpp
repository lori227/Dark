#include "KFMatchClientModule.hpp"

namespace KFrame
{
    void KFMatchClientModule::InitModule()
    {
    }

    void KFMatchClientModule::BeforeRun()
    {
        _kf_player->RegisterEnterFunction( this, &KFMatchClientModule::OnEnterQueryMatch );
        _kf_player->RegisterLeaveFunction( this, &KFMatchClientModule::OnLeaveCancelMatch );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_START_MATCH_REQ, &KFMatchClientModule::HandleStartMatchReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_START_MATCH_TO_GAME_ACK, &KFMatchClientModule::HandleStartMatchToGameAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_CANCEL_MATCH_REQ, &KFMatchClientModule::HandleCancelMatchReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_AFFIRM_MATCH_REQ, &KFMatchClientModule::HandleAffirmMatchReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_AFFIRM_MATCH_TIMEOUT_TO_GAME, &KFMatchClientModule::HandleAffirmMatchTimeoutToGame );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_MATCH_TO_GAME_ACK, &KFMatchClientModule::HandleQueryMatchToGameAck );

    }

    void KFMatchClientModule::BeforeShut()
    {
        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_START_MATCH_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_START_MATCH_TO_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CANCEL_MATCH_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_AFFIRM_MATCH_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_AFFIRM_MATCH_TIMEOUT_TO_GAME );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_MATCH_TO_GAME_ACK );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMatchClientModule::OnEnterQueryMatch( KFEntity* player )
    {
        auto kfobject = player->GetData();

        auto matchid = kfobject->GetValue( __KF_STRING__( matchid ) );
        if ( matchid == _invalid_int )
        {
            return;
        }

        auto roomid = kfobject->GetValue( __KF_STRING__( roomid ) );
        if ( roomid != _invalid_int )
        {
            return;
        }

        auto matchserverid = kfobject->GetValue( __KF_STRING__( matchserverid ) );

        KFMsg::S2SQueryMatchToMatchReq req;
        req.set_matchid( matchid );
        req.set_playerid( player->GetKeyID() );
        _kf_route->SendToServer( matchserverid, KFMsg::S2S_QUERY_MATCH_TO_MATCH_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFMatchClientModule::HandleQueryMatchToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryMatchToGameAck );

        SetMatchData( player, _invalid_int, _invalid_int );
        __LOG_DEBUG__( "player=[{}] query no match", player->GetKeyID() );
    }

    void KFMatchClientModule::OnLeaveCancelMatch( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto matchid = kfobject->GetValue( __KF_STRING__( matchid ) );
        if ( matchid == _invalid_int )
        {
            return;
        }

        auto roomid = kfobject->GetValue( __KF_STRING__( roomid ) );
        if ( roomid != _invalid_int )
        {
            return;
        }

        auto matchserverid = kfobject->GetValue( __KF_STRING__( matchserverid ) );

        KFMsg::S2SCancelMatchToShardReq req;
        req.set_matchid( matchid );
        req.set_playerid( player->GetKeyID() );
        _kf_route->SendToServer( matchserverid, KFMsg::S2S_CANCEL_MATCH_TO_SHARD_REQ, &req );

        kfobject->SetValue( __KF_STRING__( matchid ), _invalid_int );
        kfobject->SetValue( __KF_STRING__( matchserverid ), _invalid_int );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFMatchClientModule::HandleStartMatchReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgStartMatchReq );

        // 开始匹配
        auto result = ProcessStartMatch( player, kfmsg.version(), kfmsg.matchid(), kfmsg.serverid() );
        if ( result != KFMsg::MatchRequestOk )
        {
            _kf_display->SendToClient( player, result );
        }
    }

    uint32 KFMatchClientModule::ProcessStartMatch( KFEntity* player, const std::string& version, uint32 matchid, uint64 serverid )
    {
        auto kfobject = player->GetData();

        // 正在房间中
        auto roomid = kfobject->GetValue( __KF_STRING__( roomid ) );
        if ( roomid != _invalid_int )
        {
            __LOG_ERROR__( "player=[{}] already in battle[{}]", player->GetKeyID(), roomid );
            return KFMsg::MatchRequestOk;
        }

        // 是否正在匹配中
        auto waitmatchid = kfobject->GetValue( __KF_STRING__( matchid ) );
        if ( waitmatchid != _invalid_int )
        {
            __LOG_ERROR__( "player=[{}] already in match[{}] ", player->GetKeyID(), waitmatchid );
            return KFMsg::MatchAlreadyWait;
        }

        // 发送给匹配集群， 进行匹配
        KFMsg::S2SStartMatchToShardReq req;
        req.set_version( version );
        req.set_matchid( matchid );
        req.set_serverid( serverid );
        FormatMatchPlayerData( player, req.mutable_pbplayer() );
        auto ok = _kf_route->SendToObject( __KF_STRING__( match ), matchid, KFMsg::S2S_START_MATCH_TO_SHARD_REQ, &req );
        if ( !ok )
        {
            return KFMsg::MatchServerBusy;
        }

        return KFMsg::MatchRequestOk;
    }

    void KFMatchClientModule::FormatMatchPlayerData( KFEntity* player, KFMsg::PBMatchPlayer* pbplayer )
    {
        auto kfobject = player->GetData();

        pbplayer->set_id( kfobject->GetKeyID() );
        pbplayer->set_name( kfobject->GetValue< std::string >( __KF_STRING__( name ) ) );
        pbplayer->set_serverid( KFGlobal::Instance()->_app_id->GetId() );

        // 测试用
        player->RemoveData( __KF_STRING__( hero ) );

        static auto _init = false;
        static KFElements elements;
        if ( !_init )
        {
            _init = true;
            std::string _element = "[{\"hero\":{\"rand\":\"1\"}}]";
            elements.Parse( _element, __FUNC_LINE__ );
        }

        for ( auto i = 0u; i < 10u; ++i )
        {
            player->AddElement( __FUNC_LINE__, &elements, false );
        }

        // 英雄列表
        auto kfherorecord = kfobject->FindData( __KF_STRING__( hero ) );
        for ( auto kfhero = kfherorecord->FirstData(); kfhero != nullptr; kfhero = kfherorecord->NextData() )
        {
            auto& pbhero = ( *pbplayer->mutable_hero() )[ kfhero->GetKeyID() ];
            _kf_kernel->SerializeToClient( kfhero, &pbhero );
        }
    }

    void KFMatchClientModule::SetMatchData( KFEntity* player, uint32 matchid, uint64 serverid )
    {
        player->UpdateData( __KF_STRING__( matchid ), KFEnum::Set, matchid );
        player->UpdateData( __KF_STRING__( matchserverid ), KFEnum::Set, serverid );
    }

    __KF_MESSAGE_FUNCTION__( KFMatchClientModule::HandleStartMatchToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SStartMatchToGameAck );

        if ( kfmsg.result() == KFMsg::MatchRequestOk )
        {
            SetMatchData( player, kfmsg.matchid(), kfmsg.serverid() );
            __LOG_DEBUG__( "player=[{}] match=[{}|{}] ok!", kfmsg.playerid(), kfmsg.matchid(), KFAppId::ToString( kfmsg.serverid() ) );
        }

        _kf_display->SendToClient( player, kfmsg.result() );
    }

    __KF_MESSAGE_FUNCTION__( KFMatchClientModule::HandleCancelMatchReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCancelMatchReq );

        auto kfobject = player->GetData();
        auto matchid = kfobject->GetValue< uint32 >( __KF_STRING__( matchid ) );
        if ( matchid == _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::MatchNotInMatch );
        }

        auto matchserverid = kfobject->GetValue( __KF_STRING__( matchserverid ) );

        KFMsg::S2SCancelMatchToShardReq req;
        req.set_matchid( matchid );
        req.set_playerid( playerid );
        auto ok = _kf_route->SendToServer( matchserverid, KFMsg::S2S_CANCEL_MATCH_TO_SHARD_REQ, &req );
        if ( !ok )
        {
            return _kf_display->SendToClient( player, KFMsg::MatchServerBusy );
        }

        SetMatchData( player, _invalid_int, _invalid_int );
        _kf_display->SendToClient( player, KFMsg::MatchCancelOk );
        __LOG_DEBUG__( "player=[{}] cancel match req!", playerid );
    }

    __KF_MESSAGE_FUNCTION__( KFMatchClientModule::HandleAffirmMatchReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgAffirmMatchReq );

        auto kfobject = player->GetData();
        auto matchserverid = kfobject->GetValue( __KF_STRING__( matchserverid ) );
        if ( matchserverid == _invalid_int )
        {
            return;
        }

        KFMsg::S2SAffirmMatchToShardReq req;
        req.set_playerid( playerid );
        auto ok = _kf_route->SendToServer( matchserverid, KFMsg::S2S_AFFIRM_MATCH_TO_SHARD_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::MatchServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchClientModule::HandleAffirmMatchTimeoutToGame )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SAffirmMatchTimeoutToGame );

        SetMatchData( player, _invalid_int, _invalid_int );
        _kf_display->SendToClient( player, KFMsg::MatchAffirmTimeout );

        __LOG_DEBUG__( "player=[{}] affirm timeout!", kfmsg.playerid() );
    }
}