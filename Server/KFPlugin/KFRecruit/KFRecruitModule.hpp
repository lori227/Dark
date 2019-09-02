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
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFFilter/KFFilterInterface.h"
#include "KFGenerate/KFGenerateInterface.h"
#include "KFRecruitConfig.hpp"
#include "KFZConfig/KFDivisorConfig.hpp"
#include "KFZConfig/KFProfessionConfig.hpp"

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

        // 刷新招募时间
        void StartRefreshRecruitTimer( KFEntity* player, KFData* kfrefreshtime, uint32 intervaltime );

        // 招募数量改变
        __KF_UPDATE_DATA_FUNCTION__( OnRecruitCountUpdateCallBack );

        // 刷新招募列表
        void RefreshRecruitHero( KFEntity* player, uint32 type );

        // 计算招募花费
        void CalcRecruitCostData( KFEntity* player, KFData* kfrecruit );

        // 招募因子列表
        std::vector< const KFDivisorSetting* >& DivisorListByType( KFEntity* player, uint32 type );

    protected:
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };
}

#endif