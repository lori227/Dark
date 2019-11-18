#ifndef __KF_BATTLE_ROOM_H__
#define __KF_BATTLE_ROOM_H__

#include "KFBattlePlayer.h"

namespace KFrame
{
    class KFBattleRoom
    {
    protected:
        enum MyEnum
        {
            AllotState = 0,		// 分配状态
            OpenState = 1,		// 打开状态
            InformState = 2,	// 通知状态
            PlayState = 3,		// 战斗状态
            DestoryState = 4,	// 销毁状态
        };

    public:
        // 初始化
        void Init( uint64 roomid );

        // 逻辑
        bool Run();

        // 发送消息
        void SendToRoom( uint32 msgid, google::protobuf::Message* message, bool resend );

        // 确认开启战场
        void AffirmOpenBattle( bool ok );

        // 确认通知玩家
        void AffirmInformPlayer( uint64 playerid );

        // 查询房间
        bool QueryRoom( uint64 playerid, uint64 serverid );

        // 结束房间
        void FinishRoom();

        // 设置房间心跳超时时间
        void UpdateHeartBeatTime();
    protected:
        // 是否有效
        bool IsValid();

        // 改变状态
        void ChangeState( uint32 state, uint32 time, const char* function, uint32 line );

        // 分配战场
        void RunAllotBattle();

        // 开启战场
        void RunOpenBattle();

        // 通知玩家
        void RunInformPlayer();

        // 添加通知玩家
        void AddInformPlayer( KFBattlePlayer* kfplayer );

        // 发送通知消息
        void RunSendInformToPlayer();
        void SendInformToPlayer( KFBattlePlayer* kfplayer );

        // 发送结束消息
        void SendFinishToPlayer();

    public:
        // 房间id
        uint64 _id = _invalid_int;

        // 匹配id
        uint32 _match_id = _invalid_int;

        // 版本号
        std::string _version;

        // 指定的服务器id
        uint64 _battle_server_id = _invalid_int;

        // 玩家的列表
        KFHashMap< uint64, uint64, KFBattlePlayer > _player_list;

        // 有效时间
        uint64 _valid_time = _invalid_int;
    private:
        // 状态
        uint32 _state = AllotState;

        // 定时器
        KFTimer _timer;

        // 分配的服务器id
        uint64 _allot_id = _invalid_int;

        // 分配的服务器ip
        std::string _allot_ip;
        uint32 _allot_port = _invalid_int;

        // 战场开启次数
        uint32 _battle_open_count = _invalid_int;

        // 通知列表
        std::map< KFBattlePlayer*, uint64 > _inform_list;

        // 心跳超时时间
        uint64 _heartbeat_timeout = _invalid_int;
    };
}


#endif