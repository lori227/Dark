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
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_RECORD_REQ, &KFRecordShardModule::HandleDelRecordReq );
    }

    void KFRecordShardModule::BeforeShut()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_ADD_RECORD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_RECORD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ONLINE_RECORD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_DEL_RECORD_REQ );
    }

    void KFRecordShardModule::PrepareRun()
    {
        // 初始化mongo
        _mongo_driver = _kf_mongo->Create( __STRING__( record ) );

        for ( auto& iter : KFRecordConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_expire_type == KFMsg::TimeExpire )
            {
                // 过期索引
                _mongo_driver->CreateIndex( kfsetting->_record, MongoKeyword::_expire );
            }

            // 查找索引
            _mongo_driver->CreateIndex( kfsetting->_record, __STRING__( id ) );
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRecordShardModule::HandleOnlineRecordReq )
    {
        __PROTO_PARSE__( KFMsg::S2SOnlineRecordReq );

        auto setting = KFRecordConfig::Instance()->FindSetting( kfmsg.type() );
        if ( setting == nullptr || setting->_notice == 0u )
        {
            return;
        }

        // 重置最后查询的时间
        _mongo_driver->Insert( __STRING__( playerrecord ), kfmsg.playerid(), setting->_onlinetime, 0u );

        // 查询是否有未查看的纪录
        auto kfresult = _mongo_driver->QueryUInt64( __STRING__( playerrecord ), kfmsg.playerid(), setting->_newrecord );
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

        KFMongoSelector kfselector;
        kfselector._document.AddExpression( __STRING__( id ), MongoKeyword::_eq, kfmsg.id() );

        kfselector.AddLimitReturn( MongoKeyword::_id );
        kfselector.AddLimitReturn( __STRING__( id ) );

        if ( setting->_notice != 0u )
        {
            // 查询最后的时间
            auto kftimeresult = _mongo_driver->QueryUInt64( __STRING__( playerrecord ), kfmsg.id(), setting->_onlinetime );
            kfselector._document.AddExpression( __STRING__( time ), MongoKeyword::_gt, kftimeresult->_value );
        }

        auto kfrecordresult = _mongo_driver->QueryListRecord( setting->_record, kfselector );
        if ( kfrecordresult->_value.empty() )
        {
            return;
        }

        uint64 maxtime = 0u;
        KFMsg::MsgQueryRecordAck ack;
        ack.set_type( kfmsg.type() );
        ack.set_id( kfmsg.id() );
        for ( auto& dbvalue : kfrecordresult->_value )
        {
            auto pbdata = ack.add_pbrecord();
            __DBVALUE_TO_PROTO__( dbvalue, pbdata );

            auto time = dbvalue.FindValue( __STRING__( time ) );
            if ( time > maxtime )
            {
                maxtime = time;
            }
        }
        _kf_route->SendToRoute( route, KFMsg::MSG_QUERY_RECORD_ACK, &ack );

        if ( setting->_notice != 0u )
        {
            // 更新纪录
            KFDBValue dbvalue;
            dbvalue.AddValue( setting->_newrecord, 0u );
            dbvalue.AddValue( setting->_onlinetime, maxtime );
            _mongo_driver->Insert( __STRING__( playerrecord ), kfmsg.id(), dbvalue );
        }
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
        KFDBValue dbvalue;
        auto pbrecord = &kfmsg.pbrecord();
        __PROTO_TO_DBVALUE__( pbrecord, dbvalue );

        if ( setting->_expire_type == KFMsg::TimeExpire )
        {
            dbvalue.AddValue( MongoKeyword::_expire, kfmsg.time() );
        }
        else if ( setting->_expire_type == KFMsg::NumExpire )
        {
            CheckExpireRecordNum( kfmsg.playerid(), kfmsg.type() );
        }

        _mongo_driver->Insert( setting->_record, dbvalue );
        if ( setting->_notice != 0u )
        {
            // 更新有新纪录
            _mongo_driver->Insert( __STRING__( playerrecord ), kfmsg.playerid(), setting->_newrecord, 1u );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRecordShardModule::HandleDelRecordReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelRecordReq );

        DeleteRecord( kfmsg.type(), kfmsg.id() );
    }

    void KFRecordShardModule::CheckExpireRecordNum( uint64 playerid, uint32 type )
    {
        auto setting = KFRecordConfig::Instance()->FindSetting( type );
        if ( setting == nullptr )
        {
            return;
        }

        KFMongoSelector kfselector;
        kfselector.AddReturn( MongoKeyword::_id );
        kfselector.AddReturn( __STRING__( uuid ) );
        kfselector._document.AddExpression( __STRING__( id ), MongoKeyword::_eq, playerid );
        auto kfrecordresult = _mongo_driver->QueryListRecord( setting->_record, kfselector );
        if ( kfrecordresult->_value.empty() )
        {
            return;
        }

        auto recordnum = static_cast<uint32>( kfrecordresult->_value.size() );
        if ( recordnum < setting->_expire_num )
        {
            return;
        }

        // 超出记录上限删除数据
        uint32 index = 0u;
        auto expirenum = recordnum - setting->_expire_num;
        for ( auto& dbvalue : kfrecordresult->_value )
        {
            if ( index > expirenum )
            {
                break;
            }

            // 从死亡记录中找出英雄uuid
            index++;
            auto uuid = dbvalue.FindValue( __STRING__( uuid ) );
            DeleteRecord( KFMsg::LifeRecord, uuid );

            auto id = dbvalue.FindValue( MongoKeyword::_id );
            _mongo_driver->Delete( setting->_record, id );
        }
    }

    void KFRecordShardModule::DeleteRecord( uint32 type, uint64 id )
    {
        auto setting = KFRecordConfig::Instance()->FindSetting( type );
        if ( setting == nullptr || id == 0u )
        {
            return;
        }

        KFMongoSelector kfselector;
        kfselector._document.AddExpression( __STRING__( id ), MongoKeyword::_eq, id );
        _mongo_driver->Delete( setting->_record, kfselector );
    }

}