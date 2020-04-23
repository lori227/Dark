#include "KFContractModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFContractModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::MSG_CONTRACT_DATA_REQ, &KFContractModule::HandleContractDataReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CONTRACT_HERO_REQ, &KFContractModule::HandleContractHeroReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_RETIRE_HERO_REQ, &KFContractModule::HandleRetireHeroReq );
    }

    void KFContractModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::MSG_CONTRACT_DATA_REQ );
        __UN_MESSAGE__( KFMsg::MSG_CONTRACT_HERO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_RETIRE_HERO_REQ );
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
        KFMsg::MsgContractDataAck ack;
        ack.set_uuid( kfmsg.uuid() );
        auto nextdurability = CalcContractDurability( kfhero );
        if ( nextdurability != 0u )
        {
            auto& strcost = CalcContractCostPrice( kfhero, nextdurability );
            ack.set_cost( strcost );
        }
        ack.set_durability( nextdurability );
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

        auto nextdurability = CalcContractDurability( kfhero );
        if ( nextdurability == _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::ContractTimeIsZero );
        }

        static KFElements _cost_element;
        auto& strcost = CalcContractCostPrice( kfhero, nextdurability );
        if ( strcost.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::ContractPriceFormulaError );
        }

        _cost_element.Parse( strcost, __FUNC_LINE__ );
        auto& dataname = player->RemoveElement( &_cost_element, _default_multiple, __STRING__( contracthero ), 0u, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        player->UpdateData( kfhero, __STRING__( maxdurability ), KFEnum::Set, nextdurability );
        player->UpdateData( kfhero, __STRING__( durability ), KFEnum::Set, nextdurability );
        player->UpdateData( kfhero, __STRING__( adddurability ), KFEnum::Add, 1u );

        _kf_display->SendToClient( player, KFMsg::HeroContractSuc );
    }

    __KF_MESSAGE_FUNCTION__( KFContractModule::HandleRetireHeroReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRetireHeroReq );

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

        // 设置退役标记
        kfhero->Set<uint32>( __STRING__( lock ), KFMsg::Retirement );
        player->RemoveData( kfherorecord, kfmsg.uuid() );
        __LOG_INFO__( "player=[{}] retire hero=[{}]", player->GetKeyID(), kfmsg.uuid() );

        return _kf_display->SendToClient( player, KFMsg::HeroRetireSuc );
    }

    uint32 KFContractModule::CalcContractDurability( KFData* kfhero )
    {
        auto custom = kfhero->Get<uint32>( __STRING__( custom ) );
        if ( custom > 0u )
        {
            return 0u;
        }

        // 续签时间公式id
        static auto _timefid_option = KFGlobal::Instance()->FindConstant( __STRING__( contracttimefid ) );
        auto kftimeformulaparam = KFFormulaConfig::Instance()->FindFormulaParam( _timefid_option->_uint32_value );
        if ( kftimeformulaparam == nullptr || kftimeformulaparam->_params._objects.size() < 2u )
        {
            return 0u;
        }

        auto param1 = kftimeformulaparam->_params._objects[ 0 ]->_double_value;
        auto param2 = kftimeformulaparam->_params._objects[ 1 ]->_double_value;

        // 计算续签年限
        auto maxdurability = kfhero->Get<uint32>( __STRING__( maxdurability ) );
        auto nextdurability = maxdurability * param1;
        nextdurability = static_cast< uint32 >( __MAX__( nextdurability, param2 ) );
        return nextdurability;
    }

    const std::string& KFContractModule::CalcContractCostPrice( KFData* kfhero, uint32 durability )
    {
        // 参数1 保底价格
        // 参数2 品质单价
        // 参数3 年限区间
        // 参数4 每个年限区间单价
        // 参数5 等级区间
        // 参数6 每个等级区间单价
        // 参数7 品质没有白色和褐色, 修正值
        // 参数8 每次续约涨价幅度

        // 基础价格 Max((参数1+(角色品质-参数7)*参数2),参数1)
        // 年限价格 续约年限/参数3*参数4
        // 等级价格 角色等级/参数5*参数6
        // 最终价格 (基础价格+年限价格+等级价格)*(1+续约次数*参数8), 四舍五入

        // 续签价格公式id
        static auto _pricefid_option = KFGlobal::Instance()->FindConstant( __STRING__( contractpricefid ) );
        auto kfpriceformulaparam = KFFormulaConfig::Instance()->FindFormulaParam( _pricefid_option->_uint32_value );
        if ( kfpriceformulaparam == nullptr ||
                kfpriceformulaparam->_type.empty() ||
                kfpriceformulaparam->_params._objects.size() < 8u )
        {
            return _invalid_string;
        }

        // 品质
        auto quality = kfhero->Get<uint32>( __STRING__( quality ) );

        // 参数1 保底价格
        auto param1 = kfpriceformulaparam->_params._objects[ 0 ]->_double_value;
        // 参数2 品质单价
        auto param2 = kfpriceformulaparam->_params._objects[ 1 ]->_double_value;
        // 参数7 品质没有白色和褐色, 修正值
        auto param7 = kfpriceformulaparam->_params._objects[ 6 ]->_double_value;

        // 基础价格 Max((参数1+(角色品质-参数7)*参数2),参数1)
        auto basicprice = param1 + ( quality - param7 ) * param2;
        auto finalprice = __MAX__( basicprice, param1 );

        // 参数3 年限区间
        auto param3 = kfpriceformulaparam->_params._objects[ 2 ]->_double_value;
        // 参数4 每个年限区间单价
        auto param4 = kfpriceformulaparam->_params._objects[ 3 ]->_double_value;
        if ( param3 != 0 )
        {
            // 年限价格 续约年限/参数3*参数4
            finalprice += durability / param3 * param4;
        }

        // 参数5 等级区间
        auto param5 = kfpriceformulaparam->_params._objects[ 4 ]->_double_value;
        // 参数6 每个等级区间单价
        auto param6 = kfpriceformulaparam->_params._objects[ 5 ]->_double_value;
        if ( param5 != 0 )
        {
            // 等级价格 角色等级/参数5*参数6
            auto level = kfhero->Get<uint32>( __STRING__( level ) );
            finalprice += level / param5 * param6;
        }

        // 参数8 每次续约涨价幅度
        auto param8 = kfpriceformulaparam->_params._objects[ 7 ]->_double_value;
        // 最终价格 (基础价格+年限价格+等级价格)*(1+续约次数*参数8), 四舍五入
        auto adddurability = kfhero->Get<uint32>( __STRING__( adddurability ) );
        finalprice *= ( 1.0 + ( adddurability + 1.0 ) * param8 );
        return KFElementConfig::Instance()->FormatString( kfpriceformulaparam->_type, ( uint32 )std::round( finalprice ) );
    }
}