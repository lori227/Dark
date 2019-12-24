#include "KFItemModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_GET_CONFIG_VALUE__( __STRING__( item ), &KFItemModule::GetItemTotalCount );
        __REGISTER_ADD_DATA_1__( __STRING__( item ), &KFItemModule::OnAddItemCallBack );
        __REGISTER_REMOVE_DATA_1__( __STRING__( item ), &KFItemModule::OnRemoveItemCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( item ), __STRING__( count ), &KFItemModule::OnItemCountUpdateCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( item ), __STRING__( time ), &KFItemModule::OnItemTimeUpdateCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_CHECK_ADD_ELEMENT__( __STRING__( item ), &KFItemModule::CheckAddItemElement );
        __REGISTER_ADD_ELEMENT__( __STRING__( item ), &KFItemModule::AddItemElement );
        __REGISTER_CHECK_REMOVE_ELEMENT__( __STRING__( item ), &KFItemModule::CheckRemoveItemElement );
        __REGISTER_REMOVE_ELEMENT__( __STRING__( item ), &KFItemModule::RemoveItemElement );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_AFTER_ENTER_PLAYER__( &KFItemModule::OnEnterItemModule );
        __REGISTER_LEAVE_PLAYER__( &KFItemModule::OnLeaveItemModule );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_SPLIT_ITEM_REQ, &KFItemModule::HandleSplitItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MERGE_ITEM_REQ, &KFItemModule::HandleMergeItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MOVE_ITEM_REQ, &KFItemModule::HandleMoveItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MOVE_ALL_ITEM_REQ, &KFItemModule::HandleMoveAllItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_ITEM_REQ, &KFItemModule::HandleExchangeItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CLEAN_ITEM_REQ, &KFItemModule::HandleCleanItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SORT_ITEM_REQ, &KFItemModule::HandleSortItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_ITEM_REQ, &KFItemModule::HandleRemoveItemReq );
    }

    void KFItemModule::BeforeShut()
    {
        __UN_TIMER_0__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_GET_CONFIG_VALUE__( __STRING__( item ) );
        __UN_ADD_DATA_1__( __STRING__( item ) );
        __UN_REMOVE_DATA_1__( __STRING__( item ) );
        __UN_UPDATE_DATA_2__( __STRING__( item ), __STRING__( count ) );
        __UN_UPDATE_DATA_2__( __STRING__( item ), __STRING__( time ) );

        __UN_CHECK_ADD_ELEMENT__( __STRING__( item ) );
        __UN_ADD_ELEMENT__( __STRING__( item ) );
        __UN_CHECK_REMOVE_ELEMENT__( __STRING__( item ) );
        __UN_REMOVE_ELEMENT__( __STRING__( item ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SPLIT_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MERGE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MOVE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MOVE_ALL_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXCHANGE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_CLEAN_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SORT_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REMOVE_ITEM_REQ );
    }

    void KFItemModule::PrepareRun()
    {
        _item_data_list.clear();
        _item_data_list = _kf_component->GetDataList( __STRING__( item ) );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFItemModule::BindInitItemFunction( uint32 itemtype, KFItemFunction& function )
    {
        auto kffunction = _init_item_function.Create( itemtype );
        kffunction->_function = function;
    }

    void KFItemModule::UnRegisteInitItemFunction( uint32 itemtype )
    {
        _init_item_function.Remove( itemtype );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_GET_CONFIG_VALUE_FUNCTION__( KFItemModule::GetItemTotalCount )
    {
        auto kfsetting = KFItemConfig::Instance()->FindSetting( id );
        if ( kfsetting == nullptr )
        {
            return 0u;
        }

        auto kfitemrecord = FindItemRecord( player, kfsetting, 0u );
        if ( kfitemrecord == nullptr )
        {
            return 0u;
        }

        auto totalcount = 0u;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
            if ( itemid != id )
            {
                continue;
            }

            totalcount += kfitem->Get<uint32>( __STRING__( count ) );
            if ( totalcount >= maxvalue )
            {
                break;
            }
        }

        return totalcount;
    }

    __KF_CHECK_ADD_ELEMENT_FUNCTION__( KFItemModule::CheckAddItemElement )
    {
        if ( !kfelement->IsObject() )
        {
            return false;
        }

        // 计算物品数量
        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        auto itemcount = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( count ), multiple );
        if ( itemcount == _invalid_int || kfelementobject->_config_id == _invalid_int )
        {
            return false;
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        kfparent = FindItemRecord( player, kfsetting, 0u );
        if ( kfparent == nullptr )
        {
            return false;
        }

        return !CheckItemRecordFull( kfparent, kfsetting, itemcount );
    }

    bool KFItemModule::CheckItemRecordFull( KFData* kfitemrecord, const KFItemSetting* kfsetting, uint32 itemcount )
    {
        auto maxoverlaycount = kfsetting->GetOverlayCount( kfitemrecord->_data_setting->_name );

        // 计算占用格子数
        uint32 usesize = ( itemcount + maxoverlaycount - 1 ) / maxoverlaycount;
        auto maxsize = kfitemrecord->MaxSize();
        auto cursize = kfitemrecord->Size();
        auto leftsize = ( maxsize > cursize ) ? ( maxsize - cursize ) : 0u;
        if ( leftsize >= usesize )
        {
            return false;
        }

        if ( kfsetting->IsOverlay() )
        {
            // 剩余格子可叠加物品数
            auto canaddcount = leftsize * maxoverlaycount;
            for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
            {
                auto itemid = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
                if ( itemid != kfsetting->_id )
                {
                    continue;
                }

                auto oldcount = kfitem->Get( __STRING__( count ) );
                canaddcount += maxoverlaycount - __MIN__( oldcount, maxoverlaycount );
                if ( canaddcount >= itemcount )
                {
                    return false;
                }
            }
        }

        return true;
    }


    uint32 KFItemModule::CalcItemAddCount( uint32 sourcecount, uint32 targetcount, uint32 maxcount )
    {
        if ( targetcount >= maxcount )
        {
            return 0u;
        }

        auto canaddcount = maxcount - targetcount;
        return sourcecount <= canaddcount ? sourcecount : canaddcount;
    }

    uint32 KFItemModule::GetCanAddItemCount( KFEntity* player, uint32 itemid, uint32 itemcount )
    {
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return 0u;
        }

        auto kfitemrecord = FindItemRecord( player, kfsetting, 0u );
        if ( kfitemrecord == nullptr )
        {
            return 0u;
        }

        auto maxoverlaycount = kfsetting->GetOverlayCount( kfitemrecord->_data_setting->_name );

        // 计算占用格子数
        uint32 usesize = ( itemcount + maxoverlaycount - 1 ) / maxoverlaycount;

        auto maxsize = kfitemrecord->MaxSize();
        auto cursize = kfitemrecord->Size();
        auto leftsize = ( maxsize > cursize ) ? ( maxsize - cursize ) : 0u;
        if ( leftsize >= usesize )
        {
            return itemcount;
        }

        // 不能叠加
        if ( !kfsetting->IsOverlay() )
        {
            return leftsize;
        }

        // 剩余格子可添加物品数
        auto canaddcount = leftsize * maxoverlaycount;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
            if ( itemid != kfsetting->_id )
            {
                continue;
            }

            auto oldcount = kfitem->Get( __STRING__( count ) );
            canaddcount += maxoverlaycount - __MIN__( oldcount, maxoverlaycount );
            if ( canaddcount >= itemcount )
            {
                return itemcount;
            }
        }

        return canaddcount;
    }

    uint32 KFItemModule::GetItemAutoType( uint32 itemid )
    {
        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfitemsetting == nullptr )
        {
            return KFMsg::AutoInvalid;
        }

        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
        if ( kftypesetting == nullptr )
        {
            return KFMsg::AutoInvalid;
        }

        return kftypesetting->_is_auto;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFItemModule::IsExtendItem( const std::string& name )
    {
        return name == __STRING__( extend );
    }

    void KFItemModule::MoveItemDataToShow( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kftargetrecord, uint64 count )
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

    void KFItemModule::MoveItemDataToShow( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kftargetrecord, KFData* kfitem )
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
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFItemModule::AddItemElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_data_name );
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "id=[{}] itemsetting = null!", kfelementobject->_config_id );
        }

        // 查找背包数据
        kfparent = FindItemRecord( player, kfsetting, 0u );
        if ( kfparent == nullptr )
        {
            return;
        }

        // 计算物品数量
        auto itemcount = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( count ), multiple );
        if ( itemcount == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0!", kfelementobject->_config_id );
        }

        // 添加道具调用脚本
        CallItemLuaFunction( player, KFItemEnum::AddFunction, kfsetting, itemcount );

        // 自动打开的礼包
        if ( kfsetting->_type == KFItemEnum::Gift && kfparent->_data_setting->_name == __STRING__( gift ) && kfsetting->_auto_use != 0u )
        {
            kfresult->AddResult( kfsetting->_id, kfparent->_data_setting->_name, __STRING__( count ), itemcount );

            // 礼包道具 进入仓库自动使用
            if ( !kfsetting->_reward.IsEmpty() )
            {
                player->AddElement( &kfsetting->_reward, __FUNC_LINE__, itemcount );
            }
            else
            {
                __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] reward is null!", kfelementobject->_config_id );
            }
        }
        else
        {
            if ( kfsetting->_overlay_type == KFItemEnum::OverlayByTime )
            {
                // 计算时间
                auto itemtime = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( time ), multiple );
                if ( itemtime == 0u )
                {
                    return __LOG_ERROR_FUNCTION__( function, line, "time item id=[{}] count = 0!", kfelementobject->_config_id );
                }

                AddOverlayTimeItem( player, kfparent, kfresult, kfsetting, itemcount, itemtime );
            }
            else if ( kfsetting->_overlay_type == KFItemEnum::OverlayByCount )
            {
                // 按数量叠加道具
                if ( kfsetting->IsOverlay() )
                {
                    AddOverlayCountItem( player, kfparent, kfresult, kfsetting, itemcount );
                }
                else
                {
                    AddNotOverlayItem( player, kfparent, kfresult, kfsetting, itemcount );
                }
            }
        }
    }

    void KFItemModule::AddOverlayTimeItem( KFEntity* player, KFData* kfparent, KFElementResult* kfresult, const KFItemSetting* kfsetting, uint32 count, uint32 time )
    {
        std::list< KFData* > finditem;
        kfparent->Find( kfparent->_data_setting->_config_key_name, kfsetting->_id, finditem, false );

        KFData* kfitem = nullptr;
        if ( !finditem.empty() )
        {
            kfitem = finditem.front();
            player->UpdateData( kfitem, __STRING__( time ), KFEnum::Add, count * time );
        }
        else
        {
            kfitem = AddNewItemData( player, kfparent, kfsetting, count, time );
        }

        kfresult->AddResult( kfitem );
    }

    void KFItemModule::AddOverlayCountItem( KFEntity* player, KFData* kfparent, KFElementResult* kfresult, const KFItemSetting* kfsetting, uint32 count )
    {
        auto maxoverlaycount = kfsetting->GetOverlayCount( kfparent->_data_setting->_name );

        // 查找相同道具的id
        auto totaladdcount = 0u;
        for ( auto kfitem = kfparent->First(); kfitem != nullptr; kfitem = kfparent->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            if ( itemid != kfsetting->_id )
            {
                continue;
            }

            auto oldcount = kfitem->Get( __STRING__( count ) );
            auto addcount = CalcItemAddCount( count, oldcount, maxoverlaycount );
            if ( addcount == 0u )
            {
                continue;
            }

            // 更新数量
            player->UpdateData( kfitem, __STRING__( count ), KFEnum::Add, addcount );
            count -= addcount;
            totaladdcount += addcount;
            if ( count == 0u )
            {
                break;
            }
        }

        // 显示添加数量
        if ( totaladdcount != 0u )
        {
            kfresult->AddResult( kfsetting->_id, kfparent->_data_setting->_name, __STRING__( count ), totaladdcount );
        }

        // 添加完成, 直接返回
        if ( count == 0u )
        {
            return;
        }

        // 添加新物品
        do
        {
            auto kfitem = AddNewItemData( player, kfparent, kfsetting, count, 0u );
            kfresult->AddResult( kfsetting->_id, kfitem->_data_setting->_name, __STRING__( count ), kfitem->Get<uint32>( __STRING__( count ) ) );
        } while ( count > 0u );
    }

    void KFItemModule::AddNotOverlayItem( KFEntity* player, KFData* kfparent, KFElementResult* kfresult, const KFItemSetting* kfsetting, uint32 count )
    {
        // 添加新物品
        do
        {
            auto kfitem = AddNewItemData( player, kfparent, kfsetting, count, 0u );
            kfresult->AddResult( kfitem );
        } while ( count > 0u );
    }

    KFData* KFItemModule::AddNewItemData( KFEntity* player, KFData* kfparent, const KFItemSetting* kfsetting, uint32& count, uint32 time )
    {
        auto maxoverlaycount = kfsetting->GetOverlayCount( kfparent->_data_setting->_name );
        auto addcount = __MIN__( count, maxoverlaycount );

        // 判断满包情况, 需要添加另外的背包, 加入满包不显示( 测试特殊要求 )
        auto kfnewparent = FindItemRecord( player, kfsetting, addcount );
        if ( kfnewparent != nullptr && kfnewparent != kfparent )
        {
            kfparent = kfnewparent;
            maxoverlaycount = kfsetting->GetOverlayCount( kfparent->_data_setting->_name );
        }

        auto kfitem = player->CreateData( kfparent );

        // count
        kfitem->Set( __STRING__( count ), addcount );

        // id
        kfitem->Set( kfparent->_data_setting->_config_key_name, kfsetting->_id );

        // time
        if ( time != 0u )
        {
            kfitem->Set( __STRING__( time ), KFGlobal::Instance()->_real_time + time * count );
        }

        // 位置
        auto index = FindItemEmptyIndex( player, kfparent );
        if ( index != 0u )
        {
            kfitem->Set( __STRING__( index ), index );
        }

        // 初始化数据
        auto kffunction = _init_item_function.Find( kfsetting->_type );
        if ( kffunction != nullptr )
        {
            kffunction->_function( player, kfitem, kfsetting );
        }

        // uuid
        auto uuid = KFGlobal::Instance()->STMakeUUID( __STRING__( item ) );

        // 添加新的物品
        count -= addcount;
        player->AddData( kfparent, uuid, kfitem );
        return kfitem;
    }

    KFData* KFItemModule::CreateItem( KFEntity* player, uint32 itemid, KFData* kfitem, const char* function, uint32 line )
    {
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item=[{}] can't find setting!", itemid );
            return kfitem;
        }

        if ( kfitem == nullptr )
        {
            kfitem = player->CreateData( _item_data_list.front() );
            if ( kfitem == nullptr )
            {
                return kfitem;
            }
        }

        // id
        kfitem->Set( kfitem->_data_setting->_config_key_name, kfsetting->_id );

        // count
        kfitem->Set( __STRING__( count ), 1u );

        // uuid
        kfitem->SetKeyID( KFGlobal::Instance()->STMakeUUID( __STRING__( item ) ) );

        // 初始化数据
        auto kffunction = _init_item_function.Find( kfsetting->_type );
        if ( kffunction != nullptr )
        {
            kffunction->_function( player, kfitem, kfsetting );
        }

        return kfitem;
    }

    KFData* KFItemModule::FindItemRecord( KFEntity* player, const KFItemSetting* kfsetting, uint32 itemcount )
    {
        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfsetting->_type );
        if ( kftypesetting == nullptr )
        {
            __LOG_ERROR__( "item=[{}] type=[{}] no type setting!", kfsetting->_id, kfsetting->_type );
            return nullptr;
        }

        KFData* kfitemrecord = nullptr;
        auto status = player->GetStatus();
        if ( status == KFMsg::ExploreStatus || status == KFMsg::PVEStatus )
        {
            kfitemrecord = player->Find( kftypesetting->_bag_name );

            // 需要判断包满情况
            if ( kfitemrecord != nullptr && itemcount > 0u && !kftypesetting->_extend_name.empty() )
            {
                auto isfull = CheckItemRecordFull( kfitemrecord, kfsetting, itemcount );
                if ( isfull )
                {
                    kfitemrecord = player->Find( kftypesetting->_extend_name );
                }
            }
        }
        else
        {
            kfitemrecord = player->Find( kftypesetting->_store_name );
        }

        if ( kfitemrecord == nullptr )
        {
            __LOG_ERROR__( "item=[{}] store=[{}] bag=[{}] extend=[{}] status=[{}] error!",
                           kfsetting->_id, kftypesetting->_store_name, kftypesetting->_bag_name, kftypesetting->_extend_name, status );
        }

        return kfitemrecord;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFItemModule::OnItemCountUpdateCallBack )
    {
        if ( newvalue > 0u )
        {
            return;
        }

        // 数量为0, 删除道具
        player->RemoveData( kfdata->GetParent()->GetParent(), key );
    }

    bool KFItemModule::CallItemLuaFunction( KFEntity* player, uint32 functiontype, const KFItemSetting* kfsetting, uint32 itemcount )
    {
        if ( kfsetting->_lua_file.empty() )
        {
            return false;
        }

        auto& luafunction = kfsetting->GetFunction( functiontype );
        if ( luafunction.empty() )
        {
            return false;
        }

        auto playerid = player->GetKeyID();
        auto result = _kf_lua->Call( kfsetting->_lua_file, luafunction, playerid, kfsetting->_id, itemcount );
        return result != 0u;
    }

    __KF_CHECK_REMOVE_ELEMENT_FUNCTION__( KFItemModule::CheckRemoveItemElement )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_data_name );
            return false;
        }

        // 判断数量
        auto itemcount = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( count ), multiple );
        if ( itemcount == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0!", kfelementobject->_config_id );
            return false;
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] setting = nullptr!", kfelementobject->_config_id );
            return false;
        }

        kfparent = FindItemRecord( player, kfsetting, 0u );
        if ( kfparent == nullptr )
        {
            return false;
        }

        // 累计数量
        auto totalcount = 0u;
        for ( auto kfitem = kfparent->First(); kfitem != nullptr; kfitem = kfparent->Next() )
        {
            auto itemid = kfitem->Get( kfparent->_data_setting->_config_key_name );
            if ( itemid != kfsetting->_id )
            {
                continue;
            }

            totalcount += kfitem->Get<uint32>( __STRING__( count ) );
            if ( totalcount >= itemcount )
            {
                return true;
            }
        }

        auto status = player->GetStatus();
        if ( status == KFMsg::ExploreStatus || status == KFMsg::PVEStatus )
        {
            return false;
        }

        // 主营地可消耗背包内道具
        KFData* kfitemrecord = player->Find( __STRING__( explore ) );
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto itemid = kfitem->Get( kfparent->_data_setting->_config_key_name );
            if ( itemid != kfsetting->_id )
            {
                continue;
            }

            totalcount += kfitem->Get<uint32>( __STRING__( count ) );
            if ( totalcount >= itemcount )
            {
                return true;
            }
        }

        return false;
    }

    __KF_REMOVE_ELEMENT_FUNCTION__( KFItemModule::RemoveItemElement )
    {
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_data_name );
        }

        auto itemcount = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( count ), multiple );
        if ( itemcount == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0!", kfelementobject->_config_id );
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            return;
        }

        kfparent = FindItemRecord( player, kfsetting, 0u );
        if ( kfparent == nullptr )
        {
            return;
        }

        auto findcount = 0u;
        std::list< KFData* > finditem;
        for ( auto kfitem = kfparent->First(); kfitem != nullptr; kfitem = kfparent->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfparent->_data_setting->_config_key_name );
            if ( itemid != kfelementobject->_config_id )
            {
                continue;
            }

            finditem.push_back( kfitem );
            findcount += kfitem->Get<uint32>( __STRING__( count ) );
            if ( findcount >= itemcount )
            {
                break;
            }
        }

        if ( findcount < itemcount )
        {
            auto status = player->GetStatus();
            if ( status != KFMsg::ExploreStatus && status != KFMsg::PVEStatus )
            {
                // 物品数量不够,主营地可消耗背包内道具
                KFData* kfitemrecord = player->Find( __STRING__( explore ) );
                for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
                {
                    auto itemid = kfitem->Get<uint32>( kfparent->_data_setting->_config_key_name );
                    if ( itemid != kfelementobject->_config_id )
                    {
                        continue;
                    }

                    finditem.push_back( kfitem );
                    findcount += kfitem->Get<uint32>( __STRING__( count ) );
                    if ( findcount >= itemcount )
                    {
                        break;
                    }
                }
            }
        }

        // 扣除道具
        for ( auto kfitem : finditem )
        {
            auto removecount = __MIN__( itemcount, kfitem->Get<uint32>( __STRING__( count ) ) );
            player->UpdateData( kfitem, __STRING__( count ), KFEnum::Dec, removecount );
            itemcount -= removecount;
            if ( itemcount == _invalid_int )
            {
                break;
            }
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFItemModule::OnAddItemCallBack )
    {
        // 保存格子信息
        RemoveItemEmptyIndex( player, kfdata );

        auto itemtime = kfdata->Get< uint64 >();
        if ( itemtime != 0u )
        {
            StartItemCheckTimer( player, kfdata );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemModule::OnItemTimeUpdateCallBack )
    {
        if ( newvalue == 0u )
        {
            // 如果变成永久道具, 取消定时器
            __UN_TIMER_2__( player->GetKeyID(), key );
        }
        else
        {
            // 更新定时器
            StartItemCheckTimer( player, kfdata->GetParent() );
        }
    }

    __KF_AFTER_ENTER_PLAYER_FUNCTION__( KFItemModule::OnEnterItemModule )
    {
        for ( auto& dataname : _item_data_list )
        {
            auto kfitemrecord = player->Find( dataname );
            for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
            {
                auto time = kfitem->Get<uint64>( __STRING__( time ) );
                if ( time != 0u )
                {
                    StartItemCheckTimer( player, kfitem );
                }
            }

            InitItemEmptyIndexData( player, kfitemrecord );
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFItemModule::OnLeaveItemModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
        for ( auto& dataname : _item_data_list )
        {
            UnInitItemEmptyIndexData( player, dataname );
        }
    }

    void KFItemModule::StartItemCheckTimer( KFEntity* player, KFData* kfitem )
    {
        auto intervaltime = 1000u;
        auto nowtime = KFGlobal::Instance()->_real_time;
        auto itemtime = kfitem->Get<uint64>( __STRING__( time ) );
        if ( itemtime > nowtime )
        {
            intervaltime = ( itemtime - nowtime + 1 ) * 1000;
        }

        __LIMIT_TIMER_2__( player->GetKeyID(), kfitem->GetKeyID(), intervaltime, 1, &KFItemModule::OnTimerRemoveTimeItem );
    }

    __KF_TIMER_FUNCTION__( KFItemModule::OnTimerRemoveTimeItem )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        for ( auto& dataname : _item_data_list )
        {
            auto ok = player->RemoveData( dataname, subid );
            if ( ok )
            {
                break;
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleRemoveItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveItemReq );

        __LOG_INFO__( "player=[{}] remove item=[{}:{}]", playerid, kfmsg.sourcename(), kfmsg.uuid() );

        if ( kfmsg.uuid() == 0u )
        {
            player->CleanData( kfmsg.sourcename() );
        }
        else
        {
            player->RemoveData( kfmsg.sourcename(), kfmsg.uuid() );
        }
    }

    __KF_REMOVE_DATA_FUNCTION__( KFItemModule::OnRemoveItemCallBack )
    {
        // 清空索引
        AddItemEmptyIndex( player, kfdata );

        auto itemid = kfdata->Get( kfdata->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        auto count = kfdata->Get<uint32>( __STRING__( count ) );
        CallItemLuaFunction( player, KFItemEnum::RemoveFunction, kfsetting, __MAX__( count, 1 ) );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::tuple<KFData*, KFData*> KFItemModule::FindItem( KFEntity* player, uint64 itemuuid )
    {
        for ( auto& strname : _item_data_list )
        {
            auto kfitemrecord = player->Find( strname );
            auto kfitem = kfitemrecord->Find( itemuuid );
            if ( kfitem != nullptr )
            {
                return std::make_tuple( kfitemrecord, kfitem );
            }
        }

        return std::make_tuple( nullptr, nullptr );
    }

    KFData* KFItemModule::FindIndexItem( KFData* kfitemrecord, uint32 index )
    {
        if ( index != 0u )
        {
            for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
            {
                auto findindex = kfitem->Get<uint32>( __STRING__( index ) );
                if ( findindex == index )
                {
                    return kfitem;
                }
            }
        }

        return nullptr;
    }

    uint32 KFItemModule::SplitItem( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, uint32 splitcount, KFData* kftargetrecord, uint32 splitindex )
    {
        // 如果没指定索引, 查找一个索引
        if ( kftargetrecord->_data_setting->_int_logic_value == __USE_ITEM_INDEX__ )
        {
            if ( splitindex == 0u )
            {
                splitindex = FindItemEmptyIndex( player, kftargetrecord );
                if ( splitindex == 0u )
                {
                    return KFMsg::ItemBagFull;
                }
            }
        }

        // 扣除源物品数量
        MoveItemCount( player, kfsourceitem, KFEnum::Dec, splitcount );

        // 添加新的道具
        auto kftargetitem = player->CreateData( kfsourceitem );
        kftargetitem->CopyFrom( kfsourceitem );
        kftargetitem->Set( __STRING__( index ), splitindex );
        kftargetitem->Set( __STRING__( count ), splitcount );

        auto uuid = KFGlobal::Instance()->STMakeUUID( __STRING__( item ) );
        player->AddData( kftargetrecord, uuid, kftargetitem, false );

        // 添加显示
        MoveItemDataToShow( player, kfsetting, kfsourcerecord, kftargetrecord, splitcount );
        return KFMsg::Ok;
    }

    uint32 KFItemModule::MoveItem( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord, uint32 targetindex )
    {
        // 如果没指定索引, 查找一个索引
        if ( kftargetrecord->_data_setting->_int_logic_value == __USE_ITEM_INDEX__ )
        {
            if ( targetindex == 0u )
            {
                targetindex = FindItemEmptyIndex( player, kftargetrecord );
                if ( targetindex == 0u )
                {
                    return KFMsg::ItemBagFull;
                }
            }
        }

        // 源背包索引
        AddItemEmptyIndex( player, kfsourceitem );

        // 如果背包相同
        if ( kfsourcerecord == kftargetrecord )
        {
            player->UpdateData( kfsourceitem, __STRING__( index ), KFEnum::Set, targetindex );
        }
        else
        {
            // 设置新的索引
            kfsourceitem->Set( __STRING__( index ), targetindex );

            // 移动
            kfsourceitem = player->MoveData( kfsourcerecord, kfsourceitem->GetKeyID(), kftargetrecord );
            if ( kfsourceitem == nullptr )
            {
                return KFMsg::ItemMoveFailed;
            }
        }

        // 目标背包索引
        RemoveItemEmptyIndex( player, kfsourceitem );

        // 添加显示
        MoveItemDataToShow( player, kfsetting, kfsourcerecord, kftargetrecord, kfsourceitem );
        return KFMsg::Ok;
    }

    bool KFItemModule::CheckItemCanMove( const KFItemSetting* kfsetting, const std::string& sourcename, const std::string& targetname )
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

    bool KFItemModule::CheckItemCanExchange( const KFItemSetting* kfsourcesetting, KFData* kfsourceitem, const KFItemSetting* kftargetsetting, KFData* kftargetitem )
    {
        // 判断是否能移动
        if ( !CheckItemCanMove( kfsourcesetting, kfsourceitem->_data_setting->_name, kftargetitem->_data_setting->_name ) )
        {
            return false;
        }

        // 判断是否能移动
        if ( !CheckItemCanMove( kftargetsetting, kftargetitem->_data_setting->_name, kfsourceitem->_data_setting->_name ) )
        {
            return false;
        }

        // 源物品
        {
            auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
            auto maxoverlaycount = kfsourcesetting->GetOverlayCount( kftargetitem->_data_setting->_name );
            if ( sourceitemcount > maxoverlaycount )
            {
                return false;
            }
        }

        // 目标物品
        {
            auto targetitemcount = kftargetitem->Get<uint32>( __STRING__( count ) );
            auto maxoverlaycount = kftargetsetting->GetOverlayCount( kfsourceitem->_data_setting->_name );
            if ( targetitemcount > maxoverlaycount )
            {
                return false;
            }
        }

        return true;
    }

    uint32 KFItemModule::ExchangeItem( KFEntity* player, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord, KFData* kftargetitem )
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
        }
        else
        {
            kfsourceitem->Set( __STRING__( index ), targetindex );
            kftargetitem->Set( __STRING__( index ), sourceindex );

            // 交换
            player->MoveData( kfsourcerecord, kfsourceitem->GetKeyID(), kftargetrecord );
            player->MoveData( kftargetrecord, kftargetitem->GetKeyID(), kfsourcerecord );
        }

        return KFMsg::Ok;
    }

    uint32 KFItemModule::MergeItem( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, uint32 mergecount, KFData* kftargetrecord, KFData* kftargetitem )
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
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleMoveItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMoveItemReq );

        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        auto kftargetrecord = player->Find( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto maxindex = ItemMaxIndex( player, kftargetrecord );
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

        if ( !CheckItemCanMove( kfsourcesetting, kfmsg.sourcename(), kfmsg.targetname() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotStore );
        }

        uint32 result = KFMsg::Failed;
        auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        auto kftargetitem = FindIndexItem( kftargetrecord, kfmsg.targetindex() );
        if ( kftargetitem == nullptr )
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
        else
        {
            auto targetitemid = kftargetitem->Get<uint32>( kftargetitem->_data_setting->_config_key_name );
            auto kftargetsetting = KFItemConfig::Instance()->FindSetting( targetitemid );
            if ( kftargetsetting != nullptr )
            {
                // 格子上有道具, 判断id是否相同
                if ( targetitemid == sourceitemid )
                {
                    // 判断是否可堆叠
                    if ( kfsourcesetting->IsOverlay() )
                    {
                        // 移动源物品的一定数量到目标物品上
                        result = MergeItem( player, kfsourcesetting, kfsourcerecord, kfsourceitem, sourceitemcount, kftargetrecord, kftargetitem );
                    }
                    else
                    {
                        // 交换物品
                        result = ExchangeItem( player, kfsourcerecord, kfsourceitem, kftargetrecord, kftargetitem );
                    }
                }
                else
                {
                    if ( CheckItemCanExchange( kfsourcesetting, kfsourceitem, kftargetsetting, kftargetitem ) )
                    {
                        // 直接交换
                        result = ExchangeItem( player, kfsourcerecord, kfsourceitem, kftargetrecord, kftargetitem );
                    }
                    else
                    {
                        result = KFMsg::ItemCanNotStore;
                    }
                }
            }
            else
            {
                result = KFMsg::ItemSettingNotExist;
            }
        }

        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleMoveAllItemReq )
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

    void KFItemModule::MoveItemDataToRecord( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord )
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
        }

        // 不能堆叠或者剩下没有堆叠完的
        MoveItem( player, kfsetting, kfsourcerecord, kfsourceitem, kftargetrecord, 0u );
    }

    void KFItemModule::MoveItemCount( KFEntity* player, KFData* kfitem, uint32 operate, uint32 count )
    {
        auto nowcount = player->MoveData( kfitem, __STRING__( count ), operate, count );
        if ( nowcount == 0u )
        {
            player->RemoveData( kfitem->GetParent(), kfitem->GetKeyID(), false );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleSplitItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSplitItemReq );

        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        auto kftargetrecord = player->Find( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto maxindex = ItemMaxIndex( player, kftargetrecord );
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

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleMergeItemReq )
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

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleExchangeItemReq )
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

        if ( !CheckItemCanExchange( kfsourcesetting, kfsourceitem, kftargetsetting, kftargetitem ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotStore );
        }

        auto result = ExchangeItem( player, kfsourcerecord, kfsourceitem, kftargetrecord, kftargetitem );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleCleanItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCleanItemReq );

        CleanItem( player, kfmsg.sourcename() );
    }

    void KFItemModule::CleanItem( KFEntity* player, const std::string& name, bool isauto )
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
            player->AddElement( &kfitemsetting->_reward, __FUNC_LINE__, itemnum );
        }

        for ( auto iter : destorylist )
        {
            player->RemoveData( kfsourcerecord, iter );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleSortItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSortItemReq );

        for ( auto i = 0; i < kfmsg.sourcename_size(); ++i )
        {
            SortItem( player, kfmsg.sourcename( i ) );
        }

        _kf_display->SendToClient( player, KFMsg::ItemSortOk );
    }

    void KFItemModule::SortItem( KFEntity* player, const std::string& name )
    {
        auto kfitemrecord = player->Find( name );
        if ( kfitemrecord == nullptr || kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ )
        {
            return;
        }

        // 重置格子数量
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Create( key );
        kfindex->InitMaxIndex( kfitemrecord->_data_setting->_int_max_value );

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

    std::tuple<KFData*, uint32> FindMaxCountItem( uint32 maxoverlaycount, std::set< KFData* >& itemlist )
    {
        auto maxcount = 0u;
        KFData* kffind = nullptr;

        for ( auto kfitem : itemlist )
        {
            auto count = kfitem->Get<uint32>( __STRING__( count ) );
            if ( count < maxoverlaycount && count > maxcount )
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

    void KFItemModule::SortItem( KFEntity* player, KFData* kfitemrecord, KFItemIndex* kfindex, const KFItemSetting* kfsetting, std::set<KFData*>& itemlist )
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
                std::tie( maxitem, maxcount ) = FindMaxCountItem( maxoverlaycount, itemlist );
                if ( maxitem == nullptr )
                {
                    break;
                }

                if ( maxitem == minitem )
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

            auto index = kfindex->FindEmpty();
            player->UpdateData( kfitem, __STRING__( index ), KFEnum::Set, index );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFItemModule::InitItemEmptyIndexData( KFEntity* player, KFData* kfitemrecord )
    {
        if ( kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ )
        {
            return;
        }

        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Create( key );
        kfindex->InitMaxIndex( kfitemrecord->_data_setting->_int_max_value );

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
                kfindex->RemoveEmpty( index );
            }
        }

        // 如果存在没有索引的情况, 纠正数据
        if ( !invalid.empty() )
        {
            for ( auto kfitem : invalid )
            {
                auto index = kfindex->FindEmpty();
                if ( index == 0u )
                {
                    break;
                }

                kfitem->Set( __STRING__( index ), index );
            }
        }
    }

    uint32 KFItemModule::ItemMaxIndex( KFEntity* player, KFData* kfitemrecord )
    {
        if ( kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ )
        {
            return 0u;
        }

        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        if ( kfindex == nullptr )
        {
            return 0u;
        }

        return kfindex->MaxIndex();
    }

    void KFItemModule::UnInitItemEmptyIndexData( KFEntity* player, const std::string& name )
    {
        ItemIndexKey key( player->GetKeyID(), name );
        _player_item_index.Remove( key );
    }

    uint32 KFItemModule::FindItemEmptyIndex( KFEntity* player, KFData* kfitemrecord )
    {
        if ( kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ )
        {
            return 0u;
        }

        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        if ( kfindex == nullptr )
        {
            return 0u;
        }

        return kfindex->FindEmpty();
    }

    void KFItemModule::RemoveItemEmptyIndex( KFEntity* player, KFData* kfitem )
    {
        auto kfitemrecord = kfitem->GetParent();
        if ( kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ )
        {
            return;
        }

        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        if ( kfindex == nullptr )
        {
            return;
        }

        auto index = kfitem->Get<uint32>( __STRING__( index ) );
        kfindex->RemoveEmpty( index );
    }

    void KFItemModule::AddItemEmptyIndex( KFEntity* player, KFData* kfitem )
    {
        auto kfitemrecord = kfitem->GetParent();
        if ( kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ )
        {
            return;
        }

        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        if ( kfindex == nullptr )
        {
            return;
        }

        auto index = kfitem->Get<uint32>( __STRING__( index ) );
        kfindex->AddEmpty( index );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}