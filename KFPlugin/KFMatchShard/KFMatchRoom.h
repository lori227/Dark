#ifndef __KF_MATCH_ROOM_H__
#define __KF_MATCH_ROOM_H__

#include "KFMatchPlayer.h"

namespace KFrame
{
    class KFMatchQueue;
    class KFMatchRoom
    {
    protected:
        enum MyEnum
        {
            InitState = 0,		// 初始
            AffirmState = 1,	// 确认状态
            CreateState = 2,	// 创建room
            DestroyState = 3,	// 销毁
        };

    public:
        // 初始化
        void Initialize( KFMatchQueue* kfqueue, const std::string& version, uint64 battleserverid );

        // 逻辑
        bool Run();

        // 添加玩家
        void AddPlayer( KFMatchPlayer* kfplayer, uint32 campid );

        // 玩家确认
        void PlayerAffirm( uint64 playerid );

        // 确认创建
        void AffirmCreate();

        // 玩家查询
        void QueryRoom( KFMatchPlayer* kfplayer );

        // 取消匹配
        void CancelMatch( KFMatchPlayer* kfplayer );
    protected:
        // 是否有效
        bool IsValid();

        // 改变状态
        void ChangeState( uint32 state, uint32 time );

        // 发送确认消息
        void AffirmToPlayer();
        void SendAffirmToPlayer();

        // 确认超时
        void AffirmTimeout();

        // 创建房间
        void CreateRoom();

    public:
        // 房间id
        uint64 _id = _invalid_int;

        // 玩家的列表
        KFHashMap< uint64, uint64, KFMatchPlayer > _player_list;

    private:
        // 版本号
        std::string _version;

        // 房间id
        uint64 _battle_server_id = _invalid_int;

        // 状态
        uint32 _state = _invalid_int;

        // 定时器
        KFTimer _timer;

        // 匹配队列
        KFMatchQueue* _match_queue;

        // 下次通知时间
        uint64 _next_affirm_time = _invalid_int;
    };
}


#endif