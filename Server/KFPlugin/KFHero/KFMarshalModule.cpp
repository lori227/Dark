#include "KFMarshalModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFMarshalModule::BeforeRun()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        __REGISTER_REMOVE_DATA__( __KF_STRING__( hero ), &KFMarshalModule::OnRemoveHero );

        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_TEAM_CHANGE_REQ, &KFMarshalModule::HandleHeroTeamChangeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_TEAM_EXCHANGE_REQ, &KFMarshalModule::HandleHeroTeamExchangeReq );
    }

    void KFMarshalModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_REMOVE_DATA__( __KF_STRING__( hero ) );

        __UN_MESSAGE__( KFMsg::MSG_HERO_TEAM_CHANGE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_HERO_TEAM_EXCHANGE_REQ );
    }

    uint32 KFMarshalModule::GetIndexById( KFEntity* player, uint64 uuid )
    {
        auto kfteamarray = player->GetData()->FindData( __KF_STRING__( heroteam ) );
        for ( uint32 i = KFDataDefine::Array_Index; i < kfteamarray->Size(); i++ )
        {
            auto kfteam = kfteamarray->FindData( i );
            if ( kfteam == nullptr )
            {
                continue;
            }

            if ( kfteam->GetValue<uint64>() == uuid )
            {
                return i;
            }
        }

        return 0u;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_REMOVE_DATA_FUNCTION__( KFMarshalModule::OnRemoveHero )
    {
        auto index = GetIndexById( player, key );
        if ( index == 0u )
        {
            return;
        }

        player->UpdateData( __KF_STRING__( heroteam ), index, KFEnum::Set, 0u );
    }

    __KF_MESSAGE_FUNCTION__( KFMarshalModule::HandleHeroTeamChangeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgHeroTeamChangeReq );

        auto kfobject = player->GetData();

        auto kfteamarray = kfobject->FindData( __KF_STRING__( heroteam ) );
        auto kfteam = kfteamarray->FindData( kfmsg.index() );
        if ( kfteam == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroTeamIndexError );
        }

        auto old_uuid = kfteam->GetValue<uint64>();
        if ( old_uuid == kfmsg.uuid() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroTeamDataInvalid );
        }

        // 查找英雄列表是否存在uuid(uuid为0时表示删除)
        if ( kfmsg.uuid() != 0u )
        {
            auto kfhero = kfobject->FindData( __KF_STRING__( hero ), kfmsg.uuid() );
            if ( kfhero == nullptr )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
            }

            auto pos_flag = kfhero->GetValue<uint32>( __KF_STRING__( posflag ) );
            if ( pos_flag != KFMsg::HeroList )
            {
                // 该英雄不在英雄列表
                return _kf_display->SendToClient( player, KFMsg::HeroNotInHeroList );
            }

            // 更新英雄当前位置
            player->UpdateData( kfhero, __KF_STRING__( posflag ), KFEnum::Set, KFMsg::HeroTeam );
        }

        if ( old_uuid != 0u )
        {
            auto kfhero = kfobject->FindData( __KF_STRING__( hero ), old_uuid );
            if ( kfhero != nullptr )
            {
                // 更新原来英雄当前位置
                player->UpdateData( kfhero, __KF_STRING__( posflag ), KFEnum::Set, KFMsg::HeroList );
            }
        }

        player->UpdateData( kfteamarray, kfmsg.index(), KFEnum::Set, kfmsg.uuid() );
    }

    __KF_MESSAGE_FUNCTION__( KFMarshalModule::HandleHeroTeamExchangeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgHeroTeamExchangeReq );

        auto kfteamarray = player->GetData()->FindData( __KF_STRING__( heroteam ) );
        if ( kfmsg.oldindex() == kfmsg.newindex() ||
                kfmsg.oldindex() >= kfteamarray->Size() ||
                kfmsg.newindex() >= kfteamarray->Size() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroTeamIndexError );
        }

        auto oldvalue = kfteamarray->GetValue( kfmsg.oldindex() );
        auto newvalue = kfteamarray->GetValue( kfmsg.newindex() );
        if ( oldvalue == newvalue )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroTeamDataInvalid );
        }

        player->UpdateData( kfteamarray, kfmsg.oldindex(), KFEnum::Set, newvalue );
        player->UpdateData( kfteamarray, kfmsg.newindex(), KFEnum::Set, oldvalue );
    }

}