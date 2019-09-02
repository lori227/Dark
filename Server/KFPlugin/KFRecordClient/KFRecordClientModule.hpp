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
#include "KFDisplay/KFNoticeInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFReset/KFResetInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFZConfig/KFRecordConfig.hpp"

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
        void SendAddRecodToShard( KFEntity* player, uint32 recordtype, uint64 validtime, const MapString& recorddata );
    protected:
        //日报收集函数
        KFBind< std::string, const std::string&, KFRecordValuesFunction > _record_value_list;
    };
}

#endif