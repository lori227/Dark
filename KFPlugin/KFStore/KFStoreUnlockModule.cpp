#include "KFStoreUnlockModule.hpp"

namespace KFrame
{
    void KFStoreUnlockModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_DATA__( &KFStoreUnlockModule::OnAddDataUnlockStore );
        __REGISTER_REMOVE_DATA__( &KFStoreUnlockModule::OnRemoveDataUnlockStore );
        __REGISTER_UPDATE_DATA__( &KFStoreUnlockModule::OnUpdateDataUnlockStore );
        __REGISTER_BEFORE_ENTER_PLAYER__( &KFStoreUnlockModule::OnEnterUnlockStore );
    }

    void KFStoreUnlockModule::BeforeShut()
    {
        __UN_ADD_DATA__();
        __UN_REMOVE_DATA__();
        __UN_UPDATE_DATA__();
        __UN_BEFORE_ENTER_PLAYER__();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ENTER_PLAYER_FUNCTION__( KFStoreUnlockModule::OnEnterUnlockStore )
    {
        auto kfindexrecord = player->Find( __STRING__( storeindex ) );
        auto kfunlockrecord = player->Find( __STRING__( storeunlock ) );
        if ( kfindexrecord == nullptr || kfunlockrecord == nullptr )
        {
            return;
        }

        for ( auto& iter : KFStoreUnlockConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;

            // 判断如果已经激活
            auto sort = kfindexrecord->Get<uint32>( kfsetting->_index, __STRING__( sort ) );
            if ( sort >= kfsetting->_sort )
            {
                continue;
            }

            if ( kfsetting->_unlock_condition.empty() )
            {
                UnlockStoreData( player, kfsetting, kfindexrecord );
                continue;
            }

            // 已经在解锁列表中
            auto kfunlock = kfunlockrecord->Find( kfsetting->_id );
            if ( kfunlock != nullptr )
            {
                continue;
            }

            kfunlock = player->CreateData( kfunlockrecord );
            auto kfconditionobject = kfunlock->Find( __STRING__( conditions ) );
            _kf_condition->AddCondition( player, kfconditionobject, kfsetting->_unlock_condition, kfsetting->_condition_type );
            kfunlockrecord->Add( kfsetting->_id, kfunlock );

            // 判断条件
            auto complete = _kf_condition->InitCondition( player, kfconditionobject, KFConditionEnum::LimitNull, false );
            if ( complete )
            {
                kfunlockrecord->Remove( kfsetting->_id );
                UnlockStoreData( player, kfsetting, kfunlockrecord );
            }
        }
    }

    void KFStoreUnlockModule::UnlockStoreData( KFEntity* player, const KFStoreUnlockSetting* kfsetting, KFData* kfindexrecord )
    {
        // 判断是否已经解锁
        auto sort = kfindexrecord->Get<uint32>( kfsetting->_index, __STRING__( sort ) );
        if ( sort >= kfsetting->_sort )
        {
            return;
        }

        // 开始解锁
        auto kfindex = kfindexrecord->Find( kfsetting->_index );
        if ( kfindex == nullptr )
        {
            kfindex = player->CreateData( kfindexrecord );
            kfindex->Set( __STRING__( index ), kfsetting->_index );
            kfindex->Set( __STRING__( sort ), kfsetting->_sort );
            kfindex->Set( __STRING__( storeid ), kfsetting->_store_id );
            player->AddData( kfindexrecord, kfsetting->_index, kfindex );
        }
        else
        {
            // 删除原有的商店物品
            auto laststoreid = kfindex->Get<uint32>( __STRING__( storeid ) );
            _kf_store->RemoveStore( player, laststoreid );

            // 更新解锁商店
            player->UpdateData( kfindexrecord, kfsetting->_index, __STRING__( sort ), KFEnum::Set, kfsetting->_sort );
            player->UpdateData( kfindexrecord, kfsetting->_index, __STRING__( storeid ), KFEnum::Set, kfsetting->_store_id );
        }

        // 刷新商品
        _kf_store->RefreshGoods( player, kfsetting->_store_id );
    }

    bool KFStoreUnlockModule::IsStoreUnlock( KFEntity* player, uint32 storeid )
    {
        auto kfindexrecord = player->Find( __STRING__( storeindex ) );
        if ( kfindexrecord == nullptr )
        {
            return true;
        }

        for ( auto kfindex = kfindexrecord->First(); kfindex != nullptr; kfindex = kfindexrecord->Next() )
        {
            auto unlockstoreid = kfindex->Get<uint32>( __STRING__( storeid ) );
            if ( unlockstoreid == storeid )
            {
                return true;
            }
        }

        return false;
    }

#define __UPDATE_UNLOCK_LIST__( updatefunction )\
    auto kfindexrecord = player->Find( __STRING__( storeindex ) );\
    auto kfunlockrecord = player->Find( __STRING__( storeunlock ) );\
    if ( kfindexrecord == nullptr || kfunlockrecord == nullptr )\
    {\
        return; \
    }\
    UInt32List removes; \
    std::set< const KFStoreUnlockSetting* > _update_lock; \
    for ( auto kfunlock = kfunlockrecord->First(); kfunlock != nullptr; kfunlock = kfunlockrecord->Next() )\
    {\
        auto unlockid = kfunlock->Get<uint32>( __STRING__( id ) );\
        auto kfsetting = KFStoreUnlockConfig::Instance()->FindSetting( unlockid );\
        if ( kfsetting == nullptr )\
        {\
            removes.push_back( unlockid );\
        }\
        else\
        {\
            _update_lock.insert( kfsetting ); \
        }\
    }\
    for ( auto unlockid : removes )\
    {\
        kfunlockrecord->Remove( unlockid );\
    }\
    for ( auto kfsetting : _update_lock )\
    {\
        auto kfunlock = kfunlockrecord->Find( kfsetting->_id );\
        if ( kfunlock == nullptr )\
        {\
            continue;\
        }\
        auto kfconditionobject = kfunlock->Find( __STRING__( conditions ) );\
        auto complete = updatefunction;\
        if ( complete )\
        {\
            kfunlockrecord->Remove( kfsetting->_id );\
            UnlockStoreData( player, kfsetting, kfindexrecord );\
        }\
    }\

    __KF_ADD_DATA_FUNCTION__( KFStoreUnlockModule::OnAddDataUnlockStore )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateAddCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata ) );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFStoreUnlockModule::OnRemoveDataUnlockStore )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateRemoveCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata ) );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFStoreUnlockModule::OnUpdateDataUnlockStore )
    {
        __UPDATE_UNLOCK_LIST__( _kf_condition->UpdateUpdateCondition( player, kfconditionobject, KFConditionEnum::LimitNull, kfdata, operate, value, newvalue ) );
    }
}
