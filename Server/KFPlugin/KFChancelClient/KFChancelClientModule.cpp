#include "KFChancelClientModule.hpp"


namespace KFrame
{
    void KFChancelClientModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );

        __REGISTER_ADD_DATA__( __KF_STRING__( hero ), &KFChancelClientModule::OnAddHero );
        __REGISTER_REMOVE_DATA__( __KF_STRING__( hero ), &KFChancelClientModule::OnRemoveHero );

        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_DEAD_RECORD_REQ, &KFChancelClientModule::HandleQueryDeadRecordReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_LIFE_EVENT_REQ, &KFChancelClientModule::HandleQueryLifeEventReq );

        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_DEAD_RECORD_ACK, &KFChancelClientModule::HandleQueryDeadRecordAck );
    }

    void KFChancelClientModule::BeforeShut()
    {
        __UN_ADD_DATA__( __KF_STRING__( hero ) );
        __UN_REMOVE_DATA__( __KF_STRING__( hero ) );

        __UN_MESSAGE__( KFMsg::MSG_QUERY_DEAD_RECORD_REQ );
        __UN_MESSAGE__( KFMsg::MSG_QUERY_LIFE_EVENT_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_DEAD_RECORD_ACK );
    }

    __KF_ADD_DATA_FUNCTION__( KFChancelClientModule::OnAddHero )
    {
        // 添加英雄出生事件
        AddLifeEvent( player, key, KFMsg::HeroBorn );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFChancelClientModule::OnRemoveHero )
    {
        auto hp = kfdata->GetValue<uint32>( __KF_STRING__( hp ) );
        if ( hp > 0u )
        {
            DelLifeEvent( player, key );
            return;
        }

        // 更新英雄死亡数据
        UpdateDeadData( player, kfdata );

        // 添加英雄死亡事件
        AddLifeEvent( player, key, KFMsg::HeroDead );

        // 添加死亡记录列表
        AddDeadRecord( player, key, kfdata );
    }

    void KFChancelClientModule::DelLifeEvent( KFEntity* player, uint64 uuid )
    {
        auto playerid = player->GetKeyID();

        KFMsg::S2SDelLifeEventReq req;
        req.set_playerid( playerid );
        req.set_uuid( uuid );

        auto ok = SendMessageToChancel( playerid, KFMsg::S2S_DEL_LIFE_EVENT_REQ, &req );
        if ( !ok )
        {
            std::string strdata;
            google::protobuf::util::MessageToJsonString( req, &strdata );
            __LOG_ERROR__( "sendid={} event={} failed!", playerid, strdata );
        }
    }

    void KFChancelClientModule::AddLifeEvent( KFEntity* player, uint64 uuid, uint32 id )
    {
        auto playerid = player->GetKeyID();

        KFMsg::S2SAddLifeEventReq req;
        req.set_playerid( playerid );
        req.set_uuid( uuid );
        req.set_time( KFGlobal::Instance()->_real_time );
        req.set_eventid( id );

        auto ok = SendMessageToChancel( playerid, KFMsg::S2S_ADD_LIFE_EVENT_REQ, &req );
        if ( !ok )
        {
            std::string strdata;
            google::protobuf::util::MessageToJsonString( req, &strdata );
            __LOG_ERROR__( "sendid={} event={} failed!", playerid, strdata );
        }
    }

    void KFChancelClientModule::UpdateDeadData( KFEntity* player, KFData* kfhero )
    {
        // 更新种族死亡数量
        auto race = kfhero->GetValue( __KF_STRING__( race ) );
        UpdateDeadNum( player, race );

        // 更新武器类型死亡数量
        auto weapontype = kfhero->GetValue( __KF_STRING__( weapontype ) );
        UpdateDeadNum( player, weapontype );

        // 更新移动方式死亡数量
        auto profession = kfhero->GetValue( __KF_STRING__( profession ) );
        auto kfsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfsetting == nullptr )
        {
            return;
        }
        auto movetype = kfsetting->_move_type;
        UpdateDeadNum( player, movetype );
    }

    void KFChancelClientModule::UpdateDeadNum( KFEntity* player, uint32 type )
    {
        if ( !KFChancelConfig::Instance()->IsValid( type ) )
        {
            return;
        }

        auto kfchancelrecord = player->GetData()->FindData( __KF_STRING__( chancel ) );

        auto kfchancel = kfchancelrecord->FindData( type );
        if ( kfchancel == nullptr )
        {
            kfchancel = _kf_kernel->CreateObject( kfchancelrecord->_data_setting );

            kfchancel->SetValue( __KF_STRING__( type ), type );
            kfchancel->SetValue( __KF_STRING__( level ), 0u );
            kfchancel->SetValue( __KF_STRING__( num ), 1u );

            player->AddData( kfchancelrecord, type, kfchancel );
        }
        else
        {
            player->UpdateData( kfchancel, __KF_STRING__( num ), KFEnum::Add, 1u );
        }

        UpdateDeadLevel( player, type );
    }

    void KFChancelClientModule::UpdateDeadLevel( KFEntity* player, uint32 type )
    {
        auto kfchancelrecord = player->GetData()->FindData( __KF_STRING__( chancel ) );
        for ( auto kfchancel = kfchancelrecord->FirstData(); kfchancel != nullptr; kfchancel = kfchancelrecord->NextData() )
        {
            auto deadtype = kfchancel->GetValue<uint32>( __KF_STRING__( type ) );
            auto level = kfchancel->GetValue<uint32>( __KF_STRING__( level ) );

            auto settingid = KFChancelConfig::Instance()->GetChancelId( deadtype, level + 1u );
            auto kfsetting = KFChancelConfig::Instance()->FindSetting( settingid );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            if ( kfsetting->_condition_map.find( type ) == kfsetting->_condition_map.end() )
            {
                continue;
            }

            bool iscondition = true;
            for ( auto iter : kfsetting->_condition_map )
            {
                auto kfdata = kfchancelrecord->FindData( iter.first );
                if ( kfdata == nullptr )
                {
                    iscondition = false;
                    break;
                }

                auto num = kfdata->GetValue<uint32>( __KF_STRING__( num ) );
                if ( num < iter.second )
                {
                    iscondition = false;
                    break;
                }
            }

            // 满足升级条件
            if ( iscondition )
            {
                player->UpdateData( kfchancel, __KF_STRING__( level ), KFEnum::Add, 1u );
            }
        }
    }

    void KFChancelClientModule::AddDeadRecord( KFEntity* player, uint64 uuid, KFData* kfhero )
    {
        auto profession = kfhero->GetValue<uint32>( __KF_STRING__( profession ) );
        auto kfsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfsetting == nullptr )
        {
            return;
        }
        auto movetype = kfsetting->_move_type;

        static MapString _record_data;
        _record_data.clear();

        // uuid
        _record_data.insert( std::make_pair( __KF_STRING__( uuid ), __TO_STRING__( uuid ) ) );

        // 种族
        _record_data.insert( std::make_pair( __KF_STRING__( race ), __TO_STRING__( kfhero->GetValue<uint32>( __KF_STRING__( race ) ) ) ) );

        // 移动方式
        _record_data.insert( std::make_pair( __KF_STRING__( movetype ), __TO_STRING__( movetype ) ) );

        // 武器类型
        _record_data.insert( std::make_pair( __KF_STRING__( weapontype ), __TO_STRING__( kfhero->GetValue<uint32>( __KF_STRING__( weapontype ) ) ) ) );

        // 职业
        _record_data.insert( std::make_pair( __KF_STRING__( profession ), __TO_STRING__( kfhero->GetValue<uint32>( __KF_STRING__( profession ) ) ) ) );

        // 性别
        _record_data.insert( std::make_pair( __KF_STRING__( sex ), __TO_STRING__( kfhero->GetValue<uint32>( __KF_STRING__( sex ) ) ) ) );

        // 名字
        _record_data.insert( std::make_pair( __KF_STRING__( name ), kfhero->GetValue<std::string>( __KF_STRING__( name ) ) ) );

        // 时间
        _record_data.insert( std::make_pair( __KF_STRING__( time ), __TO_STRING__( KFGlobal::Instance()->_real_time ) ) );

        // 地图id
        _record_data.insert( std::make_pair( __KF_STRING__( mapid ), player->GetData()->GetValue<std::string>( __KF_STRING__( mapid ) ) ) );

        SendAddRecodToShard( player, _record_data );
    }

    bool KFChancelClientModule::SendAddRecodToShard( KFEntity* player, const MapString& recorddata )
    {
        auto playerid = player->GetKeyID();

        KFMsg::S2SAddDeadRecordReq req;
        req.set_playerid( playerid );

        auto pbdata = req.mutable_pbdeadrecord()->mutable_data();
        for ( auto& iter : recorddata )
        {
            ( *pbdata )[iter.first] = iter.second;
        }

        auto ok = SendMessageToChancel( playerid, KFMsg::S2S_ADD_DEAD_RECORD_REQ, &req );
        if ( !ok )
        {
            std::string strdata;
            google::protobuf::util::MessageToJsonString( req, &strdata );
            __LOG_ERROR__( "sendid={} record={} failed!", playerid, strdata );
        }

        return ok;
    }

    void KFChancelClientModule::SendQueryRecordMessage( KFEntity* player )
    {
        uint64 maxid = player->GetData()->GetValue( __KF_STRING__( maxdeadrecordid ) );;

        KFMsg::S2SQueryDeadRecordReq req;
        req.set_playerid( player->GetKeyID() );
        req.set_maxid( maxid );

        SendMessageToChancel( player->GetKeyID(), KFMsg::S2S_QUERY_DEAD_RECORD_REQ, &req );
    }

    void KFChancelClientModule::SendQueryEventMessage( KFEntity* player, uint64 uuid )
    {
        KFMsg::S2SQueryLifeEventReq req;
        req.set_playerid( player->GetKeyID() );
        req.set_uuid( uuid );

        SendMessageToChancel( player->GetKeyID(), KFMsg::S2S_QUERY_LIFE_EVENT_REQ, &req );
    }

    bool KFChancelClientModule::SendMessageToChancel( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_route->SendToRand( playerid, __KF_STRING__( logic ), msgid, message, true );
    }

    __KF_MESSAGE_FUNCTION__( KFChancelClientModule::HandleQueryDeadRecordReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryDeadRecordReq );

        SendQueryRecordMessage( player );
    }

    __KF_MESSAGE_FUNCTION__( KFChancelClientModule::HandleQueryDeadRecordAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryDeadRecordAck );

        uint64 maxid = 0u;
        uint64 deadrecordid = 0u;

        KFMsg::MsgQueryDeadRecordAck ack;

        for ( auto i = 0; i < kfmsg.deadrecord_size(); ++i )
        {
            auto pbdeadrecord = ack.add_deadrecord()->mutable_data();

            auto deadrecord = &kfmsg.deadrecord( i );
            for ( auto& iter : deadrecord->data() )
            {
                ( *pbdeadrecord )[iter.first] = iter.second;

                if ( iter.first == __KF_STRING__( id ) )
                {
                    deadrecordid = KFUtility::ToValue( iter.second );
                }
            }

            if ( deadrecordid > maxid )
            {
                maxid = deadrecordid;
            }
        }

        // 更新最大死亡记录id
        player->GetData()->SetValue( __KF_STRING__( maxdeadrecordid ), maxid );

        _kf_player->SendToClient( player, KFMsg::MSG_QUERY_DEAD_RECORD_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFChancelClientModule::HandleQueryLifeEventReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryLifeEventReq );

        SendQueryEventMessage( player, kfmsg.uuid() );
    }
}

