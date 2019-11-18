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
        ////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_GRANARY_GATHER_REQ, &KFGranaryModule::HandleGranaryGatherReq );
    }

    void KFGranaryModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        __UN_RECORD_VALUE__();

        __UN_ADD_DATA_2__( __STRING__( build ), KFMsg::WarehouseBuild );
        __UN_UPDATE_DATA_2__( __STRING__( granary ), __STRING__( num ) );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_GRANARY_GATHER_REQ );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFGranaryModule::OnEnterGranaryModule )
    {
        // 检查粮仓是否开启
        if ( !IsGranaryActive( player ) )
        {
            return;
        }

        // 检查定时器
        CheckGranaryTimer( player );
    }

    uint32 KFGranaryModule::GetGranaryLevel( KFEntity* player )
    {
        // 粮仓等级暂时取仓库等级
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

    const KFGranarySetting* KFGranaryModule::GetGranarySetting( KFEntity* player )
    {
        auto level = GetGranaryLevel( player );
        return KFGranaryConfig::Instance()->FindSetting( level );
    }

    void KFGranaryModule::CheckGranaryTimer( KFEntity* player )
    {
        auto setting = GetGranarySetting( player );
        if ( setting == nullptr || setting->_cd_time == 0u )
        {
            return;
        }

        auto kfgranary = player->Find( __STRING__( granary ) );
        auto num = kfgranary->Get<uint32>( __STRING__( num ) );
        if ( num >= setting->_max_num )
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
            auto cdtime = nowtime - calctime;
            if ( cdtime >= setting->_cd_time )
            {
                auto count = cdtime / setting->_cd_time;
                auto addnum = count * setting->_add_num;
                if ( num + addnum >= setting->_max_num )
                {
                    player->UpdateData( kfgranary, __STRING__( num ), KFEnum::Set, setting->_max_num );
                    return;
                }

                calctime += count * setting->_cd_time;
                player->UpdateData( kfgranary, __STRING__( num ), KFEnum::Add, addnum );
                player->UpdateData( kfgranary, __STRING__( calctime ), KFEnum::Set, calctime );
            }

            delaytime = ( setting->_cd_time - ( nowtime - calctime ) % setting->_cd_time ) * 1000u;
        }

        __LOOP_TIMER_1__( player->GetKeyID(), setting->_cd_time * 1000u, delaytime, &KFGranaryModule::OnTimerAddItem );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFGranaryModule::OnLeaveGranaryModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
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
        auto setting = GetGranarySetting( player );
        if ( setting == nullptr )
        {
            return;
        }

        auto kfparent = kfdata->GetParent();
        auto num = kfdata->Get<uint32>();
        auto calctime = kfparent->Get<uint64>( __STRING__( calctime ) );
        if ( num >= setting->_max_num  )
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
                player->UpdateData( kfparent, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );

                // 物品不满开启定时器
                __LOOP_TIMER_1__( player->GetKeyID(), setting->_cd_time * 1000, 0u, &KFGranaryModule::OnTimerAddItem );
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

        auto setting = GetGranarySetting( player );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::GranarySettingError );
        }

        auto kfgranary = player->Find( __STRING__( granary ) );
        auto num = kfgranary->Get( __STRING__( num ) );
        if ( num == 0u )
        {
            // 没有粮食不能收获
            return _kf_display->SendToClient( player, KFMsg::GranaryHaveNotItem );
        }

        static auto _option = _kf_option->FindOption( __STRING__( fooditemid ) );
        auto itemid = _option->_uint32_value;

        // 粮食放进仓库
        auto canaddcount = _kf_item->GetCanAddItemCount( player, itemid, num );
        if ( canaddcount == 0u )
        {
            // 仓库已满
            return _kf_display->SendToClient( player, KFMsg::ItemBagIsFull );
        }

        if ( canaddcount != num )
        {
            // 仓库已满
            _kf_display->SendToClient( player, KFMsg::ItemBagIsFull );
        }

        static KFElements _item_element;
        if ( _item_element.IsEmpty() )
        {
            auto kfelementsetting = KFElementConfig::Instance()->FindElementSetting( __STRING__( item ) );
            auto strelement = __FORMAT__( kfelementsetting->_element_template, __STRING__( item ), 1, itemid );
            auto ok = _item_element.Parse( strelement, __FUNC_LINE__ );
            if ( !ok )
            {
                return _kf_display->SendToClient( player, KFMsg::ElementParseError );
            }
        }

        // 添加物品进背包
        player->AddElement( &_item_element, __STRING__( granary ), __FUNC_LINE__, canaddcount );

        player->UpdateData( kfgranary, __STRING__( daynum ), KFEnum::Add, canaddcount );
        player->UpdateData( kfgranary, __STRING__( daytime ), KFEnum::Set, KFGlobal::Instance()->_real_time );

        // 更新数量
        player->UpdateData( kfgranary, __STRING__( num ), KFEnum::Set, num - canaddcount );
    }

    __KF_TIMER_FUNCTION__( KFGranaryModule::OnTimerAddItem )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto setting = GetGranarySetting( player );
        if ( setting == nullptr )
        {
            return;
        }

        auto kfgranary = player->Find( __STRING__( granary ) );

        auto addnum = setting->_add_num;
        auto curnum = kfgranary->Get<uint32>( __STRING__( num ) );
        if ( addnum + curnum > setting->_max_num )
        {
            addnum = setting->_max_num - curnum;
        }

        player->UpdateData( kfgranary, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
        player->UpdateData( kfgranary, __STRING__( num ), KFEnum::Add, addnum );
    }
}
