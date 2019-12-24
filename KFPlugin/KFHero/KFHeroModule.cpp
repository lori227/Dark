﻿#include "KFHeroModule.hpp"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    void KFHeroModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFHeroModule::OnEnterHeroModule );

        __REGISTER_ADD_ELEMENT__( __STRING__( hero ), &KFHeroModule::AddHeroElement );
        __REGISTER_REMOVE_ELEMENT__( __STRING__( hero ), &KFHeroModule::RemoveHeroElement );
        __REGISTER_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( exp ), &KFHeroModule::OnHeroExpUpdate );
        __REGISTER_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( active ), &KFHeroModule::OnHeroActiveUpdate );

        __REGISTER_EXECUTE__( __STRING__( maxherocount ), &KFHeroModule::OnExecuteTechnologyMaxHeroCount );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_LOCK_HERO_REQ, &KFHeroModule::HandleLockHeroReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_HERO_REQ, &KFHeroModule::HandleRemoveHeroReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_HERO_NAME_REQ, &KFHeroModule::HandleSetHeroNameReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_HERO_ACTIVE_SKILL_REQ, &KFHeroModule::HandleSetHeroActiveSkillReq );
    }

    void KFHeroModule::BeforeShut()
    {
        __UN_ENTER_PLAYER__();

        __UN_ADD_ELEMENT__( __STRING__( hero ) );
        __UN_REMOVE_ELEMENT__( __STRING__( hero ) );
        __UN_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( exp ) );
        __UN_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( active ) );

        __UN_EXECUTE__( __STRING__( maxherocount ) );
        /////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_LOCK_HERO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REMOVE_HERO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_HERO_NAME_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_HERO_ACTIVE_SKILL_REQ );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    __KF_ENTER_PLAYER_FUNCTION__( KFHeroModule::OnEnterHeroModule )
    {
        auto& version = KFLevelConfig::Instance()->GetVersion();		// 配置版本号
        auto levelversion = player->Get<std::string>( __STRING__( levelversion ) );
        if ( levelversion == version )
        {
            return;
        }
        player->Set( __STRING__( levelversion ), version );

        auto kfherorecord = player->Find( __STRING__( hero ) );
        for ( auto kfhero = kfherorecord->First(); kfhero != nullptr; kfhero = kfherorecord->Next() )
        {
            auto exp = kfhero->Get<uint32>( __STRING__( exp ) );
            auto level = kfhero->Get<uint32>( __STRING__( level ) );
            auto curlevel = KFLevelConfig::Instance()->GetLevelByExp( exp, level );
            if ( curlevel != level )
            {
                player->UpdateData( kfhero, __STRING__( level ), KFEnum::Set, curlevel );
            }
        }
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFHeroModule::AddHeroElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
        }
        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );

        // 已经存在的英雄
        auto kfuuid = kfelementobject->_values.Find( __STRING__( uuid ) );
        if ( kfuuid != nullptr )
        {
            auto uuid = kfuuid->CalcUseValue( nullptr, 1.0f );
            auto kfhero = kfparent->Find( uuid );
            if ( kfhero == nullptr )
            {
                return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] uuid[{}] hero not exist!", kfelement->_data_name, uuid );
            }

            // 更新数据
            return player->UpdateElementToData( kfelementobject, kfhero, multiple );
        }

        if ( kfelementobject->_config_id == 0u )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] id!", kfelement->_data_name );
        }

        // 创建新的英雄
        auto kfhero = player->CreateData( kfparent );
        auto rethero = _kf_generate->GeneratePlayerHero( player, kfhero, kfelementobject->_config_id );
        if ( rethero == nullptr )
        {
            return;
        }

        // 有设定属性
        player->SetElementToData( kfelementobject, kfhero, multiple );

        // 创建guid
        auto uuid = KFGlobal::Instance()->STMakeUUID( __STRING__( hero ) );
        player->AddData( kfparent, uuid, kfhero );

        // 添加结果
        kfresult->AddResult( kfhero );
    }

    __KF_REMOVE_ELEMENT_FUNCTION__( KFHeroModule::RemoveHeroElement )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
            return;
        }
        auto kfelementobject = reinterpret_cast<KFElementObject*>( kfelement );

        auto kfuuid = kfelementobject->_values.Find( __STRING__( uuid ) );
        if ( kfuuid == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] uuid not exist!", kfelement->_data_name );
            return;
        }

        auto uuid = kfuuid->CalcUseValue( nullptr, 1.0f );
        auto kfhero = kfparent->Find( uuid );
        if ( kfhero == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] uuid[{}] hero not exist!", kfelement->_data_name, uuid );
            return;
        }

        // 更新数据
        player->UpdateElementToData( kfelementobject, kfhero, multiple );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFHeroModule::OnHeroExpUpdate )
    {
        auto kfhero = kfdata->GetParent();
        if ( kfhero == nullptr )
        {
            return;
        }

        auto exp = kfdata->Get<uint32>();
        auto level = kfhero->Get<uint32>( __STRING__( level ) );
        auto curlevel = KFLevelConfig::Instance()->GetLevelByExp( exp, level );
        if ( curlevel != level )
        {
            player->UpdateData( kfhero, __STRING__( level ), KFEnum::Set, curlevel );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFHeroModule::OnHeroActiveUpdate )
    {
        auto kfhero = kfdata->GetParent()->GetParent();
        if ( kfhero == nullptr )
        {
            return;
        }

        // 当英雄没装备技能时，获得的技能自动装备
        if ( kfhero->Get( __STRING__( activeindex ) ) == 0u )
        {
            player->UpdateData( kfhero, __STRING__( activeindex ), KFEnum::Set, key );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFHeroModule::HandleLockHeroReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgLockHeroReq );

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        if ( kfmsg.oper() >= KFMsg::CountLock )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroLockFlagError );
        }

        auto lock = kfhero->Get<uint32>( __STRING__( lock ) );
        if ( lock == kfmsg.oper() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroLockNotChange );
        }

        player->UpdateData( kfhero, __STRING__( lock ), KFEnum::Set, kfmsg.oper() );
    }

    __KF_MESSAGE_FUNCTION__( KFHeroModule::HandleRemoveHeroReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveHeroReq );

        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfhero = kfherorecord->Find( kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto lock = kfhero->Get<uint32>( __STRING__( lock ) );
        if ( lock != KFMsg::NoLock )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroIsLocked );
        }

        auto posflag = kfhero->Get<uint32>( __STRING__( posflag ) );
        if ( posflag != KFMsg::HeroList )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotInHeroList );
        }

        player->RemoveData( kfherorecord, kfmsg.uuid() );
        __LOG_INFO__( "player=[{}] remove hero=[{}]", player->GetKeyID(), kfmsg.uuid() );
    }

    __KF_MESSAGE_FUNCTION__( KFHeroModule::HandleSetHeroNameReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetHeroNameReq );

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        if ( kfmsg.name().empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNameEmpty );
        }

        auto kfname = kfhero->Find( __STRING__( name ) );
        if ( kfmsg.name().size() > kfname->_data_setting->_int_max_value )
        {
            return _kf_display->SendToClient( player, KFMsg::NameLengthError );
        }

        if ( kfmsg.name() == kfname->Get<std::string>() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNameNotChange );
        }

        auto ok = _kf_filter->CheckFilter( kfmsg.name() );
        if ( ok )
        {
            return _kf_display->SendToClient( player, KFMsg::NameFilterError );
        }

        player->UpdateData( kfhero, __STRING__( name ), kfmsg.name() );
    }

    __KF_MESSAGE_FUNCTION__( KFHeroModule::HandleSetHeroActiveSkillReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetHeroActiveSkillReq );

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto kfactive = kfhero->Find( __STRING__( active ), kfmsg.index() );
        if ( kfactive == nullptr || kfactive->Get<uint32>() == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroSkillIndexError );
        }

        auto activeindex = kfhero->Get<uint32>( __STRING__( activeindex ) );
        if ( activeindex == kfmsg.index() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroSkillNotChange );
        }

        player->UpdateData( kfhero, __STRING__( activeindex ), KFEnum::Set, kfmsg.index() );
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    __KF_EXECUTE_FUNCTION__( KFHeroModule::OnExecuteTechnologyMaxHeroCount )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto count = executedata->_param_list._params[ 0 ]->_int_value;
        player->UpdateData( __STRING__( effect ), __STRING__( maxherocount ), KFEnum::Add, count );
        return true;
    }

    bool KFHeroModule::IsFull( KFEntity* player, KFData* kfherorecord )
    {
        auto count = kfherorecord->Size();
        auto maxcount = kfherorecord->MaxSize();
        maxcount += player->Get<uint32>( __STRING__( effect ), __STRING__( maxherocount ) );
        return count >= maxcount;
    }

    uint32 KFHeroModule::AddHeroData( KFEntity* player, KFData* kfhero, const std::string& name, int32 value )
    {
        if ( name == __STRING__( hp ) )
        {
            return AddHp( player, kfhero, abs( value ) );
        }

        if ( name == __STRING__( exp ) )
        {
            return AddExp( player, kfhero, abs( value ) );
        }

        auto kfdata = kfhero->Find( name );
        if ( kfdata == nullptr )
        {
            kfdata = kfdata->Find( __STRING__( fighter ), name );
            if ( kfdata == nullptr )
            {
                return 0u;
            }
        }

        if ( value >= 0 )
        {
            player->UpdateData( kfdata, KFEnum::Add, abs( value ) );

        }
        else
        {
            player->UpdateData( kfdata, KFEnum::Dec, abs( value ) );
        }

        return abs( value );
    }

    uint32 KFHeroModule::AddExp( KFEntity* player, KFData* kfhero, uint32 exp )
    {
        auto profession = kfhero->Get( __STRING__( profession ) );
        auto kfsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfsetting == nullptr )
        {
            return 0u;
        }

        auto level = kfhero->Get( __STRING__( level ) );
        if ( level >= kfsetting->_max_level )
        {
            // 英雄已满级
            return 0u;
        }

        auto hp = kfhero->Get( __STRING__( fighter ), __STRING__( hp ) );
        if ( hp == 0u )
        {
            // 英雄已经死亡
            return 0u;
        }

        auto curexp = kfhero->Get( __STRING__( exp ) );
        auto maxexp = KFLevelConfig::Instance()->FindSetting( kfsetting->_max_level )->_exp;
        if ( curexp + exp > maxexp )
        {
            // 经验溢出
            exp = maxexp - curexp;
        }

        player->UpdateData( kfhero, __STRING__( exp ), KFEnum::Add, exp );
        return exp;
    }

    bool KFHeroModule::IsMaxLevel( KFData* kfhero )
    {
        auto profession = kfhero->Get( __STRING__( profession ) );
        auto kfsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        auto level = kfhero->Get( __STRING__( level ) );
        return level >= kfsetting->_max_level;
    }

    uint32 KFHeroModule::AddHp( KFEntity* player, KFData* kfhero, uint32 hp )
    {
        auto kffighter = kfhero->Find( __STRING__( fighter ) );
        auto maxhp = kffighter->Get<uint32>( __STRING__( maxhp ) );

        auto kfhp = kffighter->Find( __STRING__( hp ) );
        auto nowhp = kfhp->Get<uint32>();

        auto addhp = maxhp - nowhp;
        addhp = __MIN__( addhp, hp );
        if ( addhp > 0u )
        {
            player->UpdateData( kfhp, KFEnum::Add, addhp );
        }

        return addhp;
    }

    KFData* KFHeroModule::FindAliveHero( KFData* kfherorecord, uint64 uuid )
    {
        auto kfhero = kfherorecord->Find( uuid );
        if ( kfhero == nullptr )
        {
            return nullptr;
        }

        auto hp = kfhero->Get<uint32>( __STRING__( fighter ), __STRING__( hp ) );
        if ( hp == 0u )
        {
            return nullptr;
        }

        return kfhero;
    }
    uint32 KFHeroModule::GetHeroDeathReason( KFData* kfhero )
    {
        if ( kfhero == nullptr )
        {
            return KFMsg::NoExist;
        }

        if ( kfhero->Get<uint32>( __STRING__( durability ) ) == 0u )
        {
            return KFMsg::NoEnoughDurability;
        }

        return KFMsg::NoDeathReason;
    }
}