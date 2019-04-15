#include "KFBattleAllot.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////
    void KFBattleAllot::Initialize( KFRedisDriver* redisdriver )
    {
        _room_redis = redisdriver;
    }

    bool KFBattleAllot::AddBattle( uint64 serverid, const std::string& ip, uint32 port, const std::string& version )
    {
        // 为了避免单点问题, 保存到redis缓存中
        MapString values;
        values[ __KF_STRING__( ip ) ] = ip;
        values[ __KF_STRING__( version ) ] = version;
        values[ __KF_STRING__( port ) ] = __TO_STRING__( port );

        _room_redis->Append( values, "hmset {}:{}", __KF_STRING__( battle ), serverid );
        _room_redis->Append( "sadd {}:{} {}", __KF_STRING__( battlelist ), version, serverid );
        auto kfresult = _room_redis->Pipeline();
        return kfresult->IsOk();
    }

    void KFBattleAllot::RemoveBattle( uint64 serverid, const std::string& version )
    {
        _room_redis->Append( "del {}:{}", __KF_STRING__( battle ), serverid );
        _room_redis->Append( "srem {}:{} {}", __KF_STRING__( battlelist ), version, serverid );
        _room_redis->Pipeline();
    }

    std::tuple< uint64, std::string, uint32 > KFBattleAllot::AllotBattle( uint64 serverid, const std::string& version )
    {
        if ( serverid != _invalid_int )
        {
            return AllotSpecificBattle( serverid );
        }

        return AllotVersionBattle( version );
    }

    std::tuple< uint64, std::string, uint32 > KFBattleAllot::AllotSpecificBattle( uint64 serverid )
    {
        std::string ip = "";
        uint32 port = _invalid_int;
        uint64 allotid = _invalid_int;

        auto mapresult = _room_redis->QueryMap( "hgetall {}:{}", __KF_STRING__( battle ), serverid );
        if ( !mapresult->_value.empty() )
        {
            allotid = serverid;
            ip = mapresult->_value[ __KF_STRING__( ip ) ];
            port = KFUtility::ToValue< uint32 >( mapresult->_value[ __KF_STRING__( port ) ] );
            if ( ip.empty() || port == _invalid_int )
            {
                __LOG_ERROR__( "battle[{}:{}] data error!", allotid, KFAppId::ToString( serverid ) );
            }
        }
        else
        {
            __LOG_ERROR__( "battle[{}:{}] data empty!", allotid, KFAppId::ToString( serverid ) );
        }

        return std::make_tuple( allotid, ip, port );
    }

    std::tuple< uint64, std::string, uint32 > KFBattleAllot::AllotVersionBattle( const std::string& version )
    {
        std::string ip = "";
        uint32 port = _invalid_int;
        uint64 allotid = _invalid_int;

        // 弹出一个可用的服务器
        auto kfquery = _room_redis->QueryUInt64( "spop {}:{}", __KF_STRING__( battlelist ), version );
        if ( kfquery->_value != _invalid_int )
        {
            std::tie( allotid, ip, port ) = AllotSpecificBattle( kfquery->_value );
        }
        else
        {
            __LOG_ERROR__( "version[{}] battle list empty!", version );
        }

        return std::make_tuple( allotid, ip, port );
    }

}