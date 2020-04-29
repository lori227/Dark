#ifndef __KF_REALM_DATA_H__
#define __KF_REALM_DATA_H__

#include "KFrameEx.h"
#include "KFCore/KFData.h"
#include "KFKernel/KFEntity.h"
#include "KFKernel/KFElementResult.h"
#include "KFProtocol/KFProtocol.h"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFItemTypeConfig.hpp"
#include "KFZConfig/KFItemBagConfig.hpp"

namespace KFrame
{
    class KFGambleData
    {
    public:
        // 抽奖次数
        uint32 _gamble_count = 0u;

        // 上次消耗个数
        uint32 _cost_item_count = 0u;
    };

    ////////////////////////////////////////////////////////////////////////////////
    class KFRealmData
    {
    public:
        enum BalanceType
        {
            // 统计类型
            StartType = 0,		// 开始
            EndType = 1,		// 结束
        };

        KFRealmData() = default;
        ~KFRealmData();

        // 重置
        KFRealmData* Reset();

        // 查找英雄
        KFMsg::PBBalanceHeroServer* FindHero( uint64 uuid );

        // 查找道具
        KFMsg::PBBalanceItemServer* FindItem( uint32 id, uint64 uuid );

        // 丢失的道具
        KFMsg::PBBalanceItem* FindLose( uint32 id, uint64 uuid );

        // 查找npc数据
        KFMsg::PBExploreNpcData* FindNpcData( const std::string& key );

        // 查找探索数据
        KFMsg::PBExploreData* FindExeploreData( uint32 level );
        void RemoveExeploreData( uint32 level );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化道具数据
        void RecordBeginItems( KFEntity* player );

        // 道具最终数据
        void RecordEndItems( KFEntity* player );

        // 初始化英雄数据
        void RecordBeginHeros( KFEntity* player );

        // 记录英雄期间数据(结算前)
        void RecordPeriodHeros( KFEntity* player );

        // 英雄最终数据
        void RecordEndHeros( KFEntity* player );

        // 记录回城英雄数据
        void RecordTownHeros( KFEntity* player );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 结算掉落
        void BalanceDropData( const KFElementResult* kfresult );

        // 结算
        void BalanceRealmRecord( KFMsg::PBBalanceData* pbdata );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 是否在里世界
        bool IsInnerWorld() const;

        // 计算探索内获得的道具数量
        uint32 CalcRealmItemCount( const KFMsg::PBBalanceItemServer* pbitem );

        // 添加使用的道具数量
        void AddRealmUseItemCount( uint32 id, uint64 uuid, uint32 count );
    protected:
        // 计算英雄数据
        void RecordHeroBeginData( KFData* kfhero, KFMsg::PBBalanceHeroServer* pbhero );
        void RecordHeroPeriodData( KFData* kfhero, KFMsg::PBBalanceHeroServer* pbhero );
        void RecordHeroEndData( KFData* kfhero, KFMsg::PBBalanceHeroServer* pbhero );
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 计算道具数据
        void RecordItemRecordData( KFData* kfitemrecord, uint32 balancetype );
        void RecordHeroWeapon( KFEntity* player, uint32 balancetype );
        void RecordItemData( KFData* kfitem, uint32 balancetype );
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 结算各项数据
        void BalanceHeroRecord( KFMsg::PBBalanceData* pbdata );
        void BalanceItemRecord( KFMsg::PBBalanceData* pbdata );

        // 判断是否是出战英雄
        bool IsFightHero( uint64 herouuid );

        // 计算英雄属性
        void BalanceHeroAttributes( const KFMsg::PBBalanceHeroServer* pbheroserver, KFMsg::PBBalanceHero* pbheroclient );

        // 计算英雄主动技能
        void BalanceHeroActives( const KFMsg::PBBalanceHeroServer* pbheroserver, KFMsg::PBBalanceHero* pbheroclient );

        // 计算英雄天赋技能
        void BalanceHeroInnates( const KFMsg::PBBalanceHeroServer* pbheroserver, KFMsg::PBBalanceHero* pbheroclient );

        // 计算英雄伤病
        void BalanceHeroInjurys( const KFMsg::PBBalanceHeroServer* pbheroserver, KFMsg::PBBalanceHero* pbheroclient );

    public:
        // 是否在结算中
        bool _is_in_balance = false;

        // 秘境数据
        KFMsg::PBRealmData _data;

        // 玩家的出战英雄列表
        UInt64Set _fight_hero;

        // 玩家抽奖数据
        KFHashMap< uint32, uint32, KFGambleData > _gamble_list;
    };
}

#endif