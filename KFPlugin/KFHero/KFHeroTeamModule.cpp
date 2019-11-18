#include "KFHeroTeamModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFHeroTeamModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFHeroTeamModule::OnEnterHeroTeamModule );

        __REGISTER_REMOVE_DATA_1__( __STRING__( hero ), &KFHeroTeamModule::OnRemoveHero );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_TEAM_CHANGE_REQ, &KFHeroTeamModule::HandleHeroTeamChangeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_TEAM_EXCHANGE_REQ, &KFHeroTeamModule::HandleHeroTeamExchangeReq );
    }

    void KFHeroTeamModule::BeforeShut()
    {
        __UN_ENTER_PLAYER__();
        __UN_REMOVE_DATA_1__( __STRING__( hero ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_HERO_TEAM_CHANGE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_HERO_TEAM_EXCHANGE_REQ );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFHeroTeamModule::OnEnterHeroTeamModule )
    {
        RemoveTeamDeadHero( player );
    }

    uint32 KFHeroTeamModule::GetIndexById( KFEntity* player, uint64 uuid )
    {
        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        for ( uint32 i = KFDataDefine::Array_Index; i < kfteamarray->Size(); i++ )
        {
            auto kfteam = kfteamarray->Find( i );
            if ( kfteam == nullptr )
            {
                continue;
            }

            if ( kfteam->Get<uint64>() == uuid )
            {
                return i;
            }
        }

        return 0u;
    }

    uint32 KFHeroTeamModule::GetHeroCount( KFEntity* player )
    {
        return player->Get<uint32>( __STRING__( heroteamcount ) );
    }

    uint32 KFHeroTeamModule::GetAliveHeroCount( KFEntity* player )
    {
        auto herocount = 0u;

        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        for ( uint32 i = KFDataDefine::Array_Index; i < kfteamarray->Size(); ++i )
        {
            auto uuid = kfteamarray->Get<uint64>( i );
            auto kfhero = kfherorecord->Find( uuid );
            if ( kfhero == nullptr || kfhero->Get( __STRING__( fighter ), __STRING__( hp ) ) == 0u )
            {
                continue;
            }

            ++herocount;
        }

        return herocount;
    }

    uint32 KFHeroTeamModule::GetDeadHeroCount( KFEntity* player )
    {
        auto herocount = 0u;

        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        for ( uint32 i = KFDataDefine::Array_Index; i < kfteamarray->Size(); ++i )
        {
            auto uuid = kfteamarray->Get<uint64>( i );
            auto kfhero = kfherorecord->Find( uuid );
            if ( kfhero == nullptr || kfhero->Get( __STRING__( fighter ), __STRING__( hp ) ) != 0u )
            {
                continue;
            }

            ++herocount;
        }

        return herocount;
    }

    void KFHeroTeamModule::RemoveTeamDeadHero( KFEntity* player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        for ( uint32 i = KFDataDefine::Array_Index; i < kfteamarray->Size(); ++i )
        {
            auto uuid = kfteamarray->Get<uint64>( i );
            auto kfhero = kfherorecord->Find( uuid );
            if ( kfhero == nullptr )
            {
                continue;
            }

            auto kffighter = kfhero->Find( __STRING__( fighter ) );
            auto hp = kffighter->Get<uint32>( __STRING__( hp ) );
            if ( hp == 0u )
            {
                player->RemoveData( __STRING__( hero ), uuid );
                __LOG_INFO__( "player=[{}] death remove hero=[{}]", player->GetKeyID(), uuid );
            }
            else
            {
                // 重置ep为0
                player->UpdateData( kffighter, __STRING__( ep ), KFEnum::Set, 0u );
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_REMOVE_DATA_FUNCTION__( KFHeroTeamModule::OnRemoveHero )
    {
        auto index = GetIndexById( player, key );
        if ( index == 0u )
        {
            return;
        }

        player->UpdateData( __STRING__( heroteam ), index, KFEnum::Set, 0u );
        player->UpdateData( __STRING__( heroteamcount ), KFEnum::Dec, 1u );
    }

    __KF_MESSAGE_FUNCTION__( KFHeroTeamModule::HandleHeroTeamChangeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgHeroTeamChangeReq );

        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        auto kfteam = kfteamarray->Find( kfmsg.index() );
        if ( kfteam == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroTeamIndexError );
        }

        auto old_uuid = kfteam->Get<uint64>();
        if ( old_uuid == kfmsg.uuid() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroTeamDataInvalid );
        }

        // 查找英雄列表是否存在uuid(uuid为0时表示删除)
        if ( kfmsg.uuid() != 0u )
        {
            auto kfhero = player->Find( __STRING__( hero ), kfmsg.uuid() );
            if ( kfhero == nullptr )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
            }

            auto pos_flag = kfhero->Get<uint32>( __STRING__( posflag ) );
            if ( pos_flag != KFMsg::HeroList )
            {
                // 该英雄不在英雄列表
                return _kf_display->SendToClient( player, KFMsg::HeroNotInHeroList );
            }

            // 更新英雄当前位置
            player->UpdateData( kfhero, __STRING__( posflag ), KFEnum::Set, KFMsg::HeroTeam );

            player->UpdateData( __STRING__( heroteamcount ), KFEnum::Add, 1u );
        }

        if ( old_uuid != 0u )
        {
            auto kfhero = player->Find( __STRING__( hero ), old_uuid );
            if ( kfhero != nullptr )
            {
                // 更新原来英雄当前位置
                player->UpdateData( kfhero, __STRING__( posflag ), KFEnum::Set, KFMsg::HeroList );
            }

            player->UpdateData( __STRING__( heroteamcount ), KFEnum::Dec, 1u );
        }

        player->UpdateData( kfteamarray, kfmsg.index(), KFEnum::Set, kfmsg.uuid() );
    }

    __KF_MESSAGE_FUNCTION__( KFHeroTeamModule::HandleHeroTeamExchangeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgHeroTeamExchangeReq );

        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        if ( kfmsg.oldindex() == kfmsg.newindex() ||
                kfmsg.oldindex() >= kfteamarray->Size() ||
                kfmsg.newindex() >= kfteamarray->Size() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroTeamIndexError );
        }

        auto oldvalue = kfteamarray->Get( kfmsg.oldindex() );
        auto newvalue = kfteamarray->Get( kfmsg.newindex() );
        if ( oldvalue == newvalue )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroTeamDataInvalid );
        }

        player->UpdateData( kfteamarray, kfmsg.oldindex(), KFEnum::Set, newvalue );
        player->UpdateData( kfteamarray, kfmsg.newindex(), KFEnum::Set, oldvalue );
    }

}