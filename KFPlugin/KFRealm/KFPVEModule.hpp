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
#include "KFZConfig/KFPunishConfig.hpp"
#include "KFItem/KFItemRuneModule.hpp"

namespace KFrame
{
    class DynamicPunishData : public KFWeight
    {
    public:
        DynamicPunishData() {}
        DynamicPunishData( const std::string& name, uint32 key, uint32 value )
            : _name( name ), _key( key ), _value( value )
        {

        }
    public:
        std::string _name;
        uint32 _key = 0u;
        uint32 _value = 0u;
    };
    typedef std::list< DynamicPunishData > PunishDataList;
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

        // 操作(里世界)信仰值
        virtual void OperateFaith( KFEntity* player, uint32 operate, uint32 value );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnPlayerPVEEnter );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnPlayerPVELeave );

        // 执行战斗逻辑
        __KF_EXECUTE_FUNCTION__( OnExecutePVEFighter );

        // 掉落英雄经验
        __KF_DROP_LOGIC_FUNCTION__( OnDropHeroExp );

    protected:
        // 请求npc数据
        __KF_MESSAGE_FUNCTION__( HandleNpcGroupDataReq );

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

        // 更新信仰值
        __KF_MESSAGE_FUNCTION__( HandleUpdateFaithReq );

        // 回合结束
        __KF_MESSAGE_FUNCTION__( HandleTurnStartReq );

        // 回合结束
        __KF_MESSAGE_FUNCTION__( HandleTurnFinishReq );
    protected:
        virtual void BindPVEStartFunction( const std::string& module, KFPVEStartFunction& function );
        virtual void UnBindPVEStartFunction( const std::string& module );

        virtual void BindPVEFinishFunction( const std::string& module, KFPVEFinishFunction& function );
        virtual void UnBindPVEFinishFunction( const std::string& module );

        virtual void BindTurnStartFunction( const std::string& module, KFTurnStartFunction& function );
        virtual void UnBindTurnStartFunction( const std::string& module );

        virtual void BindTurnFinishFunction( const std::string& module, KFTurnFinishFunction& function );
        virtual void UnBindTurnFinishFunction( const std::string& module );
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // 部署战斗
        uint32 PVEFighter( KFEntity* player, uint32 pveid, uint32 npcgrouid, uint32 spawnrule, uint32 dungeonid, const std::string& modulename, uint64 moduleid );

        // 随机npc敌人组
        const KFNpcGroupSetting* RandNpcGroupSetting( const KFPVESetting* kfpvesetting, uint32 npcgroupid, uint32& spawnrule );

        // 查找探索的战斗npc信息
        std::tuple<uint32, uint32, uint32> FindRealmNpcData( KFEntity* player, uint32 pveid, const std::string& npckey );

        // 结算战斗
        uint32 PVEBalance( KFEntity* player, uint32 result, uint32 truns );
        uint32 PVEBalanceVictory( KFEntity* player, KFRealmData* kfpvedata, uint32 truns );
        uint32 PVEBalanceFailed( KFEntity* player, KFRealmData* kfpvedata, uint32 truns );
        uint32 PVEBalanceFlee( KFEntity* player, KFRealmData* kfpvedata, uint32 truns );
        uint32 PVEBalanceAce( KFEntity* player, KFRealmData* kfpvedata, uint32 truns );

        // 战斗结果条件结算
        void PVEBalanceResultCondition( KFEntity* player, KFRealmData* kfpvedata, uint32 result, uint32 truns );

        // 结算数据纪录
        void PVEBalanceRecord( KFEntity* player, KFRealmData* kfpvedata, uint32 result );

        // 战斗掉落结果
        void PVEBalanceDrop( KFEntity* player, KFRealmData* kfpvedata, uint32 result );
        const UInt32Vector& PVEGetDropList( KFEntity* player, const KFPVESetting* kfsetting, uint32 result );

        // 结算清除数据
        void PVEBalanceClearData( KFEntity* player );

        // 发送战斗结算
        void SendPVEBalanceToClient( KFEntity* player, KFRealmData* kfpvedata, uint32 result );

        // 统计英雄击杀npc
        void StatisticsHeroKillNpcs( KFEntity* player, KFRealmData* kfrecord, uint32 pveresult );
        void StatisticsHeroKillNpc( KFEntity* player, KFData* kfnpcrecord, KFRealmData* kfrecord, uint32 pveresult, uint64 herouuid, uint64 npcid );

        // 计算逃跑成功率
        uint32 CalcFleeSuccessRate( KFEntity* player, const KFPVESetting* kfsetting, double herolevel, double npclevel, bool isinrealm );

        // 逃跑惩罚
        PunishDataList& PVEFleePunish( KFEntity* player, KFRealmData* kfpvedata );
        void PVEFleePunishData( KFEntity* player, KFRealmData* kfpvedata, PunishDataList& punishlist );
        void PVEFleePunishData( KFEntity* player, const DynamicPunishData* punishdata, PunishDataList& punishlist );

        // 逃跑扣除道具
        uint32 PVEFleePunishItem( KFEntity* player, const std::string& dataname, const DynamicPunishData* punishdata, bool percent );

        // 判断PVE是否在里世界
        bool IsInnerWorld( KFEntity* player );
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 玩家的战斗结算数据
        KFHashMap< uint64, uint64, KFRealmData > _pve_record;

        // 开始战斗回调
        KFBind< std::string, const std::string&, KFPVEStartFunction > _pve_start_function;

        // 结束战斗回调
        KFBind< std::string, const std::string&, KFPVEFinishFunction > _pve_finish_function;

        // 开始回合回调
        KFBind< std::string, const std::string&, KFTurnFinishFunction > _turn_start_function;

        // 结束回合回调
        KFBind< std::string, const std::string&, KFTurnFinishFunction > _turn_finish_function;
    };
}

#endif