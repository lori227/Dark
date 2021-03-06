﻿#include "KFPVEModule.hpp"
#include "KFRealmModule.hpp"

namespace KFrame
{
    void KFPVEModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFPVEModule::OnPlayerPVEEnter );
        __REGISTER_LEAVE_PLAYER__( &KFPVEModule::OnPlayerPVELeave );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_DROP_LOGIC__( __STRING__( exp ), &KFPVEModule::OnDropHeroExp );
        __REGISTER_EXECUTE__( __STRING__( pve ), &KFPVEModule::OnExecutePVEFighter );
        __REGISTER_EXECUTE__( __STRING__( addfaith ), &KFPVEModule::OnExecuteAddFaith );
        __REGISTER_EXECUTE__( __STRING__( decfaith ), &KFPVEModule::OnExecuteDecFaith );
        __REGISTER_ELEMENT_RESULT__( __STRING__( pvedrop ), &KFPVEModule::OnPVEDropElementResult );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_NPC_GROUP_DATA_REQ, &KFPVEModule::HandleNpcGroupDataReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_PVE_REQ, &KFPVEModule::HandlePVEReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_PVE_BALANCE_REQ, &KFPVEModule::HandlePVEBalanceReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_FIGHTER_HERO_REQ, &KFPVEModule::HandleUpdateFighterHeroReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_KILL_NPC_REQ, &KFPVEModule::HandleKillNpcReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_FIGHT_HERO_LIST_REQ, &KFPVEModule::HandleFightHeroListReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_FAITH_REQ, &KFPVEModule::HandleUpdateFaithReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_PVE_TURN_START_REQ, &KFPVEModule::HandleTurnStartReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_PVE_TURN_FINISH_REQ, &KFPVEModule::HandleTurnFinishReq );
    }

    void KFPVEModule::BeforeShut()
    {
        __UN_LEAVE_PLAYER__();
        __UN_DROP_LOGIC__( __STRING__( exp ) );
        __UN_EXECUTE__( __STRING__( pve ) );
        __UN_EXECUTE__( __STRING__( addfaith ) );
        __UN_EXECUTE__( __STRING__( decfaith ) );
        __UN_ELEMENT_RESULT__( __STRING__( pvedrop ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_NPC_GROUP_DATA_REQ );
        __UN_MESSAGE__( KFMsg::MSG_PVE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_PVE_BALANCE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_FIGHTER_HERO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_KILL_NPC_REQ );
        __UN_MESSAGE__( KFMsg::MSG_FIGHT_HERO_LIST_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_FAITH_REQ );
        __UN_MESSAGE__( KFMsg::MSG_PVE_TURN_START_REQ );
        __UN_MESSAGE__( KFMsg::MSG_PVE_TURN_FINISH_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPVEModule::BindPVEStartFunction( const std::string& module, KFPVEStartFunction& function )
    {
        auto kffunction = _pve_start_function.Create( module );
        kffunction->_function = function;
    }

    void KFPVEModule::UnBindPVEStartFunction( const std::string& module )
    {
        _pve_start_function.Remove( module );
    }

    void KFPVEModule::BindPVEFinishFunction( const std::string& module, KFPVEFinishFunction& function )
    {
        auto kffunction = _pve_finish_function.Create( module );
        kffunction->_function = function;
    }

    void KFPVEModule::UnBindPVEFinishFunction( const std::string& module )
    {
        _pve_finish_function.Remove( module );
    }

    void KFPVEModule::BindTurnStartFunction( const std::string& module, KFTurnStartFunction& function )
    {
        auto kffunction = _turn_start_function.Create( module );
        kffunction->_function = function;
    }

    void KFPVEModule::UnBindTurnStartFunction( const std::string& module )
    {
        _turn_start_function.Remove( module );
    }

    void KFPVEModule::BindTurnFinishFunction( const std::string& module, KFTurnFinishFunction& function )
    {
        auto kffunction = _turn_finish_function.Create( module );
        kffunction->_function = function;
    }

    void KFPVEModule::UnBindTurnFinishFunction( const std::string& module )
    {
        _turn_finish_function.Remove( module );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFRealmData* KFPVEModule::GetPVEData( KFEntity* player )
    {
        return _pve_record.Find( player->GetKeyID() );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFPVEModule::OnPlayerPVEEnter )
    {
        auto strdata = player->Get<std::string>( __STRING__( pvedata ) );
        if ( !strdata.empty() )
        {
            player->Set<std::string>( __STRING__( pvedata ), _invalid_string );

            auto kfpvedata = _pve_record.Create( player->GetKeyID() );
            kfpvedata->_data.ParseFromString( strdata );
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFPVEModule::OnPlayerPVELeave )
    {
        PVEBalance( player, KFMsg::Flee, 0u );

        // 如果是不允许逃跑的, 保存纪录
        auto kfpvedata = _pve_record.Find( player->GetKeyID() );
        if ( kfpvedata != nullptr )
        {
            auto strdata = kfpvedata->_data.SerializeAsString();
            player->Set( __STRING__( pvedata ), strdata );

            _pve_record.Remove( player->GetKeyID() );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_EXECUTE_FUNCTION__( KFPVEModule::OnExecutePVEFighter )
    {
        if ( executedata->_param_list._params.size() < 3u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "pve execute param size<3" );
            return false;
        }

        auto pveid = executedata->_param_list._params[ 0 ]->_int_value;
        auto npcgroupid = executedata->_param_list._params[ 1 ]->_int_value;
        auto spwanrule = executedata->_param_list._params[ 2 ]->_int_value;
        auto result = PVEFighter( player, pveid, npcgroupid, spwanrule, 0u, modulename, moduleid );
        if ( result != KFMsg::Ok )
        {
            _kf_display->SendToClient( player, result );
        }

        return result == KFMsg::Ok;
    }


    __KF_EXECUTE_FUNCTION__( KFPVEModule::OnExecuteAddFaith )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "addfaith execute param size<1" );
            return false;
        }

        auto faith = executedata->_param_list._params[0]->_int_value;
        OperateFaith( player, KFEnum::Add, faith );

        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFPVEModule::OnExecuteDecFaith )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "decfaith execute param size<1" );
            return false;
        }

        auto faith = executedata->_param_list._params[0]->_int_value;
        OperateFaith( player, KFEnum::Dec, faith );

        return true;
    }

    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandlePVEReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPVEReq );

        auto npcgrouid = 0u;
        auto spawnrule = 0u;
        auto dungeonid = 0u;
        std::tie( npcgrouid, spawnrule, dungeonid ) = FindRealmNpcData( player, kfmsg.pveid(), kfmsg.npckey() );
        auto result = PVEFighter( player, kfmsg.pveid(), npcgrouid, spawnrule, dungeonid, kfmsg.modulename(), kfmsg.pveid() );
        if ( result != KFMsg::Ok )
        {
            _kf_display->SendToClient( player, result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandleNpcGroupDataReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgNpcGroupDataReq );

        // 不在探索里
        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInExplore );
        }

        auto pbexplore = kfrealmdata->FindExeploreData( kfrealmdata->_data.level() );
        auto pbnpcdatas = pbexplore->mutable_npcdata();
        auto& pbnpcdata = ( *pbnpcdatas )[ kfmsg.npckey() ];
        pbnpcdata.set_key( kfmsg.npckey() );

        for ( auto i = 0; i < kfmsg.pveid_size(); ++i )
        {
            auto pveid = kfmsg.pveid( i );
            auto& pbnpcgroupdata = ( *pbnpcdata.mutable_npcgroupdata() )[ pveid ];

            auto kfpvesetting = KFPVEConfig::Instance()->FindSetting( pveid );
            if ( kfpvesetting == nullptr )
            {
                continue;
            }

            auto kfweight = kfpvesetting->_npc_rand_setting->_npc_weight_list.Rand();
            if ( kfweight == nullptr )
            {
                continue;
            }

            pbnpcgroupdata.set_npcgroupid( kfweight->_id );
            pbnpcgroupdata.set_spawnrule( kfweight->_spawn_rule );
            pbnpcgroupdata.set_dungeonid( kfpvesetting->RandDungeonId() );

            auto kfgroupsetting = KFNpcGroupConfig::Instance()->FindSetting( kfweight->_id );
            if ( kfgroupsetting != nullptr )
            {
                pbnpcgroupdata.set_spawnmodel( kfgroupsetting->_show_model );
            }
        }

        // 发送给客户端
        KFMsg::MsgNpcGroupDataAck ack;
        ack.mutable_npcdata()->CopyFrom( pbnpcdata );
        _kf_player->SendToClient( player, KFMsg::MSG_NPC_GROUP_DATA_ACK, &ack );
    }

    std::tuple<uint32, uint32, uint32> KFPVEModule::FindRealmNpcData( KFEntity* player, uint32 pveid, const std::string& npckey )
    {
        // 不在探索里
        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return std::make_tuple( 0u, 0u, 0u );
        }

        // 查找npc信息
        auto pbexplore = kfrealmdata->FindExeploreData( kfrealmdata->_data.level() );
        auto pbnpcdata = &pbexplore->npcdata();
        auto npciter = pbnpcdata->find( npckey );
        if ( npciter == pbnpcdata->end() )
        {
            return std::make_tuple( 0u, 0u, 0u );
        }

        // 查找npc组数据
        auto pbnpcgroupdata = &npciter->second.npcgroupdata();
        auto groupiter = pbnpcgroupdata->find( pveid );
        if ( groupiter == pbnpcgroupdata->end() )
        {
            return std::make_tuple( 0u, 0u, 0u );
        }

        auto pbgroupdata = &groupiter->second;
        return std::make_tuple( pbgroupdata->npcgroupid(), pbgroupdata->spawnrule(), pbgroupdata->dungeonid() );
    }

    uint32 KFPVEModule::PVEFighter( KFEntity* player, uint32 pveid, uint32 npcgrouid, uint32 spawnrule, uint32 dungeonid, const std::string& modulename, uint64 moduleid )
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
            __LOG_ERROR__( "pveid=[{}] can't find setting", pveid );
            return KFMsg::PVEIdError;
        }

        if ( kfsetting->_npc_rand_setting == nullptr )
        {
            return KFMsg::NpcRandError;
        }

        // 随机敌人组
        auto kfnpcgroupsetting = RandNpcGroupSetting( kfsetting, npcgrouid, spawnrule );
        if ( kfnpcgroupsetting == nullptr )
        {
            return KFMsg::NpcGroupError;
        }

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            // 不在探索中的战斗需要消耗耐久度
            if ( !_kf_hero_team->IsDurabilityEnough( player ) )
            {
                return KFMsg::HeroTeamDurabilityNotEnough;
            }
        }

        if ( dungeonid == 0u )
        {
            dungeonid = kfsetting->RandDungeonId();
        }

        // 扣除费用
        auto& dataname = player->RemoveElement( &kfsetting->_consume, _default_multiple, __STRING__( pve ), pveid, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return KFMsg::DataNotEnough;
        }

        // 初始化纪录
        auto kfpvedata = _pve_record.Create( player->GetKeyID() );
        kfpvedata->Reset();

        kfpvedata->_data.set_status( status );
        kfpvedata->_data.set_id( pveid );
        kfpvedata->_data.set_moduleid( moduleid );
        kfpvedata->_data.set_modulename( modulename );

        kfpvedata->RecordBeginHeros( player );

        /////////////////////////////////////////////////////////////////////////////////////////////
        KFMsg::MsgPVEAck ack;
        ack.set_pveid( pveid );
        ack.set_spawnrule( spawnrule );
        ack.set_dungeonid( dungeonid );
        ack.set_npcgroupid( kfnpcgroupsetting->_id );

        // team
        auto herocount = 0u;
        auto herolevel = 0.0f;
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

            ++herocount;
            herolevel += kfhero->Get<uint32>( __STRING__( level ) );
        }
        if ( herocount != 0u )
        {
            herolevel = herolevel / static_cast< double >( herocount );
        }

        // npc
        auto npccount = 0u;
        auto npclevel = 0.0f;
        auto kfnpcrecord = player->Find( __STRING__( npc ) );
        player->CleanData( kfnpcrecord, false );
        for ( auto generateid : kfnpcgroupsetting->_npc_generate_list )
        {
            auto kfnpc = _kf_generate->GenerateNpcHero( player, kfnpcrecord, generateid, 1u );
            if ( kfnpc == nullptr )
            {
                continue;
            }

            // 添加到纪录中
            kfnpcrecord->Add( kfnpc->GetKeyID(), kfnpc );

            auto pbhero = _kf_kernel->Serialize( kfnpc );
            ( *ack.mutable_npc() )[ kfnpc->GetKeyID() ].CopyFrom( *pbhero );

            ++npccount;
            npclevel += kfnpc->Get<uint32>( __STRING__( level ) );
        }
        if ( npccount != 0u )
        {
            npclevel = npclevel / static_cast< double >( npccount );
        }

        // buff (探索特有)
        if ( kfrealmdata != nullptr )
        {
            // 信仰值
            ack.set_realmid( kfrealmdata->_data.id() );
            ack.set_faith( kfrealmdata->_data.faith() );
            ack.set_worldflag( kfrealmdata->_data.innerworld() );
            ack.mutable_buffdata()->CopyFrom( kfrealmdata->_data.buffdata() );
        }

        // 逃跑成功率
        auto fleerate = CalcFleeSuccessRate( player, kfsetting, herolevel, npclevel, kfrealmdata != nullptr );
        ack.set_fleerate( fleerate );
        kfpvedata->_data.set_fleerate( fleerate );

        _kf_player->SendToClient( player, KFMsg::MSG_PVE_ACK, &ack );
        /////////////////////////////////////////////////////////////////////////////////////////////
        player->SetStatus( KFMsg::PVEStatus );
        player->UpdateData( __STRING__( pveid ), KFEnum::Set, pveid );

        // 开始战斗回调
        for ( auto& iter : _pve_start_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player, kfrealmdata, kfpvedata );
        }

        return KFMsg::Ok;
    }

    const KFNpcGroupSetting* KFPVEModule::RandNpcGroupSetting( const KFPVESetting* kfpvesetting, uint32 npcgroupid, uint32& spawnrule )
    {
        if ( npcgroupid == 0u )
        {
            auto kfweight = kfpvesetting->_npc_rand_setting->_npc_weight_list.Rand();
            if ( kfweight == nullptr )
            {
                __LOG_ERROR__( "pve=[{}] npcrand=[{}] npclist empty", kfpvesetting->_id, kfpvesetting->_npc_rand_id );
                return nullptr;
            }

            npcgroupid = kfweight->_id;
            spawnrule = kfweight->_spawn_rule;
        }

        auto kfgroupsetting = KFNpcGroupConfig::Instance()->FindSetting( npcgroupid );
        if ( kfgroupsetting == nullptr )
        {
            __LOG_ERROR__( "pve=[{}] npcgroup=[{}] can't find setting", kfpvesetting->_id, npcgroupid );
        }

        return kfgroupsetting;
    }

    uint32 KFPVEModule::CalcFleeSuccessRate( KFEntity* player, const KFPVESetting* kfsetting, double herolevel, double npclevel, bool isinrealm )
    {
        auto fleerate = kfsetting->_normal_flee_rate;
        if ( IsInnerWorld( player ) )
        {
            fleerate = kfsetting->_inner_flee_rate;
        }
        if ( fleerate == 0u )
        {
            return 0u;
        }

        // 在探索中计算逃跑率
        if ( isinrealm )
        {
            fleerate += ( herolevel - npclevel ) * kfsetting->_level_coefficient;

            // 团长逃跑成功率属性
            fleerate += player->Get<uint32>( __STRING__( flee ) );
        }
        else
        {
            fleerate = KFRandEnum::TenThousand;
        }

        return fleerate;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandlePVEBalanceReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPVEBalanceReq );
        auto result = PVEBalance( player, kfmsg.result(), kfmsg.truns() );
        if ( result != KFMsg::Ok )
        {
            _kf_display->SendToClient( player, result );
        }
    }

    uint32 KFPVEModule::PVEBalance( KFEntity* player, uint32 result, uint32 truns )
    {
        auto kfpvedata = _pve_record.Find( player->GetKeyID() );
        if ( kfpvedata == nullptr )
        {
            return KFMsg::PVENotInStatus;
        }
        player->SetStatus( kfpvedata->_data.status() );

        uint32 errorcode = KFMsg::Error;
        switch ( result )
        {
        case KFMsg::Victory:
            errorcode = PVEBalanceVictory( player, kfpvedata, truns );
            break;
        case KFMsg::Failed:
            errorcode = PVEBalanceFailed( player, kfpvedata, truns );
            break;
        case KFMsg::Flee:
            errorcode = PVEBalanceFlee( player, kfpvedata, truns );
            break;
        case KFMsg::Ace:
            errorcode = PVEBalanceAce( player, kfpvedata, truns );
            break;
        default:
            break;
        }

        if ( errorcode == KFMsg::Ok )
        {
            // 清除战斗数据
            PVEBalanceClearData( player );

            // 结束战斗回调
            auto kfrealmdata = _kf_realm->GetRealmData( player );
            for ( auto& iter : _pve_finish_function._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( player, kfrealmdata, kfpvedata, result, truns );
            }
        }

        return errorcode;
    }

    void KFPVEModule::PVEBalanceResultCondition( KFEntity* player, KFRealmData* kfpvedata, uint32 result, uint32 truns )
    {
        // 先计算击杀npc统计
        StatisticsHeroKillNpcs( player, kfpvedata, result );

        // 设置条件更新属性
        auto kfbalance = player->Find( __STRING__( balance ) );
        kfbalance->Set( __STRING__( id ), kfpvedata->_data.id() );

        auto teamheronum = _kf_hero_team->GetHeroCount( player );
        player->UpdateData( kfbalance, __STRING__( team ), KFEnum::Set, teamheronum );

        auto teamdeathnum = _kf_hero_team->GetDeadHeroCount( player );
        player->UpdateData( kfbalance, __STRING__( death ), KFEnum::Set, teamdeathnum );

        player->UpdateData( kfbalance, __STRING__( truns ), KFEnum::Set, truns );
        player->UpdateData( kfbalance, __STRING__( pveresult ), KFEnum::Set, result );
    }

    void KFPVEModule::PVEBalanceDrop( KFEntity* player, KFRealmData* kfpvedata, uint32 result )
    {
        auto kfsetting = KFPVEConfig::Instance()->FindSetting( kfpvedata->_data.id() );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 结算胜利掉落
        auto& droplist = PVEGetDropList( player, kfsetting, result );
        if ( !droplist.empty() )
        {
            _kf_drop->Drop( player, droplist, __STRING__( pvedrop ), kfpvedata->_data.id(), __FUNC_LINE__ );
        }
    }

    __KF_ELEMENT_RESULT_FUNCTION__( KFPVEModule::OnPVEDropElementResult )
    {
        auto kfpvedata = _pve_record.Find( player->GetKeyID() );
        if ( kfpvedata != nullptr )
        {
            kfpvedata->BalanceDropData( kfresult );
        }

        return false;
    }

    const UInt32Vector& KFPVEModule::PVEGetDropList( KFEntity* player, const KFPVESetting* kfsetting, uint32 result )
    {
        if ( IsInnerWorld( player ) )
        {
            if ( result == KFMsg::Victory )
            {
                return kfsetting->_inner_victory_drop_list;
            }

            return kfsetting->_inner_fail_drop_list;
        }

        if ( result == KFMsg::Victory )
        {
            return kfsetting->_victory_drop_list;
        }

        return kfsetting->_fail_drop_list;
    }

    void KFPVEModule::PVEBalanceClearData( KFEntity* player )
    {
        // 设置pveid为0
        player->Set( __STRING__( pveid ), 0u );

        // 删除纪录
        _pve_record.Remove( player->GetKeyID() );
    }

    void KFPVEModule::SendPVEBalanceToClient( KFEntity* player, KFRealmData* kfpvedata, uint32 result )
    {
        KFMsg::MsgPVEBalanceAck ack;
        ack.set_result( result );
        ack.set_moduleid( kfpvedata->_data.moduleid() );
        ack.set_modulename( kfpvedata->_data.modulename() );
        ack.set_realmid( player->Get<uint32>( __STRING__( realmid ) ) );
        kfpvedata->BalanceRealmRecord( ack.mutable_balance() );
        _kf_player->SendToClient( player, KFMsg::MSG_PVE_BALANCE_ACK, &ack );
    }

    void KFPVEModule::PVEBalanceRecord( KFEntity* player, KFRealmData* kfpvedata, uint32 result )
    {
        kfpvedata->RecordPeriodHeros( player );

        if ( _kf_realm->GetRealmData( player ) == nullptr )
        {
            // 更新死亡英雄(死亡英雄可获得结算经验)
            _kf_hero_team->UpdateDeadHero( player );
        }

        // 战斗结算
        PVEBalanceDrop( player, kfpvedata, result );

        kfpvedata->RecordEndHeros( player );
    }

    uint32 KFPVEModule::PVEBalanceVictory( KFEntity* player, KFRealmData* kfpvedata, uint32 truns )
    {
        // 胜场增加
        player->UpdateData( __STRING__( pvevictory ), KFEnum::Add, 1u );

        // 战斗纪录
        PVEBalanceRecord( player, kfpvedata, KFMsg::Victory );

        // 发送给客户端
        SendPVEBalanceToClient( player, kfpvedata, KFMsg::Victory );

        // 战斗结果条件结算
        PVEBalanceResultCondition( player, kfpvedata, KFMsg::Victory, truns );
        return KFMsg::Ok;
    }

    uint32 KFPVEModule::PVEBalanceFailed( KFEntity* player, KFRealmData* kfpvedata, uint32 truns )
    {
        // 战斗纪录
        PVEBalanceRecord( player, kfpvedata, KFMsg::Failed );

        // 发送给客户端
        SendPVEBalanceToClient( player, kfpvedata, KFMsg::Failed );

        // 战斗结果条件结算
        PVEBalanceResultCondition( player, kfpvedata, KFMsg::Failed, truns );
        return KFMsg::Ok;
    }

    uint32 KFPVEModule::PVEBalanceAce( KFEntity* player, KFRealmData* kfpvedata, uint32 truns )
    {
        // 战斗纪录
        PVEBalanceRecord( player, kfpvedata, KFMsg::Ace );

        // 发送给客户端
        SendPVEBalanceToClient( player, kfpvedata, KFMsg::Ace );

        // 战斗结果条件结算
        PVEBalanceResultCondition( player, kfpvedata, KFMsg::Ace, truns );
        return KFMsg::Ok;
    }

    uint32 KFPVEModule::PVEBalanceFlee( KFEntity* player, KFRealmData* kfpvedata, uint32 truns )
    {
        if ( kfpvedata->_data.fleerate() == 0u )
        {
            return KFMsg::PVECanNotFlee;
        }

        if ( _kf_realm->GetRealmData( player ) == nullptr )
        {
            // 更新死亡英雄(死亡英雄可获得结算经验)
            _kf_hero_team->UpdateDeadHero( player );
        }

        // 计算逃跑惩罚
        auto& punishlist = PVEFleePunish( player, kfpvedata );

        KFMsg::MsgPVEFleePunishAck ack;
        ack.set_success( punishlist.empty() );
        for ( auto punishdata : punishlist )
        {
            auto pbdata = ack.add_punishdata();
            pbdata->set_name( punishdata._name );
            pbdata->set_id( punishdata._key );
            pbdata->set_value( punishdata._value );
        }
        _kf_player->SendToClient( player, KFMsg::MSG_PVE_FLEE_PUNISH_ACK, &ack );

        return KFMsg::Ok;
    }

    PunishDataList& KFPVEModule::PVEFleePunish( KFEntity* player, KFRealmData* kfpvedata )
    {
        static PunishDataList _punish_list;
        _punish_list.clear();

        auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
        if ( rand >= kfpvedata->_data.fleerate() )
        {
            PVEFleePunishData( player, kfpvedata, _punish_list );
        }

        return _punish_list;
    }

    void KFPVEModule::PVEFleePunishData( KFEntity* player, KFRealmData* kfpvedata, PunishDataList& punishlist )
    {
        auto kfpvesetting = KFPVEConfig::Instance()->FindSetting( kfpvedata->_data.id() );
        if ( kfpvesetting == nullptr )
        {
            return;
        }

        auto kfpunishsetting = KFPunishConfig::Instance()->FindSetting( kfpvesetting->_punish_id );
        if ( kfpunishsetting == nullptr )
        {
            return;
        }

        // 先计算总权重
        auto tempid = 0u;
        KFWeightList< DynamicPunishData > randlist;
        for ( auto& punishdata : kfpunishsetting->_punish_list )
        {
            auto weight = punishdata._range_weight.CalcValue();
            auto randdata = randlist.Create( ++tempid, weight );
            randdata->_name = punishdata._name;
            randdata->_key = punishdata._key;
            randdata->_value = punishdata._range_value.CalcValue();
        }

        // 计算惩罚数量
        UInt32Set excludelist;
        auto punishcount = kfpunishsetting->_range_count.CalcValue();
        for ( auto i = 0u; i < punishcount; ++i )
        {
            auto punishdata = randlist.Rand( excludelist, true );
            if ( punishdata != nullptr )
            {
                excludelist.insert( punishdata->_id );
                PVEFleePunishData( player, punishdata, punishlist );
            }
        }
    }

#define  __ADD_PUNISH_DATA__( name, key, value )\
    if ( value != 0u )\
    {\
        punishlist.emplace_back( name, key, value );\
    }\

    void KFPVEModule::PVEFleePunishData( KFEntity* player, const DynamicPunishData* punishdata, PunishDataList& punishlist )
    {
        if ( punishdata->_name == __STRING__( dechp ) )
        {
            // 扣除血量
            _kf_hero_team->OperateHpValue( player, KFEnum::Dec, punishdata->_value );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, punishdata->_value );
        }
        else if ( punishdata->_name == __STRING__( addbuff ) )
        {
            // 添加buff( 用掉落实现 )
            auto& droplist = _kf_drop->Drop( player, punishdata->_value, __STRING__( pve ), punishdata->_id, __FUNC_LINE__ );
            for ( auto dropdata : droplist )
            {
                __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, dropdata->_data_key );
            }
        }
        else if ( punishdata->_name == __STRING__( faith ) )
        {
            // 添加信仰值
            OperateFaith( player, KFEnum::Add, punishdata->_value );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, punishdata->_value );
        }
        else if ( punishdata->_name == __STRING__( rune ) )
        {
            // 失去符石
            auto kfitemrecord = player->Find( __STRING__( rune ) );
            for ( auto i = 0u; i < punishdata->_value; ++i )
            {
                uint32 itemcount = 0u;
                auto itemid = _kf_item->RandRemoveItem( player, kfitemrecord, itemcount );
                __ADD_PUNISH_DATA__( punishdata->_name, itemid, itemcount );
            }
        }
        else if ( punishdata->_name == __STRING__( money ) )
        {
            auto itemcount = PVEFleePunishItem( player, punishdata, false );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
        else if ( punishdata->_name == __STRING__( moneypercent ) )
        {
            auto itemcount = PVEFleePunishItem( player, punishdata, true );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
        else if ( punishdata->_name == __STRING__( supplies ) )
        {
            auto itemcount = PVEFleePunishItem( player, punishdata, false );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
        else if ( punishdata->_name == __STRING__( suppliespercent ) )
        {
            auto itemcount = PVEFleePunishItem( player, punishdata, true );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
        else if ( punishdata->_name == __STRING__( food ) )
        {
            // 扣除粮食
            auto itemcount = PVEFleePunishItem( player, punishdata, false );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
        else if ( punishdata->_name == __STRING__( foodpercent ) )
        {
            // 扣除粮食百分比
            auto itemcount = PVEFleePunishItem( player, punishdata, true );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
    }

    uint32 KFPVEModule::PVEFleePunishItem( KFEntity* player, const DynamicPunishData* punishdata, bool percent )
    {
        auto kfitemrecord = _kf_item->FindItemRecord( player, punishdata->_key );

        auto itemcount = 0u;
        if ( !percent )
        {
            itemcount = _kf_item->GetItemCount( player, kfitemrecord, punishdata->_key, punishdata->_value );
        }
        else
        {
            itemcount = _kf_item->GetItemCount( player, kfitemrecord, punishdata->_key );
            itemcount = static_cast< double >( itemcount ) * static_cast< double >( punishdata->_value ) / KFRandEnum::TenThousand;
        }

        _kf_item->RemoveItem( player, kfitemrecord, punishdata->_key, itemcount );
        return itemcount;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_DROP_LOGIC_FUNCTION__( KFPVEModule::OnDropHeroExp )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );

        auto kfblancerecord = _pve_record.Find( player->GetKeyID() );
        if ( kfblancerecord != nullptr && !kfblancerecord->_fight_hero.empty() )
        {
            for ( auto uuid : kfblancerecord->_fight_hero )
            {
                auto kfhero = _kf_hero->FindAliveHero( kfherorecord, uuid );
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
            auto hp = _kf_hero->OperateHp( player, kfhero, KFEnum::Set, pbdata->hp(), true );
            if ( hp == 0u )
            {
                continue;
            }

            //  更新exp
            if ( pbdata->exp() > 0u )
            {
                _kf_hero->AddExp( player, kfhero, pbdata->exp() );
            }
        }

        player->SyncDataToClient();
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

        auto kfpvesetting = KFPVEConfig::Instance()->FindSetting( kfrecord->_data.id() );
        if ( kfpvesetting == nullptr )
        {
            __LOG_ERROR__( "pveid=[{}] can't find setting", kfrecord->_data.id() );
            return _kf_display->SendToClient( player, KFMsg::PVEIdError );
        }

        auto kfherorecord = player->Find( __STRING__( hero ) );
        for ( auto i = 0; i < kfmsg.herolist_size(); i++ )
        {
            auto herouuid = kfmsg.herolist( i );

            auto kfhero = kfherorecord->Find( herouuid );
            if ( kfhero == nullptr || kfhero->Get( __STRING__( fighter ), __STRING__( hp ) ) == 0u )
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

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr && kfpvesetting->_durability != 0u )
        {
            // 不在探索中的战斗需要消耗耐久度
            _kf_hero_team->DecHeroPVEDurability( player, kfrecord->_fight_hero, kfpvesetting->_durability );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandleUpdateFaithReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateFaithReq );

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            kfrealmdata = _pve_record.Find( player->GetKeyID() );
            if ( kfrealmdata == nullptr )
            {
                return;
            }
        }

        if ( kfrealmdata->IsInnerWorld() )
        {
            if ( kfmsg.faith() >= kfrealmdata->_data.faith() )
            {
                // 已经在里世界不能增加信仰
                return;
            }
        }

        OperateFaith( player, KFEnum::Set, kfmsg.faith() );
    }

    void KFPVEModule::OperateFaith( KFEntity* player, uint32 operate, uint32 value )
    {
        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            kfrealmdata = _pve_record.Find( player->GetKeyID() );
            if ( kfrealmdata == nullptr )
            {
                return;
            }
        }

        switch ( operate )
        {
        case KFEnum::Add:
            if ( kfrealmdata->IsInnerWorld() )
            {
                // 已经在里世界不能增加信仰
                return;
            }
            kfrealmdata->_data.set_faith( value + kfrealmdata->_data.faith() );
            break;
        case KFEnum::Dec:
            kfrealmdata->_data.set_faith( kfrealmdata->_data.faith() - __MIN__( value, kfrealmdata->_data.faith() ) );
            break;
        case KFEnum::Set:
            kfrealmdata->_data.set_faith( value );
            break;
        }

        static auto _enteroption = KFGlobal::Instance()->FindConstant( __STRING__( enterfaith ) );
        static auto _leaveoption = KFGlobal::Instance()->FindConstant( __STRING__( leavefaith ) );

        if ( kfrealmdata->_data.faith() >= _enteroption->_uint32_value )
        {
            kfrealmdata->_data.set_innerworld( 1u );
        }
        else if ( kfrealmdata->_data.faith() <= _leaveoption->_uint32_value )
        {
            kfrealmdata->_data.set_innerworld( 0u );
        }

        // 发消息更新给客户端
        KFMsg::MsgUpdateFaithAck ack;
        ack.set_faith( kfrealmdata->_data.faith() );
        ack.set_worldflag( kfrealmdata->_data.innerworld() );
        _kf_player->SendToClient( player, KFMsg::MSG_UPDATE_FAITH_ACK, &ack );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandleKillNpcReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgKillNpcReq );

        auto kfnpc = player->Find( __STRING__( npc ), kfmsg.npcuuid() );
        if ( kfnpc == nullptr )
        {
            return;
        }
        kfnpc->Set( __STRING__( death ), 1u );

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.herouuid() );
        if ( kfhero == nullptr )
        {
            return;
        }

        kfnpc->Set( __STRING__( killrace ), kfhero->Get( __STRING__( race ) ) );
        kfnpc->Set( __STRING__( killprofession ), kfhero->Get( __STRING__( profession ) ) );
        kfnpc->Set( __STRING__( killweapontype ), kfhero->Get( __STRING__( weapontype ) ) );
        kfnpc->Set( __STRING__( killmovetype ), kfhero->Get( __STRING__( movetype ) ) );
    }

    void KFPVEModule::StatisticsHeroKillNpcs( KFEntity* player, KFRealmData* kfrecord, uint32 pveresult )
    {
        auto kfnpcrecord = player->Find( __STRING__( npc ) );
        for ( auto kfnpc = kfnpcrecord->First(); kfnpc != nullptr; kfnpc = kfnpcrecord->Next() )
        {
            kfnpc->Set( __STRING__( pveresult ), pveresult );
        }
        player->CleanData( kfnpcrecord, true );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFPVEModule::IsInnerWorld( KFEntity* player )
    {
        if ( _kf_realm->IsInnerWorld( player ) )
        {
            return true;
        }

        auto kfpvedata = _pve_record.Find( player->GetKeyID() );
        if ( kfpvedata == nullptr )
        {
            return false;
        }

        return kfpvedata->IsInnerWorld();
    }

    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandleTurnStartReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPVETurnStartReq );

        auto kfpvedata = _pve_record.Find( player->GetKeyID() );
        if ( kfpvedata == nullptr )
        {
            return;
        }

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        for ( auto& iter : _turn_start_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player, kfrealmdata, kfpvedata, kfmsg.turn() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandleTurnFinishReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPVETurnFinsihReq );

        auto kfpvedata = _pve_record.Find( player->GetKeyID() );
        if ( kfpvedata == nullptr )
        {
            return;
        }

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        for ( auto& iter : _turn_finish_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player, kfrealmdata, kfpvedata, kfmsg.turn() );
        }
    }
}