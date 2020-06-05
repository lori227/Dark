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

    uint32 KFGoodsModule::BuyGoods( KFEntity* player, uint32 goodsid, uint32 index, uint32 buycount )
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
        if ( buycount == _invalid_int )
        {
            return KFMsg::StoreBuyCountError;
        }

        // 判断如果是限购商品
        if ( kfsetting->IsLimitBuy() )
        {
            auto hasbuycount = GetHasBuyCount( player, goodsid, index );
            if ( hasbuycount + buycount > kfsetting->_limit_buy_count )
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
            auto kfgoodsrecord = player->Find( __STRING__( goods ) );
            auto kfgoods = kfgoodsrecord->Find( goodsid );
            if ( kfgoods != nullptr )
            {
                auto kfgoodsdatarecord = kfgoods->Find( __STRING__( goodsdata ) );
                auto kfgoodsdata = kfgoodsdatarecord->Find( index );
                if ( kfgoodsdata != nullptr )
                {
                    player->UpdateData( kfgoodsdata, __STRING__( count ), KFEnum::Add, buycount );
                }
                else
                {
                    kfgoodsdata = player->CreateData( kfgoodsdatarecord );
                    kfgoodsdata->Set( __STRING__( count ), buycount );
                    player->AddData( kfgoodsdatarecord, index, kfgoodsdata );
                }
            }
            else
            {
                kfgoods = player->CreateData( kfgoodsrecord );
                player->AddData( kfgoodsrecord, goodsid, kfgoods );
                kfgoods = kfgoodsrecord->Find( goodsid );

                auto kfgoodsdatarecord = kfgoods->Find( __STRING__( goodsdata ) );
                auto kfgoodsdata = player->CreateData( kfgoodsdatarecord );
                kfgoodsdata->Set( __STRING__( count ), buycount );
                player->AddData( kfgoodsdatarecord, index, kfgoodsdata );
            }
        }

        // 添加商品
        player->AddElement( &kfsetting->_goods_data, buycount, __STRING__( goods ), goodsid, __FUNC_LINE__ );
        return KFMsg::StoreBuyOK;
    }

    uint32 KFGoodsModule::GetHasBuyCount( KFEntity* player, uint32 goodsid, uint32 index )
    {
        auto kfgoods = player->Find( __STRING__( goods ), goodsid );
        if ( kfgoods == nullptr )
        {
            return 0u;
        }

        auto kfgoodsdata = kfgoods->Find( __STRING__( goodsdata ), index );
        if ( kfgoodsdata == nullptr )
        {
            return 0u;
        }

        return kfgoodsdata->Get<uint32>( __STRING__( count ) );
    }

    uint32 KFGoodsModule::RandGoods( KFEntity* player, uint32 groupid, UInt32Set& excludelist )
    {
        auto kfgoodsweightdata = KFGoodsConfig::Instance()->RandGroupGoods( groupid, excludelist );
        if ( kfgoodsweightdata == nullptr )
        {
            return 0u;
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
                    player->RemoveData( kfgoodsrecord, kfsetting->_id );
                }
            }
        }

        return kfgoodsweightdata->_id;
    }
}
