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
#include "KFProtocol/KFProtocol.h"
#include "KFExploreInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFHero/KFHeroInterface.h"
#include "KFDrop/KFDropInterface.h"
#include "KFGenerate/KFGenerateInterface.h"
#include "KFZConfig/KFExploreConfig.hpp"

namespace KFrame
{
    class KFExploreRecord
    {
    public:
        enum RecordType
        {
            TypeFight = 0x1,
            TypeExplore = 0x2,
            TypeAll = 0x3,
        };

    public:
        // 查找显示数据
        KFMsg::PBShowData* FindShowData( KFMsg::PBBalanceData* balance, const std::string& name, uint64 id );

        // 查找英雄数据
        KFMsg::PBBalanceHero* FindBalanceHero( KFMsg::PBBalanceData* balance, uint64 uuid );

        // 添加掉落道具结算数据
        void AddShowData( uint32 recordtype, const KFDropData* dropdata );

        // 添加英雄结算数据
        void AddHeroExpRecord( uint32 recordtype, uint64 uuid, uint32 exp, uint32 levelup );

    protected:
        // 添加掉落道具
        void AddShowData( KFMsg::PBShowData* showdata, const KFElement* kfelement );

        // 添加英雄结算数据
        void AddHeroExpRecord( KFMsg::PBBalanceHero* balancehero, uint32 exp, uint32 levelup );

    public:
        // 玩家id
        uint64 _id = 0u;

        // pve战斗结算数据
        KFMsg::PBBalanceData _fight_balance;

        // 探索结算数据
        KFMsg::PBBalanceData _explore_balance;
    };
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
    protected:
        // 探索请求
        __KF_MESSAGE_FUNCTION__( HandleExploreReq );

        // 战斗请求
        __KF_MESSAGE_FUNCTION__( HandleFighterReq );

        // 更新战斗数据
        __KF_MESSAGE_FUNCTION__( HandleUpdateFighterReq );

        // 结算请求
        __KF_MESSAGE_FUNCTION__( HandlePVEBalanceReq );

        // 退出探索
        __KF_MESSAGE_FUNCTION__( HandleExploreExitReq );

        // 探索结算
        __KF_MESSAGE_FUNCTION__( HandleExploreBalanceReq );

    protected:
        // 英雄hp更新
        __KF_UPDATE_DATA_FUNCTION__( OnHeroHpUpdate );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeavePlayerSaveExplore );

        // 发送数据到战斗
        void SendDataToFighter( KFEntity* player, uint64 uuid, const std::string& name, uint32 value );

        // 探索结算
        void ExploreBalance( KFEntity* player, uint32 result );

        // 探索掉落英雄经验
        void ExploreDropHeroData( KFEntity* player, uint32 dropid, uint32 recordtype );

        // 探索添加英雄经验, 返回新等级
        uint32 ExploreAddHeroExp( KFEntity* player, KFData* kfhero, uint32 exp, uint32 recordtype );

        // 探索掉落属性
        void ExploreDropElement( KFEntity* player, uint32 dropid, uint32 recordtype );

        //////////////////////////////////////////////////////////////////////////////////////
        // 初始化探索纪录
        void InitExploreRecord( KFEntity* player );
        //////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 玩家的探索结算数据
        KFHashMap< uint64, uint64, KFExploreRecord > _explore_record;
    };
}

#endif