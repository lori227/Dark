#include "KFItemMoveModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemMoveModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_LOGIC__( __STRING__( item ), &KFItemMoveModule::OnAddItemMoveLogic );
        __REGISTER_REMOVE_LOGIC__( __STRING__( item ), &KFItemMoveModule::OnRemoveItemMoveLogic );
        __REGISTER_UPDATE_DATA_2__( __STRING__( item ), __STRING__( index ), &KFItemMoveModule::OnUpdateItemMoveCallBack );

        __REGISTER_AFTER_ENTER_PLAYER__( &KFItemMoveModule::OnEnterItemMoveModule );
        __REGISTER_LEAVE_PLAYER__( &KFItemMoveModule::OnLeaveItemMoveModule );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_EXECUTE__( __STRING__( item ), &KFItemMoveModule::OnExecuteItemMaxCount );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_SPLIT_ITEM_REQ, &KFItemMoveModule::HandleSplitItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MERGE_ITEM_REQ, &KFItemMoveModule::HandleMergeItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MOVE_ITEM_REQ, &KFItemMoveModule::HandleMoveItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MOVE_ALL_ITEM_REQ, &KFItemMoveModule::HandleMoveAllItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_ITEM_REQ, &KFItemMoveModule::HandleExchangeItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CLEAN_ITEM_REQ, &KFItemMoveModule::HandleCleanItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SORT_ITEM_REQ, &KFItemMoveModule::HandleSortItemReq );
    }

    void KFItemMoveModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_ADD_LOGIC__( __STRING__( item ) );
        __UN_REMOVE_LOGIC__( __STRING__( item ) );
        __UN_UPDATE_DATA_2__( __STRING__( item ), __STRING__( index ) );

        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_EXECUTE__( __STRING__( item ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SPLIT_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MERGE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MOVE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MOVE_ALL_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXCHANGE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_CLEAN_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SORT_ITEM_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_EXECUTE_FUNCTION__( KFItemMoveModule::OnExecuteItemMaxCount )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            return false;
        }

        auto itemname = executedata->_param_list._params[ 0 ]->_str_value;
        auto addcount = executedata->_param_list._params[ 1 ]->_int_value;
        auto kfitemrecord = player->Find( itemname );
        if ( kfitemrecord == nullptr )
        {
            return false;
        }

        // 添加科技效果
        player->UpdateData( __STRING__( effect ), itemname, KFEnum::Add, addcount );

        // 添加索引
        AddItemMaxIndex( player, kfitemrecord, addcount );
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_DATA_FUNCTION__( KFItemMoveModule::OnAddItemMoveLogic )
    {
        ItemIndexKey indexkey( player->GetKeyID(), kfparent->_data_setting->_name );
        auto kfindex = _player_item_index.Find( indexkey );
        if ( kfindex == nullptr )
        {
            return;
        }

        // 判断已经在索引中
        auto index = kfdata->Get<uint32>( __STRING__( index ) );
        if ( index == 0u )
        {
            // 索引为0, 直接找一个空格子
            index = kfindex->FindEmpty( key );
            kfdata->Set( __STRING__( index ), index );
        }
        else
        {
            auto uuid = kfindex->GetUUID( index );
            if ( uuid == 0u )
            {
                // 获得索引没设置uuid
                kfindex->RemoveEmpty( index, key );
            }
            else if ( uuid != key )
            {
                // 索引和uuid不匹配
                index = kfindex->FindEmpty( key );
                kfdata->Set( __STRING__( index ), index );
            }
        }
    }

    __KF_REMOVE_DATA_FUNCTION__( KFItemMoveModule::OnRemoveItemMoveLogic )
    {
        if ( key == 0u )
        {
            return;
        }

        ItemIndexKey indexkey( player->GetKeyID(), kfparent->_data_setting->_name );
        auto kfindex = _player_item_index.Find( indexkey );
        if ( kfindex == nullptr )
        {
            return;
        }

        auto index = kfdata->Get<uint32>( __STRING__( index ) );
        auto uuid = kfindex->GetUUID( index );
        if ( uuid == key )
        {
            // 索引和uuid匹配
            kfindex->AddEmpty( index );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemMoveModule::OnUpdateItemMoveCallBack )
    {
        ItemIndexKey indexkey( player->GetKeyID(), kfdata->GetParent()->_data_setting->_name );
        auto kfindex = _player_item_index.Find( indexkey );
        if ( kfindex == nullptr )
        {
            return;
        }

        if ( oldvalue != 0u )
        {
            auto uuid = kfindex->GetUUID( oldvalue );
            if ( uuid == key )
            {
                kfindex->AddEmpty( oldvalue );
            }
        }

        kfindex->RemoveEmpty( newvalue, key );
    }

    __KF_AFTER_ENTER_PLAYER_FUNCTION__( KFItemMoveModule::OnEnterItemMoveModule )
    {
        auto& itemdatalist = _kf_item->GetItemRecordList();
        for ( auto& dataname : itemdatalist )
        {
            auto kfitemrecord = player->Find( dataname );
            InitItemEmptyIndexData( player, kfitemrecord );
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFItemMoveModule::OnLeaveItemMoveModule )
    {
        auto& itemdatalist = _kf_item->GetItemRecordList();
        for ( auto& dataname : itemdatalist )
        {
            UnInitItemEmptyIndexData( player, dataname );
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFItemMoveModule::FindIndexItem( KFEntity* player, KFData* kfitemrecord, uint32 index, const KFItemSetting* kfsetting )
    {
        //// 索引为空, 找到可以叠加的道具
        //if ( index == 0u )
        //{
        //    // 如果是可堆叠的
        //    if ( kfsetting->IsOverlay() )
        //    {
        //        auto maxtargetoverlaycount = kfsetting->GetOverlayCount( kfitemrecord->_data_setting->_name );
        //        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        //        {
        //            auto itemid = kfitem->Get( __STRING__( id ) );
        //            if ( itemid != kfsetting->_id )
        //            {
        //                continue;
        //            }

        //            auto count = kfitem->Get<uint32>( __STRING__( count ) );
        //            if ( count < maxtargetoverlaycount )
        //            {
        //                return kfitem;
        //            }
        //        }
        //    }

        //    return nullptr;
        //}

        // 指定索引
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        if ( kfindex == nullptr )
        {
            return nullptr;
        }

        auto uuid = kfindex->GetUUID( index );
        return kfitemrecord->Find( uuid );
    }

    uint32 KFItemMoveModule::SplitItem( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, uint32 splitcount, KFData* kftargetrecord, uint32 splitindex )
    {
        // 扣除源物品数量
        MoveItemCount( player, kfsourceitem, KFEnum::Dec, splitcount );

        // 添加新的道具
        auto kftargetitem = player->CreateData( kfsourceitem );
        kftargetitem->CopyFrom( kfsourceitem );
        auto uuid = KFGlobal::Instance()->STMakeUUID( __STRING__( item ) );
        kftargetitem->SetKeyID( uuid );
        kftargetitem->Set( __STRING__( count ), splitcount );

        // 如果没指定索引, 查找一个索引
        if ( splitindex == 0u )
        {
            splitindex = GetItemEmptyIndex( player, kftargetrecord, uuid );
        }
        kftargetitem->Set( __STRING__( index ), splitindex );
        player->AddData( kftargetrecord, uuid, kftargetitem, false );

        // 添加显示
        MoveItemDataToShow( player, kfsetting, kfsourcerecord, kftargetrecord, splitcount );
        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::MoveItem( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord, uint32 targetindex )
    {
        // 如果背包相同
        if ( kfsourcerecord == kftargetrecord )
        {
            if ( targetindex == 0u || targetindex == kfsourceitem->Get<uint32>( __STRING__( index ) ) )
            {
                return KFMsg::ItemIndexError;
            }

            // 源背包索引
            player->UpdateData( kfsourceitem, __STRING__( index ), KFEnum::Set, targetindex );
        }
        else
        {
            // 源背包索引
            AddItemEmptyIndex( player, kfsourcerecord, kfsourceitem->Get<uint32>( __STRING__( index ) ) );

            kfsourceitem->Set( __STRING__( index ), targetindex );
            kfsourceitem = player->MoveData( kfsourcerecord, kfsourceitem->GetKeyID(), kftargetrecord );
            if ( kfsourceitem == nullptr )
            {
                return KFMsg::ItemMoveFailed;
            }
        }

        // 添加显示
        MoveItemDataToShow( player, kfsetting, kfsourcerecord, kftargetrecord, kfsourceitem );
        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::ExchangeItem( KFEntity* player,
                                           KFData* kfsourcerecord, KFData* kfsourceitem, const KFItemSetting* kfsourcesetting,
                                           KFData* kftargetrecord, KFData* kftargetitem, const KFItemSetting* kftargetsetting )
    {
        if ( kfsourceitem == kftargetitem )
        {
            return KFMsg::ItemIndexError;
        }

        auto sourceindex = kfsourceitem->Get<uint32>( __STRING__( index ) );
        auto targetindex = kftargetitem->Get<uint32>( __STRING__( index ) );
        if ( kfsourcerecord == kftargetrecord )
        {
            // 背包相同, 直接更新索引
            player->UpdateData( kfsourceitem, __STRING__( index ), KFEnum::Set, targetindex );
            player->UpdateData( kftargetitem, __STRING__( index ), KFEnum::Set, sourceindex );
            return KFMsg::Ok;
        }

        // 判断是否能移动
        if ( !CheckItemCanMove( kfsourcesetting, kfsourceitem->_data_setting->_name, kftargetitem->_data_setting->_name ) )
        {
            return KFMsg::ItemCanNotStore;
        }

        // 找到可以移动到的背包
        auto kffindrecord = FindItemMoveRecord( player, kftargetsetting, kftargetrecord->_data_setting->_name );
        if ( kffindrecord == nullptr )
        {
            return KFMsg::ItemCanNotStore;
        }

        // 判断源道具数量
        auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        auto maxsourceoverlaycount = kfsourcesetting->GetOverlayCount( kftargetrecord->_data_setting->_name );

        // 判断目标道具数量
        auto targetitemcount = kftargetitem->Get<uint32>( __STRING__( count ) );
        auto maxtargetoverlaycount = kftargetsetting->GetOverlayCount( kffindrecord->_data_setting->_name );

        if ( sourceitemcount > maxsourceoverlaycount && targetitemcount > maxtargetoverlaycount )
        {
            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->IsItemRecordFull( player, kffindrecord ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->IsItemRecordFull( player, kftargetrecord ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 源道具拆分
            SplitItem( player, kfsourcesetting, kfsourcerecord, kfsourceitem, maxsourceoverlaycount, kftargetrecord, 0u );

            // 目标道具拆分
            SplitItem( player, kftargetsetting, kftargetrecord, kftargetrecord, maxtargetoverlaycount, kffindrecord, 0u );
        }
        else if ( sourceitemcount > maxsourceoverlaycount )
        {
            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->IsItemRecordFull( player, kffindrecord ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 移动目标道具
            MoveItem( player, kftargetsetting, kftargetrecord, kftargetitem, kffindrecord, 0u );

            // 拆分源道具
            SplitItem( player, kfsourcesetting, kfsourcerecord, kfsourceitem, maxsourceoverlaycount, kftargetrecord, targetindex );
        }
        else if ( targetitemcount > maxtargetoverlaycount )
        {
            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->IsItemRecordFull( player, kftargetrecord ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 移动源道具
            MoveItem( player, kfsourcesetting, kfsourcerecord, kfsourceitem, kftargetrecord, 0u );

            // 拆分目标道具
            SplitItem( player, kftargetsetting, kftargetrecord, kftargetitem, maxtargetoverlaycount, kffindrecord, sourceindex );
        }
        else
        {
            if ( kffindrecord == kfsourcerecord )
            {
                AddItemEmptyIndex( player, kfsourcerecord, sourceindex );
                AddItemEmptyIndex( player, kftargetrecord, targetindex );

                kfsourceitem->Set( __STRING__( index ), targetindex );
                kftargetitem->Set( __STRING__( index ), sourceindex );

                // 交换
                player->MoveData( kfsourcerecord, kfsourceitem->GetKeyID(), kftargetrecord );
                player->MoveData( kftargetrecord, kftargetitem->GetKeyID(), kfsourcerecord );
            }
            else
            {
                if ( _kf_item->IsItemRecordFull( player, kffindrecord ) )
                {
                    return KFMsg::ItemBagIsFull;
                }

                MoveItem( player, kftargetsetting, kftargetrecord, kftargetitem, kffindrecord, 0u );
                MoveItem( player, kfsourcesetting, kfsourcerecord, kfsourceitem, kftargetrecord, targetindex );
            }
        }

        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::MergeItem( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, uint32 mergecount, KFData* kftargetrecord, KFData* kftargetitem )
    {
        if ( kfsourceitem == kftargetitem )
        {
            return KFMsg::ItemIndexError;
        }

        auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        if ( sourceitemcount < mergecount )
        {
            mergecount = sourceitemcount;
        }

        // 已经达到最大
        auto targetitemcount = kftargetitem->Get<uint32>( __STRING__( count ) );
        auto maxtargetoverlaycount = kfsetting->GetOverlayCount( kftargetrecord->_data_setting->_name );
        auto canaddcount = CalcItemAddCount( mergecount, targetitemcount, maxtargetoverlaycount );
        if ( canaddcount == 0u )
        {
            return KFMsg::ItemOverlayCountMax;
        }

        // 移动数量
        MoveItemCount( player, kfsourceitem, KFEnum::Dec, canaddcount );
        MoveItemCount( player, kftargetitem, KFEnum::Add, canaddcount );

        // 添加显示
        MoveItemDataToShow( player, kfsetting, kfsourcerecord, kftargetrecord, canaddcount );
        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::CalcItemAddCount( uint32 sourcecount, uint32 targetcount, uint32 maxcount )
    {
        if ( targetcount >= maxcount )
        {
            return 0u;
        }

        auto canaddcount = maxcount - targetcount;
        return sourcecount <= canaddcount ? sourcecount : canaddcount;
    }

    void KFItemMoveModule::MoveItemCount( KFEntity* player, KFData* kfitem, uint32 operate, uint32 count )
    {
        if ( operate == KFEnum::Dec )
        {
            auto oldcount = kfitem->Get<uint32>( __STRING__( count ) );
            if ( count >= oldcount )
            {
                player->RemoveData( kfitem->GetParent(), kfitem->GetKeyID(), false );
                return;
            }
        }

        player->MoveData( kfitem, __STRING__( count ), operate, count );
    }

    bool KFItemMoveModule::CheckItemCanMove( const KFItemSetting* kfsetting, const std::string& sourcename, const std::string& targetname )
    {
        if ( sourcename == targetname )
        {
            return true;
        }

        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfsetting->_type );
        if ( kftypesetting == nullptr )
        {
            return false;
        }

        // 额外的背包, 并且是探索背包
        if ( sourcename == kftypesetting->_extend_name && targetname == kftypesetting->_bag_name )
        {
            return true;
        }

        return kftypesetting->_move_name_list.find( targetname ) != kftypesetting->_move_name_list.end();
    }

    KFData* KFItemMoveModule::FindItemMoveRecord( KFEntity* player, const KFItemSetting* kfsetting, const std::string& excludename )
    {
        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfsetting->_type );
        if ( kftypesetting == nullptr )
        {
            return nullptr;
        }

        for ( auto& name : kftypesetting->_move_name_list )
        {
            if ( name == excludename || name == kftypesetting->_extend_name )
            {
                continue;
            }

            auto kfitemrecord = player->Find( name );
            if ( kfitemrecord != nullptr )
            {
                return kfitemrecord;
            }
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleMoveItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMoveItemReq );

        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        auto kftargetrecord = player->Find( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto maxindex = GetItemMaxIndex( player, kftargetrecord );
        if ( kfmsg.targetindex() > maxindex )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemIndexError );
        }

        auto kfsourceitem = kfsourcerecord->Find( kfmsg.sourceuuid() );
        if ( kfsourceitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        // 判断是否限制
        auto sourceitemid = kfsourceitem->Get<uint32>( kfsourceitem->_data_setting->_config_key_name );
        auto kfsourcesetting = KFItemConfig::Instance()->FindSetting( sourceitemid );
        if ( kfsourcesetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
        }

        // 判断能否移动
        if ( !CheckItemCanMove( kfsourcesetting, kfmsg.sourcename(), kfmsg.targetname() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotStore );
        }

        uint32 result = KFMsg::Failed;
        auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        auto kftargetitem = FindIndexItem( player, kftargetrecord, kfmsg.targetindex(), kfsourcesetting );
        if ( kftargetitem == nullptr )
        {
            if ( !_kf_item->IsItemRecordFull( player, kftargetrecord ) )
            {
                // 移动到空格子上
                // 判断堆叠数量, 如果目标小于源堆叠, 并且源数量大于目标堆叠数量, 拆分物品
                auto maxtargetoverlaycount = kfsourcesetting->GetOverlayCount( kftargetrecord->_data_setting->_name );
                if ( sourceitemcount > maxtargetoverlaycount )
                {
                    // 拆分物品
                    result = SplitItem( player, kfsourcesetting, kfsourcerecord, kfsourceitem, maxtargetoverlaycount, kftargetrecord, kfmsg.targetindex() );
                }
                else
                {
                    // 移动物品
                    result = MoveItem( player, kfsourcesetting, kfsourcerecord, kfsourceitem, kftargetrecord, kfmsg.targetindex() );
                }
            }
        }
        else
        {
            auto targetitemid = kftargetitem->Get<uint32>( kftargetitem->_data_setting->_config_key_name );
            auto kftargetsetting = KFItemConfig::Instance()->FindSetting( targetitemid );
            if ( kftargetsetting != nullptr )
            {
                // 判断道具是否能堆叠
                if ( CheckItemCanMerge( kfsourcesetting, kfsourceitem, kftargetsetting, kftargetitem ) )
                {
                    // 移动源物品的一定数量到目标物品上
                    result = MergeItem( player, kfsourcesetting, kfsourcerecord, kfsourceitem, sourceitemcount, kftargetrecord, kftargetitem );
                }
                else
                {
                    // 直接交换
                    result = ExchangeItem( player, kfsourcerecord, kfsourceitem, kfsourcesetting, kftargetrecord, kftargetitem, kftargetsetting );
                }
            }
            else
            {
                result = KFMsg::ItemSettingNotExist;
            }
        }

        if ( result != KFMsg::Ok )
        {
            player->UpdateData( kfsourceitem, __STRING__( index ), KFEnum::Set, kfsourceitem->Get<uint32>( __STRING__( index ) ) );
        }
    }

    bool KFItemMoveModule::CheckItemCanMerge( const KFItemSetting* kfsourcesetting, KFData* kfsourceitem, const KFItemSetting* kftargetsetting, KFData* kftargetitem )
    {
        // 不同道具
        if ( kfsourcesetting->_id != kftargetsetting->_id || !kfsourcesetting->IsOverlay() )
        {
            return false;
        }

        // 判断目标数量
        auto targetitemcount = kftargetitem->Get<uint32>( __STRING__( count ) );
        auto maxtargetoverlaycount = kfsourcesetting->GetOverlayCount( kftargetitem->_data_setting->_name );
        if ( targetitemcount < maxtargetoverlaycount )
        {
            return true;
        }

        // 目标满了, 如果源数据小于堆叠数量, 需要可以交换
        auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        if ( sourceitemcount > maxtargetoverlaycount )
        {
            return true;
        }

        return false;
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleMoveAllItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMoveAllItemReq );

        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        auto kftargetrecord = player->Find( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        // 遍历出可以移动的道具
        std::list< std::tuple< KFData*, const KFItemSetting* > > movelist;
        for ( auto kfitem = kfsourcerecord->First(); kfitem != nullptr; kfitem = kfsourcerecord->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            auto ok = CheckItemCanMove( kfsetting, kfmsg.sourcename(), kfmsg.targetname() );
            if ( !ok )
            {
                continue;
            }

            movelist.push_back( std::make_tuple( kfitem, kfsetting ) );
        }

        for ( auto& tupledata : movelist )
        {
            KFData* kfsourceitem = nullptr;
            const KFItemSetting* kfsetting = nullptr;
            std::tie( kfsourceitem, kfsetting ) = tupledata;
            MoveItemDataToRecord( player, kfsetting, kfsourcerecord, kfsourceitem, kftargetrecord );
        }
    }

    void KFItemMoveModule::MoveItemDataToRecord( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord )
    {
        // 判断是否能堆叠
        if ( kfsetting->IsOverlay() )
        {
            auto sourcecount = kfsourceitem->Get<uint32>( __STRING__( count ) );
            auto maxoverlaycount = kfsetting->GetOverlayCount( kftargetrecord->_data_setting->_name );

            // 堆叠起来
            for ( auto kfitem = kftargetrecord->First(); kfitem != nullptr; kfitem = kftargetrecord->Next() )
            {
                auto itemid = kfitem->Get<uint32>( kftargetrecord->_data_setting->_config_key_name );
                if ( itemid != kfsetting->_id )
                {
                    continue;
                }

                auto targetcount = kfitem->Get<uint32>( __STRING__( count ) );
                auto movecount = CalcItemAddCount( sourcecount, targetcount, maxoverlaycount );
                if ( movecount == 0u )
                {
                    continue;
                }

                // 移动物品
                MoveItemCount( player, kfsourceitem, KFEnum::Dec, movecount );
                MoveItemCount( player, kfitem, KFEnum::Add, movecount );

                // 添加显示
                MoveItemDataToShow( player, kfsetting, kfsourcerecord, kftargetrecord, movecount );

                sourcecount -= movecount;
                if ( sourcecount == 0u )
                {
                    return;
                }
            }

            // 剩下的道具数量
            while ( sourcecount > maxoverlaycount )
            {
                SplitItem( player, kfsetting, kfsourcerecord, kfsourceitem, maxoverlaycount, kftargetrecord, 0u );
                sourcecount -= maxoverlaycount;
            }
        }

        // 不能堆叠或者剩下没有堆叠完的
        MoveItem( player, kfsetting, kfsourcerecord, kfsourceitem, kftargetrecord, 0u );
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleSplitItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSplitItemReq );

        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        auto kftargetrecord = player->Find( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto maxindex = GetItemMaxIndex( player, kftargetrecord );
        if ( kfmsg.targetindex() > maxindex )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemIndexError );
        }

        auto kfsourceitem = kfsourcerecord->Find( kfmsg.sourceuuid() );
        if ( kfsourceitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto count = kfsourceitem->Get<uint32>( __STRING__( count ) );
        if ( count <= kfmsg.sourcecount() || kfmsg.sourcecount() == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSplitCountError );
        }

        // 判断是否限制
        auto itemid = kfsourceitem->Get<uint32>( kfsourceitem->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
        }

        if ( !CheckItemCanMove( kfsetting, kfmsg.sourcename(), kfmsg.targetname() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotStore );
        }

        // 同背包不能相同的索引
        if ( kfmsg.sourcename() == kfmsg.targetname() )
        {
            auto sourceindex = kfsourceitem->Get<uint32>( __STRING__( index ) );
            if ( sourceindex == kfmsg.targetindex() )
            {
                return _kf_display->SendToClient( player, KFMsg::ItemIndexError );
            }
        }

        uint32 maxoverlaycount = kfsetting->GetOverlayCount( kftargetrecord->_data_setting->_name );
        auto splitcount = __MIN__( maxoverlaycount, kfmsg.sourcecount() );

        auto result = SplitItem( player, kfsetting, kfsourcerecord, kfsourceitem, splitcount, kftargetrecord, kfmsg.targetindex() );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleMergeItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMergeItemReq );

        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        auto kftargetrecord = player->Find( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto kfsourceitem = kfsourcerecord->Find( kfmsg.sourceuuid() );
        auto kftargetitem = kftargetrecord->Find( kfmsg.targetuuid() );
        if ( kfsourceitem == nullptr || kftargetitem == nullptr || kfsourceitem == kftargetitem )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto sourceid = kfsourceitem->Get<uint32>( kfsourceitem->_data_setting->_config_key_name );
        auto targetid = kftargetitem->Get<uint32>( kftargetitem->_data_setting->_config_key_name );
        if ( sourceid != targetid )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemMergeIdError );
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( sourceid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, sourceid );
        }

        auto sourcecount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        auto result = MergeItem( player, kfsetting, kfsourcerecord, kfsourceitem, sourcecount, kftargetrecord, kftargetitem );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleExchangeItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExchangeItemReq );

        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        auto kftargetrecord = player->Find( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto kfsourceitem = kfsourcerecord->Find( kfmsg.sourceuuid() );
        auto kftargetitem = kftargetrecord->Find( kfmsg.targetuuid() );
        if ( kfsourceitem == nullptr || kftargetitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto sourceitemid = kfsourceitem->Get<uint32>( kfsourceitem->_data_setting->_config_key_name );
        auto targetitemid = kftargetitem->Get<uint32>( kftargetitem->_data_setting->_config_key_name );
        auto kfsourcesetting = KFItemConfig::Instance()->FindSetting( sourceitemid );
        auto kftargetsetting = KFItemConfig::Instance()->FindSetting( targetitemid );
        if ( kfsourcesetting == nullptr || kftargetsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
        }

        auto result = ExchangeItem( player, kfsourcerecord, kfsourceitem, kfsourcesetting, kftargetrecord, kftargetitem, kftargetsetting );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleCleanItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCleanItemReq );

        CleanItem( player, kfmsg.sourcename() );
    }

    void KFItemMoveModule::CleanItem( KFEntity* player, const std::string& name, bool isauto )
    {
        auto kfsourcerecord = player->Find( name );
        if ( kfsourcerecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        // 储存列表
        std::list< std::tuple< KFData*, const KFItemSetting*, const KFItemTypeSeting* > > storelist;

        // 使用列表
        std::list<std::tuple< KFData*, const KFItemSetting* >> uselist;

        // 销毁列表
        std::list< uint64 > destorylist;

        for ( auto kfitem = kfsourcerecord->First(); kfitem != nullptr; kfitem = kfsourcerecord->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            auto kfitemsetting = KFItemConfig::Instance()->FindSetting( itemid );
            if ( kfitemsetting == nullptr )
            {
                continue;
            }

            auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
            if ( kftypesetting == nullptr )
            {
                continue;
            }

            if ( isauto )
            {
                if ( kfitemsetting->_type == KFItemEnum::Gift && kfitemsetting->_auto_use != 0u )
                {
                    // 优先自动使用礼包道具
                    uselist.push_back( std::make_tuple( kfitem, kfitemsetting ) );

                    auto itemuuid = kfitem->Get<uint64>( __STRING__( uuid ) );
                    destorylist.push_back( itemuuid );
                }
                else if ( kftypesetting->_is_auto == KFMsg::AutoStore )
                {
                    // 自动存入仓库
                    storelist.push_back( std::make_tuple( kfitem, kfitemsetting, kftypesetting ) );
                }
                else if ( kftypesetting->_is_auto == KFMsg::AutoDestory )
                {
                    // 自动销毁
                    auto itemuuid = kfitem->Get<uint64>( __STRING__( uuid ) );
                    destorylist.push_back( itemuuid );
                }
            }
            else
            {
                // 手动操作的检查物品能否移动
                if ( CheckItemCanMove( kfitemsetting, name, kftypesetting->_store_name ) )
                {
                    storelist.push_back( std::make_tuple( kfitem, kfitemsetting, kftypesetting ) );
                }
            }
        }

        player->SyncDataSequence( KFEnum::Dec, KFEnum::Add, KFEnum::Set );
        for ( auto& tupledata : storelist )
        {
            KFData* kfsourceitem = nullptr;
            const KFItemSetting* kfitemsetting = nullptr;
            const KFItemTypeSeting* kftypesetting = nullptr;
            std::tie( kfsourceitem, kfitemsetting, kftypesetting ) = tupledata;
            auto kftargetrecord = player->Find( kftypesetting->_store_name );
            if ( kftargetrecord != nullptr )
            {
                MoveItemDataToRecord( player, kfitemsetting, kfsourcerecord, kfsourceitem, kftargetrecord );
            }
        }

        for ( auto& tupledata : uselist )
        {
            KFData* kfsourceitem = nullptr;
            const KFItemSetting* kfitemsetting = nullptr;
            std::tie( kfsourceitem, kfitemsetting ) = tupledata;
            auto itemnum = kfsourceitem->Get<uint32>( __STRING__( count ) );
            player->AddElement( &kfitemsetting->_reward, itemnum, __STRING__( useitem ), kfitemsetting->_id, __FUNC_LINE__ );
        }

        for ( auto iter : destorylist )
        {
            player->RemoveData( kfsourcerecord, iter );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleSortItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSortItemReq );

        for ( auto i = 0; i < kfmsg.sourcename_size(); ++i )
        {
            SortItem( player, kfmsg.sourcename( i ) );
        }

        _kf_display->SendToClient( player, KFMsg::ItemSortOk );
    }

    void KFItemMoveModule::SortItem( KFEntity* player, const std::string& name )
    {
        auto kfitemrecord = player->Find( name );
        if ( kfitemrecord == nullptr )
        {
            return;
        }

        // 重置格子数量
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Create( key );

        auto maxitemcount = _kf_item->GetItemRecordMaxCount( player, kfitemrecord );
        kfindex->InitMaxIndex( maxitemcount );

        // sort从小到大, 品质从大到小, id从小到大
        std::map<uint32, std::map<uint32, std::map<const KFItemSetting*, std::set<KFData*>>>> sortlist;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto id = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
            auto kfsetting = KFItemConfig::Instance()->FindSetting( id );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfsetting->_type );
            if ( kftypesetting == nullptr )
            {
                continue;
            }

            sortlist[ kftypesetting->_sort_index ][ kfsetting->_quality ][ kfsetting ].insert( kfitem );
        }

        for ( auto& iter : sortlist )
        {
            auto& qualitylist = iter.second;
            for ( auto miter = qualitylist.rbegin(); miter != qualitylist.rend(); ++miter )
            {
                auto& itemlist = miter->second;
                for ( auto& qiter : itemlist )
                {
                    SortItem( player, kfitemrecord, kfindex, qiter.first, qiter.second );
                }
            }
        }
    }

    std::tuple<KFData*, uint32> FindMaxCountItem( uint32 maxoverlaycount, std::set< KFData* >& itemlist, KFData* kfminitem )
    {
        auto maxcount = 0u;
        KFData* kffind = nullptr;

        for ( auto kfitem : itemlist )
        {
            auto count = kfitem->Get<uint32>( __STRING__( count ) );
            if ( count < maxoverlaycount && count > maxcount && kfitem != kfminitem )
            {
                kffind = kfitem;
                maxcount = count;
            }
        }

        return std::make_tuple( kffind, maxcount );
    }

    std::tuple<KFData*, uint32> FindMinCountItem( uint32 maxoverlaycount, std::set< KFData* >& itemlist )
    {
        auto mincount = __MAX_UINT32__;
        KFData* kffind = nullptr;

        for ( auto kfitem : itemlist )
        {
            auto count = kfitem->Get<uint32>( __STRING__( count ) );
            if ( count < maxoverlaycount && count < mincount )
            {
                kffind = kfitem;
                mincount = count;
            }
        }

        return std::make_tuple( kffind, mincount );
    }

    KFData* PopMaxCountItem( uint32 maxoverlaycount, std::set< KFData* >& itemlist )
    {
        auto maxcount = 0u;
        KFData* kffind = nullptr;

        for ( auto kfitem : itemlist )
        {
            auto count = kfitem->Get<uint32>( __STRING__( count ) );
            if ( count == 0u || count >= maxoverlaycount )
            {
                kffind = kfitem;
                break;
            }

            if ( count > maxcount )
            {
                kffind = kfitem;
                maxcount = count;
            }
        }

        if ( kffind != nullptr )
        {
            itemlist.erase( kffind );
        }

        return kffind;
    }

    void KFItemMoveModule::SortItem( KFEntity* player, KFData* kfitemrecord, KFItemIndex* kfindex, const KFItemSetting* kfsetting, std::set<KFData*>& itemlist )
    {
        auto maxoverlaycount = kfsetting->GetOverlayCount( kfitemrecord->_data_setting->_name );
        if ( kfsetting->IsOverlay() )
        {
            while ( true )
            {
                auto mincount = 0u;
                KFData* minitem = nullptr;
                std::tie( minitem, mincount ) = FindMinCountItem( maxoverlaycount, itemlist );
                if ( minitem == nullptr )
                {
                    break;
                }

                auto maxcount = 0u;
                KFData* maxitem = nullptr;
                std::tie( maxitem, maxcount ) = FindMaxCountItem( maxoverlaycount, itemlist, minitem );
                if ( maxitem == nullptr )
                {
                    break;
                }

                auto canaddcount = maxoverlaycount - maxcount;
                canaddcount = __MIN__( mincount, canaddcount );

                MoveItemCount( player, minitem, KFEnum::Dec, canaddcount );
                MoveItemCount( player, maxitem, KFEnum::Add, canaddcount );

                if ( canaddcount == mincount )
                {
                    itemlist.erase( minitem );
                }
            }
        }

        while ( true )
        {
            auto kfitem = PopMaxCountItem( maxoverlaycount, itemlist );
            if ( kfitem == nullptr )
            {
                break;
            }

            kfitem->Set( __STRING__( index ), 0u );
            auto index = kfindex->FindEmpty( kfitem->GetKeyID() );
            player->UpdateData( kfitem, __STRING__( index ), KFEnum::Set, index );
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFItemMoveModule::InitItemEmptyIndexData( KFEntity* player, KFData* kfitemrecord )
    {
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Create( key );

        auto maxitemcount = _kf_item->GetItemRecordMaxCount( player, kfitemrecord );
        kfindex->InitMaxIndex( maxitemcount );

        std::list< KFData* > invalid;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto index = kfitem->Get<uint32>( __STRING__( index ) );
            if ( index == 0u )
            {
                invalid.push_back( kfitem );
            }
            else
            {
                kfindex->RemoveEmpty( index, kfitem->GetKeyID() );
            }
        }

        // 如果存在没有索引的情况, 纠正数据
        if ( !invalid.empty() )
        {
            for ( auto kfitem : invalid )
            {
                auto index = kfindex->FindEmpty( kfitem->GetKeyID() );
                if ( index == 0u )
                {
                    break;
                }

                kfitem->Set( __STRING__( index ), index );
            }
        }
    }

    void KFItemMoveModule::UnInitItemEmptyIndexData( KFEntity* player, const std::string& name )
    {
        ItemIndexKey key( player->GetKeyID(), name );
        _player_item_index.Remove( key );
    }

    uint32 KFItemMoveModule::GetItemMaxIndex( KFEntity* player, KFData* kfitemrecord )
    {
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        if ( kfindex == nullptr )
        {
            return 0u;
        }

        return kfindex->MaxIndex();
    }

    void KFItemMoveModule::AddItemMaxIndex( KFEntity* player, KFData* kfitemrecord, uint32 count )
    {
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        if ( kfindex == nullptr )
        {
            return;
        }

        kfindex->AddMaxIndex( count );
    }

    uint32 KFItemMoveModule::GetItemEmptyIndex( KFEntity* player, KFData* kfitemrecord, uint64 uuid )
    {
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        if ( kfindex == nullptr )
        {
            return 0u;
        }

        return kfindex->FindEmpty( uuid );
    }

    void KFItemMoveModule::AddItemEmptyIndex( KFEntity* player, KFData* kfitemrecord, uint32 index )
    {
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        if ( kfindex == nullptr )
        {
            return;
        }

        kfindex->AddEmpty( index );
    }

    bool KFItemMoveModule::IsIndexEmpty( KFEntity* player, KFData* kfitemrecord, uint32 index )
    {
        if ( index == 0u )
        {
            return false;
        }

        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        if ( kfindex == nullptr )
        {
            return false;
        }

        return kfindex->IsEmpty( index );
    }

    void KFItemMoveModule::RemoveItemEmptyIndex( KFEntity* player, KFData* kfitemrecord, uint32 index, uint64 uuid )
    {
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        if ( kfindex == nullptr )
        {
            return;
        }

        kfindex->RemoveEmpty( index, uuid );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFItemMoveModule::IsExtendItem( const std::string& name )
    {
        return name == __STRING__( extend );
    }

    void KFItemMoveModule::MoveItemDataToShow( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kftargetrecord, uint64 count )
    {
        if ( !IsExtendItem( kfsourcerecord->_data_setting->_name ) || IsExtendItem( kftargetrecord->_data_setting->_name ) )
        {
            return;
        }

        KeyValue values;
        values[ __STRING__( id ) ] = kfsetting->_id;
        values[ __STRING__( count ) ] = count;
        player->AddDataToShow( kfsourcerecord->_data_setting->_name, __STRING__( item ), kfsetting->_id, values, true, kftargetrecord->_data_setting->_name );
    }

    void KFItemMoveModule::MoveItemDataToShow( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kftargetrecord, KFData* kfitem )
    {
        if ( !IsExtendItem( kfsourcerecord->_data_setting->_name ) || IsExtendItem( kftargetrecord->_data_setting->_name ) )
        {
            return;
        }

        if ( kfsetting->IsOverlay() )
        {
            player->AddDataToShow( kfitem );
        }
        else
        {
            KeyValue values;
            values[ __STRING__( id ) ] = kfsetting->_id;
            values[ __STRING__( count ) ] = kfitem->Get( __STRING__( count ) );
            player->AddDataToShow( kfsourcerecord->_data_setting->_name, __STRING__( item ), kfsetting->_id, values, true, kftargetrecord->_data_setting->_name );
        }
    }
}