#include "KFExploreData.hpp"
#include "KFHero/KFHeroInterface.h"

namespace KFrame
{
    KFExploreRecord::~KFExploreRecord()
    {
        Reset();
    }

    KFExploreRecord* KFExploreRecord::Reset()
    {
        _data.Clear();
        _npcs.Clear();
        return this;
    }

    KFMsg::PBBalanceItemServer* KFExploreRecord::FindItem( const std::string& name, uint32 key )
    {
        for ( auto i = 0; i < _data.itemdata_size(); ++i )
        {
            auto pbitem = _data.mutable_itemdata( i );
            if ( pbitem->name() == name && pbitem->key() == key )
            {
                return pbitem;
            }
        }

        auto pbitem = _data.add_itemdata();
        pbitem->set_name( name );
        pbitem->set_key( key );
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

    void KFExploreRecord::BalanceHeroBeginData( KFEntity* player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        for ( uint32 i = KFDataDefine::Array_Index; i < kfteamarray->Size(); ++i )
        {
            auto kfteam = kfteamarray->Find( i );
            if ( kfteam == nullptr )
            {
                continue;
            }

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
        }
    }

    void KFExploreRecord::BalanceHeroEndData( KFEntity* player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        for ( auto i = 0; i < _data.herodata_size(); ++i )
        {
            auto pbhero = _data.mutable_herodata( i );
            auto kfhero = _kf_hero->FindAliveHero( kfherorecord, pbhero->uuid() );
            if ( kfhero != nullptr )
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
                        pbattributes[ kfchild->_data_setting->_name ] = kfchild->Get<uint32>();
                    }
                }
            }
            else
            {
                pbhero->set_death( true );
            }
        }
    }

    void KFExploreRecord::BalanceItemBeginData( KFEntity* player )
    {
        // 所有货币
        for ( auto iter : KFCurrencyConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            auto pbcurrency = FindItem( kfsetting->_name, 0u );
            pbcurrency->set_beginvalue( player->Get<uint32>( kfsetting->_name ) );
        }

        // 道具
        auto kfitemrecord = player->Find( __STRING__( item ) );
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto count = kfitem->Get<uint32>( __STRING__( count ) );
            auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            auto pbitem = FindItem( __STRING__( item ), itemid );
            pbitem->set_beginvalue( pbitem->beginvalue() + count );
        }
    }

    void KFExploreRecord::BalanceItemEndData( KFEntity* player )
    {
        // 所有货币
        for ( auto iter : KFCurrencyConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            auto pbcurrency = FindItem( kfsetting->_name, 0u );
            pbcurrency->set_endvalue( player->Get<uint32>( kfsetting->_name ) );
        }

        // 道具
        auto kfitemrecord = player->Find( __STRING__( item ) );
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto count = kfitem->Get<uint32>( __STRING__( count ) );
            auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            auto pbitem = FindItem( __STRING__( item ), itemid );
            pbitem->set_endvalue( pbitem->endvalue() + count );
        }
    }

    void KFExploreRecord::BalanceRecord( KFEntity* player, KFMsg::PBBalanceData* pbdata )
    {
        //__LOG_INFO__( "{}", _data.DebugString() );

        // 结算英雄
        for ( auto i = 0; i < _data.herodata_size(); ++i )
        {
            auto pbheroserver = &_data.herodata( i );
            auto pbheroclient = pbdata->add_herodata();

            pbheroclient->set_uuid( pbheroserver->uuid() );
            pbheroclient->set_death( pbheroserver->death() );
            if ( pbheroclient->death() )
            {
                pbheroclient->set_name( pbheroserver->name() );
                pbheroclient->set_race( pbheroserver->race() );
                pbheroclient->set_profession( pbheroserver->profession() );
                pbheroclient->set_sex( pbheroserver->sex() );
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

        // 结算道具
        for ( auto i = 0; i < _data.itemdata_size(); ++i )
        {
            auto pbitemserver = &_data.itemdata( i );
            if ( pbitemserver->endvalue() <= pbitemserver->beginvalue() )
            {
                continue;
            }

            auto pbitemclient = pbdata->add_itemdata();
            pbitemclient->set_name( pbitemserver->name() );
            pbitemclient->set_key( pbitemserver->key() );
            pbitemclient->set_value( pbitemserver->endvalue() - pbitemserver->beginvalue() );
        }

        //__LOG_INFO__( "{}", pbdata->DebugString() );
    }

    KFMsg::PBExploreNpcData* KFExploreRecord::FindNpcData( const std::string& key )
    {
        auto npcdatalist = _data.mutable_exploredata()->mutable_npcdata();
        auto iter = npcdatalist->find( key );
        if ( iter != npcdatalist->end() )
        {
            return &iter->second;
        }

        auto& npcdata = ( *npcdatalist )[ key ];
        return &npcdata;
    }
}