#include "KFBattlePlayer.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////
    void KFBattlePlayer::CopyFrom( const KFMsg::PBMatchPlayer* pbplayer )
    {
        _id = pbplayer->id();
        _pb_player.CopyFrom( *pbplayer );
    }

    void KFBattlePlayer::SaveTo( KFMsg::PBMatchPlayer* pbplayer )
    {
        pbplayer->CopyFrom( _pb_player );
    }

    bool KFBattlePlayer::SendToGame( uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_route->SendToPlayer( _pb_player.serverid(), _id, msgid, message );
    }


}