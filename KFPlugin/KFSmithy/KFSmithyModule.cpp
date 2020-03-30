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
        __REGISTER_UPDATE_DATA_2__( __STRING__( effect ), __STRING__( smithycollectmax ), &KFSmithyModule::OnItemNumUpdate );

        __REGISTER_EXECUTE__( __STRING__( smithyaddnum ), &KFSmithyModule::OnExecuteSmithyAddData );
        __REGISTER_EXECUTE__( __STRING__( smithycollectmax ), &KFSmithyModule::OnExecuteSmithyAddData );
        __REGISTER_EXECUTE__( __STRING__( smithystoremax ), &KFSmithyModule::OnExecuteSmithyAddData );
        __REGISTER_EXECUTE__( __STRING__( smithycritrate ), &KFSmithyModule::OnExecuteSmithyAddData );
        __REGISTER_EXECUTE__( __STRING__( smithycritvalue ), &KFSmithyModule::OnExecuteSmithyAddData );
        __REGISTER_EXECUTE__( __STRING__( smithymakemax ), &KFSmithyModule::OnExecuteSmithyAddData );
        __REGISTER_EXECUTE__( __STRING__( smithymakelevel ), &KFSmithyModule::OnExecuteSmithyMakeLevel );

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
        __UN_UPDATE_DATA_2__( __STRING__( effect ), __STRING__( smithycollectmax ) );

        __UN_EXECUTE__( __STRING__( smithyaddnum ) );
        __UN_EXECUTE__( __STRING__( smithycollectmax ) );
        __UN_EXECUTE__( __STRING__( smithystoremax ) );
        __UN_EXECUTE__( __STRING__( smithycritrate ) );
        __UN_EXECUTE__( __STRING__( smithycritvalue ) );
        __UN_EXECUTE__( __STRING__( smithymakemax ) );
        __UN_EXECUTE__( __STRING__( smithymakelevel ) );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SMITHY_GATHER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SMITHY_MAKE_REQ );
    }

    __KF_EXECUTE_FUNCTION__( KFSmithyModule::OnExecuteSmithyAddData )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto adddata = executedata->_param_list._params[0]->_int_value;
        player->UpdateData( __STRING__( effect ), executedata->_name, KFEnum::Add, adddata );
        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFSmithyModule::OnExecuteSmithyMakeLevel )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            return false;
        }

        auto kfrecord = player->Find( __STRING__( effect ), __STRING__( smithymakelevel ) );

        auto makelevel = executedata->_param_list._params[0]->_int_value;
        for ( auto i = 1u; i < executedata->_param_list._params.size(); ++i )
        {
            auto weapontype = executedata->_param_list._params[i]->_int_value;
            if ( weapontype == 0u )
            {
                break;
            }

            player->UpdateData( kfrecord, weapontype, kfrecord->_data_setting->_value_key_name, KFEnum::Add, makelevel );
        }

        return true;
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFSmithyModule::OnEnterSmithyModule )
    {
        // 检查定时器
        CheckSmithyTimer( player );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFSmithyModule::OnLeaveSmithyModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
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

    void KFSmithyModule::CheckSmithyTimer( KFEntity* player )
    {
        // 检查铁匠铺是否开启
        if ( !IsSmithyActive( player ) )
        {
            return;
        }

        auto collectmax = player->Get( __STRING__( effect ), __STRING__( smithycollectmax ) );
        auto kfsmithy = player->Find( __STRING__( smithy ) );
        auto num = kfsmithy->Get<uint32>( __STRING__( num ) );
        if ( num >= collectmax )
        {
            return;
        }

        static auto _option = KFGlobal::Instance()->FindConstant( __STRING__( smithycdtime ) );
        auto cdtime = _option->_uint32_value;
        if ( cdtime == 0u )
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
            auto pasttime = nowtime - calctime;
            if ( pasttime >= cdtime )
            {
                auto addnum = player->Get( __STRING__( effect ), __STRING__( smithyaddnum ) );
                auto count = pasttime / cdtime;
                auto totalnum = count * addnum;
                if ( num + totalnum >= collectmax )
                {
                    player->UpdateData( kfsmithy, __STRING__( num ), KFEnum::Set, collectmax );
                    return;
                }

                calctime += count * cdtime;

                player->UpdateData( kfsmithy, __STRING__( num ), KFEnum::Add, totalnum );
                player->UpdateData( kfsmithy, __STRING__( calctime ), KFEnum::Set, calctime );
            }

            delaytime = ( cdtime - pasttime % cdtime ) * 1000u;
        }

        __LOOP_TIMER_1__( player->GetKeyID(), cdtime * 1000u, delaytime, &KFSmithyModule::OnTimerAddItem );
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
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto storemax = player->Get( __STRING__( effect ), __STRING__( smithystoremax ) );
        if ( storemax == 0u )
        {
            // 未解锁时可获得材料数量
            static auto _option = KFGlobal::Instance()->FindConstant( __STRING__( smithystorenum ) );
            storemax = _option->_uint32_value;
        }

        auto kfsmithy = player->Find( __STRING__( smithy ) );
        auto curnum = kfsmithy->Get<uint32>( __STRING__( totalnum ) );
        if ( curnum >= storemax )
        {
            return false;
        }

        auto kfelementobject = reinterpret_cast<KFElementObject*>( kfelement );
        auto totalnum = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( totalnum ), kfresult->_multiple );

        totalnum = __MIN__( totalnum, storemax - curnum );
        player->UpdateData( kfsmithy, __STRING__( totalnum ), KFEnum::Add, totalnum );

        return kfresult->AddResult( __STRING__( totalnum ), totalnum );
    }

    __KF_ADD_DATA_FUNCTION__( KFSmithyModule::OnAddSmithyBuild )
    {
        // 粮仓功能开启
        CheckSmithyTimer( player );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFSmithyModule::OnItemNumUpdate )
    {
        static auto _option = KFGlobal::Instance()->FindConstant( __STRING__( smithycdtime ) );
        auto cdtime = _option->_uint32_value;
        if ( cdtime == 0u )
        {
            return;
        }

        auto collectmax = player->Get( __STRING__( effect ), __STRING__( smithycollectmax ) );
        if ( collectmax == 0u )
        {
            return;
        }

        auto kfsmithy = player->Find( __STRING__( smithy ) );
        auto num = kfsmithy->Get<uint32>( __STRING__( num ) );
        auto calctime = kfsmithy->Get<uint64>( __STRING__( calctime ) );

        if ( num >= collectmax )
        {
            if ( calctime != 0u )
            {
                // 物品满取消定时器
                __UN_TIMER_1__( player->GetKeyID() );
                player->UpdateData( kfsmithy, __STRING__( calctime ), KFEnum::Set, 0u );
            }
        }
        else
        {
            if ( calctime == 0u )
            {
                // 物品不满开启定时器
                __LOOP_TIMER_1__( player->GetKeyID(), cdtime * 1000, 0u, &KFSmithyModule::OnTimerAddItem );
                player->UpdateData( kfsmithy, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
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

        auto kfsmithy = player->Find( __STRING__( smithy ) );
        auto addnum = kfsmithy->Get<uint32>( __STRING__( num ) );
        if ( addnum == 0u )
        {
            // 没有材料不能收获
            return _kf_display->SendToClient( player, KFMsg::SmithyHaveNotItem );
        }

        auto storemax = player->Get( __STRING__( effect ), __STRING__( smithystoremax ) );
        auto totalnum = kfsmithy->Get<uint32>( __STRING__( totalnum ) );
        if ( totalnum >= storemax )
        {
            // 储存已满
            return _kf_display->SendToClient( player, KFMsg::SmithyStoreIsMax );
        }

        if ( addnum + totalnum > storemax )
        {
            addnum = storemax - totalnum;
            _kf_display->SendToClient( player, KFMsg::SmithyStoreIsMax );
        }
        auto decnum = addnum;

        // 计算暴击率
        uint32 critresult = _invalid_int;
        auto critrate = player->Get( __STRING__( effect ), __STRING__( smithycritrate ) );
        auto critvalue = player->Get( __STRING__( effect ), __STRING__( smithycritvalue ) );
        auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
        if ( rand < critrate )
        {
            addnum += addnum * critvalue / KFRandEnum::TenThousand;
            addnum = __MIN__( addnum, storemax - totalnum );

            critresult = 1u;
        }

        player->UpdateData( kfsmithy, __STRING__( daynum ), KFEnum::Add, addnum );
        player->UpdateData( kfsmithy, __STRING__( daytime ), KFEnum::Set, KFGlobal::Instance()->_real_time );

        // 更新数量
        player->UpdateData( kfsmithy, __STRING__( totalnum ), KFEnum::Add, addnum );
        player->UpdateData( kfsmithy, __STRING__( num ), KFEnum::Dec, decnum );

        // 收取暴击协议
        KFMsg::MsgSmithyGatherAck ack;
        ack.set_crit( critresult );
        _kf_player->SendToClient( player, KFMsg::MSG_SMITHY_GATHER_ACK, &ack );

        static auto _option = KFGlobal::Instance()->FindConstant( __STRING__( smithyitemid ) );

        // 获得道具飘字
        {
            KeyValue values;
            values[ __STRING__( id ) ] = _option->_uint32_value;
            values[ __STRING__( count ) ] = addnum;
            player->AddDataToShow( __STRING__( smithy ), __STRING__( item ), _option->_uint32_value, values, false );
        }

        // 条件更新
        {
            auto kfbuildgather = player->Find( __STRING__( buildgather ) );
            kfbuildgather->Set( __STRING__( id ), _option->_uint32_value );
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

        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( kfmsg.id() );
        if ( kfweaponsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
        }

        auto kflevelrecord = player->Find( __STRING__( effect ), __STRING__( smithymakelevel ) );
        auto kflevel = kflevelrecord->Find( kfitemsetting->_weapon_type );
        if ( kflevel == nullptr || kflevel->Get( kflevelrecord->_data_setting->_value_key_name ) < kfitemsetting->_weapon_level )
        {
            return _kf_display->SendToClient( player, KFMsg::SmithyMakeLevelLimit );
        }

        auto makemax = player->Get( __STRING__( effect ), __STRING__( smithymakemax ) );
        if ( kfmsg.num() == 0u || kfmsg.num() > makemax )
        {
            return _kf_display->SendToClient( player, KFMsg::SmithyMakeNumError );
        }

        {
            auto& dataname = player->CheckAddElement( &kfweaponsetting->_item, kfmsg.num(), __FUNC_LINE__ );
            if ( !dataname.empty() )
            {
                // 背包空间不足
                return _kf_display->SendToClient( player, KFMsg::ItemBagIsFull );
            }
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
        {
            auto& dataname = player->RemoveElement( &kfweaponsetting->_consume, kfmsg.num(), __STRING__( smithymake ), kfweaponsetting->_id, __FUNC_LINE__ );
            if ( !dataname.empty() )
            {
                return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
            }
        }

        // 扣除材料
        player->UpdateData( kfsmithy, __STRING__( totalnum ), KFEnum::Dec, neednum );

        // 增加武器
        player->AddElement( &kfweaponsetting->_item, kfmsg.num(), __STRING__( smithymake ), kfweaponsetting->_id, __FUNC_LINE__ );

        // 发送通知
        _kf_display->SendToClient( player, KFMsg::SmithyMakeWeaponSuc );

        // 条件回调
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

        auto addnum = player->Get( __STRING__( effect ), __STRING__( smithyaddnum ) );
        auto collectmax = player->Get( __STRING__( effect ), __STRING__( smithycollectmax ) );

        auto kfsmithy = player->Find( __STRING__( smithy ) );
        auto curnum = kfsmithy->Get<uint32>( __STRING__( num ) );
        addnum = __MIN__( addnum, collectmax - curnum );

        player->UpdateData( kfsmithy, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
        player->UpdateData( kfsmithy, __STRING__( num ), KFEnum::Add, addnum );
    }
}
