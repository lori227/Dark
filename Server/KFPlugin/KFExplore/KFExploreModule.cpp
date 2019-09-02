#include "KFExploreModule.hpp"

namespace KFrame
{
    void KFExploreModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        __REGISTER_UPDATE_DATA_2__( __KF_STRING__( fighter ), __KF_STRING__( hp ), &KFExploreModule::OnHeroHpUpdate );
        __REGISTER_LEAVE_PLAYER__( &KFExploreModule::OnLeavePlayerSaveExplore );

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_REQ, &KFExploreModule::HandleExploreReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_FIGHTER_REQ, &KFExploreModule::HandleFighterReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_PVE_BALANCE_REQ, &KFExploreModule::HandlePVEBalanceReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXIT_EXPLORE_REQ, &KFExploreModule::HandleExploreExitReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_FIGHTER_REQ, &KFExploreModule::HandleUpdateFighterReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXPLORE_BALANCE_REQ, &KFExploreModule::HandleExploreBalanceReq );
    }

    void KFExploreModule::BeforeShut()
    {
        __UN_LEAVE_PLAYER__();
        __UN_UPDATE_DATA_2__( __KF_STRING__( fighter ), __KF_STRING__( hp ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_FIGHTER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_PVE_BALANCE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXIT_EXPLORE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_FIGHTER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXPLORE_BALANCE_REQ );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFExploreModule::OnHeroHpUpdate )
    {
        if ( newvalue > 0u )
        {
            return;
        }

        player->RemoveData( __KF_STRING__( hero ), key );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleExploreReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExploreReq );

        auto kfsetting = KFExploreConfig::Instance()->FindSetting( kfmsg.mapid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ExploreMapError );
        }

        auto kfobject = player->GetData();
        auto mapid = kfobject->GetValue<std::string>( __KF_STRING__( mapid ) );
        if ( !mapid.empty() )
        {
            if ( mapid != kfmsg.mapid() )
            {
                return _kf_display->SendToClient( player, KFMsg::ExploreMapLimit );
            }
        }
        else
        {
            player->UpdateData( __KF_STRING__( mapid ), kfmsg.mapid() );
            player->UpdateData( __KF_STRING__( random ), KFEnum::Set, KFGlobal::Instance()->RandRatio( ( uint32 )__MAX_INT32__ ) );
        }

        KFMsg::MsgExploreAck ack;
        ack.set_mapid( kfmsg.mapid() );
        ack.set_random( kfobject->GetValue<uint32>( __KF_STRING__( random ) ) );
        ack.set_data( kfobject->GetValue<std::string>( __KF_STRING__( explore ) ) );
        _kf_player->SendToClient( player, KFMsg::MSG_EXPLORE_ACK, &ack );

        InitExploreRecord( player );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( status ), KFEnum::Set, KFMsg::ExploreStatus );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleFighterReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgFighterReq );

        auto kfobject = player->GetData();
        auto status = kfobject->GetValue( __KF_STRING__( basic ), __KF_STRING__( status ) );
        if ( status != KFMsg::ExploreStatus )
        {
            return _kf_display->SendToClient( player, KFMsg::FighterNotInExplore );
        }

        KFMsg::MsgFighterAck ack;

        // team
        auto kfherorecord = kfobject->FindData( __KF_STRING__( hero ) );
        auto kfteamarray = kfobject->FindData( __KF_STRING__( heroteam ) );
        for ( uint32 i = KFDataDefine::Array_Index; i < kfteamarray->Size(); i++ )
        {
            auto kfteam = kfteamarray->FindData( i );
            if ( kfteam == nullptr )
            {
                continue;
            }

            auto uuid = kfteam->GetValue<uint64>();
            auto kfhero = kfherorecord->FindData( uuid );
            if ( kfhero == nullptr )
            {
                continue;
            }

            auto& pbobject = ( *ack.mutable_hero() )[ uuid ];
            auto pbhero = _kf_kernel->SerializeToClient( kfhero );
            pbobject.CopyFrom( *pbhero );
        }

        // npc
        for ( auto i = 0; i < kfmsg.generate_size(); ++i )
        {
            auto generateid = kfmsg.generate( i );
            auto kfnpc = _kf_generate->GenerateNpcHero( player, generateid, kfmsg.level(), __FUNC_LINE__ );
            if ( kfnpc == nullptr )
            {
                continue;
            }

            auto& pbobject = ( *ack.mutable_npc() )[ kfnpc->GetKeyID() ];
            auto pbhero = _kf_kernel->SerializeToClient( kfnpc );
            pbobject.CopyFrom( *pbhero );
        }

        _kf_player->SendToClient( player, KFMsg::MSG_FIGHTER_ACK, &ack );

        // 重置战斗统计数据
        _explore_record.Create( playerid )->_fight_balance.Clear();
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( status ), KFEnum::Set, KFMsg::PVEStatus );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandleUpdateFighterReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateFighterReq );

        auto kfherorecord = player->GetData()->FindData( __KF_STRING__( hero ) );

        for ( auto i = 0; i < kfmsg.data_size(); ++i )
        {
            auto pbdata = &kfmsg.data( i );
            auto kfhero = kfherorecord->FindData( pbdata->uuid() );
            if ( kfhero == nullptr )
            {
                continue;
            }

            // 更新hp
            auto kffighter = kfhero->FindData( __KF_STRING__( fighter ) );
            auto hp = player->UpdateData( pbdata->uuid(), kffighter, __KF_STRING__( hp ), KFEnum::Set, pbdata->hp() );
            if ( hp == 0u )
            {
                continue;
            }

            //  更新exp
            if ( pbdata->exp() > 0u )
            {
                auto newlevel = ExploreAddHeroExp( player, kfhero, pbdata->exp(), KFExploreRecord::TypeAll );
                if ( newlevel != 0u )
                {
                    // 更新到战斗
                    SendDataToFighter( player, pbdata->uuid(), __KF_STRING__( level ), newlevel );
                }
            }
        }
    }

    void KFExploreModule::SendDataToFighter( KFEntity* player, uint64 uuid, const std::string& name, uint32 value )
    {
        KFMsg::MsgSyncFighterData sync;
        sync.set_uuid( uuid );
        sync.set_dataname( name );
        sync.set_value( value );
        _kf_player->SendToClient( player, KFMsg::MSG_SYNC_FIGHTER_DATA, &sync );
    }

    __KF_MESSAGE_FUNCTION__( KFExploreModule::HandlePVEBalanceReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgPVEBalanceReq );

        player->UpdateData( __KF_STRING__( explore ), kfmsg.data() );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( status ), KFEnum::Set, KFMsg::ExploreStatus );

        // 计算掉落
        if ( kfmsg.dropid() != 0u )
        {
            ExploreDropElement( player, kfmsg.dropid(), KFExploreRecord::TypeAll );
        }

        // 发送给客户端
        KFMsg::MsgPVEBalanceAck ack;
        ack.set_result( kfmsg.result() );
        ack.mutable_balance()->Swap( &_explore_record.Create( player->GetKeyID() )->_fight_balance );
        _kf_player->SendToClient( player, KFMsg::MSG_PVE_BALANCE_ACK, &ack );
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

        auto kfobject = player->GetData();
        auto mapid = kfobject->GetValue<std::string>( __KF_STRING__( mapid ) );
        auto kfsetting = KFExploreConfig::Instance()->FindSetting( mapid );
        if ( kfsetting != nullptr )
        {
            // 获得道具
            auto itemdropid = ( kfmsg.result() == KFMsg::Victory ? kfsetting->_victory_item_drop_id : kfsetting->_fail_item_drop_id );
            if ( itemdropid != 0u )
            {
                ExploreDropElement( player, itemdropid, KFExploreRecord::TypeExplore );
            }

            // 英雄经验
            auto expdropid = ( kfmsg.result() == KFMsg::Victory ? kfsetting->_victory_exp_drop_id : kfsetting->_fail_exp_drop_id );
            if ( expdropid != 0u )
            {
                ExploreDropHeroData( player, expdropid, KFExploreRecord::TypeExplore );
            }
        }

        // 探索结算
        ExploreBalance( player, kfmsg.result() );
    }

    void KFExploreModule::ExploreBalance( KFEntity* player, uint32 result )
    {
        // 清除探索内道具背包
        auto kfobject = player->GetData();
        kfobject->SetValue( __KF_STRING__( mapid ), _invalid_str );
        kfobject->SetValue( __KF_STRING__( explore ), _invalid_str );
        kfobject->SetValue( __KF_STRING__( random ), KFEnum::Set, 0 );
        kfobject->SetValue( __KF_STRING__( explorerecord ), _invalid_str );

        player->RemoveData( __KF_STRING__( other ) );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( status ), KFEnum::Set, KFMsg::OnlineStatus );

        // 发送消息
        KFMsg::MsgExploreBalanceAck ack;
        ack.set_result( result );
        ack.mutable_balance()->Swap( &_explore_record.Create( player->GetKeyID() )->_explore_balance );
        _kf_player->SendToClient( player, KFMsg::MSG_EXPLORE_BALANCE_ACK, &ack );

        // 清除纪录
        _explore_record.Remove( player->GetKeyID() );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFExploreModule::InitExploreRecord( KFEntity* player )
    {
        auto kfrecord = _explore_record.Create( player->GetKeyID() );
        kfrecord->_fight_balance.Clear();

        if ( kfrecord->_id == 0u )
        {
            kfrecord->_id = player->GetKeyID();
            auto record = player->GetData()->GetValue<std::string>( __KF_STRING__( explorerecord ) );
            if ( !record.empty() )
            {
                kfrecord->_explore_balance.ParseFromString( record );
            }
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFExploreModule::OnLeavePlayerSaveExplore )
    {
        auto kfrecord = _explore_record.Find( player->GetKeyID() );
        if ( kfrecord != nullptr )
        {
            auto strbalance = kfrecord->_explore_balance.SerializeAsString();
            player->GetData()->SetValue( __KF_STRING__( explorerecord ), strbalance );
            _explore_record.Remove( player->GetKeyID() );
        }
        else
        {
            player->GetData()->SetValue( __KF_STRING__( explorerecord ), _invalid_str );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFExploreRecord::AddShowData( uint32 recordtype, const KFDropData* dropdata )
    {
        for ( auto kfelement : dropdata->_elements._element_list )
        {
            if ( KFUtility::HaveBitMask( recordtype, ( uint32 )KFExploreRecord::TypeFight ) )
            {
                auto pbshowdata = FindShowData( &_fight_balance, dropdata->_data_name, dropdata->_data_key );
                AddShowData( pbshowdata, kfelement );
            }

            if ( KFUtility::HaveBitMask( recordtype, ( uint32 )KFExploreRecord::TypeExplore ) )
            {
                auto pbshowdata = FindShowData( &_explore_balance, dropdata->_data_name, dropdata->_data_key );
                AddShowData( pbshowdata, kfelement );
            }
        }
    }

    void KFExploreRecord::AddShowData( KFMsg::PBShowData* showdata, const KFElement* kfelement )
    {
        if ( kfelement->IsValue() )
        {
            auto kfelementvalue = reinterpret_cast< const KFElementValue* >( kfelement );
            showdata->set_name( kfelementvalue->_data_name );
            showdata->set_value( kfelementvalue->_value->GetUseValue() );
        }
        else if ( kfelement->IsObject() )
        {
            auto kfelementobject = reinterpret_cast< const KFElementObject* >( kfelement );
            showdata->set_name( kfelementobject->_data_name );
            showdata->set_key( kfelementobject->_config_id );
            for ( auto& iter : kfelementobject->_values._objects )
            {
                auto kfvalue = iter.second;
                ( *showdata->mutable_pbuint64() )[ iter.first ] = kfvalue->GetUseValue();
            }
        }
    }

    void KFExploreRecord::AddHeroExpRecord( uint32 recordtype, uint64 uuid, uint32 exp, uint32 levelup )
    {
        if ( KFUtility::HaveBitMask( recordtype, ( uint32 )KFExploreRecord::TypeFight ) )
        {
            auto pbbalancehero = FindBalanceHero( &_fight_balance, uuid );
            AddHeroExpRecord( pbbalancehero, exp, levelup );
        }

        if ( KFUtility::HaveBitMask( recordtype, ( uint32 )KFExploreRecord::TypeExplore ) )
        {
            auto pbbalancehero = FindBalanceHero( &_explore_balance, uuid );
            AddHeroExpRecord( pbbalancehero, exp, levelup );
        }
    }

    void KFExploreRecord::AddHeroExpRecord( KFMsg::PBBalanceHero* balancehero, uint32 exp, uint32 levelup )
    {
        balancehero->set_exp( balancehero->exp() + exp );
        if ( levelup > 0u )
        {
            balancehero->set_levelup( balancehero->levelup() + levelup );
        }
    }

    KFMsg::PBShowData* KFExploreRecord::FindShowData( KFMsg::PBBalanceData* balance, const std::string& name, uint64 id )
    {
        for ( auto i = 0; i < balance->showdata_size(); ++i )
        {
            auto pbshowdata = balance->mutable_showdata( i );
            if ( pbshowdata->name() == name && pbshowdata->key() == id )
            {
                return pbshowdata;
            }
        }

        auto pbshowdata = balance->add_showdata();
        pbshowdata->set_name( name );
        pbshowdata->set_key( id );
        return pbshowdata;
    }

    KFMsg::PBBalanceHero* KFExploreRecord::FindBalanceHero( KFMsg::PBBalanceData* balance, uint64 uuid )
    {
        for ( auto i = 0; i < balance->herodata_size(); ++i )
        {
            auto pbhero = balance->mutable_herodata( i );
            if ( pbhero->uuid() == uuid )
            {
                return pbhero;
            }
        }

        auto pbhero = balance->add_herodata();
        pbhero->set_uuid( uuid );
        return pbhero;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFExploreModule::ExploreAddHeroExp( KFEntity* player, KFData* kfhero, uint32 exp, uint32 recordtype )
    {
        auto kflevel = kfhero->FindData( __KF_STRING__( level ) );
        auto oldlevel = kflevel->GetValue();
        exp = _kf_hero->AddExp( player, kfhero, exp );
        auto newlevel = kflevel->GetValue();

        // 保存纪录
        auto kfrecord = _explore_record.Create( player->GetKeyID() );
        kfrecord->AddHeroExpRecord( recordtype, kfhero->GetKeyID(), exp, newlevel - oldlevel );
        return newlevel > oldlevel ? newlevel : 0u;
    }

    void KFExploreModule::ExploreDropHeroData( KFEntity* player, uint32 dropid, uint32 recordtype )
    {
        auto& droplist = _kf_drop->DropLogic( player, dropid, 1u, __FUNC_LINE__ );

        auto kfobject = player->GetData();
        auto kfteamarray = kfobject->FindData( __KF_STRING__( heroteam ) );
        auto kfherorecord = kfobject->FindData( __KF_STRING__( hero ) );

        for ( uint32 i = KFDataDefine::Array_Index; i < kfteamarray->Size(); ++i )
        {
            auto kfteam = kfteamarray->FindData( i );
            if ( kfteam == nullptr )
            {
                continue;
            }

            auto uuid = kfteam->GetValue<uint64>();
            auto kfhero = kfherorecord->FindData( uuid );
            if ( kfhero == nullptr )
            {
                continue;
            }

            for ( auto dropdata : droplist )
            {
                if ( dropdata->_data_name == __KF_STRING__( exp ) )
                {
                    ExploreAddHeroExp( player, kfhero, dropdata->GetValue(), recordtype );
                }
            }
        }
    }

    void KFExploreModule::ExploreDropElement( KFEntity* player, uint32 dropid, uint32 recordtype )
    {
        auto kfrecord = _explore_record.Create( player->GetKeyID() );
        auto& droplist = _kf_drop->DropElement( player, dropid, 1u, false, __FUNC_LINE__ );
        for ( auto dropdata : droplist )
        {
            kfrecord->AddShowData( recordtype, dropdata );
        }
    }
}