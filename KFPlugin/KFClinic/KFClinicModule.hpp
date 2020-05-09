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
#include "KFZConfig/KFFormulaConfig.h"
#include "KFZConfig/KFElementConfig.h"

namespace KFrame
{
    class KFClinicSetting
    {
    public:
        // 常态数据
        void UpdateStaticPart();
        // 特化数据
        void UpdateDynamicPart( KFEntity* player );

    public:
        // HP物品 存储上限
        uint32 _hp_item_max_count = 0u;
        // HP物品 每次获得的冷却时间(s)
        uint32 _hp_item_cd_time = 0u;
        // HP物品 每次获得的数量
        uint32 _hp_item_add_count = 0u;
        // HP物品 使用后的回复量
        uint32 _hp_item_cure = 0u;

        // 医疗 栏位数量
        uint32 _cure_count = 0u;

        // 医疗 消耗材料数量
        uint32 _consume_hp_item_count = 0u;

        // 医疗 消耗金币公式id
        uint32 _consume_money_fid = 0u;
        // 医疗 消耗金币缩放
        double _scale_consume_money = 1.0;
    };

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

        // 医疗所执行科技效果
        __KF_EXECUTE_FUNCTION__( OnExecuteClinicAddData );

    protected:
        // 获取医疗所等级
        uint32 GetClinicLevel( KFEntity* player );

        // 医疗所是否激活
        bool IsClinicActive( KFEntity* player );

        // 获取医疗所配置
        const KFClinicSetting* GetClinicSetting( KFEntity* player );

        // 检查定时器
        void CheckClinicTimer( KFEntity* player );

        // 英雄uuid 回复hp
        typedef std::list<std::pair< uint64, uint32 >> HeroList;

        // 错误码 消耗材料 消耗货币
        typedef std::tuple<uint32, uint32, const std::string> CureDataReturn;

        // 计算治疗英雄数据
        CureDataReturn CalcCureHerosData( KFEntity* player, HeroList& herolist );

        // 计算治疗消耗道具数量
        uint32 CalcCureHerosItemNum( KFEntity* player, HeroList& herolist );

        // 计算治疗消耗费用
        const std::string& CalcCureHerosCost( KFEntity* player, uint32 num );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif