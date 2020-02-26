#include "KFPVEModule.hpp"
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
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_PVE_REQ, &KFPVEModule::HandlePVEReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_PVE_BALANCE_REQ, &KFPVEModule::HandlePVEBalanceReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_FIGHTER_HERO_REQ, &KFPVEModule::HandleUpdateFighterHeroReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_KILL_NPC_REQ, &KFPVEModule::HandleKillNpcReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_FIGHT_HERO_LIST_REQ, &KFPVEModule::HandleFightHeroListReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_FAITH_REQ, &KFPVEModule::HandleUpdateFaithReq );
    }

    void KFPVEModule::BeforeShut()
    {
        __UN_LEAVE_PLAYER__();
        __UN_EXECUTE__( __STRING__( pve ) );
        __UN_DROP_LOGIC__( __STRING__( exp ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_PVE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_PVE_BALANCE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_FIGHTER_HERO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_KILL_NPC_REQ );
        __UN_MESSAGE__( KFMsg::MSG_FIGHT_HERO_LIST_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_FAITH_REQ );
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

    __KF_MESSAGE_FUNCTION__( KFPVEModule::HandleUpdateFaithReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateFaithReq );

        OperateFaith( player, KFEnum::Set, kfmsg.faith() );
    }

    void KFPVEModule::OperateFaith( KFEntity* player, uint32 operate, uint32 value )
    {
        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            kfrealmdata = _pve_record.Find( player->GetKeyID() );
        }

        if ( kfrealmdata == nullptr )
        {
            return;
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

        static auto _enteroption = _kf_option->FindOption( __STRING__( enterfaith ) );
        static auto _leaveoption = _kf_option->FindOption( __STRING__( leavefaith ) );

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
            __LOG_ERROR__( "pveid=[{}] can't find setting", pveid );
            return KFMsg::PVEIdError;
        }

        if ( kfsetting->_npc_group_setting == nullptr )
        {
            __LOG_ERROR__( "pveid=[{}] npcgroup=[{}] can't find setting", pveid, kfsetting->_npc_group_id );
            return KFMsg::NpcGroupError;
        }

        // 扣除费用
        auto& dataname = player->RemoveElement( &kfsetting->_consume, _default_multiple, __STRING__( pve ), pveid, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return KFMsg::DataNotEnough;
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
        /////////////////////////////////////////////////////////////////////////////////////////////
        KFMsg::MsgPVEAck ack;
        ack.set_pveid( pveid );
        ack.set_battleid( battleid );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // team
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );

        auto herocount = 0u;
        auto herolevel = 0.0f;
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
        /////////////////////////////////////////////////////////////////////////////////////////////
        // npc
        auto kfnpcrecord = player->Find( __STRING__( npc ) );
        player->CleanData( kfnpcrecord, false );

        auto npccount = 0u;
        auto npclevel = 0.0f;
        for ( auto generateid : kfsetting->_npc_group_setting->_npc_generate_list )
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
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 逃跑成功率
        auto fleerate = CalcFleeSuccessRate( player, kfsetting, herolevel, npclevel );
        ack.set_fleerate( fleerate );
        kfrecord->_data.set_fleerate( fleerate );

        // buff (探索特有)
        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata != nullptr )
        {
            // 信仰值
            ack.set_faith( kfrealmdata->_data.faith() );
            ack.set_worldflag( kfrealmdata->_data.innerworld() );
            ack.mutable_buffdata()->CopyFrom( kfrealmdata->_data.buffdata() );
        }

        _kf_player->SendToClient( player, KFMsg::MSG_PVE_ACK, &ack, 10u );

        player->SetStatus( KFMsg::PVEStatus );
        player->UpdateData( __STRING__( pveid ), KFEnum::Set, pveid );
        return KFMsg::Ok;
    }

    uint32 KFPVEModule::CalcFleeSuccessRate( KFEntity* player, const KFPVESetting* kfsetting, double herolevel, double npclevel )
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

        fleerate += ( herolevel - npclevel ) * kfsetting->_level_coefficient;

        // 团长逃跑成功率属性
        fleerate += player->Get<uint32>( __STRING__( flee ) );
        return fleerate;
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
        default:
            break;
        }

        if ( errorcode == KFMsg::Ok )
        {
            // 清除战斗数据
            PVEBalanceClearData( player );
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
        // 把原来的显示同步到客户端
        player->ShowElementToClient();

        // 结算胜利掉落
        auto& droplist = PVEGetDropList( player, kfsetting, result );
        _kf_drop->Drop( player, droplist, __STRING__( realm ), kfpvedata->_data.id(), __FUNC_LINE__ );

        // 掉落显示
        kfpvedata->BalanceAddDropData( player );
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

    bool KFPVEModule::IsInnerWorld( KFEntity* player )
    {
        if ( _kf_realm->IsInnerWorld( player ) )
        {
            return true;
        }

        auto kfrealmdata = _pve_record.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            return false;
        }

        return kfrealmdata->IsInnerWorld();
    }

    void KFPVEModule::PVEBalanceClearData( KFEntity* player )
    {
        // 设置pveid为0
        player->Set( __STRING__( pveid ), 0u );
        player->CleanData( __STRING__( npc ), false );

        auto kfrealmdata = static_cast< KFRealmModule* >( _kf_realm )->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            // 不在探索里面 战斗后就移除寿命不足
            _kf_hero_team->RemoveTeamHeroDurability( player );

            // 清空队伍英雄ep
            _kf_hero_team->ClearTeamHeroEp( player );
        }

        // 删除纪录
        _pve_record.Remove( player->GetKeyID() );
    }

    void KFPVEModule::SendPVEBalanceToClient( KFEntity* player, KFRealmData* kfpvedata, uint32 result )
    {
        KFMsg::MsgPVEBalanceAck ack;
        ack.set_result( result );
        ack.set_moduleid( kfpvedata->_data.moduleid() );
        ack.set_modulename( kfpvedata->_data.modulename() );
        kfpvedata->BalanceRealmRecord( ack.mutable_balance(), KFMsg::PVEStatus );
        _kf_player->SendToClient( player, KFMsg::MSG_PVE_BALANCE_ACK, &ack, 10u );
    }

    void KFPVEModule::PVEBalanceRecord( KFEntity* player, KFRealmData* kfpvedata, uint32 result )
    {
        // 更新死亡英雄(死亡英雄可获得结算经验)
        _kf_hero_team->UpdateTeamDeadHero( player );

        // 先结算货币
        kfpvedata->BalanceCurrencyEndData( player );

        // 战斗结算
        PVEBalanceDrop( player, kfpvedata, result );

        // 结算最终数据
        kfpvedata->BalanceHeroEndData( player );
    }

    uint32 KFPVEModule::PVEBalanceVictory( KFEntity* player, KFRealmData* kfpvedata, uint32 truns )
    {
        // 胜场增加
        player->UpdateData( __STRING__( pvevictory ), KFEnum::Add, 1u );

        // 战斗结果条件结算
        PVEBalanceResultCondition( player, kfpvedata, KFMsg::Victory, truns );

        // 战斗纪录
        PVEBalanceRecord( player, kfpvedata, KFMsg::Victory );

        // 发送给客户端
        SendPVEBalanceToClient( player, kfpvedata, KFMsg::Victory );
        return KFMsg::Ok;
    }

    uint32 KFPVEModule::PVEBalanceFailed( KFEntity* player, KFRealmData* kfpvedata, uint32 truns )
    {
        // 战斗结果条件结算
        PVEBalanceResultCondition( player, kfpvedata, KFMsg::Failed, truns );

        // 战斗纪录
        PVEBalanceRecord( player, kfpvedata, KFMsg::Failed );

        // 发送给客户端
        SendPVEBalanceToClient( player, kfpvedata, KFMsg::Failed );
        return KFMsg::Ok;
    }

    uint32 KFPVEModule::PVEBalanceFlee( KFEntity* player, KFRealmData* kfpvedata, uint32 truns )
    {
        if ( kfpvedata->_data.fleerate() == 0u )
        {
            return KFMsg::PVECanNotFlee;
        }

        // 更新死亡英雄(死亡英雄可获得结算经验)
        _kf_hero_team->UpdateTeamDeadHero( player );

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
            auto weight = KFGlobal::Instance()->RandRange( punishdata._min_weight, punishdata._max_weight, 1u );
            auto randdata = randlist.Create( ++tempid, weight );
            randdata->_name = punishdata._name;
            randdata->_key = punishdata._key;
            randdata->_value = KFGlobal::Instance()->RandRange( punishdata._min_value, punishdata._max_value, 1u );
        }

        // 计算惩罚数量
        UInt32Set excludelist;
        auto punishcount = KFGlobal::Instance()->RandRange( kfpunishsetting->_min_count, kfpunishsetting->_max_count, 1u );
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
            _kf_hero_team->OperateTeamHeroHp( player, KFEnum::Dec, punishdata->_value );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, punishdata->_value );
        }
        else if ( punishdata->_name == __STRING__( addbuff ) )
        {
            // 添加buff( 用掉落实现 )
            auto& droplist = _kf_drop->Drop( player, punishdata->_value, __STRING__( pve ), punishdata->_id, __FUNC_LINE__ );
            for ( auto dropdata : droplist )
            {
                __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, dropdata->GetValue() );
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
            auto itemcount = PVEFleePunishItem( player, __STRING__( other ), punishdata, false );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
        else if ( punishdata->_name == __STRING__( moneypercent ) )
        {
            auto itemcount = PVEFleePunishItem( player, __STRING__( other ), punishdata, true );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
        else if ( punishdata->_name == __STRING__( supplies ) )
        {
            auto itemcount = PVEFleePunishItem( player, __STRING__( other ), punishdata, false );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
        else if ( punishdata->_name == __STRING__( suppliespercent ) )
        {
            auto itemcount = PVEFleePunishItem( player, __STRING__( other ), punishdata, true );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
        else if ( punishdata->_name == __STRING__( food ) )
        {
            // 扣除粮食
            auto itemcount = PVEFleePunishItem( player, __STRING__( explore ), punishdata, false );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
        else if ( punishdata->_name == __STRING__( foodpercent ) )
        {
            // 扣除粮食百分比
            auto itemcount = PVEFleePunishItem( player, __STRING__( explore ), punishdata, true );
            __ADD_PUNISH_DATA__( punishdata->_name, punishdata->_key, itemcount );
        }
    }

    uint32 KFPVEModule::PVEFleePunishItem( KFEntity* player, const std::string& dataname, const DynamicPunishData* punishdata, bool percent )
    {
        auto itemcount = 0u;
        auto kfitemrecord = player->Find( dataname );
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

}