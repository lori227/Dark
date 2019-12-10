#ifndef __KF_TRAIN_CAMP_MOUDLE_H__
#define __KF_TRAIN_CAMP_MOUDLE_H__

/************************************************************************
//    @Moudle			:    训练所系统
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-07-18
************************************************************************/

#include "KFrameEx.h"
#include "KFTrainCampInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFHero/KFHeroInterface.h"
#include "KFRecordClient/KFRecordClientInterface.h"
#include "KFTrainCampConfig.hpp"
#include "KFZConfig/KFLevelConfig.hpp"

namespace KFrame
{
    class KFTrainCampModule : public KFTrainCampInterface
    {
    public:
        KFTrainCampModule() = default;
        ~KFTrainCampModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterTrainCampModule );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveTrainCampModule );

        // 训练改变请求(增加删除)
        __KF_MESSAGE_FUNCTION__( HandleTrainChangeReq );

        // 一键清空训练栏
        __KF_MESSAGE_FUNCTION__( HandleTrainCleanReq );

        // 一键完成训练栏
        __KF_MESSAGE_FUNCTION__( HandleTrainOnekeyReq );

        // 定时增加经验
        __KF_TIMER_FUNCTION__( OnTimerAddExp );

        // 更新计算经验时间
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateCalcTime );

        // 删除训练所英雄
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveTrainHero );
    protected:
        // 获取训练所等级
        uint32 GetTrainCampLevel( KFEntity* player );

        // 训练所是否激活
        bool IsTrainCampActive( KFEntity* player );

        // 通过uuid获取训练栏位置
        KFData* GetTrainById( KFEntity* player, uint64 uuid );

        // 获取训练所配置
        const KFTrainCampSetting* GetTrainCampSetting( KFEntity* player );

        // 添加英雄到栏位
        void AddTrainCampHero( KFEntity* player, KFData* kftrainrecord, uint64 uuid, uint32 index );

        // 检查定时器
        void EnterStartTrainCampTimer( KFEntity* player );

        // 启动定时
        void StartTrainCampTimer( KFEntity* player, const KFTrainCampSetting* kfsetting, uint32 index, uint32 delaytime );

        // 添加训练所英雄经验
        uint32 AddTrainCampHeroExp( KFEntity* player, KFData* kftrain, uint32 count, bool isnow = false );

        // 添加训练所英雄升级纪录
        void AddTrainHeroLevelRecord( KFEntity* player, const KFTrainCampSetting* kfsetting, KFData* kftrain, KFData* kfhero, uint32 newlevel, uint32 addexp );

        // 删除训练所英雄
        void RemoveTrainCampHero( KFEntity* player, KFData* kftrainrecord, KFData* kftrain );

        // 添加训练所条件
        void AddTrainCampCondition( KFEntity* player, KFData* kftrain );
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif