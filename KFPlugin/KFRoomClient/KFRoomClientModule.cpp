#include "KFRoomClientModule.hpp"

namespace KFrame
{
    void KFRoomClientModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_INFORM_BATTLE_TO_GAME_REQ, &KFRoomClientModule::HandleInformBattleToGameReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_INFORM_BATTLE_ACK, &KFRoomClientModule::HandleInformBattleAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_ROOM_TO_GAME_ACK, &KFRoomClientModule::HandleQueryRoomToGameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_FINISH_ROOM_TO_GAME_REQ, &KFRoomClientModule::HandleFinishRoomToGameReq );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_ENTER_PLAYER__( &KFRoomClientModule::OnEnterQueryRoom );
    }

    void KFRoomClientModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::S2S_INFORM_BATTLE_TO_GAME_REQ );
        __UN_MESSAGE__( KFMsg::MSG_INFORM_BATTLE_ACK );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_ROOM_TO_GAME_ACK );
        __UN_MESSAGE__( KFMsg::S2S_FINISH_ROOM_TO_GAME_REQ );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_ENTER_PLAYER__();
    }

    __KF_MESSAGE_FUNCTION__( KFRoomClientModule::HandleInformBattleToGameReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SInformBattleToGameReq );

        __LOG_DEBUG__( "player=[{}] room=[{}] battle=[{}|{}:{}]", kfmsg.playerid(), kfmsg.roomid(), KFAppId::ToString( kfmsg.battleid() ), kfmsg.ip(), kfmsg.port() );

        SetRoomData( player, kfmsg.roomid(), __ROUTE_SERVER_ID__ );

        // 通知客户端
        KFMsg::MsgInformBattleReq req;
        req.set_roomid( kfmsg.roomid() );
        req.set_battleid( kfmsg.battleid() );
        req.set_ip( kfmsg.ip() );
        req.set_port( kfmsg.port() );
        _kf_player->SendToClient( player, KFMsg::MSG_INFORM_BATTLE_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRoomClientModule::HandleInformBattleAck )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgInformBattleAck );

        __LOG_DEBUG__( "player=[{}] affirm battle!", playerid );

        auto roomid = player->Get( __STRING__( roomid ) );
        auto roomserverid = player->Get( __STRING__( roomserverid ) );
        if ( roomserverid == _invalid_int || roomid == _invalid_int )
        {
            return __LOG_ERROR__( "player=[{}] affirm battle failed!", playerid );
        }

        KFMsg::S2SInformBattleToRoomAck ack;
        ack.set_playerid( playerid );
        ack.set_roomid( roomid );
        _kf_route->SendToServer( roomserverid, KFMsg::S2S_INFORM_BATTLE_TO_ROOM_ACK, &ack, false );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFRoomClientModule::OnEnterQueryRoom )
    {
        auto roomid = player->Get( __STRING__( roomid ) );
        if ( roomid == _invalid_int )
        {
            return;
        }

        auto roomserverid = player->Get( __STRING__( roomserverid ) );
        if ( roomserverid == _invalid_int )
        {
            player->Set( __STRING__( roomid ), _invalid_int );
            player->Set( __STRING__( matchid ), _invalid_int );
        }
        else
        {
            KFMsg::S2SQueryRoomToRoomReq req;
            req.set_roomid( roomid );
            req.set_playerid( player->GetKeyID() );
            _kf_route->SendToServer( roomserverid, KFMsg::S2S_QUERY_ROOM_TO_ROOM_REQ, &req, true );
        }
    }

    void KFRoomClientModule::SetRoomData( KFEntity* player, uint64 roomid, uint64 roomserverid )
    {
        player->UpdateData( __STRING__( roomid ), KFEnum::Set, roomid );
        player->UpdateData( __STRING__( roomserverid ), KFEnum::Set, roomserverid );

        if ( roomid == _invalid_int )
        {
            player->UpdateData( __STRING__( matchid ), KFEnum::Set, _invalid_int );
            player->UpdateData( __STRING__( matchserverid ), KFEnum::Set, _invalid_int );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRoomClientModule::HandleQueryRoomToGameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryRoomToGameAck );

        SetRoomData( player, _invalid_int, _invalid_int );
    }

    __KF_MESSAGE_FUNCTION__( KFRoomClientModule::HandleFinishRoomToGameReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SFinishRoomToGameReq );

        __LOG_DEBUG__( "player=[{}] room=[{}] finish!", kfmsg.playerid(), kfmsg.roomid() );

        SetRoomData( player, _invalid_int, _invalid_int );

        KFMsg::MsgFinishRoomReq req;
        req.set_roomid( kfmsg.roomid() );
        _kf_player->SendToClient( player, KFMsg::MSG_FINISH_ROOM_REQ, &req );
    }
}