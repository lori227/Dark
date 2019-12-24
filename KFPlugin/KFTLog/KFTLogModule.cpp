#include "KFTLogModule.hpp"

namespace KFrame
{
    void KFTLogModule::BeforeRun()
    {
        auto kfglobal = KFGlobal::Instance();
        _spdlog = KFLogger::Instance()->NewLogger( kfglobal->_app_id->GetId(), __STRING__( tlog ), kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->ToString() );
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
        KFLogger::Instance()->DeleteLogger( KFGlobal::Instance()->_app_id->GetId() );
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