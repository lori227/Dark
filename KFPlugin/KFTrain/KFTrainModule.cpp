﻿#include "KFTrainModule.hpp"

namespace KFrame
{
#define __KF_SET_STATIC_OPTION_TRAIN__( optionname, optionvalue, classvar)  \
    {   \
        static auto _static_var = KFGlobal::Instance()->FindConstant( optionname ); \
        if ( _static_var->optionvalue != classvar )  \
        {   \
            classvar = _static_var->optionvalue;   \
        }   \
    }   \

    void KFTrainSetting::UpdateStaticPart()
    {
        __KF_SET_STATIC_OPTION_TRAIN__( "trainconsumefid", _uint32_value, _consume_fid );
        __KF_SET_STATIC_OPTION_TRAIN__( "trainonekeyconsumefid", _uint32_value, _onekey_consume_fid );
        __KF_SET_STATIC_OPTION_TRAIN__( "traintotaltime", _uint32_value, _total_time );
        __KF_SET_STATIC_OPTION_TRAIN__( "traincdtime", _uint32_value, _cd_time );
        __KF_SET_STATIC_OPTION_TRAIN__( "trainunittime", _uint32_value, _unit_time );
    }

    void  KFTrainSetting::UpdateDynamicPart( KFEntity* player )
    {
        _count = player->Get<uint32>( __STRING__( effect ), __STRING__( traincount ) );

        // 单位时间所得经验
        auto unitexp = player->Get<uint64>( __STRING__( effect ), __STRING__( trainunitexp ) );
        auto expaddscale = player->Get<uint64>( __STRING__( effect ), __STRING__( trainexpscale ) ) / ( double )KFRandEnum::TenThousand;
        _add_exp = unitexp * ( 1.0 + expaddscale );

        // 一般消耗 缩减比例
        auto consumedecscale = player->Get<uint64>( __STRING__( effect ), __STRING__( traincostscaledec ) ) / ( double )KFRandEnum::TenThousand;
        _scale_consume = 1.0 - consumedecscale;
    }

    void KFTrainModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFTrainModule::OnEnterTrainModule );
        __REGISTER_LEAVE_PLAYER__( &KFTrainModule::OnLeaveTrainModule );

        __REGISTER_REMOVE_DATA_1__( __STRING__( hero ), &KFTrainModule::OnRemoveHero );
        __REGISTER_REMOVE_DATA_1__( __STRING__( train ), &KFTrainModule::OnRemoveTrainHero );
        __REGISTER_UPDATE_DATA_2__( __STRING__( train ), __STRING__( calctime ), &KFTrainModule::OnUpdateCalcTime );

        __REGISTER_EXECUTE__( __STRING__( traincostscaledec ), &KFTrainModule::OnExecuteTrainCostScaleDec );
        __REGISTER_EXECUTE__( __STRING__( traincount ), &KFTrainModule::OnExecuteTrainCount );
        __REGISTER_EXECUTE__( __STRING__( trainunitexp ), &KFTrainModule::OnExecuteTrainUnitExp );
        __REGISTER_EXECUTE__( __STRING__( trainexpscale ), &KFTrainModule::OnExecuteTrainExpScale );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_CHANGE_REQ, &KFTrainModule::HandleTrainChangeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_CLEAN_REQ, &KFTrainModule::HandleTrainCleanReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_ONEKEY_REQ, &KFTrainModule::HandleTrainOnekeyReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_AGAIN_REQ, &KFTrainModule::HandleTrainAgainReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_FEE_REQ, &KFTrainModule::HandleTrainFeeReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRAIN_FEE_ONE_KEY_REQ, &KFTrainModule::HandleTrainOneKeyFeeReq );
    }

    void KFTrainModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();

        __UN_REMOVE_DATA_1__( __STRING__( hero ) );
        __UN_REMOVE_DATA_1__( __STRING__( train ) );
        __UN_UPDATE_DATA_2__( __STRING__( train ), __STRING__( calctime ) );

        __UN_EXECUTE__( __STRING__( traincostscaledec ) );
        __UN_EXECUTE__( __STRING__( trainunitexp ) );
        __UN_EXECUTE__( __STRING__( trainexpscale ) );

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_TRAIN_CHANGE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TRAIN_CLEAN_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TRAIN_ONEKEY_REQ );
        __UN_MESSAGE__( KFMsg::MSG_TRAIN_AGAIN_REQ );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFTrainModule::OnUpdateCalcTime )
    {
        auto kftrain = kfdata->GetParent();
        auto endtime = kftrain->Get<uint64>( __STRING__( endtime ) );
        if ( newvalue == endtime )
        {
            // 训练完成增加条件次数
            AddTrainCondition( player, kftrain );
        }
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTrainModule::OnRemoveHero )
    {
        auto posflag = kfdata->Get<uint32>( __STRING__( posflag ) );
        if ( posflag != KFMsg::TrainBuild )
        {
            return;
        }

        auto kftrainrecord = player->Find( __STRING__( train ) );
        auto kftrain = GetTrainById( player, key );
        if ( kftrain == nullptr )
        {
            return;
        }

        // 删除训练英雄
        player->RemoveData( kftrainrecord, kftrain->GetKeyID() );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFTrainModule::OnRemoveTrainHero )
    {
        // 取消定时器
        __UN_TIMER_2__( player->GetKeyID(), key );

        // 设置标记
        auto uuid = kfdata->Get<uint64>( __STRING__( uuid ) );
        player->UpdateData( __STRING__( hero ), uuid, __STRING__( posflag ), KFEnum::Set, KFMsg::HeroList );
    }

    __KF_MESSAGE_FUNCTION__( KFTrainModule::HandleTrainChangeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTrainChangeReq );

        auto setting = GetTrainSetting( player );
        if ( setting == nullptr )
        {
            // 训练所配置错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampSettingError );
        }

        if ( kfmsg.index() > setting->_count )
        {
            // 不支持的栏位
            return _kf_display->SendToClient( player, KFMsg::TrainCampIndexError );
        }

        auto kftraincamprecord = player->Find( __STRING__( train ) );
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
            RemoveTrainHero( player, kftraincamprecord, kftraincamp );
        }
        else
        {
            // 增加英雄进栏位
            AddTrainHero( player, kftraincamprecord, setting, kfmsg.uuid(), kfmsg.index() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTrainModule::HandleTrainCleanReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTrainCleanReq );

        player->CleanData( __STRING__( train ) );
    }

    __KF_MESSAGE_FUNCTION__( KFTrainModule::HandleTrainOnekeyReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTrainOnekeyReq );

        auto setting = GetTrainSetting( player );
        if ( setting == nullptr || setting->_unit_time == 0u || setting->_cd_time == 0u )
        {
            // 训练所配置错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampSettingError );
        }

        if ( kfmsg.index() > setting->_count )
        {
            // 训练栏位置错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampIndexError );
        }

        auto kftrain = player->Find( __STRING__( train ), kfmsg.index() );
        if ( kftrain == nullptr )
        {
            // 训练栏位置错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampIndexError );
        }

        auto uuid = kftrain->Get<uint64>( __STRING__( uuid ) );
        auto kfhero = player->Find( __STRING__( hero ), uuid );
        if ( kfhero == nullptr )
        {
            // 查找英雄列表是否存在uuid
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto calctime = kftrain->Get<uint64>( __STRING__( calctime ) );
        auto endtime = kftrain->Get<uint64>( __STRING__( endtime ) );
        if ( endtime <= calctime )
        {
            // 该栏位训练已结束
            return _kf_display->SendToClient( player, KFMsg::TrainCampIsFinished );
        }

        auto herouuid = kfhero->Get<uint32>( __STRING__( uuid ) );

        // 获取消耗
        auto lefttime = endtime - calctime;
        auto consume = CalcTrainOneKeyConsume( player, setting, lefttime );
        auto& dataname = player->RemoveElement( consume, 1u, __STRING__( trainonekey ), herouuid, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        auto addcount = lefttime / setting->_cd_time;
        addcount = __MAX__( addcount, 1u );

        AddTrainHeroExp( player, kftrain, setting, addcount, true );

        // 取消定时器
        __UN_TIMER_2__( player->GetKeyID(), kfmsg.index() );
        player->UpdateData( kftrain, __STRING__( calctime ), KFEnum::Set, endtime );
    }

    __KF_MESSAGE_FUNCTION__( KFTrainModule::HandleTrainAgainReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTrainAgainReq );

        auto setting = GetTrainSetting( player );
        if ( setting == nullptr )
        {
            // 训练营等级错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampLevelError );
        }

        if ( kfmsg.index() > setting->_count )
        {
            // 训练栏位置错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampIndexError );
        }

        auto kftrain = player->Find( __STRING__( train ), kfmsg.index() );
        if ( kftrain == nullptr )
        {
            // 训练栏位置错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampIndexError );
        }

        auto calctime = kftrain->Get<uint64>( __STRING__( calctime ) );
        auto endtime = kftrain->Get<uint64>( __STRING__( endtime ) );
        if ( calctime != endtime )
        {
            // 训练还未完成
            return _kf_display->SendToClient( player, KFMsg::TrainNotFinish );
        }

        auto uuid = kftrain->Get<uint64>( __STRING__( uuid ) );
        auto kfhero = player->Find( __STRING__( hero ), uuid );
        if ( kfhero == nullptr )
        {
            // 查找英雄列表是否存在uuid
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        if ( _kf_hero->IsMaxLevel( player, kfhero ) )
        {
            // 当前英雄已满级
            return _kf_display->SendToClient( player, KFMsg::HeroLevelIsMax );
        }

        // 消耗缩放比例
        auto heroid = kfhero->Get<uint32>( __STRING__( id ) );
        auto consume = CalcTrainConsume( player, setting );
        auto& dataname = player->RemoveElement( consume, setting->_scale_consume, __STRING__( train ), heroid, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        // 更新训练时间
        player->UpdateData( kftrain, __STRING__( calctime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
        player->UpdateData( kftrain, __STRING__( endtime ), KFEnum::Set, KFGlobal::Instance()->_real_time + setting->_total_time );

        // 启动定时器
        StartTrainTimer( player, setting, kfmsg.index(), 0u );
    }

    __KF_MESSAGE_FUNCTION__( KFTrainModule::HandleTrainFeeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTrainFeeReq );

        auto setting = GetTrainSetting( player );
        if ( setting == nullptr )
        {
            // 训练所配置错误
            return;
        }

        if ( setting->_count == 0u )
        {
            // 不支持的栏位
            return;
        }

        auto consume = CalcTrainConsume( player, setting );
        if ( consume == nullptr )
        {
            // 无效消耗
            return;
        }

        KFMsg::MsgTrainFeeAck ack;
        ack.set_element( consume->GetElement() );
        _kf_player->SendToClient( player, KFMsg::MSG_TRAIN_FEE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFTrainModule::HandleTrainOneKeyFeeReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTrainOneKeyFeeReq );

        auto setting = GetTrainSetting( player );
        if ( setting == nullptr )
        {
            // 训练所配置错误
            return;
        }

        if ( kfmsg.index() > setting->_count )
        {
            // 不支持的栏位
            return;
        }

        auto kftrain = player->Find( __STRING__( train ), kfmsg.index() );
        if ( kftrain == nullptr )
        {
            // 训练栏位置错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampIndexError );
        }

        auto uuid = kftrain->Get<uint64>( __STRING__( uuid ) );
        auto kfhero = player->Find( __STRING__( hero ), uuid );
        if ( kfhero == nullptr )
        {
            // 查找英雄列表是否存在uuid
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto calctime = kftrain->Get<uint64>( __STRING__( calctime ) );
        auto endtime = kftrain->Get<uint64>( __STRING__( endtime ) );
        if ( endtime <= calctime )
        {
            // 该栏位训练已结束
            return _kf_display->SendToClient( player, KFMsg::TrainCampIsFinished );
        }

        auto lefttime = endtime - calctime;
        auto consume = CalcTrainOneKeyConsume( player, setting, lefttime );
        if ( consume == nullptr )
        {
            // 无效消耗
            return;
        }

        KFMsg::MsgTrainOneKeyFeeAck ack;
        ack.set_index( kfmsg.index() );
        ack.set_element( consume->GetElement() );
        _kf_player->SendToClient( player, KFMsg::MSG_TRAIN_FEE_ONE_KEY_ACK, &ack );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFTrainModule::OnEnterTrainModule )
    {
        if ( !IsTrainActive( player ) )
        {
            return;
        }

        // 检查定时器
        EnterStartTrainTimer( player );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFTrainModule::OnLeaveTrainModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    __KF_EXECUTE_FUNCTION__( KFTrainModule::OnExecuteTrainCostScaleDec )
    {
        return CommonAddEffectHandle( player, executedata, __STRING__( traincostscaledec ) );
    }

    __KF_EXECUTE_FUNCTION__( KFTrainModule::OnExecuteTrainCount )
    {
        return CommonAddEffectHandle( player, executedata, __STRING__( traincount ) );
    }

    __KF_EXECUTE_FUNCTION__( KFTrainModule::OnExecuteTrainUnitExp )
    {
        return CommonAddEffectHandle( player, executedata, __STRING__( trainunitexp ) );
    }

    __KF_EXECUTE_FUNCTION__( KFTrainModule::OnExecuteTrainExpScale )
    {
        return CommonAddEffectHandle( player, executedata, __STRING__( trainexpscale ) );
    }

    bool KFTrainModule::CommonAddEffectHandle( KFEntity* player, const KFExecuteData* executedata, const std::string& fieldname )
    {
        if ( executedata == nullptr )
        {
            return false;
        }

        // params1=value
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto value = executedata->_param_list._params[0]->_int_value;
        player->UpdateData( __STRING__( effect ), fieldname, KFEnum::Add, value );
        return true;
    }

    const KFElements*  KFTrainModule::CalcTrainConsume( KFEntity* player, const KFTrainSetting* setting )
    {
        static KFElements _element;
        _element.Clear();

        // 公式配置
        auto kfformulasetting = KFFormulaConfig::Instance()->FindFormulaParam( setting->_consume_fid );
        if ( kfformulasetting == nullptr ||
                kfformulasetting->_type.empty() ||
                kfformulasetting->_params._objects.size() < 1u )
        {
            return nullptr;
        }

        auto param1 = kfformulasetting->_params._objects[0]->_double_value;

        // 消耗个数 = 训练总经验 * 公式参数1 * 消耗缩放比例
        auto totalexp = double( setting->_total_time ) / KFTimeEnum::OneMinuteSecond * setting->_add_exp;
        auto costnum = static_cast<uint32>( std::round( totalexp * param1 * setting->_scale_consume ) );

        // 费用数据格式
        KFElementConfig::Instance()->FormatElement( _element, kfformulasetting->_type, costnum );
        return &_element;
    }

    const KFElements*  KFTrainModule::CalcTrainOneKeyConsume( KFEntity* player, const KFTrainSetting* setting, uint32 lefttime )
    {
        static KFElements _element;
        _element.Clear();

        // 公式配置
        auto kfformulasetting = KFFormulaConfig::Instance()->FindFormulaParam( setting->_onekey_consume_fid );
        if ( kfformulasetting == nullptr ||
                kfformulasetting->_type.empty() ||
                kfformulasetting->_params._objects.size() < 3u )
        {
            return nullptr;
        }

        auto param1 = kfformulasetting->_params._objects[0]->_double_value;
        if ( param1 == 0.0 )
        {
            return nullptr;
        }

        auto param2 = kfformulasetting->_params._objects[1]->_double_value;
        auto param3 = kfformulasetting->_params._objects[2]->_int_value;
        auto leftmin = lefttime / KFTimeEnum::OneMinuteSecond;

        // 消耗个数 = 剩余分钟 / 参数1 * 参数2
        auto costnum = static_cast<uint32>( std::round( leftmin / param1 * param2 ) );
        costnum = __MAX__( costnum, param3 );
        KFElementConfig::Instance()->FormatElement( _element, kfformulasetting->_type, costnum );
        return &_element;
    }

    uint32 KFTrainModule::GetTrainLevel( KFEntity* player )
    {
        auto kfbuild = player->Find( __STRING__( build ), KFMsg::TrainBuild );
        if ( kfbuild == nullptr )
        {
            return 0u;
        }

        return kfbuild->Get( __STRING__( level ) );
    }

    bool KFTrainModule::IsTrainActive( KFEntity* player )
    {
        auto kfbuild = player->Find( __STRING__( build ), KFMsg::TrainBuild );
        return kfbuild != nullptr;
    }

    KFData* KFTrainModule::GetTrainById( KFEntity* player, uint64 uuid )
    {
        auto kftraincamprecord = player->Find( __STRING__( train ) );
        for ( auto kftrain = kftraincamprecord->First(); kftrain != nullptr; kftrain = kftraincamprecord->Next() )
        {
            if ( uuid == kftrain->Get<uint64>( __STRING__( uuid ) ) )
            {
                return kftrain;
            }
        }

        return nullptr;
    }

    const KFrame::KFTrainSetting* KFTrainModule::GetTrainSetting( KFEntity* player )
    {
        // 公用配置格式
        static KFTrainSetting _setting;
        _setting.UpdateStaticPart();
        _setting.UpdateDynamicPart( player );

        return &_setting;
    }

    void KFTrainModule::RemoveTrainHero( KFEntity* player, KFData* kftrainrecord, KFData* kftrain )
    {
        // 训练-条件回调
        auto calctime = kftrain->Get<uint64>( __STRING__( calctime ) );
        auto endtime = kftrain->Get<uint64>( __STRING__( endtime ) );
        if ( calctime != endtime )
        {
            // 已完成训练移出的不算条件次数
            AddTrainCondition( player, kftrain );
        }

        // 删除训练英雄
        player->RemoveData( kftrainrecord, kftrain->GetKeyID() );
    }

    void KFTrainModule::AddTrainCondition( KFEntity* player, KFData* kftrain )
    {
        auto uuid = kftrain->Get<uint64>( __STRING__( uuid ) );
        auto kfhero = player->Find( __STRING__( hero ), uuid );
        if ( kfhero != nullptr )
        {
            auto kftrainhero = player->Find( __STRING__( trainhero ) );
            kftrainhero->CopyFrom( kfhero );
            player->UpdateData( kftrainhero, __STRING__( uuid ), KFEnum::Set, uuid );
        }
    }

    void KFTrainModule::AddTrainHero( KFEntity* player, KFData* kftrainrecord, const KFTrainSetting* kfsetting, uint64 uuid, uint32 index )
    {
        if ( !IsTrainActive( player ) )
        {
            return _kf_display->SendToClient( player, KFMsg::BuildFuncNotActive );
        }

        if ( kfsetting == nullptr )
        {
            // 训练营等级错误
            return _kf_display->SendToClient( player, KFMsg::TrainCampLevelError );
        }

        if ( index == 0u || index > kfsetting->_count )
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

        if ( _kf_hero->IsMaxLevel( player, kfhero ) )
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

        // 消耗缩放比例
        auto heroid = kfhero->Get<uint32>( __STRING__( id ) );
        auto consume = CalcTrainConsume( player, kfsetting );
        auto& dataname = player->RemoveElement( consume, kfsetting->_scale_consume, __STRING__( train ), heroid, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        auto kftrain = kftrainrecord->Find( index );
        if ( kftrain != nullptr )
        {
            RemoveTrainHero( player, kftrainrecord, kftrain );
        }

        // 将英雄加入栏位
        kftrain = player->CreateData( kftrainrecord );
        kftrain->Set( __STRING__( uuid ), uuid );
        kftrain->Set( __STRING__( calctime ), KFGlobal::Instance()->_real_time );
        kftrain->Set( __STRING__( endtime ), KFGlobal::Instance()->_real_time + kfsetting->_total_time );
        player->AddData( kftrainrecord, index, kftrain );

        // 启动定时器
        StartTrainTimer( player, kfsetting, index, 0u );

        // 更新英雄当前位置
        player->UpdateData( kfhero, __STRING__( posflag ), KFEnum::Set, KFMsg::TrainBuild );
    }

    void KFTrainModule::StartTrainTimer( KFEntity* player, const KFTrainSetting* kfsetting, uint32 index, uint32 delaytime )
    {
        // 启动定时器
        __UN_TIMER_2__( player->GetKeyID(), index );
        __LOOP_TIMER_2__( player->GetKeyID(), index, kfsetting->_cd_time * 1000, delaytime * 1000, &KFTrainModule::OnTimerAddExp );
    }

    void KFTrainModule::EnterStartTrainTimer( KFEntity* player )
    {
        auto setting = GetTrainSetting( player );
        if ( setting == nullptr || setting->_cd_time == 0u )
        {
            return;
        }

        auto nowtime = KFGlobal::Instance()->_real_time;
        auto kftraincamprecord = player->Find( __STRING__( train ) );
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

            auto addexp = AddTrainHeroExp( player, kftrain, setting, count );
            if ( ( count != 0 && addexp == 0u ) || nowtime >= endtime )
            {
                player->UpdateData( kftrain, __STRING__( calctime ), KFEnum::Set, endtime );
            }
            else
            {
                calctime = kftrain->Get<uint64>( __STRING__( calctime ) );

                // 启动定时器
                auto delaytime = setting->_cd_time - ( nowtime - calctime ) % setting->_cd_time;
                StartTrainTimer( player, setting, kftrain->GetKeyID(), delaytime );
            }
        }
    }

    __KF_TIMER_FUNCTION__( KFTrainModule::OnTimerAddExp )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto kftrain = player->Find( __STRING__( train ), subid );
        if ( kftrain == nullptr )
        {
            // 数据错误取消定时器
            return __UN_TIMER_2__( objectid, subid );
        }

        auto setting = GetTrainSetting( player );
        if ( setting == nullptr )
        {
            // 数据错误取消定时器
            return __UN_TIMER_2__( objectid, subid );
        }

        auto addexp = AddTrainHeroExp( player, kftrain, setting, 1, true );
        auto calctime = kftrain->Get<uint64>( __STRING__( calctime ) );
        auto endtime = kftrain->Get<uint64>( __STRING__( endtime ) );

        if ( calctime >= endtime || addexp == 0u )
        {
            // 满级取消定时器
            __UN_TIMER_2__( objectid, subid );

            player->UpdateData( kftrain, __STRING__( calctime ), KFEnum::Set, endtime );
        }
    }

    uint32 KFTrainModule::AddTrainHeroExp( KFEntity* player, KFData* kftrain, const KFTrainSetting* kfsetting, uint32 count, bool isnow )
    {
        if ( count == 0u || kfsetting == nullptr || kfsetting->_add_exp == 0u )
        {
            return 0u;
        }

        auto calctime = kftrain->Get<uint64>( __STRING__( calctime ) );
        auto endtime = kftrain->Get<uint64>( __STRING__( endtime ) );
        if ( calctime >= endtime )
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
        auto totalexp = kfsetting->_add_exp * count;
        totalexp = _kf_hero->AddExp( player, kfhero, totalexp );
        if ( totalexp != 0u )
        {
            if ( isnow )
            {
                calctime = KFGlobal::Instance()->_real_time;
            }
            else
            {
                // 实际增加次数
                count = ( totalexp - 1u ) / kfsetting->_add_exp + 1u;
                calctime += count * kfsetting->_cd_time;
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
                AddTrainHeroLevelRecord( player, kfsetting, kftrain, kfhero );
            }
        }

        return totalexp;
    }

    void KFTrainModule::AddTrainHeroLevelRecord( KFEntity* player, const KFTrainSetting* kfsetting, KFData* kftrain, KFData* kfhero )
    {
        auto upgradetime = kftrain->Get<uint32>( __STRING__( calctime ) );
        if ( !player->IsInited() )
        {
            // 下线期间通过时间升级的, 需要精确计算出升级的时间
            auto curexp = kfhero->Get<uint32>( __STRING__( exp ) );

            // 推导出升级的具体时间
            upgradetime -= curexp / kfsetting->_add_exp * kfsetting->_cd_time;
        }

        auto level = kfhero->Get<uint32>( __STRING__( level ) );
        _kf_record_client->AddCampRecord( player, kfhero, upgradetime, KFMsg::TrainBuild, level );
    }


}