#include "KFExploreModule.hpp"

namespace KFrame
{
    void KFExploreModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_LEAVE_PLAYER__( &KFExploreModule::OnLeaveSaveExploreRecord );
        __REGISTER_DROP_LOGIC__( __STRING__( addbuff ), &KFExploreModule::OnDropHeroAddBuff );
        __REGISTER_DROP_LOGIC__( __STRING__( decbuff ), &KFExploreModule::OnDropHeroDecBuff );

        __REGISTER_EXECUTE__( __STRING__( explore ), &KFExploreModule::OnExecuteExplore );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_ENTER_REQ, &KFExploreModule::HandleExploreEnterReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_JUMP_REQ, &KFExploreModule::HandleExploreJumpReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_EXTEND_REQ, &KFExploreModule::HandleExploreExtendReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_TOWN_REQ, &KFExploreModule::HandleExploreTownReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_EXIT_REQ, &KFExploreModule::HandleExploreExitReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_BALANCE_REQ, &KFExploreModule::HandleExploreBalanceReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_PLAYER_REQ, &KFExploreModule::HandleUpdateExplorePlayerReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_NPC_REQ, &KFExploreModule::HaneleUpdateExploreNpcReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_DROP_REQ, &KFExploreModule::HandleExploreDropReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_FAITH_REQ, &KFExploreModule::HandleUpdateFaithReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_INTERACT_ITEM_REQ, &KFExploreModule::HandleInteractItemReq );
    }

    void KFExploreModule::BeforeShut()
    {
        __UN_LEAVE_PLAYER__();

        __UN_DROP_LOGIC__( __STRING__( addbuff ) );
        __UN_DROP_LOGIC__( __STRING__( decbuff ) );

        __UN_EXECUTE__( __STRING__( explore ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_ENTER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_JUMP_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_EXTEND_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_TOWN_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_EXIT_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_BALANCE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_PLAYER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_NPC_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_DROP_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_FAITH_REQ );
        __UN_MESSAGE__( KFMsg::MSG_INTERACT_ITEM_REQ );
    }

    __KF_EXECUTE_FUNCTION__( KFExploreModule::OnExecuteExplore )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute explore param size<2" );
            return false;
        }

        auto mapid = executedata->_param_list._params[ 0 ]->_int_value;
        auto level = executedata->_param_list._params[ 1 ]->_int_value;
        if ( mapid == 0u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute explore mapid=[{}] level=[{}]", mapid, level );
            return false;
        }

        level = __MAX__( 0u, level );
        return EnterExplore( player, mapid, level, KFMsg::EnterChapter, modulename, moduleid );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleExploreEnterReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExploreEnterReq );

        EnterExplore( player, kfmsg.mapid(), 0u, kfmsg.type(), _invalid_string, _invalid_int );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleExploreJumpReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExploreJumpReq );

        auto mapid = player->Get<uint32>( __STRING__( mapid ) );
        EnterExplore( player, mapid, kfmsg.level(), KFMsg::EnterJump, _invalid_string, _invalid_int );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleExploreExtendReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExploreExtendReq );

        EnterExplore( player, kfmsg.exploreid(), 0, KFMsg::EnterExtend, _invalid_string, _invalid_int );
    }

    bool KFExploreModule::EnterExplore( KFEntity* player, uint32 mapid, uint32 level, uint32 entertype, const std::string& modulename, uint64 moduleid )
    {
        uint32 result = KFMsg::Error;
        KFExploreRecord* kfrecord = nullptr;
        KFMsg::PBExploreData* pbexplore = nullptr;
        switch ( entertype )
        {
        case KFMsg::EnterChapter:	// 世界地图
            std::tie( result, kfrecord, pbexplore ) = ChapterEnterExplore( player, mapid, level, modulename, moduleid );
            break;
        case KFMsg::EnterLogin:		// 断线重连
            std::tie( result, kfrecord, pbexplore ) = LoginEnterExplore( player, mapid );
            break;
        case KFMsg::EnterTown:		// 回城回归
            std::tie( result, kfrecord, pbexplore ) = TownEnterExplore( player, mapid );
            break;
        case KFMsg::EnterJump:		// 跳转地图
            std::tie( result, kfrecord, pbexplore ) = JumpEnterExplore( player, mapid, level );
            break;
        case KFMsg::EnterExtend:	// 进裂隙层
            std::tie( result, kfrecord, pbexplore ) = ExtendEnterExplore( player, mapid );
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
        KFMsg::MsgExploreEnterAck ack;
        ack.set_faith( kfrecord->_data.faith() );
        ack.mutable_exploredata()->CopyFrom( *pbexplore );
        return _kf_player->SendToClient( player, KFMsg::MSG_EXPLORE_ENTER_ACK, &ack );
    }

    std::tuple<uint32, KFExploreRecord*, KFMsg::PBExploreData*> KFExploreModule::ChapterEnterExplore( KFEntity* player, uint32 mapid, uint32 level, const std::string& modulename, uint64 moduleid )
    {
        auto kfsetting = KFExploreConfig::Instance()->FindSetting( mapid );
        if ( kfsetting == nullptr )
        {
            return std::make_tuple( KFMsg::ExploreMapError, nullptr, nullptr );
        }

        auto kfexplorelevel = kfsetting->FindExploreLevel( level );
        if ( kfexplorelevel == nullptr )
        {
            return std::make_tuple( KFMsg::ExploreMapError, nullptr, nullptr );
        }

        // 判断和扣除花费
        if ( !kfsetting->_consume.IsEmpty() )
        {
            auto dataname = player->CheckRemoveElement( &kfsetting->_consume, __FUNC_LINE__ );
            if ( !dataname.empty() )
            {
                return std::make_tuple( KFMsg::DataNotEnough, nullptr, nullptr );
            }

            player->RemoveElement( &kfsetting->_consume, __STRING__( explore ), __FUNC_LINE__ );
        }

        // 销毁原来的探索数据, 包括背包
        auto lastmapid = player->Get<uint32>( __STRING__( mapid ) );
        if ( lastmapid != mapid )
        {
            player->CleanData( __STRING__( other ) );
            _explore_record.Remove( player->GetKeyID() );
        }

        // 保存地图
        player->UpdateData( __STRING__( mapid ), KFEnum::Set, mapid );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto kfrecord = _explore_record.Create( player->GetKeyID() );
        kfrecord->_data.set_id( mapid );
        kfrecord->_data.set_level( level );
        kfrecord->_data.set_moduleid( moduleid );
        kfrecord->_data.set_modulename( modulename );
        kfrecord->_data.set_status( player->GetStatus() );
        kfrecord->_data.set_extendlevel( KFMsg::ExtendLevel );
        kfrecord->_data.set_starttime( KFGlobal::Instance()->_real_time );

        // 纪录结算开始数据
        kfrecord->BalanceBeginData( player );

        // level explore
        auto pbexplore = kfrecord->FindExeploreData( level );
        pbexplore->set_creation( kfexplorelevel->_createion );
        pbexplore->set_innerworld( kfexplorelevel->_inner_world );
        InitExploreData( pbexplore, kfexplorelevel->_explore_id, ( uint32 )kfsetting->_levels.size(), level, 0u );
        return std::make_tuple( KFMsg::Ok, kfrecord, pbexplore );
    }

    void KFExploreModule::InitExploreData( KFMsg::PBExploreData* pbexplore, uint32 exploreid, uint32 maxlevel, uint32 level, uint32 lastlevel )
    {
        if ( pbexplore->id() != 0u )
        {
            return;
        }

        pbexplore->set_save( false );
        pbexplore->set_level( level );
        pbexplore->set_id( exploreid );
        pbexplore->set_totallevel( maxlevel );
        pbexplore->set_lastlevel( lastlevel );
        pbexplore->set_random( KFGlobal::Instance()->RandRatio( ( uint32 )__MAX_INT32__ ) );
    }

    std::tuple<uint32, KFExploreRecord*, KFMsg::PBExploreData*> KFExploreModule::LoginEnterExplore( KFEntity* player, uint32 mapid )
    {
        auto count = _kf_hero_team->GetAliveHeroCount( player );
        if ( count == 0u )
        {
            player->UpdateData( __STRING__( mapid ), KFEnum::Set, 0u );
            return std::make_tuple( KFMsg::ExploreHeroTeamEmpty, nullptr, nullptr );
        }

        auto kfrecord = _explore_record.Create( player->GetKeyID() );
        auto strrecord = player->Get<std::string>( __STRING__( explorerecord ) );
        if ( strrecord.empty() )
        {
            player->UpdateData( __STRING__( mapid ), KFEnum::Set, 0u );
            return std::make_tuple( KFMsg::ExploreNotInExplore, nullptr, nullptr );
        }

        player->Set( __STRING__( explorerecord ), _invalid_string );
        auto ok = kfrecord->_data.ParseFromString( strrecord );
        if ( !ok )
        {
            player->UpdateData( __STRING__( mapid ), KFEnum::Set, 0u );
            return std::make_tuple( KFMsg::ExploreDataError, nullptr, nullptr );
        }

        if ( mapid != kfrecord->_data.id() )
        {
            return std::make_tuple( KFMsg::ExploreIdMismatch, nullptr, nullptr );
        }

        auto pbexplore = kfrecord->FindExeploreData( kfrecord->_data.level() );
        return std::make_tuple( KFMsg::Ok, kfrecord, pbexplore );
    }

    std::tuple<uint32, KFExploreRecord*, KFMsg::PBExploreData*> KFExploreModule::TownEnterExplore( KFEntity* player, uint32 mapid )
    {
        auto kfrecord = _explore_record.Find( player->GetKeyID() );
        if ( kfrecord == nullptr )
        {
            return std::make_tuple( KFMsg::ExploreNotInStatus, nullptr, nullptr );
        }

        if ( mapid != kfrecord->_data.id() )
        {
            return std::make_tuple( KFMsg::ExploreIdMismatch, nullptr, nullptr );
        }

        auto exploretown = player->Get<uint32>( __STRING__( exploretown ) );
        if ( exploretown == 1u )
        {
            return std::make_tuple( KFMsg::ExploreNotTown, nullptr, nullptr );
        }

        // 设置探索不在回城状态
        player->UpdateData( __STRING__( exploretown ), KFEnum::Set, 0u );

        auto pbexplore = kfrecord->FindExeploreData( kfrecord->_data.level() );
        return std::make_tuple( KFMsg::Ok, kfrecord, pbexplore );
    }

    std::tuple<uint32, KFExploreRecord*, KFMsg::PBExploreData*> KFExploreModule::JumpEnterExplore( KFEntity* player, uint32 mapid, uint32 level )
    {
        auto kfrecord = _explore_record.Find( player->GetKeyID() );
        if ( kfrecord == nullptr )
        {
            return std::make_tuple( KFMsg::ExploreNotInStatus, nullptr, nullptr );
        }

        auto kfsetting = KFExploreConfig::Instance()->FindSetting( mapid );
        if ( kfsetting == nullptr )
        {
            return std::make_tuple( KFMsg::ExploreMapError, nullptr, nullptr );
        }

        auto kfexplorelevel = kfsetting->FindExploreLevel( level );
        if ( kfexplorelevel == nullptr )
        {
            return std::make_tuple( KFMsg::ExploreMapError, nullptr, nullptr );
        }

        // 清空上一次的数据
        auto lastlevel = kfrecord->_data.level();
        kfrecord->_data.set_level( level );
        if ( lastlevel < KFMsg::ExtendLevel )
        {
            auto pbexplore = kfrecord->FindExeploreData( lastlevel );
            pbexplore->mutable_npcdata()->clear();

            pbexplore->mutable_playerdata()->set_step( 0 );
            pbexplore->mutable_playerdata()->mutable_fovarr()->Clear();
        }
        else
        {
            kfrecord->RemoveExeploreData( lastlevel );
        }

        auto pbexplore = kfrecord->FindExeploreData( level );
        pbexplore->set_creation( kfexplorelevel->_createion );
        pbexplore->set_innerworld( kfexplorelevel->_inner_world );
        InitExploreData( pbexplore, kfexplorelevel->_explore_id, ( uint32 )kfsetting->_levels.size(), level, lastlevel );

        return std::make_tuple( KFMsg::Ok, kfrecord, pbexplore );
    }

    std::tuple<uint32, KFExploreRecord*, KFMsg::PBExploreData*> KFExploreModule::ExtendEnterExplore( KFEntity* player, uint32 mapid )
    {
        auto kfrecord = _explore_record.Find( player->GetKeyID() );
        if ( kfrecord == nullptr )
        {
            return std::make_tuple( KFMsg::ExploreNotInStatus, nullptr, nullptr );
        }

        // 计算裂隙层的层数
        auto extendlevel = kfrecord->_data.extendlevel() + 1;
        kfrecord->_data.set_extendlevel( extendlevel );

        auto lastlevel = kfrecord->_data.level();
        kfrecord->_data.set_level( extendlevel );
        ////////////////////////////////////////////////////////////////////////////////
        auto pbexplore = kfrecord->FindExeploreData( extendlevel );
        InitExploreData( pbexplore, mapid, 0u, extendlevel, lastlevel );
        return std::make_tuple( KFMsg::Ok, kfrecord, pbexplore );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleExploreTownReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExploreTownReq );

        // 逃跑结算
        ExploreBalance( player, KFMsg::Town );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleExploreExitReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExploreExitReq );

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

        player->SetStatus( kfrecord->_data.status() );
        switch ( result )
        {
        case KFMsg::Victory:
            ExploreBalanceVictory( player, kfrecord );
            break;
        case KFMsg::Failed:
            ExploreBalanceFailed( player, kfrecord );
            break;
        case KFMsg::Flee:
            ExploreBalanceFlee( player, kfrecord );
            break;
        case KFMsg::Town:
            ExploreBalanceTown( player, kfrecord );
            break;
        }

        // 发送消息
        KFMsg::MsgExploreBalanceAck ack;
        ack.set_result( result );
        kfrecord->BalanceRecord( ack.mutable_balance(), KFMsg::ExploreStatus );
        _kf_player->SendToClient( player, KFMsg::MSG_EXPLORE_BALANCE_ACK, &ack, 10u );
    }

    void KFExploreModule::ExploreBalanceVictory( KFEntity* player, KFExploreRecord* kfrecord )
    {
        // 先结算货币
        kfrecord->BalanceCurrencyEndData( player );

        // 结算掉落
        ExploreBalanceDrop( player, kfrecord, KFMsg::Victory );

        // 结算最终数据
        kfrecord->BalanceHeroEndData( player );
        kfrecord->BalanceItemEndData( player );

        // 设置回调属性
        ExploreBalanceResultCondition( player, kfrecord, KFMsg::Victory );

        // 清空探索数据
        ExploreBalanceClearData( player );
    }

    void KFExploreModule::ExploreBalanceFailed( KFEntity* player, KFExploreRecord* kfrecord )
    {
        // 先结算货币
        kfrecord->BalanceCurrencyEndData( player );

        // 结算掉落
        ExploreBalanceDrop( player, kfrecord, KFMsg::Failed );

        // 随机探索失败获得道具
        RandExploreFailedItems( player );

        // 结算最终数据
        kfrecord->BalanceHeroEndData( player );
        kfrecord->BalanceItemEndData( player );

        // 设置回调属性
        ExploreBalanceResultCondition( player, kfrecord, KFMsg::Failed );

        // 清空探索数据
        ExploreBalanceClearData( player );
    }

    void KFExploreModule::ExploreBalanceFlee( KFEntity* player, KFExploreRecord* kfrecord )
    {
        // 先结算
        kfrecord->BalanceEndData( player );

        // 清空探索数据
        ExploreBalanceClearData( player );
    }

    void KFExploreModule::ExploreBalanceTown( KFEntity* player, KFExploreRecord* kfrecord )
    {
        player->UpdateData( __STRING__( exploretown ), KFEnum::Set, 1u );
    }

    void KFExploreModule::ExploreBalanceDrop( KFEntity* player, KFExploreRecord* kfrecord, uint32 result )
    {
        auto kfsetting = KFExploreConfig::Instance()->FindExploreLevel( kfrecord->_data.id(), kfrecord->_data.level() );
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
            _kf_drop->Drop( player, kfsetting->_victory_drop_list, __STRING__( balance ), __FUNC_LINE__ );
            break;
        case KFMsg::Failed:
            _kf_drop->Drop( player, kfsetting->_fail_drop_list, __STRING__( balance ), __FUNC_LINE__ );
            break;
        }

        // 掉落显示
        kfrecord->BalanceDrop( player );
    }

    void KFExploreModule::ExploreBalanceClearData( KFEntity* player )
    {
        // 清除纪录
        _explore_record.Remove( player->GetKeyID() );

        // 探索纪录id
        player->Set( __STRING__( random ), KFEnum::Set, 0 );
        player->Set( __STRING__( explorerecord ), _invalid_string );
        player->UpdateData( __STRING__( mapid ), KFEnum::Set, 0u );

        // 清除探索内道具背包
        _kf_item->CleanItem( player, __STRING__( other ), true );
        _kf_item->CleanItem( player, __STRING__( explore ), true );

        // 探索结束后就移除寿命不足的英雄
        _kf_hero_team->RemoveTeamHeroDurability( player );

        // 清空队伍英雄ep
        _kf_hero_team->ClearTeamHeroEp( player );
    }

    void KFExploreModule::ExploreBalanceResultCondition( KFEntity* player, KFExploreRecord* kfrecord, uint32 result )
    {
        auto kfbalance = player->Find( __STRING__( balance ) );
        kfbalance->Set( __STRING__( id ), kfrecord->_data.id() );

        auto teamheronum = _kf_hero_team->GetHeroCount( player );
        player->UpdateData( kfbalance, __STRING__( team ), KFEnum::Set, teamheronum );

        auto teamdeathnum = _kf_hero_team->GetDeadHeroCount( player );
        player->UpdateData( kfbalance, __STRING__( death ), KFEnum::Set, teamdeathnum );

        auto usetime = KFGlobal::Instance()->_real_time - kfrecord->_data.starttime() + kfrecord->_data.usetime();
        player->UpdateData( kfbalance, __STRING__( time ), KFEnum::Set, usetime );

        player->UpdateData( kfbalance, __STRING__( exploreresult ), KFEnum::Set, result );
    }

    void KFExploreModule::RandExploreFailedItems( KFEntity* player )
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
    KFrame::KFExploreRecord* KFExploreModule::GetExploreRecord( uint64 keyid )
    {
        return _explore_record.Find( keyid );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFExploreModule::OnLeaveSaveExploreRecord )
    {
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
                kfrecord->AddBuffData( uuid, value );
                break;
            case KFEnum::Dec:
                kfrecord->RemoveBuffData( uuid, value );
                break;
            }
        }

        if ( ack.bufflist_size() > 0 )
        {
            _kf_player->SendToClient( player, KFMsg::MSG_UPDATE_EXPLORE_BUFF_ACK, &ack );
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

        auto pbexplore = kfrecord->FindExeploreData( kfrecord->_data.level() );
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
                player->RemoveElement( &_item_element, __STRING__( explore ), __FUNC_LINE__, havenum );

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
            player->RemoveElement( &_item_element, __STRING__( explore ), __FUNC_LINE__, neednum );
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

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleExploreDropReq )
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

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleUpdateFaithReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateFaithReq );

        auto kfrecord = _explore_record.Find( player->GetKeyID() );
        if ( kfrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ExploreNotInStatus );
        }

        kfrecord->_data.set_faith( kfmsg.faith() );
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