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
        auto kfsetting = KFTransferConfig::Instance()->FindTransferSetting( kfmsg.profession(), profession );
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
        auto& dataname = player->RemoveElement( &kfsetting->_cost, _default_multiple, __STRING__( transfer ), kfmsg.profession(), __FUNC_LINE__ );
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

        // 开始转职
        player->UpdateData( kfhero, __STRING__( profession ), KFEnum::Set, kfmsg.profession() );

        player->AddDataToShow( __STRING__( transfer ), 0u, kfhero );
    }

    __KF_MESSAGE_FUNCTION__( KFTransferModule::HandleSelectInnateReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSelectInnateReq );

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto quality = kfhero->Get<uint32>( __STRING__( quality ) );
        auto kfqualitysetting = KFQualityConfig::Instance()->FindSetting( quality );
        if ( kfqualitysetting->_innate_num != ( uint32 )kfmsg.innate_size() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroSelectInnateNumError );
        }

        UInt32Set selectlist;
        UInt32Vector removelist;

        auto kfinnaterecord = kfhero->Find( __STRING__( innate ) );
        for ( auto i = 0; i < kfmsg.innate_size(); ++i )
        {
            auto innateid = kfmsg.innate( i );
            auto kfinnate = kfinnaterecord->Find( innateid );
            if ( kfinnate == nullptr )
            {
                return _kf_display->SendToClient( player, KFMsg::HeroSelectInnateIdError );
            }

            selectlist.insert( innateid );
        }

        for ( auto kfinnate = kfinnaterecord->First(); kfinnate != nullptr; kfinnate = kfinnaterecord->Next() )
        {
            auto innateid = kfinnate->Get<uint32>( kfinnaterecord->_data_setting->_key_name );
            if ( selectlist.find( innateid ) == selectlist.end() )
            {
                removelist.emplace_back( innateid );
            }

            // 清空天赋的标记
            if ( kfinnate->Get( __STRING__( flag ) ) != 0u )
            {
                player->UpdateData( kfinnate, __STRING__( flag ), KFEnum::Set, 0u );
            }
        }

        for ( auto iter : removelist )
        {
            player->RemoveData( kfinnaterecord, iter );
        }

        player->AddDataToShow( __STRING__( selectinnate ), 0u, kfhero );
    }
}