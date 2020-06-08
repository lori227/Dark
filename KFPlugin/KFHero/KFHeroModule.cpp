#include "KFHeroModule.hpp"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    void KFHeroModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFHeroModule::OnEnterHeroModule );

        __REGISTER_ADD_ELEMENT__( __STRING__( hero ), &KFHeroModule::AddHeroElement );
        __REGISTER_CHECK_REMOVE_ELEMENT__( __STRING__( hero ), &KFHeroModule::CheckRemoveHeroElement );
        __REGISTER_REMOVE_ELEMENT__( __STRING__( hero ), &KFHeroModule::RemoveHeroElement );

        __REGISTER_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( exp ), &KFHeroModule::OnHeroExpUpdate );
        __REGISTER_ADD_DATA_1__( __STRING__( active ), &KFHeroModule::OnHeroActiveUpdate );

        __REGISTER_EXECUTE__( __STRING__( addhp ), &KFHeroModule::OnExecuteAddHp );
        __REGISTER_EXECUTE__( __STRING__( dechp ), &KFHeroModule::OnExecuteDecHp );
        __REGISTER_EXECUTE__( __STRING__( herolevel ), &KFHeroModule::OnExecuteTechnologyHeroLevel );
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
        __UN_CHECK_REMOVE_ELEMENT__( __STRING__( hero ) );

        __UN_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( exp ) );
        __UN_UPDATE_DATA_1__( __STRING__( active ) );

        __UN_EXECUTE__( __STRING__( addhp ) );
        __UN_EXECUTE__( __STRING__( dechp ) );
        __UN_EXECUTE__( __STRING__( herolevel ) );
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
            auto level = kfhero->Get<uint32>( __STRING__( level ) );
            auto kflevelsetting = KFLevelConfig::Instance()->FindSetting( level + 1u );
            if ( kflevelsetting == nullptr )
            {
                continue;
            }

            auto totalexp = kfhero->Get<uint32>( __STRING__( totalexp ) );
            if ( totalexp != 0u && kflevelsetting->_exp != totalexp )
            {
                auto exp = kfhero->Get<uint32>( __STRING__( exp ) );
                auto newexp = static_cast<double>( exp ) / totalexp * kflevelsetting->_exp;

                player->UpdateData( kfhero, __STRING__( exp ), KFEnum::Set, newexp );
                player->UpdateData( kfhero, __STRING__( totalexp ), KFEnum::Set, kflevelsetting->_exp );
            }
        }
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFHeroModule::AddHeroElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }
        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );

#ifdef __KF_DEBUG__
        // 已经存在的英雄
        auto kfuuid = kfelementobject->_values.Find( __STRING__( uuid ) );
        if ( kfuuid != nullptr )
        {
            auto uuid = kfuuid->CalcUseValue( nullptr, 1.0f );
            auto kfhero = kfparent->Find( uuid );
            if ( kfhero == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "element=[{}] uuid[{}] hero not exist", kfelement->_data_name, uuid );
                return false;
            }

            // 更新数据
            player->UpdateElementToData( kfhero, kfelementobject, kfresult->_multiple );
            return true;
        }
#endif // __KF_DEBUG__

        if ( kfelementobject->_config_id == 0u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] id", kfelement->_data_name );
            return false;
        }

        // 创建新的英雄
        auto kfhero = player->CreateData( kfparent );
        auto rethero = _kf_generate->GeneratePlayerHero( player, kfhero, kfelementobject->_config_id );
        if ( rethero == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "generate hero failed, id = [{}] ", kfelementobject->_config_id );
            return false;
        }

        // 有设定属性
        player->SetElementToData( kfhero, kfelementobject, kfresult->_multiple );

        // 创建guid
        auto uuid = KFGlobal::Instance()->STMakeUuid( __STRING__( hero ) );
        player->AddData( kfparent, uuid, kfhero );

        // 添加结果
        return kfresult->AddResult( kfelementobject->_config_id, kfhero );
    }

    __KF_CHECK_REMOVE_ELEMENT_FUNCTION__( KFHeroModule::CheckRemoveHeroElement )
    {
        return true;
    }

    __KF_REMOVE_ELEMENT_FUNCTION__( KFHeroModule::RemoveHeroElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }
        auto kfelementobject = reinterpret_cast<KFElementObject*>( kfelement );

        auto kfuuid = kfelementobject->_values.Find( __STRING__( uuid ) );
        if ( kfuuid == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] uuid not exist", kfelement->_data_name );
            return false;
        }

        auto uuid = kfuuid->CalcUseValue( nullptr, 1.0f );
        auto kfhero = kfparent->Find( uuid );
        if ( kfhero == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] uuid[{}] hero not exist", kfelement->_data_name, uuid );
            return false;
        }

        // 更新数据
        player->UpdateElementToData( kfhero, kfelementobject, kfresult->_multiple );
        return true;
    }

    __KF_UPDATE_DATA_FUNCTION__( KFHeroModule::OnHeroExpUpdate )
    {
        // GM使用
        if ( operate != KFEnum::Add )
        {
            return;
        }

        auto kfhero = kfdata->GetParent();
        if ( kfhero == nullptr )
        {
            return;
        }

        AddExp( player, kfhero, 0u );
    }

    void KFHeroModule::UpdateAllHeroMaxLevel( KFEntity* player )
    {
        // 更新英雄列表最大等级
        auto kfherorecord = player->Find( __STRING__( hero ) );
        for ( auto kfhero = kfherorecord->First(); kfhero != nullptr; kfhero = kfherorecord->Next() )
        {
            auto maxlevel = CalcMaxLevel( player, kfhero );
            auto curmaxlevel = kfhero->Get<uint32>( __STRING__( maxlevel ) );
            if ( curmaxlevel != maxlevel )
            {
                player->UpdateData( kfhero, __STRING__( maxlevel ), KFEnum::Set, maxlevel );
            }
        }

        // 更新招募列表最大等级
        auto kfrecruitrecord = player->Find( __STRING__( recruit ) );
        for ( auto kfrecruit = kfrecruitrecord->First(); kfrecruit != nullptr; kfrecruit = kfrecruitrecord->Next() )
        {
            auto kfhero = kfrecruit->Find( __STRING__( hero ) );
            if ( kfhero == nullptr )
            {
                continue;
            }

            auto maxlevel = CalcMaxLevel( player, kfhero );
            auto curmaxlevel = kfhero->Get<uint32>( __STRING__( maxlevel ) );
            if ( curmaxlevel != maxlevel )
            {
                player->UpdateData( kfhero, __STRING__( maxlevel ), KFEnum::Set, maxlevel );
            }
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFHeroModule::OnHeroActiveUpdate )
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

        auto custom = kfhero->Get<uint32>( __STRING__( custom ) );
        if ( custom > 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::SpecialHeroCanNotRemove );
        }

        auto posflag = kfhero->Get<uint32>( __STRING__( posflag ) );
        if ( posflag != KFMsg::HeroList )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotInHeroList );
        }

        // 设置解雇标记
        kfhero->Set<uint32>( __STRING__( lock ), KFMsg::Dismissal );
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
        if ( kfactive == nullptr )
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

    __KF_EXECUTE_FUNCTION__( KFHeroModule::OnExecuteAddHp )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "addhp execute param size<1" );
            return false;
        }

        auto herouuid = executedata->_calc_value;
        if ( herouuid == 0u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "addhp herouuid is null" );
            return false;
        }

        auto kfhero = player->Find( __STRING__( hero ), herouuid );
        if ( kfhero == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "addhp hero is null" );
            return false;
        }

        auto hp = executedata->_param_list._params[0]->_int_value;
        OperateHp( player, kfhero, KFEnum::Add, hp );

        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFHeroModule::OnExecuteDecHp )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "dechp execute param size<1" );
            return false;
        }

        auto herouuid = executedata->_calc_value;
        if ( herouuid == 0u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "dechp herouuid is null" );
            return false;
        }

        auto kfhero = player->Find( __STRING__( hero ), herouuid );
        if ( kfhero == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "dechp hero is null" );
            return false;
        }

        auto hp = executedata->_param_list._params[0]->_int_value;
        OperateHp( player, kfhero, KFEnum::Dec, hp );

        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFHeroModule::OnExecuteTechnologyHeroLevel )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto level = executedata->_param_list._params[0]->_int_value;
        player->UpdateData( __STRING__( effect ), __STRING__( herolevel ), KFEnum::Add, level );

        UpdateAllHeroMaxLevel( player );

        return true;
    }

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
        auto operatevalue = abs( value );
        auto operate = ( value > 0 ) ? KFEnum::Add : KFEnum::Dec;

        if ( name == __STRING__( hp ) )
        {
            return OperateHp( player, kfhero, operate, operatevalue );
        }

        if ( name == __STRING__( exp ) )
        {
            return AddExp( player, kfhero, operatevalue );
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

        player->UpdateData( kfdata, operate, operatevalue );
        return operatevalue;
    }

    uint32 KFHeroModule::AddExp( KFEntity* player, KFData* kfhero, uint32 exp )
    {
        auto hp = kfhero->Get( __STRING__( fighter ), __STRING__( hp ) );
        if ( hp == 0u )
        {
            // hp为0不能增加经验
            return 0u;
        }

        auto exprate = kfhero->Get<uint32>( __STRING__( exprate ) );
        if ( exprate == 0u )
        {
            // 经验倍率为0
            return 0u;
        }

        auto maxlevel = kfhero->Get( __STRING__( maxlevel ) );
        auto oldlevel = kfhero->Get( __STRING__( level ) );
        if ( oldlevel >= maxlevel )
        {
            // 英雄已满级
            return 0u;
        }

        auto kflevelsetting = KFLevelConfig::Instance()->FindSetting( oldlevel + 1u );
        if ( kflevelsetting == nullptr )
        {
            return 0u;
        }

        auto addexp = static_cast<double>( exprate ) / KFRandEnum::TenThousand * exp;
        exp = static_cast<uint32>( addexp + 0.5 );

        auto oldexp = kfhero->Get( __STRING__( exp ) );
        auto newexp = oldexp + exp;

        auto newlevel = oldlevel;
        auto totalexp = kflevelsetting->_exp;
        exp = 0u;
        while ( newexp >= totalexp )
        {
            newlevel++;
            newexp -= kflevelsetting->_exp;
            exp += kflevelsetting->_exp;

            kflevelsetting = KFLevelConfig::Instance()->FindSetting( newlevel + 1u );
            if ( kflevelsetting == nullptr )
            {
                newexp = 0u;
                break;
            }
            totalexp = kflevelsetting->_exp;

            if ( newlevel >= maxlevel )
            {
                newexp = 0u;
                break;
            }
        }

        player->UpdateData( kfhero, __STRING__( exp ), KFEnum::Set, newexp );
        if ( newlevel != oldlevel )
        {
            player->UpdateData( kfhero, __STRING__( level ), KFEnum::Set, newlevel );
            player->UpdateData( kfhero, __STRING__( totalexp ), KFEnum::Set, totalexp );
        }

        exp = exp + newexp - oldexp;
        return exp;
    }

    bool KFHeroModule::IsMaxLevel( KFEntity* player, KFData* kfhero )
    {
        auto maxlevel = kfhero->Get( __STRING__( maxlevel ) );
        auto level = kfhero->Get( __STRING__( level ) );
        return level >= maxlevel;
    }

    uint32 KFHeroModule::GetPlayerMaxLevel( KFEntity* player )
    {
        static auto _option = KFGlobal::Instance()->FindConstant( "initherolevellimit" );
        auto herolevel = player->Get<uint32>( __STRING__( effect ), __STRING__( herolevel ) );

        return _option->_uint32_value + herolevel;
    }

    uint32 KFHeroModule::CalcMaxLevel( KFEntity* player, KFData* kfhero )
    {
        auto level = kfhero->Get<uint32>( __STRING__( level ) );
        auto custom = kfhero->Get<uint32>( __STRING__( custom ) );
        if ( custom > 0u )
        {
            return level;
        }

        auto profession = kfhero->Get<uint32>( __STRING__( profession ) );
        auto kfsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfsetting == nullptr )
        {
            return level;
        }

        auto maxlevel = GetPlayerMaxLevel( player );
        maxlevel = __MIN__( maxlevel, kfsetting->_max_level );

        return maxlevel;
    }

    uint32 KFHeroModule::OperateHp( KFEntity* player, KFData* kfhero, uint32 operate, uint32 hp, bool death /*= false*/ )
    {
        auto kffighter = kfhero->Find( __STRING__( fighter ) );
        auto nowhp = kffighter->Get<uint32>( __STRING__( hp ) );
        if ( nowhp == 0u )
        {
            return nowhp;
        }

        auto maxhp = kffighter->Get<uint32>( __STRING__( maxhp ) );

        if ( operate == KFEnum::Add )
        {
            // 回血减少比例
            auto reducerate = _kf_realm->GetAddHpReduceRate( player );
            auto addrate = 1.0 - static_cast<double>( reducerate ) / static_cast<double>( KFRandEnum::TenThousand );
            hp = static_cast< uint32 >( std::ceil( addrate * hp ) );

            auto canaddhp = maxhp - nowhp;
            hp = __MIN__( hp, canaddhp );
        }
        else if ( operate == KFEnum::Set )
        {
            if ( !death )
            {
                hp = __MAX__( hp, 1u );
            }

            hp = __MIN__( hp, maxhp );
        }
        else if ( operate == KFEnum::Dec )
        {
            auto dechp = ( death ? nowhp : ( nowhp - 1 ) );
            hp = ( hp >= nowhp ? dechp : hp );

            // 扣血增加信仰
            auto dechpparam = _kf_realm->GetInnerWorldDecHpParam( player );
            auto param = static_cast<double>( dechpparam ) / static_cast<double>( KFRandEnum::TenThousand );
            auto addfaith = static_cast<uint32>( std::ceil( param * hp ) );
            if ( addfaith > 0u )
            {
                _kf_pve->OperateFaith( player, KFEnum::Add, addfaith );
            }
        }

        player->UpdateData( kffighter, __STRING__( hp ), operate, hp );

        nowhp = kffighter->Get<uint32>( __STRING__( hp ) );
        if ( nowhp == 0u )
        {
            OnHeroInjury( player, kfhero );
        }

        return nowhp;
    }

    void KFHeroModule::OnHeroInjury( KFEntity* player, KFData* kfhero )
    {
        player->UpdateData( kfhero, __STRING__( injurycount ), KFEnum::Add, 1u );

        auto injurycount = kfhero->Get<uint32>( __STRING__( injurycount ) );
        auto kfinjurysetting = KFInjuryRandomConfig::Instance()->FindInjureRandomSetting( injurycount );
        if ( kfinjurysetting == nullptr )
        {
            return;
        }

        auto randvalue = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
        if ( randvalue < kfinjurysetting->_dead_prob )
        {
            // 英雄死亡，添加死亡标记
            player->UpdateData( kfhero, __STRING__( dead ), KFEnum::Set, 1u );
        }
        else
        {
            // 随机伤病
            _kf_generate->RandInjuryData( player, kfhero, kfinjurysetting->_pool );
        }
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
}