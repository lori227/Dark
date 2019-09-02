#include "KFRecordClientModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{

    void KFRecordClientModule::BeforeRun()
    {
        // 注册日报函数
        KFTimeData timedata;
        timedata._hour = 0u;
        timedata._type = KFTimeEnum::Day;

        static auto _option = _kf_option->FindOption( "dailyrecordtime" );
        __REGISTER_RESET_COUNT__( timedata, _option->_uint32_value, &KFRecordClientModule::OnResetRecord );

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
        return _kf_route->SendToRand( playerid, __KF_STRING__( logic ), msgid, message, true );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ENTER_PLAYER_FUNCTION__( KFRecordClientModule::OnEnterRecordModule )
    {
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
        auto time = KFDate::CalcTimeData( &timedata, nowtime );

        static auto _option = _kf_option->FindOption( "dailyrecordtime" );
        auto deadtime = KFDate::CalcZeroTime( time, _option->_uint32_value );
        if ( deadtime < KFGlobal::Instance()->_real_time )
        {
            // 已过期
            return;
        }

        MapString _record_data;
        _record_data[ __KF_STRING__( time ) ] = __TO_STRING__( time - 1u );

        // 收集日报信息
        for ( auto& iter : _record_value_list._objects )
        {
            auto kffunctoin = iter.second;
            kffunctoin->_function( player, lasttime, nowtime, _record_data );
        }

        // 英雄数量
        auto kfherorecord = player->GetData()->FindData( __KF_STRING__( hero ) );
        _record_data[ __KF_STRING__( herocount ) ] = __TO_STRING__( kfherorecord->Size() );
        _record_data[ __KF_STRING__( maxherocount ) ] = __TO_STRING__( kfherorecord->MaxSize() );
        SendAddRecodToShard( player, KFMsg::DailyRecord, deadtime - time, _record_data );
    }

    void KFRecordClientModule::AddCampRecord( KFEntity* player, KFData* kfhero, uint64 time, uint32 type, uint64 value )
    {
        static auto _option = _kf_option->FindOption( "camprecordtime" );
        auto deadtime = KFDate::CalcZeroTime( time, _option->_uint32_value );
        if ( deadtime < KFGlobal::Instance()->_real_time )
        {
            // 已过期
            return;
        }

        MapString _record_data;

        // 记录类型
        _record_data[ __KF_STRING__( type ) ] = __TO_STRING__( type );

        // uuid
        _record_data[ __KF_STRING__( uuid ) ] = __TO_STRING__( kfhero->GetKeyID() );

        // 种族
        auto race = kfhero->GetValue<uint32>( __KF_STRING__( race ) );
        _record_data[ __KF_STRING__( race ) ] = __TO_STRING__( race );

        // 性别
        auto sex = kfhero->GetValue<uint32>( __KF_STRING__( sex ) );
        _record_data[ __KF_STRING__( sex ) ] = __TO_STRING__( sex );

        // 职业
        auto profession = kfhero->GetValue<uint32>( __KF_STRING__( profession ) );
        _record_data[ __KF_STRING__( profession ) ] = __TO_STRING__( profession );

        // 名字
        auto name = kfhero->GetValue<std::string>( __KF_STRING__( name ) );
        _record_data[ __KF_STRING__( name ) ] = name;

        // 时间戳
        _record_data[ __KF_STRING__( time ) ] = __TO_STRING__( time );

        // 数据
        _record_data[ __KF_STRING__( info ) ] = __TO_STRING__( value );

        SendAddRecodToShard( player, KFMsg::CampRecord, deadtime - time, _record_data );
    }

    __KF_MESSAGE_FUNCTION__( KFRecordClientModule::HandleQueryRecordReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryRecordReq );

        KFMsg::S2SQueryRecordReq req;
        req.set_playerid( player->GetKeyID() );
        req.set_type( kfmsg.type() );

        auto ok = SendMessageToRecord( player->GetKeyID(), KFMsg::S2S_QUERY_RECORD_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::LogicServerBusy );
        }
    }

    void KFRecordClientModule::SendAddRecodToShard( KFEntity* player, uint32 recordtype, uint64 validtime, const MapString& recorddata )
    {
        KFMsg::S2SAddRecordReq req;
        req.set_playerid( player->GetKeyID() );
        req.set_time( validtime );
        req.set_type( recordtype );

        auto& pbdata = *req.mutable_pbrecord()->mutable_data();
        __MAP_TO_PROTO__( recorddata, pbdata )

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

