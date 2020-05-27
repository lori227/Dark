#include "KFGoodsModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFGoodsModule::BeforeRun()
    {
        __REGISTER_RESET__( 0u, &KFGoodsModule::OnResetRefreshGoods );
    }

    void KFGoodsModule::BeforeShut()
    {
        __UN_RESET__();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_RESET_FUNCTION__( KFGoodsModule::OnResetRefreshGoods )
    {
        auto kfgoodsrecord = player->Find( __STRING__( goods ) );
        if ( kfgoodsrecord == nullptr )
        {
            return;
        }

        UInt64Set removelist;
        for ( auto kfgoods = kfgoodsrecord->First(); kfgoods != nullptr; kfgoods = kfgoodsrecord->Next() )
        {
            auto kfsetting = KFGoodsConfig::Instance()->FindSetting( kfgoods->GetKeyID() );
            if ( kfsetting == nullptr || kfsetting->_limit_buy_time_id == 0 )
            {
                continue;
            }

            if ( !kfsetting->IsLimitBuy() ||
                    _kf_reset->CheckResetTime( player, kfsetting->_limit_buy_time_id ) )
            {
                removelist.insert( kfsetting->_id );
            }
        }

        if ( removelist.empty() )
        {
            return;
        }

        for ( auto goodsid : removelist )
        {
            player->RemoveData( kfgoodsrecord, goodsid, false );
        }
        _kf_display->DelayToClient( player, KFMsg::StoreGoodsRefreshOk );
    }

    uint32 KFGoodsModule::BuyGoods( KFEntity* player, uint32 goodsid, uint32 buycount )
    {
        auto kfsetting = KFGoodsConfig::Instance()->FindSetting( goodsid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::StoreGoodsSettingError;
        }

        if ( !kfsetting->CheckInBuyTime( KFGlobal::Instance()->_real_time ) )
        {
            return KFMsg::StoreGoodsBuyTimeError;
        }

        // 判断购买数量
        if ( buycount < kfsetting->_min_buy_count || buycount > kfsetting->_max_buy_count )
        {
            return KFMsg::StoreBuyCountError;
        }

        // 判断如果是限购商品
        if ( kfsetting->IsLimitBuy() )
        {
            auto oldcount = player->Get<uint32>( __STRING__( goods ), goodsid, __STRING__( value ) );
            if ( oldcount + buycount > kfsetting->_limit_buy_count )
            {
                return KFMsg::StoreOutOfLimits;
            }
        }

        // 判断包裹是否满了
        auto& checkname = player->CheckAddElement( &kfsetting->_goods_data, buycount, __FUNC_LINE__ );
        if ( !checkname.empty() )
        {
            return KFMsg::ItemBagIsFull;
        }

        // 判断价钱
        auto costprice = kfsetting->CalcBuyPrice( KFGlobal::Instance()->_real_time );
        auto& dataname = player->RemoveElement( costprice, buycount, __STRING__( goods ), goodsid, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return KFMsg::StoreLackCost;
        }

        // 保存限购数量
        if ( kfsetting->IsLimitBuy() )
        {
            player->UpdateData( __STRING__( goods ), goodsid, __STRING__( value ), KFEnum::Add, buycount );
        }

        // 添加商品
        player->AddElement( &kfsetting->_goods_data, buycount, __STRING__( goods ), goodsid, __FUNC_LINE__ );
        return KFMsg::StoreBuyOK;
    }

    std::tuple<uint32, uint32> KFGoodsModule::RandGoods( KFEntity* player, uint32 groupid, UInt32Set& excludelist )
    {
        auto kfgoodsweightdata = KFGoodsConfig::Instance()->RandGroupGoods( groupid, excludelist );
        if ( kfgoodsweightdata == nullptr )
        {
            return std::make_tuple( 0u, 0u );
        }

        // 是否重置刷新时间和数量
        auto kfsetting = KFGoodsConfig::Instance()->FindSetting( kfgoodsweightdata->_id );
        if ( kfsetting != nullptr )
        {
            if ( kfsetting->_is_refresh_reset_time )
            {
                _kf_reset->ResetTime( player, kfsetting->_limit_buy_time_id );
            }

            if ( kfsetting->_is_refresh_reset_count )
            {
                auto kfgoodsrecord = player->Find( __STRING__( goods ) );
                auto kfgoods = kfgoodsrecord->Find( kfsetting->_id );
                if ( kfgoods != nullptr )
                {
                    player->UpdateData( kfgoods, __STRING__( value ), KFEnum::Set, 0u );
                }
            }
        }

        return std::make_tuple( kfgoodsweightdata->_id, kfgoodsweightdata->_refresh_count );
    }
}
