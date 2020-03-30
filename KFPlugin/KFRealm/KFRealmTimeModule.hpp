#ifndef __KF_REALM_TIME_MOUDLE_H__
#define __KF_REALM_TIME_MOUDLE_H__

/************************************************************************
//    @Module			:    秘境时间系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2020-3-19
************************************************************************/

#include "KFrameEx.h"
#include "KFRealmData.hpp"
#include "KFPVEInterface.h"
#include "KFRealmInterface.h"
#include "KFRealmTimeInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFZConfig/KFRealmTimeConfig.hpp"
#include "KFZConfig/KFRealmConfig.hpp"
#include "KFZConfig/KFPVEConfig.hpp"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////
    class KFRealmTimeModule : public KFRealmTimeInterface
    {
    public:
        KFRealmTimeModule() = default;
        ~KFRealmTimeModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        // 秘境进入
        __KF_REALM_ENTER_FUNCTION__( OnReamlEnterTimeModule );

        // 秘境移动
        __KF_REALM_MOVE_FUNCTION__( OnRealmMoveTimeModule );

        // 战斗开始
        __KF_PVE_START_FUNCTION__( OnPVEStartTimeModule );

        // 战斗结束
        __KF_PVE_FINISH_FUNCTION__( OnPVEFinishTimeModule );

        // 回合结束
        __KF_TURN_START_FUNCTION__( OnPVETurnStartTimeModule );
    protected:
        // 请求天气更新
        __KF_MESSAGE_FUNCTION__( HandleUpdateTimeReq );

    protected:
        // 获得时间配置
        const KFRealmTimeSetting* FindRealmTimeSetting( KFRealmData* kfrealmdata, KFRealmData* kfpvedata );

        // 发送更新时间消息
        void SendUpdateRealmTimeToClient( KFEntity* player, KFRealmData* kfrealmdata );
    };
}

#endif