#include "KFChancelModule.hpp"

namespace KFrame
{
    void KFChancelModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ADD_DATA_1__( __STRING__( hero ), &KFChancelModule::OnAddHero );
        __REGISTER_REMOVE_DATA_1__( __STRING__( hero ), &KFChancelModule::OnRemoveHero );
    }

    void KFChancelModule::BeforeShut()
    {
        __UN_ADD_DATA_1__( __STRING__( hero ) );
        __UN_REMOVE_DATA_1__( __STRING__( hero ) );
    }

    __KF_ADD_DATA_FUNCTION__( KFChancelModule::OnAddHero )
    {
        // 添加英雄出生事件
        _kf_record_client->AddLifeRecord( player, key, KFMsg::HeroBorn );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFChancelModule::OnRemoveHero )
    {
        auto hp = kfdata->Get<uint32>( __STRING__( fighter ), __STRING__( hp ) );
        if ( hp > 0u )
        {
            // 删除英雄生平
            _kf_record_client->DelLifeRecord( player, key );
        }
        else
        {
            // 更新英雄死亡数据
            UpdateDeadData( player, kfdata );

            // 添加英雄死亡事件
            _kf_record_client->AddLifeRecord( player, key, KFMsg::HeroDead );

            // 添加死亡记录
            _kf_record_client->AddDeadRecord( player, kfdata );
        }
    }

    void KFChancelModule::UpdateDeadData( KFEntity* player, KFData* kfhero )
    {
        // 更新种族死亡数量
        auto race = kfhero->Get( __STRING__( race ) );
        UpdateDeadNum( player, race );

        // 更新武器类型死亡数量
        auto weapontype = kfhero->Get( __STRING__( weapontype ) );
        UpdateDeadNum( player, weapontype );

        // 更新移动方式死亡数量
        auto profession = kfhero->Get( __STRING__( profession ) );
        auto kfsetting = KFProfessionConfig::Instance()->FindSetting( profession );
        if ( kfsetting == nullptr )
        {
            return;
        }
        auto movetype = kfsetting->_move_type;
        UpdateDeadNum( player, movetype );
    }

    void KFChancelModule::UpdateDeadNum( KFEntity* player, uint32 type )
    {
        if ( !KFChancelConfig::Instance()->IsValid( type ) )
        {
            return;
        }

        auto kfchancelrecord = player->Find( __STRING__( chancel ) );

        auto kfchancel = kfchancelrecord->Find( type );
        if ( kfchancel == nullptr )
        {
            kfchancel = _kf_kernel->CreateObject( kfchancelrecord->_data_setting );

            kfchancel->Set( __STRING__( type ), type );
            kfchancel->Set( __STRING__( level ), 0u );
            kfchancel->Set( __STRING__( num ), 1u );

            player->AddData( kfchancelrecord, type, kfchancel );
        }
        else
        {
            player->UpdateData( kfchancel, __STRING__( num ), KFEnum::Add, 1u );
        }

        UpdateDeadLevel( player, type );
    }

    void KFChancelModule::UpdateDeadLevel( KFEntity* player, uint32 type )
    {
        auto kfchancelrecord = player->Find( __STRING__( chancel ) );
        for ( auto kfchancel = kfchancelrecord->First(); kfchancel != nullptr; kfchancel = kfchancelrecord->Next() )
        {
            auto deadtype = kfchancel->Get<uint32>( __STRING__( type ) );
            auto level = kfchancel->Get<uint32>( __STRING__( level ) );

            auto settingid = KFChancelConfig::Instance()->GetChancelId( deadtype, level + 1u );
            auto kfsetting = KFChancelConfig::Instance()->FindSetting( settingid );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            if ( kfsetting->_condition_map.find( type ) == kfsetting->_condition_map.end() )
            {
                continue;
            }

            bool iscondition = true;
            for ( auto iter : kfsetting->_condition_map )
            {
                auto kfdata = kfchancelrecord->Find( iter.first );
                if ( kfdata == nullptr )
                {
                    iscondition = false;
                    break;
                }

                auto num = kfdata->Get<uint32>( __STRING__( num ) );
                if ( num < iter.second )
                {
                    iscondition = false;
                    break;
                }
            }

            // 满足升级条件
            if ( iscondition )
            {
                player->UpdateData( kfchancel, __STRING__( level ), KFEnum::Add, 1u );
            }
        }
    }
}

