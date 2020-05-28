#include "KFStoreModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFStoreModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ENTER_PLAYER__( &KFStoreModule::OnEnterStoreModule );
        __REGISTER_LEAVE_PLAYER__( &KFStoreModule::OnLeaveStoreModule );

        __REGISTER_RESET__( 0u, &KFStoreModule::OnResetRefreshStoreCount );
        //////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_STORE_BUY_GOODS_REQ, &KFStoreModule::HandleStoreBuyGoodsReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_STORE_REFRESH_REQ, &KFStoreModule::HandleStoreRefreshReq );
    }

    void KFStoreModule::BeforeShut()
    {
        __UN_RESET__();
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        //////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_STORE_BUY_GOODS_REQ );
        __UN_MESSAGE__( KFMsg::MSG_STORE_REFRESH_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFStoreModule::HandleStoreBuyGoodsReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgStoreBuyGoodsReq );

        auto kfsetting = KFStoreConfig::Instance()->FindSetting( kfmsg.storeid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreSettingError, kfmsg.storeid() );
        }

        // 固定商店
        if ( kfsetting->_refresh_type == KFMsg::RefreshNever )
        {
            // 判断是否存在该商品
            if ( !kfsetting->IsHaveGoods( kfmsg.goodsid() ) )
            {
                return _kf_display->SendToClient( player, KFMsg::StoreNotHaveGoods, kfmsg.goodsid() );
            }

            auto result = _kf_goods->BuyGoods( player, kfmsg.goodsid(), kfmsg.buycount() );
            return _kf_display->SendToClient( player, result );
        }

        // 判断刷新商店中是否有该道具
        auto kfstore = player->Find( __STRING__( store ) );
        if ( kfstore == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreNotRefresh, kfmsg.storeid() );
        }

        auto kfgoods = kfstore->Find( __STRING__( goods ), kfmsg.goodsid() );
        if ( kfgoods == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreNotHaveGoods, kfmsg.goodsid() );
        }

        auto maxcount = kfgoods->Get<uint32>( kfgoods->_data_setting->_value_key_name );
        if ( maxcount < kfmsg.buycount() )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreStockLack );
        }

        auto result = _kf_goods->BuyGoods( player, kfmsg.goodsid(), kfmsg.buycount() );
        if ( result == KFMsg::StoreBuyOK )
        {
            // 购买成功, 减少库存
            player->UpdateData( kfgoods, kfgoods->_data_setting->_value_key_name, KFEnum::Dec, kfmsg.buycount() );
        }
        _kf_display->SendToClient( player, result );
    }

    __KF_MESSAGE_FUNCTION__( KFStoreModule::HandleStoreRefreshReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgStoreRefreshReq );
        auto kfsetting = KFStoreConfig::Instance()->FindSetting( kfmsg.storeid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreSettingError, kfmsg.storeid() );
        }

        if ( !KFUtility::HaveBitMask( kfsetting->_refresh_type, ( uint32 )KFMsg::RefreshManual ) )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreNotRefreshManual );
        }

        auto kfstorerecord = player->Find( __STRING__( store ) );
        switch ( kfmsg.refreshtype() )
        {
        case KFMsg::RefreshByFree:
        {
            // 判断刷新时间
            auto refreshtime = kfstorerecord->Get( kfmsg.storeid(), __STRING__( time ) );
            if ( refreshtime > KFGlobal::Instance()->_real_time )
            {
                return _kf_display->SendToClient( player, KFMsg::StoreRefreshTimeError );
            }

            // 保存时间
            UpdateStoreRefreshTime( player, kfsetting, kfstorerecord );
        }
        break;
        case KFMsg::RefreshByCost:
        {
            auto refreshcount = kfstorerecord->Get( kfmsg.storeid(), __STRING__( refresh ) );
            auto tupldata = StoreRefreshCostElemnts( player, kfsetting, refreshcount + 1u );
            auto result = std::get<0>( tupldata );
            auto refreshtimeid = std::get<1>( tupldata );
            if ( result != KFMsg::Ok )
            {
                return _kf_display->SendToClient( player, result );
            }

            // 设置刷新数量
            if ( refreshtimeid != 0 )
            {
                player->UpdateData( kfstorerecord, kfsetting->_id, __STRING__( refresh ), KFEnum::Add, 1u );
            }

            // 是否要重置时间
            if ( kfsetting->_is_refresh_reset_time )
            {
                UpdateStoreRefreshTime( player, kfsetting, kfstorerecord );
            }
        }
        break;
        default:
            return _kf_display->SendToClient( player, KFMsg::StoreRefreshTypeError );
            break;
        }

        // 刷新商品
        StoreRefreshGoods( player, kfsetting, kfstorerecord );
    }

    std::tuple< uint32, uint32 > KFStoreModule::StoreRefreshCostElemnts( KFEntity* player, const KFStoreSetting* kfsetting, uint32 refreshcount )
    {
        if ( kfsetting->_refresh_cost_list.empty() )
        {
            return std::make_tuple( KFMsg::StoreRefreshCostEmpty, 0u );
        }

        // 刷新次数
        for ( auto& costid : kfsetting->_refresh_cost_list )
        {
            auto tupledata = KFCountCostConfig::Instance()->GetCostByCount( costid, refreshcount );
            auto costelement = std::get<0>( tupledata );
            auto refreshtimeid = std::get<1>( tupledata );
            if ( costelement == nullptr )
            {
                continue;
            }

            auto& dataname = player->RemoveElement( costelement, 1u, __STRING__( store ), kfsetting->_id, __FUNC_LINE__ );
            if ( dataname.empty() )
            {
                return std::make_tuple( KFMsg::Ok, refreshtimeid );
            }
        }

        return std::make_tuple( KFMsg::DataNotEnough, 0u );
    }

    void KFStoreModule::UpdateStoreRefreshTime( KFEntity* player, const KFStoreSetting* kfsetting, KFData* kfstorerecord )
    {
        auto nexttime = _kf_reset->CalcNextResetTime( KFGlobal::Instance()->_real_time, kfsetting->_refresh_time_id );
        player->UpdateData( kfstorerecord, kfsetting->_id, __STRING__( time ), KFEnum::Set, nexttime );
    }

    void KFStoreModule::RefreshGoods( KFEntity* player, uint32 storeid )
    {
        auto kfsetting = KFStoreConfig::Instance()->FindSetting( storeid );
        if ( kfsetting == nullptr || !KFUtility::HaveBitMask( kfsetting->_refresh_type, ( uint32 )KFMsg::RefreshAuto ) )
        {
            return;
        }

        auto refreshtime = player->Get( __STRING__( store ), kfsetting->_id, __STRING__( time ) );
        StartRefreshStoreTimer( player, kfsetting->_id, refreshtime );
    }

    void KFStoreModule::StoreRefreshGoods( KFEntity* player, const KFStoreSetting* kfsetting, KFData* kfstorerecord )
    {
        if ( kfsetting->_refresh_group_count.empty() )
        {
            return;
        }

        // 商品列表
        auto kfgoodsrecord = kfstorerecord->Find( kfsetting->_id, __STRING__( goods ) );
        player->CleanData( kfgoodsrecord, false );

        for ( auto& tupledata : kfsetting->_refresh_group_count )
        {
            auto groupid = std::get<0>( tupledata );
            auto mincount = std::get<1>( tupledata );
            auto maxcount = std::get<2>( tupledata );

            UInt32Set excludelist;
            auto count = KFGlobal::Instance()->RandRange( mincount, maxcount, 1u );
            for ( auto i = 0u; i < count; ++i )
            {
                auto goodstupledata = _kf_goods->RandGoods( player, groupid, excludelist );
                auto goodsid = std::get<0>( goodstupledata );
                auto stock = std::get<1>( goodstupledata );
                if ( goodsid == 0u || stock == 0u )
                {
                    continue;
                }

                player->UpdateData( kfgoodsrecord, goodsid, kfgoodsrecord->_data_setting->_value_key_name, KFEnum::Add, stock );

                // 判断是否需要排除相同商品
                if ( kfsetting->_random_type == KFMsg::ExcludeRandom )
                {
                    excludelist.insert( goodsid );
                }
            }
        }

        _kf_display->DelayToClient( player, KFMsg::StoreRefreshOk );
    }

    void KFStoreModule::RemoveStore( KFEntity* player, uint32 storeid )
    {
        player->RemoveData( __STRING__( store ), storeid );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFStoreModule::OnEnterStoreModule )
    {
        auto kfstorerecord = player->Find( __STRING__( store ) );
        for ( auto& iter : KFStoreConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !KFUtility::HaveBitMask( kfsetting->_refresh_type, ( uint32 )KFMsg::RefreshAuto ) )
            {
                continue;
            }

            // 木有激活
            if ( !_kf_store_unlock->IsStoreUnlock( player, kfsetting->_id ) )
            {
                continue;
            }

            // 启动定时器
            auto refreshtime = kfstorerecord->Get( kfsetting->_id, __STRING__( time ) );
            StartRefreshStoreTimer( player, kfsetting->_id, refreshtime );
        }
    }

    void KFStoreModule::StartRefreshStoreTimer( KFEntity* player, uint32 storeid, uint64 refreshtime )
    {
        auto intervaltime = ( refreshtime - __MIN__( refreshtime, KFGlobal::Instance()->_real_time ) ) * KFTimeEnum::OneSecondMicSecond;
        intervaltime = __MAX__( 1, intervaltime );
        __LIMIT_TIMER_2__( player->GetKeyID(), storeid, intervaltime, 1u, &KFStoreModule::OnTimerRefreshStore );
    }

    __KF_TIMER_FUNCTION__( KFStoreModule::OnTimerRefreshStore )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto kfsetting = KFStoreConfig::Instance()->FindSetting( subid );
        if ( kfsetting == nullptr || kfsetting->_refresh_time_id == 0u )
        {
            return;
        }

        // 保存下次时间
        auto kfstorerecord = player->Find( __STRING__( store ) );
        auto nexttime = _kf_reset->CalcNextResetTime( KFGlobal::Instance()->_real_time, kfsetting->_refresh_time_id );
        player->UpdateData( kfstorerecord, kfsetting->_id, __STRING__( time ), KFEnum::Set, nexttime );

        // 刷新商品
        StoreRefreshGoods( player, kfsetting, kfstorerecord );

        // 下次刷新定时器
        StartRefreshStoreTimer( player, kfsetting->_id, nexttime );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFStoreModule::OnLeaveStoreModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    __KF_RESET_FUNCTION__( KFStoreModule::OnResetRefreshStoreCount )
    {
        auto kfstorerecord = player->Find( __STRING__( store ) );
        for ( auto kfstore = kfstorerecord->First(); kfstore != nullptr; kfstore = kfstorerecord->Next() )
        {
            auto refreshcount = kfstore->Get<uint32>( __STRING__( refresh ) );
            if ( refreshcount == 0u )
            {
                continue;
            }

            auto kfsetting = KFStoreConfig::Instance()->FindSetting( kfstore->GetKeyID() );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            for ( auto& costid : kfsetting->_refresh_cost_list )
            {
                auto kfcostsetting = KFCountCostConfig::Instance()->FindSetting( costid );
                if ( kfcostsetting->_refresh_time_id == 0u )
                {
                    continue;
                }

                if ( _kf_reset->CheckResetTime( player, kfcostsetting->_refresh_time_id ) )
                {
                    player->UpdateData( kfstore, __STRING__( refresh ), KFEnum::Set, 0u );
                    break;
                }
            }
        }
    }
}