#ifndef __KF_RECRUIT_MOUDLE_H__
#define __KF_RECRUIT_MOUDLE_H__

/************************************************************************
//    @Moudle			:    英雄招募系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-7-15
************************************************************************/
#include "KFrameEx.h"
#include "KFRecruitInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFHero/KFHeroInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFGenerate/KFGenerateInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFRecruitConfig.hpp"
#include "KFZConfig/KFFormulaConfig.h"
#include "KFZConfig/KFDivisorConfig.hpp"
#include "KFZConfig/KFProfessionConfig.hpp"
#include "KFZConfig/KFGenerateConfig.hpp"
#include "KFZConfig/KFElementConfig.h"
#include "KFZConfig/KFTechnologyConfig.hpp"

namespace KFrame
{
    class KFDivisorSetting;
    class KFRecruitModule : public KFRecruitInterface
    {
    public:
        KFRecruitModule() = default;
        ~KFRecruitModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 打开招募所界面
        __KF_MESSAGE_FUNCTION__( HandleOpenRecruitReq );

        // 刷新招募英雄
        __KF_MESSAGE_FUNCTION__( HandleRefreshRecruitReq );

        // 选择偏好因子
        __KF_MESSAGE_FUNCTION__( HandleChooseDivisorReq );

        // 删除偏好因子
        __KF_MESSAGE_FUNCTION__( HandleRemoveDivisorReq );

        // 招募英雄
        __KF_MESSAGE_FUNCTION__( HandleRecruitHeroReq );

        // 设置英雄名字
        __KF_MESSAGE_FUNCTION__( HandleSetRecruitHeroNameReq );

    protected:
        // 添加招募英雄
        __KF_ADD_ELEMENT_FUNCTION__( AddRecruitHeroElement );
        ///////////////////////////////////////////////////////////////////////
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterRecruitModule );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveRecruitModule );

        // 刷新招募次数
        __KF_TIMER_FUNCTION__( OnTimerRefreshRecruitCount );

        // 检测刷新招募时间
        void CheckRefreshRecruitTimer( KFEntity* player );

        // 刷新招募时间
        void StartRefreshRecruitTimer( KFEntity* player, KFData* kfrefreshtime, uint32 intervaltime );

        // 招募数量改变
        __KF_UPDATE_DATA_FUNCTION__( OnRecruitCountUpdateCallBack );

        // 招募所解锁
        __KF_ADD_DATA_FUNCTION__( OnAddRecruitBuild );

        // 招募所是否激活
        bool IsRecruitActive( KFEntity* player );

        // 删除英雄
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveHero );

        // 刷新免费英雄
        void RefreshRecruitFreeHero( KFEntity* player, uint32 herocount );

        // 刷新招募列表
        uint32 RefreshRecruitLists( KFEntity* player, uint32 type );

        // 计算招募英雄的数量
        uint32 CalcRecruitHeroCount( KFData* kfeffect, const KFRecruitSetting* kfrecruitsetting );

        // 默认的招募英雄池子和权重
        uint32 CalcRecruitGenerateWeight( KFData* kfeffect, const KFRecruitSetting* kfrecruitsetting, MapUInt32& generateweight );

        // 生成招募英雄
        void GenerateRecruitHero( KFEntity* player, KFData* kfeffect, KFData* kfrecruitrecord, uint32 generateid,
                                  const DivisorList& divisorlist, const SetUInt32& professionlist, uint32 mingrowth, uint32 maxgrowth );

        // 计算招募花费
        void CalcRecruitCostData( KFData* kfeffect, KFData* kfrecruit, KFData* kfhero, uint32 generateid );

        // 计算种族亲和
        uint32 CalcRecruitHeroDiscount( KFData* kfeffect, KFData* kfhero, uint32 price );

        // 计算招募英雄的等级
        uint32 CalcRecruitHeroLevel( KFData* kfeffect );

        // 解锁的职业列表
        SetUInt32& CalcRecruitHeroProfession( KFData* kfeffect );

        // 计算招募因子
        DivisorList& CalcRecruitHeroDivisor( KFData* kfeffect );

        // 招募因子列表
        DivisorList& DivisorListByType( KFData* kfdivisorrecord, uint32 type );

        // 解锁招募因子
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyRecruitDivisor );

        // 招募数量
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyRecruitCount );

        // 招募权重
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyRecruitWeight );

        // 招募亲和
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyRecruitDiscount );

        // 解锁职业
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyRecruitProfession );

        // 解锁等级
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyRecruitLevel );

        // 招募成长率
        __KF_EXECUTE_FUNCTION__( OnExecuteTechnologyRecruitGrowth );
    protected:
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };
}

#endif