#ifndef __KF_GRANARY_MOUDLE_H__
#define __KF_GRANARY_MOUDLE_H__

/************************************************************************
//    @Moudle			:    粮仓系统
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-10-22
************************************************************************/

#include "KFrameEx.h"
#include "KFGranaryInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFItem/KFItemInterface.h"
#include "KFRecordClient/KFRecordClientInterface.h"
#include "KFGranaryConfig.hpp"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFGranaryModule : public KFGranaryInterface
    {
    public:
        KFGranaryModule() = default;
        ~KFGranaryModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterGranaryModule );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveGranaryModule );

        // 获取天总产量
        __KF_RECORD_VALUE_FUNCTION__( GetDayGranaryTotalNum );

        // 粮仓等级解锁
        __KF_ADD_DATA_FUNCTION__( OnAddGranaryBuild );

        // 物品数量更新
        __KF_UPDATE_DATA_FUNCTION__( OnItemNumUpdate );

        // 收获请求
        __KF_MESSAGE_FUNCTION__( HandleGranaryGatherReq );

        // 定时增加道具
        __KF_TIMER_FUNCTION__( OnTimerAddItem );

    protected:
        // 获取粮仓等级
        uint32 GetGranaryLevel( KFEntity* player );

        // 粮仓是否激活
        bool IsGranaryActive( KFEntity* player );

        // 获取粮仓配置
        const KFGranarySetting* GetGranarySetting( KFEntity* player );

        // 检查定时器
        void CheckGranaryTimer( KFEntity* player );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif