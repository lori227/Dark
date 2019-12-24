#include "KFCampModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFCampModule::BeforeRun()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_BUILD_SKIN_REQ, &KFCampModule::HandleSetBuildSkinReq );
    }

    void KFCampModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SET_BUILD_SKIN_REQ );
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

