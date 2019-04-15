#include "KFMatchQueue.h"
#include "KFMatchShardModule.hpp"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////
    void KFMatchQueue::RemovePlayer( uint64 id )
    {
        _player_list.Remove( id );
    }

    void KFMatchQueue::AddPlayer( KFMatchPlayer* kfplayer )
    {
        kfplayer->Reset();
        _player_list.Insert( kfplayer->_id, kfplayer );
    }

    //////////////////////////////////////////////////////////////////////////////////
    void KFMatchQueue::StartMatch( const KFMsg::PBMatchPlayer* pbplayer, const std::string& version, uint64 battleserverid )
    {
        auto kfplayer = __KF_NEW__( KFMatchPlayer );
        kfplayer->_match_id = _match_id;
        kfplayer->_version = version;
        kfplayer->_battle_server_id = battleserverid;
        kfplayer->CopyFrom( pbplayer );
        _player_list.Insert( kfplayer->_id, kfplayer );
    }

    bool KFMatchQueue::CancelMatch( uint64 playerid )
    {
        RemovePlayer( playerid );

        return true;
    }

    //////////////////////////////////////////////////////////////////////////////////
    bool KFMatchQueue::IsMatched( KFMatchPlayer* firstplayer, KFMatchPlayer* secondplayer )
    {
        // 同一个人
        if ( firstplayer->_id == secondplayer->_id )
        {
            return false;
        }

        // 指定的战斗服不一致
        if ( firstplayer->_battle_server_id != secondplayer->_battle_server_id )
        {
            return false;
        }

        // 版本不一致
        if ( firstplayer->_version != secondplayer->_version )
        {
            return false;
        }

        return true;
    }

    std::tuple< bool, KFMatchPlayer*, KFMatchPlayer* > KFMatchQueue::RunMatchPlayer()
    {
        // demo版本,简单做匹配
        for ( auto fiter = _player_list._objects.begin(); fiter != _player_list._objects.end(); ++fiter )
        {
            auto firstplayer = fiter->second;

            for ( auto siter = _player_list._objects.begin(); siter != _player_list._objects.end(); ++siter )
            {
                auto secondplayer = siter->second;

                auto ok = IsMatched( firstplayer, secondplayer );
                if ( ok )
                {
                    return std::make_tuple( true, firstplayer, secondplayer );
                }
            }
        }

        return std::make_tuple( false, nullptr, nullptr );
    }

    void KFMatchQueue::RunMatch()
    {
        // demo版本,简单做匹配
        auto ok = false;
        do
        {
            auto retvalue = RunMatchPlayer();
            ok = std::get< 0 >( retvalue );
            if ( ok )
            {
                auto firstplayer = std::get< 1 >( retvalue );
                auto secondplayer = std::get< 2 >( retvalue );

                auto kfroom = _match_module->CreateMatchRoom( this, firstplayer->_version, firstplayer->_battle_server_id );

                // 红色阵营
                _player_list.Remove( firstplayer->_id, false );
                kfroom->AddPlayer( firstplayer, KFMsg::RedCamp );

                // 蓝色阵营
                _player_list.Remove( secondplayer->_id, false );
                kfroom->AddPlayer( secondplayer, KFMsg::BlueCamp );
            }
        } while ( ok );
    }
}