#include "KFMainCampModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFMainCampModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        __REGISTER_UPDATE_DATA_1__( __KF_STRING__( buildlevel ), &KFMainCampModule::OnBuildLevelUpdate );

        __REGISTER_ENTER_PLAYER__( &KFMainCampModule::OnEnterMainCampModule );
        __REGISTER_LEAVE_PLAYER__( &KFMainCampModule::OnLeaveMainCampModule );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_BUILD_SKIN_REQ, &KFMainCampModule::HandleSetBuildSkinReq );
    }

    void KFMainCampModule::BeforeShut()
    {
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SET_BUILD_SKIN_REQ );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFMainCampModule::OnBuildLevelUpdate )
    {
        // 如果军中帐等级更新，更新所有的建筑等级
        if ( key == KFMsg::MainBuild )
        {
            UpdateBuildLevel( player );
        }
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFMainCampModule::OnEnterMainCampModule )
    {
        UpdateBuildLevel( player );
        UpdateBuildSkin( player );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFMainCampModule::OnLeaveMainCampModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    uint32 KFMainCampModule::GetMainCampLevel( KFEntity* player )
    {
        auto kflevel = player->GetData()->FindData( __KF_STRING__( buildlevel ), KFMsg::MainBuild );
        if ( kflevel == nullptr )
        {
            return 0u;
        }

        return kflevel->GetValue();
    }

    void KFMainCampModule::UpdateBuildLevel( KFEntity* player )
    {
        auto kflevelarray = player->GetData()->FindData( __KF_STRING__( buildlevel ) );

        for ( uint32 id = KFMsg::MainBuild; id < KFMsg::MaxBuild && id < kflevelarray->Size(); id++ )
        {
            auto level = kflevelarray->GetValue( id );
            if ( level > 0u )
            {
                continue;
            }

            auto maincamplevel = GetMainCampLevel( player );
            level = KFBuildLevelConfig::Instance()->GetBuildUnlockLevel( id, maincamplevel );
            if ( level > 0u )
            {
                player->UpdateData( id, kflevelarray, id, KFEnum::Set, level );
            }
        }
    }

    void KFMainCampModule::UpdateBuildSkin( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto kfbuildskinrecord = kfobject->FindData( __KF_STRING__( buildskin ) );
        if ( kfbuildskinrecord->Size() > 0 )
        {
            return;
        }

        auto skinid = kfobject->GetValue( __KF_STRING__( buildskinid ) );
        auto kfbuildskin = _kf_kernel->CreateObject( kfbuildskinrecord->_data_setting );
        kfbuildskin->SetValue( __KF_STRING__( id ), skinid );

        player->AddData( kfbuildskinrecord, skinid, kfbuildskin );
    }


    __KF_MESSAGE_FUNCTION__( KFMainCampModule::HandleSetBuildSkinReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetBuildSkinReq );

        auto setting = KFBuildSkinConfig::Instance()->FindSetting( kfmsg.id() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MainCampSkinNotExist );
        }

        auto kfobject = player->GetData();
        auto kfbuildskin = kfobject->FindData( __KF_STRING__( buildskin ), kfmsg.id() );
        if ( kfbuildskin == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MainCampSkinNotActive );
        }

        player->UpdateData( __KF_STRING__( buildskinid ), KFEnum::Set, kfmsg.id() );
    }
}

