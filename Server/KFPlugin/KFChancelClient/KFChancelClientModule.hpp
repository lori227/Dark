#ifndef __KF_CHANCEL_CLIENT_MODULE_H__
#define __KF_CHANCEL_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    英雄圣坛
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-08-21
************************************************************************/

#include "KFrameEx.h"
#include "KFChancelClientInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFZConfig/KFChancelConfig.hpp"
#include "KFZConfig/KFProfessionConfig.hpp"

namespace KFrame
{
    class KFMailSetting;
    class KFChancelClientModule : public KFChancelClientInterface
    {
    public:
        KFChancelClientModule() = default;
        ~KFChancelClientModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
    protected:

        // 添加英雄
        __KF_ADD_DATA_FUNCTION__( OnAddHero );

        // 删除英雄
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveHero );

        // 客户端查询死亡记录
        __KF_MESSAGE_FUNCTION__( HandleQueryDeadRecordReq );

        // 查询死亡记录返回
        __KF_MESSAGE_FUNCTION__( HandleQueryDeadRecordAck );

        // 客户端查询生平事件
        __KF_MESSAGE_FUNCTION__( HandleQueryLifeEventReq );

        // 查询生平事件返回
        __KF_MESSAGE_FUNCTION__( HandleQueryLifeEventAck );

    protected:
        //////////////////////////////////////////////////////////////////////////////////////
        // 删除生平事件
        void DelLifeEvent( KFEntity* player, uint64 uuid );

        // 添加生平事件
        void AddLifeEvent( KFEntity* player, uint64 uuid, uint32 id );

        // 更新英雄死亡数据
        void UpdateDeadData( KFEntity* player, KFData* kfhero );

        // 更新死亡数量
        void UpdateDeadNum( KFEntity* player, uint32 type );

        // 更新死亡等级
        void UpdateDeadLevel( KFEntity* player, uint32 type );

        // 添加死亡记录
        void AddDeadRecord( KFEntity* player, uint64 uuid, KFData* kfhero );

        // 发送添加记录
        bool SendAddRecodToShard( KFEntity* player, const MapString& recorddata );

        // 发送查询记录
        void SendQueryRecordMessage( KFEntity* player );

        // 发送查询事件
        void SendQueryEventMessage( KFEntity* player, uint64 uuid );

        // 发送消息
        bool SendMessageToChancel( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message );

    protected:
        KFComponent* _kf_component = nullptr;
    };
}

#endif