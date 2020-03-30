#include "KFGranaryModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFGranaryModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFGranaryModule::OnEnterGranaryModule );
        __REGISTER_LEAVE_PLAYER__( &KFGranaryModule::OnLeaveGranaryModule );
        __REGISTER_RECORD_VALUE__( &KFGranaryModule::GetDayGranaryTotalNum );

        __REGISTER_ADD_DATA_2__( __STRING__( build ), KFMsg::WarehouseBuild, &KFGranaryModule::OnAddGranaryBuild );
        __REGISTER_UPDATE_DATA_2__( __STRING__( granary ), __STRING__( num ), &KFGranaryModule::OnItemNumUpdate );
        __REGISTER_UPDATE_DATA_2__( __STRING__( effect ), __STRING__( granarymaxnum ), &KFGranaryModule::OnItemNumUpdate );

        __REGISTER_EXECUTE__( __STRING__( granaryaddnum ), &KFGranaryModule::OnExecuteGranaryAddData );
        __REGISTER_EXECUTE__( __STRING__( granarymaxnum ), &KFGranaryModule::OnExecuteGranaryAddData );
        __REGISTER_EXECUTE__( __STRING__( granarybuycount ), &KFGranaryModule::OnExecuteGranaryAddData );
        __REGISTER_EXECUTE__( __STRING__( granarybuyfactor ), &KFGranaryModule::OnExecuteGranaryAddData );
        __REGISTER_EXECUTE__( __STRING__( granarycritrate ), &KFGranaryModule::OnExecuteGranaryAddData );
        __REGISTER_EXECUTE__( __STRING__( granarycritvalue ), &KFGranaryModule::OnExecuteGranaryAddData );

        ////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_GRANARY_GATHER_REQ, &KFGranaryModule::HandleGranaryGatherReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_GRANARY_BUY_REQ, &KFGranaryModule::HandleGranaryBuyReq );
    }

    void KFGranaryModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        __UN_RECORD_VALUE__();

        __UN_ADD_DATA_2__( __STRING__( build ), KFMsg::WarehouseBuild );
        __UN_UPDATE_DATA_2__( __STRING__( granary ), __STRING__( num ) );
        __UN_UPDATE_DATA_2__( __STRING__( effect ), __STRING__( granarymaxnum ) );

        __UN_EXECUTE__( __STRING__( granaryaddnum ) );
        __UN_EXECUTE__( __STRING__( granarymaxnum ) );
        __UN_EXECUTE__( __STRING__( granarybuycount ) );
        __UN_EXECUTE__( __STRING__( granarybuyfactor ) );
        __UN_EXECUTE__( __STRING__( granarycritrate ) );
        __UN_EXECUTE__( __STRING__( granarycritvalue ) );

        ////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_GRANARY_GATHER_REQ );
        __UN_MESSAGE__( KFMsg::MSG_GRANARY_BUY_REQ );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFGranaryModule::OnEnterGranaryModule )
    {
        // 检查定时器
        CheckGranaryTimer( player );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFGranaryModule::OnLeaveGranaryModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    __KF_EXECUTE_FUNCTION__( KFGranaryModule::OnExecuteGranaryAddData )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto adddata = executedata->_param_list._params[0]->_int_value;
        player->UpdateData( __STRING__( effect ), executedata->_name, KFEnum::Add, adddata );
        return true;
    }

    uint32 KFGranaryModule::GetGranaryLevel( KFEntity* player )
    {
        auto kfbuild = player->Find( __STRING__( build ), KFMsg::WarehouseBuild );
        if ( kfbuild == nullptr )
        {
            return 0u;
        }

        return kfbuild->Get( __STRING__( level ) );
    }

    bool KFGranaryModule::IsGranaryActive( KFEntity* player )
    {
        auto kfbuild = player->Find( __STRING__( build ), KFMsg::WarehouseBuild );
        return kfbuild != nullptr;
    }

    bool KFGranaryModule::IsGranaryCrit( KFEntity* player, uint32& itemnum )
    {
        // 计算暴击率
        auto critrate = player->Get( __STRING__( effect ), __STRING__( granarycritrate ) );
        auto critvalue = player->Get( __STRING__( effect ), __STRING__( granarycritvalue ) );
        auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
        if ( rand < critrate )
        {
            itemnum += itemnum * critvalue / KFRandEnum::TenThousand;
            return true;
        }

        return false;
    }

    void KFGranaryModule::SendGranaryCritMsg( KFEntity* player, bool result )
    {
        // 收取暴击协议
        KFMsg::MsgGranaryGatherAck ack;
        ack.set_crit( result );
        _kf_player->SendToClient( player, KFMsg::MSG_GRANARY_GATHER_ACK, &ack );
    }

    void KFGranaryModule::CheckGranaryTimer( KFEntity* player )
    {
        // 检查粮仓是否开启
        if ( !IsGranaryActive( player ) )
        {
            return;
        }

        static auto _option = KFGlobal::Instance()->FindConstant( __STRING__( granarycdtime ) );
        auto cdtime = _option->_uint32_value;
        if ( cdtime == 0u )
        {
            return;
        }

        auto addnum = player->Get<uint32>( __STRING__( effect ), __STRING__( granaryaddnum ) );
        auto maxnum = player->Get<uint32>( __STRING__( effect ), __STRING__( granarymaxnum ) );

        auto kfgranary = player->Find( __STRING__( granary ) );
        auto num = kfgranary->Get<uint32>( __STRING__( num ) );
        if ( num >= maxnum )
        {
            return;
        }

        auto nowtime = KFGlobal::Instance()->_real_time;
        auto calctime = kfgranary->Get<uint64>( __STRING__( calctime ) );
        auto delaytime = 0u;

        if ( calctime == 0u )
        {
            player->UpdateData( kfgranary, __STRING__( calctime ), KFEnum::Set, nowtime );
        }
        else
        {
            auto pasttime = nowtime - calctime;
            if ( pasttime >= cdtime )
            {
                auto count = pasttime / cdtime;
                auto totalnum = count * addnum;
                if ( num + totalnum >= maxnum )
                {
                    player->UpdateData( kfgranary, __STRING__( num ), KFEnum::Set, maxnum );
                    return;
                }

                calctime += count * cdtime;
                player->UpdateData( kfgranary, __STRING__( num ), KFEnum::Add, totalnum );
                player->UpdateData( kfgranary, __STRING__( calctime ), KFEnum::Set, calctime );
            }

            delaytime = ( cdtime - pasttime % cdtime ) * 1000u;
        }

        __LOOP_TIMER_1__( player->GetKeyID(), cdtime * 1000u, delaytime, &KFGranaryModule::OnTimerAddItem );
    }

    __KF_RECORD_VALUE_FUNCTION__( KFGranaryModule::GetDayGranaryTotalNum )
    {
        if ( !IsGranaryActive( player ) )
        {
            return;
        }

        auto kfgranary = player->Find( __STRING__( granary ) );
        auto daynum = kfgranary->Get<uint32>( __STRING__( daynum ) );
        if ( daynum == 0u )
        {
            return;
        }

        if ( player->IsInited() )
        {
            // 零点逻辑
        }
        else
        {
            // 上线逻辑
            auto daytime = kfgranary->Get<uint64>( __STRING__( daytime ) );
            if ( daytime < lasttime || daytime > nowtime )
            {
                return;
            }
        }

        player->UpdateData( kfgranary, __STRING__( daynum ), KFEnum::Set, 0u );

        dbvalue.AddValue( __STRING__( granarynum ), daynum );
    }

    __KF_ADD_DATA_FUNCTION__( KFGranaryModule::OnAddGranaryBuild )
    {
        // 粮仓功能开启
        CheckGranaryTimer( player );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFGranaryModule::OnItemNumUpdate )
    {
        static auto _option = KFGlobal::Instance()->FindConstant( __STRING__( granarycdtime ) );
        auto cdtime = _option->_uint32_value;
        if ( cdtime == 0u )
        {
            return;
        }

        auto maxnum = player->Get<uint32>( __STRING__( effect ), __STRING__( granarymaxnum ) );
        if ( maxnum == 0u )
        {
            return;
        }

        auto kfgranary = player->Find( __STRING__( granary ) );
        auto num = kfgranary->Get<uint32>( __STRING__( num ) );
        auto calctime = kfgranary->Get<uint64>( __STRING__( calctime ) );
        if ( num >= maxnum )
        {
            if ( calctime != 0u )
            {
                // 物品满取消定时器
                __UN_TIMER_1__( player->GetKeyID() );

                player->UpdateData( kfgranary, __STRING__( calctime ), KFEnum::Set, 0u );
            }
        }
        else
        {
            if ( calctime == 0u )
            {
                player->UpdateData( kfgranary, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );

                // 物品不满开启定时器
                __LOOP_TIMER_1__( player->GetKeyID(), cdtime * 1000, 0u, &KFGranaryModule::OnTimerAddItem );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGranaryModule::HandleGranaryGatherReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgGranaryGatherReq );

        if ( !IsGranaryActive( player ) )
        {
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        auto kfgranary = player->Find( __STRING__( granary ) );
        auto num = kfgranary->Get<uint32>( __STRING__( num ) );
        if ( num == 0u )
        {
            // 没有粮食不能收获
            return _kf_display->SendToClient( player, KFMsg::GranaryHaveNotItem );
        }

        static auto _option = KFGlobal::Instance()->FindConstant( __STRING__( fooditemid ) );
        auto itemid = _option->_uint32_value;

        static KFElements _item_element;
        if ( _item_element.IsEmpty() )
        {
            auto ok = KFElementConfig::Instance()->FormatElemnt( _item_element, __STRING__( item ), "1", itemid );
            if ( !ok )
            {
                return _kf_display->SendToClient( player, KFMsg::ElementParseError );
            }
        }

        auto totalcount = num;
        auto critresult = IsGranaryCrit( player, totalcount );

        // 粮食放进仓库
        auto canaddcount = _kf_item->GetCanAddItemCount( player, itemid, totalcount );
        if ( canaddcount == 0u )
        {
            // 仓库已满
            return _kf_display->SendToClient( player, KFMsg::ItemBagIsFull );
        }

        if ( canaddcount != totalcount )
        {
            // 仓库已满
            _kf_display->SendToClient( player, KFMsg::ItemBagIsFull );
        }

        SendGranaryCritMsg( player, critresult );

        // 添加物品进背包
        player->AddElement( &_item_element, canaddcount, __STRING__( granary ), 0u, __FUNC_LINE__ );

        player->UpdateData( kfgranary, __STRING__( daynum ), KFEnum::Add, canaddcount );
        player->UpdateData( kfgranary, __STRING__( daytime ), KFEnum::Set, KFGlobal::Instance()->_real_time );

        // 更新数量
        auto leftnum = ( ( num > canaddcount ) ? ( num - canaddcount ) : 0u );
        player->UpdateData( kfgranary, __STRING__( num ), KFEnum::Set, leftnum );

        // 条件更新
        {
            auto kfbuildgather = player->Find( __STRING__( buildgather ) );
            kfbuildgather->Set( __STRING__( id ), itemid );
            player->UpdateData( kfbuildgather, __STRING__( count ), KFEnum::Set, canaddcount );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGranaryModule::HandleGranaryBuyReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgGranaryBuyReq );

        // 已购买次数
        auto buycount = player->Get<uint32>( __STRING__( granary ), __STRING__( buycount ) );
        static auto _buy_option = KFGlobal::Instance()->FindConstant( __STRING__( granaryfreebuycount ) );
        auto freecount = _buy_option->_uint32_value;

        // 付费购买次数
        auto paycount = player->Get<uint32>( __STRING__( effect ), __STRING__( granarybuycount ) );

        if ( buycount >= freecount + paycount )
        {
            return _kf_display->SendToClient( player, KFMsg::GranaryBuyCountLimit );
        }

        static auto _item_option = KFGlobal::Instance()->FindConstant( __STRING__( fooditemid ) );
        auto itemid = _item_option->_uint32_value;

        static KFElements _item_element;
        if ( _item_element.IsEmpty() )
        {
            auto ok = KFElementConfig::Instance()->FormatElemnt( _item_element, __STRING__( item ), "1", itemid );
            if ( !ok )
            {
                return _kf_display->SendToClient( player, KFMsg::ElementParseError );
            }
        }

        auto addnum = player->Get<uint32>( __STRING__( effect ), __STRING__( granaryaddnum ) );
        auto buyfactor = player->Get<uint32>( __STRING__( effect ), __STRING__( granarybuyfactor ) );

        static auto _time_option = KFGlobal::Instance()->FindConstant( __STRING__( granarycdtime ) );
        auto cdtime = _time_option->_uint32_value;
        if ( addnum == 0u || cdtime == 0u || buyfactor == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::GranaryBuyParamError );
        }

        // 判断仓库是否已满
        auto finalcount = static_cast<double>( addnum ) / cdtime * buyfactor * KFTimeEnum::OneMinuteSecond;
        auto totalcount = static_cast<uint32>( finalcount + 0.5 );
        if ( totalcount == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::GranaryBuyParamError );
        }

        auto critresult = IsGranaryCrit( player, totalcount );

        auto canaddcount = _kf_item->GetCanAddItemCount( player, itemid, totalcount );
        if ( canaddcount == 0u )
        {
            // 仓库已满
            return _kf_display->SendToClient( player, KFMsg::ItemBagIsFull );
        }

        SendGranaryCritMsg( player, critresult );

        if ( buycount >= freecount )
        {
            auto count = buycount - freecount + 1u;
            auto kfgranarysetting = KFGranaryConfig::Instance()->FindSetting( count );
            if ( kfgranarysetting == nullptr )
            {
                return _kf_display->SendToClient( player, KFMsg::GranaryBuySettingError );
            }

            // 付费购买
            auto& dataname = player->RemoveElement( &kfgranarysetting->_consume, _default_multiple, __STRING__( granarybuy ), kfgranarysetting->_id, __FUNC_LINE__ );
            if ( !dataname.empty() )
            {
                return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
            }

        }

        // 添加粮食进背包
        player->AddElement( &_item_element, canaddcount, __STRING__( granary ), 0u, __FUNC_LINE__ );

        player->UpdateData( __STRING__( granary ), __STRING__( buycount ), KFEnum::Add, 1u );
    }

    __KF_TIMER_FUNCTION__( KFGranaryModule::OnTimerAddItem )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto addnum = player->Get<uint32>( __STRING__( effect ), __STRING__( granaryaddnum ) );
        auto maxnum = player->Get<uint32>( __STRING__( effect ), __STRING__( granarymaxnum ) );
        if ( addnum == 0u || maxnum == 0u )
        {
            return;
        }

        auto kfgranary = player->Find( __STRING__( granary ) );
        auto curnum = kfgranary->Get<uint32>( __STRING__( num ) );
        addnum = __MIN__( addnum, maxnum - curnum );

        player->UpdateData( kfgranary, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
        player->UpdateData( kfgranary, __STRING__( num ), KFEnum::Add, addnum );
    }
}
