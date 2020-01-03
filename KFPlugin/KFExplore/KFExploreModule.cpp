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
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_REQ, &KFExploreModule::HandleExploreReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXIT_EXPLORE_REQ, &KFExploreModule::HandleExploreExitReq );
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
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXIT_EXPLORE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_BALANCE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_PLAYER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_EXPLORE_NPC_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_DROP_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_FAITH_REQ );
        __UN_MESSAGE__( KFMsg::MSG_INTERACT_ITEM_REQ );
    }

    __KF_EXECUTE_FUNCTION__( KFExploreModule::OnExecuteExplore )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute explore param size<1" );
            return false;
        }

        auto mapid = executedata->_param_list._params[ 0 ]->_int_value;
        if ( mapid == 0u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "execute explore mapid=[{}]", mapid );
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
            // 进入新的探索地图
            if ( !kfsetting->_consume.IsEmpty() )
            {
                auto dataname = player->CheckRemoveElement( &kfsetting->_consume, __FUNC_LINE__ );
                if ( !dataname.empty() )
                {
                    return KFMsg::DataNotEnough;
                }

                player->RemoveElement( &kfsetting->_consume, __STRING__( explore ), __FUNC_LINE__ );
            }

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

        __LOG_DEBUG__( "player=[{}] explore=[{}] balance result=[{}]", player->GetKeyID(), kfrecord->_data.id(), result );

        // 先结算货币
        kfrecord->BalanceCurrencyEndData( player );

        auto kfsetting = KFExploreConfig::Instance()->FindSetting( kfrecord->_data.id() );
        if ( kfsetting != nullptr )
        {
            // 把原来的显示同步到客户端
            player->ShowElementToClient();
            switch ( result )
            {
            case KFMsg::Victory:
                _kf_drop->Drop( player, kfsetting->_victory_drop_list, __STRING__( balance ), __FUNC_LINE__ );
                break;
            case KFMsg::Failed:
                _kf_drop->Drop( player, kfsetting->_fail_drop_list, __STRING__( balance ), __FUNC_LINE__ );
                break;
            }
            kfrecord->BalanceDrop( player );
        }

        if ( result != KFMsg::Victory )
        {
            // 随机探索失败获得道具
            RandExploreFailedItems( player );
        }

        // 结算最终数据
        kfrecord->BalanceHeroEndData( player );
        kfrecord->BalanceItemEndData( player );

        // 发送消息
        KFMsg::MsgExploreBalanceAck ack;
        ack.set_result( result );
        kfrecord->BalanceRecord( ack.mutable_balance(), KFMsg::ExploreStatus );
        _kf_player->SendToClient( player, KFMsg::MSG_EXPLORE_BALANCE_ACK, &ack, 10u );

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

            player->UpdateData( kfbalance, __STRING__( team ), KFEnum::Set, teamheronum );
            player->UpdateData( kfbalance, __STRING__( death ), KFEnum::Set, teamdeathnum );

            auto usetime = KFGlobal::Instance()->_real_time - kfrecord->_data.starttime() + kfrecord->_data.usetime();
            player->UpdateData( kfbalance, __STRING__( time ), KFEnum::Set, usetime );

            player->UpdateData( kfbalance, __STRING__( exploreresult ), KFEnum::Set, result );
        }

        // 清除纪录
        _explore_record.Remove( player->GetKeyID() );

        // 探索结束后就移除寿命不足
        _kf_hero_team->RemoveTeamHeroDurability( player );

        // 清空队伍英雄ep
        _kf_hero_team->ClearTeamHeroEp( player );

    }

    void KFExploreModule::RandExploreFailedItems( KFEntity* player )
    {
        UInt64Set destory_item_set;

        static auto _option = _kf_option->FindOption( __STRING__( explorebreakoffprob ) );

        auto kfitemrecord = player->Find( __STRING__( other ) );
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto itemuuid = kfitem->Get<uint64>( __STRING__( uuid ) );

            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
            if ( rand >= _option->_uint32_value )
            {
                destory_item_set.insert( itemuuid );
            }
        }

        for ( auto iter : destory_item_set )
        {
            player->RemoveData( kfitemrecord, iter );
        }
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

            __LOG_ERROR__( "player=[{}] parse explorerecord failed", player->GetKeyID() );
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

        auto exploredata = kfrecord->_data.mutable_exploredata();
        exploredata->set_faith( kfmsg.faith() );
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