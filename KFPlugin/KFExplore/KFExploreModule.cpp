#include "KFExploreModule.hpp"

namespace KFrame
{
    void KFExploreModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_LEAVE_PLAYER__( &KFExploreModule::OnLeaveSaveExploreRecord );
        __REGISTER_DROP_LOGIC__( __STRING__( exp ), &KFExploreModule::OnDropHeroExp );
        __REGISTER_DROP_LOGIC__( __STRING__( addhp ), &KFExploreModule::OnDropHeroAddHp );
        __REGISTER_DROP_LOGIC__( __STRING__( dechp ), &KFExploreModule::OnDropHeroDecHp );
        __REGISTER_DROP_LOGIC__( __STRING__( addbuff ), &KFExploreModule::OnDropHeroAddBuff );
        __REGISTER_DROP_LOGIC__( __STRING__( decbuff ), &KFExploreModule::OnDropHeroDecBuff );

        __REGISTER_EXECUTE__( __STRING__( pve ), &KFExploreModule::OnExecutePVEFighter );
        __REGISTER_EXECUTE__( __STRING__( explore ), &KFExploreModule::OnExecuteExplore );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_PVE_REQ, &KFExploreModule::HandlePVEReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_REQ, &KFExploreModule::HandleExploreReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_PVE_BALANCE_REQ, &KFExploreModule::HandlePVEBalanceReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXIT_EXPLORE_REQ, &KFExploreModule::HandleExploreExitReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_BALANCE_REQ, &KFExploreModule::HandleExploreBalanceReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_PLAYER_REQ, &KFExploreModule::HandleUpdateExplorePlayerReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_NPC_REQ, &KFExploreModule::HaneleUpdateExploreNpcReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_FIGHTER_HERO_REQ, &KFExploreModule::HandleUpdateFighterHeroReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_DROP_REQ, &KFExploreModule::HandleExploreDropReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_KILL_NPC_REQ, &KFExploreModule::HandleKillNpcReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_FAITH_REQ, &KFExploreModule::HandleUpdateFaithReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_INTERACT_ITEM_REQ, &KFExploreModule::HandleInteractItemReq );

    }

    void KFExploreModule::BeforeShut()
    {
        __UN_LEAVE_PLAYER__();

        __UN_DROP_LOGIC__( __STRING__( exp ) );
        __UN_DROP_LOGIC__( __STRING__( addhp ) );
        __UN_DROP_LOGIC__( __STRING__( dechp ) );
        __UN_DROP_LOGIC__( __STRING__( addbuff ) );
        __UN_DROP_LOGIC__( __STRING__( decbuff ) );

        __UN_EXECUTE__( __STRING__( pve ) );
        __UN_EXECUTE__( __STRING__( explore ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_PVE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_PVE_BALANCE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXIT_EXPLORE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_BALANCE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_PLAYER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_NPC_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_FIGHTER_HERO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_DROP_REQ );
        __UN_MESSAGE__( KFMsg::MSG_KILL_NPC_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_FAITH_REQ );
        __UN_MESSAGE__( KFMsg::MSG_INTERACT_ITEM_REQ );
    }

    __KF_EXECUTE_FUNCTION__( KFExploreModule::OnExecuteExplore )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute explore param size<1!" );
            return false;
        }

        auto mapid = executedata->_param_list._params[ 0 ]->_int_value;
        if ( mapid == 0u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute explore mapid=[{}]!", mapid );
            return false;
        }

        auto result = RequestExplore( player, mapid, modulename, moduleid );
        if ( result != KFMsg::Ok )
        {
            _kf_display->SendToClient( player, result );
        }

        return result == KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleExploreReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExploreReq );

        auto result = RequestExplore( player, kfmsg.mapid(), _invalid_string, _invalid_int );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }
    }

    uint32 KFExploreModule::RequestExplore( KFEntity* player, uint32 mapid, const std::string& modulename, uint64 moduleid )
    {
        auto status = player->GetStatus();
        if ( status == KFMsg::ExploreStatus )
        {
            return KFMsg::ExploreAlready;
        }

        if ( status == KFMsg::PVEStatus )
        {
            return KFMsg::PVEAlready;
        }

        auto count = _kf_hero_team->GetAliveHeroCount( player );
        if ( count == 0u )
        {
            player->UpdateData( __STRING__( mapid ), KFEnum::Set, 0u );
            return KFMsg::ExploreHeroTeamEmpty;
        }

        auto kfsetting = KFExploreConfig::Instance()->FindSetting( mapid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::ExploreMapError;
        }

        auto lastmapid = player->Get<uint32>( __STRING__( mapid ) );
        if ( lastmapid != 0u )
        {
            if ( lastmapid != mapid )
            {
                return KFMsg::ExploreMapLimit;
            }
        }
        else
        {
            player->UpdateData( __STRING__( mapid ), KFEnum::Set, mapid );
        }

        auto kfrecord = InitExploreRecord( player, mapid, modulename, moduleid );
        kfrecord->_data.set_starttime( KFGlobal::Instance()->_real_time );
        player->SetStatus( KFMsg::ExploreStatus );

        KFMsg::MsgExploreAck ack;
        ack.set_mapid( mapid );
        ack.mutable_exploredata()->CopyFrom( kfrecord->_data.exploredata() );
        _kf_player->SendToClient( player, KFMsg::MSG_EXPLORE_ACK, &ack );
        return KFMsg::Ok;
    }

    __KF_EXECUTE_FUNCTION__( KFExploreModule::OnExecutePVEFighter )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "pve execute param size<2" );
            return false;
        }

        auto pveid = executedata->_param_list._params[ 0 ]->_int_value;
        auto battleid = executedata->_param_list._params[ 1 ]->_int_value;
        if ( pveid == 0u || battleid == 0u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "pve execute param pveid=[{}] battleid=[{}]!", pveid, battleid );
            return false;
        }

        auto result = PVEFighter( player, pveid, battleid, modulename, moduleid );
        if ( result != KFMsg::Ok )
        {
            _kf_display->SendToClient( player, result );
        }

        return result == KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandlePVEReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPVEReq );

        auto mapid = player->Get<uint32>( __STRING__( mapid ) );
        auto result = PVEFighter( player, kfmsg.pveid(), kfmsg.battleid(), __STRING__( explore ), mapid );
        if ( result != KFMsg::Ok )
        {
            _kf_display->SendToClient( player, result );
        }
    }

    uint32 KFExploreModule::PVEFighter( KFEntity* player, uint32 pveid, uint32 battleid, const std::string& modulename, uint64 moduleid )
    {
        auto status = player->GetStatus();
        if ( status == KFMsg::PVEStatus )
        {
            return KFMsg::PVEAlready;
        }

        auto count = _kf_hero_team->GetAliveHeroCount( player );
        if ( count == 0u )
        {
            return KFMsg::ExploreHeroTeamEmpty;
        }

        auto kfsetting = KFPVEConfig::Instance()->FindSetting( pveid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::PVEIdError;
        }

        // 初始化纪录
        auto kfrecord = _pve_record.Create( player->GetKeyID() );
        kfrecord->Reset();
        kfrecord->BalanceCurrencyBeginData( player );
        kfrecord->BalanceHeroBeginData( player );
        kfrecord->_data.set_status( status );
        kfrecord->_data.set_id( pveid );
        kfrecord->_data.set_moduleid( moduleid );
        kfrecord->_data.set_modulename( modulename );

        KFMsg::MsgPVEAck ack;
        ack.set_pveid( pveid );
        ack.set_battleid( battleid );

        // team
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

            auto& pbobject = ( *ack.mutable_hero() )[ uuid ];
            auto pbhero = _kf_kernel->SerializeToClient( kfhero );
            pbobject.CopyFrom( *pbhero );
        }

        // npc
        auto npclevel = 1;
        for ( auto generateid : kfsetting->_npc_generate_list )
        {
            auto kfnpc = _kf_generate->GenerateNpcHero( player, generateid, npclevel );
            if ( kfnpc == nullptr )
            {
                continue;
            }

            auto& pbobject = ( *ack.mutable_npc() )[ kfnpc->GetKeyID() ];
            auto pbhero = _kf_kernel->SerializeToClient( kfnpc );
            pbobject.CopyFrom( *pbhero );

            // 添加到纪录中
            kfrecord->_npcs.Insert( kfnpc->GetKeyID(), kfnpc );
        }

        // buff
        auto kfexplorerecord = _explore_record.Find( player->GetKeyID() );
        if ( kfexplorerecord != nullptr )
        {
            auto exploredata = kfexplorerecord->_data.mutable_exploredata();
            ack.mutable_buffdata()->CopyFrom( exploredata->buffdata() );

            // 信仰值
            ack.set_faith( exploredata->faith() );
        }

        _kf_player->SendToClient( player, KFMsg::MSG_PVE_ACK, &ack );

        player->SetStatus( KFMsg::PVEStatus );
        player->UpdateData( __STRING__( pveid ), KFEnum::Set, pveid );
        return KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandlePVEBalanceReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPVEBalanceReq );
        auto kfrecord = _pve_record.Find( playerid );
        if ( kfrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::PVENotInStatus );
        }

        for ( auto i = 0; i < kfmsg.herolist_size(); i++ )
        {
            _fight_hero[player->GetKeyID()].insert( kfmsg.herolist( i ) );
        }

        // 先计算击杀npc统计
        StatisticsHeroKillNpcs( player, kfrecord, kfmsg.result() );

        auto teamheronum = _kf_hero_team->GetHeroCount( player );
        auto teamdeathnum = _kf_hero_team->GetDeadHeroCount( player );
        __LOG_DEBUG__( "player=[{}] pve balance result=[{}]!", playerid, kfmsg.result() );

        player->SetStatus( kfrecord->_data.status() );

        auto kfexplorerecord = _explore_record.Find( playerid );
        if ( kfexplorerecord == nullptr )
        {
            // 如果不在探索里面, 删除死亡英雄
            _kf_hero_team->RemoveTeamDeadHero( player );
            _kf_hero_team->DecTeamHeroDurability( player );
        }

        auto kfsetting = KFPVEConfig::Instance()->FindSetting( kfrecord->_data.id() );
        if ( kfsetting != nullptr )
        {
            switch ( kfmsg.result() )
            {
            case KFMsg::Victory:
                ExploreDropElement( player, kfsetting->_victory_drop_list, kfrecord );
                break;
            case KFMsg::Failed:
                ExploreDropElement( player, kfsetting->_fail_drop_list, kfrecord );
                break;
            }
        }

        _fight_hero.erase( player->GetKeyID() );

        // 结算最终数据
        kfrecord->BalanceCurrencyEndData( player );
        kfrecord->BalanceHeroEndData( player );

        // 发送给客户端
        KFMsg::MsgPVEBalanceAck ack;
        ack.set_result( kfmsg.result() );
        ack.set_moduleid( kfrecord->_data.moduleid() );
        ack.set_modulename( kfrecord->_data.modulename() );
        kfrecord->BalanceRecord( ack.mutable_balance() );
        _kf_player->SendToClient( player, KFMsg::MSG_PVE_BALANCE_ACK, &ack, 1u );

        // 设置回调属性
        if ( kfmsg.result() != KFMsg::Flee )
        {
            auto kfbalance = player->Find( __STRING__( balance ) );
            kfbalance->Set( __STRING__( id ), kfrecord->_data.id() );
            kfbalance->Set( __STRING__( pveresult ), 0u );

            player->UpdateData( kfbalance, __STRING__( team ), KFEnum::Set, teamheronum );
            player->UpdateData( kfbalance, __STRING__( death ), KFEnum::Set, teamdeathnum );
            player->UpdateData( kfbalance, __STRING__( truns ), KFEnum::Set, kfmsg.truns() );
            player->UpdateData( kfbalance, __STRING__( pveresult ), KFEnum::Set, kfmsg.result() );

            if ( kfmsg.result() == KFMsg::Victory )
            {
                player->UpdateData( __STRING__( pvevictory ), KFEnum::Add, 1u );
            }
        }

        // 删除纪录
        _pve_record.Remove( playerid );

        // 设置pveid为0
        player->Set( __STRING__( pveid ), 0u );
        player->CleanData( __STRING__( npc ), false );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleExploreExitReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExitExploreReq );

        // 逃跑结算
        ExploreBalance( player, KFMsg::Flee );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleExploreBalanceReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExploreBalanceReq );

        // 探索结算
        ExploreBalance( player, kfmsg.result() );
    }

    void KFExploreModule::ExploreBalance( KFEntity* player, uint32 result )
    {
        auto kfrecord = _explore_record.Find( player->GetKeyID() );
        if ( kfrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ExploreNotInStatus );
        }

        auto teamheronum = _kf_hero_team->GetHeroCount( player );
        auto teamdeathnum = _kf_hero_team->GetDeadHeroCount( player );

        player->SetStatus( kfrecord->_data.status() );

        _kf_hero_team->RemoveTeamDeadHero( player );
        _kf_hero_team->DecTeamHeroDurability( player );
        __LOG_DEBUG__( "player=[{}] explore=[{}] balance result=[{}]!", player->GetKeyID(), kfrecord->_data.id(), result );

        auto kfsetting = KFExploreConfig::Instance()->FindSetting( kfrecord->_data.id() );
        if ( kfsetting != nullptr )
        {
            switch ( result )
            {
            case KFMsg::Victory:
                ExploreDropElement( player, kfsetting->_victory_drop_list, kfrecord );
                break;
            case KFMsg::Failed:
                ExploreDropElement( player, kfsetting->_fail_drop_list, kfrecord );
                break;
            }
        }

        // 结算最终数据
        kfrecord->BalanceHeroEndData( player );
        kfrecord->BalanceItemEndData( player );
        kfrecord->BalanceCurrencyEndData( player );

        // 发送消息
        KFMsg::MsgExploreBalanceAck ack;
        ack.set_result( result );
        kfrecord->BalanceRecord( ack.mutable_balance() );
        _kf_player->SendToClient( player, KFMsg::MSG_EXPLORE_BALANCE_ACK, &ack, 1u );

        // 清除探索内道具背包
        _kf_item->CleanItem( player, __STRING__( explore ), true );
        _kf_item->CleanItem( player, __STRING__( other ), true );

        player->Set( __STRING__( random ), KFEnum::Set, 0 );
        player->Set( __STRING__( explorerecord ), _invalid_string );
        player->UpdateData( __STRING__( mapid ), KFEnum::Set, 0u );

        // 设置回调属性
        if ( result != KFMsg::Flee )
        {
            auto kfbalance = player->Find( __STRING__( balance ) );
            kfbalance->Set( __STRING__( id ), kfrecord->_data.id() );
            kfbalance->Set( __STRING__( exploreresult ), 0u );

            player->UpdateData( kfbalance, __STRING__( team ), KFEnum::Set, teamheronum );
            player->UpdateData( kfbalance, __STRING__( death ), KFEnum::Set, teamdeathnum );

            auto usetime = KFGlobal::Instance()->_real_time - kfrecord->_data.starttime();
            usetime += kfrecord->_data.usetime();
            player->UpdateData( kfbalance, __STRING__( time ), KFEnum::Set, usetime );

            player->UpdateData( kfbalance, __STRING__( exploreresult ), KFEnum::Set, result );
        }

        // 清除纪录
        _explore_record.Remove( player->GetKeyID() );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFExploreRecord* KFExploreModule::InitExploreRecord( KFEntity* player, uint32 mapid, const std::string& modulename, uint64 moduleid )
    {
        auto kfrecord = _explore_record.Create( player->GetKeyID() );
        kfrecord->Reset();

        auto record = player->Get<std::string>( __STRING__( explorerecord ) );
        if ( !record.empty() )
        {
            player->Set( __STRING__( explorerecord ), _invalid_string );
            auto ok = kfrecord->_data.ParseFromString( record );
            if ( ok )
            {
                return kfrecord;
            }

            __LOG_ERROR__( "player=[{}] parse explorerecord failed!", player->GetKeyID() );
        }

        kfrecord->_data.set_id( mapid );
        kfrecord->_data.set_moduleid( moduleid );
        kfrecord->_data.set_modulename( modulename );
        kfrecord->_data.set_status( player->GetStatus() );
        kfrecord->_data.mutable_exploredata()->set_random( KFGlobal::Instance()->RandRatio( ( uint32 )__MAX_INT32__ ) );
        kfrecord->_data.mutable_exploredata()->set_faith( 0u );

        // 纪录英雄的初始数值
        kfrecord->BalanceHeroBeginData( player );

        // 纪录背包的道具数值
        kfrecord->BalanceItemBeginData( player );

        // 纪录货币的初始值
        kfrecord->BalanceCurrencyBeginData( player );

        return kfrecord;
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFExploreModule::OnLeaveSaveExploreRecord )
    {
        _pve_record.Remove( player->GetKeyID() );

        auto kfrecord = _explore_record.Find( player->GetKeyID() );
        if ( kfrecord != nullptr )
        {
            auto usetime = KFGlobal::Instance()->_real_time - kfrecord->_data.starttime();
            kfrecord->_data.set_usetime( kfrecord->_data.usetime() + usetime );

            auto strbalance = kfrecord->_data.SerializeAsString();
            player->Set( __STRING__( explorerecord ), strbalance );
            _explore_record.Remove( player->GetKeyID() );
        }
        else
        {
            player->Set( __STRING__( explorerecord ), _invalid_string );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFExploreModule::ExploreDropElement( KFEntity* player, const VectorUInt32& droplist, KFExploreRecord* kfrecord )
    {
        auto& dropdatalist = _kf_drop->Drop( player, droplist, _invalid_string, __FUNC_LINE__ );
        for ( auto dropdata : dropdatalist )
        {
            for ( auto kfelement : dropdata->_elements._element_list )
            {
                // 这里只计算可以叠加的物品道具
                if ( kfelement->IsObject() && kfelement->_data_name == __STRING__( item ) )
                {
                    auto kfelementobject = reinterpret_cast< const KFElementObject* >( kfelement );
                    auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
                    if ( kfsetting->IsOverlay() )
                    {
                        auto itemcount = kfelementobject->GetValue( __STRING__( count ) );
                        auto pbitem = kfrecord->FindItem( kfelementobject->_config_id, 0u );
                        pbitem->set_endcount( pbitem->endcount() + itemcount );
                    }
                }
            }
        }
    }

    __KF_DROP_LOGIC_FUNCTION__( KFExploreModule::OnDropHeroExp )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );

        auto heroiter = _fight_hero.find( player->GetKeyID() );

        for ( uint32 i = KFDataDefine::Array_Index; i < kfteamarray->Size(); ++i )
        {
            auto kfteam = kfteamarray->Find( i );
            if ( kfteam == nullptr )
            {
                continue;
            }

            auto uuid = kfteam->Get<uint64>();
            if ( heroiter != _fight_hero.end() )
            {
                if ( heroiter->second.find( uuid ) == heroiter->second.end() )
                {
                    // 不在出战英雄列表
                    continue;
                }
            }

            auto kfhero = _kf_hero->FindAliveHero( kfherorecord, uuid );
            if ( kfhero == nullptr )
            {
                continue;
            }

            _kf_hero->AddExp( player, kfhero, dropdata->GetValue() );
        }
    }

    __KF_DROP_LOGIC_FUNCTION__( KFExploreModule::OnDropHeroAddHp )
    {
        auto hp = dropdata->GetValue();
        ChangeTeamHeroHp( player, KFEnum::Add, hp );
        player->AddDataToShow( dropdata->_data_name, hp, false );
    }

    __KF_DROP_LOGIC_FUNCTION__( KFExploreModule::OnDropHeroDecHp )
    {
        auto hp = dropdata->GetValue();
        ChangeTeamHeroHp( player, KFEnum::Dec, hp );
        player->AddDataToShow( dropdata->_data_name, hp, false );
    }

    void KFExploreModule::ChangeTeamHeroHp( KFEntity* player, uint32 operate, uint32 value )
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

            auto kffighter = kfhero->Find( __STRING__( fighter ) );
            auto hp = kffighter->Get<uint32>( __STRING__( hp ) );
            if ( operate == KFEnum::Add )
            {
                hp += value;
                auto maxhp = kffighter->Get<uint32>( __STRING__( maxhp ) );
                if ( hp > maxhp )
                {
                    // 探索加hp不超过上限
                    hp = maxhp;
                }
            }
            else if ( operate == KFEnum::Dec )
            {
                if ( hp <= value )
                {
                    // 探索减hp最小为1
                    hp = 1u;
                }
                else
                {
                    hp -= value;
                }
            }

            player->UpdateData( kffighter, __STRING__( hp ), KFEnum::Set, hp );
        }
    }

    __KF_DROP_LOGIC_FUNCTION__( KFExploreModule::OnDropHeroAddBuff )
    {
        auto buffid = dropdata->GetValue();
        ChangeTeamHeroBuff( player, KFEnum::Add, buffid );
        player->AddDataToShow( dropdata->_data_name, buffid, false );
    }

    __KF_DROP_LOGIC_FUNCTION__( KFExploreModule::OnDropHeroDecBuff )
    {
        auto buffid = dropdata->GetValue();
        ChangeTeamHeroBuff( player, KFEnum::Dec, buffid );
        player->AddDataToShow( dropdata->_data_name, buffid, false );
    }

    void KFExploreModule::ChangeTeamHeroBuff( KFEntity* player, uint32 operate, uint32 value )
    {
        auto kfrecord = _explore_record.Find( player->GetKeyID() );
        if ( kfrecord == nullptr )
        {
            return;
        }

        KFMsg::MsgUpdateExploreBuffAck ack;

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

            ChangeHeroBuff( player, uuid, operate, value );

            auto buff = ack.add_bufflist();
            buff->set_uuid( uuid );
            buff->set_operate( operate );
            buff->set_buffid( value );
        }

        if ( ack.bufflist_size() > 0 )
        {
            _kf_player->SendToClient( player, KFMsg::MSG_UPDATE_EXPLORE_BUFF_ACK, &ack );
        }
    }

    void KFExploreModule::ChangeHeroBuff( KFEntity* player, uint64 uuid, uint32 operate, uint32 value )
    {
        auto kfrecord = _explore_record.Find( player->GetKeyID() );
        if ( kfrecord == nullptr )
        {
            return;
        }

        auto exploredata = kfrecord->_data.mutable_exploredata();
        auto buff = exploredata->mutable_buffdata()->mutable_buff();

        auto buffiter = buff->find( uuid );
        if ( buffiter == buff->end() )
        {
            // 该玩家没有buff数据
            if ( operate == KFEnum::Add )
            {
                // 增加该buff
                KFMsg::PBBuffData pbbuffdata;
                pbbuffdata.set_id( value );

                auto& herobuff = ( *buff )[uuid];
                auto bufflist = herobuff.add_bufflist();
                bufflist->CopyFrom( pbbuffdata );
            }
        }
        else
        {
            // 该玩家已有buff数据
            auto& bufflistdata = buffiter->second;
            if ( operate == KFEnum::Add )
            {
                // 增加该buff
                KFMsg::PBBuffData pbbuffdata;
                pbbuffdata.set_id( value );

                auto bufflist = bufflistdata.add_bufflist();
                bufflist->CopyFrom( pbbuffdata );
            }
            else if ( operate == KFEnum::Dec )
            {
                // 删除该buff
                auto buffsize = bufflistdata.bufflist_size();
                for ( int32 index = 0; index < buffsize; index++ )
                {
                    auto buff = bufflistdata.mutable_bufflist( index );
                    if ( buff->id() == value )
                    {
                        bufflistdata.mutable_bufflist()->DeleteSubrange( index, 1 );
                        break;
                    }
                }
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleUpdateExplorePlayerReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateExplorePlayerReq );

        auto kfrecord = _explore_record.Find( playerid );
        if ( kfrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ExploreNotInStatus );
        }

        auto exploredata = kfrecord->_data.mutable_exploredata();
        exploredata->set_save( true );

        auto playerdata = exploredata->mutable_playerdata();
        auto step = playerdata->step();
        playerdata->CopyFrom( kfmsg.playerdata() );
        playerdata->set_step( step + playerdata->step() );

        // 计算移动步数, 扣除粮食
        ExploreCostFood( player, playerdata );
    }

    void KFExploreModule::ExploreCostFood( KFEntity* player, KFMsg::PBExplorePlayerData* playerdata )
    {
        // 当前队伍活着的英雄数量
        auto teamheronum = _kf_hero_team->GetAliveHeroCount( player );
        if ( teamheronum == _invalid_int )
        {
            return;
        }

        auto kfsetting = KFFoodConsumeConfig::Instance()->FindSetting( teamheronum );
        if ( kfsetting == nullptr || kfsetting->_step_count == 0u )
        {
            return;
        }

        auto count = playerdata->step() / kfsetting->_step_count;
        if ( count == 0u )
        {
            return;
        }
        playerdata->set_step( playerdata->step() - count * kfsetting->_step_count );

        // 需要粮食数量
        auto neednum = kfsetting->_food_num * count;
        if ( neednum == 0u )
        {
            return;
        }

        // 当前背包拥有粮食数量
        static auto _option = _kf_option->FindOption( __STRING__( fooditemid ) );
        auto havenum = player->GetConfigValue( __STRING__( item ), _option->_uint32_value, neednum );

        static KFElements _item_element;
        if ( _item_element.IsEmpty() )
        {
            auto kfelementsetting = KFElementConfig::Instance()->FindElementSetting( __STRING__( item ) );
            auto strelement = __FORMAT__( kfelementsetting->_element_template, __STRING__( item ), 1, _option->_uint32_value );
            auto ok = _item_element.Parse( strelement, __FUNC_LINE__ );
            if ( !ok )
            {
                return;
            }
        }

        if ( neednum > havenum )
        {
            // 消耗血量
            auto hpnum = 0u;
            if ( havenum > 0u )
            {
                player->RemoveElement( &_item_element, __FUNC_LINE__, havenum );

                // 消耗粮食次数
                auto foodcount = ( havenum - 1u ) / kfsetting->_food_num + 1u;
                hpnum = ( count - foodcount ) * kfsetting->_hp_num;
            }
            else
            {
                hpnum = count * kfsetting->_hp_num;
            }

            auto kfherorecord = player->Find( __STRING__( hero ) );
            auto kfteamarray = player->Find( __STRING__( heroteam ) );
            for ( uint32 i = KFDataDefine::Array_Index; i < kfteamarray->Size(); ++i )
            {
                auto uuid = kfteamarray->Get<uint64>( i );
                auto kfhero = kfherorecord->Find( uuid );
                if ( kfhero == nullptr )
                {
                    continue;
                }

                auto kffighter = kfhero->Find( __STRING__( fighter ) );
                auto curhp = kffighter->Get( __STRING__( hp ) ) ;
                if ( curhp <= 1u )
                {
                    // 英雄1滴血不需要继续扣血
                    continue;
                }

                if ( curhp > hpnum )
                {
                    player->UpdateData( kffighter, __STRING__( hp ), KFEnum::Dec, hpnum );
                }
                else
                {
                    // 不够扣则扣到1血
                    player->UpdateData( kffighter, __STRING__( hp ), KFEnum::Set, 1u );
                }
            }
        }
        else
        {
            player->RemoveElement( &_item_element, __FUNC_LINE__, neednum );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HaneleUpdateExploreNpcReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateExploreNpcReq );
        auto kfrecord = _explore_record.Find( playerid );
        if ( kfrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ExploreNotInStatus );
        }

        auto pbnpcdata = kfmsg.npcdata();
        auto findnpcdata = kfrecord->FindNpcData( pbnpcdata.key() );
        findnpcdata->CopyFrom( pbnpcdata );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleUpdateFighterHeroReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateFighterHeroReq );

        auto kfherorecord = player->Find( __STRING__( hero ) );
        for ( auto i = 0; i < kfmsg.data_size(); ++i )
        {
            auto pbdata = &kfmsg.data( i );
            auto kfhero = _kf_hero->FindAliveHero( kfherorecord, pbdata->uuid() );
            if ( kfhero == nullptr )
            {
                continue;
            }

            // 更新hp
            auto kffighter = kfhero->Find( __STRING__( fighter ) );
            auto maxhp = kffighter->Get<uint32>( __STRING__( maxhp ) );
            auto curhp = pbdata->hp();
            if ( curhp > maxhp )
            {
                // 修改hp不能超过角色的maxhp
                curhp = maxhp;
            }
            auto hp = player->UpdateData( pbdata->uuid(), kffighter, __STRING__( hp ), KFEnum::Set, curhp );
            if ( hp == 0u )
            {
                continue;
            }

            player->UpdateData( pbdata->uuid(), kffighter, __STRING__( ep ), KFEnum::Set, pbdata->ep() );

            //  更新exp
            if ( pbdata->exp() > 0u )
            {
                _kf_hero->AddExp( player, kfhero, pbdata->exp() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleExploreDropReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExploreDropReq );

        VectorUInt32 droplist;

        for ( auto i = 0; i < kfmsg.droplist_size(); i++ )
        {
            auto dropid = kfmsg.droplist( i );
            droplist.push_back( dropid );
        }

        if ( droplist.size() > 0 )
        {
            _kf_drop->Drop( player, droplist, __STRING__( exploredrop ), __FUNC_LINE__ );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleKillNpcReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgKillNpcReq );

        auto kfrecord = _pve_record.Find( playerid );
        if ( kfrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ExploreNotInStatus );
        }

        auto kfnpc = kfrecord->_npcs.Find( kfmsg.npcuuid() );
        if ( kfnpc == nullptr )
        {
            return;
        }

        if ( kfmsg.herouuid() != 0u )
        {
            auto pbhero = kfrecord->FindHero( kfmsg.herouuid() );
            if ( pbhero != nullptr )
            {
                pbhero->add_killnpc( kfmsg.npcuuid() );
            }
        }
        else
        {
            kfrecord->_data.add_killnpc( kfmsg.npcuuid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleUpdateFaithReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateFaithReq );

        auto kfrecord = _explore_record.Find( player->GetKeyID() );
        if ( kfrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ExploreNotInStatus );
        }

        auto exploredata = kfrecord->_data.mutable_exploredata();
        exploredata->set_faith( kfmsg.faith() );
    }

    void KFExploreModule::StatisticsHeroKillNpcs( KFEntity* player, KFExploreRecord* kfrecord, uint32 pveresult )
    {
        // 英雄击杀
        for ( auto i = 0; i < kfrecord->_data.herodata_size(); ++i )
        {
            auto pbhero = &kfrecord->_data.herodata( i );
            for ( auto j = 0; j < pbhero->killnpc_size(); ++j )
            {
                auto npcid = pbhero->killnpc( j );
                StatisticsHeroKillNpc( player, kfrecord, pveresult, pbhero->uuid(), npcid );
            }
        }

        // 系统击杀
        for ( auto i = 0; i < kfrecord->_data.killnpc_size(); ++i )
        {
            auto npcid = kfrecord->_data.killnpc( i );
            StatisticsHeroKillNpc( player, kfrecord, pveresult, 0u, npcid );
        }
    }

    void KFExploreModule::StatisticsHeroKillNpc( KFEntity* player, KFExploreRecord* kfrecord, uint32 pveresult, uint64 herouuid, uint64 npcid )
    {
        auto kfnpc = kfrecord->_npcs.Move( npcid );
        if ( kfnpc == nullptr )
        {
            return;
        }

        if ( herouuid != 0u )
        {
            auto kfhero = player->Find( __STRING__( hero ), herouuid );
            if ( kfhero != nullptr )
            {
                kfnpc->Set( __STRING__( killrace ), kfhero->Get( __STRING__( race ) ) );
                kfnpc->Set( __STRING__( killprofession ), kfhero->Get( __STRING__( profession ) ) );
                kfnpc->Set( __STRING__( killweapontype ), kfhero->Get( __STRING__( weapontype ) ) );
                kfnpc->Set( __STRING__( killmovetype ), kfhero->Get( __STRING__( movetype ) ) );
            }
        }

        kfnpc->Set( __STRING__( pveresult ), pveresult );
        player->AddData( __STRING__( npc ), npcid, kfnpc );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleInteractItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgInteractItemReq );

        auto kfinteraction = player->Find( __STRING__( interaction ) );
        kfinteraction->Reset();

        kfinteraction->Set( __STRING__( field ), kfmsg.field() );
        player->UpdateData( kfinteraction, __STRING__( id ), KFEnum::Set, kfmsg.itemid() );
    }
}