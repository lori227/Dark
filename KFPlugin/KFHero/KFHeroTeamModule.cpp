﻿#include "KFHeroTeamModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFHeroTeamModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFHeroTeamModule::OnEnterHeroTeamModule );
        __REGISTER_REMOVE_DATA_1__( __STRING__( hero ), &KFHeroTeamModule::OnRemoveHero );

        __REGISTER_EXECUTE__( __STRING__( heroteamcount ), &KFHeroTeamModule::OnExecuteTechnologyHeroTeamCount );

        __REGISTER_DROP_LOGIC__( __STRING__( addhp ), &KFHeroTeamModule::OnDropHeroAddHp );
        __REGISTER_DROP_LOGIC__( __STRING__( dechp ), &KFHeroTeamModule::OnDropHeroDecHp );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_TEAM_CHANGE_REQ, &KFHeroTeamModule::HandleHeroTeamChangeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_TEAM_EXCHANGE_REQ, &KFHeroTeamModule::HandleHeroTeamExchangeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_ADD_TEAM_HP_REQ, &KFHeroTeamModule::HandleAddTeamHpReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_DEC_TEAM_HP_REQ, &KFHeroTeamModule::HandleDecTeamHpReq );
    }

    void KFHeroTeamModule::BeforeShut()
    {
        __UN_ENTER_PLAYER__();
        __UN_REMOVE_DATA_1__( __STRING__( hero ) );

        __UN_EXECUTE__( __STRING__( heroteamcount ) );

        __UN_DROP_LOGIC__( __STRING__( addhp ) );
        __UN_DROP_LOGIC__( __STRING__( dechp ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_HERO_TEAM_CHANGE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_HERO_TEAM_EXCHANGE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_ADD_TEAM_HP_REQ );
        __UN_MESSAGE__( KFMsg::MSG_DEC_TEAM_HP_REQ );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFHeroTeamModule::OnEnterHeroTeamModule )
    {
        // 检查英雄队伍数量
        CheckHeroTeamCount( player );

        // 检查队伍里的数据, 如果英雄没在队伍里, 需要清除标记
        CheckHeroInTeam( player );

        // 探索地图内不做处理
        auto realmid = player->Get<uint32>( __STRING__( realmid ) );
        if ( realmid != 0u )
        {
            return;
        }

        UpdateDeadHero( player );
    }

    void KFHeroTeamModule::CheckHeroTeamCount( KFEntity* player )
    {
        static auto _option = KFGlobal::Instance()->FindConstant( "maxheroteamcount" );
        auto heroteamcount = player->Get<uint32>( __STRING__( effect ), __STRING__( heroteamcount ) );
        auto maxteamcount = _option->_uint32_value + heroteamcount;

        // 扩展队伍数量大小
        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        if ( maxteamcount >= kfteamarray->MaxSize() )
        {
            for ( auto i = kfteamarray->MaxSize(); i <= maxteamcount; ++i )
            {
                player->AddArray( kfteamarray, 0u );
            }
        }
    }

    void KFHeroTeamModule::CheckHeroInTeam( KFEntity* player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );

        auto heroteamcount = 0u;
        for ( auto kfhero = kfherorecord->First(); kfhero != nullptr; kfhero = kfherorecord->Next() )
        {
            auto posflag = kfhero->Get<uint32>( __STRING__( posflag ) );
            if ( posflag != KFMsg::HeroTeam )
            {
                continue;
            }

            auto index = kfteamarray->GetIndex( kfhero->GetKeyID() );
            if ( index != 0u )
            {
                ++heroteamcount;
            }
            else
            {
                kfhero->Set<uint32>( __STRING__( posflag ), KFMsg::HeroList );
            }
        }

        player->Set<uint32>( __STRING__( heroteamcount ), heroteamcount );
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
        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto kfhero = kfherorecord->Find( kfteam->Get<uint64>() );
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
        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto kfhero = kfherorecord->Find( kfteam->Get<uint64>() );
            if ( kfhero == nullptr || kfhero->Get( __STRING__( fighter ), __STRING__( hp ) ) != 0u )
            {
                continue;
            }

            ++herocount;
        }

        return herocount;
    }

    void KFHeroTeamModule::UpdateDeadHero( KFEntity* player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );

        UInt64Set removelist;
        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto kfhero = kfherorecord->Find( kfteam->Get() );
            if ( kfhero == nullptr )
            {
                continue;
            }

            auto dead = kfhero->Get<uint32>( __STRING__( dead ) );
            if ( dead > 0u )
            {
                // 删除死亡玩家
                removelist.insert( kfhero->Get( __STRING__( uuid ) ) );
            }
            else
            {
                // 复活空血玩家
                auto kffighter = kfhero->Find( __STRING__( fighter ) );
                auto hp = kffighter->Get<uint32>( __STRING__( hp ) );
                if ( hp == 0u )
                {
                    player->UpdateData( kffighter, __STRING__( hp ), KFEnum::Set, 1u );
                }
            }
        }

        for ( auto iter : removelist )
        {
            player->RemoveData( kfherorecord, iter );
        }
    }

    bool KFHeroTeamModule::IsDurabilityEnough( KFEntity* player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto kfhero = kfherorecord->Find( kfteam->Get() );
            if ( kfhero == nullptr )
            {
                continue;
            }

            if ( kfhero->Get<uint32>( __STRING__( durability ) ) == 0u )
            {
                return false;
            }
        }

        return true;
    }

    void KFHeroTeamModule::DecHeroPVEDurability( KFEntity* player, const UInt64Set& fightheros, uint32 durability )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        for ( auto& iter : fightheros )
        {
            auto kfhero = kfherorecord->Find( iter );
            if ( kfhero == nullptr )
            {
                continue;
            }

            // 扣除指定耐久度
            player->UpdateData( kfhero, __STRING__( durability ), KFEnum::Dec, durability );
        }
    }

    void KFHeroTeamModule::DecHeroRealmDurability( KFEntity* player, uint32 durability, const UInt64Set& excludelist )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto uuid = kfteam->Get();
            if ( uuid == 0u || excludelist.find( uuid ) != excludelist.end() )
            {
                continue;
            }

            auto kfhero = kfherorecord->Find( uuid );
            if ( kfhero == nullptr )
            {
                continue;
            }

            // 扣除指定耐久度
            player->UpdateData( kfhero, __STRING__( durability ), KFEnum::Dec, durability );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_REMOVE_DATA_FUNCTION__( KFHeroTeamModule::OnRemoveHero )
    {
        auto posflag = kfdata->Get<uint32>( __STRING__( posflag ) );
        if ( posflag != KFMsg::HeroTeam )
        {
            return;
        }

        auto kfteamarray = player->Find( __STRING__( heroteam ) );
        auto index = kfteamarray->GetIndex( key );
        if ( index == 0u )
        {
            return;
        }

        player->UpdateData( kfteamarray, index, KFEnum::Set, 0u );
        player->UpdateData( __STRING__( heroteamcount ), KFEnum::Dec, 1u );
    }

    __KF_EXECUTE_FUNCTION__( KFHeroTeamModule::OnExecuteTechnologyHeroTeamCount )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto count = executedata->_param_list._params[0]->_int_value;
        player->UpdateData( __STRING__( effect ), __STRING__( heroteamcount ), KFEnum::Add, count );

        CheckHeroTeamCount( player );

        return true;
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

        auto old_uuid = kfteam->Get();
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
        auto kfoldteam = kfteamarray->Find( kfmsg.oldindex() );
        auto kfnewteam = kfteamarray->Find( kfmsg.newindex() );
        if ( kfmsg.oldindex() == kfmsg.newindex() ||
                kfoldteam == nullptr || kfnewteam == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroTeamIndexError );
        }

        auto oldvalue = kfoldteam->Get();
        auto newvalue = kfnewteam->Get();

        player->UpdateData( kfteamarray, kfmsg.oldindex(), KFEnum::Set, newvalue );
        player->UpdateData( kfteamarray, kfmsg.newindex(), KFEnum::Set, oldvalue );
    }


    __KF_DROP_LOGIC_FUNCTION__( KFHeroTeamModule::OnDropHeroAddHp )
    {
        auto hp = dropdata->GetValue();
        OperateHpValue( player, KFEnum::Add, hp );
        player->AddDataToShow( dropdata->_logic_name, hp, false );
    }

    __KF_DROP_LOGIC_FUNCTION__( KFHeroTeamModule::OnDropHeroDecHp )
    {
        auto hp = dropdata->GetValue();
        OperateHpValue( player, KFEnum::Dec, hp );
        player->AddDataToShow( dropdata->_logic_name, hp, false );
    }

    bool KFHeroTeamModule::CheckHeroConditions( KFData* kfhero, const StringUInt64* conditions )
    {
        if ( conditions != nullptr )
        {
            for ( auto iter = conditions->begin(); iter != conditions->end(); ++iter )
            {
                auto value = kfhero->Get( iter->first );
                if ( value != iter->second )
                {
                    return false;
                }
            }
        }

        return true;
    }

    void KFHeroTeamModule::OperateHpValue( KFEntity* player, uint32 operate, uint32 value, const StringUInt64* conditions /* = nullptr */ )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );

        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto kfhero = _kf_hero->FindAliveHero( kfherorecord, kfteam->Get<uint64>() );
            if ( kfhero == nullptr || !CheckHeroConditions( kfhero, conditions ) )
            {
                continue;
            }

            _kf_hero->OperateHp( player, kfhero, operate, value );
        }
    }

    void KFHeroTeamModule::OperateHpPercent( KFEntity* player, uint32 operate, uint32 value, const StringUInt64* conditions /* = nullptr */ )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );

        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto kfhero = _kf_hero->FindAliveHero( kfherorecord, kfteam->Get<uint64>() );
            if ( kfhero == nullptr || !CheckHeroConditions( kfhero, conditions ) )
            {
                continue;
            }

            auto maxhp = kfhero->Get<uint32>( __STRING__( fighter ), __STRING__( maxhp ) );
            value = maxhp * value / KFRandEnum::TenThousand;
            _kf_hero->OperateHp( player, kfhero, operate, value );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFHeroTeamModule::HandleAddTeamHpReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgAddTeamHpReq );

        StringUInt64 conditions;
        auto pbconditions = &kfmsg.conditions();
        __PROTO_TO_MAP__( pbconditions, conditions );

        if ( kfmsg.hpvalue() != 0u )
        {
            OperateHpValue( player, KFEnum::Add, kfmsg.hpvalue(), &conditions );
        }
        else if ( kfmsg.hppecent() != 0u )
        {
            OperateHpPercent( player, KFEnum::Add, kfmsg.hppecent(), &conditions );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFHeroTeamModule::HandleDecTeamHpReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDecTeamHpReq );

        StringUInt64 conditions;
        auto pbconditions = &kfmsg.conditions();
        __PROTO_TO_MAP__( pbconditions, conditions );

        if ( kfmsg.hpvalue() != 0u )
        {
            OperateHpValue( player, KFEnum::Dec, kfmsg.hpvalue(), &conditions );
        }
        else if ( kfmsg.hppecent() != 0u )
        {
            OperateHpPercent( player, KFEnum::Dec, kfmsg.hppecent(), &conditions );
        }
    }
}
