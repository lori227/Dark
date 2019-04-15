#include "KFBattleRoom.h"
#include "KFBattleAllot.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    void KFBattleRoom::Init( uint64 roomid )
    {
        _id = roomid;
        ChangeState( AllotState, 1000 );
    }

    bool KFBattleRoom::Run()
    {
        if ( _timer.DoneTimer() )
        {
            switch ( _state )
            {
            case AllotState:
                RunAllotBattle();
                break;
            case OpenState:
                RunOpenBattle();
                break;
            case InformState:
                RunInformPlayer();
                break;
            case PlayState:
                break;
            default:
                break;
            }
        }

        // 发送通知消息到玩家
        RunSendInformToPlayer();
        return IsValid();
    }

    bool KFBattleRoom::IsValid()
    {
        // 有效时间过了
        if ( KFGlobal::Instance()->_game_time > _valid_time )
        {
            __LOG_ERROR__( "room=[{}] valid timeout!", _id );

            SendFinishToPlayer();
            return false;
        }

        if ( _heartbeat_timeout > 0 && KFGlobal::Instance()->_game_time > _heartbeat_timeout )
        {
            __LOG_ERROR__( "room=[{}] heartbeat timeout!", _id );

            SendFinishToPlayer();
            return false;
        }

        return _state != DestoryState;
    }

    void KFBattleRoom::UpdateHeartBeatTime()
    {
        // 3分钟没收收到消息, 认为战场服务器挂了
        _heartbeat_timeout = KFGlobal::Instance()->_game_time + 180000;
    }

    void KFBattleRoom::ChangeState( uint32 state, uint32 time )
    {
        _state = state;

        _timer.StopTimer();
        _timer.StartTimer( KFGlobal::Instance()->_game_time, time );

        __LOG_DEBUG__( "room=[{}] state=[{}] time=[{}]", _id, _state, time );
    }

    void KFBattleRoom::SendToRoom( uint32 msgid, google::protobuf::Message* message )
    {
        for ( auto& iter : _player_list._objects )
        {
            auto kfplayer = iter.second;
            kfplayer->SendToGame( msgid, message );
        }
    }

    void KFBattleRoom::RunAllotBattle()
    {
        std::tie( _allot_id, _allot_ip, _allot_port ) = _battle_allot->AllotBattle( _battle_server_id, _version );
        if ( _allot_id == _invalid_int || _allot_ip.empty() || _allot_port == _invalid_int )
        {
            ChangeState( AllotState, 10000 );

            // 发送消息到房间
            for ( auto& iter : _player_list._objects )
            {
                auto kfplayer = iter.second;
                _kf_display->SendToPlayer( kfplayer->_pb_player.serverid(), kfplayer->_id, KFMsg::RoomAllotBattle );
            }
        }
        else
        {
            _battle_open_count = _invalid_int;
            ChangeState( OpenState, 10 );
        }
    }

    void KFBattleRoom::RunOpenBattle()
    {
        if ( _battle_open_count > 10u )
        {
            return ChangeState( AllotState, 10 );
        }

        ++_battle_open_count;
        ChangeState( OpenState, 5000 );

        // 发送消息到战场服务器
        KFMsg::S2SOpenRoomToBattleReq req;
        req.set_roomid( _id );
        req.set_matchid( _match_id );
        req.set_roomserverid( KFGlobal::Instance()->_app_id->GetId() );
        for ( auto& iter : _player_list._objects )
        {
            auto kfplayer = iter.second;
            kfplayer->SaveTo( req.add_pbplayer() );
        }
        _kf_route->SendToServer( _allot_id, KFMsg::S2S_OPEN_ROOM_TO_BATTLE_REQ, &req );
    }

    void KFBattleRoom::AffirmOpenBattle( bool ok )
    {
        if ( !ok )
        {
            ChangeState( OpenState, 10 );
        }
        else
        {
            UpdateHeartBeatTime();
            ChangeState( InformState, 10 );
        }
    }

    void KFBattleRoom::RunInformPlayer()
    {
        ChangeState( PlayState, 1000 );

        // 把玩家添加到通知列表
        for ( auto& iter : _player_list._objects )
        {
            AddInformPlayer( iter.second );
        }
    }

    void KFBattleRoom::AddInformPlayer( KFBattlePlayer* kfplayer )
    {
        // 发送通知
        SendInformToPlayer( kfplayer );

        // 添加进列表
        _inform_list[ kfplayer ] = KFGlobal::Instance()->_game_time + 6000;
    }

    void KFBattleRoom::RunSendInformToPlayer()
    {
        for ( auto& iter : _inform_list )
        {
            if ( KFGlobal::Instance()->_game_time >= iter.second )
            {
                SendInformToPlayer( iter.first );
                iter.second = KFGlobal::Instance()->_game_time + 6000;
            }
        }
    }

    void KFBattleRoom::SendInformToPlayer( KFBattlePlayer* kfplayer )
    {
        KFMsg::S2SInformBattleToGameReq req;
        req.set_playerid( kfplayer->_id );
        req.set_roomid( _id );
        req.set_battleid( _allot_id );
        req.set_ip( _allot_ip );
        req.set_port( _allot_port );
        kfplayer->SendToGame( KFMsg::S2S_INFORM_BATTLE_TO_GAME_REQ, &req );

        __LOG_DEBUG__( "inform player=[{}] battle=[{}|{}:{}]", kfplayer->_id, KFAppId::ToString( _allot_id ), _allot_ip, _allot_port );
    }

    void KFBattleRoom::AffirmInformPlayer( uint64 playerid )
    {
        auto kfplayer = _player_list.Find( playerid );
        if ( kfplayer == nullptr )
        {
            return;
        }

        _inform_list.erase( kfplayer );
    }

    bool KFBattleRoom::QueryRoom( uint64 playerid, uint64 serverid )
    {
        auto kfplayer = _player_list.Find( playerid );
        if ( kfplayer == nullptr )
        {
            return false;
        }

        kfplayer->_pb_player.set_serverid( serverid );
        AddInformPlayer( kfplayer );
        return true;
    }

    void KFBattleRoom::FinishRoom()
    {
        // 通知房间
        ChangeState( DestoryState, 100 );

        // 发送结束消息给玩家
        SendFinishToPlayer();
    }

    void KFBattleRoom::SendFinishToPlayer()
    {
        for ( auto& iter : _player_list._objects )
        {
            auto kfplayer = iter.second;

            KFMsg::S2SFinishRoomToGameReq req;
            req.set_roomid( _id );
            req.set_playerid( kfplayer->_id );
            kfplayer->SendToGame( KFMsg::S2S_FINISH_ROOM_TO_GAME_REQ, &req );
        }
    }
}