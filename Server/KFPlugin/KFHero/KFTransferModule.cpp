#include "KFTransferModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFTransferModule::BeforeRun()
    {
        //_kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        __REGISTER_MESSAGE__( KFMsg::MSG_TRANSFER_PRO_REQ, &KFTransferModule::HandleTransferProReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SELECT_INNATE_REQ, &KFTransferModule::HandleSelectInnateReq );
    }

    void KFTransferModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::MSG_TRANSFER_PRO_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SELECT_INNATE_REQ );
    }

#define __RAND_WEIGHT_DATA_( poolid, config, isvalid, excludelist, randid )\
    {\
        auto weightpool =  KFWeightConfig::Instance()->FindWeightPool( poolid );\
        if ( weightpool != nullptr )\
        {\
            for ( auto weightdata : weightpool->_weight_data )\
            {   \
                auto kfconfigsetting = config->FindSetting( weightdata->_id ); \
                if ( kfconfigsetting == nullptr || !kfconfigsetting->isvalid )\
                {   \
                    excludelist.insert( weightdata->_id ); \
                }\
            }\
            const KFWeight* weightdata = nullptr; \
            if ( excludelist.empty() )\
            {   \
                weightdata = weightpool->Rand(); \
            }\
            else\
            {   \
                weightdata = weightpool->Rand( excludelist ); \
            }\
            if ( weightdata != nullptr )\
            {   \
                randid = weightdata->_id; \
            }\
        }\
    }\

    __KF_MESSAGE_FUNCTION__( KFTransferModule::HandleTransferProReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgTransferProReq );

        auto kfobject = player->GetData();
        auto kfhero = kfobject->FindData( __KF_STRING__( hero ), kfmsg.uuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto profession = kfhero->GetValue( __KF_STRING__( profession ) );
        auto kfsetting = KFTransferConfig::Instance()->FindSetting( profession );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroCanNotTransfer );
        }

        if ( kfsetting->_transfer_list.find( kfmsg.profession() ) == kfsetting->_transfer_list.end() )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroTransferDataError );
        }

        auto oldprosetting = KFProfessionConfig::Instance()->FindSetting( profession );
        auto newprosetting = KFProfessionConfig::Instance()->FindSetting( kfmsg.profession() );
        if ( oldprosetting == nullptr || newprosetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroProfessionIsError );
        }

        auto level = kfhero->GetValue( __KF_STRING__( level ) );
        if ( level < kfsetting->_level )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroLevelNotEnough );
        }

        auto dataname = player->CheckRemoveElement( &kfsetting->_cost, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }
        player->RemoveElement( &kfsetting->_cost, __FUNC_LINE__ );

        // 开始转职
        player->UpdateData( kfhero, __KF_STRING__( profession ), KFEnum::Set, kfmsg.profession() );

        auto weapontype = kfhero->GetValue( __KF_STRING__( weapontype ) );

        // 保存之前的职业信息
        auto kftransferrecord = kfhero->FindData( __KF_STRING__( transfer ) );
        auto kftransfer = _kf_kernel->CreateObject( kftransferrecord->_data_setting );
        auto index = kftransferrecord->Size() + 1;
        kftransfer->SetValue( __KF_STRING__( index ), index );
        kftransfer->SetValue( __KF_STRING__( profession ), profession );
        kftransfer->SetValue( __KF_STRING__( weapontype ), weapontype );

        player->AddData( kftransferrecord, index, kftransfer );

        if ( !newprosetting->IsWeaponTypeValid( weapontype ) )
        {
            // 随机新的武器类型
            weapontype = newprosetting->RandWeapontype();
            player->UpdateData( kfhero, __KF_STRING__( weapontype ), KFEnum::Set, weapontype );

            // 武器id
            auto kfequiptypesetting = KFEquipTypeConfig::Instance()->FindSetting( weapontype );
            if ( kfequiptypesetting != nullptr )
            {
                player->UpdateData( kfhero, __KF_STRING__( weaponid ), KFEnum::Set, kfequiptypesetting->_default_id );
            }
            else
            {
                __LOG_ERROR__( "weapon type =[{}] can't find setting!", weapontype );
            }
        }

        auto kfgrowth = kfhero->FindData( __KF_STRING__( growth ) );

        // 更新属性fighter
        auto kffighter = kfhero->FindData( __KF_STRING__( fighter ) );
        for ( auto kfchild = kffighter->FirstData(); kfchild != nullptr; kfchild = kffighter->NextData() )
        {
            auto& attrname = kfchild->_data_setting->_name;
            if ( attrname == __KF_STRING__( hp ) )
            {
                continue;
            }

            auto oldprovalue = oldprosetting->GetAttributeValue( attrname );
            auto newprovalue = newprosetting->GetAttributeValue( attrname );

            auto value = ( newprovalue > oldprovalue ) ? ( newprovalue - oldprovalue ) : 0u;
            auto growthvalue = kfgrowth->GetValue<uint32>( attrname );
            value = value * ( level + 10u ) * growthvalue;

            auto dvalue = static_cast<double>( value ) / KFRandEnum::Thousand;
            auto addvalue = static_cast<int>( round( dvalue ) );

            player->UpdateData( kfchild, KFEnum::Add, addvalue );
        }

        // 随机主动技能
        RandWeightData( player, kfmsg.uuid(), __KF_STRING__( active ), kfsetting->_active_pool_list );

        // 随机性格
        RandWeightData( player, kfmsg.uuid(), __KF_STRING__( character ), kfsetting->_character_pool_list );

        // 随机天赋
        RandWeightData( player, kfmsg.uuid(), __KF_STRING__( innate ), kfsetting->_innate_pool_list );
    }

    void KFTransferModule::RandWeightData( KFEntity* player, uint64 uuid, const std::string& str, const std::list<uint32>& slist )
    {
        auto kfhero = player->GetData()->FindData( __KF_STRING__( hero ), uuid );
        if ( kfhero == nullptr )
        {
            return;
        }

        auto kfdataarray = kfhero->FindData( str );
        if ( kfdataarray == nullptr )
        {
            return;
        }

        std::set<uint32> excludelist;
        auto arraylen = kfdataarray->Size() - KFDataDefine::Array_Index;
        for ( uint32 index = KFDataDefine::Array_Index; index <= arraylen; ++index )
        {
            auto dataid = kfdataarray->GetValue<uint64>( index );
            if ( dataid != 0u )
            {
                excludelist.insert( dataid );
            }
        }

        auto datanum = static_cast<uint32>( excludelist.size() );
        if ( datanum >= arraylen )
        {
            return;
        }

        auto race = kfhero->GetValue( __KF_STRING__( race ) );
        auto profession = kfhero->GetValue( __KF_STRING__( profession ) );
        auto background = kfhero->GetValue( __KF_STRING__( background ) );
        auto weapontype = kfhero->GetValue( __KF_STRING__( weapontype ) );

        for ( auto poolid : slist )
        {
            auto randid = 0u;
            if ( str == __KF_STRING__( active ) || str == __KF_STRING__( innate ) )
            {
                __RAND_WEIGHT_DATA_( poolid, KFSkillConfig::Instance(), IsValid( race, profession, background, weapontype ), excludelist, randid );
            }
            else if ( str == __KF_STRING__( character ) )
            {
                __RAND_WEIGHT_DATA_( poolid, KFCharacterConfig::Instance(), IsValid( race, background, profession ), excludelist, randid );
            }

            if ( randid == 0u )
            {
                continue;
            }

            for ( uint32 index = KFDataDefine::Array_Index; index <= arraylen; ++index )
            {
                auto dataid = kfdataarray->GetValue<uint64>( index );
                if ( dataid == 0u )
                {
                    player->UpdateData( kfdataarray, index, KFEnum::Set, dataid );

                    datanum++;
                    break;
                }
            }

            if ( datanum >= arraylen )
            {
                break;
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFTransferModule::HandleSelectInnateReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSelectInnateReq );

        for ( auto i = 0; i < kfmsg.innate_size(); ++i )
        {
            auto innateid = kfmsg.innate( i );

            RemoveInnateId( player, kfmsg.uuid(), innateid );
        }
    }

    void KFTransferModule::RemoveInnateId( KFEntity* player, uint64 uuid, uint32 id )
    {
        auto kfhero = player->GetData()->FindData( __KF_STRING__( hero ), uuid );
        if ( kfhero == nullptr )
        {
            return;
        }

        auto kfinnatearray = kfhero->FindData( __KF_STRING__( innate ) );
        for ( uint32 index = KFDataDefine::Array_Index; index < kfinnatearray->Size(); ++index )
        {
            auto innateid = kfinnatearray->GetValue( index );
            if ( innateid == id )
            {
                kfinnatearray->RemoveData( index );
                break;
            }
        }
    }

}