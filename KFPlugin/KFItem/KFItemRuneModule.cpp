#include "KFItemRuneModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemRuneModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_ENTER_PLAYER__( &KFItemRuneModule::OnEnterRuneModule );

        __REGISTER_ADD_DATA_1__( __STRING__( item ), &KFItemRuneModule::OnAddRuneItemCallBack );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_RUNE_PUT_ON_REQ, &KFItemRuneModule::HandleRunePutOnReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_RUNE_TAKE_OFF_REQ, &KFItemRuneModule::HandleRuneTakeOffReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_RUNE_EXCHANGE_REQ, &KFItemRuneModule::HandleRuneExchangeReq );
    }

    void KFItemRuneModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();

        __UN_ADD_DATA_1__( __STRING__( item ) );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_RUNE_PUT_ON_REQ );
        __UN_MESSAGE__( KFMsg::MSG_RUNE_TAKE_OFF_REQ );
        __UN_MESSAGE__( KFMsg::MSG_RUNE_EXCHANGE_REQ );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFItemRuneModule::OnEnterRuneModule )
    {
        auto runecompound = player->Get<uint32>( __STRING__( runecompound ) );
        if ( runecompound == 0u )
        {
            return;
        }

        auto iter = _add_rune_data.find( player->GetKeyID() );
        if ( iter == _add_rune_data.end() )
        {
            auto kfitemrecord = GetRuneItemRecord( player );
            if ( kfitemrecord == nullptr )
            {
                return;
            }

            for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
            {
                auto itemid = kfitem->Get( __STRING__( id ) );
                auto compoundid = KFItemConfig::Instance()->GetRuneCompoundId( itemid );
                if ( compoundid == _invalid_int )
                {
                    continue;
                }

                auto itemuuid = kfitem->Get( __STRING__( uuid ) );
                _add_rune_data[player->GetKeyID()].insert( itemuuid );
            }
        }

        OnDelayTimerCompoundRune( player, false );
    }

    __KF_MESSAGE_FUNCTION__( KFItemRuneModule::HandleRunePutOnReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRunePutOnReq );

        auto kfitemrecord = GetRuneItemRecord( player );
        if ( kfitemrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto kfitem = kfitemrecord->Find( kfmsg.itemuuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto kfrunearray = player->Find( __STRING__( runeslot ) );

        auto slotindex = kfmsg.index();
        if ( slotindex == 0u )
        {
            // 自动找到空的符石槽
            slotindex = kfrunearray->GetEmpty();
        }

        auto kfrune = kfrunearray->Find( slotindex );
        if ( kfrune == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RuneSlotIndexError );
        }

        auto runeid = kfrune->Get<uint32>();
        if ( runeid != _invalid_int )
        {
            // 如果该位置有符石，先脱下符石
            auto result = TakeOffRune( player, slotindex );
            if ( result != KFMsg::RuneTakeOffSuc )
            {
                return _kf_display->SendToClient( player, KFMsg::RuneTakeOffFail );
            }
        }

        auto result = PutOnRune( player, kfmsg.itemuuid(), slotindex, true );

        return _kf_display->SendToClient( player, result );
    }

    __KF_MESSAGE_FUNCTION__( KFItemRuneModule::HandleRuneTakeOffReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRuneTakeOffReq );

        auto result = TakeOffRune( player, kfmsg.index() );

        return _kf_display->SendToClient( player, result );
    }

    __KF_MESSAGE_FUNCTION__( KFItemRuneModule::HandleRuneExchangeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRuneExchangeReq );

        auto kfrunearray = player->Find( __STRING__( runeslot ) );
        if ( kfmsg.oldindex() == kfmsg.newindex() ||
                kfmsg.oldindex() == 0u ||
                kfmsg.newindex() == 0u ||
                kfmsg.oldindex() >= kfrunearray->MaxSize() ||
                kfmsg.newindex() >= kfrunearray->MaxSize() )
        {
            return _kf_display->SendToClient( player, KFMsg::RuneSlotIndexError );
        }

        auto oldvalue = kfrunearray->Get<uint32>( kfmsg.oldindex() );
        auto newvalue = kfrunearray->Get<uint32>( kfmsg.newindex() );

        player->UpdateData( kfrunearray, kfmsg.oldindex(), KFEnum::Set, newvalue );
        player->UpdateData( kfrunearray, kfmsg.newindex(), KFEnum::Set, oldvalue );
    }

    __KF_ADD_DATA_FUNCTION__( KFItemRuneModule::OnAddRuneItemCallBack )
    {
        if ( true )
        {
            return;
        }

        auto itemid = kfdata->Get<uint32>( kfdata->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        if ( kfsetting->_type != KFItemEnum::Rune )
        {
            return;
        }

        player->UpdateData( __STRING__( runecompound ), KFEnum::Set, 1u );

        // 保存添加的道具信息
        auto itemuuid = kfdata->Get( __STRING__( uuid ) );
        auto iter = _add_rune_data.find( player->GetKeyID() );
        if ( iter == _add_rune_data.end() )
        {
            __DELAY_TIMER_1__( player->GetKeyID(), 10, &KFItemRuneModule::OnDelayTimerOperate );
        }

        _add_rune_data[player->GetKeyID()].insert( itemuuid );
    }

    __KF_TIMER_FUNCTION__( KFItemRuneModule::OnDelayTimerOperate )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto iterator = _add_rune_data.find( objectid );
        if ( iterator == _add_rune_data.end() )
        {
            return;
        }

        for ( auto iter : iterator->second )
        {
            // 先穿戴符石
            PutOnRune( player, iter );
        }

        OnDelayTimerCompoundRune( player );
    }

    uint32 KFItemRuneModule::PutOnRune( KFEntity* player, uint64 itemuuid, uint32 index, bool isforce )
    {
        auto kfitemrecord = GetRuneItemRecord( player );
        if ( kfitemrecord == nullptr )
        {
            return KFMsg::ItemBagNameError;
        }

        auto kfitem = kfitemrecord->Find( itemuuid );
        if ( kfitem == nullptr )
        {
            return KFMsg::ItemDataNotExist;
        }

        auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::ItemSettingNotExist;
        }

        if ( !isforce && !kfsetting->_auto_set )
        {
            return KFMsg::RuneIsNotAutoSet;
        }

        auto kfrunearray = player->Find( __STRING__( runeslot ) );
        if ( kfrunearray->IsFull() )
        {
            return KFMsg::RuneSlotIsFull;
        }

        if ( index == 0u )
        {
            index = kfrunearray->GetEmpty();
        }

        if ( index == 0u || index >= kfrunearray->MaxSize() )
        {
            return KFMsg::RuneSlotIndexError;
        }

        if ( kfsetting->_rune_type != 0u )
        {
            for ( auto kfrune = kfrunearray->First(); kfrune != nullptr; kfrune = kfrunearray->Next() )
            {
                auto runeid = kfrune->Get<uint32>();
                if ( runeid == 0u )
                {
                    continue;
                }

                auto kfrunesetting = KFItemConfig::Instance()->FindSetting( runeid );
                if ( kfrunesetting && kfrunesetting->_rune_type == kfsetting->_rune_type )
                {
                    return KFMsg::RuneSameTypeLimit;
                }
            }
        }

        // 从背包中删除符石
        player->RemoveData( kfitemrecord, itemuuid );

        // 装上符石槽
        player->UpdateData( kfrunearray, index, KFEnum::Set, itemid );

        return KFMsg::RunePutOnSuc;
    }

    uint32 KFItemRuneModule::TakeOffRune( KFEntity* player, uint32 index )
    {
        auto kfrunearray = player->Find( __STRING__( runeslot ) );
        auto kfrune = kfrunearray->Find( index );
        if ( kfrune == nullptr )
        {
            return KFMsg::RuneSlotIndexError;
        }

        auto itemid = kfrune->Get<uint32>();
        if ( itemid == _invalid_int )
        {
            return KFMsg::RuneSlotIsEmpty;
        }

        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfitemsetting == nullptr )
        {
            return KFMsg::ItemSettingNotExist;
        }

        // 背包已满
        auto kfitemrecord = GetRuneItemRecord( player );
        if ( kfitemrecord == nullptr )
        {
            return KFMsg::ItemBagNameError;
        }

        if ( _kf_item->IsItemRecordFull( player, kfitemrecord ) )
        {
            return KFMsg::ItemBagIsFull;
        }

        // 脱下符石
        player->UpdateData( kfrunearray, index, KFEnum::Set, 0u );

        // 添加道具
        _kf_item->AddItem( player, kfitemrecord, itemid, 1u, false );

        return KFMsg::RuneTakeOffSuc;
    }

    void KFItemRuneModule::CompoundRune( KFEntity* player, KFCompoundData& compounddata )
    {
        auto runeid = compounddata.runeid;
        auto compoundid = KFItemConfig::Instance()->GetRuneCompoundId( runeid );
        if ( compoundid == _invalid_int )
        {
            if ( compounddata.compoundnum > 0u )
            {
                compounddata.addlist[runeid] = compounddata.compoundnum;
            }
            return;
        }

        // 获得物品id数量，优先消耗符石槽
        auto kfrunearray = player->Find( __STRING__( runeslot ) );
        auto slotindex = kfrunearray->GetIndex( runeid );
        auto slotnum = ( slotindex == _invalid_int ) ? 0u : 1u;

        UInt64Vector baglist;
        auto kfitemrecord = GetRuneItemRecord( player );
        if ( kfitemrecord == nullptr )
        {
            return;
        }

        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto id = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
            if ( runeid != id )
            {
                continue;
            }

            baglist.push_back( kfitem->Get( __STRING__( uuid ) ) );
        }

        // 总符石数
        auto totalnum = slotnum + compounddata.compoundnum + baglist.size();
        static auto _option = _kf_option->FindOption( "runecompoundnum" );
        if ( totalnum < _option->_uint32_value )
        {
            // 合成符石数量不足
            if ( compounddata.compoundnum > 0u )
            {
                compounddata.addlist[runeid] = compounddata.compoundnum;
            }

            return;
        }

        compounddata.compoundid = compoundid;

        // 合成消耗符石数
        auto compoundnum = totalnum / _option->_uint32_value;
        auto consumenum = compoundnum * _option->_uint32_value;

        if ( slotnum > 0u )
        {
            // 更新符石槽符石
            player->UpdateData( kfrunearray, slotindex, KFEnum::Set, compoundid );
        }

        // 添加新的符石数量
        auto addnum = 0u;
        if ( compounddata.compoundnum > consumenum - slotnum )
        {
            addnum = compounddata.compoundnum - ( consumenum - slotnum );
        }

        // 消耗背包数量
        auto bagnum = consumenum - slotnum - ( compounddata.compoundnum - addnum );

        compounddata.runeid = compoundid;
        compounddata.compoundnum = compoundnum - slotnum;

        if ( addnum > 0u )
        {
            compounddata.addlist[runeid] = addnum;
        }

        if ( bagnum > 0u )
        {
            compounddata.removelist.insert( compounddata.removelist.end(), baglist.begin(), baglist.begin() + bagnum );
        }

        CompoundRune( player, compounddata );
    }

    void KFItemRuneModule::OnDelayTimerCompoundRune( KFEntity* player, bool is_send )
    {
        player->UpdateData( __STRING__( runecompound ), KFEnum::Set, 0u );

        auto iterator = _add_rune_data.find( player->GetKeyID() );
        if ( iterator == _add_rune_data.end() )
        {
            return;
        }

        // 获取所有的符石道具列表
        auto kfitemrecord = GetRuneItemRecord( player );
        if ( kfitemrecord == nullptr )
        {
            _add_rune_data.erase( player->GetKeyID() );
            return;
        }

        UInt32Set runeset;
        for ( auto iter : iterator->second )
        {
            auto kfitem = kfitemrecord->Find( iter );
            if ( kfitem == nullptr )
            {
                continue;
            }

            auto runeid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            runeset.insert( runeid );
        }

        // 清空玩家的数据
        _add_rune_data.erase( player->GetKeyID() );

        // 符石合成表
        UInt32Map compoundmap;
        for ( auto iter : runeset )
        {
            // 合成符石
            KFCompoundData compounddata;
            compounddata.runeid = iter;

            CompoundRune( player, compounddata );

            // 保存不同类型合成信息
            if ( is_send && compounddata.compoundid != _invalid_int )
            {
                auto kfitemsetting = KFItemConfig::Instance()->FindSetting( compounddata.compoundid );

                auto compounditer = compoundmap.find( kfitemsetting->_rune_type );
                if ( compounditer == compoundmap.end() )
                {
                    compoundmap[kfitemsetting->_rune_type] = compounddata.compoundid;
                }
                else
                {
                    auto olditemid = compounditer->second;
                    auto kfolditemsetting = KFItemConfig::Instance()->FindSetting( compounditer->second );

                    if ( kfitemsetting->_rune_level > kfolditemsetting->_rune_level )
                    {
                        // 保存更高等级的合成符石
                        compounditer->second = compounddata.compoundid;
                    }
                }
            }

            // 删除符石道具
            for ( auto it : compounddata.removelist )
            {
                player->RemoveData( kfitemrecord, it );
            }

            // 添加符石道具
            for ( auto it : compounddata.addlist )
            {
                _kf_item->AddItem( player, kfitemrecord, it.first, it.second, false );
            }
        }

        // 发送合成不同类型等级最高的符石
        if ( is_send && compoundmap.size() > 0u )
        {
            KFMsg::MsgRuneCompoundAck ack;
            for ( auto iter : compoundmap )
            {
                ack.add_compoundlist( iter.second );
            }

            _kf_player->SendToClient( player, KFMsg::MSG_RUNE_COMPOUND_ACK, &ack );
        }
    }

    void KFItemRuneModule::ClearRuneSlotData( KFEntity* player )
    {
        player->CleanData( __STRING__( runeslot ) );
    }

    KFData* KFItemRuneModule::GetRuneItemRecord( KFEntity* player )
    {
        auto kfitemtypesetting = KFItemTypeConfig::Instance()->FindSetting( KFItemEnum::Rune );
        if ( kfitemtypesetting == nullptr )
        {
            __LOG_ERROR__( "item type [{}] is not exist", KFItemEnum::Rune );
            return nullptr;
        }

        auto kfitemrecord = player->Find( kfitemtypesetting->_bag_name );
        if ( kfitemrecord == nullptr )
        {
            __LOG_ERROR__( "bag name [{}] is not exist", kfitemtypesetting->_bag_name );
        }

        return kfitemrecord;
    }

}