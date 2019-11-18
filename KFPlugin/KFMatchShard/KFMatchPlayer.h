#ifndef __KF_MATCH_PLAYER_H__
#define __KF_MATCH_PLAYER_H__

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    class KFMatchPlayer
    {
    public:
        ~KFMatchPlayer();

        // 重置状态
        void Reset();

        // 加载
        void CopyFrom( const KFMsg::PBMatchPlayer* pbplayer );

        // 保存
        void SaveTo( KFMsg::PBMatchPlayer* pbplayer );

        // 发送消息
        bool SendToGame( uint32 msgid, google::protobuf::Message* message );

        // 通知匹配结果
        void TellMatchResult( uint32 time );
    public:
        // 玩家id
        uint64 _id = 0;

        // 匹配id
        uint32 _match_id = 0u;

        // 所在房间
        uint64 _room_id = 0u;

        // 玩家的版本号
        std::string _version;

        // 指定的battleid
        uint64 _battle_server_id = 0u;

        // 游戏玩家数据
        KFMsg::PBMatchPlayer _pb_player;

        // 是否已经确认
        bool _is_affirm = false;
    };

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    class KFMatchPlayerManage : public KFSingleton< KFMatchPlayerManage >
    {
    public:
        // 添加玩家
        void Add( uint64 id, KFMatchPlayer* player );

        // 删除玩家
        void Remove( uint64 id );

        // 查找玩家
        KFMatchPlayer* Find( uint64 id );

    protected:
        // 玩家列表
        std::map< uint64, KFMatchPlayer* > _player_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    static auto _match_player_manage = KFMatchPlayerManage::Instance();
    //////////////////////////////////////////////////////////////////////////////////////
}

#endif