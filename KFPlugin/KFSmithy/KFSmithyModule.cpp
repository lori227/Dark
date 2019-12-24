#include "KFSmithyModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFSmithyModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFSmithyModule::OnEnterSmithyModule );
        __REGISTER_LEAVE_PLAYER__( &KFSmithyModule::OnLeaveSmithyModule );

        __REGISTER_RECORD_VALUE__( &KFSmithyModule::GetDaySmithyTotalNum );

        __REGISTER_ADD_ELEMENT__( __STRING__( smithy ), &KFSmithyModule::AddSmithyElement );
        __REGISTER_ADD_DATA_2__( __STRING__( build ), KFMsg::SmithyBuild, &KFSmithyModule::OnAddSmithyBuild );
        __REGISTER_UPDATE_DATA_2__( __STRING__( smithy ), __STRING__( num ), &KFSmithyModule::OnItemNumUpdate );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_SMITHY_GATHER_REQ, &KFSmithyModule::HandleSmithyGatherReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SMITHY_MAKE_REQ, &KFSmithyModule::HandleSmithyMakeReq );
    }

    void KFSmithyModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();

        __UN_RECORD_VALUE__();

        __UN_ADD_ELEMENT__( __STRING__( smithy ) );
        __UN_ADD_DATA_2__( __STRING__( build ), KFMsg::SmithyBuild );
        __UN_UPDATE_DATA_2__( __STRING__( smithy ), __STRING__( num ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SMITHY_GATHER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SMITHY_MAKE_REQ );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFSmithyModule::OnEnterSmithyModule )
    {
        // 检查铁匠铺是否开启
        if ( !IsSmithyActive( player ) )
        {
            return;
        }

        // 检查定时器
        CheckSmithyTimer( player );
    }

    uint32 KFSmithyModule::GetSmithyLevel( KFEntity* player )
    {
        auto kfbuild = player->Find( __STRING__( build ), KFMsg::SmithyBuild );
        if ( kfbuild == nullptr )
        {
            return 0u;
        }

        return kfbuild->Get( __STRING__( level ) );
    }

    bool KFSmithyModule::IsSmithyActive( KFEntity* player )
    {
        auto level = GetSmithyLevel( player );
        return level > 0u ? true : false;
    }

    const KFSmithySetting* KFSmithyModule::GetSmithySetting( KFEntity* player )
    {
        auto level = GetSmithyLevel( player );
        return KFSmithyConfig::Instance()->FindSetting( level );
    }

    void KFSmithyModule::CheckSmithyTimer( KFEntity* player )
    {
        auto setting = GetSmithySetting( player );
        if ( setting == nullptr || setting->_cd_time == 0u )
        {
            return;
        }

        auto kfsmithy = player->Find( __STRING__( smithy ) );
        auto num = kfsmithy->Get<uint32>( __STRING__( num ) );
        if ( num >= setting->_collect_max )
        {
            return;
        }

        auto nowtime = KFGlobal::Instance()->_real_time;
        auto calctime = kfsmithy->Get<uint64>( __STRING__( calctime ) );
        auto delaytime = 0u;

        if ( calctime == 0u )
        {
            player->UpdateData( kfsmithy, __STRING__( calctime ), KFEnum::Set, nowtime );
        }
        else
        {
            auto cdtime = nowtime - calctime;
            if ( cdtime >= setting->_cd_time )
            {
                auto count = cdtime / setting->_cd_time;
                auto addnum = count * setting->_add_num;
                if ( num + addnum >= setting->_collect_max )
                {
                    player->UpdateData( kfsmithy, __STRING__( num ), KFEnum::Set, setting->_collect_max );
                    return;
                }

                calctime += count * setting->_cd_time;

                player->UpdateData( kfsmithy, __STRING__( num ), KFEnum::Add, addnum );
                player->UpdateData( kfsmithy, __STRING__( calctime ), KFEnum::Set, calctime );
            }

            delaytime = ( setting->_cd_time - ( nowtime - calctime ) % setting->_cd_time ) * 1000u;
        }

        __LOOP_TIMER_1__( player->GetKeyID(), setting->_cd_time * 1000u, delaytime, &KFSmithyModule::OnTimerAddItem );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFSmithyModule::OnLeaveSmithyModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    __KF_RECORD_VALUE_FUNCTION__( KFSmithyModule::GetDaySmithyTotalNum )
    {
        if ( !IsSmithyActive( player ) )
        {
            return;
        }

        auto kfsmithy = player->Find( __STRING__( smithy ) );
        auto daynum = kfsmithy->Get<uint32>( __STRING__( daynum ) );
        if ( daynum == 0u )
        {
            return;
        }

        if ( !player->IsInited() )
        {
            // 上线逻辑
            auto daytime = kfsmithy->Get<uint64>( __STRING__( daytime ) );
            if ( daytime < lasttime || daytime > nowtime )
            {
                return;
            }
        }

        // 添加入日志列表
        dbvalue.AddValue( __STRING__( smithynum ), daynum );

        // 更新数量
        player->UpdateData( kfsmithy, __STRING__( daynum ), KFEnum::Set, 0u );
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFSmithyModule::AddSmithyElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
        }

        // 未解锁时获得材料取1级的数据
        auto level = GetSmithyLevel( player );
        auto kfsetting = KFSmithyConfig::Instance()->FindSetting( __MAX__( 1u, level ) );
        if ( kfsetting == nullptr )
        {
            return;
        }

        auto kfsmithy = player->Find( __STRING__( smithy ) );
        auto curnum = kfsmithy->Get<uint32>( __STRING__( totalnum ) );

        auto kfelementobject = reinterpret_cast<KFElementObject*>( kfelement );
        auto totalnum = kfelementobject->CalcValue( kfparent->_class_setting, __STRING__( totalnum ), multiple );

        totalnum = __MIN__( totalnum, kfsetting->_store_max - curnum );
        player->UpdateData( kfsmithy, __STRING__( totalnum ), KFEnum::Add, totalnum );
    }

    __KF_ADD_DATA_FUNCTION__( KFSmithyModule::OnAddSmithyBuild )
    {
        // 粮仓功能开启
        CheckSmithyTimer( player );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFSmithyModule::OnItemNumUpdate )
    {
        auto setting = GetSmithySetting( player );
        if ( setting == nullptr )
        {
            return;
        }

        auto kfparent = kfdata->GetParent();
        auto num = kfdata->Get<uint32>();
        auto calctime = kfparent->Get<uint64>( __STRING__( calctime ) );
        if ( num >= setting->_collect_max )
        {
            if ( calctime != 0u )
            {
                // 物品满取消定时器
                __UN_TIMER_1__( player->GetKeyID() );
                player->UpdateData( kfparent, __STRING__( calctime ), KFEnum::Set, 0u );
            }
        }
        else
        {
            if ( calctime == 0u )
            {
                // 物品不满开启定时器
                __LOOP_TIMER_1__( player->GetKeyID(), setting->_cd_time * 1000, 0u, &KFSmithyModule::OnTimerAddItem );
                player->UpdateData( kfparent, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFSmithyModule::HandleSmithyGatherReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSmithyGatherReq );

        if ( !IsSmithyActive( player ) )
        {
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        auto setting = GetSmithySetting( player );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::SmithySettingError );
        }

        auto kfsmithy = player->Find( __STRING__( smithy ) );
        auto num = kfsmithy->Get<uint32>( __STRING__( num ) );
        if ( num == 0u )
        {
            // 没有材料不能收获
            return _kf_display->SendToClient( player, KFMsg::SmithyHaveNotItem );
        }

        auto totalnum = kfsmithy->Get<uint32>( __STRING__( totalnum ) );
        if ( totalnum >= setting->_store_max )
        {
            // 储存已满
            return _kf_display->SendToClient( player, KFMsg::SmithyStoreIsMax );
        }

        auto addnum = num;
        if ( num + totalnum > setting->_store_max )
        {
            addnum = setting->_store_max - totalnum;
            _kf_display->SendToClient( player, KFMsg::SmithyStoreIsMax );
        }

        player->UpdateData( kfsmithy, __STRING__( daynum ), KFEnum::Add, addnum );
        player->UpdateData( kfsmithy, __STRING__( daytime ), KFEnum::Set, KFGlobal::Instance()->_real_time );

        // 更新数量
        player->UpdateData( kfsmithy, __STRING__( totalnum ), KFEnum::Add, addnum );
        player->UpdateData( kfsmithy, __STRING__( num ), KFEnum::Dec, addnum );

        // 获得道具飘字
        {
            KeyValue values;
            values[ __STRING__( id ) ] = setting->_item_id;
            values[ __STRING__( count ) ] = addnum;
            player->AddDataToShow( __STRING__( smithy ), __STRING__( item ), setting->_item_id, values, false );
        }

        // 条件更新
        {
            auto kfbuildgather = player->Find( __STRING__( buildgather ) );
            kfbuildgather->Set( __STRING__( id ), setting->_item_id );
            player->UpdateData( kfbuildgather, __STRING__( count ), KFEnum::Set, addnum );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFSmithyModule::HandleSmithyMakeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSmithyMakeReq );

        if ( !IsSmithyActive( player ) )
        {
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        auto kfweaponsetting = KFSmithyWeaponConfig::Instance()->FindSetting( kfmsg.id() );
        if ( kfweaponsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::SmithyWeaponSettingError );
        }

        auto kfsmithysetting = GetSmithySetting( player );
        if ( kfsmithysetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::SmithySettingError );
        }

        if ( kfmsg.num() == 0u || kfmsg.num() > kfsmithysetting->_make_max )
        {
            return _kf_display->SendToClient( player, KFMsg::SmithyMakeNumError );
        }

        auto dataname = player->CheckAddElement( &kfweaponsetting->_item, __FUNC_LINE__, kfmsg.num() );
        if ( !dataname.empty() )
        {
            // 背包空间不足
            return _kf_display->SendToClient( player, KFMsg::ItemBagIsFull );
        }

        // 所需材料总数
        auto kfsmithy = player->Find( __STRING__( smithy ) );
        auto totalnum = kfsmithy->Get<uint32>( __STRING__( totalnum ) );
        auto neednum = kfweaponsetting->_need_num * kfmsg.num();
        if ( neednum > totalnum )
        {
            return _kf_display->SendToClient( player, KFMsg::SmithyMakeNumNotEnough );
        }

        // 消耗是否足够
        dataname = player->CheckRemoveElement( &kfweaponsetting->_consume, __FUNC_LINE__, kfmsg.num() );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }
        player->RemoveElement( &kfweaponsetting->_consume, __STRING__( smithymake ), __FUNC_LINE__, kfmsg.num() );

        // 扣除材料
        player->UpdateData( kfsmithy, __STRING__( totalnum ), KFEnum::Dec, neednum );

        // 增加武器
        player->AddElement( &kfweaponsetting->_item, __STRING__( smithymake ), __FUNC_LINE__, kfmsg.num() );

        // 发送通知
        _kf_display->SendToClient( player, KFMsg::SmithyMakeWeaponSuc );

        // 条件回调
        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( kfweaponsetting->_id );
        if ( kfitemsetting != nullptr )
        {
            auto kfsmithyweapon = player->Find( __STRING__( smithyweapon ) );
            kfsmithyweapon->Set( __STRING__( id ), kfitemsetting->_id );
            kfsmithyweapon->Set( __STRING__( type ), kfitemsetting->_weapon_type );
            kfsmithyweapon->Set( __STRING__( level ), kfitemsetting->_weapon_level );
            player->UpdateData( kfsmithyweapon, __STRING__( count ), KFEnum::Set, kfmsg.num() );
        }
    }

    __KF_TIMER_FUNCTION__( KFSmithyModule::OnTimerAddItem )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto setting = GetSmithySetting( player );
        if ( setting == nullptr )
        {
            return;
        }

        auto kfsmithy = player->Find( __STRING__( smithy ) );

        auto addnum = setting->_add_num;
        auto curnum = kfsmithy->Get<uint32>( __STRING__( num ) );
        if ( addnum + curnum > setting->_collect_max )
        {
            addnum = setting->_collect_max - curnum;
        }

        player->UpdateData( kfsmithy, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
        player->UpdateData( kfsmithy, __STRING__( num ), KFEnum::Add, addnum );
    }
}
