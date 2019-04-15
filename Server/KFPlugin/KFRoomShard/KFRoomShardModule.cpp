#include "KFRoomShardModule.hpp"
#include "KFBattleAllot.h"

namespace KFrame
{
    void KFRoomShardModule::InitModule()
    {

    }

    void KFRoomShardModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_BATTLE_TO_ROOM_REQ, &KFRoomShardModule::HandleRegisterBattleToRoomReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_ROOM_REQ, &KFRoomShardModule::HandleCreateRoomToRoomReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_OPEN_ROOM_TO_ROOM_ACK, &KFRoomShardModule::HandleOpenRoomToRoomAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_INFORM_BATTLE_TO_ROOM_ACK, &KFRoomShardModule::HandleInformBattleToRoomAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_ROOM_TO_ROOM_REQ, &KFRoomShardModule::HandleQueryRoomToRoomReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_FINISH_ROOM_TO_ROOM_REQ, &KFRoomShardModule::HandleFinishRoomToRoomReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_HEART_BEAT_TO_ROOM_REQ, &KFRoomShardModule::HandleHeartBeatToRoomReq );

        //////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFRoomShardModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_BATTLE_TO_ROOM_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_ROOM_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_OPEN_ROOM_TO_ROOM_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_INFORM_BATTLE_TO_ROOM_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_ROOM_TO_ROOM_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_FINISH_ROOM_TO_ROOM_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_HEART_BEAT_TO_ROOM_REQ );
    }

    void KFRoomShardModule::OnceRun()
    {
        _room_redis = _kf_redis->Create( __KF_STRING__( room ) );
        _battle_allot->Initialize( _room_redis );
    }

    void KFRoomShardModule::Run()
    {
        std::set< uint64 > removes;
        for ( auto& iter : _room_list._objects )
        {
            auto kfroom = iter.second;
            auto ok = kfroom->Run();
            if ( !ok )
            {
                removes.insert( kfroom->_id );
            }
        }

        for ( auto id : removes )
        {
            RemoveBattleRoom( id );
        }
    }

    KFBattleRoom* KFRoomShardModule::CreateBattleRoom( uint64 roomid )
    {
        auto kfroom = _room_list.Create( roomid );
        if ( kfroom->_id == _invalid_int )
        {
            // 创建房间
            kfroom->Init( roomid );
        }

        return kfroom;
    }

    void KFRoomShardModule::RemoveBattleRoom( uint64 roomid )
    {
        // 删除房间
        _room_list.Remove( roomid );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRoomShardModule::HandleRegisterBattleToRoomReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRegisterBattleToRoomReq );

        __LOG_INFO__( "register battle[{}|{}|{}:{}]!", KFAppId::ToString( kfmsg.serverid() ), kfmsg.version(), kfmsg.ip(), kfmsg.port() );

        // 注册房间
        auto ok = _battle_allot->AddBattle( kfmsg.serverid(), kfmsg.ip(), kfmsg.port(), kfmsg.version() );

        // 返回结果
        KFMsg::S2SRegisterBattleToBattleAck ack;
        ack.set_result( ok );
        _kf_route->SendToRoute( route, KFMsg::S2S_REGISTER_BATTLE_TO_BATTLE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRoomShardModule::HandleOpenRoomToRoomAck )
    {
        __PROTO_PARSE__( KFMsg::S2SOpenRoomToRoomAck );

        __LOG_DEBUG__( "open room[{}] result[{}] ack!", kfmsg.roomid(), kfmsg.result() );

        auto kfroom = _room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return __LOG_ERROR__( "can't find room[{}]!", kfmsg.roomid() );
        }

        kfroom->AffirmOpenBattle( kfmsg.result() );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRoomShardModule::HandleCreateRoomToRoomReq )
    {
        __PROTO_PARSE__( KFMsg::S2SCreateRoomToRoomReq );

        __LOG_DEBUG__( "create room[{}|{}|{}] req!", kfmsg.roomid(), kfmsg.version(), KFAppId::ToString( kfmsg.serverid() ) );

        auto kfroom = CreateBattleRoom( kfmsg.roomid() );

        kfroom->_version = kfmsg.version();
        kfroom->_match_id = kfmsg.matchid();
        kfroom->_battle_server_id = kfmsg.serverid();
        kfroom->_valid_time = KFGlobal::Instance()->_game_time + 7200000;
        for ( auto i = 0; i < kfmsg.pbplayer_size(); ++i )
        {
            auto pbplayer = &kfmsg.pbplayer( i );

            auto kfplayer = __KF_NEW__( KFBattlePlayer );
            kfplayer->CopyFrom( pbplayer );
            kfroom->_player_list.Insert( kfplayer->_id, kfplayer );
        }

        // 返回确认信息
        KFMsg::S2SCreateRoomToMatchAck ack;
        ack.set_roomid( kfmsg.roomid() );
        _kf_route->SendToRoute( route, KFMsg::S2S_CREATE_ROOM_TO_MATCH_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRoomShardModule::HandleInformBattleToRoomAck )
    {
        __PROTO_PARSE__( KFMsg::S2SInformBattleToRoomAck );

        __LOG_DEBUG__( "player=[{}] inform room=[{}] ack!", kfmsg.playerid(), kfmsg.roomid() );

        auto kfroom = _room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return __LOG_ERROR__( "can't find room[{}]!", kfmsg.roomid() );
        }

        kfroom->AffirmInformPlayer( kfmsg.playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRoomShardModule::HandleQueryRoomToRoomReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRoomToRoomReq );

        __LOG_DEBUG__( "player=[{}] query room=[{}] req!", kfmsg.playerid(), kfmsg.roomid() );

        auto ok = false;

        auto kfroom = _room_list.Find( kfmsg.roomid() );
        if ( kfroom != nullptr )
        {
            ok = kfroom->QueryRoom( kfmsg.playerid(), __ROUTE_SERVER_ID__ );
        }

        if ( !ok )
        {
            KFMsg::S2SQueryRoomToGameAck ack;
            ack.set_playerid( kfmsg.playerid() );
            _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_ROOM_TO_GAME_ACK, &ack );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRoomShardModule::HandleFinishRoomToRoomReq )
    {
        __PROTO_PARSE__( KFMsg::S2SFinishRoomToRoomReq );

        __LOG_DEBUG__( "room=[{}] finish req!", kfmsg.roomid() );

        auto kfroom = _room_list.Find( kfmsg.roomid() );
        if ( kfroom != nullptr )
        {
            kfroom->FinishRoom();
        }

        KFMsg::S2SFinishRoomToBattleAck ack;
        ack.set_roomid( kfmsg.roomid() );
        _kf_route->SendToRoute( route, KFMsg::S2S_FINISH_ROOM_TO_BATTLE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRoomShardModule::HandleHeartBeatToRoomReq )
    {
        __PROTO_PARSE__( KFMsg::S2SHeartBeatToRoomReq );

        __LOG_DEBUG__( "room=[{}] heartbeat req!", kfmsg.roomid() );

        auto kfroom = _room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return __LOG_ERROR__( "can't find room[{}]!", kfmsg.roomid() );
        }

        kfroom->UpdateHeartBeatTime();
    }
}