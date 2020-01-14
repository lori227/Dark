#ifndef __KF_PVE_MOUDLE_H__
#define __KF_PVE_MOUDLE_H__

/************************************************************************
//    @Module			:    场景战斗模块
//    @Author           :    zux
//    @QQ				:    415906519
//    @Mail			    :    415906519@qq.com
//    @Date             :    2019-12-13
************************************************************************/

#include "KFrameEx.h"
#include "KFRealmData.hpp"
#include "KFPVEInterface.h"
#include "KFRealmInterface.h"
#include "KFHero/KFHeroInterface.h"
#include "KFHero/KFHeroTeamInterface.h"
#include "KFDrop/KFDropInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFGenerate/KFGenerateInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFZConfig/KFPVEConfig.hpp"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////
    class KFPVEModule : public KFPVEInterface
    {
    public:
        KFPVEModule() = default;
        ~KFPVEModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnPlayerLeave );

        // 掉落英雄经验
        __KF_DROP_LOGIC_FUNCTION__( OnDropHeroExp );

        // 掉落增加英雄血量
        __KF_DROP_LOGIC_FUNCTION__( OnDropHeroAddHp );

        // 掉落减少英雄血量
        __KF_DROP_LOGIC_FUNCTION__( OnDropHeroDecHp );

        // 执行战斗逻辑
        __KF_EXECUTE_FUNCTION__( OnExecutePVEFighter );

    protected:
        // 战斗请求(布置战斗)
        __KF_MESSAGE_FUNCTION__( HandlePVEReq );

        // 结算请求
        __KF_MESSAGE_FUNCTION__( HandlePVEBalanceReq );

        // 更新战斗英雄数据
        __KF_MESSAGE_FUNCTION__( HandleUpdateFighterHeroReq );

        // 击杀npc怪物
        __KF_MESSAGE_FUNCTION__( HandleKillNpcReq );

        // 出战英雄列表
        __KF_MESSAGE_FUNCTION__( HandleFightHeroListReq );

    protected:
        // 部署战斗
        uint32 PVEFighter( KFEntity* player, uint32 pveid, uint32 battleid, const std::string& modulename, uint64 moduleid );

        // 结算战斗
        uint32 PVEBalance( KFEntity* player, uint32 result, uint32 truns );

        // 统计英雄击杀npc
        void StatisticsHeroKillNpcs( KFEntity* player, KFRealmData* kfrecord, uint32 pveresult );
        void StatisticsHeroKillNpc( KFEntity* player, KFData* kfnpcrecord, KFRealmData* kfrecord, uint32 pveresult, uint64 herouuid, uint64 npcid );

        // 改变队伍英雄血量
        void ChangeTeamHeroHp( KFEntity* player, uint32 operate, uint32 value );
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 玩家的战斗结算数据
        KFHashMap< uint64, uint64, KFRealmData > _pve_record;

    };
}

#endif