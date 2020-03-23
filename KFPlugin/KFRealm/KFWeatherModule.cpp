#include "KFWeatherModule.hpp"

namespace KFrame
{
    void KFWeatherModule::BeforeRun()
    {
        __REGISTER_REALM_ENTER__( &KFWeatherModule::OnReamlEnterWeatherModule );
        __REGISTER_REALM_MOVE__( &KFWeatherModule::OnRealmMoveWeatherModule );
        __REGISTER_PVE_START__( &KFWeatherModule::OnPVEStartWeatherModule );
        __REGISTER_TURN_START__( &KFWeatherModule::OnPVETurnStartWeatherModule );
    }

    void KFWeatherModule::BeforeShut()
    {
        __UN_REALM_ENTER__();
        __UN_REALM_MOVE__();
        __UN_PVE_START__();
        __UN_TURN_START__();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFWeatherModule::SendUpdateWeatherToClient( KFEntity* player, KFRealmData* kfrealmdata )
    {
        KFMsg::MsgUpdateWeatherAck ack;
        ack.set_weahterid( kfrealmdata->_data.weather() );
        ack.set_nextweahterid( kfrealmdata->_data.nextweather() );
        ack.set_realmtotalduration( kfrealmdata->_data.weathertotalstep() );
        ack.set_realmcurrentduration( kfrealmdata->_data.weathercurrentstep() );
        ack.set_pvetotalduration( kfrealmdata->_data.weathertotalturn() );
        ack.set_pvecurrentduration( kfrealmdata->_data.weathercurrentturn() );
        _kf_player->SendToClient( player, KFMsg::MSG_UDPATE_WEATHER_ACK, &ack );
    }

    const KFWeatherSetting* KFWeatherModule::FindWeatherSetting( KFRealmData* kfrealmdata, KFRealmData* kfpvedata )
    {
        auto weatherid = 0u;
        if ( kfrealmdata != nullptr )
        {
            auto kfexplorelevel = KFRealmConfig::Instance()->FindRealmLevel( kfrealmdata->_data.id(), kfrealmdata->_data.level() );
            if ( kfexplorelevel == nullptr )
            {
                __LOG_ERROR__( "realm=[{}] level=[{}] can't find setting", kfrealmdata->_data.id(), kfrealmdata->_data.level() );
                return nullptr;
            }

            weatherid = kfexplorelevel->_weather_id;
        }
        else
        {
            auto kfpvestting = KFPVEConfig::Instance()->FindSetting( kfpvedata->_data.id() );
            if ( kfpvestting == nullptr )
            {
                __LOG_ERROR__( "pve=[{}] can't find setting", kfpvedata->_data.id() );
                return nullptr;
            }

            weatherid = kfpvestting->_weather_id;
        }

        auto kfweathersetting = KFWeatherConfig::Instance()->FindSetting( weatherid );
        if ( kfweathersetting == nullptr && weatherid != 0u )
        {
            __LOG_ERROR__( "realm weatherid=[{}] can't find setting", weatherid );
        }

        return kfweathersetting;
    }

    __KF_REALM_ENTER_FUNCTION__( KFWeatherModule::OnReamlEnterWeatherModule )
    {
        auto kfweathersetting = FindWeatherSetting( kfrealmdata, nullptr );
        if ( kfweathersetting == nullptr )
        {
            return;
        }

        const KFWeatherData* kfweatherdata = nullptr;
        if ( kfrealmdata->_data.weather() == 0u )
        {
            // 还没有计算天气
            kfweatherdata = kfweathersetting->RandWeatherData();
            if ( kfweatherdata == nullptr )
            {
                return __LOG_ERROR__( "realm weatherid=[{}] is empty", kfweathersetting->_id );
            }
        }
        else
        {
            // 计算本次的持续天气
            kfweatherdata = kfweathersetting->GetCurrentWeatherData( kfrealmdata->_data.weather() );
            if ( kfweatherdata == nullptr )
            {
                return __LOG_ERROR__( "realm timeid=[{}] type=[{}] can't find setting", kfweathersetting->_id, kfrealmdata->_data.weather() );
            }
        }

        kfrealmdata->_data.set_weathercurrentstep( 0 );
        kfrealmdata->_data.set_weather( kfweatherdata->_weahter_id );
        kfrealmdata->_data.set_weathertotalstep( KFGlobal::Instance()->RandRange( kfweatherdata->_min_realm_duration, kfweatherdata->_max_realm_duration, 1u ) );
        SendUpdateWeatherToClient( player, kfrealmdata );
    }

    __KF_REALM_MOVE_FUNCTION__( KFWeatherModule::OnRealmMoveWeatherModule )
    {
        // 里世界不会更新天气
        if ( kfrealmdata->IsInnerWorld() )
        {
            return;
        }

        // 无限步数不更新
        auto pbrealmdata = &kfrealmdata->_data;
        if ( pbrealmdata->weathertotalstep() == KFMsg::InfiniteStep )
        {
            return;
        }

        // 添加移动步数
        pbrealmdata->set_weathercurrentstep( pbrealmdata->weathercurrentstep() + movestep );
        if ( pbrealmdata->weathercurrentstep() < pbrealmdata->weathertotalstep() )
        {
            return;
        }

        auto kfweathersetting = FindWeatherSetting( kfrealmdata, nullptr );
        if ( kfweathersetting == nullptr )
        {
            return;
        }

        auto kfweather = kfweathersetting->RandWeatherData();
        if ( kfweather == nullptr )
        {
            return __LOG_ERROR__( "realm weatherid=[{}] is empty", kfweathersetting->_id );
        }

        pbrealmdata->set_weather( kfweather->_weahter_id );
        pbrealmdata->set_weathercurrentstep( pbrealmdata->weathercurrentstep() - pbrealmdata->weathertotalstep() );
        pbrealmdata->set_weathertotalstep( KFGlobal::Instance()->RandRange( kfweather->_min_realm_duration, kfweather->_max_realm_duration, 1u ) );
        SendUpdateWeatherToClient( player, kfrealmdata );
    }

    __KF_PVE_START_FUNCTION__( KFWeatherModule::OnPVEStartWeatherModule )
    {
        auto kfweathersetting = FindWeatherSetting( kfrealmdata, kfpvedata );
        if ( kfweathersetting == nullptr )
        {
            return;
        }

        const KFWeatherData* kfweatherdata = nullptr;
        if ( kfrealmdata != nullptr )
        {
            // 探索中
            kfweatherdata = kfweathersetting->GetCurrentWeatherData( kfrealmdata->_data.weather() );
            if ( kfweatherdata == nullptr )
            {
                return __LOG_ERROR__( "realm weatherid=[{}] weather=[{}] can't find setting", kfweathersetting->_id, kfrealmdata->_data.weather() );
            }
        }
        else
        {
            // 不在探索中
            kfweatherdata = kfweathersetting->RandWeatherData();
            if ( kfweatherdata == nullptr )
            {
                return __LOG_ERROR__( "pve timeid=[{}] is empty", kfweathersetting->_id );
            }
        }

        // 随机下一次天气
        RandNextWeatherData( kfpvedata, kfweathersetting );

        kfpvedata->_data.set_weathercurrentturn( 0u );
        kfpvedata->_data.set_weather( kfweatherdata->_weahter_id );
        kfpvedata->_data.set_weathertotalturn( KFGlobal::Instance()->RandRange( kfweatherdata->_min_pve_duration, kfweatherdata->_max_pve_duration, 1u ) );
        SendUpdateWeatherToClient( player, kfpvedata );
    }

    void KFWeatherModule::RandNextWeatherData( KFRealmData* kfpvedata, const KFWeatherSetting* kfweathersetting )
    {
        auto kfnextweatherdata = kfweathersetting->RandWeatherData();
        if ( kfnextweatherdata == nullptr )
        {
            return;
        }

        kfpvedata->_data.set_nextweather( kfnextweatherdata->_weahter_id );
    }

    __KF_TURN_START_FUNCTION__( KFWeatherModule::OnPVETurnStartWeatherModule )
    {
        // 里世界不更新
        if ( kfpvedata->IsInnerWorld() || ( kfrealmdata != nullptr && kfrealmdata->IsInnerWorld() ) )
        {
            return;
        }

        // 无限回合数
        auto pbpvedata = &kfpvedata->_data;
        if ( pbpvedata->weathertotalturn() == KFMsg::InfiniteTurn )
        {
            return;
        }

        // 更新回合数
        pbpvedata->set_weathercurrentturn( pbpvedata->weathercurrentturn() + 1u );
        if ( pbpvedata->weathercurrentturn() < pbpvedata->weathertotalturn() )
        {
            return;
        }

        auto kfweathersetting = FindWeatherSetting( kfrealmdata, kfpvedata );
        if ( kfweathersetting == nullptr )
        {
            return;
        }

        auto kfweatherdata = kfweathersetting->GetCurrentWeatherData( pbpvedata->nextweather() );
        if ( kfweatherdata == nullptr )
        {
            return __LOG_ERROR__( "realm timeid=[{}] type=[{}] can't find next setting", kfweathersetting->_id, pbpvedata->timetype() );
        }

        // 随机下次天气
        RandNextWeatherData( kfpvedata, kfweathersetting );

        pbpvedata->set_weather( kfweatherdata->_weahter_id );
        pbpvedata->set_weathercurrentturn( pbpvedata->timecurrentturn() - pbpvedata->timetotalturn() );
        pbpvedata->set_weathertotalturn( KFGlobal::Instance()->RandRange( kfweatherdata->_min_pve_duration, kfweatherdata->_max_pve_duration, 1u ) );
        SendUpdateWeatherToClient( player, kfpvedata );

        // 如果在探索中, 更新时间
        if ( kfrealmdata != nullptr )
        {
            kfrealmdata->_data.set_weathercurrentstep( 0 );
            kfrealmdata->_data.set_weather( kfweatherdata->_weahter_id );
            kfrealmdata->_data.set_weathertotalstep( KFGlobal::Instance()->RandRange( kfweatherdata->_min_realm_duration, kfweatherdata->_max_realm_duration, 1u ) );
        }
    }
}