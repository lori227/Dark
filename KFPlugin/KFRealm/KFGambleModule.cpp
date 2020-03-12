#include "KFGambleModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFGambleModule::BeforeRun()
    {
        __REGISTER_ENTER_REALM__( &KFGambleModule::OnEnterRealm );
        __REGISTER_DROP_LOGIC__( __STRING__( gamble ), &KFGambleModule::OnDropGambleItemLogic );
        //////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_GAMBLE_ITEM_SHOW_REQ, &KFGambleModule::HandleGambleItemShowReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_GAMBLE_ITEM_START_REQ, &KFGambleModule::HandleGambleItemStartReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_RUNE_EXCHANGE_START_REQ, &KFGambleModule::HandleRuneExchangeStartReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_DROP_SELECT_REQ, &KFGambleModule::HandleDropSelectReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SELECT_ITEM_REQ, &KFGambleModule::HandleSelectItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MULTI_EVENT_REQ, &KFGambleModule::HandleMultiEventReq );
    }

    void KFGambleModule::BeforeShut()
    {
        __UN_ENTER_PLAYER__();
        __UN_DROP_LOGIC__( __STRING__( gamble ) );
        //////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_GAMBLE_ITEM_SHOW_REQ );
        __UN_MESSAGE__( KFMsg::MSG_GAMBLE_ITEM_START_REQ );
        __UN_MESSAGE__( KFMsg::MSG_RUNE_EXCHANGE_START_REQ );
        __UN_MESSAGE__( KFMsg::MSG_DROP_SELECT_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SELECT_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MULTI_EVENT_REQ );
    }

    __KF_DROP_LOGIC_FUNCTION__( KFGambleModule::OnDropGambleItemLogic )
    {
        // do nothing
    }

    __KF_ENTER_REALM_FUNCTION__( KFGambleModule::OnEnterRealm )
    {
        if ( entertype == KFMsg::EnterLogin )
        {
            if ( kfrealmdata->_data.selectcount() > 0u )
            {
                ShowDropSelectMessage( player, kfrealmdata, 2000u );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGambleModule::HandleGambleItemShowReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgGambleItemShowReq );

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInStatus );
        }

        auto kfsetting = KFGambleConfig::Instance()->FindSetting( kfmsg.gambleid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::GambleIdError );
        }

        auto kfgambledata = kfrealmdata->_gamble_list.Create( kfmsg.gambleid() );
        auto costcount = CalcGambleCostItemCount( kfgambledata, kfsetting );
        if ( costcount == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::GambleParamError );
        }

        KFMsg::MsgGambleItemShowAck ack;
        ack.set_gambleid( kfmsg.gambleid() );
        ack.set_gamblecount( kfgambledata->_gamble_count );
        ack.set_costitemid( kfsetting->_cost_item_id );
        ack.set_costitemcount( costcount );
        _kf_player->SendToClient( player, KFMsg::MSG_GAMBLE_ITEM_SHOW_ACK, &ack );
    }

    uint32 KFGambleModule::CalcGambleCostItemCount( KFGambleData* kfgambledata, const KFGambleSetting* kfsetting )
    {
        if ( kfsetting->_params.size() < 2u )
        {
            return 0u;
        }

        auto costcount = kfsetting->_init_count;
        if ( kfgambledata->_gamble_count > 0u )
        {
            costcount = kfsetting->_params[ 0 ] * kfgambledata->_cost_item_count + kfsetting->_params[ 1 ];
            if ( costcount > kfsetting->_max_count )
            {
                costcount = kfsetting->_max_count;
            }
        }

        return costcount;
    }

    __KF_MESSAGE_FUNCTION__( KFGambleModule::HandleGambleItemStartReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgGambleItemShowReq );

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInStatus );
        }

        auto kfsetting = KFGambleConfig::Instance()->FindSetting( kfmsg.gambleid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::GambleIdError );
        }

        auto kfgambledata = kfrealmdata->_gamble_list.Create( kfmsg.gambleid() );
        auto costcount = CalcGambleCostItemCount( kfgambledata, kfsetting );
        if ( costcount == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::GambleParamError );
        }

        // 判断资源是否足够
        auto& dataname = player->RemoveElement( &kfsetting->_cost_item, costcount, __STRING__( gamble ), kfsetting->_id, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        // 保留次数和消耗数量
        kfgambledata->_gamble_count++;
        kfgambledata->_cost_item_count = costcount;

        // 执行掉落
        auto dropid = kfsetting->_normal_drop_id;
        if ( kfrealmdata->IsInnerWorld() )
        {
            dropid = kfsetting->_inner_drop_id;
        }

        auto& droplist = _kf_drop->Drop( player, dropid, kfsetting->_show_count, __STRING__( gamble ), kfsetting->_id, __FUNC_LINE__ );

        KFMsg::MsgGambleItemFinishAck ack;
        ack.set_gambleid( kfmsg.gambleid() );
        ack.set_gamblecount( kfgambledata->_gamble_count );
        ack.set_costitemid( kfsetting->_cost_item_id );
        ack.set_costitemcount( costcount );

        if ( !droplist.empty() )
        {
            for ( auto dropdata : droplist )
            {
                ack.add_showitemid( dropdata->_data_key );
            }

            auto index = KFGlobal::Instance()->RandRatio( droplist.size() );
            ack.set_showindex( index + 1 );

            // 添加道具属性
            auto iter = droplist.begin();
            std::advance( iter, index );
            player->AddElement( &( *iter )->_elements, _default_multiple, __STRING__( gamble ), kfsetting->_id, __FUNC_LINE__ );
        }

        _kf_player->SendToClient( player, KFMsg::MSG_GAMBLE_ITEM_FINISH_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGambleModule::HandleRuneExchangeStartReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRuneExchangeStartReq );

        auto kfexchangesetting = KFExchangeConfig::Instance()->FindSetting( kfmsg.exchangeid() );
        if ( kfexchangesetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ExchangeIdError );
        }

        auto kfitemrecord = player->Find( kfmsg.itemname() );
        if ( kfitemrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto kfitem = kfitemrecord->Find( kfmsg.itemuuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( kfitem->Get<uint32>( __STRING__( id ) ) );
        if ( kfitemsetting == nullptr || kfitemsetting->_type != KFItemEnum::Rune )
        {
            return _kf_display->SendToClient( player, KFMsg::ExchangeNotRune );
        }

        auto kfexchangedata = kfexchangesetting->_exchange_list.Find( kfitemsetting->_quality );
        if ( kfexchangedata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ExchangeLevelError );
        }

        // 扣除符石
        player->RemoveData( kfitemrecord, kfmsg.itemuuid() );

        // 开始掉落新的符石
        auto dropid = kfexchangedata->_normal_drop_id;
        if ( _kf_realm->IsInnerWorld( player ) )
        {
            dropid = kfexchangedata->_inner_drop_id;
        }
        _kf_drop->Drop( player, dropid, __STRING__( exchange ), kfexchangesetting->_id, __FUNC_LINE__ );

        KFMsg::MsgRuneExchangeFinishAck ack;
        ack.set_exchangeid( kfexchangesetting->_id );
        _kf_player->SendToClient( player, KFMsg::MSG_RUNE_EXCHANGE_FINISH_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGambleModule::HandleDropSelectReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDropSelectReq );

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInStatus );
        }

        auto kfsetting = KFSelectConfig::Instance()->FindSetting( kfmsg.selectid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::SelectIdError );
        }

        // 清空原来的道具
        player->CleanData( __STRING__( select ), false );

        // 设置状态
        auto status = player->Get( __STRING__( basic ), __STRING__( status ) );
        player->Set<uint32>( __STRING__( basic ), __STRING__( status ), KFMsg::DropSelectStatus );

        // 开始掉落
        auto dropid = kfsetting->_normal_drop_id;
        if ( kfrealmdata->_data.innerworld() )
        {
            dropid = kfsetting->_inner_drop_id;
        }
        _kf_drop->Drop( player, dropid, _invalid_string, 0u, __FUNC_LINE__ );

        // 还原状态
        kfrealmdata->_data.set_selectid( kfmsg.selectid() );
        kfrealmdata->_data.set_selectcount( kfsetting->_count );
        player->Set( __STRING__( basic ), __STRING__( status ), status );

        // 通知客户端显示
        ShowDropSelectMessage( player, kfrealmdata, 300u );
    }
    void KFGambleModule::ShowDropSelectMessage( KFEntity* player, KFRealmData* kfrealmdata, uint32 delaytime )
    {
        KFMsg::MsgDropSelectAck ack;
        ack.set_selectid( kfrealmdata->_data.selectid() );
        ack.set_selectcount( kfrealmdata->_data.selectcount() );
        _kf_player->SendToClient( player, KFMsg::MSG_DROP_SELECT_ACK, &ack, delaytime );
    }

    __KF_MESSAGE_FUNCTION__( KFGambleModule::HandleSelectItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSelectItemReq );

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInStatus );
        }

        if ( ( uint32 )kfmsg.itemuuid_size() > kfrealmdata->_data.selectcount() )
        {
            return _kf_display->SendToClient( player, KFMsg::SelectCountError );
        }

        auto kfselectrecord = player->Find( __STRING__( select ) );
        for ( auto i = 0; i < kfmsg.itemuuid_size(); ++i )
        {
            auto kfitem = kfselectrecord->Find( kfmsg.itemuuid( i ) );
            if ( kfitem == nullptr )
            {
                _kf_display->SendToClient( player, KFMsg::SelectUUIDError );
                continue;
            }

            auto kfitemrecord = _kf_item->FindItemRecord( player, kfitem );
            if ( kfitemrecord == nullptr )
            {
                _kf_display->SendToClient( player, KFMsg::SelectRecordError );
                continue;
            }

            if ( _kf_item->IsItemRecordFull( player, kfitemrecord ) )
            {
                kfitemrecord = player->Find( __STRING__( extend ) );
            }

            // 先移除
            player->MoveData( kfselectrecord, kfitem->GetKeyID() );

            // 添加
            player->AddData( kfitemrecord, kfitem );

            // 显示逻辑
            if ( kfitemrecord->_data_setting->_name != __STRING__( extend ) )
            {
                player->AddDataToShow( __STRING__( select ), kfitem );
            }
        }

        // 清空数据
        kfrealmdata->_data.set_selectid( 0u );
        kfrealmdata->_data.set_selectcount( 0u );
        player->CleanData( kfselectrecord, false );

        KFMsg::MsgSelectItemAck ack;
        _kf_player->SendToClient( player, KFMsg::MSG_SELECT_ITEM_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGambleModule::HandleMultiEventReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMultiEventReq );

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RealmNotInStatus );
        }

        auto kfsetting = KFMultiEventConfig::Instance()->FindSetting( kfmsg.multieventid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MultiEventIdError );
        }

        auto kfeventdata = kfsetting->_multi_event_data_list.Find( kfmsg.index() );
        if ( kfeventdata == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MultiEventIndexError );
        }

        auto& dataname = player->RemoveElement( &kfeventdata->_cost_elements, _default_multiple, __STRING__( realm ), kfrealmdata->_data.id(), __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        KFMsg::MsgMultiEventAck ack;
        ack.set_multieventid( kfmsg.multieventid() );
        ack.set_eventid( kfeventdata->_event_id );
        _kf_player->SendToClient( player, KFMsg::MSG_MULTI_EVENT_ACK, &ack );
    }
}