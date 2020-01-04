#include "KFTLogModule.hpp"

namespace KFrame
{
    void KFTLogModule::BeforeRun()
    {
        InitTLogData();
        /////////////////////////////////////////////////////////////////////////////
        auto timeid = 1u;
        // 5分钟打印一次服务器状态
        __LOOP_TIMER_1__( ++timeid, 5 * KFTimeEnum::OneMinuteMicSecond, 1, &KFTLogModule::OnTimerLogServerStatus );

        // 3分钟打印一次在线玩家
        __LOOP_TIMER_1__( ++timeid, 3 * KFTimeEnum::OneMinuteMicSecond, 1, &KFTLogModule::OnTimerLogOnlineCount );
        /////////////////////////////////////////////////////////////////////////////
        __REGISTER_ENTER_PLAYER__( &KFTLogModule::OnEnterTLogModule );
        __REGISTER_LEAVE_PLAYER__( &KFTLogModule::OnLeaveTLogModule );
        /////////////////////////////////////////////////////////////////////////////
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_LOG_ELEMENT__( __STRING__( item ), &KFTLogModule::LogItemElement );
        __REGISTER_LOG_ELEMENT__( __STRING__( money ), &KFTLogModule::LogCurrencyElement );
        __REGISTER_LOG_ELEMENT__( __STRING__( diamond ), &KFTLogModule::LogCurrencyElement );
        __REGISTER_LOG_ELEMENT__( __STRING__( supplies ), &KFTLogModule::LogCurrencyElement );

        /////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_UPDATE_STRING_2__( __STRING__( basic ), __STRING__( name ), &KFTLogModule::OnUpdateNameLogCreateRole );
    }

    void KFTLogModule::ShutDown()
    {
        __UN_TIMER_0__();
        /////////////////////////////////////////////////////////////////////////////
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        /////////////////////////////////////////////////////////////////////////////
        __UN_LOG_ELEMENT__( __STRING__( item ) );
        __UN_LOG_ELEMENT__( __STRING__( money ) );
        __UN_LOG_ELEMENT__( __STRING__( diamond ) );
        __UN_LOG_ELEMENT__( __STRING__( supplies ) );
        /////////////////////////////////////////////////////////////////////////////
        __UN_UPDATE_STRING_2__( __STRING__( basic ), __STRING__( name ) );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFSpdLog* KFTLogModule::CreateLog( const std::string& name )
    {
        auto kfglobal = KFGlobal::Instance();
        auto spdlog = KFLogger::Instance()->NewLogger( kfglobal->_app_id->GetId(), name, kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString() );
        return spdlog;
    }
#define __TLOG__( myfmt, ... )\
    {\
        auto content = __FORMAT__( myfmt, __VA_ARGS__ );\
        spdlog->Log( KFLogEnum::Info, content );\
    }\

    void KFTLogModule::InitTLogData()
    {
        _game_app_id = KFTLogConfig::Instance()->GetString( "gameid" );
        _log_version = KFTLogConfig::Instance()->GetUInt64( "version" );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFTLogModule::OnTimerLogServerStatus )
    {
        auto kfglobal = KFGlobal::Instance();
        auto spdlog = CreateLog( __STRING__( onelevel ) );

        __TLOG__( "GameSvrState|{}|{}|{}|{}|{}|{}|{}|{}|{}",
                  _log_version,	// logVersion
                  kfglobal->_app_id->ToString(),	// vGameSvrId
                  KFDate::GetTimeString( kfglobal->_real_time ),	// dtEventTime
                  _game_app_id,	// vGameAppid
                  _invalid_int,	// iZoneAreaID
                  _kf_ip_address->GetLocalIp(),	// vGameIP
                  _invalid_string,	// serverKey
                  kfglobal->GetVersion(),	// serverVersion
                  _invalid_string	// custom
                );
    }

    __KF_TIMER_FUNCTION__( KFTLogModule::OnTimerLogOnlineCount )
    {
        auto kfglobal = KFGlobal::Instance();
        auto spdlog = CreateLog( __STRING__( onelevel ) );

        KFDate nowdate( kfglobal->_real_time );
        auto hhmm = __FORMAT__( "{}:{}", nowdate.GetHour(), nowdate.GetMinute() );

        __TLOG__( "OnlineCount|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}",
                  _log_version,	// logVersion
                  kfglobal->_app_id->ToString(),	// vGameSvrId
                  KFDate::GetTimeString( kfglobal->_real_time ),	// dtEventTime
                  _game_app_id,	// vGameAppid
                  _invalid_int,	// vPlatID
                  kfglobal->_channel,	// vPlatformType
                  _invalid_int,	// registerZoneID
                  _invalid_int,	// currentZoneID
                  _kf_component->GetEntityCount(),	// count
                  hhmm,	// hhmm
                  nowdate.GetDay(),	// ymd
                  kfglobal->_real_time	// lastTime
                );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_STRING_FUNCTION__( KFTLogModule::OnUpdateNameLogCreateRole )
    {
        // 如果原来有名字, 说明是改名字, 不是第一次创建角色
        if ( !oldvalue.empty() )
        {
            return;
        }

        auto kfglobal = KFGlobal::Instance();
        auto spdlog = CreateLog( __STRING__( onelevel ) );

        __TLOG__( "PlayerRegister|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}",
                  _log_version,	// logVersion
                  kfglobal->_app_id->ToString(),	// vGameSvrId
                  KFDate::GetTimeString( kfglobal->_real_time ),	// dtEventTime
                  _game_app_id,	// vGameAppid
                  _invalid_int,	// vPlatID
                  kfglobal->_channel,	// vPlatformType
                  _invalid_int,	// registerZoneID
                  _invalid_int, // currentZoneID
                  player->Get<uint64>( __STRING__( accountid ) ),	// accountID
                  player->Get<std::string>( __STRING__( account ) ),	// vopenid

                  _invalid_string,	// clientVersion
                  _invalid_string,	// systemSoftware
                  _invalid_string,	// systemHardware
                  _invalid_string,	// telecomOper
                  _invalid_string,	// network
                  _invalid_int,	// screenWidth
                  _invalid_int,	// screenHight
                  _invalid_int,	// density
                  _invalid_string,	// cpuHardware
                  _invalid_int,	// memory
                  _invalid_string, // gLRender
                  _invalid_string,	// gLVersion
                  _invalid_string,	// deviceID
                  player->Get<std::string>( __STRING__( ip ) ),	// clientIP

                  player->Get<uint32>( __STRING__( channel ) ), // loginChannel
                  player->GetKeyID(),	// roleID
                  newvalue,	// roleName
                  _invalid_int,	// roleTypeId
                  _invalid_int,	// roleSex
                  _invalid_string	// custom
                );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFTLogModule::OnEnterTLogModule )
    {
        // 没有创建角色之前不打印
        auto name = player->Get<std::string>( __STRING__( basic ), __STRING__( name ) );
        if ( name.empty() )
        {
            return;
        }

        auto kfglobal = KFGlobal::Instance();
        auto spdlog = CreateLog( __STRING__( onelevel ) );

        __TLOG__( "PlayerLogin|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}",
                  _log_version,	// logVersion
                  kfglobal->_app_id->ToString(),	// vGameSvrId
                  KFDate::GetTimeString( kfglobal->_real_time ),	// dtEventTime
                  _game_app_id,	// vGameAppid
                  _invalid_int,	// vPlatID
                  kfglobal->_channel,	// vPlatformType
                  _invalid_int,	// registerZoneID
                  _invalid_int, // currentZoneID
                  player->Get<uint64>( __STRING__( accountid ) ),	// accountID
                  player->Get<std::string>( __STRING__( account ) ),	// vopenid

                  _invalid_string,	// clientVersion
                  _invalid_string,	// systemSoftware
                  _invalid_string,	// systemHardware
                  _invalid_string,	// telecomOper
                  _invalid_string,	// network
                  _invalid_int,	// screenWidth
                  _invalid_int,	// screenHight
                  _invalid_int,	// density
                  _invalid_string,	// cpuHardware
                  _invalid_int,	// memory
                  _invalid_string, // gLRender
                  _invalid_string,	// gLVersion
                  _invalid_string,	// deviceID
                  player->Get<std::string>( __STRING__( ip ) ),	// clientIP

                  player->Get<uint32>( __STRING__( channel ) ), // loginChannel
                  player->GetKeyID(),	// roleID
                  player->Get<std::string>( __STRING__( basic ), __STRING__( name ) ),	// roleName
                  _invalid_int,	// level
                  _invalid_int,	// vipLevel
                  KFDate::GetTimeString( player->Get( __STRING__( birthday ) ) ),	// registerTime
                  _invalid_int,	// guildId
                  _invalid_int,	// rechargeNum
                  player->Get( __STRING__( diamond ) ),	// valueAmount
                  _invalid_int,	// bindAmount
                  player->Get( __STRING__( money ) ),	// iconAmount
                  _invalid_string,	// otherAmount

                  ( player->IsNew() ? 1u : 0u ),	// firstLogin
                  _invalid_string	// custom
                );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFTLogModule::OnLeaveTLogModule )
    {
        // 在线时长
        LogOnlineTime( player );

        // 登出游戏
        LogLeaveGame( player );
    }

    void KFTLogModule::LogOnlineTime( KFEntity* player )
    {
        auto kfglobal = KFGlobal::Instance();
        auto spdlog = CreateLog( __STRING__( onelevel ) );
        auto onlinetime = kfglobal->_real_time - player->Get( __STRING__( logintime ) );

        __TLOG__( "OnlineTime|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}",
                  _log_version,	// logVersion
                  kfglobal->_app_id->ToString(),	// vGameSvrId
                  KFDate::GetTimeString( kfglobal->_real_time ),	// dtEventTime
                  _game_app_id,	// vGameAppid
                  _invalid_int,	// vPlatID
                  kfglobal->_channel,	// vPlatformType
                  _invalid_int,	// registerZoneID
                  _invalid_int,	// currentZoneID
                  player->Get<uint64>( __STRING__( accountid ) ),	// accountID
                  player->Get<std::string>( __STRING__( account ) ),	// vopenid
                  player->GetKeyID(),	// roleid
                  onlinetime,	// time
                  kfglobal->_real_time	// ymd
                );
    }

    void KFTLogModule::LogLeaveGame( KFEntity* player )
    {
        auto kfglobal = KFGlobal::Instance();
        auto spdlog = CreateLog( __STRING__( onelevel ) );
        auto onlinetime = kfglobal->_real_time - player->Get( __STRING__( logintime ) );

        __TLOG__( "PlayerLogout|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}",
                  _log_version,	// logVersion
                  kfglobal->_app_id->ToString(),	// vGameSvrId
                  KFDate::GetTimeString( kfglobal->_real_time ),	// dtEventTime
                  _game_app_id,	// vGameAppid
                  _invalid_int,	// vPlatID
                  kfglobal->_channel,	// vPlatformType
                  _invalid_int,	// registerZoneID
                  _invalid_int, // currentZoneID
                  player->Get<uint64>( __STRING__( accountid ) ),	// accountID
                  player->Get<std::string>( __STRING__( account ) ),	// vopenid

                  _invalid_string,	// clientVersion
                  _invalid_string,	// systemSoftware
                  _invalid_string,	// systemHardware
                  _invalid_string,	// telecomOper
                  _invalid_string,	// network
                  _invalid_int,	// screenWidth
                  _invalid_int,	// screenHight
                  _invalid_int,	// density
                  _invalid_string,	// cpuHardware
                  _invalid_int,	// memory
                  _invalid_string, // gLRender
                  _invalid_string,	// gLVersion
                  _invalid_string,	// deviceID
                  player->Get<std::string>( __STRING__( ip ) ),	// clientIP

                  player->GetKeyID(),	// roleID
                  player->Get<std::string>( __STRING__( basic ), __STRING__( name ) ),	// roleName
                  _invalid_int,	// level
                  _invalid_int,	// vipLevel
                  KFDate::GetTimeString( player->Get( __STRING__( birthday ) ) ),	// registerTime
                  _invalid_int,	// guildId
                  _invalid_int,	// rechargeNum
                  player->Get( __STRING__( diamond ) ),	// valueAmount
                  _invalid_int,	// bindAmount
                  player->Get( __STRING__( money ) ),	// iconAmount
                  _invalid_string,	// otherAmount

                  onlinetime,	// OnlineTime
                  _invalid_string	// custom
                );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_LOG_ELEMENT_FUNCTION__( KFTLogModule::LogItemElement )
    {
        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfresult->_config_id );
        if ( kfsetting == nullptr )
        {
            return;
        }

        auto kfglobal = KFGlobal::Instance();
        auto spdlog = CreateLog( __STRING__( onelevel ) );

        __TLOG__( "ItemFlow|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}",
                  _log_version,	// logVersion
                  kfglobal->_app_id->ToString(),	// vGameSvrId
                  KFDate::GetTimeString( kfglobal->_real_time ),	// dtEventTime
                  _game_app_id,	// vGameAppid
                  _invalid_int,	// vPlatID
                  kfglobal->_channel,	// vPlatformType
                  _invalid_int,	// registerZoneID
                  _invalid_int,	// currentZoneID
                  player->Get<uint64>( __STRING__( accountid ) ),	// accountID
                  player->Get<std::string>( __STRING__( account ) ),	// vopenid

                  player->GetKeyID(),	// roleid
                  _invalid_int,	// level
                  _invalid_int,	// viplevel
                  KFDate::GetTimeString( player->Get( __STRING__( birthday ) ) ),	// registerTime
                  _invalid_int,	// rechargeNum
                  player->Get( __STRING__( diamond ) ),	// valueAmount
                  _invalid_int,	// bindAmount
                  player->Get( __STRING__( money ) ),	// iconAmount
                  _invalid_string,	// otherAmount

                  kfresult->_sequence,	// sequence
                  kfsetting->_type,	// goodsType
                  kfsetting->_id,	// iGoodsId
                  _invalid_int,	// uniqueId
                  kfresult->_total_value,	// quantity
                  _invalid_int,	// afterQuantity
                  _invalid_int,	// itemCauseid
                  modulename,	// itemLocation
                  _invalid_string,	// iMoney
                  _invalid_string,	// iMoneyType
                  kfresult->_operate,	// addOrReduce
                  _invalid_string	// custom
                );
    }

    __KF_LOG_ELEMENT_FUNCTION__( KFTLogModule::LogCurrencyElement )
    {
        if ( !kfresult->_element->IsValue() )
        {
            return;
        }

        auto kfglobal = KFGlobal::Instance();
        auto spdlog = CreateLog( __STRING__( onelevel ) );

        auto moneytype = KFCurrencyConfig::Instance()->GetIdByName( kfresult->_element->_data_name );
        auto aftermoney = player->Get( kfresult->_element->_data_name );

        __TLOG__( "MoneyFlow|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}",
                  _log_version,	// logVersion
                  kfglobal->_app_id->ToString(),	// vGameSvrId
                  KFDate::GetTimeString( kfglobal->_real_time ),	// dtEventTime
                  _game_app_id,	// vGameAppid
                  _invalid_int,	// vPlatID
                  kfglobal->_channel,	// vPlatformType
                  _invalid_int,	// registerZoneID
                  _invalid_int,	// currentZoneID
                  player->Get<uint64>( __STRING__( accountid ) ),	// accountID
                  player->Get<std::string>( __STRING__( account ) ),	// vopenid

                  player->GetKeyID(),	// roleid
                  _invalid_int,	// level
                  _invalid_int,	// viplevel
                  KFDate::GetTimeString( player->Get( __STRING__( birthday ) ) ),	// registerTime
                  _invalid_int,	// rechargeNum
                  player->Get( __STRING__( diamond ) ),	// valueAmount
                  _invalid_int,	// bindAmount
                  player->Get( __STRING__( money ) ),	// iconAmount
                  _invalid_string,	// otherAmount

                  kfresult->_sequence,	// sequence
                  aftermoney,	// afterMoney
                  kfresult->_total_value,	// iMoney
                  moneytype,	// iMoneyType
                  _invalid_int,	// moneyCauseid
                  modulename,	// moneyLocation
                  kfresult->_operate,	// addOrReduce
                  _invalid_string	// custom
                );
    }

}