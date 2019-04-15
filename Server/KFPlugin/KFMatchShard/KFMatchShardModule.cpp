#include "KFMatchShardModule.hpp"

namespace KFrame
{
    void KFMatchShardModule::InitModule()
    {
    }

    void KFMatchShardModule::BeforeRun()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_START_MATCH_TO_SHARD_REQ, &KFMatchShardModule::HandleStartMatchToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_SHARD_REQ, &KFMatchShardModule::HandleCancelMatchToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_AFFIRM_MATCH_TO_SHARD_REQ, &KFMatchShardModule::HandleAffirmMatchToShardReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_MATCH_ACK, &KFMatchShardModule::HandleCreateRoomToMatchAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_MATCH_TO_MATCH_REQ, &KFMatchShardModule::HandleQueryMatchToMatchReq );
    }

    void KFMatchShardModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_START_MATCH_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_AFFIRM_MATCH_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROOM_TO_MATCH_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_MATCH_TO_MATCH_REQ );
    }

    void KFMatchShardModule::OnceRun()
    {
        // 添加匹配模式
        RouteObjectList matchlist;
        matchlist.insert( 1 );
        _kf_route->SyncObject( matchlist );
    }

    void KFMatchShardModule::Run()
    {
        // 匹配逻辑
        for ( auto& iter : _match_queue_list._objects )
        {
            auto kfqueue = iter.second;
            kfqueue->RunMatch();
        }

        // 房间逻辑
        {
            std::set< uint64 > _removes;
            for ( auto& iter : _match_room_list._objects )
            {
                auto kfroom = iter.second;
                auto ok = kfroom->Run();
                if ( !ok )
                {
                    _removes.insert( kfroom->_id );
                }
            }

            for ( auto id : _removes )
            {
                _match_room_list.Remove( id );
            }
        }
    }

    KFMatchRoom* KFMatchShardModule::CreateMatchRoom( KFMatchQueue* kfqueue, const std::string& version, uint64 battleserverid )
    {
        auto kfroom = __KF_NEW__( KFMatchRoom );
        kfroom->Initialize( kfqueue, version, battleserverid );

        _match_room_list.Insert( kfroom->_id, kfroom );
        return kfroom;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    KFMatchQueue* KFMatchShardModule::FindMatchQueue( uint32 matchid )
    {
        auto kfqueue = _match_queue_list.Find( matchid );
        if ( kfqueue == nullptr )
        {
            kfqueue = _match_queue_list.Create( matchid );
            kfqueue->_match_id = matchid;
            kfqueue->_match_module = this;
        }

        return kfqueue;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFMatchShardModule::StartMatch( const KFMsg::PBMatchPlayer* pbplayer, uint32 matchid, const std::string& version, uint64 battleserverid )
    {
        // 先判断是否在匹配中
        auto kfplayer = _match_player_manage->Find( pbplayer->id() );
        if ( kfplayer != nullptr )
        {
            if ( kfplayer->_room_id != _invalid_int )
            {
                return KFMsg::MatchInRoom;
            }

            // 删除旧玩家
            auto kfqueue = FindMatchQueue( kfplayer->_match_id );
            kfqueue->RemovePlayer( pbplayer->id() );
        }

        // 判断版本


        auto kfqueue = FindMatchQueue( matchid );
        kfqueue->StartMatch( pbplayer, version, battleserverid );
        return KFMsg::MatchRequestOk;
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleStartMatchToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SStartMatchToShardReq );

        auto pbplayer = &kfmsg.pbplayer();
        __LOG_DEBUG__( "player=[{}] match=[{}] req!", pbplayer->id(), kfmsg.matchid() );

        // 处理匹配
        auto result = StartMatch( pbplayer, kfmsg.matchid(), kfmsg.version(), kfmsg.serverid() );
        if ( result != KFMsg::MatchRequestOk )
        {
            __LOG_ERROR__( "player=[{}] match failed=[{}]!", pbplayer->id(), result );
        }

        // 发送给玩家
        KFMsg::S2SStartMatchToGameAck ack;
        ack.set_result( result );
        ack.set_matchid( kfmsg.matchid() );
        ack.set_playerid( pbplayer->id() );
        ack.set_serverid( KFGlobal::Instance()->_app_id->GetId() );
        auto ok = _kf_route->SendToRoute( route, KFMsg::S2S_START_MATCH_TO_GAME_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player=[{}] send match ack failed!", pbplayer->id() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleCancelMatchToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SCancelMatchToShardReq );

        __LOG_DEBUG__( "player=[{}] cancel match req!", kfmsg.playerid() );

        auto kfplayer = _match_player_manage->Find( kfmsg.playerid() );
        if ( kfplayer == nullptr )
        {
            return __LOG_ERROR__( "player=[{}] not in match!", kfmsg.playerid() );
        }

        auto kfroom = _match_room_list.Find( kfplayer->_room_id );
        if ( kfroom != nullptr )
        {
            kfroom->CancelMatch( kfplayer );
        }
        else
        {
            // 删除玩家
            auto kfqueue = FindMatchQueue( kfplayer->_match_id );
            kfqueue->CancelMatch( kfmsg.playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleAffirmMatchToShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAffirmMatchToShardReq );

        __LOG_DEBUG__( "player=[{}] affrim match req!", kfmsg.playerid() );

        auto kfplayer = _match_player_manage->Find( kfmsg.playerid() );
        if ( kfplayer == nullptr )
        {
            return __LOG_ERROR__( "can't find player=[{}]!", kfmsg.playerid() );
        }

        auto kfroom = _match_room_list.Find( kfplayer->_room_id );
        if ( kfroom == nullptr )
        {
            return __LOG_ERROR__( "can't find room=[{}]!", kfplayer->_room_id );
        }

        kfroom->PlayerAffirm( kfplayer->_id );
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleCreateRoomToMatchAck )
    {
        __PROTO_PARSE__( KFMsg::S2SCreateRoomToMatchAck );

        __LOG_DEBUG__( "room=[{}] create ack!", kfmsg.roomid() );

        auto kfroom = _match_room_list.Find( kfmsg.roomid() );
        if ( kfroom == nullptr )
        {
            return __LOG_ERROR__( "can't find room=[{}]!", kfmsg.roomid() );
        }

        kfroom->AffirmCreate();
    }

    __KF_MESSAGE_FUNCTION__( KFMatchShardModule::HandleQueryMatchToMatchReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryMatchToMatchReq );

        __LOG_DEBUG__( "player=[{}] query match req!", kfmsg.playerid() );
        auto kfplayer = _match_player_manage->Find( kfmsg.playerid() );
        if ( kfplayer != nullptr )
        {
            kfplayer->_pb_player.set_serverid( __ROUTE_SERVER_ID__ );
            auto kfroom = _match_room_list.Find( kfplayer->_room_id );
            if ( kfroom != nullptr )
            {
                kfroom->QueryRoom( kfplayer );
            }

            return __LOG_DEBUG__( "player=[{}] query in match!", kfmsg.playerid() );
        }

        KFMsg::S2SQueryMatchToGameAck ack;
        ack.set_playerid( kfmsg.playerid() );
        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_MATCH_TO_GAME_ACK, &ack );
    }
}