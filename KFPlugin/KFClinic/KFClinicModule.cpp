#include "KFClinicModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFClinicModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFClinicModule::OnEnterClinicModule );
        __REGISTER_LEAVE_PLAYER__( &KFClinicModule::OnLeaveClinicModule );
        __REGISTER_RECORD_VALUE__( &KFClinicModule::GetDayClinicTotalNum );

        __REGISTER_ADD_ELEMENT__( __STRING__( clinic ), &KFClinicModule::AddClinicElement );
        __REGISTER_ADD_DATA_2__( __STRING__( build ), KFMsg::ClinicBuild, &KFClinicModule::OnAddClinicBuild );
        __REGISTER_UPDATE_DATA_2__( __STRING__( clinic ), __STRING__( num ), &KFClinicModule::OnItemNumUpdate );

        __REGISTER_EXECUTE__( __STRING__( clinicaddnum ), &KFClinicModule::OnExecuteTechnologyClinicAddNum );
        __REGISTER_EXECUTE__( __STRING__( clinicmaxnum ), &KFClinicModule::OnExecuteTechnologyClinicMaxNum );
        __REGISTER_EXECUTE__( __STRING__( clinicsubmoneypercent ), &KFClinicModule::OnExecuteTechnologyClinicMoneySubPercent );

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

        __UN_ADD_ELEMENT__( __STRING__( smithy ) );
        __UN_ADD_DATA_2__( __STRING__( build ), KFMsg::ClinicBuild );
        __UN_UPDATE_DATA_2__( __STRING__( clinic ), __STRING__( num ) );

        __UN_EXECUTE__( __STRING__( clinicaddnum ) );
        __UN_EXECUTE__( __STRING__( clinicmaxnum ) );
        __UN_EXECUTE__( __STRING__( clinicsubmoneypercent ) );
        ///////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_CLINIC_CURE_REQ );
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

        // 未解锁时获得材料取1级的数据
        auto level = GetClinicLevel( player );
        level = __MAX__( 1u, level );
        auto kfsetting = KFClinicConfig::Instance()->FindSetting( level );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "clinic level=[{}] can't find setting", level );
            return false;
        }

        auto kfclinic = player->Find( __STRING__( clinic ) );
        auto curnum = kfclinic->Get<uint32>( __STRING__( num ) );

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        uint32 totalnum = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( num ), kfresult->_multiple );
        auto maxnum = GetClinicMaterialsMaxNum( player, kfsetting );
        totalnum = __MIN__( totalnum, maxnum - curnum );
        player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Add, totalnum );

        return kfresult->AddResult( __STRING__( num ), totalnum );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFClinicModule::OnEnterClinicModule )
    {
        // 检查医疗所是否开启
        if ( !IsClinicActive( player ) )
        {
            return;
        }

        // 检查定时器
        CheckClinicTimer( player );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFClinicModule::OnLeaveClinicModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    __KF_EXECUTE_FUNCTION__( KFClinicModule::OnExecuteTechnologyClinicAddNum )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto addnum = executedata->_param_list._params[0]->_int_value;
        player->UpdateData( __STRING__( effect ), __STRING__( clinicaddnum ), KFEnum::Add, addnum );
        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFClinicModule::OnExecuteTechnologyClinicMaxNum )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto maxnum = executedata->_param_list._params[0]->_int_value;
        player->UpdateData( __STRING__( effect ), __STRING__( clinicmaxnum ), KFEnum::Add, maxnum );
        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFClinicModule::OnExecuteTechnologyClinicMoneySubPercent )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto percent = executedata->_param_list._params[0]->_int_value;
        player->UpdateData( __STRING__( effect ), __STRING__( clinicsubmoneypercent ), KFEnum::Add, percent );
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
        auto level = GetClinicLevel( player );
        return KFClinicConfig::Instance()->FindSetting( level );
    }

    void KFClinicModule::CheckClinicTimer( KFEntity* player )
    {
        auto setting = GetClinicSetting( player );
        if ( setting == nullptr || setting->_cd_time == 0u )
        {
            return;
        }

        auto kfclinic = player->Find( __STRING__( clinic ) );
        auto num = kfclinic->Get<uint32>( __STRING__( num ) );
        if ( num >= GetClinicMaterialsMaxNum( player, setting ) )
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
            if ( cdtime >= setting->_cd_time )
            {
                auto count = cdtime / setting->_cd_time;
                auto addnum = count * GetClinicUnitTimeMaterialsAddNum( player, setting );
                auto maxnum = GetClinicMaterialsMaxNum( player, setting );
                if ( num + addnum >= maxnum )
                {
                    player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Set, maxnum );
                    return;
                }

                calctime += count * setting->_cd_time;

                player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Add, addnum );
                player->UpdateData( kfclinic, __STRING__( calctime ), KFEnum::Set, calctime );
            }

            delaytime = ( setting->_cd_time - ( nowtime - calctime ) % setting->_cd_time ) * 1000u;
        }

        __LOOP_TIMER_1__( player->GetKeyID(), setting->_cd_time * 1000u, delaytime, &KFClinicModule::OnTimerAddItem );
    }

    uint32 KFClinicModule::GetClinicUnitTimeMaterialsAddNum( KFEntity* player, const KFClinicSetting* setting )
    {
        auto kfeffectaddnum = player->Get( __STRING__( effect ), __STRING__( clinicaddnum ) );
        return setting->_add_num + kfeffectaddnum;
    }

    uint32 KFClinicModule::GetClinicMaterialsMaxNum( KFEntity* player, const KFClinicSetting* setting )
    {
        auto kfeffectmaxnum = player->Get( __STRING__( effect ), __STRING__( clinicmaxnum ) );
        return setting->_max_num + kfeffectmaxnum;
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
            if ( setting == nullptr || setting->_cd_time == 0u )
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

                auto count = ( nowtime - calctime ) / setting->_cd_time;
                auto addnum = count * GetClinicUnitTimeMaterialsAddNum( player, setting );

                auto num = kfclinic->Get<uint32>( __STRING__( num ) );
                auto maxnum = GetClinicMaterialsMaxNum( player, setting );
                if ( num + addnum > maxnum )
                {
                    addnum = maxnum - num;
                    calctime = 0u;
                }
                else
                {
                    calctime += count * setting->_cd_time;
                }

                auto daynum = kfclinic->Get<uint32>( __STRING__( daynum ) );
                totalnum = daynum + addnum;

                player->UpdateData( kfclinic, __STRING__( calctime ), KFEnum::Set, calctime );
                player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Add, addnum );
                player->UpdateData( kfclinic, __STRING__( daynum ), KFEnum::Set, 0u );
            }
            else
            {
                auto count = ( lasttime - calctime ) / setting->_cd_time;
                auto addnum = count * GetClinicUnitTimeMaterialsAddNum( player, setting );

                auto num = kfclinic->Get<uint32>( __STRING__( num ) );
                num += addnum;
                auto maxnum = GetClinicMaterialsMaxNum( player, setting );
                if ( num > maxnum )
                {
                    player->UpdateData( kfclinic, __STRING__( calctime ), KFEnum::Set, 0u );
                    player->UpdateData( kfclinic, __STRING__( num ), KFEnum::Set, maxnum );
                    player->UpdateData( kfclinic, __STRING__( daynum ), KFEnum::Set, 0u );

                    return;
                }
                else
                {
                    calctime += count * setting->_cd_time;
                }

                count = KFTimeEnum::OneDaySecond / setting->_cd_time;
                addnum = count * GetClinicUnitTimeMaterialsAddNum( player, setting );

                if ( num + addnum > maxnum )
                {
                    addnum = maxnum - num;
                    calctime = 0u;
                }
                else
                {
                    calctime += count * setting->_cd_time;
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
        auto setting = GetClinicSetting( player );
        if ( setting == nullptr )
        {
            return;
        }

        auto kfparent = kfdata->GetParent();
        auto num = kfdata->Get<uint32>();
        auto calctime = kfparent->Get<uint64>( __STRING__( calctime ) );
        auto maxnum = GetClinicMaterialsMaxNum( player, setting );
        if ( num >= maxnum )
        {
            if ( calctime != 0u )
            {
                // 物品满取消定时器
                __UN_TIMER_1__( player->GetKeyID() );

                player->UpdateData( kfparent, __STRING__( calctime ), KFEnum::Set, 0u );
            }
        }
        else
        {
            if ( calctime == 0u )
            {
                player->UpdateData( kfparent, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );

                // 物品不满开启定时器
                __LOOP_TIMER_1__( player->GetKeyID(), setting->_cd_time * 1000, 0u, &KFClinicModule::OnTimerAddItem );
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
        auto addnum = GetClinicUnitTimeMaterialsAddNum( player, setting );
        auto curnum = kfclinic->Get<uint32>( __STRING__( num ) );
        auto maxnum = GetClinicMaterialsMaxNum( player, setting );

        if ( addnum + curnum > maxnum )
        {
            addnum = maxnum - curnum;
        }

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

        KFMsg::MsgClinicMedicalFeeAck ack;
        ack.set_element( std::get<1>( returndata ) );
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

        uint32 errnum = KFMsg::Error;
        std::string strcost( _invalid_string );
        const KFClinicSetting* setting = nullptr;
        uint32 needcurehp = 0u;

        std::tie( errnum, strcost, setting, needcurehp ) = CalcClinicCureMoney( player, herolist );
        if ( errnum != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, errnum );
        }

        auto kfclinic = player->Find( __STRING__( clinic ) );
        auto num = kfclinic->Get( __STRING__( num ) );
        if ( num < setting->_consume_num )
        {
            // 医疗所道具不足
            return _kf_display->SendToClient( player, KFMsg::ClinicItemIsNotEnough );
        }

        // 金钱是否足够
        KFElements elementsmoney;
        elementsmoney.Parse( strcost, __FUNC_LINE__ );
        auto& dataname = player->RemoveElement( &elementsmoney, _default_multiple, __STRING__( clinic ), 0u, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        auto totaldecnum = 0;
        for ( auto i = 0u; i < ( uint32 )kfmsg.uuid_size() && i < setting->_count; i++ )
        {
            if ( num < setting->_consume_num )
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
            auto count = ( addhp - 1 ) / setting->_add_hp + 1;
            auto neednum = count * setting->_consume_num;
            if ( num < neednum )
            {
                // 道具不足，全部消耗
                count = num / setting->_consume_num;
                neednum = count * setting->_consume_num;
                addhp = count * setting->_add_hp;
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
        std::string strcost( _invalid_string );
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

            if ( setting->_add_hp == 0u || setting->_consume_num == 0u )
            {
                errnum = KFMsg::ClinicSettingError;
                break;
            }

            if ( herolist.size() > setting->_count )
            {
                errnum = KFMsg::ClinicCountIsNotEnough;
                break;
            }

            needcurehp = CalcClinicHerosNeedCurehp( player, herolist );
            if ( needcurehp == 0u )
            {
                errnum = KFMsg::ClinicNotNeedCure;
                break;
            }

            strcost = CalcClinicMoney( player, setting, needcurehp );
            if ( strcost.empty() )
            {
                errnum = KFMsg::FormulaParamError;
                break;
            }
        } while ( false );

        return std::make_tuple( errnum, strcost, setting, needcurehp );
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

    std::string KFClinicModule::CalcClinicMoney( KFEntity* player, const KFClinicSetting* setting, uint32 addhp )
    {
        // *记得 setting 与 addhp 在上层判定

        // 公式配置
        auto kfformulasetting = KFFormulaConfig::Instance()->FindSetting( setting->_formual_id );
        if ( kfformulasetting == nullptr || kfformulasetting->_type.empty() || kfformulasetting->_params.size() < 2u )
        {
            return _invalid_string;
        }

        // 折扣比例
        auto clinicsubmoneypercent = player->Get<uint32>( __STRING__( effect ), __STRING__( clinicsubmoneypercent ) ) / ( double )KFRandEnum::TenThousand;
        if ( clinicsubmoneypercent >= 1.0f )
        {
            return _invalid_string;
        }

        auto param1 = kfformulasetting->_params[0]->_double_value;
        auto param2 = kfformulasetting->_params[1]->_double_value;
        auto moneycount = static_cast<uint32>(
                              std::round(
                                  static_cast<double>( __MAX__( addhp * param1, param2 ) )
                                  * ( 1.0 - clinicsubmoneypercent )
                              )
                          );

        // 费用数据格式
        return KFElementConfig::Instance()->StringElemnt( kfformulasetting->_type, moneycount, _invalid_int );
    }
}