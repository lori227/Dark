#include "KFRecruitModule.hpp"

namespace KFrame
{
    void KFRecruitModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_ELEMENT__( __STRING__( recruit ), &KFRecruitModule::AddRecruitHeroElement );
        __REGISTER_REMOVE_DATA_1__( __STRING__( hero ), &KFRecruitModule::OnRemoveHero );
        __REGISTER_UPDATE_DATA_1__( __STRING__( recruitcount ), &KFRecruitModule::OnRecruitCountUpdateCallBack );
        __REGISTER_ADD_DATA_2__( __STRING__( build ), KFMsg::RecruitBuild, &KFRecruitModule::OnAddRecruitBuild );

        __REGISTER_ENTER_PLAYER__( &KFRecruitModule::OnEnterRecruitModule );
        __REGISTER_LEAVE_PLAYER__( &KFRecruitModule::OnLeaveRecruitModule );

        __REGISTER_AFTER_ENTER_PLAYER__( &KFRecruitModule::OnAfterEnterRecruitModule );

        __REGISTER_EXECUTE__( __STRING__( recruitdivisor ), &KFRecruitModule::OnExecuteTechnologyRecruitDivisor );
        __REGISTER_EXECUTE__( __STRING__( recruitcount ), &KFRecruitModule::OnExecuteTechnologyRecruitCount );
        __REGISTER_EXECUTE__( __STRING__( recruitweight ), &KFRecruitModule::OnExecuteTechnologyRecruitWeight );
        __REGISTER_EXECUTE__( __STRING__( recruitdiscount ), &KFRecruitModule::OnExecuteTechnologyRecruitDiscount );
        __REGISTER_EXECUTE__( __STRING__( recruitprofession ), &KFRecruitModule::OnExecuteTechnologyRecruitProfession );
        __REGISTER_EXECUTE__( __STRING__( recruitlevel ), &KFRecruitModule::OnExecuteTechnologyRecruitLevel );
        __REGISTER_EXECUTE__( __STRING__( recruitgrowth ), &KFRecruitModule::OnExecuteTechnologyRecruitGrowth );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_REFRESH_RECRUIT_REQ, &KFRecruitModule::HandleRefreshRecruitReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHOOSE_DIVISOR_REQ, &KFRecruitModule::HandleChooseDivisorReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DIVISOR_REQ, &KFRecruitModule::HandleRemoveDivisorReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_RECRUIT_HERO_REQ, &KFRecruitModule::HandleRecruitHeroReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_RECRUIT_HERO_NAME_REQ, &KFRecruitModule::HandleSetRecruitHeroNameReq );
    }

    void KFRecruitModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        __UN_AFTER_ENTER_PLAYER__();

        __UN_ADD_ELEMENT__( __STRING__( recruit ) );
        __UN_REMOVE_DATA_1__( __STRING__( hero ) );
        __UN_UPDATE_DATA_1__( __STRING__( recruitcount ) );
        __UN_ADD_DATA_2__( __STRING__( build ), KFMsg::RecruitBuild );

        __UN_EXECUTE__( __STRING__( recruitdivisor ) );
        __UN_EXECUTE__( __STRING__( recruitcount ) );
        __UN_EXECUTE__( __STRING__( recruitweight ) );
        __UN_EXECUTE__( __STRING__( recruitdiscount ) );
        __UN_EXECUTE__( __STRING__( recruitprofession ) );
        __UN_EXECUTE__( __STRING__( recruitlevel ) );
        __UN_EXECUTE__( __STRING__( recruitgrowth ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_REFRESH_RECRUIT_REQ );
        __UN_MESSAGE__( KFMsg::MSG_CHOOSE_DIVISOR_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REMOVE_DIVISOR_REQ );
        __UN_MESSAGE__( KFMsg::MSG_RECRUIT_HERO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SET_RECRUIT_HERO_NAME_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFRecruitModule::AddRecruitHeroElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        auto objvalue = kfelementobject->CreateObjectValue( __STRING__( hero ) );
        auto kfgenerate = objvalue->_element->_values.Find( __STRING__( id ) );
        if ( kfgenerate == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not generateid", kfelement->_data_name );
            return false;
        }

        auto kfrecruit = player->CreateData( kfparent );
        auto kfhero = kfrecruit->Find( __STRING__( hero ) );
        auto generateid = kfgenerate->CalcUseValue( nullptr, 1.0f );
        auto rethero = _kf_generate->GeneratePlayerHero( player, kfhero, generateid );
        if ( rethero == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "recruit generate hero=[{}] failed", generateid );
            return false;
        }

        // 添加到招募列表
        auto uuid = KFGlobal::Instance()->STMakeUUID( __STRING__( hero ) );
        player->AddData( kfparent, uuid, kfrecruit );
        return true;
    }

    __KF_ADD_DATA_FUNCTION__( KFRecruitModule::OnAddRecruitBuild )
    {
        // 招募所功能开启
        CheckRefreshRecruitTimer( player );
        RefreshRecruitFreeHero( player, 0u );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFRecruitModule::OnRemoveHero )
    {
        RefreshRecruitFreeHero( player, 1u );
    }

    bool KFRecruitModule::IsRecruitActive( KFEntity* player )
    {
        auto kfbuild = player->Find( __STRING__( build ), KFMsg::RecruitBuild );
        return kfbuild != nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ENTER_PLAYER_FUNCTION__( KFRecruitModule::OnEnterRecruitModule )
    {
        // 检查招募所是否开启
        if ( !IsRecruitActive( player ) )
        {
            return;
        }

        CheckRefreshRecruitTimer( player );
        RefreshRecruitFreeHero( player, 0u );
    }

    __KF_AFTER_ENTER_PLAYER_FUNCTION__( KFRecruitModule::OnAfterEnterRecruitModule )
    {
        auto kfeffect = player->Find( __STRING__( effect ) );
        auto newversion = kfeffect->Get<uint32>( __STRING__( newversion ) );
        if ( newversion == 1u )
        {
            RefreshRecruitHeroDiscount( player, kfeffect, 0u );
        }
    }

    void KFRecruitModule::CheckRefreshRecruitTimer( KFEntity* player )
    {
        // 刷新招募次数
        auto kfrefreshtime = player->Find( __STRING__( recruittime ) );
        auto kfrecruitcount = player->Find( __STRING__( recruitcount ) );

        auto intervaltime = kfrefreshtime->_data_setting->_int_logic_value;
        auto refreshtime = kfrefreshtime->Get<uint64>();
        if ( refreshtime > 0u )
        {
            // 只有设置了时间才计算招募次数
            auto nowtime = KFGlobal::Instance()->_real_time;
            if ( nowtime >= refreshtime )
            {
                // 计算时间
                auto passtime = nowtime - refreshtime;
                intervaltime = kfrefreshtime->_data_setting->_int_logic_value - ( passtime % kfrefreshtime->_data_setting->_int_logic_value );

                auto addcount = ( passtime / kfrefreshtime->_data_setting->_int_logic_value ) + 1;
                kfrecruitcount->Operate( KFEnum::Add, addcount );
            }
            else
            {
                intervaltime = refreshtime - nowtime;
            }
        }

        if ( kfrecruitcount->IsFull() )
        {
            // 重置时间
            kfrefreshtime->Set( 0u );
        }
        else
        {
            StartRefreshRecruitTimer( player, kfrefreshtime, intervaltime );
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFRecruitModule::OnLeaveRecruitModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    void KFRecruitModule::StartRefreshRecruitTimer( KFEntity* player, KFData* kfrefreshtime, uint32 intervaltime )
    {
        player->UpdateData( kfrefreshtime, KFEnum::Set, KFGlobal::Instance()->_real_time + intervaltime );
        __LOOP_TIMER_1__( player->GetKeyID(), kfrefreshtime->_data_setting->_int_logic_value * 1000, intervaltime * 1000, &KFRecruitModule::OnTimerRefreshRecruitCount );
    }

    __KF_TIMER_FUNCTION__( KFRecruitModule::OnTimerRefreshRecruitCount )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto kfrefreshtime = player->Find( __STRING__( recruittime ) );
        auto kfrecruitcount = player->Find( __STRING__( recruitcount ) );

        // 设置时间
        player->UpdateData( kfrefreshtime, KFEnum::Set, KFGlobal::Instance()->_real_time + kfrefreshtime->_data_setting->_int_logic_value );

        // 在更新数量, 数量满了会重置时间为0
        player->UpdateData( kfrecruitcount, KFEnum::Add, 1 );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRecruitModule::OnRecruitCountUpdateCallBack )
    {
        if ( kfdata->IsFull() )
        {
            // 删除定时器
            __UN_TIMER_1__( player->GetKeyID() );

            // 设置时间为0, 不需要倒计时
            player->UpdateData( __STRING__( recruittime ), KFEnum::Set, 0u );
        }
        else
        {
            auto kfrefreshtime = player->Find( __STRING__( recruittime ) );
            if ( kfrefreshtime->Get() == 0u )
            {
                StartRefreshRecruitTimer( player, kfrefreshtime, kfrefreshtime->_data_setting->_int_logic_value );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRecruitModule::HandleRefreshRecruitReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRefreshRecruitReq );

        if ( !IsRecruitActive( player ) )
        {
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        if ( kfmsg.type() == KFMsg::RefreshByFree )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroRecruitRefreshFreeError );
        }
        else if ( kfmsg.type() == KFMsg::RefreshByCount )
        {
            auto refreshcount = player->Get<uint32>( __STRING__( recruitcount ) );
            if ( refreshcount == 0u )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroRefreshCountNotEnough );
            }
        }

        auto result = RefreshRecruitLists( player, kfmsg.type() );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }

        if ( kfmsg.type() != KFMsg::RefreshByFree )
        {
            _kf_display->SendToClient( player, KFMsg::HeroRecruitRefresh );
            if ( kfmsg.type() == KFMsg::RefreshByCount )
            {
                player->UpdateData( __STRING__( recruitcount ), KFEnum::Dec, 1u );
            }
        }

        _kf_display->SendToClient( player, KFMsg::HeroRecruitRefreshSuc );
    }

    void KFRecruitModule::RefreshRecruitFreeHero( KFEntity* player, uint32 herocount )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );
        if ( kfherorecord->Size() > herocount )
        {
            return;
        }

        // 判断招募列表里面不能有免费的
        auto kfrecruitrecord = player->Find( __STRING__( recruit ) );
        for ( auto kfrecruit = kfrecruitrecord->First(); kfrecruit != nullptr; kfrecruit = kfrecruitrecord->Next() )
        {
            auto cost = kfrecruit->Get<std::string>( __STRING__( cost ) );
            if ( cost.empty() )
            {
                return;
            }
        }

        // 免费刷新一批英雄
        RefreshRecruitLists( player, KFMsg::RefreshByFree );
    }

    uint32 KFRecruitModule::RefreshRecruitLists( KFEntity* player, uint32 type )
    {
        auto kfrecruitsetting = KFRecruitConfig::Instance()->FindSetting( type );
        if ( kfrecruitsetting == nullptr )
        {
            return KFMsg::HeroRecruitRefreshTypeError;
        }

        // 计算招募的数量
        auto kfeffect = player->Find( __STRING__( effect ) );
        auto recruitcount = CalcRecruitHeroCount( kfeffect, kfrecruitsetting );
        if ( recruitcount == 0u )
        {
            return KFMsg::HeroRecruitRefreshCount;
        }

        // 计算招募英雄池和权重
        UInt32Map generateweight;
        auto totalweight = CalcRecruitGenerateWeight( kfeffect, kfrecruitsetting, generateweight, type );
        if ( totalweight == 0u )
        {
            return KFMsg::HeroRecruitRefreshWeight;
        }
        if ( generateweight.empty() )
        {
            return KFMsg::HeroRecruitRefreshGenerate;
        }

        // 扣除资源
        auto& dataname = player->RemoveElement( &kfrecruitsetting->_cost_elements, _default_multiple, __STRING__( recruitrefresh ), 0u, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return KFMsg::DataNotEnough;
        }

        // 清空原来的英雄
        auto kfrecruitrecord = player->Find( __STRING__( recruit ) );
        if ( kfrecruitsetting->_is_clear )
        {
            player->CleanData( kfrecruitrecord );
        }

        // 招募因子列表
        auto& divisorlist = CalcRecruitHeroDivisor( kfeffect );

        // 解锁的职业列表
        auto& professionlist = CalcRecruitHeroProfession( kfeffect );

        // 成长率
        auto mingrowth = kfeffect->Get<uint32>( __STRING__( mingrowth ) );
        auto maxgrowth = kfeffect->Get<uint32>( __STRING__( maxgrowth ) );

        // 刷新招募列表
        for ( auto i = 0u; i < recruitcount; ++i )
        {
            auto generateid = KFUtility::RandMapValue( generateweight, totalweight, KFGlobal::Instance()->Rand() );
            if ( generateid != 0u )
            {
                GenerateRecruitHero( player, kfeffect, kfrecruitrecord, generateid, divisorlist, professionlist, mingrowth, maxgrowth );
            }
        }

        return KFMsg::Ok;
    }

    uint32 KFRecruitModule::CalcRecruitHeroCount( KFData* kfeffect, const KFRecruitSetting* kfrecruitsetting )
    {
        // 科技效果添加数量
        return kfeffect->Get<uint32>( __STRING__( recruitcount ) );
    }

    uint32 KFRecruitModule::CalcRecruitGenerateWeight( KFData* kfeffect, const KFRecruitSetting* kfrecruitsetting, UInt32Map& generateweight, uint32 type )
    {
        // 配置表中的权重
        UInt32Map _genterate_weight;
        {
            auto kftechnologysetting = KFTechnologyConfig::Instance()->FindSetting( kfrecruitsetting->_generate_technology_id );
            if ( kftechnologysetting == nullptr )
            {
                return 0u;
            }

            for ( auto kfparam : kftechnologysetting->_execute_data._param_list._params )
            {
                if ( kfparam->_map_value.empty() )
                {
                    break;
                }

                for ( auto& iter : kfparam->_map_value )
                {
                    _genterate_weight[ iter.first ] = iter.second;
                }
            }
        }

        // 计算科技影响招募
        auto totalweight = 0u;
        generateweight.clear();

        auto kfweightrecord = kfeffect->Find( __STRING__( recruitweight ) );
        for ( auto& iter : _genterate_weight )
        {
            auto weight = iter.second;
            if ( type != KFMsg::RefreshByFree )
            {
                // 科技不影响免费刷新
                auto effectvalue = kfweightrecord->Get<uint32>( iter.first, kfweightrecord->_data_setting->_value_key_name );
                weight += effectvalue;
            }

            totalweight += weight;
            generateweight[ iter.first ] = weight;
        }

        return totalweight;
    }

    uint32 KFRecruitModule::CalcRecruitHeroLevel( KFData* kfeffect )
    {
        auto minlevel = kfeffect->Get<uint32>( __STRING__( recruitminlevel ) );
        auto maxlevel = kfeffect->Get<uint32>( __STRING__( recruitmaxlevel ) );
        return KFGlobal::Instance()->RandRange( minlevel, maxlevel, 1u );
    }

    UInt32Set& KFRecruitModule::CalcRecruitHeroProfession( KFData* kfeffect )
    {
        static UInt32Set _profession_list;
        _profession_list.clear();

        auto kfrecord = kfeffect->Find( __STRING__( recruitprofession ) );
        for ( auto kfchild = kfrecord->First(); kfchild != nullptr; kfchild = kfrecord->Next() )
        {
            _profession_list.insert( kfchild->GetKeyID() );
        }

        return _profession_list;
    }

    DivisorList& KFRecruitModule::CalcRecruitHeroDivisor( KFData* kfeffect )
    {
        static DivisorList _out_list;
        _out_list.clear();

        auto kfrecord = kfeffect->Find( __STRING__( recruitdivisor ) );
        for ( auto kfdivisor = kfrecord->First(); kfdivisor != nullptr; kfdivisor = kfrecord->Next() )
        {
            auto value = kfdivisor->Get<uint32>( kfrecord->_data_setting->_value_key_name );
            if ( value == 0u )
            {
                continue;
            }

            auto kfsetting = KFDivisorConfig::Instance()->FindSetting( kfdivisor->GetKeyID() );
            if ( kfsetting != nullptr )
            {
                _out_list.push_back( kfsetting );
            }
        }

        return _out_list;
    }

    DivisorList& KFRecruitModule::DivisorListByType( KFData* kfdivisorrecord, uint32 type )
    {
        static DivisorList _out_list;
        _out_list.clear();

        for ( auto kfdivisor = kfdivisorrecord->First(); kfdivisor != nullptr; kfdivisor = kfdivisorrecord->Next() )
        {
            auto value = kfdivisor->Get<uint32>( kfdivisorrecord->_data_setting->_value_key_name );
            if ( value == 0u )
            {
                continue;
            }

            auto kfsetting = KFDivisorConfig::Instance()->FindSetting( kfdivisor->GetKeyID() );
            if ( kfsetting != nullptr && kfsetting->_type == type )
            {
                _out_list.push_back( kfsetting );
            }
        }

        return _out_list;
    }

    void KFRecruitModule::GenerateRecruitHero( KFEntity* player, KFData* kfeffect, KFData* kfrecruitrecord, uint32 generateid,
            const DivisorList& divisorlist, const UInt32Set& professionlist, uint32 mingrowth, uint32 maxgrowth )
    {
        // 计算招募等级
        uint32 level = CalcRecruitHeroLevel( kfeffect );

        auto kfrecruit = player->CreateData( kfrecruitrecord );
        auto kfhero = kfrecruit->Find( __STRING__( hero ) );
        auto rethero = _kf_generate->GeneratePlayerHero( player, kfhero, generateid, divisorlist, professionlist, level, mingrowth, maxgrowth );
        if ( rethero == nullptr )
        {
            return;
        }

        // 招募价钱
        CalcRecruitCostData( player, kfeffect, kfrecruit, kfhero, generateid, false );

        // 添加到招募列表
        auto uuid = KFGlobal::Instance()->STMakeUUID( __STRING__( hero ) );
        player->AddData( kfrecruitrecord, uuid, kfrecruit );
    }

    void KFRecruitModule::CalcRecruitCostData( KFEntity* player, KFData* kfeffect, KFData* kfrecruit, KFData* kfhero, uint32 generateid, bool update )
    {
        auto kfgeneratesetting = KFGenerateConfig::Instance()->FindSetting( generateid );
        if ( kfgeneratesetting == nullptr || kfgeneratesetting->_cost_formula_id == 0u )
        {
            return;
        }

        auto kfformulasetting = KFFormulaConfig::Instance()->FindSetting( kfgeneratesetting->_cost_formula_id );
        if ( kfformulasetting == nullptr || kfformulasetting->_type.empty() || kfformulasetting->_params.size() < 5u )
        {
            return;
        }

        // 计算平均成长率
        auto totalgrowth = 0u;
        auto kfgrowth = kfhero->Find( __STRING__( growth ) );
        for ( auto kfchild = kfgrowth->First(); kfchild != nullptr; kfchild = kfgrowth->Next() )
        {
            totalgrowth += kfchild->Get<uint32>();
        }
        auto averagegrowth = static_cast<double>( totalgrowth ) / kfgrowth->Size();

        auto price = 0.0;
        auto param1 = kfformulasetting->_params[ 0 ]->_double_value;
        auto param2 = kfformulasetting->_params[ 1 ]->_double_value;
        auto param3 = kfformulasetting->_params[ 2 ]->_double_value;
        auto param4 = kfformulasetting->_params[ 3 ]->_double_value;
        auto param5 = kfformulasetting->_params[ 4 ]->_double_value;
        if ( param5 != 0u )
        {
            if ( averagegrowth >= param2 )
            {
                price = param1 + ( averagegrowth - param2 ) * param3 / param5;
            }
            else
            {
                auto value = ( param2 - averagegrowth ) * param3 / param5;
                if ( value >= param1 )
                {
                    price = value - param1;
                }
                else
                {
                    price = param1 - value;
                }
            }
        }

        price = __MAX__( price, param4 );

        // 种族亲和, 费用减低
        auto finalprice = CalcRecruitHeroDiscount( kfeffect, kfhero, price );

        // 格式化费用数据
        auto& strcost = KFElementConfig::Instance()->StringElemnt( kfformulasetting->_type, finalprice, 0u );
        if ( !update )
        {
            kfrecruit->Set( __STRING__( cost ), strcost );
        }
        else
        {
            player->UpdateData( kfrecruit, __STRING__( cost ), strcost );
        }
    }

    uint32 KFRecruitModule::CalcRecruitHeroDiscount( KFData* kfeffect, KFData* kfhero, uint32 price )
    {
        auto race = kfhero->Get<uint32>( __STRING__( race ) );
        auto discount = kfeffect->Get<uint32>( __STRING__( recruitdiscount ), race, __STRING__( value ) );
        if ( discount == 0u )
        {
            return price;
        }

        double ratio = 1.0f - static_cast< double >( discount ) / static_cast< double >( KFRandEnum::TenThousand );
        auto finalprice = static_cast< double >( price ) * ratio + 0.5;
        return static_cast< uint32 >( finalprice );
    }

    void KFRecruitModule::RefreshRecruitHeroDiscount( KFEntity* player, KFData* kfeffect, uint32 race )
    {
        auto kfrecruitrecord = player->Find( __STRING__( recruit ) );
        for ( auto kfrecruit = kfrecruitrecord->First(); kfrecruit != nullptr; kfrecruit = kfrecruitrecord->Next() )
        {
            auto kfhero = kfrecruit->Find( __STRING__( hero ) );
            auto herorace = kfhero->Get<uint32>( __STRING__( race ) );
            if ( race == 0u || herorace == race )
            {
                auto generateid = kfhero->Get<uint32>( __STRING__( id ) );
                CalcRecruitCostData( player, kfeffect, kfrecruit, kfhero, generateid, player->IsInited() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRecruitModule::HandleRecruitHeroReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRecruitHeroReq );

        if ( !IsRecruitActive( player ) )
        {
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        auto kfrecruitrecord = player->Find( __STRING__( recruit ) );
        auto kfrecruit = kfrecruitrecord->Find( kfmsg.uuid() );
        if ( kfrecruit == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroRecruitNotExist );
        }

        // 判断英雄是否满了
        auto kfherorecord = player->Find( __STRING__( hero ) );
        if ( _kf_hero->IsFull( player, kfherorecord ) )
        {
            return _kf_display->SendToClient( player, KFMsg::DataIsFull, __STRING__( hero ) );
        }

        // 判断花费
        auto strcost = kfrecruit->Get< std::string >( __STRING__( cost ) );
        if ( !strcost.empty() )
        {
            KFElements costelements;
            if ( !costelements.Parse( strcost, __FUNC_LINE__ ) )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroRecruitCostError );
            }

            auto id = kfrecruit->Get<uint32>( __STRING__( hero ), __STRING__( id ) );
            auto& dataname = player->RemoveElement( &costelements, _default_multiple, __STRING__( recruithero ), id, __FUNC_LINE__ );
            if ( !dataname.empty() )
            {
                return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
            }
        }

        // 招募英雄
        auto kfhero = kfrecruit->Move( __STRING__( hero ), false );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroRecruitDataError );
        }

        // 删除招募英雄
        player->RemoveData( kfrecruitrecord, kfmsg.uuid() );

        // 添加英雄
        player->AddData( kfherorecord, kfmsg.uuid(), kfhero );
        player->AddDataToShow( __STRING__( recruit ), kfhero );

        _kf_display->SendToClient( player, KFMsg::HeroRecruitHeroSuc );
    }

    __KF_MESSAGE_FUNCTION__( KFRecruitModule::HandleSetRecruitHeroNameReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetRecruitHeroNameReq );

        auto kfrecruit = player->Find( __STRING__( recruit ), kfmsg.uuid() );
        if ( kfrecruit == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroRecruitNotExist );
        }

        auto kfhero = kfrecruit->Find( __STRING__( hero ) );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroRecruitDataError );
        }

        auto kfname = kfhero->Find( __STRING__( name ) );
        if ( kfmsg.name().size() > kfname->_data_setting->_int_max_value )
        {
            return _kf_display->SendToClient( player, KFMsg::NameLengthError );
        }

        auto ok = _kf_filter->CheckFilter( kfmsg.name() );
        if ( ok )
        {
            return _kf_display->SendToClient( player, KFMsg::NameFilterError );
        }

        player->UpdateData( kfhero, __STRING__( name ), kfmsg.name() );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRecruitModule::HandleChooseDivisorReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChooseDivisorReq );

        auto kfrecord = player->Find( __STRING__( effect ), __STRING__( recruitdivisor ) );
        for ( auto i = 0; i < kfmsg.id_size(); ++i )
        {
            auto kfsetting = KFDivisorConfig::Instance()->FindSetting( kfmsg.id( i ) );
            if ( kfsetting == nullptr )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroDivisorError );
            }

            auto kfdivisor = kfrecord->Find( kfsetting->_id );
            if ( kfdivisor == nullptr )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroDivisorNotUnlock );
            }

            auto value = kfdivisor->Get<uint32>( kfrecord->_data_setting->_value_key_name );
            if ( value != 0u )
            {
                continue;
            }

            // 同类型的不能都选择
            auto& outlist = DivisorListByType( kfrecord, kfsetting->_type );
            if ( ( uint32 )outlist.size() >= kfsetting->_max_count )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroDivisorMaxCount );
            }

            player->UpdateData( kfdivisor, kfrecord->_data_setting->_value_key_name, KFEnum::Set, 1u );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRecruitModule::HandleRemoveDivisorReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveDivisorReq );

        auto kfrecord = player->Find( __STRING__( effect ), __STRING__( recruitdivisor ) );
        for ( auto i = 0; i < kfmsg.id_size(); ++i )
        {
            auto divisorid = kfmsg.id( i );
            auto kfdivisor = kfrecord->Find( divisorid );
            if ( kfdivisor == nullptr )
            {
                continue;
            }

            player->UpdateData( kfdivisor, kfrecord->_data_setting->_value_key_name, KFEnum::Set, 0u );
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_EXECUTE_FUNCTION__( KFRecruitModule::OnExecuteTechnologyRecruitDivisor )
    {
        auto kfrecord = player->Find( __STRING__( effect ), __STRING__( recruitdivisor ) );
        for ( auto kfparam : executedata->_param_list._params )
        {
            auto divisorid = kfparam->_int_value;
            if ( divisorid == 0u )
            {
                break;
            }

            auto kfdivisor = kfrecord->Find( divisorid );
            if ( kfdivisor != nullptr )
            {
                continue;
            }

            // 添加招募因子列表
            player->UpdateData( kfrecord, divisorid, kfrecord->_data_setting->_value_key_name, KFEnum::Set, 0u );
        }

        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFRecruitModule::OnExecuteTechnologyRecruitCount )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto addcount = executedata->_param_list._params[ 0 ]->_int_value;
        player->UpdateData( __STRING__( effect ), __STRING__( recruitcount ), KFEnum::Add, addcount );
        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFRecruitModule::OnExecuteTechnologyRecruitWeight )
    {
        auto kfrecord = player->Find( __STRING__( effect ), __STRING__( recruitweight ) );
        for ( auto kfparam : executedata->_param_list._params )
        {
            if ( kfparam->_map_value.empty() )
            {
                break;
            }

            for ( auto& iter : kfparam->_map_value )
            {
                if ( iter.second != 0u )
                {
                    player->UpdateData( kfrecord, iter.first, kfrecord->_data_setting->_value_key_name, KFEnum::Add, iter.second );
                }
            }
        }

        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFRecruitModule::OnExecuteTechnologyRecruitDiscount )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            return false;
        }

        auto race = executedata->_param_list._params[ 0 ]->_int_value;
        auto discount = executedata->_param_list._params[ 1 ]->_int_value;

        auto kfeffect = player->Find( __STRING__( effect ) );
        auto kfrecord = kfeffect->Find( __STRING__( recruitdiscount ) );
        player->UpdateData( kfrecord, race, kfrecord->_data_setting->_value_key_name, KFEnum::Add, discount );

        if ( player->IsInited() )
        {
            // 刷新招募价格
            RefreshRecruitHeroDiscount( player, kfeffect, race );
        }
        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFRecruitModule::OnExecuteTechnologyRecruitProfession )
    {
        auto kfrecord = player->Find( __STRING__( effect ), __STRING__( recruitprofession ) );
        for ( auto kfparam : executedata->_param_list._params )
        {
            if ( kfparam->_int_value == 0u )
            {
                break;
            }

            player->UpdateData( kfrecord, kfparam->_int_value, kfrecord->_data_setting->_value_key_name, KFEnum::Set, 0u );
        }

        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFRecruitModule::OnExecuteTechnologyRecruitLevel )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            return false;
        }

        auto minlevel = executedata->_param_list._params[ 0 ]->_int_value;
        auto maxlevel = executedata->_param_list._params[ 1 ]->_int_value;

        auto kfeffect = player->Find( __STRING__( effect ) );
        player->UpdateData( kfeffect, __STRING__( recruitminlevel ), KFEnum::Add, minlevel );
        player->UpdateData( kfeffect, __STRING__( recruitmaxlevel ), KFEnum::Add, maxlevel );

        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFRecruitModule::OnExecuteTechnologyRecruitGrowth )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            return false;
        }

        auto mingrowth = executedata->_param_list._params[ 0 ]->_int_value;
        auto maxgrowth = executedata->_param_list._params[ 1 ]->_int_value;

        auto kfeffect = player->Find( __STRING__( effect ) );
        player->UpdateData( kfeffect, __STRING__( mingrowth ), KFEnum::Add, mingrowth );
        player->UpdateData( kfeffect, __STRING__( maxgrowth ), KFEnum::Add, maxgrowth );

        return true;
    }
}