#include "KFRecordClientModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRecordClientModule::BeforeRun()
    {
        // 注册日报函数
        auto timeid = KFGlobal::Instance()->GetUInt32( "dailyresettime" );
        auto daynum = KFRecordConfig::Instance()->GetExpireNum( KFMsg::DailyRecord );
        __REGISTER_RESET_COUNT__( timeid, daynum, &KFRecordClientModule::OnResetRecord );

        __REGISTER_ENTER_PLAYER__( &KFRecordClientModule::OnEnterRecordModule );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_RECORD_REQ, &KFRecordClientModule::HandleQueryRecordReq );
    }

    void KFRecordClientModule::BeforeShut()
    {
        __UN_RESET__();
        __UN_ENTER_PLAYER__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_QUERY_RECORD_REQ );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRecordClientModule::AddRecordValueFunction( const std::string& module, KFRecordValuesFunction& function )
    {
        auto kffunction = _record_value_list.Create( module );
        kffunction->_function = function;
    }

    void KFRecordClientModule::RemoveRecordValueFunction( const std::string& module )
    {
        _record_value_list.Remove( module );
    }

    bool KFRecordClientModule::SendMessageToRecord( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_route->RepeatToRand( playerid, __STRING__( logic ), msgid, message );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ENTER_PLAYER_FUNCTION__( KFRecordClientModule::OnEnterRecordModule )
    {
        // 新玩家不查询
        if ( player->IsNew() )
        {
            return;
        }

        auto playerid = player->GetKeyID();
        for ( auto& iter : KFRecordConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_login_update )
            {
                KFMsg::S2SOnlineRecordReq req;
                req.set_playerid( playerid );
                req.set_type( kfsetting->_id );
                SendMessageToRecord( playerid, KFMsg::S2S_ONLINE_RECORD_REQ, &req );
            }
        }
    }

    __KF_RESET_FUNCTION__( KFRecordClientModule::OnResetRecord )
    {
        // 计算出重置的具体时间点, 然后显示需要前一天的
        auto time = KFDate::CalcTimeData( timedata, nowtime );

        auto daynum = KFRecordConfig::Instance()->GetExpireNum( KFMsg::DailyRecord );
        auto deadtime = KFDate::CalcZeroTime( time, daynum );
        if ( deadtime < KFGlobal::Instance()->_real_time )
        {
            // 已过期
            return;
        }

        KFDBValue recorddata;
        recorddata.AddValue( __STRING__( id ), player->GetKeyID() );
        recorddata.AddValue( __STRING__( time ), time - 1u );

        // 收集日报信息
        for ( auto& iter : _record_value_list._objects )
        {
            auto kffunctoin = iter.second;
            kffunctoin->_function( player, lasttime, nowtime, recorddata );
        }

        // 英雄数量
        auto kfherorecord = player->Find( __STRING__( hero ) );
        recorddata.AddValue( __STRING__( herocount ), kfherorecord->Size() );
        recorddata.AddValue( __STRING__( maxherocount ), kfherorecord->MaxSize() );
        SendAddRecodToShard( player, KFMsg::DailyRecord, deadtime - time + 1u, recorddata );
    }

    void KFRecordClientModule::AddCampRecord( KFEntity* player, KFData* kfhero, uint64 time, uint32 type, uint64 value )
    {
        auto daynum = KFRecordConfig::Instance()->GetExpireNum( KFMsg::CampRecord );
        auto deadtime = KFDate::CalcZeroTime( time, daynum );
        if ( deadtime < KFGlobal::Instance()->_real_time )
        {
            // 已过期
            return;
        }

        KFDBValue recorddata;
        recorddata.AddValue( __STRING__( id ), player->GetKeyID() );
        recorddata.AddValue( __STRING__( type ), type );
        recorddata.AddValue( __STRING__( uuid ), kfhero->GetKeyID() );
        recorddata.AddValue( __STRING__( race ), kfhero->Get<uint32>( __STRING__( race ) ) );
        recorddata.AddValue( __STRING__( sex ), kfhero->Get<uint32>( __STRING__( sex ) ) );
        recorddata.AddValue( __STRING__( profession ), kfhero->Get<uint32>( __STRING__( profession ) ) );
        recorddata.AddValue( __STRING__( name ), kfhero->Get<std::string>( __STRING__( name ) ) );
        recorddata.AddValue( __STRING__( time ), time );
        recorddata.AddValue( __STRING__( info ), value );
        SendAddRecodToShard( player, KFMsg::CampRecord, deadtime - time, recorddata );
    }

    void KFRecordClientModule::AddDeadRecord( KFEntity* player, KFData* kfhero )
    {
        KFDBValue recorddata;
        recorddata.AddValue( __STRING__( id ), player->GetKeyID() );
        recorddata.AddValue( __STRING__( uuid ), kfhero->GetKeyID() );
        recorddata.AddValue( __STRING__( race ), kfhero->Get<uint32>( __STRING__( race ) ) );
        recorddata.AddValue( __STRING__( sex ), kfhero->Get<uint32>( __STRING__( sex ) ) );

        auto profession = kfhero->Get<uint32>( __STRING__( profession ) );
        recorddata.AddValue( __STRING__( profession ), profession );
        recorddata.AddValue( __STRING__( weapontype ), kfhero->Get<uint32>( __STRING__( weapontype ) ) );
        recorddata.AddValue( __STRING__( name ), kfhero->Get<std::string>( __STRING__( name ) ) );
        recorddata.AddValue( __STRING__( time ), KFGlobal::Instance()->_real_time );
        recorddata.AddValue( __STRING__( realmid ), player->Get<uint32>( __STRING__( realmid ) ) );
        auto kfsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfsetting != nullptr )
        {
            recorddata.AddValue( __STRING__( movetype ), kfsetting->_move_type );
        }
        SendAddRecodToShard( player, KFMsg::DeadRecord, 0u, recorddata );
    }

    void KFRecordClientModule::AddLifeRecord( KFEntity* player, uint64 uuid, uint32 eventid )
    {
        KFDBValue recorddata;
        recorddata.AddValue( __STRING__( uuid ), uuid );
        recorddata.AddValue( __STRING__( lifeevent ), eventid );
        recorddata.AddValue( __STRING__( time ), KFGlobal::Instance()->_real_time );
        SendAddRecodToShard( player, KFMsg::LifeRecord, 0u, recorddata );
    }

    void KFRecordClientModule::DelLifeRecord( KFEntity* player, uint32 uuid )
    {
        KFMsg::S2SDelRecordReq req;
        req.set_id( uuid );
        req.set_type( KFMsg::LifeRecord );
        auto ok = SendMessageToRecord( player->GetKeyID(), KFMsg::S2S_DEL_RECORD_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFRecordClientModule::HandleQueryRecordReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryRecordReq );

        KFMsg::S2SQueryRecordReq req;

        auto id = player->GetKeyID();
        if ( kfmsg.id() != 0u )
        {
            id = kfmsg.id();
        }
        req.set_id( id );
        req.set_type( kfmsg.type() );

        auto ok = SendMessageToRecord( player->GetKeyID(), KFMsg::S2S_QUERY_RECORD_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::LogicServerBusy );
        }
    }

    void KFRecordClientModule::SendAddRecodToShard( KFEntity* player, uint32 recordtype, uint64 validtime, const KFDBValue& recorddata )
    {
        KFMsg::S2SAddRecordReq req;
        req.set_playerid( player->GetKeyID() );
        req.set_time( validtime );
        req.set_type( recordtype );

        auto pbdata = req.mutable_pbrecord();
        __DBVALUE_TO_PROTO__( recorddata, pbdata )

        auto ok = SendMessageToRecord( player->GetKeyID(), KFMsg::S2S_ADD_RECORD_REQ, &req );
        if ( ok )
        {
            auto setting = KFRecordConfig::Instance()->FindSetting( recordtype );
            if ( setting != nullptr && setting->_notice != 0u )
            {
                // 小红点通知
                _kf_notice->SendToPlayer( player, setting->_notice );
            }
        }
    }
}

