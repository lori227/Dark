#ifndef __KF_MATCH_QUEUE_H__
#define __KF_MATCH_QUEUE_H__

#include "KFMatchRoom.h"

namespace KFrame
{
    class KFMatchShardModule;
    class KFMatchQueue
    {
    public:
        //////////////////////////////////////////////////////////////////////////////////////
        // 删除匹配玩家
        void RemovePlayer( uint64 id );

        // 添加玩家
        void AddPlayer( KFMatchPlayer* kfplayer );
        //////////////////////////////////////////////////////////////////////////////////////
        // 开始匹配
        void StartMatch( const KFMsg::PBMatchPlayer* pbplayer, const std::string& version, uint64 battleserverid );

        // 取消匹配
        bool CancelMatch( uint64 id );

        // 逻辑
        void RunMatch();

    protected:
        // 判断是否匹配
        bool IsMatched( KFMatchPlayer* firstplayer, KFMatchPlayer* secondplayer );

        // 匹配两个玩家
        std::tuple< bool, KFMatchPlayer*, KFMatchPlayer* > RunMatchPlayer();

    public:
        // 匹配id
        uint32 _match_id = 0u;

        // 匹配模式
        KFMatchShardModule* _match_module;

        // 玩家的列表
        KFHashMap< uint64, uint64, KFMatchPlayer > _player_list;
    };
}


#endif