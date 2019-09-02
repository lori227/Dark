#ifndef __KF_RECORD_SHARD_MODULE_H__
#define __KF_RECORD_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    日报数据模块
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-07-26
************************************************************************/

#include "KFrameEx.h"
#include "KFRecordShardInterface.h"
#include "KFMongo/KFMongoInterface.h"
#include "KFDisplay/KFNoticeInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFZConfig/KFRecordConfig.hpp"

namespace KFrame
{
    class KFRecordShardModule : public KFRecordShardInterface
    {
    public:
        KFRecordShardModule() = default;
        ~KFRecordShardModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void PrepareRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 添加记录
        __KF_MESSAGE_FUNCTION__( HandleAddRecordReq );

        // 查询记录
        __KF_MESSAGE_FUNCTION__ ( HandleQueryRecordReq );

        // 记录上线逻辑
        __KF_MESSAGE_FUNCTION__( HandleOnlineRecordReq );

        ///////////////////////////////////////////////////////////////////////////////
    private:
        // mongo数据库
        KFMongoDriver* _mongo_driver = nullptr;
    };
}

#endif