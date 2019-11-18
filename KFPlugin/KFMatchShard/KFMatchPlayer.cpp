#include "KFMatchPlayer.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    void KFMatchPlayerManage::Add( uint64 id, KFMatchPlayer* player )
    {
        _player_list[ id ] = player;
    }

    void KFMatchPlayerManage::Remove( uint64 id )
    {
        _player_list.erase( id );
    }

    KFMatchPlayer* KFMatchPlayerManage::Find( uint64 id )
    {
        auto iter = _player_list.find( id );
        if ( iter == _player_list.end() )
        {
            return nullptr;
        }

        return iter->second;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    KFMatchPlayer::~KFMatchPlayer()
    {
        _match_player_manage->Remove( _id );
    }

    void KFMatchPlayer::Reset()
    {
        _room_id = 0u;
        _is_affirm = false;
        _pb_player.set_campid( 0 );
    }

    void KFMatchPlayer::CopyFrom( const KFMsg::PBMatchPlayer* pbplayer )
    {
        _id = pbplayer->id();
        _pb_player.CopyFrom( *pbplayer );
        _match_player_manage->Add( _id, this );
    }

    void KFMatchPlayer::SaveTo( KFMsg::PBMatchPlayer* pbplayer )
    {
        pbplayer->CopyFrom( _pb_player );
    }

    bool KFMatchPlayer::SendToGame( uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_route->SendToPlayer( _pb_player.serverid(), _id, msgid, message, true );
    }

    void KFMatchPlayer::TellMatchResult( uint32 time )
    {
        KFMsg::MsgInformMatchResult result;
        result.set_time( time );
        SendToGame( KFMsg::MSG_INFORM_MATCH_RESULT, &result );
    }

}