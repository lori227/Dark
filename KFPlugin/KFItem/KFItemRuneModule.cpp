#include "KFItemRuneModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemRuneModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_ADD_DATA_1__( __STRING__( item ), &KFItemRuneModule::OnAddRuneItemCallBack );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_RUNE_SLOT_MOVE_REQ, &KFItemRuneModule::HandleRuneSlotMoveReq );
    }

    void KFItemRuneModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_ADD_DATA_1__( __STRING__( item ) );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_RUNE_SLOT_MOVE_REQ );
    }

    __KF_ADD_DATA_FUNCTION__( KFItemRuneModule::OnAddRuneItemCallBack )
    {
        if ( true )
        {
            // 暂时屏蔽，避免进入槽位
            return;
        }

        auto itemid = kfdata->Get<uint32>( kfdata->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR__( "id=[{}] itemsetting = null", itemid );
            return;
        }

        if ( kfsetting->_type != KFItemEnum::Rune )
        {
            return;
        }

        // 先尝试合成
        if ( kfsetting->_compound_id != _invalid_int )
        {
            auto kfcompoundsetting = KFCompoundRuneConfig::Instance()->FindSetting( kfsetting->_compound_id );
            if ( kfcompoundsetting == nullptr )
            {
                // 符石道具 合成id非法
                __LOG_ERROR__( "id=[{}] compoundid=[{}] is not exist.", itemid, kfsetting->_compound_id );
                return;
            }

            auto compoundlevel = kfcompoundsetting->GetLevelById( itemid );
            if ( CompoundRuneByLevel( player, kfcompoundsetting, compoundlevel ) )
            {
                __LOG_DEBUG__( "id=[{}] compoundid=[{}] level=[{}] compound succeed.", itemid, kfsetting->_compound_id, compoundlevel );

                // 合成成功后，就代表原有道具被消耗了
                return;
            }
        }

        // 然后尝试自动装填
        if ( kfsetting->_get_auto_set )
        {
            auto kfruneequiprecord = player->Find( __STRING__( runeequips ) );
            if ( !_kf_item->IsItemRecordFull( player, kfruneequiprecord ) )
            {
                if ( IsHasEquipCompoundId( player, kfsetting->_compound_id ) == nullptr )
                {
                    // 放入槽位列表
                    if ( MoveRuneItem( player, kfparent, kfdata, kfruneequiprecord ) == KFMsg::Ok )
                    {
                        __LOG_DEBUG__( "id=[{}] runeitem be equipped.", itemid );

                    }
                    return;
                }
            }
        }

        if ( kfparent->_data_setting->_name == __STRING__( extend ) )
        {
            // 情况1：不是自动装备，并且已经处于满格背包，就丢弃
            // 情况2：自动装备失败，并且已经处于满格背包，就丢弃
            kfparent->Remove( key );

            __LOG_DEBUG__( "id=[{}:{}] be discarded.", itemid, key );
            return;
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemRuneModule::HandleRuneSlotMoveReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRuneSlotMoveReq );

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

        auto sourceitemid = kfsourceitem->Get<uint32>( kfsourceitem->_data_setting->_config_key_name );
        auto kfsourcesetting = KFItemConfig::Instance()->FindSetting( sourceitemid );
        if ( kfsourcesetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
        }

        auto result = MoveRuneItem( player, kfsourcerecord, kfsourceitem, kftargetrecord, kfmsg.targetindex() );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }
    }

    KFData* KFItemRuneModule::IsHasEquipCompoundId( KFEntity* player, uint32 compoundid )
    {
        auto kfruneequiprecord = player->Find( __STRING__( runeequips ) );
        for ( auto kfitem = kfruneequiprecord->First(); kfitem != nullptr; kfitem = kfruneequiprecord->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            auto kfsourcesetting = KFItemConfig::Instance()->FindSetting( itemid );
            if ( kfsourcesetting == nullptr || kfsourcesetting->_compound_id == _invalid_int )
            {
                continue;
            }

            if ( kfsourcesetting->_compound_id == compoundid )
            {
                // 槽位上不存在相同合成id道具
                return kfitem;
            }
        }

        return nullptr;
    }

    uint32 KFItemRuneModule::MoveRuneItem( KFEntity* player, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord, uint32 targetindex /*= 0u */ )
    {
        auto sourceitemid = kfsourceitem->Get<uint32>( kfsourceitem->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( sourceitemid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::ItemSettingNotExist;
        }

        if ( kftargetrecord->_data_setting->_name == __STRING__( runeequips ) )
        {
            if ( IsHasEquipCompoundId( player, kfsetting->_compound_id ) != nullptr )
            {
                return KFMsg::ItemRuneCompondOnly;
            }
        }

        if ( targetindex == 0u )
        {
            targetindex = _kf_item->FindItemEmptyIndex( player, kftargetrecord );
        }
        if ( targetindex == _invalid_int )
        {
            if ( kftargetrecord->_data_setting->_name == __STRING__( runeequips ) )
            {
                return KFMsg::ItemRuneSlotFull;
            }
            else
            {
                return KFMsg::ItemBagIsFull;
            }
        }

        auto itemuuid = kfsourceitem->Get<uint64>( __STRING__( uuid ) );
        /////////////////////////////////////////////////////////////////
        auto kftargetitem = player->CreateData( kfsourceitem );
        kftargetitem->CopyFrom( kfsourceitem );
        kftargetitem->Set( __STRING__( index ), targetindex );
        player->AddData( kftargetrecord, itemuuid, kftargetitem );
        /////////////////////////////////////////////////////////////////
        player->RemoveData( kfsourcerecord, itemuuid );
        /////////////////////////////////////////////////////////////////

        return KFMsg::Ok;
    }

    bool KFItemRuneModule::CompoundAllRune( KFEntity* player )
    {
        auto hadsucceful = false;
        for ( auto kfsetting = KFCompoundRuneConfig::Instance()->_settings.First(); kfsetting != nullptr; kfsetting = KFCompoundRuneConfig::Instance()->_settings.Next() )
        {
            hadsucceful |= CompoundRuneByLevel( player, kfsetting );
        }
        return hadsucceful;
    }

    bool KFItemRuneModule::CompoundRuneByLevel( KFEntity* player, const KFCompoundRuneSetting* setting, uint32 startlevel )
    {
        if ( setting == nullptr || startlevel == _invalid_int )
        {
            return false;
        }

        auto maxlevel = _kf_option->FindOption( "maxrunelevel" );
        if ( maxlevel == nullptr || maxlevel->_uint32_value == _invalid_int )
        {
            return false;
        }

        if ( startlevel + 1 > maxlevel->_uint32_value )
        {
            return false;
        }

        auto inputitemid = setting->GetIdByLevel( startlevel );
        auto outputitemid = setting->GetIdByLevel( startlevel + 1 );
        if ( outputitemid == _invalid_int )
        {
            // 已经不存在更大一级
            return false;
        }

        return CompoundRune( player, inputitemid, outputitemid );
    }

    bool KFItemRuneModule::CompoundRune( KFEntity* player, uint32 inputitemid, uint32 outputitemid )
    {
        auto eachcount = _kf_option->FindOption( "runesynthesisnumb" );
        if ( eachcount == nullptr || eachcount->_uint32_value == 0u )
        {
            __LOG_ERROR__( "eachcount=[{}] can not be 0.", eachcount->_uint32_value );
            return false;
        }

        if ( inputitemid == outputitemid )
        {
            __LOG_ERROR__( "inputitem=[{}] outputitem=[{}] can not same.", inputitemid, outputitemid );
            return false;
        }

        auto inputtotal = 0u;
        std::list< std::tuple< KFData*, uint32 > > compoundlist;

        {
            auto kfrecord = player->Find( __STRING__( runeequips ) );
            auto hascount = _kf_item->GetItemCount( player, kfrecord, inputitemid );
            compoundlist.push_back( std::make_tuple( kfrecord, hascount ) );
            inputtotal += hascount;
        }

        {
            // 因为符文背包满的时候会进入满包，合成的时候也要计算
            auto kfrecord = player->Find( __STRING__( extend ) );
            auto hascount = _kf_item->GetItemCount( player, kfrecord, inputitemid );
            compoundlist.push_back( std::make_tuple( kfrecord, hascount ) );
            inputtotal += hascount;
        }

        {
            auto kfrecord = player->Find( __STRING__( rune ) );
            auto hascount = _kf_item->GetItemCount( player, kfrecord, inputitemid );
            compoundlist.push_back( std::make_tuple( kfrecord, hascount ) );
            inputtotal += hascount;
        }

        if ( inputtotal < eachcount->_uint32_value )
        {
            // 已有个数不满足合成需要
            return false;
        }

        auto modcount = inputtotal % eachcount->_uint32_value;
        auto outputcount = inputtotal / eachcount->_uint32_value;
        inputtotal -= modcount;
        if ( inputtotal == 0u )
        {
            return false;
        }

        // 按顺序扣除消耗
        for ( auto& iter : compoundlist )
        {
            KFData* kfrecord = nullptr;
            uint32 hascount = 0u;
            std::tie( kfrecord, hascount ) = iter;

            if ( hascount == 0u )
            {
                continue;
            }

            if ( inputtotal < hascount )
            {
                hascount = inputtotal;
            }

            _kf_item->RemoveItem( player, kfrecord, inputitemid, hascount );
            inputtotal -= hascount;
        }

        auto consumestr = KFElementConfig::Instance()->StringElemnt( __STRING__( item ), outputcount, outputitemid );
        static KFElements outputelements;
        outputelements.Parse( consumestr, __FUNC_LINE__ );

        // 增加合成道具
        player->AddElement( &outputelements, _default_multiple, __STRING__( compound ), 0u, __FUNC_LINE__ );
        return true;
    }

    bool KFItemRuneModule::ClearRuneSlotData( KFEntity* player )
    {
        return player->CleanData( __STRING__( runeequips ) );
    }

}