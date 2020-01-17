#ifndef __KF_MATCH_SHARD_MOUDLE_H__
#define __KF_MATCH_SHARD_MOUDLE_H__

/************************************************************************
//    @Moudle			:    匹配系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-3-21
************************************************************************/
#include "KFrameEx.h"
#include "KFMatchQueue.h"
#include "KFMatchShardInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFZConfig/KFMatchConfig.hpp"
#include "KFOption/KFOptionInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    class KFMatchShardModule : public KFMatchShardInterface
    {
    public:
        KFMatchShardModule() = default;
        ~KFMatchShardModule() = default;

        // 逻辑
        virtual void BeforeRun();
        virtual void PrepareRun();
        virtual void Run();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 创建房间
        KFMatchRoom* CreateMatchRoom( KFMatchQueue* kfqueue, const std::string& version, uint64 battleserverid );

    protected:
        // 请求开始匹配
        __KF_MESSAGE_FUNCTION__( HandleStartMatchToShardReq );

        // 请求取消匹配
        __KF_MESSAGE_FUNCTION__( HandleCancelMatchToShardReq );

        // 请求确认房间
        __KF_MESSAGE_FUNCTION__( HandleAffirmMatchToShardReq );

        // 创建房间
        __KF_MESSAGE_FUNCTION__( HandleCreateRoomToMatchAck );

        // 查询匹配
        __KF_MESSAGE_FUNCTION__( HandleQueryMatchToMatchReq );

    protected:
        // 查找匹配模式
        KFMatchQueue* FindMatchQueue( const KFMatchSetting* kfsetting );

        // 开始匹配
        uint32 StartMatch( const KFMsg::PBMatchPlayer* pbplayer, uint32 matchid, const std::string& version, uint64 battleserverid );
    private:
        // 版本列表
        StringSet _battle_version_list;

        // 匹配模式列表
        KFHashMap< uint32, uint32, KFMatchQueue > _match_queue_list;

        // 匹配的房间
        KFHashMap< uint64, uint64, KFMatchRoom > _match_room_list;
    };
}



#endif