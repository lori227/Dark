#ifndef __KF_TRAIN_MOUDLE_H__
#define __KF_TRAIN_MOUDLE_H__

/************************************************************************
//    @Moudle			:    训练所系统
//    @Author           :    erlking
//    @QQ				:    1326273445
//    @Mail			    :    1326273445@qq.com
//    @Date             :    2019-07-18
************************************************************************/

#include "KFrameEx.h"
#include "KFTrainInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFHero/KFHeroInterface.h"
#include "KFRecordClient/KFRecordClientInterface.h"
#include "KFTrainConfig.hpp"
#include "KFZConfig/KFLevelConfig.hpp"
#include "KFExecute/KFExecuteInterface.h"

namespace KFrame
{
    class KFTrainModule : public KFTrainInterface
    {
    public:
        KFTrainModule() = default;
        ~KFTrainModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterTrainModule );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveTrainModule );

        // 训练改变请求(增加删除)
        __KF_MESSAGE_FUNCTION__( HandleTrainChangeReq );

        // 一键清空训练栏
        __KF_MESSAGE_FUNCTION__( HandleTrainCleanReq );

        // 一键完成训练栏
        __KF_MESSAGE_FUNCTION__( HandleTrainOnekeyReq );

        // 再次训练请求
        __KF_MESSAGE_FUNCTION__( HandleTrainAgainReq );

        // 定时增加经验
        __KF_TIMER_FUNCTION__( OnTimerAddExp );

        // 更新计算经验时间
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateCalcTime );

        // 删除训练所英雄
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveTrainHero );

        // 更新 增加单位时间训练经验 科技
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyTrainExp );

        // 更新 降低训练的物资消耗 科技
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyTrainCost );

        // 更新 增加新栏位 科技
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyTrainCount );

    protected:
        // 获取训练所等级
        uint32 GetTrainLevel( KFEntity* player );

        // 训练所是否激活
        bool IsTrainActive( KFEntity* player );

        // 通过uuid获取训练栏位置
        KFData* GetTrainById( KFEntity* player, uint64 uuid );

        // 获取训练所配置
        const KFTrainSetting* GetTrainSetting( KFEntity* player );

        // 添加英雄到栏位
        void AddTrainHero( KFEntity* player, KFData* kftrainrecord, uint64 uuid, uint32 index );

        // 检查定时器
        void EnterStartTrainTimer( KFEntity* player );

        // 启动定时
        void StartTrainTimer( KFEntity* player, const KFTrainSetting* kfsetting, uint32 index, uint32 delaytime );

        // 添加训练所英雄经验
        uint32 AddTrainHeroExp( KFEntity* player, KFData* kftrain, uint32 count, bool isnow = false );

        // 添加训练所英雄升级纪录
        void AddTrainHeroLevelRecord( KFEntity* player, const KFTrainSetting* kfsetting, KFData* kftrain, KFData* kfhero, uint32 newlevel, uint32 addexp );

        // 删除训练所英雄
        void RemoveTrainHero( KFEntity* player, KFData* kftrainrecord, KFData* kftrain );

        // 添加训练所条件
        void AddTrainCondition( KFEntity* player, KFData* kftrain );

        // 检查并消耗
        std::string CheckAndRemoveElement( KFEntity* player, const KFElements* kfelements, const char* function, uint32 line, uint32 consumecount );

        // 获取训练所全部栏数
        uint32 GetTrainCampCount( KFEntity* player, const KFTrainSetting* kfsetting );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif