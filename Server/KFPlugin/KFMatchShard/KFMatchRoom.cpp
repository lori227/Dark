#include "KFMatchRoom.h"
#include "KFMatchQueue.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    void KFMatchRoom::Initialize( KFMatchQueue* kfqueue, const std::string& version, uint64 battleserverid )
    {
        _version = version;
        _match_queue = kfqueue;
        _battle_server_id = battleserverid;
        _id = KFGlobal::Instance()->MakeUUID();

        ChangeState( InitState, 1000 );
    }

    void KFMatchRoom::ChangeState( uint32 state, uint32 time )
    {
        _state = state;

        _timer.StopTimer();
        _timer.StartTimer( KFGlobal::Instance()->_game_time, time );

        __LOG_DEBUG__( "room=[{}] state=[{}] time=[{}]", _id, _state, time );
    }

    void KFMatchRoom::AddPlayer( KFMatchPlayer* kfplayer, uint32 campid )
    {
        kfplayer->_room_id = _id;
        kfplayer->_pb_player.set_campid( campid );
        _player_list.Insert( kfplayer->_id, kfplayer );
    }

    bool KFMatchRoom::IsValid()
    {
        return _state != DestroyState;
    }

    bool KFMatchRoom::Run()
    {
        if ( _timer.DoneTimer() )
        {
            switch ( _state )
            {
            case InitState:
                AffirmToPlayer();
                break;
            case AffirmState:
                AffirmTimeout();
                break;
            case CreateState:
                CreateRoom();
                break;
            default:
                break;
            }
        }

        return IsValid();
    }

    void KFMatchRoom::AffirmToPlayer()
    {
        // 等待时间
        static uint32 _affirm_time = 1000u;

        ChangeState( AffirmState, ( _affirm_time + 1 ) * 1000 );
        //////////////////////////////////////////////////////
        // 发送消息
        for ( auto& iter : _player_list._objects )
        {
            auto player = iter.second;
            player->TellMatchResult( _affirm_time );
        }
    }

    void KFMatchRoom::QueryRoom( KFMatchPlayer* kfplayer )
    {
        if ( _state == AffirmState )
        {
            auto lefttime = _timer.GetLeftKeepTime( KFGlobal::Instance()->_game_time );
            kfplayer->TellMatchResult( lefttime );
        }
    }

    void KFMatchRoom::AffirmTimeout()
    {
        // 销毁房间
        ChangeState( DestroyState, 1000 );
        //////////////////////////////////////////////////////

        std::set< uint64 > _removes;
        for ( auto& iter : _player_list._objects )
        {
            auto player = iter.second;
            if ( player->_is_affirm )
            {
                _removes.insert( player->_id );
                _match_queue->AddPlayer( player );
            }
            else
            {
                KFMsg::S2SAffirmMatchTimeoutToGame msg;
                msg.set_playerid( player->_id );
                player->SendToGame( KFMsg::S2S_AFFIRM_MATCH_TIMEOUT_TO_GAME, &msg );
            }
        }

        for ( auto id : _removes )
        {
            _player_list.Remove( id, false );
        }
    }

    void KFMatchRoom::PlayerAffirm( uint64 playerid )
    {
        auto player = _player_list.Find( playerid );
        if ( player == nullptr )
        {
            return;
        }

        player->_is_affirm = true;

        // 判断是否全部确认
        for ( auto& iter : _player_list._objects )
        {
            auto player = iter.second;
            if ( !player->_is_affirm )
            {
                return;
            }
        }

        // 1秒后创建房间
        ChangeState( CreateState, 1000 );
    }

    void KFMatchRoom::CreateRoom()
    {
        ChangeState( CreateState, 5000 );

        KFMsg::S2SCreateRoomToRoomReq req;
        req.set_roomid( _id );
        req.set_version( _version );
        req.set_serverid( _battle_server_id );
        req.set_matchid( _match_queue->_match_id );

        for ( auto& iter : _player_list._objects )
        {
            auto player = iter.second;
            player->SaveTo( req.add_pbplayer() );
        }

        _kf_route->SendToBalance( __KF_STRING__( room ), KFMsg::S2S_CREATE_ROOM_TO_ROOM_REQ, &req );
    }

    void KFMatchRoom::AffirmCreate()
    {
        ChangeState( DestroyState, 100 );
    }

    void KFMatchRoom::CancelMatch( KFMatchPlayer* kfplayer )
    {
        // 有人取消, 直接消费
        ChangeState( DestroyState, 100 );
    }
}