#include "KFChancelShardModule.hpp"

namespace KFrame
{
    void KFChancelShardModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_DEAD_RECORD_REQ, &KFChancelShardModule::HandleAddDeadRecordReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_DEAD_RECORD_REQ, &KFChancelShardModule::HandleQueryDeadRecordReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_LIFE_EVENT_REQ, &KFChancelShardModule::HandleAddLifeEventReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DEL_LIFE_EVENT_REQ, &KFChancelShardModule::HandleDelLifeEventReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_LIFE_EVENT_REQ, &KFChancelShardModule::HandleQueryLifeEventReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFChancelShardModule::BeforeShut()
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_ADD_DEAD_RECORD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_DEAD_RECORD_REQ );
        __UN_MESSAGE__( KFMsg::S2S_ADD_LIFE_EVENT_REQ );
        __UN_MESSAGE__( KFMsg::S2S_DEL_LIFE_EVENT_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_LIFE_EVENT_REQ );
    }

    void KFChancelShardModule::PrepareRun()
    {
        // 初始化redis
        _chancel_driver = _kf_redis->Create( __KF_STRING__( public ) );
    }

    __KF_MESSAGE_FUNCTION__( KFChancelShardModule::HandleAddDeadRecordReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddDeadRecordReq );

        MapString recorddata;
        auto pbdeadrecord = &kfmsg.pbdeadrecord().data();
        for ( auto iter = pbdeadrecord->begin(); iter != pbdeadrecord->end(); ++iter )
        {
            recorddata[iter->first] = iter->second;
        }

        auto recordid = AddDeadRecord( kfmsg.playerid(), recorddata );

        if ( recordid == _invalid_int )
        {
            std::string strdata;
            google::protobuf::util::MessageToJsonString( kfmsg, &strdata );
            __LOG_ERROR__( "playerid[{}] add record[{}] failed!", kfmsg.playerid(), strdata );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFChancelShardModule::HandleQueryDeadRecordReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryDeadRecordReq );

        // 个人死亡记录列表
        auto recordlist = _chancel_driver->QueryList( "zrangebyscore {}:{} -inf +inf", __KF_STRING__( deadrecordlist ), kfmsg.playerid() );
        if ( recordlist->_value.empty() )
        {
            return;
        }

        KFMsg::S2SQueryDeadRecordAck ack;
        ack.set_playerid( kfmsg.playerid() );

        ListString overduelist;
        for ( auto& strrecordid : recordlist->_value )
        {
            // 判断是否已经下发
            auto recordid = KFUtility::ToValue<uint64>( strrecordid );
            if ( recordid <= kfmsg.maxid() )
            {
                continue;
            }

            auto kfresult = _chancel_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( deadrecord ), recordid );
            if ( !kfresult->IsOk() )
            {
                continue;
            }

            if ( kfresult->_value.empty() )
            {
                continue;
            }

            // 死亡记录
            auto pbdeadrecord = ack.add_deadrecord()->mutable_data();
            for ( auto& iter : kfresult->_value )
            {
                ( *pbdeadrecord )[iter.first] = iter.second;
            }

        }

        if ( ack.deadrecord_size() != 0 )
        {
            _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_DEAD_RECORD_ACK, &ack );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFChancelShardModule::HandleAddLifeEventReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddLifeEventReq );

        // 增加英雄生平
        _chancel_driver->Execute( "hset {}:{}:{} {} {} ", __KF_STRING__( lifeevent ), kfmsg.playerid(), kfmsg.uuid(), kfmsg.time(), kfmsg.eventid() );
    }

    __KF_MESSAGE_FUNCTION__( KFChancelShardModule::HandleDelLifeEventReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDelLifeEventReq );

        // 删除生平事件
        _chancel_driver->Execute( "del {}:{}:{}", __KF_STRING__( lifeevent ), kfmsg.playerid(), kfmsg.uuid() );
    }

    __KF_MESSAGE_FUNCTION__( KFChancelShardModule::HandleQueryLifeEventReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryLifeEventReq );

        // 生平事件列表
        auto eventlist = _chancel_driver->QueryMap( "hgetall {}:{}:{}", __KF_STRING__( lifeevent ), kfmsg.playerid(), kfmsg.uuid() );
        if ( eventlist->_value.empty() )
        {
            return;
        }

        KFMsg::MsgQueryLifeEventAck ack;
        ack.set_uuid( kfmsg.uuid() );

        for ( auto& iter : eventlist->_value )
        {
            auto pblifeevent = ack.add_lifeevent()->mutable_data();

            ( *pblifeevent )[iter.first] = iter.second;
        }

        if ( ack.lifeevent_size() != 0 )
        {
            _kf_route->SendToRoute( route, KFMsg::MSG_QUERY_LIFE_EVENT_ACK, &ack );
        }
    }

    uint64 KFChancelShardModule::AddDeadRecord( uint64 playerid, MapString& recorddata )
    {
        // 创建一个死亡记录id
        auto uint64result = _chancel_driver->Execute( "incr {}", __KF_STRING__( deadrecordidcreater ) );
        if ( uint64result->_value == _invalid_int )
        {
            return _invalid_int;
        }

        auto recordid = uint64result->_value;
        recorddata[ __KF_STRING__( id ) ] = __TO_STRING__( recordid );

        // 个人死亡记录列表
        auto recordlist = _chancel_driver->QueryList( "zrangebyscore {}:{} -inf +inf", __KF_STRING__( deadrecordlist ), playerid );

        static auto _deadoption = _kf_option->FindOption( "deadrecordcount" );
        auto recordcount = static_cast<uint32>( recordlist->_value.size() );
        if ( recordcount >= _deadoption->_uint32_value )
        {
            // 超出记录上限删除数据
            auto overcount = recordcount - _deadoption->_uint32_value;

            auto kfresult = _chancel_driver->QueryList( "zrange {}:{} {} {}", __KF_STRING__( deadrecordlist ), playerid, 0, overcount );
            if ( !kfresult->_value.empty() )
            {
                // 获取uuid来删除生平记录
                ListString uuidlist;
                for ( auto iter : kfresult->_value )
                {
                    auto recordid = KFUtility::ToValue<uint64>( iter );

                    auto kfdeadrecord = _chancel_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( deadrecord ), recordid );
                    if ( !kfdeadrecord->IsOk() )
                    {
                        continue;
                    }

                    if ( kfdeadrecord->_value.empty() )
                    {
                        continue;
                    }

                    auto uuiditer = kfdeadrecord->_value.find( __KF_STRING__( uuid ) );
                    if ( uuiditer != kfdeadrecord->_value.end() )
                    {
                        uuidlist.push_back( uuiditer->second );
                    }
                }

                _chancel_driver->Update( uuidlist, "hdel {}:{}", __KF_STRING__( lifeevent ), playerid );
                _chancel_driver->Update( kfresult->_value, "hdel {}", __KF_STRING__( deadrecord ) );
                _chancel_driver->Update( kfresult->_value, "zrem {}:{}", __KF_STRING__( deadrecordlist ), playerid );
            }
        }

        // 所有死亡记录
        _chancel_driver->Append( recorddata, "hmset {}:{}", __KF_STRING__( deadrecord ), recordid );

        // 个人死亡记录
        _chancel_driver->Append( "zadd {}:{} {} {} ", __KF_STRING__( deadrecordlist ), playerid, recordid, recordid );

        // 执行添加
        auto kfresult = _chancel_driver->Pipeline();
        if ( !kfresult->IsOk() )
        {
            recordid = _invalid_int;
        }

        return recordid;
    }

}