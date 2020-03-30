#include "KFRealmData.hpp"
#include "KFHero/KFHeroInterface.h"
#include "KFItem/KFItemInterface.h"

namespace KFrame
{
    KFRealmData::~KFRealmData()
    {
        Reset();
    }

    KFRealmData* KFRealmData::Reset()
    {
        _data.Clear();
        return this;
    }

    bool KFRealmData::IsInnerWorld() const
    {
        return _data.innerworld() > 0u;
    }

    KFMsg::PBBalanceItemServer* KFRealmData::FindItem( uint32 id, uint64 uuid )
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

    KFMsg::PBBalanceItem* KFRealmData::FindLose( uint32 id )
    {
        for ( auto i = 0; i < _data.loseitem_size(); ++i )
        {
            auto pbitem = _data.mutable_loseitem( i );
            if ( pbitem->id() == id )
            {
                return pbitem;
            }
        }

        auto pbitem = _data.add_loseitem();
        pbitem->set_id( id );
        return pbitem;
    }

    KFMsg::PBBalanceHeroServer* KFRealmData::FindHero( uint64 uuid )
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

    KFMsg::PBExploreData* KFRealmData::FindExeploreData( uint32 level )
    {
        auto pbexplore = _data.mutable_explore();
        auto iter = pbexplore->find( level );
        if ( iter != pbexplore->end() )
        {
            return &iter->second;
        }

        auto& pbdata = ( *pbexplore )[ level ];
        pbdata.set_level( level );
        pbdata.mutable_npcdata()->clear();
        return &pbdata;
    }

    void KFRealmData::RemoveExeploreData( uint32 level )
    {
        auto pbexplore = _data.mutable_explore();
        pbexplore->erase( level );
    }

    KFMsg::PBExploreNpcData* KFRealmData::FindNpcData( const std::string& key )
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRealmData::RecordBeginHeros( KFEntity* player )
    {
        _data.clear_herodata();

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

            RecordHeroBeginData( kfhero, _data.add_herodata() );
        }
    }

    void KFRealmData::RecordEndHeros( KFEntity* player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        for ( auto i = 0; i < _data.herodata_size(); ++i )
        {
            auto pbhero = _data.mutable_herodata( i );
            auto kfhero = _kf_hero->FindAliveHero( kfherorecord, pbhero->uuid() );
            if ( kfhero == nullptr )
            {
                continue;
            }

            RecordHeroEndData( kfhero, pbhero );
        }
    }

    void KFRealmData::RecordTownHeros( KFEntity* player )
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

            auto& townhero = *_data.mutable_townhero();
            townhero[uuid] = _data.level();
        }
    }

    void KFRealmData::RecordHeroBeginData( KFData* kfhero, KFMsg::PBBalanceHeroServer* pbhero )
    {
        pbhero->set_death( true );
        pbhero->set_uuid( kfhero->GetKeyID() );
        pbhero->set_name( kfhero->Get<std::string>( __STRING__( name ) ) );
        pbhero->set_race( kfhero->Get<uint32>( __STRING__( race ) ) );
        pbhero->set_profession( kfhero->Get<uint32>( __STRING__( profession ) ) );
        pbhero->set_sex( kfhero->Get<uint32>( __STRING__( sex ) ) );

        pbhero->set_beginexp( kfhero->Get<uint32>( __STRING__( exp ) ) );
        pbhero->set_beginlevel( kfhero->Get<uint32>( __STRING__( level ) ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 战斗属性
        auto kffighter = kfhero->Find( __STRING__( fighter ) );
        auto& pbattributes = *pbhero->mutable_beginattributes();
        for ( auto kfchild = kffighter->First(); kfchild != nullptr; kfchild = kffighter->Next() )
        {
            pbattributes[ kfchild->_data_setting->_name ] = kfchild->Get<uint32>();
        }

        // 主动技能
        auto kfactiverecord = kfhero->Find( __STRING__( active ) );
        for ( auto kfactive = kfactiverecord->First(); kfactive != nullptr; kfactive = kfactiverecord->Next() )
        {
            pbhero->add_beginactive( kfactive->GetKeyID() );
        }

        // 天赋技能
        auto kfinnaterecord = kfhero->Find( __STRING__( innate ) );
        for ( auto kfinnate = kfinnaterecord->First(); kfinnate != nullptr; kfinnate = kfinnaterecord->Next() )
        {
            pbhero->add_begininnate( kfinnate->GetKeyID() );
        }
    }

    void KFRealmData::RecordHeroEndData( KFData* kfhero, KFMsg::PBBalanceHeroServer* pbhero )
    {
        pbhero->set_death( false );
        pbhero->set_endexp( kfhero->Get<uint32>( __STRING__( exp ) ) );
        pbhero->set_endlevel( kfhero->Get<uint32>( __STRING__( level ) ) );
        if ( pbhero->endlevel() == pbhero->beginlevel() )
        {
            return;
        }

        // 战斗属性
        auto kffighter = kfhero->Find( __STRING__( fighter ) );
        auto& pbattributes = *pbhero->mutable_endattributes();
        for ( auto kfchild = kffighter->First(); kfchild != nullptr; kfchild = kffighter->Next() )
        {
            pbattributes[ kfchild->_data_setting->_name ] = kfchild->Get<uint32>();
        }

        // 主动技能
        auto kfactiverecord = kfhero->Find( __STRING__( active ) );
        for ( auto kfactive = kfactiverecord->First(); kfactive != nullptr; kfactive = kfactiverecord->Next() )
        {
            pbhero->add_endactive( kfactive->GetKeyID() );
        }

        // 天赋技能
        auto kfinnaterecord = kfhero->Find( __STRING__( innate ) );
        for ( auto kfinnate = kfinnaterecord->First(); kfinnate != nullptr; kfinnate = kfinnaterecord->Next() )
        {
            pbhero->add_endinnate( kfinnate->GetKeyID() );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRealmData::RecordBeginItems( KFEntity* player )
    {
        _data.clear_itemdata();

        // 探索背包
        auto kfitemrecord = player->Find( __STRING__( explore ) );
        RecordItemRecordData( kfitemrecord, StartType );

        // 奖励背包
        auto kfotherrecord = player->Find( __STRING__( other ) );
        RecordItemRecordData( kfotherrecord, StartType );

        // 英雄装备
        RecordHeroWeapon( player, StartType );
    }

    void KFRealmData::RecordEndItems( KFEntity* player )
    {
        // 探索背包
        auto kfitemrecord = player->Find( __STRING__( explore ) );
        RecordItemRecordData( kfitemrecord, EndType );

        // 奖励背包
        auto kfotherrecord = player->Find( __STRING__( other ) );
        RecordItemRecordData( kfotherrecord, EndType );

        // 英雄装备
        RecordHeroWeapon( player, EndType );
    }

    void KFRealmData::RecordHeroWeapon( KFEntity* player, uint32 balancetype )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto kfhero = kfherorecord->Find( kfteam->Get<uint64>() );
            if ( kfhero == nullptr )
            {
                continue;
            }

            // 计算武器
            auto kfweapon = kfhero->Find( __STRING__( weapon ) );
            if ( kfweapon != nullptr && kfweapon->GetKeyID() != 0u )
            {
                RecordItemData( kfweapon, balancetype );
            }
        }
    }

    void KFRealmData::RecordItemRecordData( KFData* kfitemrecord, uint32 balancetype )
    {
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            RecordItemData( kfitem, balancetype );
        }
    }

    void KFRealmData::RecordItemData( KFData* kfitem, uint32 balancetype )
    {
        auto count = kfitem->Get<uint32>( __STRING__( count ) );
        auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 自动销毁的道具不纪录
        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfsetting->_type );
        if ( kftypesetting == nullptr || kftypesetting->_is_auto == KFMsg::AutoDestory )
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
    void KFRealmData::BalanceRealmRecord( KFMsg::PBBalanceData* pbdata )
    {
        // 英雄
        BalanceHeroRecord( pbdata );

        // 获得的道具
        BalanceItemRecord( pbdata );

        // 失去的道具
        pbdata->mutable_loseitem()->CopyFrom( _data.loseitem() );
    }

    void KFRealmData::BalanceDropData( KFEntity* player )
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

    void KFRealmData::BalanceItemRecord( KFMsg::PBBalanceData* pbdata )
    {
        for ( auto i = 0; i < _data.itemdata_size(); ++i )
        {
            auto pbitemserver = &_data.itemdata( i );
            if ( pbitemserver->endcount() <= pbitemserver->begincount() )
            {
                continue;
            }

            auto pbitemclient = pbdata->add_gainitem();
            pbitemclient->set_id( pbitemserver->id() );
            pbitemclient->set_uuid( pbitemserver->uuid() );
            pbitemclient->set_count( pbitemserver->endcount() - pbitemserver->begincount() );
        }
    }

    bool KFRealmData::IsFightHero( uint64 herouuid )
    {
        if ( _fight_hero.empty() )
        {
            return true;
        }

        // PVE只结算出战英雄列表
        return _fight_hero.find( herouuid ) != _fight_hero.end();
    }

    void KFRealmData::BalanceHeroRecord( KFMsg::PBBalanceData* pbdata )
    {
        for ( auto i = 0; i < _data.herodata_size(); ++i )
        {
            auto pbheroserver = &_data.herodata( i );
            if ( !IsFightHero( pbheroserver->uuid() ) )
            {
                continue;
            }

            auto pbheroclient = pbdata->add_herodata();
            pbheroclient->set_death( pbheroserver->death() );
            if ( pbheroclient->death() )
            {
                pbheroclient->set_sex( pbheroserver->sex() );
                pbheroclient->set_name( pbheroserver->name() );
                pbheroclient->set_race( pbheroserver->race() );
                pbheroclient->set_profession( pbheroserver->profession() );
            }
            else
            {
                pbheroclient->set_uuid( pbheroserver->uuid() );
                pbheroclient->set_exp( pbheroserver->endexp() - pbheroserver->beginexp() );
                pbheroclient->set_level( pbheroserver->endlevel() - pbheroserver->beginlevel() );
                if ( pbheroclient->level() > 0u )
                {
                    // 战斗属性
                    BalanceHeroAttributes( pbheroserver, pbheroclient );

                    // 主动技能
                    BalanceHeroActives( pbheroserver, pbheroclient );

                    // 天赋技能
                    BalanceHeroInnates( pbheroserver, pbheroclient );
                }
            }
        }
    }

    void KFRealmData::BalanceHeroAttributes( const KFMsg::PBBalanceHeroServer* pbheroserver, KFMsg::PBBalanceHero* pbheroclient )
    {
        auto& pbattributes = *pbheroclient->mutable_attributes();
        auto& pbbeginattributes = pbheroserver->beginattributes();
        auto& pbendattributes = pbheroserver->endattributes();
        for ( auto& enditer : pbendattributes )
        {
            auto& name = enditer.first;
            auto beigniter = pbbeginattributes.find( name );
            if ( beigniter != pbbeginattributes.end() )
            {
                if ( enditer.second > beigniter->second )
                {
                    pbattributes[ name ] = enditer.second - beigniter->second;
                }
            }
        }
    }

    void KFRealmData::BalanceHeroActives( const KFMsg::PBBalanceHeroServer* pbheroserver, KFMsg::PBBalanceHero* pbheroclient )
    {
        static auto _check_begin_active = []( const KFMsg::PBBalanceHeroServer * pbheroserver, uint32 activeid )
        {
            for ( auto i = 0; i < pbheroserver->beginactive_size(); ++i )
            {
                if ( pbheroserver->beginactive( i ) == activeid )
                {
                    return true;
                }
            }
            return false;
        };

        for ( auto i = 0; i < pbheroserver->endactive_size(); ++i )
        {
            auto activeid = pbheroserver->endactive( i );
            if ( !_check_begin_active( pbheroserver, activeid ) )
            {
                ( *pbheroclient->mutable_active() )[ pbheroclient->active_size() + 1 ] = activeid;
            }
        }
    }

    void KFRealmData::BalanceHeroInnates( const KFMsg::PBBalanceHeroServer* pbheroserver, KFMsg::PBBalanceHero* pbheroclient )
    {
        static auto _check_begin_innate = []( const KFMsg::PBBalanceHeroServer * pbheroserver, uint32 innateid )
        {
            for ( auto i = 0; i < pbheroserver->begininnate_size(); ++i )
            {
                if ( pbheroserver->begininnate( i ) == innateid )
                {
                    return true;
                }
            }
            return false;
        };

        for ( auto i = 0; i < pbheroserver->endinnate_size(); ++i )
        {
            auto innateid = pbheroserver->endinnate( i );
            if ( !_check_begin_innate( pbheroserver, innateid ) )
            {
                ( *pbheroclient->mutable_innate() )[ pbheroclient->innate_size() + 1 ] = innateid;
            }
        }
    }
}