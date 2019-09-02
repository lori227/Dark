#ifndef __KF_ROOM_SHARD_MOUDLE_H__
#define __KF_ROOM_SHARD_MOUDLE_H__

/************************************************************************
//    @Moudle			:    房间系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-3-21
************************************************************************/
#include "KFrameEx.h"
#include "KFBattleRoom.h"
#include "KFRoomShardInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    class KFRoomShardModule : public KFRoomShardInterface
    {
    public:
        KFRoomShardModule() = default;
        ~KFRoomShardModule() = default;

        // 逻辑
        virtual void BeforeRun();
        virtual void PrepareRun();
        virtual void Run();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 注册战场
        __KF_MESSAGE_FUNCTION__( HandleRegisterBattleToRoomReq );

        // 同步战场
        __KF_MESSAGE_FUNCTION__( HandleSyncBattleToRoomReq );

        // 创建房间
        __KF_MESSAGE_FUNCTION__( HandleCreateRoomToRoomReq );

        // 开启房间
        __KF_MESSAGE_FUNCTION__( HandleOpenRoomToRoomAck );

        // 通知房间
        __KF_MESSAGE_FUNCTION__( HandleInformBattleToRoomAck );

        // 查询房间
        __KF_MESSAGE_FUNCTION__( HandleQueryRoomToRoomReq );

        // 结束房间
        __KF_MESSAGE_FUNCTION__( HandleFinishRoomToRoomReq );

        // 房间心跳
        __KF_MESSAGE_FUNCTION__( HandleHeartBeatToRoomReq );

    protected:
        // 创建战场房间
        KFBattleRoom* CreateBattleRoom( uint64 roomid );

        // 删除战场房间
        void RemoveBattleRoom( uint64 roomid );

    private:
        // 房间列表
        KFHashMap< uint64, uint64, KFBattleRoom > _room_list;

        // battle redis
        KFRedisDriver* _room_redis = nullptr;
    };
}



#endif