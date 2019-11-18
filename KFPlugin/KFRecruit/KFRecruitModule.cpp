#include "KFRecruitModule.hpp"

namespace KFrame
{
    void KFRecruitModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_ELEMENT__( __STRING__( recruit ), &KFRecruitModule::AddRecruitHeroElement );
        __REGISTER_UPDATE_DATA_1__( __STRING__( recruitcount ), &KFRecruitModule::OnRecruitCountUpdateCallBack );
        __REGISTER_ADD_DATA_2__( __STRING__( build ), KFMsg::RecruitBuild, &KFRecruitModule::OnAddRecruitBuild );
        __REGISTER_REMOVE_DATA_1__( __STRING__( hero ), &KFRecruitModule::OnRemoveHero );

        __REGISTER_ENTER_PLAYER__( &KFRecruitModule::OnEnterRecruitModule );
        __REGISTER_LEAVE_PLAYER__( &KFRecruitModule::OnLeaveRecruitModule );
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

        __UN_ADD_ELEMENT__( __STRING__( recruit ) );
        __UN_UPDATE_DATA_1__( __STRING__( recruitcount ) );
        __UN_REMOVE_DATA_1__( __STRING__( hero ) );
        __UN_ADD_DATA_2__( __STRING__( build ), KFMsg::RecruitBuild );

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
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        auto objvalue = kfelementobject->CreateObjectValue( __STRING__( hero ) );
        auto kfgenerate = objvalue->_element->_values.Find( __STRING__( generateid ) );
        if ( kfgenerate == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not generateid!", kfelement->_data_name );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        auto generateid = kfgenerate->CalcUseValue( nullptr, 1.0f );

        // 创建招募英雄
        auto uuid = KFGlobal::Instance()->STMakeUUID( __STRING__( hero ) );
        auto kfrecruit = _kf_kernel->CreateObject( kfparent->_data_setting );

        auto kfhero = kfrecruit->Find( __STRING__( hero ) );
        auto* rethero = _kf_generate->GeneratePlayerHero( player, kfhero, generateid, true, function, line );
        if ( rethero == nullptr )
        {
            _kf_kernel->ReleaseObject( kfrecruit );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        // 是否需要花费
        auto kfusecost = kfelementobject->_values.Find( __STRING__( usecost ) );
        auto usecost = ( kfusecost == nullptr ) ? false : ( kfusecost->GetUseValue() == 1u );
        if ( usecost )
        {
            CalcRecruitCostData( player, kfrecruit );
        }

        player->AddData( kfparent, uuid, kfrecruit );
        return std::make_tuple( KFDataDefine::Show_Data, kfrecruit );
    }

    void KFRecruitModule::CalcRecruitCostData( KFEntity* player, KFData* kfrecruit )
    {
        auto kfhero = kfrecruit->Find( __STRING__( hero ) );
        auto professionid = kfhero->Get< uint32 >( __STRING__( profession ) );
        auto kfsetting = KFProfessionConfig::Instance()->FindSetting( professionid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 计算系数, 职阶系数 * 品质系数
        auto multiple = 1.0f;
        auto& strcost = kfsetting->_recruit_cost.CalcElement( multiple );
        kfrecruit->Set( __STRING__( cost ), strcost );
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
            kfrefreshtime->Operate( KFEnum::Set, 0u );
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

        RefreshRecruitHero( player, kfmsg.type() );
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
        RefreshRecruitHero( player, KFMsg::RefreshByFree );
    }

    void KFRecruitModule::RefreshRecruitHero( KFEntity* player, uint32 type )
    {
        auto kfsetting = KFRefreshConfig::Instance()->FindSetting( type );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroRecruitRefreshTypeError );
        }

        // 判断资源是否足够
        auto dataname = player->CheckRemoveElement( &kfsetting->_cost_elements, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        // 扣除资源
        player->RemoveElement( &kfsetting->_cost_elements, __FUNC_LINE__ );

        // 清空原来的英雄
        if ( kfsetting->_is_clear )
        {
            player->CleanData( __STRING__( recruit ) );
        }

        // 刷新招募列表
        for ( auto i = 0u; i < kfsetting->_count; ++i )
        {
            player->AddElement( &kfsetting->_hero_generate, __FUNC_LINE__ );
        }

        // 显示提示
        if ( kfsetting->_display_id != 0u )
        {
            _kf_display->SendToClient( player, kfsetting->_display_id );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRecruitModule::HandleChooseDivisorReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChooseDivisorReq );

        for ( auto i = 0; i < kfmsg.id_size(); ++i )
        {
            auto kfsetting = KFDivisorConfig::Instance()->FindSetting( kfmsg.id( i ) );
            if ( kfsetting == nullptr )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroDivisorError );
            }

            auto kfrecruitrecord = player->Find( __STRING__( divisor ) );
            auto kfrecruit = kfrecruitrecord->Find( kfsetting->_id );
            if ( kfrecruit != nullptr )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroDivisorExist );
            }

            // 同类型的不能都选择
            auto& outlist = DivisorListByType( player, kfsetting->_type );
            if ( ( uint32 )outlist.size() >= kfsetting->_max_count )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroDivisorMaxCount );
            }
            else
            {
                kfrecruit = _kf_kernel->CreateObject( kfrecruitrecord->_data_setting );
                player->AddData( kfrecruitrecord, kfsetting->_id, kfrecruit );
            }
        }
    }

    std::vector< const KFDivisorSetting* >& KFRecruitModule::DivisorListByType( KFEntity* player, uint32 type )
    {
        static std::vector< const KFDivisorSetting* > outlist;
        outlist.clear();

        auto kfdivisorrecord = player->Find( __STRING__( divisor ) );
        for ( auto kfdivisor = kfdivisorrecord->First(); kfdivisor != nullptr; kfdivisor = kfdivisorrecord->Next() )
        {
            auto id = kfdivisor->GetKeyID();
            auto kfsetting = KFDivisorConfig::Instance()->FindSetting( id );
            if ( kfsetting != nullptr && kfsetting->_type == type )
            {
                outlist.push_back( kfsetting );
            }
        }

        return outlist;
    }

    __KF_MESSAGE_FUNCTION__( KFRecruitModule::HandleRemoveDivisorReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveDivisorReq );

        for ( auto i = 0; i < kfmsg.id_size(); ++i )
        {
            player->RemoveData( __STRING__( divisor ), kfmsg.id( i ) );
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
        if ( kfherorecord->IsFull() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataIsFull, __STRING__( hero ) );
        }

        // 判断花费
        auto costdata = kfrecruit->Get< std::string >( __STRING__( cost ) );
        if ( !costdata.empty() )
        {
            KFElements costelements;
            if ( !costelements.Parse( costdata, __FUNC_LINE__ ) )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroRecruitCostError );
            }

            auto dataname = player->CheckRemoveElement( &costelements, __FUNC_LINE__ );
            if ( !dataname.empty() )
            {
                return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
            }

            player->RemoveElement( &costelements, __FUNC_LINE__ );
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
        player->AddDataToShow( __STRING__( recruit ), kfhero, false );

        // 发送通知
        //_kf_display->SendToClient( player, KFMsg::HeroRecruitOk, kfmsg.uuid() );
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
}