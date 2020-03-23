#ifndef __KF_REALM_MOUDLE_H__
#define __KF_REALM_MOUDLE_H__

/************************************************************************
//    @Module			:    秘境探索系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-7-27
************************************************************************/

#include "KFrameEx.h"
#include "KFRealmData.hpp"
#include "KFRealmInterface.h"
#include "KFHero/KFHeroInterface.h"
#include "KFHero/KFHeroTeamInterface.h"
#include "KFItem/KFItemInterface.h"
#include "KFItem/KFItemMoveInterface.h"
#include "KFDrop/KFDropInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFGenerate/KFGenerateInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFRealmConfig.hpp"
#include "KFZConfig/KFInnerWorldConfig.hpp"
#include "KFItem/KFItemRuneInterface.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////
    class KFRealmModule : public KFRealmInterface
    {
    public:
        KFRealmModule() = default;
        ~KFRealmModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        ////////////////////////////////////////////////////////////////////////////////
        // 获得秘境数据
        virtual KFRealmData* GetRealmData( KFEntity* player );

        // 判断是否在里世界
        virtual bool IsInnerWorld( KFEntity* player );

        // 获取生命回复比列
        virtual double GetAddHpRate( KFEntity* player );

    protected:
        // 秘境请求
        __KF_MESSAGE_FUNCTION__( HandleRealmEnterReq );
        __KF_MESSAGE_FUNCTION__( HandleRealmJumpReq );
        __KF_MESSAGE_FUNCTION__( HandleRealmExtendReq );

        // 请求回城
        __KF_MESSAGE_FUNCTION__( HandleRealmTownReq );

        // 退出探索
        __KF_MESSAGE_FUNCTION__( HandleRealmExitReq );

        // 秘境结算
        __KF_MESSAGE_FUNCTION__( HandleRealmBalanceReq );

        //////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////
        // 更新探索内玩家数据
        __KF_MESSAGE_FUNCTION__( HandleUpdateExplorePlayerReq );

        // 更新探索内npc数据
        __KF_MESSAGE_FUNCTION__( HaneleUpdateExploreNpcReq );

        // 探索掉落请求
        __KF_MESSAGE_FUNCTION__( HandleExploreDropReq );

        // 物件交互请求
        __KF_MESSAGE_FUNCTION__( HandleInteractItemReq );
    protected:
        // 秘境逻辑事件
        __KF_EXECUTE_FUNCTION__( OnExecuteRealm );

        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterInitRealmData );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveSaveRealmData );

        // 移动步数扣除粮食
        __KF_REALM_MOVE_FUNCTION__( OnReamlMoveCostFood );
    protected:
        virtual void BindRealmEnterFunction( const std::string& module, KFRealmEnterFunction& function );
        virtual void UnBindRealmEnterFunction( const std::string& module );

        virtual void BindRealmMoveFunction( const std::string& module, KFRealmMoveFunction& function );
        virtual void UnBindRealmMoveFunction( const std::string& module );
        //////////////////////////////////////////////////////////////////////////////////////
        // 进入探索
        bool RealmEnter( KFEntity* player, uint32 realmid, uint32 level, uint32 entertype, const std::string& modulename, uint64 moduleid );
        std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> RealmChapterEnter( KFEntity* player, uint32 realmid, uint32 level, const std::string& modulename, uint64 moduleid );
        std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> RealmLoginEnter( KFEntity* player, uint32 realmid );
        std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> RealmTownEnter( KFEntity* player, uint32 realmid );
        std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> RealmJumpEnter( KFEntity* player, uint32 realmid, uint32 level, uint32 birthplace );
        std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> RealmExtendEnter( KFEntity* player, uint32 exploreid );

        // 探索结算
        void RealmBalance( KFEntity* player, uint32 result );
        void RealmBalanceVictory( KFEntity* player, KFRealmData* kfrealmdata );
        void RealmBalanceFailed( KFEntity* player, KFRealmData* kfrealmdata );
        void RealmBalanceFlee( KFEntity* player, KFRealmData* kfrealmdata );
        void RealmBalanceTown( KFEntity* player, KFRealmData* kfrealmdata );

        // 发送结算消息
        void SendRealmBalanceToClient( KFEntity* player, KFRealmData* kfrealmdata, uint32 result );

        // 结算掉落
        void RealmBalanceDrop( KFEntity* player, KFRealmData* kfrealmdata, uint32 result );
        const UInt32Vector& GetRealmDropList( const KFRealmLevel* kfsetting, uint32 result );

        // 结算清空数据
        void RealmBalanceClearData( KFEntity* player, uint32 result );

        // 结算道具
        void RealmBalanceItem( KFEntity* player, uint32 result );

        // 结算结果
        void RealmBalanceResultCondition( KFEntity* player, KFRealmData* kfrealmdata, uint32 result );
        void RealmJumpCondition( KFEntity* player, uint32 realmid, uint32 lastlevel, uint32 nowlevel );

        // 随机探索失败获得道具
        void RealmRandFailedItems( KFEntity* player, KFRealmData* kfrealmdata );
        //////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////
        // 初始化探索数据
        void ExploreInitData( KFMsg::PBExploreData* pbexplore, uint32 exploreid, uint32 maxlevel, uint32 level, uint32 lastlevel );
        //////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////
        // 获取玩家当前秘境层配置
        const KFRealmLevel* FindRealmLevelSetting( KFEntity* player );
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 玩家的探索结算数据
        KFHashMap< uint64, uint64, KFRealmData > _realm_data;

        // 秘境进入回调
        KFBind< std::string, const std::string&, KFRealmEnterFunction > _realm_enter_function;

        // 秘境移动回调
        KFBind< std::string, const std::string&, KFRealmMoveFunction > _realm_move_function;
    };
}

#endif