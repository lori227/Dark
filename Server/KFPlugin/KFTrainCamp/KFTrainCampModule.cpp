#include "KFTrainCampModule.hpp"

namespace KFrame
{
    void KFTrainCampModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        __REGISTER_REMOVE_DATA__( __KF_STRING__( traincamp ), &KFTrainCampModule::OnRemoveTrainHero );
        __REGISTER_ENTER_PLAYER__( &KFTrainCampModule::OnEnterTrainCampModule );
        __REGISTER_LEAVE_PLAYER__( &KFTrainCampModule::OnLeaveTrainCampModule );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_CHANGE_REQ, &KFTrainCampModule::HandleTrainChangeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_EXCHANGE_REQ, &KFTrainCampModule::HandleTrainExchangeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_CLEAN_REQ, &KFTrainCampModule::HandleTrainCleanReq );
    }

    void KFTrainCampModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();

        __UN_REMOVE_DATA__( __KF_STRING__( traincamp ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_TRAIN_CHANGE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TRAIN_EXCHANGE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TRAIN_CLEAN_REQ );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTrainCampModule::OnRemoveTrainHero )
    {
        // 取消定时器
        __UN_TIMER_2__( player->GetKeyID(), key );

        // 设置标记
        auto uuid = kfdata->GetValue<uint64>( __KF_STRING__( uuid ) );
        player->UpdateData( __KF_STRING__( hero ), uuid, __KF_STRING__( posflag ), KFEnum::Set, KFMsg::HeroList );
    }

    __KF_MESSAGE_FUNCTION__( KFTrainCampModule::HandleTrainChangeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTrainChangeReq );

        auto kftraincamprecord = player->GetData()->FindData( __KF_STRING__( traincamp ) );
        if ( kfmsg.uuid() == 0u )
        {
            // 将英雄移出栏位
            auto kftraincamp = kftraincamprecord->FindData( kfmsg.index() );
            if ( kftraincamp == nullptr )
            {
                // 当前训练栏位没有英雄
                return _kf_display->SendToClient( player, KFMsg::TrainCampHeroNotExist );
            }

            // 将英雄移出栏位
            RemoveTrainCampHero( player, kftraincamprecord, kftraincamp );
        }
        else
        {
            // 增加英雄进栏位
            AddTrainCampHero( player, kftraincamprecord, kfmsg.uuid(), kfmsg.index() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTrainCampModule::HandleTrainExchangeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTrainExchangeReq );
    }

    __KF_MESSAGE_FUNCTION__( KFTrainCampModule::HandleTrainCleanReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTrainCleanReq );

        player->RemoveData( __KF_STRING__( traincamp ) );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFTrainCampModule::OnEnterTrainCampModule )
    {
        if ( !IsTrainCampActive( player ) )
        {
            return;
        }

        // 检查定时器
        EnterStartTrainCampTimer( player );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFTrainCampModule::OnLeaveTrainCampModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    uint32 KFTrainCampModule::GetTrainCampLevel( KFEntity* player )
    {
        auto kflevel = player->GetData()->FindData( __KF_STRING__( buildlevel ), KFMsg::TrainBuild );
        if ( kflevel == nullptr )
        {
            return 0u;
        }

        return kflevel->GetValue();
    }

    bool KFTrainCampModule::IsTrainCampActive( KFEntity* player )
    {
        auto level = GetTrainCampLevel( player );
        return level > 0u ? true : false;
    }

    KFData* KFTrainCampModule::GetTrainById( KFEntity* player, uint64 uuid )
    {
        auto kftraincamprecord = player->GetData()->FindData( __KF_STRING__( traincamp ) );
        for ( auto kftrain = kftraincamprecord->FirstData(); kftrain != nullptr; kftrain = kftraincamprecord->NextData() )
        {
            if ( uuid == kftrain->GetValue<uint64>( __KF_STRING__( uuid ) ) )
            {
                return kftrain;
            }
        }

        return nullptr;
    }

    const KFTrainCampSetting* KFTrainCampModule::GetTrainCampSetting( KFEntity* player )
    {
        auto level = GetTrainCampLevel( player );
        return KFTrainCampConfig::Instance()->FindSetting( level );
    }

    void KFTrainCampModule::RemoveTrainCampHero( KFEntity* player, KFData* kftarinrecord, KFData* kftarin )
    {
        player->RemoveData( kftarinrecord, kftarin->GetKeyID() );
    }

    void KFTrainCampModule::AddTrainCampHero( KFEntity* player, KFData* kftarinrecord, uint64 uuid, uint32 index )
    {
        if ( !IsTrainCampActive( player ) )
        {
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        auto kftraincamp = kftarinrecord->FindData( index );
        if ( kftraincamp != nullptr )
        {
            RemoveTrainCampHero( player, kftarinrecord, kftraincamp );
        }

        auto setting = GetTrainCampSetting( player );
        if ( setting == nullptr )
        {
            // 训练营等级错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampLevelError );
        }

        if ( index == 0u || index > setting->_count )
        {
            // 训练栏位置错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampIndexError );
        }

        auto kfhero = player->GetData()->FindData( __KF_STRING__( hero ), uuid );
        if ( kfhero == nullptr )
        {
            // 查找英雄列表是否存在uuid
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        if ( _kf_hero->IsMaxLevel( kfhero ) )
        {
            // 当前英雄已满级
            return _kf_display->SendToClient( player, KFMsg::HeroLevelIsMax );
        }

        auto posflag = kfhero->GetValue<uint32>( __KF_STRING__( posflag ) );
        if ( posflag != KFMsg::HeroList )
        {
            // 该英雄不在英雄列表
            return _kf_display->SendToClient( player, KFMsg::HeroNotInHeroList );
        }

        // 将英雄加入栏位
        kftraincamp = _kf_kernel->CreateObject( kftarinrecord->_data_setting );
        kftraincamp->SetValue( __KF_STRING__( uuid ), uuid );
        kftraincamp->SetValue( __KF_STRING__( calctime ), KFGlobal::Instance()->_real_time );
        kftraincamp->SetValue( __KF_STRING__( endtime ), KFGlobal::Instance()->_real_time + setting->_total_time );
        player->AddData( kftarinrecord, index, kftraincamp );

        // 启动定时器
        StartTrainCampTimer( player, setting, index, 0u );

        // 更新英雄当前位置
        player->UpdateData( kfhero, __KF_STRING__( posflag ), KFEnum::Set, KFMsg::TrainCamp );
    }

    void KFTrainCampModule::StartTrainCampTimer( KFEntity* player, const KFTrainCampSetting* kfsetting, uint32 index, uint32 delaytime )
    {
        // 启动定时器
        __LOOP_TIMER_2__( player->GetKeyID(), index, kfsetting->_cd_time * 1000, delaytime * 1000, &KFTrainCampModule::OnTimerAddExp );
    }

    void KFTrainCampModule::EnterStartTrainCampTimer( KFEntity* player )
    {
        auto setting = GetTrainCampSetting( player );
        if ( setting == nullptr || setting->_cd_time == 0u )
        {
            return __LOG_ERROR__( "traincamp id=[{}] setting is null", setting->_id );
        }

        std::list<KFData*> removes;
        auto nowtime = KFGlobal::Instance()->_real_time;
        auto kftraincamprecord = player->GetData()->FindData( __KF_STRING__( traincamp ) );
        for ( auto kftrain = kftraincamprecord->FirstData(); kftrain != nullptr; kftrain = kftraincamprecord->NextData() )
        {
            auto endtime = kftrain->GetValue<uint64>( __KF_STRING__( endtime ) );
            auto calctime = kftrain->GetValue<uint64>( __KF_STRING__( calctime ) );
            auto mintime = __MIN__( nowtime, endtime );

            auto count = ( mintime - calctime ) / setting->_cd_time;

            auto addexp = AddTrainCampHeroExp( player, kftrain, count );
            if ( addexp == 0u )
            {
                // 满级不做任何处理
            }
            else if ( nowtime >= endtime )
            {
                removes.push_back( kftrain );
            }
            else
            {
                calctime = kftrain->GetValue<uint64>( __KF_STRING__( calctime ) );

                // 启动定时器
                auto delaytime = setting->_cd_time - ( nowtime - calctime ) % setting->_cd_time;
                StartTrainCampTimer( player, setting, kftrain->GetKeyID(), delaytime );
            }
        }

        for ( auto kftrain : removes )
        {
            // 超时将英雄移出训练栏
            RemoveTrainCampHero( player, kftraincamprecord, kftrain );
        }
    }

    __KF_TIMER_FUNCTION__( KFTrainCampModule::OnTimerAddExp )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto kftrain = player->GetData()->FindData( __KF_STRING__( traincamp ), subid );
        if ( kftrain == nullptr )
        {
            // 数据错误取消定时器
            return __UN_TIMER_2__( objectid, subid );
        }

        auto addexp = AddTrainCampHeroExp( player, kftrain, 1 );
        if ( addexp == 0u )
        {
            // 满级取消定时器
            return __UN_TIMER_2__( objectid, subid );
        }

        if ( KFGlobal::Instance()->_real_time >= kftrain->GetValue<uint64>( __KF_STRING__( endtime ) ) )
        {
            // 超时将英雄移出训练栏
            auto kftarinrecord = player->GetData()->FindData( __KF_STRING__( traincamp ) );
            RemoveTrainCampHero( player, kftarinrecord, kftrain );
        }
    }

    uint32 KFTrainCampModule::AddTrainCampHeroExp( KFEntity* player, KFData* kftrain, uint32 count )
    {
        auto setting = GetTrainCampSetting( player );
        if ( setting == nullptr )
        {
            return 0u;
        }

        auto uuid = kftrain->GetValue<uint64>( __KF_STRING__( uuid ) );
        auto kfhero = player->GetData()->FindData( __KF_STRING__( hero ), uuid );
        if ( kfhero == nullptr )
        {
            return 0u;
        }

        // 原来等级
        auto kflevel = kfhero->FindData( __KF_STRING__( level ) );
        auto oldlevel = kflevel->GetValue();

        // 为当前英雄增加经验
        auto addexp = setting->_add_exp * count;
        addexp = _kf_hero->AddExp( player, kfhero, addexp );
        if ( addexp != 0u )
        {
            auto calctime = kftrain->GetValue<uint64>( __KF_STRING__( calctime ) );
            if ( calctime == 0u )
            {
                calctime = KFGlobal::Instance()->_real_time;
            }
            else
            {
                // 实际增加次数
                count = ( addexp - 1u ) / setting->_add_exp + 1u;
                calctime += count * setting->_cd_time;
            }

            // 保存计算经验时间
            player->UpdateData( kftrain, __KF_STRING__( calctime ), KFEnum::Set, calctime );

            // 如果升级了
            auto newlevel = kflevel->GetValue();
            if ( newlevel > oldlevel )
            {
                // 发送纪录
                AddTrainHeroLevelRecord( player, setting, kftrain, kfhero, newlevel, addexp );
            }
        }

        return addexp;
    }

    void KFTrainCampModule::AddTrainHeroLevelRecord( KFEntity* player, const KFTrainCampSetting* kfsetting, KFData* kftrain, KFData* kfhero, uint32 newlevel, uint32 addexp )
    {
        auto upgradetime = kftrain->GetValue<uint32>( __KF_STRING__( calctime ) );
        if ( !player->IsInited() )
        {
            // 下线期间通过时间升级的, 需要精确计算出升级的时间( 策划需求, 我的天 )
            auto kflevelsetting = KFLevelConfig::Instance()->FindSetting( newlevel );
            if ( kflevelsetting == nullptr )
            {
                return;
            }

            // 计算升级需要的经验值
            auto totalexp = kfhero->GetValue<uint32>( __KF_STRING__( exp ) );

            auto totalcount = ( addexp - 1u ) / kfsetting->_add_exp + 1u;
            auto count = ( kflevelsetting->_exp - ( totalexp - addexp ) - 1u ) / kfsetting->_add_exp + 1u;

            // 推导出升级的具体时间
            upgradetime -= ( totalcount - count ) * kfsetting->_cd_time;
        }

        _kf_record_client->AddCampRecord( player, kfhero, upgradetime, KFMsg::TrainBuild, newlevel );
    }
}