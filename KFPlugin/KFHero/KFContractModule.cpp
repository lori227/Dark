#include "KFContractModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFContractModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::MSG_CONTRACT_DATA_REQ, &KFContractModule::HandleContractDataReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CONTRACT_HERO_REQ, &KFContractModule::HandleContractHeroReq );
    }

    void KFContractModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::MSG_CONTRACT_DATA_REQ );
        __UN_MESSAGE__( KFMsg::MSG_CONTRACT_HERO_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFContractModule::HandleContractDataReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgContractDataReq );

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto durability = kfhero->Get<uint32>( __STRING__( durability ) );
        if ( durability > 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroDurabilityNotZero );
        }

        // 获取续签数据
        uint32 nextdurability;
        std::string cost;
        uint32 errnum;
        if ( !GetContractData( kfhero, nextdurability, cost, errnum ) )
        {
            return _kf_display->SendToClient( player, errnum );
        }

        // 通知客户端数据
        KFMsg::MsgContractDataAck ack;
        ack.set_uuid( kfmsg.uuid() );
        ack.set_durability( nextdurability );
        ack.set_cost( cost );

        _kf_player->SendToClient( player, KFMsg::MSG_CONTRACT_DATA_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFContractModule::HandleContractHeroReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgContractHeroReq );

        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfhero = kfherorecord->Find( kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto durability = kfhero->Get<uint32>( __STRING__( durability ) );
        if ( durability > 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroDurabilityNotZero );
        }

        if ( kfmsg.choice() == _invalid_int )
        {
            // 退役
            player->RemoveData( kfherorecord, kfmsg.uuid() );
            __LOG_INFO__( "player=[{}] retire hero=[{}]", player->GetKeyID(), kfmsg.uuid() );

            return;
        }

        // 获取续签数据
        uint32 nextdurability;
        std::string cost;
        uint32 errnum;
        if ( !GetContractData( kfhero, nextdurability, cost, errnum ) )
        {
            return _kf_display->SendToClient( player, errnum );
        }

        if ( nextdurability == _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::ContractTimeIsZero );
        }

        static KFElements _cost_element;
        _cost_element.Parse( cost, __FUNC_LINE__ );

        // 消耗是否足够
        auto& dataname = player->RemoveElement( &_cost_element, _default_multiple, __STRING__( contracthero ), 0u, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        player->UpdateData( kfhero, __STRING__( maxdurability ), KFEnum::Set, nextdurability );
        player->UpdateData( kfhero, __STRING__( durability ), KFEnum::Set, nextdurability );
        player->UpdateData( kfhero, __STRING__( adddurability ), KFEnum::Add, 1u );
    }

    bool KFContractModule::GetContractData( KFData* kfhero, uint32& durability, std::string& cost, uint32& errnum )
    {
        // 续签时间公式id
        static auto _timefid_option = _kf_option->FindOption( __STRING__( contracttimefid ) );
        auto kftimesetting = KFFormulaConfig::Instance()->FindSetting( _timefid_option->_uint32_value );
        if ( kftimesetting == nullptr || kftimesetting->_params.size() < 2u )
        {
            errnum = KFMsg::ContractTimeFormulaError;
            return false;
        }

        auto param1 = kftimesetting->_params[0]->_double_value;
        auto param2 = kftimesetting->_params[1]->_double_value;

        // 计算续签年限
        auto maxdurability = kfhero->Get<uint32>( __STRING__( maxdurability ) );
        auto nowdurability = static_cast<uint32>( __MAX__( maxdurability * param1, param2 ) );

        // 续签价格公式id
        static auto _pricefid_option = _kf_option->FindOption( __STRING__( contractpricefid ) );
        auto kfpricesetting = KFFormulaConfig::Instance()->FindSetting( _pricefid_option->_uint32_value );
        if ( kfpricesetting == nullptr || kfpricesetting->_type.empty() || kfpricesetting->_params.size() < 8u )
        {
            errnum = KFMsg::ContractPriceFormulaError;
            return false;
        }

        auto price = 0.0;
        auto basicprice = 0.0;
        auto levelprice = 0.0;
        param1 = kfpricesetting->_params[0]->_double_value;
        param2 = kfpricesetting->_params[1]->_double_value;
        auto param3 = kfpricesetting->_params[2]->_double_value;
        auto param4 = kfpricesetting->_params[3]->_double_value;
        auto param5 = kfpricesetting->_params[4]->_double_value;
        auto param6 = kfpricesetting->_params[5]->_double_value;
        auto param7 = kfpricesetting->_params[6]->_double_value;
        auto param8 = kfpricesetting->_params[7]->_double_value;

        // 计算平均成长率
        auto totalgrowth = 0u;
        auto kfgrowth = kfhero->Find( __STRING__( growth ) );
        for ( auto kfchild = kfgrowth->First(); kfchild != nullptr; kfchild = kfgrowth->Next() )
        {
            totalgrowth += kfchild->Get<uint32>();
        }
        auto averagegrowth = static_cast<double>( totalgrowth ) / kfgrowth->Size();

        if ( param5 != 0u )
        {
            // 基础价格
            basicprice = param1 + ( averagegrowth - param2 ) * param3 / param5;
            basicprice = __MAX__( basicprice, param4 );
        }

        if ( param6 != 0u )
        {
            // 等级价格
            auto level = kfhero->Get<uint32>( __STRING__( level ) );
            levelprice = level * param7 / param6;
        }

        auto adddurability = kfhero->Get<uint32>( __STRING__( adddurability ) );
        price = ( basicprice * nowdurability + levelprice ) * ( 1 + adddurability * adddurability * param8 );

        auto finalprice = static_cast<uint32>( price + 0.5 );

        cost = KFElementConfig::Instance()->StringElemnt( kfpricesetting->_type, finalprice, 0u );

        return true;
    }


}