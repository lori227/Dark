#ifndef __KF_CHANCEL_SHARD_MODULE_H__
#define __KF_CHANCEL_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    英雄圣坛
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-08-21
************************************************************************/

#include "KFrameEx.h"
#include "KFChancelShardInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFOption/KFOptionInterface.h"

namespace KFrame
{
    class KFChancelShardModule : public KFChancelShardInterface
    {
    public:
        KFChancelShardModule() = default;
        ~KFChancelShardModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void PrepareRun();

        // 关闭
        virtual void BeforeShut ();
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 添加死亡记录
        __KF_MESSAGE_FUNCTION__( HandleAddDeadRecordReq );

        // 查询死亡记录
        __KF_MESSAGE_FUNCTION__ ( HandleQueryDeadRecordReq );

        // 添加生平事件
        __KF_MESSAGE_FUNCTION__( HandleAddLifeEventReq );

        // 删除生平事件
        __KF_MESSAGE_FUNCTION__( HandleDelLifeEventReq );

        // 查询生平事件
        __KF_MESSAGE_FUNCTION__( HandleQueryLifeEventReq );

        ///////////////////////////////////////////////////////////////////////////////
    protected:
        // 添加死亡记录
        uint64 AddDeadRecord( uint64 playerid, MapString& recorddata );

    private:
        // 英雄圣坛数据库
        KFRedisDriver* _chancel_driver = nullptr;
    };
}

#endif