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
        __KF_MESSAGE_FUNCTION__( HandleExploreEnterReq );
        __KF_MESSAGE_FUNCTION__( HandleExploreJumpReq );
        __KF_MESSAGE_FUNCTION__( HandleExploreExtendReq );

        // 请求回城
        __KF_MESSAGE_FUNCTION__( HandleExploreTownReq );

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
        // 进入探索
        bool EnterExplore( KFEntity* player, uint32 mapid, uint32 level, uint32 entertype, const std::string& modulename, uint64 moduleid );
        std::tuple<uint32, KFExploreRecord*, KFMsg::PBExploreData*> ChapterEnterExplore( KFEntity* player, uint32 mapid, uint32 level, const std::string& modulename, uint64 moduleid );
        std::tuple<uint32, KFExploreRecord*, KFMsg::PBExploreData*> LoginEnterExplore( KFEntity* player, uint32 mapid );
        std::tuple<uint32, KFExploreRecord*, KFMsg::PBExploreData*> TownEnterExplore( KFEntity* player, uint32 mapid );
        std::tuple<uint32, KFExploreRecord*, KFMsg::PBExploreData*> JumpEnterExplore( KFEntity* player, uint32 mapid, uint32 level );
        std::tuple<uint32, KFExploreRecord*, KFMsg::PBExploreData*> ExtendEnterExplore( KFEntity* player, uint32 mapid );

        // 初始化探索数据
        void InitExploreData( KFMsg::PBExploreData* pbexplore, uint32 exploreid, uint32 maxlevel, uint32 level, uint32 lastlevel );

        // 探索结算
        void ExploreBalance( KFEntity* player, uint32 result );
        void ExploreBalanceVictory( KFEntity* player, KFExploreRecord* kfrecord );
        void ExploreBalanceFailed( KFEntity* player, KFExploreRecord* kfrecord );
        void ExploreBalanceFlee( KFEntity* player, KFExploreRecord* kfrecord );
        void ExploreBalanceTown( KFEntity* player, KFExploreRecord* kfrecord );

        // 结算掉落
        void ExploreBalanceDrop( KFEntity* player, KFExploreRecord* kfrecord, uint32 result );

        // 结算清空数据
        void ExploreBalanceClearData( KFEntity* player );

        // 结算结果
        void ExploreBalanceResultCondition( KFEntity* player, KFExploreRecord* kfrecord, uint32 result );

        // 随机探索失败获得道具
        void RandExploreFailedItems( KFEntity* player );

        // 探索扣除粮食
        void ExploreCostFood( KFEntity* player, KFMsg::PBExplorePlayerData* playerdata );

        // 改变队伍英雄buff
        void ChangeTeamHeroBuff( KFEntity* player, uint32 operate, uint32 value );

        //////////////////////////////////////////////////////////////////////////////////////
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