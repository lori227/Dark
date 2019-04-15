#ifndef __KF_BATTLE_PLAYER_H__
#define __KF_BATTLE_PLAYER_H__

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    class KFBattlePlayer
    {
    public:
        // 加载
        void CopyFrom( const KFMsg::PBMatchPlayer* pbplayer );

        // 保存
        void SaveTo( KFMsg::PBMatchPlayer* pbplayer );

        // 发送消息
        bool SendToGame( uint32 msgid, google::protobuf::Message* message );

    public:
        // 玩家id
        uint64 _id = 0u;

        // 游戏玩家数据
        KFMsg::PBMatchPlayer _pb_player;
    };
}

#endif