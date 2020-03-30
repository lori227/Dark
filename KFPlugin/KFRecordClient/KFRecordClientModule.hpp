#ifndef __KF_RECORD_CLIENT_MODULE_H__
#define __KF_RECORD_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    日报功能模块
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-07-26
************************************************************************/

#include "KFrameEx.h"
#include "KFRecordClientInterface.h"
#include "KFReset/KFResetInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFDisplay/KFNoticeInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFZConfig/KFRecordConfig.hpp"
#include "KFZConfig/KFProfessionConfig.hpp"

namespace KFrame
{
    class KFRecordClientModule : public KFRecordClientInterface
    {
    public:
        KFRecordClientModule() = default;
        ~KFRecordClientModule() = default;

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 营地纪录
        virtual void AddCampRecord( KFEntity* player, KFData* kfhero, uint64 time, uint32 type, uint64 value );

        // 添加死亡记录
        virtual void AddDeadRecord( KFEntity* player, KFData* kfhero );

        // 添加生平记录
        virtual void AddLifeRecord( KFEntity* player, uint64 uuid, uint32 eventid );

        // 删除生平记录
        virtual void DelLifeRecord( KFEntity* player, uint32 uuid );

    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterRecordModule );

        // 日报逻辑
        __KF_RESET_FUNCTION__( OnResetRecord );
    protected:

        // 客户端查询记录
        __KF_MESSAGE_FUNCTION__( HandleQueryRecordReq );

    protected:
        virtual void AddRecordValueFunction( const std::string& module, KFRecordValuesFunction& function );
        virtual void RemoveRecordValueFunction( const std::string& module );
        //////////////////////////////////////////////////////////////////////////////////////
        // 发送消息
        bool SendMessageToRecord( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message );

        // 添加记录
        void SendAddRecodToShard( KFEntity* player, uint32 recordtype, uint64 validtime, const KFDBValue& recorddata );
    protected:
        //日报收集函数
        KFBind< std::string, const std::string&, KFRecordValuesFunction > _record_value_list;
    };
}

#endif