#include "KFTrainCampModule.hpp"

namespace KFrame
{
    void KFTrainCampModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_REMOVE_DATA_1__( __STRING__( traincamp ), &KFTrainCampModule::OnRemoveTrainHero );
        __REGISTER_ENTER_PLAYER__( &KFTrainCampModule::OnEnterTrainCampModule );
        __REGISTER_LEAVE_PLAYER__( &KFTrainCampModule::OnLeaveTrainCampModule );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_CHANGE_REQ, &KFTrainCampModule::HandleTrainChangeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_EXCHANGE_REQ, &KFTrainCampModule::HandleTrainExchangeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_CLEAN_REQ, &KFTrainCampModule::HandleTrainCleanReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_ONEKEY_REQ, &KFTrainCampModule::HandleTrainOnekeyReq );
    }

    void KFTrainCampModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        __UN_REMOVE_DATA_1__( __STRING__( traincamp ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_TRAIN_CHANGE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TRAIN_EXCHANGE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TRAIN_CLEAN_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TRAIN_ONEKEY_REQ );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTrainCampModule::OnRemoveTrainHero )
    {
        // 取消定时器
        __UN_TIMER_2__( player->GetKeyID(), key );

        // 设置标记
        auto uuid = kfdata->Get<uint64>( __STRING__( uuid ) );
        player->UpdateData( __STRING__( hero ), uuid, __STRING__( posflag ), KFEnum::Set, KFMsg::HeroList );
    }

    __KF_MESSAGE_FUNCTION__( KFTrainCampModule::HandleTrainChangeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTrainChangeReq );

        auto kftraincamprecord = player->Find( __STRING__( traincamp ) );
        if ( kfmsg.uuid() == 0u )
        {
            // 将英雄移出栏位
            auto kftraincamp = kftraincamprecord->Find( kfmsg.index() );
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

        player->CleanData( __STRING__( traincamp ) );
    }

    __KF_MESSAGE_FUNCTION__( KFTrainCampModule::HandleTrainOnekeyReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTrainOnekeyReq );

        auto setting = GetTrainCampSetting( player );
        if ( setting == nullptr || setting->_unit_time == 0u || setting->_cd_time == 0u )
        {
            // 训练所配置错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampSettingError );
        }

        auto kftraincamprecord = player->Find( __STRING__( traincamp ) );
        auto kftraincamp = kftraincamprecord->Find( kfmsg.index() );
        if ( kftraincamp == nullptr )
        {
            // 训练栏位置错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampIndexError );
        }

        auto calctime = kftraincamp->Get<uint64>( __STRING__( calctime ) );
        auto endtime = kftraincamp->Get<uint64>( __STRING__( endtime ) );

        if ( endtime <= calctime )
        {
            // 该栏位训练已结束
            return _kf_display->SendToClient( player, KFMsg::TrainCampIsFinished );
        }

        auto consumecount = ( endtime - calctime - 1u ) / setting->_unit_time + 1u;

        // 金钱是否足够
        auto dataname = player->CheckRemoveElement( &setting->_onekey_consume, __FUNC_LINE__, consumecount );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }
        player->RemoveElement( &setting->_onekey_consume, __FUNC_LINE__, consumecount );

        auto addcount = ( endtime - calctime ) / setting->_cd_time;
        if ( addcount == 0u )
        {
            addcount = 1u;
        }
        AddTrainCampHeroExp( player, kftraincamp, addcount, true );

        // 取消定时器
        __UN_TIMER_2__( player->GetKeyID(), kfmsg.index() );

        player->UpdateData( kftraincamp, __STRING__( calctime ), KFEnum::Set, endtime );
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
        auto kfbuild = player->Find( __STRING__( build ), KFMsg::TrainBuild );
        if ( kfbuild == nullptr )
        {
            return 0u;
        }

        return kfbuild->Get( __STRING__( level ) );
    }

    bool KFTrainCampModule::IsTrainCampActive( KFEntity* player )
    {
        auto kfbuild = player->Find( __STRING__( build ), KFMsg::TrainBuild );
        return kfbuild != nullptr;
    }

    KFData* KFTrainCampModule::GetTrainById( KFEntity* player, uint64 uuid )
    {
        auto kftraincamprecord = player->Find( __STRING__( traincamp ) );
        for ( auto kftrain = kftraincamprecord->First(); kftrain != nullptr; kftrain = kftraincamprecord->Next() )
        {
            if ( uuid == kftrain->Get<uint64>( __STRING__( uuid ) ) )
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

        auto kftraincamp = kftarinrecord->Find( index );
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

        auto kfhero = player->Find( __STRING__( hero ), uuid );
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

        auto posflag = kfhero->Get<uint32>( __STRING__( posflag ) );
        if ( posflag != KFMsg::HeroList )
        {
            // 该英雄不在英雄列表
            return _kf_display->SendToClient( player, KFMsg::HeroNotInHeroList );
        }

        // 金钱是否足够
        auto dataname = player->CheckRemoveElement( &setting->_consume, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }
        player->RemoveElement( &setting->_consume, __FUNC_LINE__ );

        // 将英雄加入栏位
        kftraincamp = _kf_kernel->CreateObject( kftarinrecord->_data_setting );
        kftraincamp->Set( __STRING__( uuid ), uuid );
        kftraincamp->Set( __STRING__( calctime ), KFGlobal::Instance()->_real_time );
        kftraincamp->Set( __STRING__( endtime ), KFGlobal::Instance()->_real_time + setting->_total_time );
        player->AddData( kftarinrecord, index, kftraincamp );

        // 启动定时器
        StartTrainCampTimer( player, setting, index, 0u );

        // 更新英雄当前位置
        player->UpdateData( kfhero, __STRING__( posflag ), KFEnum::Set, KFMsg::TrainBuild );
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
            return;
        }

        auto nowtime = KFGlobal::Instance()->_real_time;
        auto kftraincamprecord = player->Find( __STRING__( traincamp ) );
        for ( auto kftrain = kftraincamprecord->First(); kftrain != nullptr; kftrain = kftraincamprecord->Next() )
        {
            auto endtime = kftrain->Get<uint64>( __STRING__( endtime ) );
            auto calctime = kftrain->Get<uint64>( __STRING__( calctime ) );
            auto mintime = __MIN__( nowtime, endtime );

            if ( mintime <= calctime )
            {
                continue;
            }

            auto count = ( mintime - calctime ) / setting->_cd_time;

            auto addexp = AddTrainCampHeroExp( player, kftrain, count );
            if ( ( count != 0 && addexp == 0u ) || nowtime >= endtime )
            {
                player->UpdateData( kftrain, __STRING__( calctime ), KFEnum::Set, endtime );
            }
            else
            {
                calctime = kftrain->Get<uint64>( __STRING__( calctime ) );

                // 启动定时器
                auto delaytime = setting->_cd_time - ( nowtime - calctime ) % setting->_cd_time;
                StartTrainCampTimer( player, setting, kftrain->GetKeyID(), delaytime );
            }
        }
    }

    __KF_TIMER_FUNCTION__( KFTrainCampModule::OnTimerAddExp )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto kftrain = player->Find( __STRING__( traincamp ), subid );
        if ( kftrain == nullptr )
        {
            // 数据错误取消定时器
            return __UN_TIMER_2__( objectid, subid );
        }

        auto addexp = AddTrainCampHeroExp( player, kftrain, 1, true );

        auto calctime = kftrain->Get<uint64>( __STRING__( calctime ) );
        auto endtime = kftrain->Get<uint64>( __STRING__( endtime ) );

        if ( calctime >= endtime || addexp == 0u )
        {
            // 满级取消定时器
            __UN_TIMER_2__( objectid, subid );

            player->UpdateData( kftrain, __STRING__( calctime ), KFEnum::Set, endtime );
        }
    }

    uint32 KFTrainCampModule::AddTrainCampHeroExp( KFEntity* player, KFData* kftrain, uint32 count, bool isnow )
    {
        if ( count == 0u )
        {
            return 0u;
        }

        auto calctime = kftrain->Get<uint64>( __STRING__( calctime ) );
        auto endtime = kftrain->Get<uint64>( __STRING__( endtime ) );
        if ( calctime >= endtime )
        {
            return 0;
        }

        auto setting = GetTrainCampSetting( player );
        if ( setting == nullptr )
        {
            return 0u;
        }

        auto uuid = kftrain->Get<uint64>( __STRING__( uuid ) );
        auto kfhero = player->Find( __STRING__( hero ), uuid );
        if ( kfhero == nullptr )
        {
            return 0u;
        }

        // 原来等级
        auto kflevel = kfhero->Find( __STRING__( level ) );
        auto oldlevel = kflevel->Get();

        // 为当前英雄增加经验
        auto addexp = setting->_add_exp * count;
        addexp = _kf_hero->AddExp( player, kfhero, addexp );
        if ( addexp != 0u )
        {
            if ( isnow )
            {
                calctime = KFGlobal::Instance()->_real_time;
            }
            else
            {
                // 实际增加次数
                count = ( addexp - 1u ) / setting->_add_exp + 1u;
                calctime += count * setting->_cd_time;
            }

            if ( calctime > endtime )
            {
                calctime = endtime;
            }

            // 保存计算经验时间
            player->UpdateData( kftrain, __STRING__( calctime ), KFEnum::Set, calctime );

            // 如果升级了
            auto newlevel = kflevel->Get();
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
        auto upgradetime = kftrain->Get<uint32>( __STRING__( calctime ) );
        if ( !player->IsInited() )
        {
            // 下线期间通过时间升级的, 需要精确计算出升级的时间
            auto kflevelsetting = KFLevelConfig::Instance()->FindSetting( newlevel );
            if ( kflevelsetting == nullptr )
            {
                return;
            }

            // 计算升级需要的经验值
            auto totalexp = kfhero->Get<uint32>( __STRING__( exp ) );

            auto totalcount = ( addexp - 1u ) / kfsetting->_add_exp + 1u;
            auto count = ( kflevelsetting->_exp - ( totalexp - addexp ) - 1u ) / kfsetting->_add_exp + 1u;

            // 推导出升级的具体时间
            upgradetime -= ( totalcount - count ) * kfsetting->_cd_time;
        }

        _kf_record_client->AddCampRecord( player, kfhero, upgradetime, KFMsg::TrainBuild, newlevel );
    }
}