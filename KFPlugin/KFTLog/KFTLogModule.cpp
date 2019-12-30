#include "KFTLogModule.hpp"

namespace KFrame
{
    void KFTLogModule::BeforeRun()
    {
        InitTLogData();
        /////////////////////////////////////////////////////////////////////////////
        // 5分钟打印一次服务器状态
        __LOOP_TIMER_1__( 1, 5 * KFTimeEnum::OneMinuteMicSecond, 0, &KFTLogModule::OnTimerLogServerStatus );

        // 3分钟打印一次在线玩家
        __LOOP_TIMER_1__( 2, 3 * KFTimeEnum::OneMinuteMicSecond, 0, &KFTLogModule::OnTimerLogOnlineCount );
        /////////////////////////////////////////////////////////////////////////////
        __REGISTER_NEW_PLAYER__( &KFTLogModule::OnNewTLogModule );
        __REGISTER_ENTER_PLAYER__( &KFTLogModule::OnEnterTLogModule );
        __REGISTER_LEAVE_PLAYER__( &KFTLogModule::OnLeaveTLogModule );
        /////////////////////////////////////////////////////////////////////////////
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_LOG_ELEMENT__( __STRING__( item ), &KFTLogModule::LogItemElement );
        __REGISTER_LOG_ELEMENT__( __STRING__( money ), &KFTLogModule::LogCurrenyElement );
        __REGISTER_LOG_ELEMENT__( __STRING__( diamond ), &KFTLogModule::LogCurrenyElement );
        __REGISTER_LOG_ELEMENT__( __STRING__( supplies ), &KFTLogModule::LogCurrenyElement );
    }

    void KFTLogModule::ShutDown()
    {
        __UN_TIMER_0__();
        /////////////////////////////////////////////////////////////////////////////
        __UN_NEW_PLAYER__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        /////////////////////////////////////////////////////////////////////////////
        __UN_LOG_ELEMENT__( __STRING__( item ) );
        __UN_LOG_ELEMENT__( __STRING__( money ) );
        __UN_LOG_ELEMENT__( __STRING__( diamond ) );
        __UN_LOG_ELEMENT__( __STRING__( supplies ) );
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
        _game_app_id = _kf_project->GetString( "gameid" );
        _log_version = _kf_project->GetUInt64( "logversion" );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFTLogModule::OnTimerLogServerStatus )
    {
        auto kfglobal = KFGlobal::Instance();
        auto spdlog = CreateLog( __STRING__( onelevel ) );

        __TLOG__( "GameSvrState|{}|{}|{}|{}|{}|{}|{}|{}|{}",
                  _log_version,
                  kfglobal->_app_id->ToString(),
                  KFDate::GetTimeString(),
                  _game_app_id,
                  _invalid_int,
                  _kf_ip_address->GetLocalIp(),
                  _invalid_string,
                  kfglobal->GetVersion(),
                  _invalid_string
                )
    }

    __KF_TIMER_FUNCTION__( KFTLogModule::OnTimerLogOnlineCount )
    {
        auto kfglobal = KFGlobal::Instance();
        auto spdlog = CreateLog( __STRING__( onelevel ) );

        KFDate nowdate( kfglobal->_real_time );
        auto hhmm = __FORMAT__( "{}:{}", nowdate.GetHour(), nowdate.GetMinute() );

        __TLOG__( "OnlineCount|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}|{}",
                  _log_version,
                  kfglobal->_app_id->ToString(),
                  KFDate::GetTimeString(),
                  _game_app_id,
                  _invalid_int,
                  _invalid_int,
                  _invalid_int,
                  _kf_component->GetEntityCount(),
                  hhmm,
                  nowdate.GetDay(),
                  kfglobal->_real_time
                )
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NEW_PLAYER_FUNCTION__( KFTLogModule::OnNewTLogModule )
    {
        //TLog( "hhhh" );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFTLogModule::OnEnterTLogModule )
    {

    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFTLogModule::OnLeaveTLogModule )
    {

    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_LOG_ELEMENT_FUNCTION__( KFTLogModule::LogItemElement )
    {

    }

    __KF_LOG_ELEMENT_FUNCTION__( KFTLogModule::LogCurrenyElement )
    {

    }

}