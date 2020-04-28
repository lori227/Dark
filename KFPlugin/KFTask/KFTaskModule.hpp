﻿#ifndef __KF_TASK_MOUDLE_H__
#define __KF_TASK_MOUDLE_H__

/************************************************************************
//    @Module			:    任务系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-10-31
************************************************************************/

#include "KFrameEx.h"
#include "KFTaskInterface.h"
#include "KFTaskChainInterface.h"
#include "KFDrop/KFDropInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFCondition/KFConditionInterface.h"
#include "KFTaskConfig.hpp"
#include "KFTaskChainConfig.hpp"

namespace KFrame
{
    class KFTaskModule : public KFTaskInterface
    {
    public:
        KFTaskModule() = default;
        ~KFTaskModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        // 开启时限任务
        virtual KFData* OpenTask( KFEntity* player, uint32 taskid, uint32 status, uint64 time );

        // 开启任务链中的任务
        virtual KFData* OpenTask( KFEntity* player, uint32 taskid, uint32 status, uint64 time, uint32 refreshid, uint32 taskchainid, uint32 order, const UInt32List& logicids );
    protected:
        // 接取任务
        __KF_MESSAGE_FUNCTION__( HandleTaskReceiveReq );

        // 领取任务奖励
        __KF_MESSAGE_FUNCTION__( HandleTaskRewardReq );

        // 放弃任务
        __KF_MESSAGE_FUNCTION__( HandleTaskRemoveReq );

        // 添加任务
        __KF_MESSAGE_FUNCTION__( HandleTaskAddReq );
    protected:
        // 添加任务
        __KF_ADD_ELEMENT_FUNCTION__( AddTaskElement );

        // 进入游戏检查任务
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterTaskModule );
        // 离开游戏删除定时器
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveTaskModule );

        // 任务超时定时器
        __KF_TIMER_FUNCTION__( OnTimerTaskTimeout );
        // 设置任务时间
        void SetTaskTime( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting, uint64 time, bool update );
        // 启动任务超时定时器
        void StartTaskTimeoutTimer( KFEntity* player, uint32 taskid, uint64 time );
        void StopTaskTimeoutTimer( KFEntity* player, KFData* kftask, uint32 taskid );

        // 开启任务
        KFData* OpenTask( KFEntity* player, const KFTaskSetting* kfsetting, uint32 status, uint64 time, uint32 refreshid );
        void UpdataTaskStatus( KFEntity* player, const KFTaskSetting* kfsetting, KFData* kftask, uint32 status, uint64 time );

        // 任务条件完成
        void DoneTask( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting, bool update );

        // 添加到完成任务列表
        void AddFinishTask( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting, bool update );
        __KF_TIMER_FUNCTION__( OnTimerTaskFinish );

        // 任务交付完成
        void FinishTask( KFEntity* player, uint32 taskid );
        void FinishTask( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting );

        // 任务输出执行
        void TaskExecute( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting );

        // 更新数据回调
        __KF_ADD_DATA_FUNCTION__( OnAddDataTaskModule );
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveDataTaskModule );
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataTaskModule );

        // 删除任务回调
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveTask );

        // 判断任务是否能更新
        uint32 CheckTaskUpdateStatus( KFData* kftask, const KFTaskSetting* kfsetting );
        uint32 CheckTaskCanUpdate( KFEntity* player, KFData* kftask, const KFTaskSetting* kfsetting );

        // 初始化任务条件
        void InitTaskCondition( KFEntity* player, const KFTaskSetting* kfsetting, KFData* kftask, uint32 status, bool update );

        // 执行更新任务状态
        __KF_EXECUTE_FUNCTION__( OnExecuteUpdateTaskStatus );
        // 执行更新任务条件
        __KF_EXECUTE_FUNCTION__( OnExecuteUpdateTaskCondition );

        // 判断是否在秘境探索中
        bool IsPlayerInRealm( KFEntity* player );

        // 判断任务区域
        bool CheckTaskArea( KFEntity* player, const KFTaskSetting* kfsetting );

        // 秘境状态改变任务提交完成
        __KF_UPDATE_DATA_FUNCTION__( OnRealmTaskFinish );
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}

#endif