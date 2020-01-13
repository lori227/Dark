#ifndef __KF_CLINIC_MOUDLE_H__
#define __KF_CLINIC_MOUDLE_H__

/************************************************************************
//    @Moudle			:    医疗所系统
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-07-18
************************************************************************/

#include "KFrameEx.h"
#include "KFMacros.h"
#include "KFClinicInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRecordClient/KFRecordClientInterface.h"
#include "KFClinicConfig.hpp"
#include "KFZConfig/KFFormulaConfig.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFClinicModule : public KFClinicInterface
    {
    public:
        KFClinicModule() = default;
        ~KFClinicModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterClinicModule );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveClinicModule );

        // 获取天总产量
        __KF_RECORD_VALUE_FUNCTION__( GetDayClinicTotalNum );

        // 医疗所等级解锁
        __KF_ADD_DATA_FUNCTION__( OnAddClinicBuild );

        // 物品数量更新
        __KF_UPDATE_DATA_FUNCTION__( OnItemNumUpdate );

        // 添加材料数量
        __KF_ADD_ELEMENT_FUNCTION__( AddClinicElement );

        // 治疗请求
        __KF_MESSAGE_FUNCTION__( HandleClinicCureReq );

        // 治疗请求
        __KF_MESSAGE_FUNCTION__( HandleClinicMedicalFeeReq );

        // 定时增加道具
        __KF_TIMER_FUNCTION__( OnTimerAddItem );

        // 额外增加的单位时间产出材料量
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyClinicAddNum );

        // 额外增加治疗材料的存储上限
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyClinicMaxNum );

        // 额外减少单个材料所需要花费的货币比例
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyClinicMoneySubPercent );

    protected:
        // 获取医疗所等级
        uint32 GetClinicLevel( KFEntity* player );

        // 医疗所是否激活
        bool IsClinicActive( KFEntity* player );

        // 获取医疗所配置
        const KFClinicSetting* GetClinicSetting( KFEntity* player );

        // 检查定时器
        void CheckClinicTimer( KFEntity* player );

        // 获取单位时间产出的材料量
        uint32 GetClinicUnitTimeMaterialsAddNum( KFEntity* player, const KFClinicSetting* setting );

        // 获取治疗材料的存储上限
        uint32 GetClinicMaterialsMaxNum( KFEntity* player, const KFClinicSetting* setting );

        // 获取所选的英雄列表需要的治疗量
        uint32 CalcClinicHerosNeedCurehp( KFEntity* player, UInt64List& herolist );

        // 能否使用治疗所治疗, 返回错误码
        uint32 CalcClinicCureMoney( KFEntity* player, UInt64List& herolist );

        // 获取治疗所花费金币量
        std::string CalcClinicMoney( KFEntity* player, const KFClinicSetting* setting, uint32 addhp );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif