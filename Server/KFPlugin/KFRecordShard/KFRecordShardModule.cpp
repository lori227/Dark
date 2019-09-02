#include "KFRecordShardModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRecordShardModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_RECORD_REQ, &KFRecordShardModule::HandleAddRecordReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RECORD_REQ, &KFRecordShardModule::HandleQueryRecordReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ONLINE_RECORD_REQ, &KFRecordShardModule::HandleOnlineRecordReq );
    }

    void KFRecordShardModule::BeforeShut()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_ADD_RECORD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_RECORD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ONLINE_RECORD_REQ );
    }

    void KFRecordShardModule::PrepareRun()
    {
        // 初始化mongo
        _mongo_driver = _kf_mongo->Create( __KF_STRING__( record ) );

        // 过期索引
        _mongo_driver->CreateIndex( __KF_STRING__( record ), MongoKeyword::_expire );

        // 查找索引
        _mongo_driver->CreateIndex( __KF_STRING__( record ), __KF_STRING__( id ) );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRecordShardModule::HandleOnlineRecordReq )
    {
        __PROTO_PARSE__( KFMsg::S2SOnlineRecordReq );

        auto setting = KFRecordConfig::Instance()->FindSetting( kfmsg.type() );
        if ( setting == nullptr )
        {
            return;
        }

        // 重置最后查询的时间
        _mongo_driver->InsertT( __KF_STRING__( playerrecord ), kfmsg.playerid(), setting->_onlinetime, 0u );

        // 查询是否有未查看的纪录Z
        auto kfresult = _mongo_driver->QueryUInt64( __KF_STRING__( playerrecord ), kfmsg.playerid(), setting->_newrecord );
        if ( kfresult->_value == 1u )
        {
            _kf_notice->SendToPlayer( route, setting->_notice );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRecordShardModule::HandleQueryRecordReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRecordReq );

        auto setting = KFRecordConfig::Instance()->FindSetting( kfmsg.type() );
        if ( setting == nullptr )
        {
            return;
        }

        // 查询最后的时间
        auto kftimeresult = _mongo_driver->QueryUInt64( __KF_STRING__( playerrecord ), kfmsg.playerid(), setting->_onlinetime );

        KFMongoSelector kfselector;
        kfselector._document.AddExpression( __KF_STRING__( id ), MongoKeyword::_eq, kfmsg.playerid() );
        kfselector._document.AddExpression( __KF_STRING__( time ), MongoKeyword::_gt, kftimeresult->_value );
        auto kfrecordresult = _mongo_driver->QueryListMapString( setting->_record, kfselector );
        if ( kfrecordresult->_value.empty() )
        {
            return;
        }

        uint64 maxtime = 0u;
        KFMsg::MsgQueryRecordAck ack;
        ack.set_type( kfmsg.type() );

        for ( auto& values : kfrecordresult->_value )
        {
            auto& pbdata = *ack.add_pbrecord()->mutable_data();
            __MAP_TO_PROTO__( values, pbdata );

            auto time = KFUtility::ToValue( values[ __KF_STRING__( time ) ] );
            if ( time > maxtime )
            {
                maxtime = time;
            }
        }
        _kf_route->SendToRoute( route, KFMsg::MSG_QUERY_RECORD_ACK, &ack );

        // 更新纪录
        MapString values;
        values[ setting->_newrecord ] = "0";
        values[ setting->_onlinetime ] = __TO_STRING__( maxtime );
        _mongo_driver->Update( __KF_STRING__( playerrecord ), kfmsg.playerid(), values );
    }

    __KF_MESSAGE_FUNCTION__( KFRecordShardModule::HandleAddRecordReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddRecordReq );

        auto setting = KFRecordConfig::Instance()->FindSetting( kfmsg.type() );
        if ( setting == nullptr )
        {
            return;
        }

        // 营地记录
        auto pbrecord = &kfmsg.pbrecord().data();

        MapString values;
        __PROTO_TO_MAP__( pbrecord, values );
        values[ MongoKeyword::_expire ] = __TO_STRING__( kfmsg.time() );
        values[ __KF_STRING__( id ) ] = __TO_STRING__( kfmsg.playerid() );
        _mongo_driver->Insert( setting->_record, values );

        // 更新有新纪录
        _mongo_driver->InsertT( __KF_STRING__( playerrecord ), kfmsg.playerid(), setting->_newrecord, 1u );
    }
}