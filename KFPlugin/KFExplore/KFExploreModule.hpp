#ifndef __KF_EXPLORE_MOUDLE_H__
#define __KF_EXPLORE_MOUDLE_H__

/************************************************************************
//    @Module			:    探索系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-7-27
************************************************************************/

#include "KFrameEx.h"
#include "KFExploreData.hpp"
#include "KFExploreInterface.h"
#include "KFFighterInterface.h"
#include "KFHero/KFHeroInterface.h"
#include "KFHero/KFHeroTeamInterface.h"
#include "KFItem/KFItemInterface.h"
#include "KFDrop/KFDropInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFGenerate/KFGenerateInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFElementConfig.h"
#include "KFZConfig/KFExploreConfig.hpp"
#include "KFZConfig/KFPVEConfig.hpp"
#include "KFZConfig/KFFoodConsumeConfig.hpp"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////
    class KFExploreModule : public KFExploreInterface
    {
    public:
        KFExploreModule() = default;
        ~KFExploreModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        ////////////////////////////////////////////////////////////////////////////////

        virtual KFExploreRecord* GetExploreRecord( uint64 keyid );

    protected:
        // 探索请求
        __KF_MESSAGE_FUNCTION__( HandleExploreReq );

        // 退出探索
        __KF_MESSAGE_FUNCTION__( HandleExploreExitReq );

        // 探索结算
        __KF_MESSAGE_FUNCTION__( HandleExploreBalanceReq );

        // 更新探索内玩家数据
        __KF_MESSAGE_FUNCTION__( HandleUpdateExplorePlayerReq );

        // 更新探索内npc数据
        __KF_MESSAGE_FUNCTION__( HaneleUpdateExploreNpcReq );

        // 探索掉落请求
        __KF_MESSAGE_FUNCTION__( HandleExploreDropReq );

        // 更新信仰值
        __KF_MESSAGE_FUNCTION__( HandleUpdateFaithReq );

        // 物件交互请求
        __KF_MESSAGE_FUNCTION__( HandleInteractItemReq );

    protected:
        // 掉落增加英雄buff
        __KF_DROP_LOGIC_FUNCTION__( OnDropHeroAddBuff );

        // 掉落减少英雄buff
        __KF_DROP_LOGIC_FUNCTION__( OnDropHeroDecBuff );

        // 探索逻辑事件
        __KF_EXECUTE_FUNCTION__( OnExecuteExplore );

    protected:
        uint32 RequestExplore( KFEntity* player, uint32 mapid, const std::string& modulename, uint64 moduleid );

        // 探索结算
        void ExploreBalance( KFEntity* player, uint32 result );

        // 随机探索失败获得道具
        void RandExploreFailedItems( KFEntity* player );

        // 探索扣除粮食
        void ExploreCostFood( KFEntity* player, KFMsg::PBExplorePlayerData* playerdata );

        // 改变队伍英雄buff
        void ChangeTeamHeroBuff( KFEntity* player, uint32 operate, uint32 value );

        // 改变英雄buff
        void ChangeHeroBuff( KFEntity* player, uint64 uuid, uint32 operate, uint32 value );

        //////////////////////////////////////////////////////////////////////////////////////
        // 初始化探索纪录
        KFExploreRecord* InitExploreRecord( KFEntity* player, uint32 mapid, const std::string& modulename, uint64 moduleid );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveSaveExploreRecord );

        //////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 玩家的探索结算数据
        KFHashMap< uint64, uint64, KFExploreRecord > _explore_record;
    };
}

#endif