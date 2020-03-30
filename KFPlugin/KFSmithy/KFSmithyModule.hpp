#ifndef __KF_SMITHY_MOUDLE_H__
#define __KF_SMITHY_MOUDLE_H__

/************************************************************************
//    @Moudle			:    铁匠铺系统
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-10-25
************************************************************************/

#include "KFrameEx.h"
#include "KFSmithyInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRecordClient/KFRecordClientInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFSmithyWeaponConfig.hpp"
#include "KFZConfig/KFItemConfig.hpp"

namespace KFrame
{
    class KFSmithyModule : public KFSmithyInterface
    {
    public:
        KFSmithyModule() = default;
        ~KFSmithyModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterSmithyModule );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveSmithyModule );

        // 获取天总产量
        __KF_RECORD_VALUE_FUNCTION__( GetDaySmithyTotalNum );

        // 添加材料数量
        __KF_ADD_ELEMENT_FUNCTION__( AddSmithyElement );

        // 铁匠铺解锁
        __KF_ADD_DATA_FUNCTION__( OnAddSmithyBuild );

        // 物品数量更新
        __KF_UPDATE_DATA_FUNCTION__( OnItemNumUpdate );

        // 收获请求
        __KF_MESSAGE_FUNCTION__( HandleSmithyGatherReq );

        // 打造请求
        __KF_MESSAGE_FUNCTION__( HandleSmithyMakeReq );

        // 定时增加道具
        __KF_TIMER_FUNCTION__( OnTimerAddItem );

        // 增加数据
        __KF_EXECUTE_FUNCTION__( OnExecuteSmithyAddData );

        // 打造等级限制
        __KF_EXECUTE_FUNCTION__( OnExecuteSmithyMakeLevel );

    protected:
        // 获取铁匠铺等级
        uint32 GetSmithyLevel( KFEntity* player );

        // 铁匠铺是否激活
        bool IsSmithyActive( KFEntity* player );

        // 检查定时器
        void CheckSmithyTimer( KFEntity* player );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif