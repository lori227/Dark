#ifndef __KF_REALM_DATA_H__
#define __KF_REALM_DATA_H__

#include "KFrameEx.h"
#include "KFCore/KFData.h"
#include "KFKernel/KFEntity.h"
#include "KFProtocol/KFProtocol.h"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFCurrencyConfig.hpp"

namespace KFrame
{
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

        // 查找货币
        KFMsg::PBBalanceCurrency* FindCurrency( const std::string& name );

        // 查找npc数据
        KFMsg::PBExploreNpcData* FindNpcData( const std::string& key );

        // 查找探索数据
        KFMsg::PBExploreData* FindExeploreData( uint32 level );
        void RemoveExeploreData( uint32 level );

        // 添加buff
        void AddBuffData( uint64 uuid, uint32 value );
        void RemoveBuffData( uint64 uuid, uint32 value );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化数据
        void BalanceBeginData( KFEntity* player );

        // 结算数据
        void BalanceEndData( KFEntity* player );

        // 初始化英雄数据
        void BalanceHeroBeginData( KFEntity* player );

        // 英雄最终数据
        void BalanceHeroEndData( KFEntity* player );

        // 初始化道具数据
        void BalanceItemBeginData( KFEntity* player );

        // 道具最终数据
        void BalanceItemEndData( KFEntity* player );

        // 初始化货币数据
        void BalanceCurrencyBeginData( KFEntity* player );

        // 最终货币数据
        void BalanceCurrencyEndData( KFEntity* player );

        // 结算掉落
        void BalanceAddDropData( KFEntity* player );

        // 结算
        void BalanceRealmRecord( KFMsg::PBBalanceData* pbdata, uint32 status );
        ////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        // 清空结算数据
        void BalanceClearData( KFEntity* player );

        // 计算道具数据
        void BalanceItemRecordData( KFData* kfitemrecord, uint32 balancetype );
        void BalanceItemData( KFData* kfitem, uint32 balancetype );

        // 结算各项数据
        void BalanceHeroRecord( KFMsg::PBBalanceData* pbdata );
        void BalanceItemRecord( KFMsg::PBBalanceData* pbdata, uint32 status );
        void BalanceCurrencyRecord( KFMsg::PBBalanceData* pbdata );
    public:
        // 秘境数据
        KFMsg::PBRealmData _data;

        // 玩家的出战英雄列表
        UInt64Set _fight_hero;
    };
}

#endif