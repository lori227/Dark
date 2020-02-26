#include "KFTransferModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFTransferModule::BeforeRun()
    {
        //_kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRANSFER_PRO_REQ, &KFTransferModule::HandleTransferProReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SELECT_INNATE_REQ, &KFTransferModule::HandleSelectInnateReq );
    }

    void KFTransferModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::MSG_TRANSFER_PRO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SELECT_INNATE_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFTransferModule::HandleTransferProReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTransferProReq );

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto profession = kfhero->Get( __STRING__( profession ) );
        auto transferid = KFTransferConfig::Instance()->GetTransferId( kfmsg.profession(), profession );
        auto kfsetting = KFTransferConfig::Instance()->FindSetting( transferid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroTransferDataError );
        }

        auto oldprosetting = KFProfessionConfig::Instance()->FindSetting( profession );
        auto newprosetting = KFProfessionConfig::Instance()->FindSetting( kfmsg.profession() );
        if ( oldprosetting == nullptr || newprosetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroProfessionIsError );
        }

        auto race = kfhero->Get( __STRING__( race ) );
        auto racesetting = KFRaceConfig::Instance()->FindSetting( race );
        if ( racesetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroRaceIsError );
        }

        auto level = kfhero->Get( __STRING__( level ) );
        if ( level < kfsetting->_level )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroLevelNotEnough );
        }

        // 扣除消耗
        auto& dataname = player->RemoveElement( &kfsetting->_cost, _default_multiple, __STRING__( transfer ), transferid, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        // 清空天赋的标记
        auto kfinnaterecord = kfhero->Find( __STRING__( innate ) );
        for ( auto kfinnate = kfinnaterecord->First(); kfinnate != nullptr; kfinnate = kfinnaterecord->Next() )
        {
            if ( kfinnate->Get( __STRING__( flag ) ) != 0u )
            {
                player->UpdateData( kfinnate, __STRING__( flag ), KFEnum::Set, 0u );
            }
        }

        auto weapontype = kfhero->Get( __STRING__( weapontype ) );

        // 保存之前的职业信息
        auto kftransferrecord = kfhero->Find( __STRING__( transfer ) );
        auto kftransfer = player->CreateData( kftransferrecord );
        auto index = kftransferrecord->Size() + 1;
        kftransfer->Set( __STRING__( index ), index );
        kftransfer->Set( __STRING__( profession ), profession );
        kftransfer->Set( __STRING__( weapontype ), weapontype );

        player->AddData( kftransferrecord, index, kftransfer );

        // 开始转职
        player->UpdateData( kfhero, __STRING__( profession ), KFEnum::Set, kfmsg.profession() );

        return _kf_display->SendToClient( player, KFMsg::HeroTransferSuc );
    }

    __KF_MESSAGE_FUNCTION__( KFTransferModule::HandleSelectInnateReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSelectInnateReq );

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        bool ret = false;
        for ( auto i = 0; i < kfmsg.innate_size(); ++i )
        {
            auto innateid = kfmsg.innate( i );

            ret |= RemoveInnateId( player, kfhero, innateid );
        }

        return _kf_display->SendToClient( player, KFMsg::HeroSelectInnateSuc, ret );
    }

    bool KFTransferModule::RemoveInnateId( KFEntity* player, KFData* kfhero, uint32 id )
    {
        static auto _option = _kf_option->FindOption( "useinnatecount" );
        auto kfinnaterecord = kfhero->Find( __STRING__( innate ) );
        if ( kfinnaterecord->Size() <= _option->_uint32_value )
        {
            return false;
        }

        auto kfinnate = kfinnaterecord->Find( id );
        if ( kfinnate == nullptr )
        {
            return false;
        }

        player->RemoveData( kfinnaterecord, id );

        return true;
    }

}