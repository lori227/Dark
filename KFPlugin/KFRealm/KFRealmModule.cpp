#include "KFRealmModule.hpp"

namespace KFrame
{
    void KFRealmModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_BEFORE_ENTER_PLAYER__( &KFRealmModule::OnEnterInitRealmData );
        __REGISTER_LEAVE_PLAYER__( &KFRealmModule::OnLeaveSaveRealmData );
        __REGISTER_REALM_MOVE__( &KFRealmModule::OnReamlMoveCostFood );
        __REGISTER_EXECUTE__( __STRING__( realm ), &KFRealmModule::OnExecuteRealm );
        __REGISTER_EXECUTE__( __STRING__( logicevent ), &KFRealmModule::OnExecuteLogicEvent );
        __REGISTER_DROP_LOGIC__( __STRING__( logicevent ), &KFRealmModule::OnDropLogicEvent );

        __REGISTER_UPDATE_DATA_2__( __STRING__( item ), __STRING__( count ), &KFRealmModule::OnRealmUseItem );
        __REGISTER_REMOVE_DATA_1__( __STRING__( item ), &KFRealmModule::OnRealmRemoveItem );

        __REGISTER_ELEMENT_RESULT__( __STRING__( realmdrop ), &KFRealmModule::OnRealmDropElementResult );
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
        __REGISTER_MESSAGE__( KFMsg::MSG_INTERACT_ITEM_REQ, &KFRealmModule::HandleInteractItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_EVENT_REQ, &KFRealmModule::HandleUpdateExploreEventReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REALM_LEVEL_FINISH_REQ, &KFRealmModule::HandleRealmLevelFinishReq );
    }

    void KFRealmModule::BeforeShut()
    {
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        __UN_REALM_MOVE__();
        __UN_EXECUTE__( __STRING__( realm ) );
        __UN_EXECUTE__( __STRING__( logicevent ) );
        __UN_DROP_LOGIC__( __STRING__( logicevent ) );

        __UN_UPDATE_DATA_2__( __STRING__( item ), __STRING__( count ) );
        __UN_REMOVE_DATA_1__( __STRING__( item ) );
        __UN_ELEMENT_RESULT__( __STRING__( realmdrop ) );
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
        __UN_MESSAGE__( KFMsg::MSG_INTERACT_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_EVENT_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REALM_LEVEL_FINISH_REQ );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRealmModule::BindRealmEnterFunction( const std::string& module, KFRealmEnterFunction& function )
    {
        auto kffunction = _realm_enter_function.Create( module );
        kffunction->_function = function;
    }

    void KFRealmModule::UnBindRealmEnterFunction( const std::string& module )
    {
        _realm_enter_function.Remove( module );
    }

    void KFRealmModule::BindRealmMoveFunction( const std::string& module, KFRealmMoveFunction& function )
    {
        auto kffunction = _realm_move_function.Create( module );
        kffunction->_function = function;
    }

    void KFRealmModule::UnBindRealmMoveFunction( const std::string& module )
    {
        _realm_move_function.Remove( module );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
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

    __KF_EXECUTE_FUNCTION__( KFRealmModule::OnExecuteLogicEvent )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "logicevent execute param size<1" );
            return false;
        }

        auto& eventlist = executedata->_param_list._params[0]->_vector_value;
        SendLogicEventToClient( player, eventlist );
        return true;
    }

    __KF_DROP_LOGIC_FUNCTION__( KFRealmModule::OnDropLogicEvent )
    {
        SendLogicEventToClient( player, dropdata->_data_key );
    }

    void KFRealmModule::SendLogicEventToClient( KFEntity* player, uint32 eventid )
    {
        UInt32Vector eventlist;
        eventlist.push_back( eventid );
        return SendLogicEventToClient( player, eventlist );
    }

    void KFRealmModule::SendLogicEventToClient( KFEntity* player, const UInt32Vector& eventlist )
    {
        KFMsg::MsgLogicEventAck ack;
        for ( auto eventid : eventlist )
        {
            ( *ack.mutable_id() )[ack.id_size() + 1] = eventid;
        }
        _kf_player->SendToClient( player, KFMsg::MSG_LOGIC_EVENT_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleRealmLevelFinishReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRealmLevelFinishReq );

        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInStatus );
        }

        // 直接使用leave条件来当完成条件( 策划说不要改)
        RealmJumpCondition( player, kfrealmdata->_data.id(), kfrealmdata->_data.level(), 0u );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleRealmEnterReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRealmEnterReq );

        RealmEnter( player, kfmsg.realmid(), 1u, kfmsg.type(), _invalid_string, _invalid_int );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleRealmJumpReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRealmJumpReq );

        auto realmid = player->Get<uint32>( __STRING__( realmid ) );
        RealmEnter( player, realmid, kfmsg.level(), KFMsg::EnterJump, _invalid_string, kfmsg.birthplace() );
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
            std::tie( result, kfrealmdata, pbexplore ) = RealmJumpEnter( player, realmid, level, moduleid );
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
        kfrealmdata->_is_in_balance = false;

        // 进入秘境回调
        for ( auto& iter : _realm_enter_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player, kfrealmdata, entertype );
        }

        // 下发给客户端
        KFMsg::MsgRealmEnterAck ack;
        ack.set_entertype( entertype );
        ack.set_realmid( kfrealmdata->_data.id() );
        ack.set_faith( kfrealmdata->_data.faith() );
        ack.set_foodstep( kfrealmdata->_data.foodstep() );
        ack.set_worldflag( kfrealmdata->_data.innerworld() );
        ack.mutable_exploredata()->CopyFrom( *pbexplore );
        ack.mutable_buffdata()->CopyFrom( kfrealmdata->_data.buffdata() );
        *ack.mutable_eventdata() = kfrealmdata->_data.eventdata();
        return _kf_player->SendToClient( player, KFMsg::MSG_REALM_ENTER_ACK, &ack, 1u );
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

        if ( !_kf_hero_team->IsDurabilityEnough( player ) )
        {
            return std::make_tuple( KFMsg::HeroTeamDurabilityNotEnough, nullptr, nullptr );
        }

        // 判断和扣除花费
        auto& dataname = player->RemoveElement( &kfsetting->_consume, _default_multiple, __STRING__( realm ), realmid, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return std::make_tuple( KFMsg::DataNotEnough, nullptr, nullptr );
        }

        if ( kfexplorelevel->_durability != 0u )
        {
            UInt64Set excludelist;
            _kf_hero_team->DecHeroRealmDurability( player, kfexplorelevel->_durability, excludelist );
        }

        // 销毁原来的探索数据, 包括背包
        auto lastrealmid = player->Get<uint32>( __STRING__( realmid ) );
        if ( lastrealmid != 0u && lastrealmid != realmid )
        {
            RealmBalanceClearData( player, KFMsg::Chapter );
        }

        // 保存地图
        player->Set( __STRING__( realmdata ), _invalid_string );
        player->UpdateData( __STRING__( realmtown ), KFEnum::Set, _invalid_int );
        player->UpdateData( __STRING__( realmid ), KFEnum::Set, realmid );
        player->UpdateData( __STRING__( realmlevel ), KFEnum::Set, level );
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
        kfrealmdata->_data.set_totallevel( ( uint32 )kfsetting->_levels.size() );

        // level explore
        auto pbexplore = kfrealmdata->FindExeploreData( level );
        pbexplore->set_creation( kfexplorelevel->_createion );
        pbexplore->set_innerworld( kfexplorelevel->_inner_world );
        pbexplore->set_innerworldparameter( kfexplorelevel->_inner_parameter );
        ExploreInitData( pbexplore, kfexplorelevel->_explore_id, ( uint32 )kfsetting->_levels.size(), level, 0u );

        // 进入关卡条件回调
        RealmJumpCondition( player, realmid, 0u, level );

        kfrealmdata->RecordBeginHeros( player );

        // 纪录背包数据
        kfrealmdata->RecordBeginItems( player );
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
        pbexplore->mutable_npcdata()->clear();
    }

    std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> KFRealmModule::RealmLoginEnter( KFEntity* player, uint32 realmid )
    {
        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            return std::make_tuple( KFMsg::RealmNotInStatus, nullptr, nullptr );
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
        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            return std::make_tuple( KFMsg::RealmNotInExplore, nullptr, nullptr );
        }

        auto realmtown = player->Get<uint32>( __STRING__( realmtown ) );
        if ( realmtown == 0u )
        {
            return std::make_tuple( KFMsg::RealmNotTown, nullptr, nullptr );
        }

        auto kfsetting = KFRealmConfig::Instance()->FindSetting( realmid );
        if ( kfsetting == nullptr )
        {
            return std::make_tuple( KFMsg::RealmMapError, nullptr, nullptr );
        }

        auto kfexplorelevel = kfsetting->FindRealmLevel( kfrealmdata->_data.level() );
        if ( kfexplorelevel == nullptr )
        {
            return std::make_tuple( KFMsg::RealmMapError, nullptr, nullptr );
        }

        if ( !_kf_hero_team->IsDurabilityEnough( player ) )
        {
            return std::make_tuple( KFMsg::HeroTeamDurabilityNotEnough, nullptr, nullptr );
        }

        if ( kfexplorelevel->_durability != 0u )
        {
            // 回城的英雄不需要扣耐久
            UInt64Set excludelist;
            auto townhero = kfrealmdata->_data.townhero();
            for ( auto iter : townhero )
            {
                if ( iter.second == kfrealmdata->_data.level() )
                {
                    excludelist.insert( iter.first );
                }
            }

            _kf_hero_team->DecHeroRealmDurability( player, kfexplorelevel->_durability, excludelist );
        }

        // 设置探索不在回城状态
        player->UpdateData( __STRING__( realmtown ), KFEnum::Set, 0u );

        // 设置开始时间
        kfrealmdata->_data.set_starttime( KFGlobal::Instance()->_real_time );

        kfrealmdata->RecordBeginHeros( player );

        // 纪录初始道具信息
        kfrealmdata->RecordBeginItems( player );

        auto pbexplore = kfrealmdata->FindExeploreData( kfrealmdata->_data.level() );
        return std::make_tuple( KFMsg::Ok, kfrealmdata, pbexplore );
    }

    std::tuple<uint32, KFRealmData*, KFMsg::PBExploreData*> KFRealmModule::RealmJumpEnter( KFEntity* player, uint32 realmid, uint32 level, uint32 birthplace )
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

        if ( kfexplorelevel->_durability != 0u )
        {
            UInt64Set excludelist;
            _kf_hero_team->DecHeroRealmDurability( player, kfexplorelevel->_durability, excludelist );
        }

        // 清空上一次的数据
        auto lastlevel = kfrealmdata->_data.level();
        kfrealmdata->_data.set_level( level );
        if ( lastlevel < KFMsg::ExtendLevel )
        {
            auto pbexplore = kfrealmdata->FindExeploreData( lastlevel );

            pbexplore->set_save( false );
            pbexplore->mutable_npcdata()->clear();
            pbexplore->mutable_playerdata()->Clear();
        }
        else
        {
            kfrealmdata->RemoveExeploreData( lastlevel );
        }

        auto pbexplore = kfrealmdata->FindExeploreData( level );
        pbexplore->set_birthplace( birthplace );
        pbexplore->set_creation( kfexplorelevel->_createion );
        pbexplore->set_innerworld( kfexplorelevel->_inner_world );
        pbexplore->set_innerworldparameter( kfexplorelevel->_inner_parameter );
        ExploreInitData( pbexplore, kfexplorelevel->_explore_id, ( uint32 )kfsetting->_levels.size(), level, lastlevel );

        // 完成/进入关卡条件回调
        RealmJumpCondition( player, kfrealmdata->_data.id(), 0u, level );

        player->UpdateData( __STRING__( realmlevel ), KFEnum::Set, level );
        return std::make_tuple( KFMsg::Ok, kfrealmdata, pbexplore );
    }

    void KFRealmModule::RealmJumpCondition( KFEntity* player, uint32 realmid, uint32 lastlevel, uint32 nowlevel )
    {
        auto kfbalance = player->Find( __STRING__( balance ) );
        kfbalance->Set( __STRING__( id ), realmid );

        // 完成关卡
        if ( lastlevel != 0u )
        {
            kfbalance->Set( __STRING__( level ), lastlevel );
            player->UpdateData( kfbalance, __STRING__( jump ), KFEnum::Set, KFMsg::Leave );
        }

        // 进入关卡
        if ( nowlevel != 0u )
        {
            kfbalance->Set( __STRING__( level ), nowlevel );
            player->UpdateData( kfbalance, __STRING__( jump ), KFEnum::Set, KFMsg::Enter );
        }
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
        kfrealmdata->_data.set_level( extendlevel );

        // 清空上一层的步数
        auto lastlevel = kfrealmdata->_data.level();
        auto lastpbexplore = kfrealmdata->FindExeploreData( lastlevel );
        auto playdata = lastpbexplore->mutable_playerdata();
        playdata->set_step( 0u );

        ////////////////////////////////////////////////////////////////////////////////
        auto pbexplore = kfrealmdata->FindExeploreData( extendlevel );
        ExploreInitData( pbexplore, exploreid, 0u, extendlevel, lastlevel );
        return std::make_tuple( KFMsg::Ok, kfrealmdata, pbexplore );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleRealmTownReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRealmTownReq );

        // 回城结算
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

        kfrealmdata->_is_in_balance = true;

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
        ack.set_level( kfrealmdata->_data.level() );
        ack.set_maxlevel( kfrealmdata->_data.totallevel() );
        kfrealmdata->BalanceRealmRecord( ack.mutable_balance() );
        _kf_player->SendToClient( player, KFMsg::MSG_REALM_BALANCE_ACK, &ack );
    }

    void KFRealmModule::RealmBalanceVictory( KFEntity* player, KFRealmData* kfrealmdata )
    {
        player->SetStatus( kfrealmdata->_data.status() );

        // 纪录英雄数据
        kfrealmdata->RecordPeriodHeros( player );

        // 更新死亡英雄(死亡英雄可获得结算经验)
        _kf_hero_team->UpdateDeadHero( player );

        // 结算掉落
        RealmBalanceDrop( player, kfrealmdata, KFMsg::Victory );

        // 结算最终数据
        kfrealmdata->RecordEndItems( player );
        kfrealmdata->RecordEndHeros( player );

        // 发送消息
        SendRealmBalanceToClient( player, kfrealmdata, KFMsg::Victory );

        // 结果条件
        RealmBalanceResultCondition( player, kfrealmdata, KFMsg::Victory );

        // 设置回调属性
        RealmJumpCondition( player, kfrealmdata->_data.id(), kfrealmdata->_data.level(), 0u );

        // 清空探索数据
        RealmBalanceClearData( player, KFMsg::Victory );
    }

    void KFRealmModule::RealmBalanceFailed( KFEntity* player, KFRealmData* kfrealmdata )
    {
        // 随机探索失败获得道具
        RealmRandFailedItems( player, kfrealmdata );

        // 切换状态
        player->SetStatus( kfrealmdata->_data.status() );

        // 纪录英雄数据
        kfrealmdata->RecordPeriodHeros( player );

        // 更新死亡英雄(死亡英雄可获得结算经验)
        _kf_hero_team->UpdateDeadHero( player );

        // 结算掉落
        RealmBalanceDrop( player, kfrealmdata, KFMsg::Failed );

        // 结算最终数据
        kfrealmdata->RecordEndHeros( player );
        kfrealmdata->RecordEndItems( player );

        // 发送消息
        SendRealmBalanceToClient( player, kfrealmdata, KFMsg::Failed );

        // 结果条件
        RealmBalanceResultCondition( player, kfrealmdata, KFMsg::Failed );

        // 清空探索数据
        RealmBalanceClearData( player, KFMsg::Failed );
    }

    void KFRealmModule::RealmBalanceFlee( KFEntity* player, KFRealmData* kfrealmdata )
    {
        // 随机探索失败获得道具
        RealmRandFailedItems( player, kfrealmdata );

        // 切换状态
        player->SetStatus( kfrealmdata->_data.status() );

        // 纪录英雄数据
        kfrealmdata->RecordPeriodHeros( player );

        // 更新死亡英雄(死亡英雄可获得结算经验)
        _kf_hero_team->UpdateDeadHero( player );

        kfrealmdata->RecordEndHeros( player );

        // 纪录最终道具
        kfrealmdata->RecordEndItems( player );

        // 发送消息
        SendRealmBalanceToClient( player, kfrealmdata, KFMsg::Flee );

        // 结果条件
        RealmBalanceResultCondition( player, kfrealmdata, KFMsg::Flee );

        // 清空探索数据
        RealmBalanceClearData( player, KFMsg::Flee );
    }

    void KFRealmModule::RealmBalanceTown( KFEntity* player, KFRealmData* kfrealmdata )
    {
        // 纪录英雄数据
        kfrealmdata->RecordPeriodHeros( player );

        // 更新死亡英雄(死亡英雄可获得结算经验)
        _kf_hero_team->UpdateDeadHero( player );

        kfrealmdata->RecordEndHeros( player );

        // 结算获得的道具
        kfrealmdata->RecordEndItems( player );

        // 发送消息b
        SendRealmBalanceToClient( player, kfrealmdata, KFMsg::Town );

        // 结果条件
        RealmBalanceResultCondition( player, kfrealmdata, KFMsg::Town );

        // 设置回城状态
        player->SetStatus( KFMsg::TownStatus );
        player->UpdateData( __STRING__( realmtown ), KFEnum::Set, 1u );
        player->UpdateData( __STRING__( realmid ), KFEnum::Set, kfrealmdata->_data.id() );

        // 结算背包
        RealmBalanceItem( player, KFMsg::Town );

        // 重新纪录玩家道具信息
        kfrealmdata->RecordBeginItems( player );

        // 记录回城英雄
        kfrealmdata->RecordTownHeros( player );
    }

    void KFRealmModule::RealmBalanceDrop( KFEntity* player, KFRealmData* kfrealmdata, uint32 result )
    {
        auto kfsetting = KFRealmConfig::Instance()->FindRealmLevel( kfrealmdata->_data.id(), kfrealmdata->_data.level() );
        if ( kfsetting == nullptr )
        {
            return;
        }

        auto& droplist = GetRealmDropList( kfsetting, result );
        if ( !droplist.empty() )
        {
            _kf_drop->Drop( player, droplist, __STRING__( realmdrop ), kfrealmdata->_data.id(), __FUNC_LINE__ );
        }
    }

    __KF_ELEMENT_RESULT_FUNCTION__( KFRealmModule::OnRealmDropElementResult )
    {
        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata != nullptr )
        {
            kfrealmdata->BalanceDropData( kfresult );
        }

        return false;
    }

    const UInt32Vector& KFRealmModule::GetRealmDropList( const KFRealmLevel* kfsetting, uint32 result )
    {
        if ( result == KFMsg::Victory )
        {
            return kfsetting->_victory_drop_list;
        }

        return kfsetting->_fail_drop_list;
    }

    void KFRealmModule::RealmBalanceClearData( KFEntity* player, uint32 result )
    {
        // 清除数据
        _realm_data.Remove( player->GetKeyID() );

        // 清空队伍英雄ep
        _kf_hero_team->ClearHeroEp( player );

        // 清除探索内道具背包
        RealmBalanceItem( player, result );

        // 探索纪录id
        player->Set( __STRING__( random ), KFEnum::Set, 0 );
        player->Set( __STRING__( realmdata ), _invalid_string );
        player->UpdateData( __STRING__( realmtown ), KFEnum::Set, 0u );
        player->UpdateData( __STRING__( realmid ), KFEnum::Set, 0u );
        player->UpdateData( __STRING__( realmlevel ), KFEnum::Set, 0u );
    }

    uint32 KFRealmModule::GetItemBagRealmBalanceType( const KFItemBagSetting* kfsetting, uint32 result )
    {
        switch ( result )
        {
        case KFMsg::Victory:
        case KFMsg::Failed:
        case KFMsg::Flee:
            return kfsetting->_balance_realm_type;
        case KFMsg::Chapter:
            return kfsetting->_enter_realm_type;
        case KFMsg::Town:
            return kfsetting->_town_realm_type;
        }

        return 0u;
    }

    void KFRealmModule::RealmBalanceItem( KFEntity* player, uint32 result )
    {
        for ( auto& iter : KFItemBagConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            auto type = GetItemBagRealmBalanceType( kfsetting, result );
            switch ( type )
            {
            case KFItemBagEnum::RealmClean:
                player->CleanData( kfsetting->_id );
                break;
            case KFItemBagEnum::RealmBalance:
                _kf_item_move->BalanceItem( player, kfsetting->_id, result != KFMsg::Town );
                break;
            }
        }
    }

    void KFRealmModule::RealmBalanceResultCondition( KFEntity* player, KFRealmData* kfrealmdata, uint32 result )
    {
        auto kfbalance = player->Find( __STRING__( balance ) );
        kfbalance->Set( __STRING__( id ), kfrealmdata->_data.id() );
        kfbalance->Set( __STRING__( level ), kfrealmdata->_data.level() );

        auto teamheronum = _kf_hero_team->GetHeroCount( player );
        player->UpdateData( kfbalance, __STRING__( team ), KFEnum::Set, teamheronum );

        auto teamdeathnum = _kf_hero_team->GetDeadHeroCount( player );
        player->UpdateData( kfbalance, __STRING__( death ), KFEnum::Set, teamdeathnum );

        auto usetime = KFGlobal::Instance()->_real_time - kfrealmdata->_data.starttime() + kfrealmdata->_data.usetime();
        player->UpdateData( kfbalance, __STRING__( time ), KFEnum::Set, usetime );

        player->UpdateData( kfbalance, __STRING__( realmresult ), KFEnum::Set, result );
    }

    void KFRealmModule::RealmRandFailedItems( KFEntity* player, KFRealmData* kfrealmdata )
    {
        auto kfrealmsetting = KFRealmConfig::Instance()->FindSetting( kfrealmdata->_data.id() );
        if ( kfrealmsetting == nullptr )
        {
            return;
        }

        kfrealmdata->RecordEndItems( player );
        for ( auto i = 0; i < kfrealmdata->_data.itemdata_size(); ++i )
        {
            auto pbitem = &kfrealmdata->_data.itemdata( i );

            // 获得失败惩罚的丢失比例
            auto weight = kfrealmsetting->GetFunishItemWeight( pbitem->id() );
            if ( weight == 0u )
            {
                continue;
            }

            auto itemcount = kfrealmdata->CalcRealmItemCount( pbitem );
            auto punishcount = std::round( ( double )itemcount * ( double )weight / ( double )KFRandEnum::TenThousand );
            if ( punishcount == 0u )
            {
                continue;
            }

            auto kfitemrecord = _kf_item->FindItemRecord( player, pbitem->id() );
            if ( pbitem->uuid() == 0u )
            {
                // 可堆叠的
                _kf_item->RemoveItem( player, kfitemrecord, pbitem->id(), punishcount );

                // 纪录失去的道具
                auto pblostitem = kfrealmdata->FindLose( pbitem->id(), pbitem->uuid() );
                pblostitem->set_count( pblostitem->count() + punishcount );
            }
            else
            {
                auto ok = player->RemoveData( kfitemrecord, pbitem->uuid() );
                if ( ok )
                {
                    auto pblostitem = kfrealmdata->FindLose( pbitem->id(), pbitem->uuid() );
                    pblostitem->set_count( punishcount );
                }
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFRealmData* KFRealmModule::GetRealmData( KFEntity* player )
    {
        return _realm_data.Find( player->GetKeyID() );
    }

    bool KFRealmModule::IsInnerWorld( KFEntity* player )
    {
        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            return false;
        }

        return kfrealmdata->IsInnerWorld();
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFRealmModule::OnEnterInitRealmData )
    {
        auto strdata = player->Get<std::string>( __STRING__( realmdata ) );
        if ( strdata.empty() )
        {
            player->Set( __STRING__( realmid ), 0u );
            player->Set( __STRING__( realmtown ), 0u );
        }
        else
        {
            auto kfrealmdata = _realm_data.Create( player->GetKeyID() );
            auto ok = kfrealmdata->_data.ParseFromString( strdata );
            if ( !ok )
            {
                player->Set( __STRING__( realmid ), 0u );
                player->Set( __STRING__( realmtown ), 0u );
            }
            else
            {
                auto realmtown = player->Get<uint32>( __STRING__( realmtown ) );
                if ( realmtown == 1u )
                {
                    player->SetStatus( KFMsg::TownStatus );
                }
            }
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFRealmModule::OnLeaveSaveRealmData )
    {
        auto kfrecord = _realm_data.Find( player->GetKeyID() );
        if ( kfrecord != nullptr )
        {
            auto usetime = KFGlobal::Instance()->_real_time - kfrecord->_data.starttime();
            kfrecord->_data.set_usetime( kfrecord->_data.usetime() + usetime );

            auto strdata = kfrecord->_data.SerializeAsString();
            player->Set( __STRING__( realmdata ), strdata );
            player->Set( __STRING__( realmid ), kfrecord->_data.id() );
            _realm_data.Remove( player->GetKeyID() );
        }
        else
        {
            player->Set( __STRING__( realmdata ), _invalid_string );
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
        for ( auto i = 0; i < kfmsg.playerdata().fovarr_size(); ++i )
        {
            auto fov = kfmsg.playerdata().fovarr( i );
            playerdata->add_fovarr( fov );
        }

        // 秘境移动逻辑回调
        if ( kfmsg.playerdata().step() > 0u )
        {
            for ( auto& iter : _realm_move_function._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( player, kfrealmdata, kfmsg.playerdata().step() );
            }
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
        findnpcdata->set_x( pbnpcdata.x() );
        findnpcdata->set_y( pbnpcdata.y() );
        findnpcdata->set_z( pbnpcdata.z() );
        findnpcdata->set_yaw( pbnpcdata.yaw() );
        findnpcdata->set_bkilled( pbnpcdata.bkilled() );
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

        _kf_drop->Drop( player, droplist, __STRING__( exploredrop ), 0u, __FUNC_LINE__ );
    }

    double KFRealmModule::GetAddHpRate( KFEntity* player )
    {
        double rate = 1.0;

        auto realmtown = player->Get<uint32>( __STRING__( realmtown ) );
        if ( realmtown > 0u )
        {
            return rate;
        }

        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr || !kfrealmdata->IsInnerWorld() )
        {
            return rate;
        }

        auto kfrealmsetting = FindRealmLevelSetting( player );
        if ( kfrealmsetting == nullptr )
        {
            return rate;
        }

        auto kfworldsetting = KFInnerWorldConfig::Instance()->FindSetting( kfrealmsetting->_inner_world );
        if ( kfworldsetting == nullptr )
        {
            return rate;
        }

        rate -= static_cast<double>( kfworldsetting->_reduced_treatment ) / static_cast<double>( KFRandEnum::TenThousand );
        return rate;
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleInteractItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgInteractItemReq );

        auto kfinteraction = player->Find( __STRING__( interaction ) );
        kfinteraction->Reset();

        kfinteraction->Set( __STRING__( field ), kfmsg.field() );
        player->UpdateData( kfinteraction, __STRING__( id ), KFEnum::Set, kfmsg.itemid() );
    }

    __KF_MESSAGE_FUNCTION__( KFRealmModule::HandleUpdateExploreEventReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateExploreEventReq );

        auto kfrealmdata = _realm_data.Find( playerid );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInStatus );
        }

        auto& eventdata = *kfrealmdata->_data.mutable_eventdata();
        for ( auto iter : kfmsg.eventdata() )
        {
            eventdata[iter.first] = iter.second;
        }
    }

    const KFRealmLevel* KFRealmModule::FindRealmLevelSetting( KFEntity* player )
    {
        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr )
        {
            return nullptr;
        }

        auto pbexplore = kfrealmdata->FindExeploreData( kfrealmdata->_data.level() );
        auto curlevel = pbexplore->level();
        if ( curlevel >= KFMsg::ExtendLevel )
        {
            // 裂隙层取上一层的等级
            curlevel = pbexplore->lastlevel();
        }

        auto kfsetting = KFRealmConfig::Instance()->FindRealmLevel( kfrealmdata->_data.id(), curlevel );
        return kfsetting;
    }

    __KF_REALM_MOVE_FUNCTION__( KFRealmModule::OnReamlMoveCostFood )
    {
        static auto _food_option = KFGlobal::Instance()->FindConstant( __STRING__( fooditemid ) );

        kfrealmdata->_data.set_foodstep( kfrealmdata->_data.foodstep() + movestep );
        auto kfsetting = FindRealmLevelSetting( player );
        if ( kfsetting == nullptr || kfsetting->_food_step == 0u || kfsetting->_hp_step == 0u )
        {
            return;
        }

        // 拥有粮食数量
        auto kfitemrecord = player->Find( __STRING__( explore ) );
        auto havenum = _kf_item->GetItemCount( player, kfitemrecord, _food_option->_uint32_value, 1u );
        if ( havenum > 0u )
        {
            auto count = kfrealmdata->_data.foodstep() / kfsetting->_food_step;
            if ( count != 0u )
            {
                kfrealmdata->_data.set_foodstep( kfrealmdata->_data.foodstep() - count * kfsetting->_food_step );

                // 需要粮食数量
                auto neednum = kfsetting->_food_num * count;
                _kf_item->RemoveItem( player, kfitemrecord, _food_option->_uint32_value, neednum );
            }
        }
        else
        {
            auto count = kfrealmdata->_data.foodstep() / kfsetting->_hp_step;
            if ( count != 0u )
            {
                kfrealmdata->_data.set_foodstep( kfrealmdata->_data.foodstep() - count * kfsetting->_hp_step );

                auto dechp = kfsetting->_hp_num * count;
                _kf_hero_team->OperateHpValue( player, KFEnum::Dec, dechp );
            }
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRealmModule::OnRealmUseItem )
    {
        // 只记录扣除
        if ( operate != KFEnum::Dec )
        {
            return;
        }

        auto kfitem = kfdata->GetParent();
        auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
        AddRealmUseItemCount( player, kfitem->_data_setting->_name, itemid, 0u, value );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFRealmModule::OnRealmRemoveItem )
    {
        auto itemcount = kfdata->Get<uint32>( __STRING__( count ) );
        if ( itemcount == 0u )
        {
            return;
        }

        auto itemid = kfdata->Get<uint32>( kfdata->_data_setting->_config_key_name );
        AddRealmUseItemCount( player, kfdata->_data_setting->_name, itemid, key, itemcount );
    }

    void KFRealmModule::AddRealmUseItemCount( KFEntity* player, const std::string& bagname, uint32 itemid, uint64 uuid, uint32 count )
    {
        auto kfitembagsetting = KFItemBagConfig::Instance()->FindSetting( bagname );
        if ( kfitembagsetting == nullptr || !kfitembagsetting->_is_realm_record )
        {
            return;
        }

        // 判断玩家是否在探索中
        auto kfrealmdata = _realm_data.Find( player->GetKeyID() );
        if ( kfrealmdata == nullptr || kfrealmdata->_is_in_balance )
        {
            return;
        }

        // 回城状态不计数
        auto realmtown = player->Get<uint32>( __STRING__( realmtown ) );
        if ( realmtown == 1u )
        {
            return;
        }

        kfrealmdata->AddRealmUseItemCount( itemid, uuid, count );
    }
}