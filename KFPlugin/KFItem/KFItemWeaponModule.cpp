#include "KFItemWeaponModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemWeaponModule::BeforeRun()
    {
        __REGISTER_INIT_ITEM__( KFItemEnum::Weapon, &KFItemWeaponModule::InitWeaponData );

        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA_2__( __STRING__( fighter ), __STRING__( hp ), &KFItemWeaponModule::OnHeroHpUpdate );
        __REGISTER_UPDATE_DATA_2__( __STRING__( weapon ), __STRING__( durability ), &KFItemWeaponModule::OnWeaponDurabilityUpdate );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_WEAPON_REQ, &KFItemWeaponModule::HandleHeroWeaponReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_UNWEAPON_REQ, &KFItemWeaponModule::HandleHeroUnWeaponReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_DURABILITY_REQ, &KFItemWeaponModule::HandleUpdateDurabilityReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_WEAPON_ANOTHER_REQ, &KFItemWeaponModule::HandleHeroWeaponAnotherReq );
    }

    void KFItemWeaponModule::BeforeShut()
    {
        __UN_INIT_ITEM__( KFItemEnum::Weapon );

        __UN_UPDATE_DATA_2__( __STRING__( fighter ), __STRING__( hp ) );
        __UN_UPDATE_DATA_2__( __STRING__( weapon ), __STRING__( durability ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_HERO_WEAPON_REQ );
        __UN_MESSAGE__( KFMsg::MSG_HERO_UNWEAPON_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_DURABILITY_REQ );
        __UN_MESSAGE__( KFMsg::MSG_HERO_WEAPON_ANOTHER_REQ );
    }

    __KF_INIT_ITEM_FUNCTION__( KFItemWeaponModule::InitWeaponData )
    {
        // 耐久
        kfitem->Set( __STRING__( durability ), kfsetting->_durability );

        // 词缀
        auto affixcount = kfsetting->_affix_pool_list.size();
        if ( affixcount > 0u )
        {
            auto kfarray = kfitem->Find( __STRING__( affix ) );
            for ( auto poolid : kfsetting->_affix_pool_list )
            {
                // 随机词缀
                auto weightdata = KFWeightConfig::Instance()->RandWeight( poolid );
                if ( weightdata == nullptr )
                {
                    __LOG_ERROR__( "weightpool=[{}] can't rand weight!", poolid );
                    continue;
                }

                if ( weightdata->_id == 0 )
                {
                    continue;
                }

                _kf_kernel->AddArray( kfarray, weightdata->_id );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemWeaponModule::HandleHeroWeaponReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgHeroWeaponReq );

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.herouuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto kfitemrecord = player->Find( kfmsg.itemname() );
        auto kfitem = kfitemrecord->Find( kfmsg.itemuuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
        auto kfsettting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsettting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
        }

        if ( kfsettting->_type != KFItemEnum::Weapon )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemWeaponOnly );
        }

        // 种族限制
        if ( !kfsettting->_race_limit_list.empty() )
        {
            auto race = kfhero->Get<uint32>( __STRING__( race ) );
            if ( kfsettting->_race_limit_list.find( race ) == kfsettting->_race_limit_list.end() )
            {
                return _kf_display->SendToClient( player, KFMsg::ItemWeaponRaceLimit );
            }
        }

        // 英雄武器限制
        auto weapontype = kfhero->Get<uint32>( __STRING__( weapontype ) );
        if ( kfsettting->_weapon_type != weapontype )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemWeaponHeroLimit );
        }

        // 等级限制
        if ( kfsettting->_level_limit != 0u )
        {
            auto level = kfhero->Get<uint32>( __STRING__( level ) );
            if ( level < kfsettting->_level_limit )
            {
                return _kf_display->SendToClient( player, KFMsg::ItemWeaponLevelLimit );
            }
        }

        // 卸载武器
        player->MoveData( kfhero, __STRING__( weapon ), kfitemrecord );

        // 装备武器
        auto kfweapon = player->MoveData( kfitemrecord, kfmsg.itemuuid(), kfhero, __STRING__( weapon ) );
        if ( kfweapon == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemWeaponFailed );
        }

        // 通知装备成功
        _kf_display->DelayToClient( player, KFMsg::ItemWeaponOk );
    }

    __KF_MESSAGE_FUNCTION__( KFItemWeaponModule::HandleHeroWeaponAnotherReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgHeroWeaponAnotherReq );

        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfsourcehero = kfherorecord->Find( kfmsg.sourceherouuid() );
        auto kftargethero = kfherorecord->Find( kfmsg.targetherouuid() );
        if ( kfsourcehero == nullptr || kftargethero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto kfweapon = kfsourcehero->Find( __STRING__( weapon ) );
        if ( kfweapon == nullptr || kfweapon->GetKeyID() == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemWeaponNotExist );
        }

        // 卸载武器
        player->MoveData( kftargethero, __STRING__( weapon ), __STRING__( weapon ) );

        // 装备武器
        kfweapon = player->MoveData( kfsourcehero, __STRING__( weapon ), kftargethero, __STRING__( weapon ) );
        if ( kfweapon == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemWeaponFailed );
        }

        // 通知装备成功
        _kf_display->DelayToClient( player, KFMsg::ItemWeaponOk );
    }

    __KF_MESSAGE_FUNCTION__( KFItemWeaponModule::HandleHeroUnWeaponReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgHeroUnWeaponReq );

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.herouuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto weaponid = kfhero->Get( __STRING__( weapon ), __STRING__( uuid ) );
        if ( weaponid == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemWeaponNotExist );
        }

        // 判断武器背包是否满了
        auto kfitemrecord = player->Find( __STRING__( weapon ) );
        if ( kfitemrecord->Size() >= kfitemrecord->MaxSize() )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagFull );
        }

        auto kfweapon = player->MoveData( kfhero, __STRING__( weapon ), kfitemrecord );
        if ( kfweapon == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemUnWeaponFailed );
        }

        _kf_display->DelayToClient( player, KFMsg::ItemUnWeaponOk );
    }

    __KF_MESSAGE_FUNCTION__( KFItemWeaponModule::HandleUpdateDurabilityReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateDurabilityReq );

        if ( kfmsg.operate() != KFEnum::Add && kfmsg.operate() != KFEnum::Dec )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemUpdateDurabilityError );
        }

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.herouuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto kfweapon = kfhero->Find( __STRING__( weapon ) );
        auto weaponid = kfweapon->Get( __STRING__( uuid ) );
        if ( weaponid == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemWeaponNotExist );
        }

        player->UpdateData( kfweapon, __STRING__( durability ), kfmsg.operate(), kfmsg.durability() );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemWeaponModule::OnHeroHpUpdate )
    {
        if ( newvalue == 0u )
        {
            // hp为0, 卸下身上的武器
            auto kfhero = kfdata->GetParent()->GetParent();
            player->MoveData( kfhero, __STRING__( weapon ), __STRING__( weapon ) );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemWeaponModule::OnWeaponDurabilityUpdate )
    {
        if ( newvalue == 0u )
        {
            // 删除武器
            auto kfhero = kfdata->GetParent()->GetParent();
            player->RemoveData( kfhero, __STRING__( weapon ) );
        }
    }
}