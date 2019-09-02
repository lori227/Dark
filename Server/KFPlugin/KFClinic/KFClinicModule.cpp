#include "KFClinicModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFClinicModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFClinicModule::OnEnterClinicModule );
        __REGISTER_LEAVE_PLAYER__( &KFClinicModule::OnLeaveClinicModule );

        __REGISTER_RECORD_VALUE__( &KFClinicModule::GetDayTotalNum );

        __REGISTER_UPDATE_DATA_1__( __KF_STRING__( buildlevel ), &KFClinicModule::OnLevelUpdate );
        __REGISTER_UPDATE_DATA_2__( __KF_STRING__( clinic ), __KF_STRING__( num ), &KFClinicModule::OnItemNumUpdate );
        __REGISTER_MESSAGE__( KFMsg::MSG_CLINIC_CURE_REQ, &KFClinicModule::HandleClinicCureReq );
    }

    void KFClinicModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        __UN_RECORD_VALUE__();

        __UN_MESSAGE__( KFMsg::MSG_CLINIC_CURE_REQ );

        __UN_UPDATE_DATA_1__( __KF_STRING__( buildlevel ) );
        __UN_UPDATE_DATA_2__( __KF_STRING__( clinic ), __KF_STRING__( num ) );
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

    uint32 KFClinicModule::GetClinicLevel( KFEntity* player )
    {
        auto kflevel = player->GetData()->FindData( __KF_STRING__( buildlevel ), KFMsg::ClinicBuild );
        if ( kflevel == nullptr )
        {
            return 0u;
        }

        return kflevel->GetValue();
    }

    bool KFClinicModule::IsClinicActive( KFEntity* player )
    {
        auto level = GetClinicLevel( player );
        return level > 0u ? true : false;
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

        auto kfobject = player->GetData();
        auto kfclinic = kfobject->FindData( __KF_STRING__( clinic ) );

        auto nowtime = KFGlobal::Instance()->_real_time;
        if ( kfclinic->GetValue( __KF_STRING__( id ) ) == 0u )
        {
            player->UpdateData( kfclinic, __KF_STRING__( id ), KFEnum::Set, setting->_item_id );
            player->UpdateData( kfclinic, __KF_STRING__( calctime ), KFEnum::Set, nowtime );

            // 启动定时器
            __LOOP_TIMER_2__( player->GetKeyID(), setting->_item_id, setting->_cd_time * 1000, 0u, &KFClinicModule::OnTimerAddItem );
        }
        else
        {
            auto num = kfclinic->GetValue<uint32>( __KF_STRING__( num ) );
            auto calctime = kfclinic->GetValue<uint64>( __KF_STRING__( calctime ) );
            auto totalnum = kfclinic->GetValue<uint32>( __KF_STRING__( totalnum ) );

            if ( calctime == 0u )
            {
                // 物品已满
                return;
            }

            auto delaytime = setting->_cd_time - ( nowtime - calctime ) % setting->_cd_time;
            __LOOP_TIMER_2__( player->GetKeyID(), setting->_item_id, setting->_cd_time * 1000, delaytime, &KFClinicModule::OnTimerAddItem );
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFClinicModule::OnLeaveClinicModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    __KF_RECORD_VALUE_FUNCTION__( KFClinicModule::GetDayTotalNum )
    {
        if ( !IsClinicActive( player ) )
        {
            return;
        }

        auto kfclinic = player->GetData()->FindData( __KF_STRING__( clinic ) );

        // 当天产量
        auto daynum = 0u;

        if ( player->IsInited() )
        {
            // 零点逻辑
            auto totalnum = kfclinic->GetValue<uint32>( __KF_STRING__( totalnum ) );
            if ( totalnum == 0u )
            {
                return;
            }

            daynum = totalnum;

            player->UpdateData( kfclinic, __KF_STRING__( totalnum ), KFEnum::Set, 0u );
        }
        else
        {
            // 上线逻辑
            auto calctime = kfclinic->GetValue<uint64>( __KF_STRING__( calctime ) );
            if ( calctime == 0u )
            {
                // 物品已满
                return;
            }

            auto setting = GetClinicSetting( player );
            if ( setting == nullptr || setting->_cd_time == 0u )
            {
                return;
            }

            if ( lasttime < calctime )
            {
                // 当天内
                if ( nowtime < calctime )
                {
                    return;
                }

                auto count = ( nowtime - calctime ) / setting->_cd_time;
                auto addnum = count * setting->_add_num;

                auto num = kfclinic->GetValue<uint32>( __KF_STRING__( num ) );
                if ( num + addnum > setting->_max_num )
                {
                    addnum = setting->_max_num - num;
                    calctime = 0u;
                }
                else
                {
                    calctime += count * setting->_cd_time;
                }

                auto totalnum = kfclinic->GetValue<uint32>( __KF_STRING__( totalnum ) );
                daynum = totalnum + addnum;

                player->UpdateData( kfclinic, __KF_STRING__( calctime ), KFEnum::Set, calctime );
                player->UpdateData( kfclinic, __KF_STRING__( num ), KFEnum::Add, addnum );
                player->UpdateData( kfclinic, __KF_STRING__( totalnum ), KFEnum::Set, 0u );
            }
            else
            {
                auto count = ( lasttime - calctime ) / setting->_cd_time;
                auto addnum = count * setting->_add_num;

                auto num = kfclinic->GetValue<uint32>( __KF_STRING__( num ) );
                num += addnum;
                if ( num > setting->_max_num )
                {
                    player->UpdateData( kfclinic, __KF_STRING__( calctime ), KFEnum::Set, 0u );
                    player->UpdateData( kfclinic, __KF_STRING__( num ), KFEnum::Set, setting->_max_num );
                    player->UpdateData( kfclinic, __KF_STRING__( totalnum ), KFEnum::Set, 0u );

                    return;
                }
                else
                {
                    calctime += count * setting->_cd_time;
                }

                count = KFTimeEnum::OneDaySecond / setting->_cd_time;
                addnum = count * setting->_add_num;
                if ( num + addnum > setting->_max_num )
                {
                    addnum = setting->_max_num - num;
                    calctime = 0u;
                }
                else
                {
                    calctime += count * setting->_cd_time;
                }

                player->UpdateData( kfclinic, __KF_STRING__( calctime ), KFEnum::Set, calctime );
                player->UpdateData( kfclinic, __KF_STRING__( num ), KFEnum::Add, addnum );
                player->UpdateData( kfclinic, __KF_STRING__( totalnum ), KFEnum::Set, 0u );
            }
        }

        if ( daynum == 0u )
        {
            return;
        }

        values[__KF_STRING__( clinicnum )] = __TO_STRING__( daynum );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFClinicModule::OnLevelUpdate )
    {
        // 医疗所功能开启
        if ( key != KFMsg::ClinicBuild )
        {
            return;
        }

        auto level = GetClinicLevel( player );
        if ( level == 1u )
        {
            CheckClinicTimer( player );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFClinicModule::OnItemNumUpdate )
    {
        auto setting = GetClinicSetting( player );
        if ( setting == nullptr )
        {
            return;
        }

        auto id = kfdata->GetParent()->GetValue<uint32>( __KF_STRING__( id ) );
        auto num = kfdata->GetValue<uint32>();
        auto calctime = kfdata->GetParent()->GetValue<uint64>( __KF_STRING__( calctime ) );
        if ( num >= setting->_max_num  )
        {
            if ( calctime != 0u )
            {
                // 物品满取消定时器
                __UN_TIMER_2__( player->GetKeyID(), id );

                player->UpdateData( kfdata->GetParent(), __KF_STRING__( calctime ), KFEnum::Set, 0u );
            }
        }
        else
        {
            auto calctime = kfdata->GetParent()->GetValue<uint64>( __KF_STRING__( calctime ) );
            if ( calctime == 0u )
            {
                player->UpdateData( kfdata->GetParent(), __KF_STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );

                // 物品不满开启定时器
                __LOOP_TIMER_2__( player->GetKeyID(), id, setting->_cd_time * 1000, 0u, &KFClinicModule::OnTimerAddItem );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClinicModule::HandleClinicCureReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgClinicCureReq );

        if ( !IsClinicActive( player ) )
        {
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        auto kfobject = player->GetData();
        auto kfhero = kfobject->FindData( __KF_STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto kffight = kfhero->FindData( __KF_STRING__( fighter ) );
        auto hp = kffight->GetValue( __KF_STRING__( hp ) );
        auto maxhp = kffight->GetValue( __KF_STRING__( maxhp ) );
        if ( hp >= maxhp )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroHpIsMax );
        }

        auto setting = GetClinicSetting( player );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        if ( setting->_add_hp == 0u || setting->_consume_num == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::ClinicSettingError );
        }

        auto kfclinic = kfobject->FindData( __KF_STRING__( clinic ) );
        auto num = kfclinic->GetValue( __KF_STRING__( num ) );
        if ( num < setting->_consume_num )
        {
            // 医疗所道具不足
            return _kf_display->SendToClient( player, KFMsg::ClinicItemIsNotEnough );
        }

        // 金钱是否足够
        auto dataname = player->CheckRemoveElement( &setting->_money, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }
        player->RemoveElement( &setting->_money, __FUNC_LINE__ );

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

        // 消耗道具
        player->UpdateData( kfclinic, __KF_STRING__( num ), KFEnum::Dec, neednum );
        player->UpdateData( kffight, __KF_STRING__( hp ), KFEnum::Add, addhp );
    }

    __KF_TIMER_FUNCTION__( KFClinicModule::OnTimerAddItem )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto kfclinic = player->GetData()->FindData( __KF_STRING__( clinic ) );
        if ( kfclinic->GetValue( __KF_STRING__( id ) ) != subid )
        {
            return;
        }

        auto setting = GetClinicSetting( player );
        if ( setting == nullptr )
        {
            return;
        }

        auto calctime = kfclinic->GetValue<uint64>( __KF_STRING__( calctime ) );
        auto nowtime = KFGlobal::Instance()->_real_time;
        auto count = ( nowtime - calctime ) / setting->_cd_time;

        auto addnum = count * setting->_add_num;
        auto curnum = kfclinic->GetValue<uint32>( __KF_STRING__( num ) );
        if ( addnum + curnum > setting->_max_num )
        {
            addnum = setting->_max_num - curnum;
        }

        player->UpdateData( kfclinic, __KF_STRING__( num ), KFEnum::Add, addnum );
        player->UpdateData( kfclinic, __KF_STRING__( totalnum ), KFEnum::Add, addnum );
        player->UpdateData( kfclinic, __KF_STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
    }
}