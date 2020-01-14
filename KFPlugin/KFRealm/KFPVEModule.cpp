#include "KFPVEModule.hpp"

namespace KFrame
{
    void KFPVEModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_LEAVE_PLAYER__( &KFPVEModule::OnPlayerLeave );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_DROP_LOGIC__( __STRING__( exp ), &KFPVEModule::OnDropHeroExp );
        __REGISTER_DROP_LOGIC__( __STRING__( addhp ), &KFPVEModule::OnDropHeroAddHp );
        __REGISTER_DROP_LOGIC__( __STRING__( dechp ), &KFPVEModule::OnDropHeroDecHp );

        __REGISTER_EXECUTE__( __STRING__( pve ), &KFPVEModule::OnExecutePVEFighter );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_PVE_REQ, &KFPVEModule::HandlePVEReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_PVE_BALANCE_REQ, &KFPVEModule::HandlePVEBalanceReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_FIGHTER_HERO_REQ, &KFPVEModule::HandleUpdateFighterHeroReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_KILL_NPC_REQ, &KFPVEModule::HandleKillNpcReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_FIGHT_HERO_LIST_REQ, &KFPVEModule::HandleFightHeroListReq );

    }

    void KFPVEModule::BeforeShut()
    {
        __UN_LEAVE_PLAYER__();

        __UN_DROP_LOGIC__( __STRING__( exp ) );
        __UN_DROP_LOGIC__( __STRING__( addhp ) );
        __UN_DROP_LOGIC__( __STRING__( dechp ) );

        __UN_EXECUTE__( __STRING__( pve ) );

        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_PVE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_PVE_BALANCE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_FIGHTER_HERO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_KILL_NPC_REQ );
        __UN_MESSAGE__( KFMsg::MSG_FIGHT_HERO_LIST_REQ );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFPVEModule::OnPlayerLeave )
    {
        _pve_record.Remove( player->GetKeyID() );
    }

    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandlePVEReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPVEReq );

        auto realmid = player->Get<uint32>( __STRING__( realmid ) );
        if ( kfmsg.realmid() != 0u && realmid != kfmsg.realmid() )
        {
            return _kf_display->SendToClient( player, KFMsg::PVEMapIdError );
        }

        auto result = PVEFighter( player, kfmsg.pveid(), kfmsg.battleid(), kfmsg.modulename(), realmid );
        if ( result != KFMsg::Ok )
        {
            _kf_display->SendToClient( player, result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandlePVEBalanceReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPVEBalanceReq );
        auto result = PVEBalance( player, kfmsg.result(), kfmsg.truns() );
        if ( result != KFMsg::Ok )
        {
            _kf_display->SendToClient( player, result );
        }
    }

    __KF_DROP_LOGIC_FUNCTION__( KFPVEModule::OnDropHeroExp )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );

        auto kfblancerecord = _pve_record.Find( player->GetKeyID() );
        if ( kfblancerecord != nullptr )
        {
            for ( auto& iter : kfblancerecord->_fight_hero )
            {
                auto kfhero = _kf_hero->FindAliveHero( kfherorecord, iter );
                if ( kfhero == nullptr )
                {
                    continue;
                }

                _kf_hero->AddExp( player, kfhero, dropdata->GetValue() );
            }
        }
        else
        {
            for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
            {
                auto kfhero = _kf_hero->FindAliveHero( kfherorecord, kfteam->Get<uint64>() );
                if ( kfhero == nullptr )
                {
                    continue;
                }

                _kf_hero->AddExp( player, kfhero, dropdata->GetValue() );
            }
        }
    }

    __KF_DROP_LOGIC_FUNCTION__( KFPVEModule::OnDropHeroAddHp )
    {
        auto hp = dropdata->GetValue();
        ChangeTeamHeroHp( player, KFEnum::Add, hp );
        player->AddDataToShow( dropdata->_data_name, hp, false );
    }

    __KF_DROP_LOGIC_FUNCTION__( KFPVEModule::OnDropHeroDecHp )
    {
        auto hp = dropdata->GetValue();
        ChangeTeamHeroHp( player, KFEnum::Dec, hp );
        player->AddDataToShow( dropdata->_data_name, hp, false );
    }

    __KF_EXECUTE_FUNCTION__( KFPVEModule::OnExecutePVEFighter )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "pve execute param size<2" );
            return false;
        }

        auto pveid = executedata->_param_list._params[0]->_int_value;
        auto battleid = executedata->_param_list._params[1]->_int_value;
        if ( pveid == 0u || battleid == 0u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "pve execute param pveid=[{}] battleid=[{}]", pveid, battleid );
            return false;
        }

        auto result = PVEFighter( player, pveid, battleid, modulename, moduleid );
        if ( result != KFMsg::Ok )
        {
            _kf_display->SendToClient( player, result );
        }

        return result == KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandleUpdateFighterHeroReq )
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
            auto curhp = __MIN__( pbdata->hp(), maxhp );
            auto kfhp = kffighter->Find( __STRING__( hp ) );
            if ( curhp != kfhp->Get<uint32>() )
            {
                auto hp = player->UpdateData( pbdata->uuid(), kfhp, KFEnum::Set, curhp );
                if ( hp == 0u )
                {
                    continue;
                }
            }

            // 更新ep
            auto kfep = kffighter->Find( __STRING__( ep ) );
            if ( pbdata->ep() != kfep->Get<uint32>() )
            {
                player->UpdateData( pbdata->uuid(), kfep, KFEnum::Set, pbdata->ep() );
            }

            //  更新exp
            if ( pbdata->exp() > 0u )
            {
                _kf_hero->AddExp( player, kfhero, pbdata->exp() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandleKillNpcReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgKillNpcReq );

        auto kfrecord = _pve_record.Find( playerid );
        if ( kfrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::PVENotInStatus );
        }

        auto kfnpc = player->Find( __STRING__( npc ), kfmsg.npcuuid() );
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

    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandleFightHeroListReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgFightHeroListReq );

        auto kfrecord = _pve_record.Find( playerid );
        if ( kfrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::PVENotInStatus );
        }

        if ( !kfrecord->_fight_hero.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::PVEHeroTeamExist );
        }

        auto kfherorecord = player->Find( __STRING__( hero ) );
        for ( auto i = 0; i < kfmsg.herolist_size(); i++ )
        {
            auto herouuid = kfmsg.herolist( i );

            auto kfhero = kfherorecord->Find( herouuid );
            if ( kfhero == nullptr ||
                    kfhero->Get( __STRING__( durability ) ) == 0u ||
                    kfhero->Get( __STRING__( fighter ), __STRING__( hp ) ) == 0u )
            {
                __LOG_ERROR__( "player=[{}] herouuid=[{}] fighthero is invalid", player->GetKeyID(), herouuid );
                continue;
            }

            auto posflag = kfhero->Get<uint32>( __STRING__( posflag ) );
            if ( posflag != KFMsg::HeroTeam )
            {
                __LOG_ERROR__( "player=[{}] herouuid=[{}] fighthero not in heroteam", player->GetKeyID(), herouuid );
                continue;
            }

            // 加入出战列表
            kfrecord->_fight_hero.emplace( herouuid );
        }

        // 扣除指定耐久度
        _kf_hero_team->DecTeamHeroDurability( player, kfrecord->_fight_hero );
    }

    uint32 KFPVEModule::PVEFighter( KFEntity* player, uint32 pveid, uint32 battleid, const std::string& modulename, uint64 moduleid )
    {
        auto status = player->GetStatus();
        if ( status == KFMsg::PVEStatus )
        {
            return KFMsg::PVEAlready;
        }

        auto count = _kf_hero_team->GetAliveHeroCount( player );
        if ( count == 0u )
        {
            return KFMsg::PVEHeroTeamEmpty;
        }

        auto kfsetting = KFPVEConfig::Instance()->FindSetting( pveid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::PVEIdError;
        }

        if ( !kfsetting->_consume.IsEmpty() )
        {
            auto dataname = player->CheckRemoveElement( &kfsetting->_consume, __FUNC_LINE__ );
            if ( !dataname.empty() )
            {
                return KFMsg::DataNotEnough;
            }

            player->RemoveElement( &kfsetting->_consume, __STRING__( pve ), __FUNC_LINE__ );
        }

        // 初始化纪录
        auto kfrecord = _pve_record.Create( player->GetKeyID() );
        kfrecord->Reset();

        kfrecord->BalanceHeroBeginData( player );
        kfrecord->BalanceCurrencyBeginData( player );

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

        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto uuid = kfteam->Get<uint64>();
            auto kfhero = _kf_hero->FindAliveHero( kfherorecord, uuid );
            if ( kfhero == nullptr )
            {
                continue;
            }

            auto pbhero = _kf_kernel->SerializeToClient( kfhero );
            ( *ack.mutable_hero() )[ uuid ].CopyFrom( *pbhero );
        }

        // npc
        auto kfnpcrecord = player->Find( __STRING__( npc ) );
        player->CleanData( kfnpcrecord, false );

        auto npclevel = 1;
        for ( auto generateid : kfsetting->_npc_generate_list )
        {
            auto kfnpc = _kf_generate->GenerateNpcHero( player, kfnpcrecord, generateid, npclevel );
            if ( kfnpc == nullptr )
            {
                continue;
            }

            // 添加到纪录中
            kfnpcrecord->Add( kfnpc->GetKeyID(), kfnpc );

            auto pbhero = _kf_kernel->Serialize( kfnpc );
            ( *ack.mutable_npc() )[ kfnpc->GetKeyID() ].CopyFrom( *pbhero );
        }

        // buff (探索特有)
        auto realmdata = _kf_realm->GetRealmData( player->GetKeyID() );
        if ( realmdata != nullptr )
        {
            // 信仰值
            ack.set_faith( realmdata->_data.faith() );
            ack.mutable_buffdata()->CopyFrom( realmdata->_data.buffdata() );
        }

        _kf_player->SendToClient( player, KFMsg::MSG_PVE_ACK, &ack, 10u );

        player->SetStatus( KFMsg::PVEStatus );
        player->UpdateData( __STRING__( pveid ), KFEnum::Set, pveid );
        return KFMsg::Ok;
    }

    uint32 KFPVEModule::PVEBalance( KFEntity* player, uint32 result, uint32 truns )
    {
        auto kfrecord = _pve_record.Find( player->GetKeyID() );
        if ( kfrecord == nullptr )
        {
            return KFMsg::PVENotInStatus;
        }

        // 先计算击杀npc统计
        StatisticsHeroKillNpcs( player, kfrecord, result );

        auto teamheronum = _kf_hero_team->GetHeroCount( player );
        auto teamdeathnum = _kf_hero_team->GetDeadHeroCount( player );
        __LOG_DEBUG__( "player=[{}] pve balance result=[{}]", player->GetKeyID(), result );

        player->SetStatus( kfrecord->_data.status() );

        // 更新死亡英雄(死亡英雄可获得结算经验)
        _kf_hero_team->UpdateTeamDeadHero( player );

        // 先结算货币
        kfrecord->BalanceCurrencyEndData( player );

        auto kfsetting = KFPVEConfig::Instance()->FindSetting( kfrecord->_data.id() );
        if ( kfsetting != nullptr )
        {
            // 把原来的显示同步到客户端
            player->ShowElementToClient();
            switch ( result )
            {
            case KFMsg::Victory:
                _kf_drop->Drop( player, kfsetting->_victory_drop_list, __STRING__( pve ), __FUNC_LINE__ );
                break;
            case KFMsg::Failed:
                _kf_drop->Drop( player, kfsetting->_fail_drop_list, __STRING__( pve ), __FUNC_LINE__ );
                break;
            }
            kfrecord->BalanceAddDropData( player );
        }

        // 结算最终数据
        kfrecord->BalanceHeroEndData( player );

        // 发送给客户端
        KFMsg::MsgPVEBalanceAck ack;
        ack.set_result( result );
        ack.set_moduleid( kfrecord->_data.moduleid() );
        ack.set_modulename( kfrecord->_data.modulename() );
        kfrecord->BalanceRealmRecord( ack.mutable_balance(), KFMsg::PVEStatus );
        _kf_player->SendToClient( player, KFMsg::MSG_PVE_BALANCE_ACK, &ack, 10u );

        // 设置回调属性
        if ( result != KFMsg::Flee )
        {
            auto kfbalance = player->Find( __STRING__( balance ) );
            kfbalance->Set( __STRING__( id ), kfrecord->_data.id() );

            player->UpdateData( kfbalance, __STRING__( team ), KFEnum::Set, teamheronum );
            player->UpdateData( kfbalance, __STRING__( death ), KFEnum::Set, teamdeathnum );
            player->UpdateData( kfbalance, __STRING__( truns ), KFEnum::Set, truns );
            player->UpdateData( kfbalance, __STRING__( pveresult ), KFEnum::Set, result );

            if ( result == KFMsg::Victory )
            {
                player->UpdateData( __STRING__( pvevictory ), KFEnum::Add, 1u );
            }
        }

        // 删除纪录
        _pve_record.Remove( player->GetKeyID() );

        // 设置pveid为0
        player->Set( __STRING__( pveid ), 0u );
        player->CleanData( __STRING__( npc ), false );

        auto kfrealmdata = _kf_realm->GetRealmData( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            // 不在探索里面 战斗后就移除寿命不足
            _kf_hero_team->RemoveTeamHeroDurability( player );

            // 清空队伍英雄ep
            _kf_hero_team->ClearTeamHeroEp( player );
        }

        return KFMsg::Ok;
    }

    void KFPVEModule::StatisticsHeroKillNpcs( KFEntity* player, KFRealmData* kfrecord, uint32 pveresult )
    {
        auto kfnpcrecord = player->Find( __STRING__( npc ) );

        // 英雄击杀
        for ( auto i = 0; i < kfrecord->_data.herodata_size(); ++i )
        {
            auto pbhero = &kfrecord->_data.herodata( i );
            for ( auto j = 0; j < pbhero->killnpc_size(); ++j )
            {
                auto npcid = pbhero->killnpc( j );
                StatisticsHeroKillNpc( player, kfnpcrecord, kfrecord, pveresult, pbhero->uuid(), npcid );
            }
        }

        // 系统击杀
        for ( auto i = 0; i < kfrecord->_data.killnpc_size(); ++i )
        {
            auto npcid = kfrecord->_data.killnpc( i );
            StatisticsHeroKillNpc( player, kfnpcrecord, kfrecord, pveresult, 0u, npcid );
        }

        // 清空剩余的npc
        player->CleanData( kfnpcrecord, false );
    }

    void KFPVEModule::StatisticsHeroKillNpc( KFEntity* player, KFData* kfnpcrecord, KFRealmData* kfrecord, uint32 pveresult, uint64 herouuid, uint64 npcid )
    {
        auto kfnpc = kfnpcrecord->Find( npcid );
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
        player->RemoveData( kfnpcrecord, npcid );
    }

    void KFPVEModule::ChangeTeamHeroHp( KFEntity* player, uint32 operate, uint32 value )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto kfhero = _kf_hero->FindAliveHero( kfherorecord, kfteam->Get<uint64>() );
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

}