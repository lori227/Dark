#include "KFCampModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFCampModule::BeforeRun()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_EXECUTE__( __STRING__( campupgradelist ), &KFCampModule::OnExecuteCampAddData );
        __REGISTER_EXECUTE__( __STRING__( campdectime ), &KFCampModule::OnExecuteCampAddData );
        __REGISTER_EXECUTE__( __STRING__( campdecconsume ), &KFCampModule::OnExecuteCampAddData );

        __REGISTER_MESSAGE__( KFMsg::MSG_SET_BUILD_SKIN_REQ, &KFCampModule::HandleSetBuildSkinReq );
    }

    void KFCampModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_EXECUTE__( __STRING__( campupgradelist ) );
        __UN_EXECUTE__( __STRING__( campdectime ) );
        __UN_EXECUTE__( __STRING__( campdecconsume ) );

        __UN_MESSAGE__( KFMsg::MSG_SET_BUILD_SKIN_REQ );
    }

    __KF_EXECUTE_FUNCTION__( KFCampModule::OnExecuteCampAddData )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto adddata = executedata->_param_list._params[0]->_int_value;
        player->UpdateData( __STRING__( effect ), executedata->_name, KFEnum::Add, adddata );
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFCampModule::HandleSetBuildSkinReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetBuildSkinReq );

        auto setting = KFBuildSkinConfig::Instance()->FindSetting( kfmsg.id() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MainCampSkinNotExist );
        }

        auto kfbuildskin = player->Find( __STRING__( buildskin ), kfmsg.id() );
        if ( kfbuildskin == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MainCampSkinNotActive );
        }

        player->UpdateData( __STRING__( buildskinid ), KFEnum::Set, kfmsg.id() );
    }
}

