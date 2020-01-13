#include "KFExploreData.hpp"
#include "KFHero/KFHeroInterface.h"
#include "KFItem/KFItemInterface.h"

namespace KFrame
{
    KFExploreRecord::~KFExploreRecord()
    {
        Reset();
    }

    KFExploreRecord* KFExploreRecord::Reset()
    {
        _data.Clear();
        return this;
    }

    KFMsg::PBBalanceItemServer* KFExploreRecord::FindItem( uint32 id, uint64 uuid )
    {
        for ( auto i = 0; i < _data.itemdata_size(); ++i )
        {
            auto pbitem = _data.mutable_itemdata( i );
            if ( pbitem->id() == id && pbitem->uuid() == uuid )
            {
                return pbitem;
            }
        }

        auto pbitem = _data.add_itemdata();
        pbitem->set_id( id );
        pbitem->set_uuid( uuid );
        return pbitem;
    }

    KFMsg::PBBalanceHeroServer* KFExploreRecord::FindHero( uint64 uuid )
    {
        for ( auto i = 0; i < _data.herodata_size(); ++i )
        {
            auto pbhero = _data.mutable_herodata( i );
            if ( pbhero->uuid() == uuid )
            {
                return pbhero;
            }
        }

        return nullptr;
    }

    KFMsg::PBBalanceCurrency* KFExploreRecord::FindCurrency( const std::string& name )
    {
        for ( auto i = 0; i < _data.currencydata_size(); ++i )
        {
            auto pbcurrency = _data.mutable_currencydata( i );
            if ( pbcurrency->name() == name )
            {
                return pbcurrency;
            }
        }

        auto pbcurrency = _data.add_currencydata();
        pbcurrency->set_name( name );
        return pbcurrency;
    }

    KFMsg::PBExploreData* KFExploreRecord::FindExeploreData( uint32 level )
    {
        auto pbexplore = _data.mutable_explore();
        auto iter = pbexplore->find( level );
        if ( iter != pbexplore->end() )
        {
            return &iter->second;
        }

        auto& pbdata = ( *pbexplore )[ level ];
        pbdata.set_level( level );
        return &pbdata;
    }

    void KFExploreRecord::RemoveExeploreData( uint32 level )
    {
        auto pbexplore = _data.mutable_explore();
        pbexplore->erase( level );
    }

    KFMsg::PBExploreNpcData* KFExploreRecord::FindNpcData( const std::string& key )
    {
        auto pbexplore = FindExeploreData( _data.level() );
        auto npcdatalist = pbexplore->mutable_npcdata();
        auto iter = npcdatalist->find( key );
        if ( iter != npcdatalist->end() )
        {
            return &iter->second;
        }

        auto& npcdata = ( *npcdatalist )[ key ];
        return &npcdata;
    }

    void KFExploreRecord::AddBuffData( uint64 uuid, uint32 value )
    {
        auto buff = _data.mutable_buffdata()->mutable_buff();

        KFMsg::PBBuffListData* bufflistdata = nullptr;
        auto iter = buff->find( uuid );
        if ( iter != buff->end() )
        {
            bufflistdata = &iter->second;
        }
        else
        {
            bufflistdata = &( ( *buff )[ uuid ] );
        }

        auto buffdata = bufflistdata->add_bufflist();
        buffdata->set_id( value );
    }

    void KFExploreRecord::RemoveBuffData( uint64 uuid, uint32 value )
    {
        auto buff = _data.mutable_buffdata()->mutable_buff();
        auto iter = buff->find( uuid );
        if ( iter == buff->end() )
        {
            return;
        }

        // 删除该buff
        auto bufflistdata = &iter->second;
        auto buffsize = bufflistdata->bufflist_size();
        for ( int32 index = 0; index < buffsize; ++index )
        {
            auto buff = &bufflistdata->bufflist( index );
            if ( buff->id() == value )
            {
                bufflistdata->mutable_bufflist()->DeleteSubrange( index, 1 );
                break;
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFExploreRecord::BalanceBeginData( KFEntity* player )
    {
        // 纪录货币的初始值
        BalanceCurrencyBeginData( player );

        // 纪录英雄的初始数值
        BalanceHeroBeginData( player );

        // 纪录背包的道具数值
        BalanceItemBeginData( player );
    }

    void KFExploreRecord::BalanceEndData( KFEntity* player )
    {
        // 货币
        BalanceCurrencyEndData( player );

        // 英雄最终数据
        BalanceHeroEndData( player );

        // 道具最终数据
        BalanceItemEndData( player );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KFExploreRecord::BalanceHeroBeginData( KFEntity* player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto uuid = kfteam->Get<uint64>();
            auto kfhero = _kf_hero->FindAliveHero( kfherorecord, uuid );
            if ( kfhero == nullptr )
            {
                continue;
            }

            auto pbhero = _data.add_herodata();
            pbhero->set_uuid( uuid );
            pbhero->set_name( kfhero->Get<std::string>( __STRING__( name ) ) );
            pbhero->set_race( kfhero->Get<uint32>( __STRING__( race ) ) );
            pbhero->set_profession( kfhero->Get<uint32>( __STRING__( profession ) ) );
            pbhero->set_sex( kfhero->Get<uint32>( __STRING__( sex ) ) );
            pbhero->set_beginexp( kfhero->Get<uint32>( __STRING__( exp ) ) );
            pbhero->set_beginlevel( kfhero->Get<uint32>( __STRING__( level ) ) );

            // 战斗属性
            auto kffighter = kfhero->Find( __STRING__( fighter ) );
            auto& pbattributes = *pbhero->mutable_beginattributes();
            for ( auto kfchild = kffighter->First(); kfchild != nullptr; kfchild = kffighter->Next() )
            {
                pbattributes[ kfchild->_data_setting->_name ] = kfchild->Get<uint32>();
            }

            // 计算武器
            auto kfweapon = kfhero->Find( __STRING__( weapon ) );
            if ( kfweapon != nullptr && kfweapon->GetKeyID() != 0u )
            {
                BalanceItemData( kfweapon, StartType );
            }
        }
    }

    void KFExploreRecord::BalanceHeroEndData( KFEntity* player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        for ( auto i = 0; i < _data.herodata_size(); ++i )
        {
            auto pbhero = _data.mutable_herodata( i );
            auto kfhero = kfherorecord->Find( pbhero->uuid() );
            auto deathreason = _kf_hero->GetHeroDeathReason( kfhero );

            // 对有效单位计算
            if ( deathreason == KFMsg::NoDeathReason )
            {
                pbhero->set_death( false );
                pbhero->set_endexp( kfhero->Get<uint32>( __STRING__( exp ) ) );
                pbhero->set_endlevel( kfhero->Get<uint32>( __STRING__( level ) ) );
                if ( pbhero->endlevel() != pbhero->beginlevel() )
                {
                    // 战斗属性
                    auto kffighter = kfhero->Find( __STRING__( fighter ) );
                    auto& pbattributes = *pbhero->mutable_endattributes();
                    for ( auto kfchild = kffighter->First(); kfchild != nullptr; kfchild = kffighter->Next() )
                    {
                        pbattributes[kfchild->_data_setting->_name] = kfchild->Get<uint32>();
                    }
                }

                // 计算武器
                auto kfweapon = kfhero->Find( __STRING__( weapon ) );
                if ( kfweapon != nullptr && kfweapon->GetKeyID() != 0u )
                {
                    BalanceItemData( kfweapon, EndType );
                }
            }
            else
            {
                pbhero->set_death( true );
                pbhero->set_deathreason( deathreason );
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFExploreRecord::BalanceItemBeginData( KFEntity* player )
    {
        // 道具
        auto kfitemrecord = player->Find( __STRING__( explore ) );
        BalanceItemRecordData( kfitemrecord, StartType );

        // other
        auto kfotherrecord = player->Find( __STRING__( other ) );
        BalanceItemRecordData( kfotherrecord, StartType );
    }

    void KFExploreRecord::BalanceItemEndData( KFEntity* player )
    {
        // 道具
        auto kfitemrecord = player->Find( __STRING__( explore ) );
        BalanceItemRecordData( kfitemrecord, EndType );

        // other
        auto kfotherrecord = player->Find( __STRING__( other ) );
        BalanceItemRecordData( kfotherrecord, EndType );
    }

    void KFExploreRecord::BalanceItemRecordData( KFData* kfitemrecord, uint32 balancetype )
    {
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            BalanceItemData( kfitem, balancetype );
        }
    }

    void KFExploreRecord::BalanceItemData( KFData* kfitem, uint32 balancetype )
    {
        auto count = kfitem->Get<uint32>( __STRING__( count ) );
        auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        KFMsg::PBBalanceItemServer* pbitem = nullptr;
        if ( kfsetting->IsOverlay() )
        {
            pbitem = FindItem( itemid, 0u );
        }
        else
        {
            pbitem = FindItem( itemid, kfitem->GetKeyID() );
        }

        switch ( balancetype )
        {
        case StartType:
            pbitem->set_begincount( pbitem->begincount() + count );
            break;
        case EndType:
            pbitem->set_endcount( pbitem->endcount() + count );
            break;
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFExploreRecord::BalanceCurrencyBeginData( KFEntity* player )
    {
        // 所有货币
        for ( auto& iter : KFCurrencyConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            auto pbcurrency = FindCurrency( kfsetting->_name );
            pbcurrency->set_beginvalue( player->Get<uint32>( kfsetting->_name ) );
        }
    }

    void KFExploreRecord::BalanceCurrencyEndData( KFEntity* player )
    {
        // 所有货币
        for ( auto& iter : KFCurrencyConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            auto pbcurrency = FindCurrency( kfsetting->_name );
            pbcurrency->set_endvalue( player->Get<uint32>( kfsetting->_name ) );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFExploreRecord::BalanceRecord( KFMsg::PBBalanceData* pbdata, uint32 status )
    {
        //__LOG_INFO__( "{}", _data.DebugString() );

        // 英雄
        BalanceHeroRecord( pbdata );

        // 道具
        BalanceItemRecord( pbdata, status );

        // 货币
        BalanceCurrencyRecord( pbdata );

        //__LOG_INFO__( "{}", pbdata->DebugString() );
    }

    void KFExploreRecord::BalanceDrop( KFEntity* player )
    {
        KFMsg::PBShowElement pbelement;
        if ( !player->GetShowElement( &pbelement ) )
        {
            return;
        }

        for ( auto i = 0; i < pbelement.pbdata_size(); i++ )
        {
            auto& showdata = pbelement.pbdata( i );
            if ( showdata.name() == __STRING__( item ) )
            {
                uint32 itemid = 0u;
                uint32 itemcount = 0u;
                uint64 itemuuid = 0u;

                for ( auto& iter : showdata.pbuint64() )
                {
                    if ( iter.first == __STRING__( id ) )
                    {
                        itemid = iter.second;
                    }
                    else if ( iter.first == __STRING__( count ) )
                    {
                        itemcount = iter.second;
                    }
                    else if ( iter.first == __STRING__( uuid ) )
                    {
                        itemuuid = iter.second;
                    }
                }

                auto pbitem = FindItem( itemid, itemuuid );
                pbitem->set_endcount( pbitem->endcount() + itemcount );
            }
        }
    }

    void KFExploreRecord::BalanceHeroRecord( KFMsg::PBBalanceData* pbdata )
    {
        for ( auto i = 0; i < _data.herodata_size(); ++i )
        {
            auto pbheroserver = &_data.herodata( i );

            // PVE只结算出战英雄列表
            if ( !_fight_hero.empty() && _fight_hero.find( pbheroserver->uuid() ) == _fight_hero.end() )
            {
                continue;
            }

            auto pbheroclient = pbdata->add_herodata();

            pbheroclient->set_uuid( pbheroserver->uuid() );
            pbheroclient->set_death( pbheroserver->death() );
            if ( pbheroclient->death() )
            {
                pbheroclient->set_name( pbheroserver->name() );
                pbheroclient->set_race( pbheroserver->race() );
                pbheroclient->set_profession( pbheroserver->profession() );
                pbheroclient->set_sex( pbheroserver->sex() );
                pbheroclient->set_deathreason( pbheroserver->deathreason() );
            }
            else
            {
                pbheroclient->set_exp( pbheroserver->endexp() - pbheroserver->beginexp() );
                pbheroclient->set_level( pbheroserver->endlevel() - pbheroserver->beginlevel() );
                if ( pbheroclient->level() > 0u )
                {
                    // 战斗属性
                    auto& pbattributes = *pbheroclient->mutable_attributes();
                    auto& pbbeginattributes = pbheroserver->beginattributes();
                    auto& pbendattributes = pbheroserver->endattributes();
                    for ( auto& iter : pbendattributes )
                    {
                        auto& name = iter.first;
                        auto beigniter = pbbeginattributes.find( name );
                        if ( beigniter != pbbeginattributes.end() )
                        {
                            if ( iter.second > beigniter->second )
                            {
                                pbattributes[ name ] = iter.second - beigniter->second;
                            }
                        }
                    }
                }
            }
        }
    }

    void KFExploreRecord::BalanceItemRecord( KFMsg::PBBalanceData* pbdata, uint32 status )
    {
        for ( auto i = 0; i < _data.itemdata_size(); ++i )
        {
            auto pbitemserver = &_data.itemdata( i );
            if ( pbitemserver->endcount() <= pbitemserver->begincount() )
            {
                continue;
            }

            // 探索自动销毁的道具不做统计
            if ( status == KFMsg::ExploreStatus )
            {
                auto auto_type = _kf_item->GetItemAutoType( pbitemserver->id() );
                if ( auto_type == KFMsg::AutoDestory )
                {
                    continue;
                }
            }

            auto pbitemclient = pbdata->add_itemdata();
            pbitemclient->set_id( pbitemserver->id() );
            pbitemclient->set_uuid( pbitemserver->uuid() );
            pbitemclient->set_count( pbitemserver->endcount() - pbitemserver->begincount() );
        }
    }

    void KFExploreRecord::BalanceCurrencyRecord( KFMsg::PBBalanceData* pbdata )
    {
        for ( auto i = 0; i < _data.currencydata_size(); ++i )
        {
            auto pbcurrencyserver = &_data.currencydata( i );

            auto pbcurrencyclient = pbdata->add_currencydata();
            pbcurrencyclient->CopyFrom( *pbcurrencyserver );
        }
    }
}