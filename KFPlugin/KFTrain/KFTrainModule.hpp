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
#include "KFZConfig/KFLevelConfig.hpp"
#include "KFExecute/KFExecuteInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFTrainSetting;
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

        // 科技效果
        __KF_EXECUTE_FUNCTION__( OnExecuteTrainCostScaleDec );
        __KF_EXECUTE_FUNCTION__( OnExecuteTrainUnitExp );
        __KF_EXECUTE_FUNCTION__( OnExecuteTrainExpScale );

    protected:
        // 获取训练所等级
        uint32 GetTrainLevel( KFEntity* player );

        // 训练所是否激活
        bool IsTrainActive( KFEntity* player );

        // 通过uuid获取训练栏位置
        KFData* GetTrainById( KFEntity* player, uint64 uuid );

        // 获取训练所玩家参数
        const KFTrainSetting* GetTrainSetting( KFEntity* player );

        // 添加英雄到栏位
        void AddTrainHero( KFEntity* player, KFData* kftrainrecord, const KFTrainSetting* kfsetting, uint64 uuid, uint32 index );

        // 检查定时器
        void EnterStartTrainTimer( KFEntity* player );

        // 启动定时
        void StartTrainTimer( KFEntity* player, const KFTrainSetting* kfsetting, uint32 index, uint32 delaytime );

        // 添加训练所英雄经验
        uint32 AddTrainHeroExp( KFEntity* player, KFData* kftrain, const KFTrainSetting* kfsetting, uint32 count, bool isnow = false );

        // 添加训练所英雄升级纪录
        void AddTrainHeroLevelRecord( KFEntity* player, const KFTrainSetting* kfsetting, KFData* kftrain, KFData* kfhero, uint32 newlevel, uint32 addexp );

        // 删除训练所英雄
        void RemoveTrainHero( KFEntity* player, KFData* kftrainrecord, KFData* kftrain );

        // 添加训练所条件
        void AddTrainCondition( KFEntity* player, KFData* kftrain );

        // 通用科技效果操作
        bool CommonAddEffectHandle( KFEntity* player, const KFExecuteData* executedata, const std::string& fieldname );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };

    class KFTrainSetting
    {
    public:
        KFTrainSetting();

        // 特化数据
        const void UpdateTrainSetting( KFEntity* player );

    private:
        KFTrainSetting( const KFTrainSetting& other ) = delete;
        KFTrainSetting& operator=( const KFTrainSetting& other ) = delete;

    public:
        // 栏位数量
        uint32 _count = 0u;

        // 总时间(s)
        uint64 _total_time = 0u;

        // 间隔时间(s)
        uint64 _cd_time = 0u;

        // 单位时间增加经验
        uint32 _unit_exp = 0u;
        double _scale_unit_exp = 1.0;
        uint32 _add_exp = 0u;

        // 消耗
        KFElements _consume;
        KFElements _onekey_consume;

        // 消耗缩放比例
        double _scale_cost = 1.0;

        // 一键完成单位时间（单位消耗-单位时间）
        uint32 _unit_time;
    };
}



#endif