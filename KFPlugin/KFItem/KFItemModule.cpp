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
        __REGISTER_ENTER_PLAYER__( &KFItemModule::OnEnterItemModule );
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
        auto itemcount = kfelementobject->CalcValue( kfparent->_class_setting, __STRING__( count ), multiple );
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

    uint32 KFItemModule::GetItemAutoType( KFData* kfitem )
    {
        auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
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

    void KFItemModule::AddItemDataToShow( KFEntity* player, uint64 id, uint64 count, const std::string& extendname )
    {
        KeyValue values;
        values[ __STRING__( id ) ] = id;
        values[ __STRING__( count ) ] = count;
        player->AddDataToShow( __STRING__( item ), id, values, true, extendname );
    }

    void KFItemModule::AddItemDataToShow( KFEntity* player, const std::string& modulename, uint64 id, uint64 count, const std::string& extendname )
    {
        KeyValue values;
        values[ __STRING__( id ) ] = id;
        values[ __STRING__( count ) ] = count;
        player->AddDataToShow( modulename, __STRING__( item ), id, values, true, extendname );
    }

    void KFItemModule::AddItemDataToShow( KFEntity* player, KFData* kfitem, const std::string& extendname )
    {
        player->AddDataToShow( kfitem, false, extendname );
    }

    void KFItemModule::AddItemDataToShow( KFEntity* player, const std::string& modulename, KFData* kfitem, const std::string& extendname )
    {
        player->AddDataToShow( modulename, kfitem, false, extendname );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFItemModule::AddItemElement )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_data_name );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "id=[{}] itemsetting = null!", kfelementobject->_config_id );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        // 查找背包数据
        kfparent = FindItemRecord( player, kfsetting, 0u );
        if ( kfparent == nullptr )
        {
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        // 计算物品数量
        auto itemcount = kfelementobject->CalcValue( kfparent->_class_setting, __STRING__( count ), multiple );
        if ( itemcount == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0!", kfelementobject->_config_id );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        // 添加道具调用脚本
        CallItemLuaFunction( player, KFItemEnum::AddFunction, kfsetting, itemcount );

        // 自动打开的礼包
        if ( kfsetting->_type == KFItemEnum::Gift && kfparent->_data_setting->_name == __STRING__( gift ) && kfsetting->_auto_use != 0u )
        {
            // 礼包道具 进入仓库自动使用
            if ( !kfsetting->_reward.IsEmpty() )
            {
                player->AddElement( &kfsetting->_reward, __FUNC_LINE__, itemcount );
            }

            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        if ( kfsetting->_overlay_type == KFItemEnum::OverlayByTime )
        {
            // 计算时间
            auto itemtime = kfelementobject->CalcValue( kfparent->_class_setting, __STRING__( time ), multiple );
            if ( itemtime == 0u )
            {
                __LOG_ERROR_FUNCTION__( function, line, "time item id=[{}] count = 0!", kfelementobject->_config_id );
                return std::make_tuple( KFDataDefine::Show_None, nullptr );
            }

            AddOverlayTimeItem( player, kfparent, kfelementobject, kfsetting, itemcount, itemtime );
        }
        else if ( kfsetting->_overlay_type == KFItemEnum::OverlayByCount )
        {
            // 按数量叠加道具
            if ( kfsetting->IsOverlay() )
            {
                AddOverlayCountItem( player, kfparent, kfelementobject, kfsetting, itemcount );
            }
            else
            {
                AddNotOverlayItem( player, kfparent, kfelementobject, kfsetting, itemcount );
            }
        }

        return std::make_tuple( KFDataDefine::Show_None, nullptr );
    }

    void KFItemModule::AddOverlayTimeItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count, uint32 time )
    {
        std::list< KFData* > finditem;
        kfparent->Find( kfparent->_data_setting->_config_key_name, kfsetting->_id, finditem, false );

        if ( !finditem.empty() )
        {
            auto kfitem = finditem.front();
            player->UpdateData( kfitem, __STRING__( time ), KFEnum::Add, count * time );
            AddItemDataToShow( player, kfsetting->_id, count, kfitem->GetParent()->_data_setting->_name );
        }
        else
        {
            auto kfitem = AddNewItemData( player, kfparent, kfelementobject, kfsetting, count, time );
            AddItemDataToShow( player, kfsetting->_id, count, kfitem->GetParent()->_data_setting->_name );
        }
    }

    void KFItemModule::AddOverlayCountItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count )
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
            AddItemDataToShow( player, kfsetting->_id, totaladdcount, kfparent->_data_setting->_name );
        }

        // 添加完成, 直接返回
        if ( count == 0u )
        {
            return;
        }

        // 添加新物品
        do
        {
            auto kfitem = AddNewItemData( player, kfparent, kfelementobject, kfsetting, count, 0u );
            auto addcount = kfitem->Get<uint32>( __STRING__( count ) );
            AddItemDataToShow( player, kfsetting->_id, addcount, kfitem->GetParent()->_data_setting->_name );
        } while ( count > 0u );
    }

    void KFItemModule::AddNotOverlayItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count )
    {
        // 添加新物品
        do
        {
            auto lastcount = count;
            auto kfitem = AddNewItemData( player, kfparent, kfelementobject, kfsetting, count, 0u );
            AddItemDataToShow( player, kfitem, kfitem->GetParent()->_data_setting->_name );
        } while ( count > 0u );
    }

    KFData* KFItemModule::AddNewItemData( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32& count, uint32 time )
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

        auto datasetting = kfparent->_data_setting;
        auto kfitem = _kf_kernel->CreateObject( datasetting );

        // count
        kfitem->Set( __STRING__( count ), addcount );

        // id
        kfitem->Set( datasetting->_config_key_name, kfsetting->_id );

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
        auto itemcount = kfelementobject->CalcValue( kfparent->_class_setting, __STRING__( count ), multiple );
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

        auto itemcount = kfelementobject->CalcValue( kfparent->_class_setting, __STRING__( count ), multiple );
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

    __KF_ENTER_PLAYER_FUNCTION__( KFItemModule::OnEnterItemModule )
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
    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleSplitItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSplitItemReq );

        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        auto kftargetrecord = player->Find( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        if ( kftargetrecord->IsFull() )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagFull );
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

        // 索引判断
        auto index = kfmsg.targetindex();
        if ( kftargetrecord->_data_setting->_int_logic_value == __USE_ITEM_INDEX__ )
        {
            // 同背包不能相同的索引
            if ( kfmsg.sourcename() == kfmsg.targetname() )
            {
                auto sourceindex = kfsourceitem->Get<uint32>( __STRING__( index ) );
                if ( sourceindex == index )
                {
                    return;
                }
            }

            if ( index != 0u )
            {
                if ( index > kftargetrecord->_data_setting->_int_max_value )
                {
                    return _kf_display->SendToClient( player, KFMsg::ItemIndexError );
                }
            }
            else
            {
                index = FindItemEmptyIndex( player, kftargetrecord );
            }
        }

        uint32 maxoverlaycount = kfsetting->GetOverlayCount( kftargetrecord->_data_setting->_name );
        auto splitcount = __MIN__( maxoverlaycount, kfmsg.sourcecount() );

        // 扣除数量
        MoveItemCount( player, kfsourceitem, KFEnum::Dec, splitcount );

        auto kftargetitem = _kf_kernel->CreateObject( kfsourceitem->_data_setting );
        kftargetitem->CopyFrom( kfsourceitem );
        kftargetitem->Set( __STRING__( index ), index );
        kftargetitem->Set( __STRING__( count ), splitcount );

        // 添加新的道具
        auto uuid = KFGlobal::Instance()->STMakeUUID( __STRING__( item ) );
        player->AddData( kftargetrecord, uuid, kftargetitem, false );
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

        auto maxoverlaycount = kfsetting->GetOverlayCount( kftargetrecord->_data_setting->_name );
        auto targetcount = kftargetitem->Get<uint32>( __STRING__( count ) );
        if ( targetcount >= maxoverlaycount )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemOverlayCountMax );
        }

        auto sourcecount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        auto canaddcount = CalcItemAddCount( sourcecount, targetcount, maxoverlaycount );

        MoveItemCount( player, kfsourceitem, KFEnum::Dec, canaddcount );
        MoveItemCount( player, kftargetitem, KFEnum::Add, canaddcount );

        if ( IsExtendItem( kfmsg.sourcename() ) && !IsExtendItem( kfmsg.targetname() ) )
        {
            if ( kfsetting->IsOverlay() )
            {
                AddItemDataToShow( player, kfmsg.targetname(), kfsetting->_id, canaddcount, kfmsg.targetname() );
            }
            else
            {
                AddItemDataToShow( player, kfmsg.targetname(), kfsourceitem, kfmsg.targetname() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleMoveItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMoveItemReq );

        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        auto kftargetrecord = player->Find( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto kfsourceitem = kfsourcerecord->Find( kfmsg.sourceuuid() );
        if ( kfsourceitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
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

        auto itemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        auto result = MoveItem( player, kfsourcerecord, kfsourceitem, kfmsg.sourceuuid(), kftargetrecord, kfmsg.targetindex() );
        if ( result == KFMsg::Ok )
        {
            if ( IsExtendItem( kfmsg.sourcename() ) && !IsExtendItem( kfmsg.targetname() ) )
            {
                if ( kfsetting->IsOverlay() )
                {
                    AddItemDataToShow( player, kfmsg.targetname(), kfsetting->_id, itemcount, kfmsg.targetname() );
                }
                else
                {
                    AddItemDataToShow( player, kfmsg.targetname(), kfsourceitem, kfmsg.targetname() );
                }
            }
        }
        else
        {
            _kf_display->SendToClient( player, result );
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
            auto itemcount = MoveItemData( player, kfsourcerecord, kfsourceitem, kftargetrecord, kfsetting );
            if ( itemcount != 0u && IsExtendItem( kfmsg.sourcename() ) && !IsExtendItem( kfmsg.targetname() ) )
            {
                if ( kfsetting->IsOverlay() )
                {
                    AddItemDataToShow( player, kfmsg.targetname(), kfsetting->_id, itemcount, kfmsg.targetname() );
                }
                else
                {
                    AddItemDataToShow( player, kfmsg.targetname(), kfsourceitem, kfmsg.targetname() );
                }
            }
        }
    }

    KFData* KFItemModule::FindIndexItem( KFData* kfitemrecord, uint32 index )
    {
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto findindex = kfitem->Get<uint32>( __STRING__( index ) );
            if ( findindex == index )
            {
                return kfitem;
            }
        }

        return nullptr;
    }

    uint32 KFItemModule::MoveItem( KFEntity* player, KFData* kfsourcerecord, KFData* kfsourceitem, uint64 sourceuuid, KFData* kftargetrecord, uint32 targetindex )
    {
        if ( kftargetrecord->IsFull() )
        {
            return KFMsg::ItemBagFull;
        }

        // 索引判断
        if ( kftargetrecord->_data_setting->_int_logic_value == __USE_ITEM_INDEX__ )
        {
            // 同背包不能相同的索引
            if ( targetindex != 0u )
            {
                if ( targetindex > kftargetrecord->_data_setting->_int_max_value )
                {
                    return KFMsg::ItemIndexError;
                }

                // 判断该索引是否有道具
                if ( FindIndexItem( kftargetrecord, targetindex ) != nullptr )
                {
                    return KFMsg::ItemMoveFailed;
                }
            }
            else
            {
                targetindex = FindItemEmptyIndex( player, kftargetrecord );
            }

            // 原背包索引
            AddItemEmptyIndex( player, kfsourceitem );
            if ( kfsourcerecord == kftargetrecord )
            {
                player->UpdateData( kfsourceitem, __STRING__( index ), KFEnum::Set, targetindex );
                RemoveItemEmptyIndex( player, kfsourceitem );
                return KFMsg::Ok;
            }

            // 设置新的索引
            kfsourceitem->Set( __STRING__( index ), targetindex );
        }

        // 移动
        auto kfitem = player->MoveData( kfsourcerecord, sourceuuid, kftargetrecord );
        if ( kfitem != nullptr )
        {
            RemoveItemEmptyIndex( player, kfitem );
        }

        return KFMsg::Ok;
    }

    void KFItemModule::MoveItemCount( KFEntity* player, KFData* kfitem, uint32 operate, uint32 count )
    {
        auto nowcount = player->MoveData( kfitem, __STRING__( count ), operate, count );
        if ( nowcount == 0u )
        {
            player->RemoveData( kfitem->GetParent(), kfitem->GetKeyID(), false );
        }
    }


    uint32 KFItemModule::MoveItemData( KFEntity* player, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord, const KFItemSetting* kfsetting )
    {
        auto totalmovecount = 0u;
        auto sourcecount = kfsourceitem->Get<uint32>( __STRING__( count ) );

        // 判断是否能堆叠
        if ( !kfsetting->IsOverlay() )
        {
            // 不能堆叠, 直接移动
            auto result = MoveItem( player, kfsourcerecord, kfsourceitem, kfsourceitem->GetKeyID(), kftargetrecord, 0u );
            if ( result == KFMsg::Ok )
            {
                totalmovecount = sourcecount;
            }
        }
        else
        {
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

                MoveItemCount( player, kfsourceitem, KFEnum::Dec, movecount );
                MoveItemCount( player, kfitem, KFEnum::Add, movecount );

                sourcecount -= movecount;
                totalmovecount += movecount;
                if ( sourcecount == 0u )
                {
                    break;
                }
            }

            if ( sourcecount > 0u )
            {
                // 剩下的没有合并完, 直接移动
                auto result = MoveItem( player, kfsourcerecord, kfsourceitem, kfsourceitem->GetKeyID(), kftargetrecord, 0u );
                if ( result == KFMsg::Ok )
                {
                    totalmovecount += sourcecount;
                }
            }
        }

        return totalmovecount;
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

        auto sourceindex = kfsourceitem->Get<uint32>( __STRING__( index ) );
        auto targetindex = kftargetitem->Get<uint32>( __STRING__( index ) );

        if ( kfmsg.sourcename() == kfmsg.targetname() )
        {
            // 交换索引
            player->UpdateData( kfsourceitem, __STRING__( index ), KFEnum::Set, targetindex );
            player->UpdateData( kftargetitem, __STRING__( index ), KFEnum::Set, sourceindex );
        }
        else
        {
            // 判断存放是否限制
            {
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
            }

            // 判断存放是否限制
            {
                auto itemid = kftargetitem->Get<uint32>( kftargetitem->_data_setting->_config_key_name );
                auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
                if ( kfsetting == nullptr )
                {
                    return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
                }

                if ( !CheckItemCanMove( kfsetting, kfmsg.targetname(), kfmsg.sourcename() ) )
                {
                    return _kf_display->SendToClient( player, KFMsg::ItemCanNotStore );
                }
            }

            // 先空出索引
            AddItemEmptyIndex( player, kfsourceitem );
            AddItemEmptyIndex( player, kftargetitem );

            // 交换
            kfsourceitem->Set( __STRING__( index ), targetindex );
            kfsourceitem = player->MoveData( kfsourcerecord, kfmsg.sourceuuid(), kftargetrecord );
            RemoveItemEmptyIndex( player, kfsourceitem );

            kftargetitem->Set( __STRING__( index ), sourceindex );
            kftargetitem = player->MoveData( kftargetrecord, kfmsg.targetuuid(), kfsourcerecord );
            RemoveItemEmptyIndex( player, kftargetitem );
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
                MoveItemData( player, kfsourcerecord, kfsourceitem, kftargetrecord, kfitemsetting );
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

    uint32 KFItemModule::CalcItemAddCount( uint32 sourcecount, uint32 targetcount, uint32 maxcount )
    {
        if ( targetcount >= maxcount )
        {
            return 0u;
        }

        auto canaddcount = maxcount - targetcount;
        return sourcecount <= canaddcount ? sourcecount : canaddcount;
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

    void KFItemModule::UnInitItemEmptyIndexData( KFEntity* player, const std::string& name )
    {
        ItemIndexKey key( player->GetKeyID(), name );
        _player_item_index.Remove( key );
    }

    uint32 KFItemModule::FindItemEmptyIndex( KFEntity* player, KFData* kfitemrecord )
    {
        if ( kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ ||
                kfitemrecord->IsFull() )
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

}