#include "KFHeroTeamModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFHeroTeamModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFHeroTeamModule::OnEnterHeroTeamModule );
        __REGISTER_REMOVE_DATA_1__( __STRING__( hero ), &KFHeroTeamModule::OnRemoveHero );

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
        // 检查队伍里的数据, 如果英雄没在队伍里, 需要清除标记
        CheckHeroInTeam( player );

        UpdateDeadHero( player );

        // 探索地图内不做处理
        auto realmid = player->Get<uint32>( __STRING__( realmid ) );
        if ( realmid != 0u )
        {
            return;
        }

        // 清空队伍英雄ep
        ClearHeroEp( player );
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
        // 随机死亡性格池
        static auto _option = _kf_option->FindOption( "roledeath_characterpool" );

        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );

        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto kfhero = kfherorecord->Find( kfteam->Get<uint64>() );
            if ( kfhero == nullptr )
            {
                continue;
            }

            auto kffighter = kfhero->Find( __STRING__( fighter ) );
            auto hp = kffighter->Get<uint32>( __STRING__( hp ) );
            if ( hp == 0u )
            {
                player->UpdateData( kffighter, __STRING__( hp ), KFEnum::Set, 1u );

                UInt32Vector character_pool_list;
                character_pool_list.push_back( _option->_uint32_value );
                _kf_generate->RandWeightData( player, kfhero, __STRING__( character ), character_pool_list );
            }
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

    void KFHeroTeamModule::DecHeroRealmDurability( KFEntity* player, uint32 durability )
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

            // 扣除指定耐久度
            player->UpdateData( kfhero, __STRING__( durability ), KFEnum::Dec, durability );
        }
    }

    void KFHeroTeamModule::ClearHeroEp( KFEntity* player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfteamarray = player->Find( __STRING__( heroteam ) );

        for ( auto kfteam = kfteamarray->First(); kfteam != nullptr; kfteam = kfteamarray->Next() )
        {
            auto kfhero = kfherorecord->Find( kfteam->Get<uint64>() );
            if ( kfhero == nullptr )
            {
                continue;
            }

            // 重置ep为0
            auto kffighter = kfhero->Find( __STRING__( fighter ) );
            auto ep = kffighter->Get<uint32>( __STRING__( ep ) );
            if ( ep != 0u )
            {
                player->UpdateData( kffighter, __STRING__( ep ), KFEnum::Set, 0u );
            }
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
                kfmsg.oldindex() == 0u ||
                kfmsg.newindex() == 0u ||
                kfmsg.oldindex() >= kfteamarray->MaxSize() ||
                kfmsg.newindex() >= kfteamarray->MaxSize() )
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

    bool KFHeroTeamModule::CheckHeroConditions( KFData* kfhero, const KeyValue* conditions )
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

    void KFHeroTeamModule::OperateHpValue( KFEntity* player, uint32 operate, uint32 value, const KeyValue* conditions /* = nullptr */ )
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

    void KFHeroTeamModule::OperateHpPercent( KFEntity* player, uint32 operate, uint32 value, const KeyValue* conditions /* = nullptr */ )
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

            auto nowhp = kfhero->Get<uint32>( __STRING__( fighter ), __STRING__( hp ) );
            value = nowhp * value / KFRandEnum::TenThousand;
            _kf_hero->OperateHp( player, kfhero, operate, value );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFHeroTeamModule::HandleAddTeamHpReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgAddTeamHpReq );

        KeyValue conditions;
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

        KeyValue conditions;
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
