#include "KFRealmModule.hpp"

namespace KFrame
{
    void KFRealmModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_LEAVE_PLAYER__( &KFRealmModule::OnLeaveSaveRealmData );
        __REGISTER_DROP_LOGIC__( __STRING__( addbuff ), &KFRealmModule::OnDropHeroAddBuff );
        __REGISTER_DROP_LOGIC__( __STRING__( decbuff ), &KFRealmModule::OnDropHeroDecBuff );

        __REGISTER_EXECUTE__( __STRING__( realm ), &KFRealmModule::OnExecuteRealm );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_REALM_ENTER_REQ, &KFRealmModule::HandleRealmEnterReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REALM_JUMP_REQ, &KFRealmModule::HandleRealmJumpReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REALM_EXTEND_REQ, &KFRealmModule::HandleRealmExtendReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REALM_TOWN_REQ, &KFRealmModule::HandleRealmTownReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REALM_EXIT_REQ, &KFRealmModule::HandleRealmExitReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REALM_BALANCE_REQ, &KFRealmModule::HandleRealmBalanceReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_PLAYER_REQ, &KFRealmModule::HandleUpdateExplorePlayerReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_NPC_REQ, &KFRealmModule::HaneleUpdateExploreNpcReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_DROP_REQ, &KFRealmModule::HandleExploreDropReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_FAITH_REQ, &KFRealmModule::HandleUpdateFaithReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_INTERACT_ITEM_REQ, &KFRealmModule::HandleInteractItemReq );
    }

    void KFRealmModule::BeforeShut()
    {
        __UN_LEAVE_PLAYER__();

        __UN_DROP_LOGIC__( __STRING__( addbuff ) );
        __UN_DROP_LOGIC__( __STRING__( decbuff ) );

        __UN_EXECUTE__( __STRING__( realm ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_REALM_ENTER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REALM_JUMP_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REALM_EXTEND_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REALM_TOWN_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REALM_EXIT_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REALM_BALANCE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_PLAYER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_NPC_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_DROP_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_FAITH_REQ );
        __UN_MESSAGE__( KFMsg::MSG_INTERACT_ITEM_REQ );
    }

    __KF_EXECUTE_FUNCTION__( KFRealmModule::OnExecuteRealm )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute explore param size<2" );
            return false;
        }

        auto realmid = executedata->_param_list._params[ 0 ]->_int_value;
        auto level = executedata->_param_list._params[ 1 ]->_int_value;
        if ( realmid == 0u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute realmid=[{}] level=[{}]", realmid, level );
            return false;
        }

        level = __MAX__( 1u, level );
        return RealmEnter( player, realmid, level, KFMsg::EnterChapter, modulename, moduleid );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleRealmEnterReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRealmEnterReq );

        RealmEnter( player, kfmsg.realmid(), 0u, kfmsg.type(), _invalid_string, _invalid_int );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleRealmJumpReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRealmJumpReq );

        auto realmid = player->Get<uint32>( __STRING__( realmid ) );
        RealmEnter( player, realmid, kfmsg.level(), KFMsg::EnterJump, _invalid_string, _invalid_int );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleRealmExtendReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRealmExtendReq );

        RealmEnter( player, kfmsg.exploreid(), 0, KFMsg::EnterExtend, _invalid_string, _invalid_int );
    }

    bool KFRealmModule::RealmEnter( KFEntity* player, uint32 realmid, uint32 level, uint32 entertype, const std::string& modulename, uint64 moduleid )
    {
        uint32 result = KFMsg::Error;
        KFRealmData* kfrealmdata = nullptr;
        KFMsg::PBExploreData* pbexplore = nullptr;
        switch ( entertype )
        {
        case KFMsg::EnterChapter:	// 世界地图
            std::tie( result, kfrealmdata, pbexplore ) = RealmChapterEnter( player, realmid, level, modulename, moduleid );
            break;
        case KFMsg::EnterLogin:		// 断线重连
            std::tie( result, kfrealmdata, pbexplore ) = RealmLoginEnter( player, realmid );
            break;
        case KFMsg::EnterTown:		// 回城回归
            std::tie( result, kfrealmdata, pbexplore ) = RealmTownEnter( player, realmid );
            break;
        case KFMsg::EnterJump:		// 跳转地图
            std::tie( result, kfrealmdata, pbexplore ) = RealmJumpEnter( player, realmid, level );
            break;
        case KFMsg::EnterExtend:	// 进裂隙层
            std::tie( result, kfrealmdata, pbexplore ) = RealmExtendEnter( player, realmid );
            break;
        default:
            break;
        }

        if ( result != KFMsg::Ok )
        {
            _kf_display->SendToClient( player, result );
            return false;
        }

        // 设置为探索状态
        player->SetStatus( KFMsg::ExploreStatus );

        // 下发给客户端
        KFMsg::MsgRealmEnterAck ack;
        ack.set_realmid( kfrealmdata->_data.id() );
        ack.set_faith( kfrealmdata->_data.faith() );
        ack.mutable_exploredata()->CopyFrom( *pbexplore );
        ack.mutable_buffdata()->CopyFrom( kfrealmdata->_data.buffdata() );
        return _kf_player->SendToClient( player, KFMsg::MSG_REALM_ENTER_ACK, &ack );
    }

    std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> KFRealmModule::RealmChapterEnter( KFEntity* player, uint32 realmid, uint32 level, const std::string& modulename, uint64 moduleid )
    {
        auto kfsetting = KFRealmConfig::Instance()->FindSetting( realmid );
        if ( kfsetting == nullptr )
        {
            return std::make_tuple( KFMsg::RealmMapError, nullptr, nullptr );
        }

        auto kfexplorelevel = kfsetting->FindRealmLevel( level );
        if ( kfexplorelevel == nullptr )
        {
            return std::make_tuple( KFMsg::RealmMapError, nullptr, nullptr );
        }

        // 判断和扣除花费
        auto& dataname = player->RemoveElement( &kfsetting->_consume, _default_multiple, __STRING__( realm ), realmid, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return std::make_tuple( KFMsg::DataNotEnough, nullptr, nullptr );
        }

        // 销毁原来的探索数据, 包括背包
        auto lastrealmid = player->Get<uint32>( __STRING__( realmid ) );
        if ( lastrealmid != 0u && lastrealmid != realmid )
        {
            RealmBalanceClearData( player, KFMsg::Chapter );
        }

        // 保存地图
        player->UpdateData( __STRING__( realmid ), KFEnum::Set, realmid );
        player->UpdateData( __STRING__( realmtown ), KFEnum::Set, _invalid_int );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto kfrealmdata = _realm_data.Create( player->GetKeyID() );
        kfrealmdata->_data.set_id( realmid );
        kfrealmdata->_data.set_level( level );
        kfrealmdata->_data.set_moduleid( moduleid );
        kfrealmdata->_data.set_modulename( modulename );
        kfrealmdata->_data.set_status( player->GetStatus() );
        kfrealmdata->_data.set_extendlevel( KFMsg::ExtendLevel );
        kfrealmdata->_data.set_starttime( KFGlobal::Instance()->_real_time );

        // 纪录结算开始数据
        kfrealmdata->BalanceBeginData( player );

        // level explore
        auto pbexplore = kfrealmdata->FindExeploreData( level );
        pbexplore->set_creation( kfexplorelevel->_createion );
        pbexplore->set_innerworld( kfexplorelevel->_inner_world );
        ExploreInitData( pbexplore, kfexplorelevel->_explore_id, ( uint32 )kfsetting->_levels.size(), level, 0u );
        return std::make_tuple( KFMsg::Ok, kfrealmdata, pbexplore );
    }

    void KFRealmModule::ExploreInitData( KFMsg::PBExploreData* pbexplore, uint32 exploreid, uint32 maxlevel, uint32 level, uint32 lastlevel )
    {
        pbexplore->set_lastlevel( lastlevel );
        if ( pbexplore->id() != 0u )
        {
            return;
        }

        pbexplore->set_save( false );
        pbexplore->set_level( level );
        pbexplore->set_id( exploreid );
        pbexplore->set_totallevel( maxlevel );
        pbexplore->set_random( KFGlobal::Instance()->RandRatio( ( uint32 )__MAX_INT32__ ) );
    }

    std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> KFRealmModule::RealmLoginEnter( KFEntity* player, uint32 realmid )
    {
        auto count = _kf_hero_team->GetAliveHeroCount( player );
        if ( count == 0u )
        {
            player->UpdateData( __STRING__( realmid ), KFEnum::Set, 0u );
            return std::make_tuple( KFMsg::RealmHeroTeamEmpty, nullptr, nullptr );
        }

        auto strdata = player->Get<std::string>( __STRING__( realmdata ) );
        if ( strdata.empty() )
        {
            player->UpdateData( __STRING__( realmid ), KFEnum::Set, 0u );
            return std::make_tuple( KFMsg::RealmNotInExplore, nullptr, nullptr );
        }

        player->Set( __STRING__( realmdata ), _invalid_string );
        auto kfrealmdata = _realm_data.Create( player->GetKeyID() );
        auto ok = kfrealmdata->_data.ParseFromString( strdata );
        if ( !ok )
        {
            player->UpdateData( __STRING__( realmid ), KFEnum::Set, 0u );
            return std::make_tuple( KFMsg::RealmDataError, nullptr, nullptr );
        }

        if ( realmid != kfrealmdata->_data.id() )
        {
            return std::make_tuple( KFMsg::RealmIdMismatch, nullptr, nullptr );
        }

        // 如果在回城状态
        auto realmtown = player->Get<uint32>( __STRING__( realmtown ) );
        if ( realmtown == 1u )
        {
            return std::make_tuple( KFMsg::RealmInTown, nullptr, nullptr );
        }

        // 设置开始时间
        kfrealmdata->_data.set_starttime( KFGlobal::Instance()->_real_time );

        auto pbexplore = kfrealmdata->FindExeploreData( kfrealmdata->_data.level() );
        return std::make_tuple( KFMsg::Ok, kfrealmdata, pbexplore );
    }

    std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> KFRealmModule::RealmTownEnter( KFEntity* player, uint32 realmid )
    {
        auto realmtown = player->Get<uint32>( __STRING__( realmtown ) );
        if ( realmtown == 0u )
        {
            return std::make_tuple( KFMsg::RealmNotTown, nullptr, nullptr );
        }

        // 设置探索不在回城状态
        player->UpdateData( __STRING__( realmtown ), KFEnum::Set, 0u );

        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            auto strdata = player->Get<std::string>( __STRING__( realmdata ) );
            if ( strdata.empty() )
            {
                player->UpdateData( __STRING__( realmid ), KFEnum::Set, 0u );
                return std::make_tuple( KFMsg::RealmNotInExplore, nullptr, nullptr );
            }

            player->Set( __STRING__( realmdata ), _invalid_string );
            kfrealmdata = _realm_data.Create( player->GetKeyID() );
            auto ok = kfrealmdata->_data.ParseFromString( strdata );
            if ( !ok )
            {
                player->UpdateData( __STRING__( realmid ), KFEnum::Set, 0u );
                return std::make_tuple( KFMsg::RealmDataError, nullptr, nullptr );
            }

            // 设置开始时间
            kfrealmdata->_data.set_starttime( KFGlobal::Instance()->_real_time );
        }

        // 重新统计数据
        kfrealmdata->BalanceBeginData( player );

        auto pbexplore = kfrealmdata->FindExeploreData( kfrealmdata->_data.level() );
        return std::make_tuple( KFMsg::Ok, kfrealmdata, pbexplore );
    }

    std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> KFRealmModule::RealmJumpEnter( KFEntity* player, uint32 realmid, uint32 level )
    {
        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            return std::make_tuple( KFMsg::RealmNotInStatus, nullptr, nullptr );
        }

        auto kfsetting = KFRealmConfig::Instance()->FindSetting( realmid );
        if ( kfsetting == nullptr )
        {
            return std::make_tuple( KFMsg::RealmMapError, nullptr, nullptr );
        }

        auto kfexplorelevel = kfsetting->FindRealmLevel( level );
        if ( kfexplorelevel == nullptr )
        {
            return std::make_tuple( KFMsg::RealmMapError, nullptr, nullptr );
        }

        // 清空上一次的数据
        auto lastlevel = kfrealmdata->_data.level();
        kfrealmdata->_data.set_level( level );
        if ( lastlevel < KFMsg::ExtendLevel )
        {
            auto pbexplore = kfrealmdata->FindExeploreData( lastlevel );
            pbexplore->mutable_npcdata()->clear();

            pbexplore->mutable_playerdata()->set_step( 0 );
            pbexplore->mutable_playerdata()->mutable_fovarr()->Clear();
        }
        else
        {
            kfrealmdata->RemoveExeploreData( lastlevel );
        }

        auto pbexplore = kfrealmdata->FindExeploreData( level );
        pbexplore->set_creation( kfexplorelevel->_createion );
        pbexplore->set_innerworld( kfexplorelevel->_inner_world );
        ExploreInitData( pbexplore, kfexplorelevel->_explore_id, ( uint32 )kfsetting->_levels.size(), level, lastlevel );

        return std::make_tuple( KFMsg::Ok, kfrealmdata, pbexplore );
    }

    std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> KFRealmModule::RealmExtendEnter( KFEntity* player, uint32 exploreid )
    {
        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            return std::make_tuple( KFMsg::RealmNotInStatus, nullptr, nullptr );
        }

        // 计算裂隙层的层数
        auto extendlevel = kfrealmdata->_data.extendlevel() + 1;
        kfrealmdata->_data.set_extendlevel( extendlevel );

        auto lastlevel = kfrealmdata->_data.level();
        kfrealmdata->_data.set_level( extendlevel );
        ////////////////////////////////////////////////////////////////////////////////
        auto pbexplore = kfrealmdata->FindExeploreData( extendlevel );
        ExploreInitData( pbexplore, exploreid, 0u, extendlevel, lastlevel );
        return std::make_tuple( KFMsg::Ok, kfrealmdata, pbexplore );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleRealmTownReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRealmTownReq );

        // 逃跑结算
        RealmBalance( player, KFMsg::Town );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleRealmExitReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRealmExitReq );

        // 逃跑结算
        RealmBalance( player, KFMsg::Flee );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleRealmBalanceReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRealmBalanceReq );

        // 探索结算
        RealmBalance( player, kfmsg.result() );
    }

    void KFRealmModule::RealmBalance( KFEntity* player, uint32 result )
    {
        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInStatus );
        }

        player->SetStatus( kfrealmdata->_data.status() );
        switch ( result )
        {
        case KFMsg::Victory:
            RealmBalanceVictory( player, kfrealmdata );
            break;
        case KFMsg::Failed:
            RealmBalanceFailed( player, kfrealmdata );
            break;
        case KFMsg::Flee:
            RealmBalanceFlee( player, kfrealmdata );
            break;
        case KFMsg::Town:
            RealmBalanceTown( player, kfrealmdata );
            break;
        }
    }

    void KFRealmModule::SendRealmBalanceToClient( KFEntity* player, KFRealmData* kfrealmdata, uint32 result )
    {
        KFMsg::MsgRealmBalanceAck ack;
        ack.set_result( result );
        kfrealmdata->BalanceRealmRecord( ack.mutable_balance(), KFMsg::ExploreStatus );
        _kf_player->SendToClient( player, KFMsg::MSG_REALM_BALANCE_ACK, &ack, 10u );
    }

    void KFRealmModule::RealmBalanceVictory( KFEntity* player, KFRealmData* kfrealmdata )
    {
        // 先结算货币
        kfrealmdata->BalanceCurrencyEndData( player );

        // 结算掉落
        RealmBalanceDrop( player, kfrealmdata, KFMsg::Victory );

        // 结算最终数据
        kfrealmdata->BalanceHeroEndData( player );
        kfrealmdata->BalanceItemEndData( player );

        // 发送消息
        SendRealmBalanceToClient( player, kfrealmdata, KFMsg::Victory );

        // 设置回调属性
        RealmBalanceResultCondition( player, kfrealmdata, KFMsg::Victory );

        // 清空探索数据
        RealmBalanceClearData( player, KFMsg::Victory );
    }

    void KFRealmModule::RealmBalanceFailed( KFEntity* player, KFRealmData* kfrealmdata )
    {
        // 先结算货币
        kfrealmdata->BalanceCurrencyEndData( player );

        // 结算掉落
        RealmBalanceDrop( player, kfrealmdata, KFMsg::Failed );

        // 随机探索失败获得道具
        RealmRandFailedItems( player );

        // 结算最终数据
        kfrealmdata->BalanceHeroEndData( player );
        kfrealmdata->BalanceItemEndData( player );

        // 发送消息
        SendRealmBalanceToClient( player, kfrealmdata, KFMsg::Failed );

        // 设置回调属性
        RealmBalanceResultCondition( player, kfrealmdata, KFMsg::Failed );

        // 清空探索数据
        RealmBalanceClearData( player, KFMsg::Failed );
    }

    void KFRealmModule::RealmBalanceFlee( KFEntity* player, KFRealmData* kfrealmdata )
    {
        // 先结算
        kfrealmdata->BalanceEndData( player );

        // 发送消息
        SendRealmBalanceToClient( player, kfrealmdata, KFMsg::Flee );

        // 清空探索数据
        RealmBalanceClearData( player, KFMsg::Flee );
    }

    void KFRealmModule::RealmBalanceTown( KFEntity* player, KFRealmData* kfrealmdata )
    {
        // 结算获得的道具
        kfrealmdata->BalanceEndData( player );

        // 发送消息
        SendRealmBalanceToClient( player, kfrealmdata, KFMsg::Town );

        // 设置回城状态
        player->UpdateData( __STRING__( realmtown ), KFEnum::Set, 1u );
    }

    void KFRealmModule::RealmBalanceDrop( KFEntity* player, KFRealmData* kfrealmdata, uint32 result )
    {
        auto kfsetting = KFRealmConfig::Instance()->FindRealmLevel( kfrealmdata->_data.id(), kfrealmdata->_data.level() );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 把原来的显示同步到客户端
        player->ShowElementToClient();

        // 结算胜利掉落
        switch ( result )
        {
        case KFMsg::Victory:
            _kf_drop->Drop( player, kfsetting->_victory_drop_list, __STRING__( realm ), __FUNC_LINE__ );
            break;
        case KFMsg::Failed:
            _kf_drop->Drop( player, kfsetting->_fail_drop_list, __STRING__( realm ), __FUNC_LINE__ );
            break;
        }

        // 掉落显示
        kfrealmdata->BalanceAddDropData( player );
    }

    void KFRealmModule::RealmBalanceClearData( KFEntity* player, uint32 result )
    {
        // 清除数据
        _realm_data.Remove( player->GetKeyID() );

        // 探索结束后就移除寿命不足的英雄
        _kf_hero_team->RemoveTeamHeroDurability( player );

        // 清空队伍英雄ep
        _kf_hero_team->ClearTeamHeroEp( player );

        // 清除探索内道具背包
        switch ( result )
        {
        case KFMsg::Victory:
        case KFMsg::Failed:
            _kf_item->CleanItem( player, __STRING__( other ), true );
            _kf_item->CleanItem( player, __STRING__( explore ), true );
            break;
        case KFMsg::Flee:
            player->CleanData( __STRING__( other ) );
            _kf_item->CleanItem( player, __STRING__( explore ), true );
            break;
        case KFMsg::Chapter:
            player->CleanData( __STRING__( other ) );
            break;
        }

        // 探索纪录id
        player->Set( __STRING__( random ), KFEnum::Set, 0 );
        player->Set( __STRING__( realmdata ), _invalid_string );
        player->UpdateData( __STRING__( realmid ), KFEnum::Set, 0u );
        player->UpdateData( __STRING__( realmtown ), KFEnum::Set, 0u );
    }

    void KFRealmModule::RealmBalanceResultCondition( KFEntity* player, KFRealmData* kfrealmdata, uint32 result )
    {
        auto kfbalance = player->Find( __STRING__( balance ) );
        kfbalance->Set( __STRING__( id ), kfrealmdata->_data.id() );

        auto teamheronum = _kf_hero_team->GetHeroCount( player );
        player->UpdateData( kfbalance, __STRING__( team ), KFEnum::Set, teamheronum );

        auto teamdeathnum = _kf_hero_team->GetDeadHeroCount( player );
        player->UpdateData( kfbalance, __STRING__( death ), KFEnum::Set, teamdeathnum );

        auto usetime = KFGlobal::Instance()->_real_time - kfrealmdata->_data.starttime() + kfrealmdata->_data.usetime();
        player->UpdateData( kfbalance, __STRING__( time ), KFEnum::Set, usetime );

        player->UpdateData( kfbalance, __STRING__( realmresult ), KFEnum::Set, result );
    }

    void KFRealmModule::RealmRandFailedItems( KFEntity* player )
    {
        static auto _option = _kf_option->FindOption( __STRING__( explorebreakoffprob ) );

        UInt64Set destory_item_set;
        auto kfitemrecord = player->Find( __STRING__( other ) );
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
            if ( rand >= _option->_uint32_value )
            {
                destory_item_set.insert( kfitem->GetKeyID() );
            }
        }

        for ( auto iter : destory_item_set )
        {
            player->RemoveData( kfitemrecord, iter );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFrame::KFRealmData* KFRealmModule::GetRealmData( uint64 keyid )
    {
        return _realm_data.Find( keyid );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFRealmModule::OnLeaveSaveRealmData )
    {
        auto kfrecord = _realm_data.Find( player->GetKeyID() );
        if ( kfrecord == nullptr )
        {
            return;
        }

        auto usetime = KFGlobal::Instance()->_real_time - kfrecord->_data.starttime();
        kfrecord->_data.set_usetime( kfrecord->_data.usetime() + usetime );

        auto strdata = kfrecord->_data.SerializeAsString();
        player->Set( __STRING__( realmdata ), strdata );
        _realm_data.Remove( player->GetKeyID() );
    }

    __KF_DROP_LOGIC_FUNCTION__( KFRealmModule::OnDropHeroAddBuff )
    {
        auto buffid = dropdata->GetValue();
        ChangeTeamHeroBuff( player, KFEnum::Add, buffid );
        player->AddDataToShow( dropdata->_data_name, buffid, false );
    }

    __KF_DROP_LOGIC_FUNCTION__( KFRealmModule::OnDropHeroDecBuff )
    {
        auto buffid = dropdata->GetValue();
        ChangeTeamHeroBuff( player, KFEnum::Dec, buffid );
        player->AddDataToShow( dropdata->_data_name, buffid, false );
    }

    void KFRealmModule::ChangeTeamHeroBuff( KFEntity* player, uint32 operate, uint32 value )
    {
        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            return;
        }

        KFMsg::MsgUpdateExploreBuffAck ack;

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

            auto buff = ack.add_bufflist();
            buff->set_uuid( uuid );
            buff->set_operate( operate );
            buff->set_buffid( value );

            switch ( operate )
            {
            case KFEnum::Add:
                kfrealmdata->AddBuffData( uuid, value );
                break;
            case KFEnum::Dec:
                kfrealmdata->RemoveBuffData( uuid, value );
                break;
            }
        }

        if ( ack.bufflist_size() > 0 )
        {
            _kf_player->SendToClient( player, KFMsg::MSG_UPDATE_EXPLORE_BUFF_ACK, &ack );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleUpdateExplorePlayerReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateExplorePlayerReq );

        auto kfrealmdata = _realm_data.Find( playerid );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInStatus );
        }

        auto pbexplore = kfrealmdata->FindExeploreData( kfrealmdata->_data.level() );
        pbexplore->set_save( true );

        auto playerdata = pbexplore->mutable_playerdata();

        playerdata->set_x( kfmsg.playerdata().x() );
        playerdata->set_y( kfmsg.playerdata().y() );
        playerdata->set_z( kfmsg.playerdata().z() );
        playerdata->set_yaw( kfmsg.playerdata().yaw() );
        playerdata->set_step( playerdata->step() + kfmsg.playerdata().step() );

        for ( auto i = 0; i < kfmsg.playerdata().fovarr_size(); ++i )
        {
            auto fov = kfmsg.playerdata().fovarr( i );
            playerdata->add_fovarr( fov );
        }

        // 计算移动步数, 扣除粮食
        ExploreCostFood( player, pbexplore );
    }

    void KFRealmModule::ExploreCostFood( KFEntity* player, KFMsg::PBExploreData* pbexplore )
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

        auto playerdata = pbexplore->mutable_playerdata();
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
            auto ok = KFElementConfig::Instance()->FormatElemnt( _item_element, __STRING__( item ), "1", _option->_uint32_value );
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
                player->RemoveElement( &_item_element, havenum, __STRING__( explore ), pbexplore->id(), __FUNC_LINE__ );

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
            for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
            {
                auto kfhero = kfherorecord->Find( kfteam->Get<uint64>() );
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
            player->RemoveElement( &_item_element, neednum, __STRING__( explore ), pbexplore->id(), __FUNC_LINE__ );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HaneleUpdateExploreNpcReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateExploreNpcReq );
        auto kfrealmdata = _realm_data.Find( playerid );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInStatus );
        }

        auto pbnpcdata = kfmsg.npcdata();
        auto findnpcdata = kfrealmdata->FindNpcData( pbnpcdata.key() );
        findnpcdata->CopyFrom( pbnpcdata );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleExploreDropReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExploreDropReq );

        UInt32Vector droplist;

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

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleUpdateFaithReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateFaithReq );

        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInStatus );
        }

        kfrealmdata->_data.set_faith( kfmsg.faith() );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleInteractItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgInteractItemReq );

        auto kfinteraction = player->Find( __STRING__( interaction ) );
        kfinteraction->Reset();

        kfinteraction->Set( __STRING__( field ), kfmsg.field() );
        player->UpdateData( kfinteraction, __STRING__( id ), KFEnum::Set, kfmsg.itemid() );
    }

}