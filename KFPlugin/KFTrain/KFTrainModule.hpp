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
#include "KFZConfig/KFFormulaConfig.h"

namespace KFrame
{
    class KFTrainSetting
    {
    public:
        // 常态数据
        void UpdateStaticPart();
        // 特化数据
        void UpdateDynamicPart( KFEntity* player );

    public:
        // 栏位数量
        uint32 _count = 0u;

        // 总时间(s)
        uint64 _total_time = 0u;

        // 间隔时间(s)
        uint64 _cd_time = 0u;

        // 单位时间增加经验
        uint32 _add_exp = 0u;

        // 训练消耗公式
        uint32 _consume_fid = 0u;

        // 消耗缩放比例（只对训练消耗）
        double _scale_consume = 1.0;

        // 加速消耗公式
        uint32 _onekey_consume_fid = 0u;

        // 一键完成单位时间（单位消耗-单位时间）
        uint32 _unit_time = 0u;
    };

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

        // 训练费用请求
        __KF_MESSAGE_FUNCTION__( HandleTrainFeeReq );

        // 一键完成训练费用请求
        __KF_MESSAGE_FUNCTION__( HandleTrainOneKeyFeeReq );

        // 定时增加经验
        __KF_TIMER_FUNCTION__( OnTimerAddExp );

        // 更新计算经验时间
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateCalcTime );

        // 删除训练所英雄
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveTrainHero );

        // 消耗货币所需比例
        __KF_EXECUTE_FUNCTION__( OnExecuteTrainCostScaleDec );

        // 训练所栏位
        __KF_EXECUTE_FUNCTION__( OnExecuteTrainCount );

        // 训练所单位时间所得经验
        __KF_EXECUTE_FUNCTION__( OnExecuteTrainUnitExp );

        // 训练所单位时间所得经验比例
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

        // 计算消耗
        const KFElements* CalcTrainConsume( KFEntity* player, const KFTrainSetting* setting );
        const KFElements* CalcTrainOneKeyConsume( KFEntity* player, const KFTrainSetting* setting, uint32 lefttime );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };

}



#endif