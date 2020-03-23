#include "KFRealmTimeModule.hpp"

namespace KFrame
{
    void KFRealmTimeModule::BeforeRun()
    {
        __REGISTER_REALM_ENTER__( &KFRealmTimeModule::OnReamlEnterTimeModule );
        __REGISTER_REALM_MOVE__( &KFRealmTimeModule::OnRealmMoveTimeModule );
        __REGISTER_PVE_START__( &KFRealmTimeModule::OnPVEStartTimeModule );
        __REGISTER_TURN_START__( &KFRealmTimeModule::OnPVETurnStartTimeModule );
    }

    void KFRealmTimeModule::BeforeShut()
    {
        __UN_REALM_ENTER__();
        __UN_REALM_MOVE__();
        __UN_PVE_START__();
        __UN_TURN_FINISH__();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    const KFRealmTimeSetting* KFRealmTimeModule::FindRealmTimeSetting( KFRealmData* kfrealmdata, KFRealmData* kfpvedata )
    {
        auto timeid = 0u;
        if ( kfrealmdata != nullptr )
        {
            auto kfexplorelevel = KFRealmConfig::Instance()->FindRealmLevel( kfrealmdata->_data.id(), kfrealmdata->_data.level() );
            if ( kfexplorelevel == nullptr )
            {
                __LOG_ERROR__( "realm=[{}] level=[{}] can't find setting", kfrealmdata->_data.id(), kfrealmdata->_data.level() );
                return nullptr;
            }

            timeid = kfexplorelevel->_time_id;
        }
        else
        {
            auto kfpvestting = KFPVEConfig::Instance()->FindSetting( kfpvedata->_data.id() );
            if ( kfpvestting == nullptr )
            {
                __LOG_ERROR__( "pve=[{}] can't find setting", kfpvedata->_data.id() );
                return nullptr;
            }

            timeid = kfpvestting->_time_id;
        }

        auto kftimesetting = KFRealmTimeConfig::Instance()->FindSetting( timeid );
        if ( kftimesetting == nullptr && timeid != 0u )
        {
            __LOG_ERROR__( "realm timeid=[{}] can't find setting", timeid );
        }

        return kftimesetting;
    }

    void KFRealmTimeModule::SendUpdateRealmTimeToClient( KFEntity* player, KFRealmData* kfrealmdata )
    {
        KFMsg::MsgUpdateTimeAck ack;
        ack.set_timetype( kfrealmdata->_data.timetype() );
        ack.set_realmduration( kfrealmdata->_data.timetotalstep() );
        ack.set_pveduration( kfrealmdata->_data.timetotalturn() );
        _kf_player->SendToClient( player, KFMsg::MSG_UPDATE_TIME_ACK, &ack );
    }

    __KF_REALM_ENTER_FUNCTION__( KFRealmTimeModule::OnReamlEnterTimeModule )
    {
        auto kftimesetting = FindRealmTimeSetting( kfrealmdata, nullptr );
        if ( kftimesetting == nullptr )
        {
            return;
        }

        const KFRealmTimeData* kftimedata = nullptr;
        if ( kfrealmdata->_data.timetype() == 0u )
        {
            // 还没有计算时间
            kftimedata = kftimesetting->RandRealmTimeData();
            if ( kftimedata == nullptr )
            {
                return __LOG_ERROR__( "realm timeid=[{}] is empty", kftimesetting->_id );
            }
        }
        else
        {
            // 计算本次的持续时间
            kftimedata = kftimesetting->GetCurrentRealmTimeData( kfrealmdata->_data.timetype() );
            if ( kftimedata == nullptr )
            {
                return __LOG_ERROR__( "realm timeid=[{}] type=[{}] can't find setting", kftimesetting->_id, kfrealmdata->_data.timetype() );
            }
        }

        kfrealmdata->_data.set_timecurrentstep( 0 );
        kfrealmdata->_data.set_timetype( kftimedata->_time_type );
        kfrealmdata->_data.set_timetotalstep( KFGlobal::Instance()->RandRange( kftimedata->_min_realm_duration, kftimedata->_max_realm_duration, 1u ) );
        SendUpdateRealmTimeToClient( player, kfrealmdata );
    }

    __KF_REALM_MOVE_FUNCTION__( KFRealmTimeModule::OnRealmMoveTimeModule )
    {
        // 里世界不会更新时间
        if ( kfrealmdata->IsInnerWorld() )
        {
            return;
        }

        // 无限步数不更新
        auto pbrealmdata = &kfrealmdata->_data;
        if ( pbrealmdata->timetotalstep() == KFMsg::InfiniteStep )
        {
            return;
        }

        // 添加移动步数
        pbrealmdata->set_timecurrentstep( pbrealmdata->timecurrentstep() + movestep );
        if ( pbrealmdata->timecurrentstep() < pbrealmdata->timetotalstep() )
        {
            return;
        }

        auto kftimesetting = FindRealmTimeSetting( kfrealmdata, nullptr );
        if ( kftimesetting == nullptr )
        {
            return;
        }

        auto kftimedata = kftimesetting->GetNextRealmTimeData( pbrealmdata->timetype() );
        if ( kftimedata == nullptr )
        {
            return __LOG_ERROR__( "realm timeid=[{}] type=[{}] can't find next setting", kftimesetting->_id, pbrealmdata->timetype() );
        }

        pbrealmdata->set_timetype( kftimedata->_time_type );
        pbrealmdata->set_timecurrentstep( pbrealmdata->timecurrentstep() - pbrealmdata->timetotalstep() );
        pbrealmdata->set_timetotalstep( KFGlobal::Instance()->RandRange( kftimedata->_min_realm_duration, kftimedata->_max_realm_duration, 1u ) );
        SendUpdateRealmTimeToClient( player, kfrealmdata );
    }

    __KF_PVE_START_FUNCTION__( KFRealmTimeModule::OnPVEStartTimeModule )
    {
        auto kftimesetting = FindRealmTimeSetting( kfrealmdata, kfpvedata );
        if ( kftimesetting == nullptr )
        {
            return;
        }

        const KFRealmTimeData* kftimedata = nullptr;
        if ( kfrealmdata != nullptr )
        {
            // 探索中
            kftimedata = kftimesetting->GetCurrentRealmTimeData( kfrealmdata->_data.timetype() );
            if ( kftimedata == nullptr )
            {
                return __LOG_ERROR__( "realm timeid=[{}] type=[{}] can't find setting", kftimesetting->_id, kfrealmdata->_data.timetype() );
            }
        }
        else
        {
            // 不在探索中
            kftimedata = kftimesetting->RandRealmTimeData();
            if ( kftimedata == nullptr )
            {
                return __LOG_ERROR__( "pve timeid=[{}] is empty", kftimesetting->_id );
            }
        }

        kfpvedata->_data.set_timecurrentturn( 0u );
        kfpvedata->_data.set_timetype( kftimedata->_time_type );
        kfpvedata->_data.set_timetotalturn( KFGlobal::Instance()->RandRange( kftimedata->_min_pve_duration, kftimedata->_max_pve_duration, 1u ) );
        SendUpdateRealmTimeToClient( player, kfpvedata );
    }

    __KF_TURN_START_FUNCTION__( KFRealmTimeModule::OnPVETurnStartTimeModule )
    {
        // 里世界不更新
        if ( kfpvedata->IsInnerWorld() || ( kfrealmdata != nullptr && kfrealmdata->IsInnerWorld() ) )
        {
            return;
        }

        // 无限回合数
        auto pbpvedata = &kfpvedata->_data;
        if ( pbpvedata->timetotalturn() == KFMsg::InfiniteTurn )
        {
            return;
        }

        // 更新回合数
        pbpvedata->set_timecurrentturn( pbpvedata->timecurrentturn() + 1u );
        if ( pbpvedata->timecurrentturn() < pbpvedata->timetotalturn() )
        {
            return;
        }

        auto kftimesetting = FindRealmTimeSetting( kfrealmdata, kfpvedata );
        if ( kftimesetting == nullptr )
        {
            return;
        }

        auto kftimedata = kftimesetting->GetNextRealmTimeData( pbpvedata->timetype() );
        if ( kftimedata == nullptr )
        {
            return __LOG_ERROR__( "realm timeid=[{}] type=[{}] can't find next setting", kftimesetting->_id, pbpvedata->timetype() );
        }

        pbpvedata->set_timetype( kftimedata->_time_type );
        pbpvedata->set_timecurrentturn( pbpvedata->timecurrentturn() - pbpvedata->timetotalturn() );
        pbpvedata->set_timetotalturn( KFGlobal::Instance()->RandRange( kftimedata->_min_pve_duration, kftimedata->_max_pve_duration, 1u ) );
        SendUpdateRealmTimeToClient( player, kfpvedata );

        // 如果在探索中, 更新时间
        if ( kfrealmdata != nullptr )
        {
            kfrealmdata->_data.set_timecurrentstep( 0 );
            kfrealmdata->_data.set_timetype( kftimedata->_time_type );
            kfrealmdata->_data.set_timetotalstep( KFGlobal::Instance()->RandRange( kftimedata->_min_realm_duration, kftimedata->_max_realm_duration, 1u ) );
        }
    }
}