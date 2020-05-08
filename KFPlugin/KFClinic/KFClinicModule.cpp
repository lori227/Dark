#include "KFClinicModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __KF_SET_STATIC_OPTION_CLINIC__( optionname, optionvalue, classvar)  \
    {   \
        static auto _static_var = KFGlobal::Instance()->FindConstant( optionname ); \
        if ( _static_var->optionvalue != classvar )  \
        {   \
            classvar = _static_var->optionvalue;   \
        }   \
    }   \

    void KFClinicSetting::UpdateStaticPart()
    {
        __KF_SET_STATIC_OPTION_CLINIC__( "cliniccdtime", _uint32_value, _hp_item_cd_time );
        __KF_SET_STATIC_OPTION_CLINIC__( "clinichpvalue", _uint32_value, _hp_item_cure );
        __KF_SET_STATIC_OPTION_CLINIC__( "cliniccurecount", _uint32_value, _cure_count );
        __KF_SET_STATIC_OPTION_CLINIC__( "clinicconsumefid", _uint32_value, _consume_money_fid );
        __KF_SET_STATIC_OPTION_CLINIC__( "clinicconsumenum", _uint32_value, _consume_hp_item_count );
    }

    void KFClinicSetting::UpdateDynamicPart( KFEntity* player )
    {
        _hp_item_add_count = player->Get<uint32>( __STRING__( effect ), __STRING__( clinicaddnum ) );
        _hp_item_max_count = player->Get<uint32>( __STRING__( effect ), __STRING__( clinicmaxnum ) );

        auto submoneypercent = player->Get<uint64>( __STRING__( effect ), __STRING__( clinicsubmoneypercent ) ) / ( double )KFRandEnum::TenThousand;
        _scale_consume_money = 1.0 - submoneypercent;
    }

    void KFClinicModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFClinicModule::OnEnterClinicModule );
        __REGISTER_LEAVE_PLAYER__( &KFClinicModule::OnLeaveClinicModule );
        __REGISTER_RECORD_VALUE__( &KFClinicModule::GetDayClinicTotalNum );

        __REGISTER_ADD_ELEMENT__( __STRING__( clinic ), &KFClinicModule::AddClinicElement );
        __REGISTER_ADD_DATA_2__( __STRING__( build ), KFMsg::ClinicBuild, &KFClinicModule::OnAddClinicBuild );
        __REGISTER_UPDATE_DATA_2__( __STRING__( clinic ), __STRING__( num ), &KFClinicModule::OnItemNumUpdate );
        __REGISTER_UPDATE_DATA_2__( __STRING__( effect ), __STRING__( clinicmaxnum ), &KFClinicModule::OnItemNumUpdate );

        __REGISTER_EXECUTE__( __STRING__( clinicaddnum ), &KFClinicModule::OnExecuteClinicAddData );
        __REGISTER_EXECUTE__( __STRING__( clinicmaxnum ), &KFClinicModule::OnExecuteClinicAddData );
        __REGISTER_EXECUTE__( __STRING__( clinicsubmoneypercent ), &KFClinicModule::OnExecuteClinicAddData );

        ///////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_CLINIC_CURE_REQ, &KFClinicModule::HandleClinicCureReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CLINIC_MEDICAL_FEE_REQ, &KFClinicModule::HandleClinicMedicalFeeReq );
    }

    void KFClinicModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        __UN_RECORD_VALUE__();

        __UN_ADD_ELEMENT__( __STRING__( clinic ) );
        __UN_ADD_DATA_2__( __STRING__( build ), KFMsg::ClinicBuild );
        __UN_UPDATE_DATA_2__( __STRING__( clinic ), __STRING__( num ) );
        __UN_UPDATE_DATA_2__( __STRING__( effect ), __STRING__( clinicmaxnum ) );

        __UN_EXECUTE__( __STRING__( clinicaddnum ) );
        __UN_EXECUTE__( __STRING__( clinicmaxnum ) );
        __UN_EXECUTE__( __STRING__( clinicsubmoneypercent ) );
        ///////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_CLINIC_CURE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_CLINIC_MEDICAL_FEE_REQ );
    }
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFClinicModule::AddClinicElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto maxnum = player->Get( __STRING__( effect ), __STRING__( clinicmaxnum ) );
        if ( maxnum == 0u )
        {
            // 储存数量为0时不可获得材料
            return false;
        }

        auto kfclinic = player->Find( __STRING__( clinic ) );
        auto curnum = kfclinic->Get<uint32>( __STRING__( num ) );

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        uint32 totalnum = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( num ), kfresult->_multiple );
        totalnum = __MIN__( totalnum, maxnum - curnum );

        player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Add, totalnum );

        return kfresult->AddResult( __STRING__( num ), totalnum );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFClinicModule::OnEnterClinicModule )
    {
        // 检查定时器
        CheckClinicTimer( player );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFClinicModule::OnLeaveClinicModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    __KF_EXECUTE_FUNCTION__( KFClinicModule::OnExecuteClinicAddData )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto adddata = executedata->_param_list._params[0]->_int_value;
        player->UpdateData( __STRING__( effect ), executedata->_name, KFEnum::Add, adddata );
        return true;
    }

    uint32 KFClinicModule::GetClinicLevel( KFEntity* player )
    {
        auto kfbuild = player->Find( __STRING__( build ), KFMsg::ClinicBuild );
        if ( kfbuild == nullptr )
        {
            return 0u;
        }

        return kfbuild->Get( __STRING__( level ) );
    }

    bool KFClinicModule::IsClinicActive( KFEntity* player )
    {
        auto kfbuild = player->Find( __STRING__( build ), KFMsg::ClinicBuild );
        return kfbuild != nullptr;
    }

    const KFClinicSetting* KFClinicModule::GetClinicSetting( KFEntity* player )
    {
        static KFClinicSetting _kf_clinic_setting;
        _kf_clinic_setting.UpdateStaticPart();
        _kf_clinic_setting.UpdateDynamicPart( player );

        return &_kf_clinic_setting;
    }

    void KFClinicModule::CheckClinicTimer( KFEntity* player )
    {
        // 检查医疗所是否开启
        if ( !IsClinicActive( player ) )
        {
            return;
        }

        auto setting = GetClinicSetting( player );
        if ( setting == nullptr || setting->_hp_item_cd_time == 0u )
        {
            return;
        }

        auto kfclinic = player->Find( __STRING__( clinic ) );
        auto num = kfclinic->Get<uint32>( __STRING__( num ) );
        if ( num >= setting->_hp_item_max_count )
        {
            return;
        }

        auto nowtime = KFGlobal::Instance()->_real_time;
        auto calctime = kfclinic->Get<uint64>( __STRING__( calctime ) );
        auto delaytime = 0u;

        if ( calctime == 0u )
        {
            player->UpdateData( kfclinic, __STRING__( calctime ), KFEnum::Set, nowtime );
        }
        else
        {
            auto cdtime = nowtime - calctime;
            if ( cdtime >= setting->_hp_item_cd_time )
            {
                auto count = cdtime / setting->_hp_item_cd_time;
                auto addnum = count * setting->_hp_item_add_count;
                auto maxnum = setting->_hp_item_max_count;

                if ( num + addnum >= maxnum )
                {
                    player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Set, maxnum );
                    return;
                }

                calctime += count * setting->_hp_item_cd_time;

                player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Add, addnum );
                player->UpdateData( kfclinic, __STRING__( calctime ), KFEnum::Set, calctime );
            }

            delaytime = ( setting->_hp_item_cd_time - ( nowtime - calctime ) % setting->_hp_item_cd_time ) * 1000u;
        }

        __LOOP_TIMER_1__( player->GetKeyID(), setting->_hp_item_cd_time * 1000u, delaytime, &KFClinicModule::OnTimerAddItem );
    }

    __KF_RECORD_VALUE_FUNCTION__( KFClinicModule::GetDayClinicTotalNum )
    {
        if ( !IsClinicActive( player ) )
        {
            return;
        }

        auto kfclinic = player->Find( __STRING__( clinic ) );

        // 当天产量
        auto totalnum = 0u;
        if ( player->IsInited() )
        {
            // 零点逻辑
            auto daynum = kfclinic->Get<uint32>( __STRING__( daynum ) );
            if ( daynum == 0u )
            {
                return;
            }

            totalnum = daynum;
            player->UpdateData( kfclinic, __STRING__( daynum ), KFEnum::Set, 0u );
        }
        else
        {
            // 上线逻辑
            auto setting = GetClinicSetting( player );
            if ( setting == nullptr || setting->_hp_item_cd_time == 0u )
            {
                return;
            }

            auto calctime = kfclinic->Get<uint64>( __STRING__( calctime ) );
            if ( calctime == 0u )
            {
                // 物品已满
                auto daynum = kfclinic->Get<uint32>( __STRING__( daynum ) );
                if ( daynum == 0u )
                {
                    return;
                }

                auto daytime = kfclinic->Get<uint64>( __STRING__( daytime ) );
                if ( daytime < lasttime || daytime > nowtime )
                {
                    return;
                }

                totalnum = daynum;

                player->UpdateData( kfclinic, __STRING__( daynum ), KFEnum::Set, 0u );
            }
            else if ( lasttime < calctime )
            {
                // 当天内
                if ( nowtime < calctime )
                {
                    return;
                }

                auto count = ( nowtime - calctime ) / setting->_hp_item_cd_time;
                auto addnum = count * setting->_hp_item_add_count;

                auto num = kfclinic->Get<uint32>( __STRING__( num ) );
                if ( num + addnum > setting->_hp_item_max_count )
                {
                    addnum = setting->_hp_item_max_count - num;
                    calctime = 0u;
                }
                else
                {
                    calctime += count * setting->_hp_item_cd_time;
                }

                auto daynum = kfclinic->Get<uint32>( __STRING__( daynum ) );
                totalnum = daynum + addnum;

                player->UpdateData( kfclinic, __STRING__( calctime ), KFEnum::Set, calctime );
                player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Add, addnum );
                player->UpdateData( kfclinic, __STRING__( daynum ), KFEnum::Set, 0u );
            }
            else
            {
                auto count = ( lasttime - calctime ) / setting->_hp_item_cd_time;
                auto addnum = count * setting->_hp_item_add_count;

                auto num = kfclinic->Get<uint32>( __STRING__( num ) );
                num += addnum;
                if ( num > setting->_hp_item_max_count )
                {
                    player->UpdateData( kfclinic, __STRING__( calctime ), KFEnum::Set, 0u );
                    player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Set, setting->_hp_item_max_count );
                    player->UpdateData( kfclinic, __STRING__( daynum ), KFEnum::Set, 0u );

                    return;
                }
                else
                {
                    calctime += count * setting->_hp_item_cd_time;
                }

                count = KFTimeEnum::OneDaySecond / setting->_hp_item_cd_time;
                addnum = count * setting->_hp_item_add_count;

                if ( num + addnum > setting->_hp_item_max_count )
                {
                    addnum = setting->_hp_item_max_count - num;
                    calctime = 0u;
                }
                else
                {
                    calctime += count * setting->_hp_item_cd_time;
                }

                player->UpdateData( kfclinic, __STRING__( calctime ), KFEnum::Set, calctime );
                player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Add, addnum );
                player->UpdateData( kfclinic, __STRING__( daynum ), KFEnum::Set, 0u );
            }
        }

        if ( totalnum == 0u )
        {
            return;
        }

        dbvalue.AddValue( __STRING__( clinicnum ), totalnum );
    }

    __KF_ADD_DATA_FUNCTION__( KFClinicModule::OnAddClinicBuild )
    {
        // 医疗所功能开启
        CheckClinicTimer( player );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFClinicModule::OnItemNumUpdate )
    {
        static auto _option = KFGlobal::Instance()->FindConstant( __STRING__( cliniccdtime ) );
        auto cdtime = _option->_uint32_value;
        if ( cdtime == 0u )
        {
            return;
        }

        auto maxnum = player->Get( __STRING__( effect ), __STRING__( clinicmaxnum ) );
        if ( maxnum == 0u )
        {
            return;
        }

        auto kfclinic = player->Find( __STRING__( clinic ) );
        auto num = kfclinic->Get<uint32>( __STRING__( num ) );
        auto calctime = kfclinic->Get( __STRING__( calctime ) );

        if ( num >= maxnum )
        {
            if ( calctime != 0u )
            {
                // 物品满取消定时器
                __UN_TIMER_1__( player->GetKeyID() );
                player->UpdateData( kfclinic, __STRING__( calctime ), KFEnum::Set, 0u );
            }
        }
        else
        {
            if ( calctime == 0u )
            {
                // 物品不满开启定时器
                __LOOP_TIMER_1__( player->GetKeyID(), cdtime * 1000, 0u, &KFClinicModule::OnTimerAddItem );
                player->UpdateData( kfclinic, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
            }
        }
    }

    __KF_TIMER_FUNCTION__( KFClinicModule::OnTimerAddItem )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto setting = GetClinicSetting( player );
        if ( setting == nullptr )
        {
            return;
        }

        auto kfclinic = player->Find( __STRING__( clinic ) );

        // 单位时间的产量
        auto curnum = kfclinic->Get<uint32>( __STRING__( num ) );
        auto addnum = __MIN__( setting->_hp_item_add_count, setting->_hp_item_max_count - curnum );

        player->UpdateData( kfclinic, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
        player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Add, addnum );
        player->UpdateData( kfclinic, __STRING__( daynum ), KFEnum::Add, addnum );
        player->UpdateData( kfclinic, __STRING__( daytime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
    }

    __KF_MESSAGE_FUNCTION__( KFClinicModule::HandleClinicMedicalFeeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgClinicMedicalFeeReq );

        UInt64List herolist;
        for ( auto i = 0u; i < ( uint32 )kfmsg.uuid_size(); ++i )
        {
            herolist.emplace_back( kfmsg.uuid( i ) );
        }

        // 直接获取费用 tuple
        auto returndata = CalcClinicCureMoney( player, herolist );
        auto costmoney = std::get<1>( returndata );

        KFMsg::MsgClinicMedicalFeeAck ack;
        ack.set_element( costmoney != nullptr ? costmoney->GetElement() : _invalid_string );
        _kf_player->SendToClient( player, KFMsg::MSG_CLINIC_MEDICAL_FEE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFClinicModule::HandleClinicCureReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgClinicCureReq );

        UInt64List herolist;
        for ( auto i = 0u; i < ( uint32 )kfmsg.uuid_size(); ++i )
        {
            herolist.emplace_back( kfmsg.uuid( i ) );
        }

        auto calcresult = CalcClinicCureMoney( player, herolist );
        auto& errnum = std::get<0>( calcresult );
        auto& costmoney = std::get<1>( calcresult );
        auto& setting = std::get<2>( calcresult );
        auto& needcurehp = std::get<3>( calcresult );

        if ( errnum != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, errnum );
        }

        // 金钱是否足够
        auto& dataname = player->RemoveElement( costmoney, _default_multiple, __STRING__( clinic ), 0u, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        auto kfclinic = player->Find( __STRING__( clinic ) );
        auto num = kfclinic->Get( __STRING__( num ) );
        auto totaldecnum = 0;
        auto wanttocure = ( uint32 )kfmsg.uuid_size();
        for ( auto i = 0u; i < wanttocure && i < setting->_cure_count; i++ )
        {
            if ( num < setting->_consume_hp_item_count )
            {
                break;
            }

            auto uuid = kfmsg.uuid( i );

            auto kfhero = player->Find( __STRING__( hero ), uuid );
            if ( kfhero == nullptr )
            {
                continue;
            }

            auto kffight = kfhero->Find( __STRING__( fighter ) );
            auto hp = kffight->Get( __STRING__( hp ) );
            auto maxhp = kffight->Get( __STRING__( maxhp ) );
            if ( hp >= maxhp )
            {
                continue;
            }

            // 计算消耗
            auto addhp = maxhp - hp;
            auto count = ( addhp - 1 ) / setting->_hp_item_cure + 1;
            auto neednum = count * setting->_consume_hp_item_count;
            if ( num < neednum )
            {
                // 道具不足，全部消耗
                count = num / setting->_consume_hp_item_count;
                neednum = count * setting->_consume_hp_item_count;
                addhp = count * setting->_hp_item_cure;
            }

            num -= neednum;
            totaldecnum += neednum;

            player->UpdateData( kffight, __STRING__( hp ), KFEnum::Add, addhp );
        }

        // 消耗道具
        player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Dec, totaldecnum );
    }

    KFrame::KFClinicModule::CalcCureMoneyReturn KFClinicModule::CalcClinicCureMoney( KFEntity* player, const UInt64List& herolist )
    {
        uint32 errnum = KFMsg::Ok;
        const KFElements* costmoney = nullptr;
        const KFClinicSetting* setting = nullptr;
        uint32 needcurehp = 0u;

        do
        {
            if ( !IsClinicActive( player ) )
            {
                errnum = KFMsg::BuildFuncNotActive;
                break;
            }

            setting = GetClinicSetting( player );
            if ( setting == nullptr )
            {
                errnum = KFMsg::BuildFuncNotActive;
                break;
            }

            if ( setting->_hp_item_cure == 0u || setting->_consume_hp_item_count == 0u )
            {
                errnum = KFMsg::ClinicSettingError;
                break;
            }

            if ( herolist.size() > setting->_cure_count )
            {
                errnum = KFMsg::ClinicCountIsNotEnough;
                break;
            }

            auto kfclinic = player->Find( __STRING__( clinic ) );
            auto num = kfclinic->Get( __STRING__( num ) );
            if ( num < setting->_consume_hp_item_count )
            {
                errnum = KFMsg::ClinicItemIsNotEnough;
                break;
            }

            needcurehp = CalcClinicHerosNeedCurehp( player, herolist );
            if ( needcurehp == 0u )
            {
                errnum = KFMsg::ClinicNotNeedCure;
                break;
            }

            costmoney = CalcClinicConsumeMoney( player, setting, needcurehp );
            if ( costmoney == nullptr )
            {
                errnum = KFMsg::FormulaParamError;
                break;
            }
        } while ( false );

        // costmoney 与 setting 在下层判定指针
        return std::make_tuple( errnum, costmoney, setting, needcurehp );
    }

    uint32 KFClinicModule::CalcClinicHerosNeedCurehp( KFEntity* player, const UInt64List& herolist )
    {
        auto needcurehp = 0u;
        for ( auto& uuid : herolist )
        {
            auto kfhero = player->Find( __STRING__( hero ), uuid );
            if ( kfhero == nullptr )
            {
                continue;
            }

            auto kffight = kfhero->Find( __STRING__( fighter ) );
            auto hp = kffight->Get( __STRING__( hp ) );
            auto maxhp = kffight->Get( __STRING__( maxhp ) );
            if ( hp < maxhp )
            {
                needcurehp += maxhp - hp;
            }
        }

        return needcurehp;
    }

    const KFElements*  KFClinicModule::CalcClinicConsumeMoney( KFEntity* player, const KFClinicSetting* setting, uint32 addhp )
    {
        // *记得 setting 与 addhp 在上层判定
        static KFElements _clinic_consume_money;
        _clinic_consume_money.Clear();

        // 公式配置
        auto kfformulaparam = KFFormulaConfig::Instance()->FindFormulaParam( setting->_consume_money_fid );
        if ( kfformulaparam == nullptr ||
                kfformulaparam->_type.empty() ||
                kfformulaparam->_params._objects.size() < 2u )
        {
            return nullptr;
        }

        auto param1 = kfformulaparam->_params._objects[0]->_double_value;
        auto param2 = kfformulaparam->_params._objects[1]->_double_value;

        // 基本金币
        auto moneycount = __MAX__( addhp * param1, param2 );
        // 实际金币
        moneycount = std::round( moneycount * setting->_scale_consume_money );

        // 费用数据格式
        KFElementConfig::Instance()->FormatElement( _clinic_consume_money, kfformulaparam->_type, moneycount );
        return &_clinic_consume_money;
    }
}