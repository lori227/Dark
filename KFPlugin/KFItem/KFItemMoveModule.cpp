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

        // 判断索引
        auto index = kfdata->Get<uint32>( __STRING__( index ) );
        if ( index == 0u || index > kfindex->_max_count )
        {
            // 索引为0, 直接找一个空格子
            index = kfindex->FindEmpty( key );
            kfdata->Set( __STRING__( index ), index );
            return;
        }

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
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFData* KFItemMoveModule::FindIndexItem( KFEntity* player, KFData* kfitemrecord, uint32 index )
    {
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

    uint32 KFItemMoveModule::SplitItemLogic( KFEntity* player, const KFItemSetting* kfitemsetting,
            const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourcerecord, KFData* kfsourceitem, uint32 splitcount,
            const KFItemBagSetting* kftargetbagsetting, KFData* kftargetrecord, uint32 splitindex )
    {
        // 扣除源物品数量
        MoveItemCountLogic( player, kfitemsetting, kfsourcebagsetting, kfsourceitem, splitcount, kftargetbagsetting, nullptr );

        // 添加新的道具
        auto kftargetitem = player->CreateData( kfsourceitem );
        kftargetitem->CopyFrom( kfsourceitem );
        auto uuid = KFGlobal::Instance()->STMakeUuid( __STRING__( item ) );
        kftargetitem->SetKeyID( uuid );
        kftargetitem->Set( __STRING__( count ), splitcount );
        kftargetitem->Set( __STRING__( index ), splitindex );

        auto isupdate = kftargetbagsetting->IsMoveAddUpdate( kfsourcebagsetting->_id );
        player->AddData( kftargetrecord, uuid, kftargetitem, isupdate );

        // 添加客户端显示
        if ( kfsourcebagsetting->IsMoveShow( kftargetbagsetting->_id ) )
        {
            MoveItemDataToShow( player, kfitemsetting, kfsourcerecord, kftargetrecord, splitcount );
        }

        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::MoveItemLogic( KFEntity* player, const KFItemSetting* kfitemsetting,
                                            const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourcerecord, KFData* kfsourceitem,
                                            const KFItemBagSetting* kftargetbagsetting, KFData* kftargetrecord, uint32 targetindex )
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
            return KFMsg::Ok;
        }

        // 不同背包, 先移除源背包
        {
            auto isupdate = kfsourcebagsetting->IsMoveAddUpdate( kftargetbagsetting->_id );
            player->MoveData( kfsourcerecord, kfsourceitem->GetKeyID(), isupdate );
        }

        // 添加目标背包
        kfsourceitem->Set( __STRING__( index ), targetindex );
        auto isupdate = kftargetbagsetting->IsMoveAddUpdate( kfsourcebagsetting->_id );
        player->AddData( kftargetrecord, kfsourceitem->GetKeyID(), kfsourceitem, isupdate );

        // 添加客户端显示
        if ( kfsourcebagsetting->IsMoveShow( kftargetbagsetting->_id ) )
        {
            MoveItemDataToShow( player, kfitemsetting, kfsourcerecord, kftargetrecord, kfsourceitem );
        }

        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::ExchangeItemLogic( KFEntity* player,
            const KFItemSetting* kfsourceitemsetting, const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourcerecord, KFData* kfsourceitem,
            const KFItemSetting* kftargetitemsetting, const KFItemBagSetting* kftargetbagsetting, KFData* kftargetrecord, KFData* kftargetitem )
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
        if ( !CheckItemCanMove( kfsourceitemsetting, kfsourceitem->_data_setting->_name, kftargetitem->_data_setting->_name ) )
        {
            return KFMsg::ItemCanNotStore;
        }

        // 找到可以移动到的背包
        auto kffindrecord = FindItemMoveRecord( player, kftargetitemsetting, kfsourceitem->_data_setting->_name, kftargetrecord->_data_setting->_name );
        if ( kffindrecord == nullptr )
        {
            return KFMsg::ItemCanNotStore;
        }

        auto kffindbagsetting = KFItemBagConfig::Instance()->FindSetting( kffindrecord->_data_setting->_name );
        if ( kffindbagsetting == nullptr )
        {
            return KFMsg::ItemCanNotStore;
        }

        // 判断源道具数量
        auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        auto maxsourceoverlaycount = kfsourceitemsetting->GetOverlayCount( kftargetrecord->_data_setting->_name );

        // 判断目标道具数量
        auto targetitemcount = kftargetitem->Get<uint32>( __STRING__( count ) );
        auto maxtargetoverlaycount = kftargetitemsetting->GetOverlayCount( kffindrecord->_data_setting->_name );

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
            SplitItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, maxsourceoverlaycount, kftargetbagsetting, kftargetrecord, 0u );

            // 目标道具拆分
            SplitItemLogic( player, kftargetitemsetting, kftargetbagsetting, kftargetrecord, kftargetrecord, maxtargetoverlaycount, kffindbagsetting, kffindrecord, 0u );
        }
        else if ( sourceitemcount > maxsourceoverlaycount )
        {
            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->IsItemRecordFull( player, kffindrecord ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 移动目标道具
            MoveItemLogic( player, kftargetitemsetting, kftargetbagsetting, kftargetrecord, kftargetitem, kffindbagsetting, kffindrecord, 0u );

            // 拆分源道具
            SplitItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, maxsourceoverlaycount, kftargetbagsetting, kftargetrecord, targetindex );
        }
        else if ( targetitemcount > maxtargetoverlaycount )
        {
            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->IsItemRecordFull( player, kftargetrecord ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 移动源道具
            MoveItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, kftargetbagsetting, kftargetrecord, 0u );

            // 拆分目标道具
            SplitItemLogic( player, kftargetitemsetting, kftargetbagsetting, kftargetrecord, kftargetitem, maxtargetoverlaycount, kffindbagsetting, kffindrecord, sourceindex );
        }
        else
        {
            if ( kffindrecord != kfsourcerecord )
            {
                if ( _kf_item->IsItemRecordFull( player, kffindrecord ) )
                {
                    return KFMsg::ItemBagIsFull;
                }

                sourceindex = 0u;
            }
            else
            {
                AddItemEmptyIndex( player, kfsourcerecord, sourceindex );
                AddItemEmptyIndex( player, kftargetrecord, targetindex );
            }

            MoveItemLogic( player, kftargetitemsetting, kftargetbagsetting, kftargetrecord, kftargetitem, kffindbagsetting, kffindrecord, sourceindex );
            MoveItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, kftargetbagsetting, kftargetrecord, targetindex );
        }

        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::MergeItemLogic( KFEntity* player, const KFItemSetting* kfitemsetting,
            const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourceitem, uint32 mergecount,
            const KFItemBagSetting* kftargetbagsetting, KFData* kftargetitem )
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
        auto maxtargetoverlaycount = kfitemsetting->GetOverlayCount( kftargetitem->_data_setting->_name );
        auto canaddcount = CalcItemAddCount( mergecount, targetitemcount, maxtargetoverlaycount );
        if ( canaddcount == 0u )
        {
            return KFMsg::ItemOverlayCountMax;
        }

        // 移动数量
        MoveItemCountLogic( player, kfitemsetting, kfsourcebagsetting, kfsourceitem, canaddcount, kftargetbagsetting, kftargetitem );
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

    void KFItemMoveModule::MoveItemCountLogic( KFEntity* player, const KFItemSetting* kfitemsetting,
            const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourceitem, uint32 movecount,
            const KFItemBagSetting* kftargetbagsetting, KFData* kftargetitem )
    {
        // 源背包
        if ( kfsourcebagsetting->IsMoveDecUpdate( kftargetbagsetting->_id ) )
        {
            player->UpdateData( kfsourceitem, __STRING__( count ), KFEnum::Dec, movecount );
        }
        else
        {
            auto sourcecount = kfsourceitem->Get<uint32>( __STRING__( count ) );
            if ( sourcecount <= movecount )
            {
                player->RemoveData( kfsourceitem->GetParent(), kfsourceitem->GetKeyID(), false );
            }
            else
            {
                player->MoveData( kfsourceitem, __STRING__( count ), KFEnum::Dec, movecount );
            }
        }

        // 目标背包
        if ( kftargetitem == nullptr )
        {
            return;
        }

        // 是否移动更新
        if ( kftargetbagsetting->IsMoveAddUpdate( kfsourcebagsetting->_id ) )
        {
            player->UpdateData( kftargetitem, __STRING__( count ), KFEnum::Add, movecount );
        }
        else
        {
            player->MoveData( kftargetitem, __STRING__( count ), KFEnum::Add, movecount );
        }

        // 添加显示
        if ( kfsourcebagsetting->IsMoveShow( kftargetbagsetting->_id ) )
        {
            MoveItemDataToShow( player, kfitemsetting, kfsourceitem->GetParent(), kftargetitem->GetParent(), movecount );
        }
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

        return kftypesetting->CheckCanMove( sourcename, targetname );
    }

    KFData* KFItemMoveModule::FindItemMoveRecord( KFEntity* player, const KFItemSetting* kfsetting, const std::string& sourcename, const std::string& excludename )
    {
        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfsetting->_type );
        if ( kftypesetting == nullptr )
        {
            return nullptr;
        }

        if ( kftypesetting->_move_name_list.find( sourcename ) != kftypesetting->_move_name_list.end() )
        {
            return player->Find( sourcename );
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
    uint32 KFItemMoveModule::MoveItem( KFEntity* player, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord, uint32 targetindex )
    {
        // 判断是否限制
        auto sourceitemid = kfsourceitem->Get<uint32>( kfsourceitem->_data_setting->_config_key_name );
        auto kfsourceitemsetting = KFItemConfig::Instance()->FindSetting( sourceitemid );
        if ( kfsourceitemsetting == nullptr )
        {
            return KFMsg::ItemSettingNotExist;
        }

        // 判断能否移动
        if ( !CheckItemCanMove( kfsourceitemsetting, kfsourcerecord->_data_setting->_name, kftargetrecord->_data_setting->_name ) )
        {
            return KFMsg::ItemCanNotStore;
        }

        auto kfsourcebagsetting = KFItemBagConfig::Instance()->FindSetting( kfsourcerecord->_data_setting->_name );
        auto kftargetbagsetting = KFItemBagConfig::Instance()->FindSetting( kftargetrecord->_data_setting->_name );
        if ( kfsourcebagsetting == nullptr || kftargetbagsetting == nullptr )
        {
            return KFMsg::ItemCanNotStore;
        }

        uint32 result = KFMsg::Failed;
        auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        auto kftargetitem = FindIndexItem( player, kftargetrecord, targetindex );
        if ( kftargetitem == nullptr )
        {
            if ( !_kf_item->IsItemRecordFull( player, kftargetrecord ) )
            {
                // 移动到空格子上
                // 判断堆叠数量, 如果目标小于源堆叠, 并且源数量大于目标堆叠数量, 拆分物品
                auto maxtargetoverlaycount = kfsourceitemsetting->GetOverlayCount( kftargetrecord->_data_setting->_name );
                if ( sourceitemcount > maxtargetoverlaycount )
                {
                    // 拆分物品
                    result = SplitItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, maxtargetoverlaycount, kftargetbagsetting, kftargetrecord, targetindex );
                }
                else
                {
                    // 移动物品
                    result = MoveItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, kftargetbagsetting, kftargetrecord, targetindex );
                }
            }
            else
            {
                result = MoveItemDataLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, kftargetbagsetting, kftargetrecord );
            }
        }
        else
        {
            auto targetitemid = kftargetitem->Get<uint32>( kftargetitem->_data_setting->_config_key_name );
            auto kftargetitemsetting = KFItemConfig::Instance()->FindSetting( targetitemid );
            if ( kftargetitemsetting != nullptr )
            {
                // 判断道具是否能堆叠
                if ( CheckItemCanMerge( kfsourceitemsetting, kfsourceitem, kftargetitemsetting, kftargetitem ) )
                {
                    // 移动源物品的一定数量到目标物品上
                    result = MergeItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourceitem, sourceitemcount, kftargetbagsetting, kftargetitem );
                }
                else
                {
                    // 直接交换
                    result = ExchangeItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, kftargetitemsetting, kftargetbagsetting, kftargetrecord, kftargetitem );
                }
            }
            else
            {
                result = KFMsg::ItemSettingNotExist;
            }
        }

        return result;
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

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleMoveItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMoveItemReq );

        {
            auto kfitembagsetting = KFItemBagConfig::Instance()->FindSetting( kfmsg.sourcename() );
            if ( kfitembagsetting != nullptr && !kfitembagsetting->_is_can_move )
            {
                return _kf_display->SendToClient( player, KFMsg::ItemBagCanNotMove );
            }
        }

        {
            auto kfitembagsetting = KFItemBagConfig::Instance()->FindSetting( kfmsg.targetname() );
            if ( kfitembagsetting != nullptr && !kfitembagsetting->_is_can_move )
            {
                return _kf_display->SendToClient( player, KFMsg::ItemBagCanNotMove );
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

        auto result = MoveItem( player, kfsourcerecord, kfsourceitem, kftargetrecord, kfmsg.targetindex() );
        if ( result != KFMsg::Ok )
        {
            player->UpdateData( kfsourceitem, __STRING__( index ), KFEnum::Set, kfsourceitem->Get<uint32>( __STRING__( index ) ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleMoveAllItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMoveAllItemReq );

        auto kfsourcebagsetting = KFItemBagConfig::Instance()->FindSetting( kfmsg.sourcename() );
        auto kftargetbagsetting = KFItemBagConfig::Instance()->FindSetting( kfmsg.targetname() );
        if ( kfsourcebagsetting == nullptr || kftargetbagsetting == nullptr || !kfsourcebagsetting->_is_can_move_all )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagCanNotMove );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 全部拾取
        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        auto kftargetrecord = player->Find( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        // 遍历出可以移动的道具
        std::map< KFData*, const KFItemSetting* > movelist;
        for ( auto kfitem = kfsourcerecord->First(); kfitem != nullptr; kfitem = kfsourcerecord->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            movelist[ kfitem ] = kfsetting;
        }

        for ( auto& iter : movelist )
        {
            auto result = MoveItemDataLogic( player, iter.second, kfsourcebagsetting, kfsourcerecord, iter.first, kftargetbagsetting, kftargetrecord );
            if ( result != KFMsg::Ok )
            {
                return _kf_display->SendToClient( player, result );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleCleanItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCleanItemReq );

        auto kfsourcebagsetting = KFItemBagConfig::Instance()->FindSetting( kfmsg.sourcename() );
        if ( kfsourcebagsetting != nullptr && !kfsourcebagsetting->_is_can_clean )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagCanNotClean );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////
        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        if ( kfsourcerecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        // 储存列表
        std::list< std::tuple< KFData*, KFData*, const KFItemSetting*, const KFItemBagSetting* > > movelist;
        for ( auto kfitem = kfsourcerecord->First(); kfitem != nullptr; kfitem = kfsourcerecord->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            auto kfitemsetting = KFItemConfig::Instance()->FindSetting( itemid );
            if ( kfitemsetting == nullptr )
            {
                continue;
            }

            auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
            if ( kftypesetting == nullptr || !kftypesetting->CheckCanMove( kfmsg.sourcename(), kftypesetting->_store_name ) )
            {
                continue;
            }

            auto kftargetrecord = player->Find( kftypesetting->_store_name );
            if ( kftargetrecord == nullptr )
            {
                continue;
            }

            auto kftargetbagsetting = KFItemBagConfig::Instance()->FindSetting( kftargetrecord->_data_setting->_name );
            if ( kftargetbagsetting == nullptr )
            {
                continue;
            }

            movelist.push_back( std::make_tuple( kfitem, kftargetrecord, kfitemsetting, kftargetbagsetting ) );
        }

        // 移动道具
        for ( auto& tupledata : movelist )
        {
            auto kfitem = std::get<0>( tupledata );
            auto kftargetrecord = std::get<1>( tupledata );
            auto kfitemsetting = std::get<2>( tupledata );
            auto kftargetbagsetting = std::get<3>( tupledata );
            MoveItemDataLogic( player, kfitemsetting, kfsourcebagsetting, kfsourcerecord, kfitem, kftargetbagsetting, kftargetrecord );
        }
    }

    uint32 KFItemMoveModule::MoveItemDataLogic( KFEntity* player, const KFItemSetting* kfitemsetting,
            const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourcerecord, KFData* kfsourceitem,
            const KFItemBagSetting* kftargetbagsetting, KFData* kftargetrecord )
    {
        // 判断是否能堆叠
        if ( kfitemsetting->IsOverlay() )
        {
            auto sourcecount = kfsourceitem->Get<uint32>( __STRING__( count ) );
            auto maxoverlaycount = kfitemsetting->GetOverlayCount( kftargetrecord->_data_setting->_name );

            // 堆叠起来
            for ( auto kftargetitem = kftargetrecord->First(); kftargetitem != nullptr; kftargetitem = kftargetrecord->Next() )
            {
                auto itemid = kftargetitem->Get<uint32>( kftargetrecord->_data_setting->_config_key_name );
                if ( itemid != kfitemsetting->_id )
                {
                    continue;
                }

                auto targetcount = kftargetitem->Get<uint32>( __STRING__( count ) );
                auto movecount = CalcItemAddCount( sourcecount, targetcount, maxoverlaycount );
                if ( movecount == 0u )
                {
                    continue;
                }

                // 移动道具数量
                MoveItemCountLogic( player, kfitemsetting, kfsourcebagsetting, kfsourceitem, movecount, kftargetbagsetting, kftargetitem );
                sourcecount -= movecount;
                if ( sourcecount == 0u )
                {
                    return KFMsg::Ok;
                }
            }

            // 剩下的道具数量
            while ( sourcecount > maxoverlaycount )
            {
                if ( _kf_item->IsItemRecordFull( player, kftargetrecord ) )
                {
                    break;
                }

                SplitItemLogic( player, kfitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, maxoverlaycount, kftargetbagsetting, kftargetrecord, 0u );
                sourcecount -= maxoverlaycount;
            }
        }

        // 不能堆叠或者剩下没有堆叠完的
        auto isfull = _kf_item->IsItemRecordFull( player, kftargetrecord );
        if ( !isfull )
        {
            MoveItemLogic( player, kfitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, kftargetbagsetting, kftargetrecord, 0u );
        }

        return isfull ? KFMsg::ItemBagIsFull : KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleSplitItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSplitItemReq );
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleMergeItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMergeItemReq );

    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleExchangeItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExchangeItemReq );

    }

    void KFItemMoveModule::BalanceItem( KFEntity* player, const std::string& name, bool autodestory )
    {
        auto kfsourcerecord = player->Find( name );
        if ( kfsourcerecord == nullptr )
        {
            return;
        }

        auto kfsourcebagsetting = KFItemBagConfig::Instance()->FindSetting( name );
        if ( kfsourcebagsetting == nullptr )
        {
            return;
        }

        // 储存列表
        std::list< std::tuple< KFData*, KFData*, const KFItemSetting*, const KFItemBagSetting* > > storelist;

        // 使用列表
        std::map< const KFItemSetting*, uint32 > uselist;

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

            if ( kfitemsetting->_type == KFItemEnum::Gift && kfitemsetting->_auto_use != 0u )
            {
                // 优先自动使用礼包道具
                uselist[ kfitemsetting ] += kfitem->Get<uint32>( __STRING__( count ) );

                // 加入销毁队列
                destorylist.emplace_back( kfitem->GetKeyID() );
            }
            else if ( kftypesetting->_is_auto == KFMsg::AutoStore )
            {
                // 自动存入仓库
                auto kftargetrecord = player->Find( kftypesetting->_store_name );
                if ( kftargetrecord != nullptr )
                {
                    auto kftargetbagsetting = KFItemBagConfig::Instance()->FindSetting( kftargetrecord->_data_setting->_name );
                    if ( kftargetbagsetting != nullptr )
                    {
                        storelist.emplace_back( std::make_tuple( kfitem, kftargetrecord, kfitemsetting, kftargetbagsetting ) );
                    }
                }
            }
            else if ( kftypesetting->_is_auto == KFMsg::AutoDestory )
            {
                if ( autodestory )
                {
                    // 自动销毁
                    auto itemuuid = kfitem->Get<uint64>( __STRING__( uuid ) );
                    destorylist.emplace_back( itemuuid );
                }
            }
        }

        // 移动道具
        for ( auto& tupledata : storelist )
        {
            auto kfitem = std::get<0>( tupledata );
            auto kftargetrecord = std::get<1>( tupledata );
            auto kfitemsetting = std::get<2>( tupledata );
            auto kftargetbagsetting = std::get<3>( tupledata );
            MoveItemDataLogic( player, kfitemsetting, kfsourcebagsetting, kfsourcerecord, kfitem, kftargetbagsetting, kftargetrecord );
        }

        // 使用道具
        for ( auto& iter : uselist )
        {
            auto kfsetting = iter.first;
            auto itemnum = iter.second;
            player->AddElement( &kfsetting->_reward, itemnum, __STRING__( useitem ), kfsetting->_id, __FUNC_LINE__ );
        }

        // 销毁道具
        for ( auto uuid : destorylist )
        {
            player->RemoveData( kfsourcerecord, uuid );
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

        auto kfitembagsetting = KFItemBagConfig::Instance()->FindSetting( name );
        if ( kfitembagsetting == nullptr )
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
                    SortItem( player, qiter.first, kfitembagsetting, kfindex, kfitemrecord, qiter.second );
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

    void KFItemMoveModule::SortItem( KFEntity* player, const KFItemSetting* kfitemsetting, const KFItemBagSetting* kfbagsetting,
                                     KFItemIndex* kfindex, KFData* kfitemrecord, std::set<KFData*>& itemlist )
    {
        auto maxoverlaycount = kfitemsetting->GetOverlayCount( kfitemrecord->_data_setting->_name );
        if ( kfitemsetting->IsOverlay() )
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
                MoveItemCountLogic( player, kfitemsetting, kfbagsetting, minitem, canaddcount, kfbagsetting, maxitem );
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
    void KFItemMoveModule::MoveItemDataToShow( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kftargetrecord, uint32 count )
    {
        KeyValue values;
        values[ __STRING__( count ) ] = count;
        values[ __STRING__( id ) ] = kfsetting->_id;
        player->AddDataToShow( kfsourcerecord->_data_setting->_name, __STRING__( item ), kfsetting->_id, values, true, kftargetrecord->_data_setting->_name );
    }

    void KFItemMoveModule::MoveItemDataToShow( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kftargetrecord, KFData* kfitem )
    {
        if ( kfsetting->IsOverlay() )
        {
            player->AddDataToShow( kfsourcerecord->_data_setting->_name, kfitem );
        }
        else
        {
            auto count = kfitem->Get( __STRING__( count ) );
            MoveItemDataToShow( player, kfsetting, kfsourcerecord, kftargetrecord, count );
        }
    }
}