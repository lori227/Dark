﻿#ifndef __KF_TASK_CHAIN_MOUDLE_H__
#define __KF_TASK_CHAIN_MOUDLE_H__

/************************************************************************
//    @Module			:    任务链系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-10-31
************************************************************************/

#include "KFrameEx.h"
#include "KFTaskInterface.h"
#include "KFTaskChainInterface.h"
#include "KFReset/KFResetInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFCommand/KFCommandInterface.h"
#include "KFCondition/KFConditionInterface.h"
#include "KFTaskChainConfig.hpp"

namespace KFrame
{
    class KFTaskChainModule : public KFTaskChainInterface
    {
    public:
        KFTaskChainModule() = default;
        ~KFTaskChainModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        // 完成任务链任务
        virtual void FinishTaskChain( KFEntity* player, KFData* kftask, const char* function, uint32 line );

        // 删除任务链任务
        virtual void RemoveTaskChain( KFEntity* player, KFData* kftask );
    protected:
        // 添加任务链
        __KF_ADD_ELEMENT_FUNCTION__( AddTaskChainElement );

        // 刷新时间组
        __KF_RESET_FUNCTION__( OnResetRefreshTaskChain );

        // 命令刷新任务链( 传入刷新id )
        __KF_COMMAND_FUNCTION__( OnCommandRefreshTaskChain );

        // 开启任务链
        bool OpenTaskChain( KFEntity* player, uint32 taskchainid, uint32 order, uint32 time, uint32 refreshid, const char* function, uint32 line );

        // 开启任务链数据
        bool OpenTaskLogicDataList( KFEntity* player, const KFWeightList<KFTaskData>* taskdatalist, uint32 taskchainid, uint32 order, uint64 time, uint32 refreshid, const char* function, uint32 line );

        // 开启任务链逻辑数据
        bool OpenTaskLogicData( KFEntity* player, uint32 taskchainid, uint32 order, const KFTaskData* taskdata, uint32 logicid, const char* function, uint32 line );

        // 完成任务链逻辑数据
        void FinishTaskLogicData( KFEntity* player, KFData* kftask, const KFTaskData* taskdata );

        // 删除任务链逻辑数据
        void RemoveTaskLogicData( KFEntity* player, KFData* kftask, const KFTaskData* taskdata );

        // 清除任务链
        void CleanTaskChain( KFEntity* player, uint32 taskchainid );

    private:
        virtual void BindOpenTaskChainFunction( const std::string& name, KFOpenTaskChainFunction& function );
        virtual void UnBindOpenTaskChainFunction( const std::string& name );

        virtual void BindFinishTaskChainFunction( const std::string& name, KFFinishTaskChainFunction& function );
        virtual void UnBindFinishTaskChainFunction( const std::string& name );
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 开启任务链回调
        KFBind < std::string, const std::string&, KFOpenTaskChainFunction > _open_task_chain_function;

        // 关闭任务链回调
        KFBind < std::string, const std::string&, KFFinishTaskChainFunction > _finish_task_chain_function;
    };
}

#endif