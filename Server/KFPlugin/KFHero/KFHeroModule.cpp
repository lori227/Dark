#include "KFHeroModule.hpp"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    void KFHeroModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );

        __REGISTER_ADD_ELEMENT__( __KF_STRING__( hero ), &KFHeroModule::AddHeroElement );
        __REGISTER_UPDATE_DATA_2__( __KF_STRING__( hero ), __KF_STRING__( exp ), &KFHeroModule::OnHeroExpUpdate );
        __REGISTER_UPDATE_DATA_2__( __KF_STRING__( fighter ), __KF_STRING__( maxhp ), &KFHeroModule::OnHeroMaxhpUpdate );

        ////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_LOCK_HERO_REQ, &KFHeroModule::HandleLockHeroReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_HERO_REQ, &KFHeroModule::HandleRemoveHeroReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_HERO_NAME_REQ, &KFHeroModule::HandleSetHeroNameReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_HERO_ACTIVE_SKILL_REQ, &KFHeroModule::HandleSetHeroActiveSkillReq );
    }

    void KFHeroModule::BeforeShut()
    {
        __UN_ADD_ELEMENT__( __KF_STRING__( hero ) );
        __UN_UPDATE_DATA_2__( __KF_STRING__( hero ), __KF_STRING__( exp ) );
        __UN_UPDATE_DATA_2__( __KF_STRING__( fighter ), __KF_STRING__( maxhp ) );

        /////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_LOCK_HERO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REMOVE_HERO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_HERO_NAME_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_HERO_ACTIVE_SKILL_REQ );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFHeroModule::AddHeroElement )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }
        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );

        // 已经存在的英雄
        auto kfuuid = kfelementobject->_values.Find( __KF_STRING__( uuid ) );
        if ( kfuuid != nullptr )
        {
            auto uuid = kfuuid->CalcUseValue( nullptr, 1.0f );
            auto kfhero = kfparent->FindData( uuid );
            if ( kfhero == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "element=[{}] uuid[{}] hero not exist!", kfelement->_data_name, uuid );
                return std::make_tuple( KFDataDefine::Show_None, nullptr );
            }

            // 更新数据
            player->UpdateElementToData( kfelementobject, kfhero, multiple );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        // 创建guid
        auto uuid = KFGlobal::Instance()->MakeUUID( KFMsg::UUidHero );

        // 创建新的英雄
        auto kfhero = _kf_kernel->CreateObject( kfparent->_data_setting );

        auto kfgenerate = kfelementobject->_values.Find( __KF_STRING__( generateid ) );
        if ( kfgenerate != nullptr )
        {
            auto generateid = kfgenerate->CalcUseValue( nullptr, 1.0f );
            auto* rethero = _kf_generate->GeneratePlayerHero( player, kfhero, generateid, false, function, line );
            if ( rethero == nullptr )
            {
                _kf_kernel->ReleaseObject( kfhero );
                return std::make_tuple( KFDataDefine::Show_None, nullptr );
            }
        }

        // 有设定属性
        player->SetElementToData( kfelementobject, kfhero, multiple );

        // 添加英雄
        player->AddData( kfparent, uuid, kfhero );
        return std::make_tuple( KFDataDefine::Show_Data, kfhero );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFHeroModule::OnHeroExpUpdate )
    {
        auto kfhero = kfdata->GetParent();
        if ( kfhero == nullptr )
        {
            return;
        }

        auto exp = kfdata->GetValue<uint32>();
        auto level = kfhero->GetValue<uint32>( __KF_STRING__( level ) );
        auto curlevel = KFLevelConfig::Instance()->GetLevelByExp( exp, level );
        if ( curlevel != level )
        {
            player->UpdateData( kfhero, __KF_STRING__( level ), KFEnum::Set, curlevel );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFHeroModule::OnHeroMaxhpUpdate )
    {
        if ( operate == KFEnum::Add )
        {
            auto kffighter = kfdata->GetParent();
            player->UpdateData( kffighter, __KF_STRING__( hp ), KFEnum::Add, value );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFHeroModule::HandleLockHeroReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgLockHeroReq );

        auto kfhero = player->GetData()->FindData( __KF_STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        if ( kfmsg.oper() >= KFMsg::CountLock )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroLockFlagError );
        }

        auto lock = kfhero->GetValue<uint32>( __KF_STRING__( lock ) );
        if ( lock == kfmsg.oper() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroLockNotChange );
        }

        player->UpdateData( kfhero, __KF_STRING__( lock ), KFEnum::Set, kfmsg.oper() );
    }

    __KF_MESSAGE_FUNCTION__( KFHeroModule::HandleRemoveHeroReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveHeroReq );

        auto kfherorecord = player->GetData()->FindData( __KF_STRING__( hero ) );
        auto kfhero = kfherorecord->FindData( kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto lock = kfhero->GetValue<uint32>( __KF_STRING__( lock ) );
        if ( lock != KFMsg::NoLock )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroIsLocked );
        }

        auto posflag = kfhero->GetValue<uint32>( __KF_STRING__( posflag ) );
        if ( posflag != KFMsg::HeroList )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotInHeroList );
        }

        player->RemoveData( kfherorecord, kfmsg.uuid() );
    }

    __KF_MESSAGE_FUNCTION__( KFHeroModule::HandleSetHeroNameReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetHeroNameReq );

        auto kfhero = player->GetData()->FindData( __KF_STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        if ( kfmsg.name().empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::NameEmpty );
        }

        auto kfname = kfhero->FindData( __KF_STRING__( name ) );
        if ( kfmsg.name().size() > kfname->_data_setting->_int_max_value )
        {
            return _kf_display->SendToClient( player, KFMsg::NameLengthError );
        }

        if ( kfmsg.name() == kfname->GetValue<std::string>() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNameNotChange );
        }

        auto ok = _kf_filter->CheckFilter( kfmsg.name() );
        if ( ok )
        {
            return _kf_display->SendToClient( player, KFMsg::NameFilterError );
        }

        player->UpdateData( kfhero, __KF_STRING__( name ), kfmsg.name() );
    }

    __KF_MESSAGE_FUNCTION__( KFHeroModule::HandleSetHeroActiveSkillReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetHeroActiveSkillReq );

        auto kfhero = player->GetData()->FindData( __KF_STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto kfactive = kfhero->FindData( __KF_STRING__( active ), kfmsg.index() );
        if ( kfactive == nullptr || kfactive->GetValue<uint32>() == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroSkillIndexError );
        }

        auto activeindex = kfhero->GetValue<uint32>( __KF_STRING__( activeindex ) );
        if ( activeindex == kfmsg.index() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroSkillNotChange );
        }

        player->UpdateData( kfhero, __KF_STRING__( activeindex ), KFEnum::Set, kfmsg.index() );
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    uint32 KFHeroModule::AddHeroData( KFEntity* player, KFData* kfhero, const std::string& name, int32 value )
    {
        if ( name == __KF_STRING__( hp ) )
        {
            return AddHp( player, kfhero, abs( value ) );
        }

        if ( name == __KF_STRING__( exp ) )
        {
            return AddExp( player, kfhero, abs( value ) );
        }

        auto kfdata = kfhero->FindData( name );
        if ( kfdata == nullptr )
        {
            kfdata = kfdata->FindData( __KF_STRING__( fighter ), name );
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
        auto profession = kfhero->GetValue( __KF_STRING__( profession ) );
        auto kfsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfsetting == nullptr )
        {
            return 0u;
        }

        auto level = kfhero->GetValue( __KF_STRING__( level ) );
        if ( level >= kfsetting->_max_level )
        {
            // 英雄已满级
            return 0u;
        }

        auto curexp = kfhero->GetValue( __KF_STRING__( exp ) );
        auto maxexp = KFLevelConfig::Instance()->FindSetting( kfsetting->_max_level )->_exp;
        if ( curexp + exp > maxexp )
        {
            // 经验溢出
            exp = maxexp - curexp;
        }

        player->UpdateData( kfhero, __KF_STRING__( exp ), KFEnum::Add, exp );
        return exp;
    }

    bool KFHeroModule::IsMaxLevel( KFData* kfhero )
    {
        auto profession = kfhero->GetValue( __KF_STRING__( profession ) );
        auto kfsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        auto level = kfhero->GetValue( __KF_STRING__( level ) );
        return level >= kfsetting->_max_level;
    }

    uint32 KFHeroModule::AddHp( KFEntity* player, KFData* kfhero, uint32 hp )
    {
        auto kffighter = kfhero->FindData( __KF_STRING__( fighter ) );
        auto maxhp = kffighter->GetValue<uint32>( __KF_STRING__( maxhp ) );

        auto kfhp = kffighter->FindData( __KF_STRING__( hp ) );
        auto nowhp = kfhp->GetValue();

        auto addhp = maxhp - nowhp;
        addhp = __MIN__( addhp, hp );
        if ( addhp > 0u )
        {
            player->UpdateData( kfhp, KFEnum::Add, addhp );
        }

        return addhp;
    }
}