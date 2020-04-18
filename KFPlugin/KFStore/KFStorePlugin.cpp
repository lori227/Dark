#include "KFStorePlugin.hpp"
#include "KFStoreModule.hpp"
#include "KFGoodsModule.hpp"
#include "KFStoreUnlockModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFStorePlugin::Install()
    {
        __REGISTER_MODULE__( KFStore );
        __REGISTER_MODULE__( KFStoreUnlock );
        __REGISTER_MODULE__( KFGoods );
    }

    void KFStorePlugin::UnInstall()
    {
        __UN_MODULE__( KFStore );
        __UN_MODULE__( KFStoreUnlock );
        __UN_MODULE__( KFGoods );
    }

    void KFStorePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_reset, KFResetInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_condition, KFConditionInterface );
        __FIND_MODULE__( _kf_goods, KFGoodsInterface );
        __FIND_MODULE__( _kf_store_unlock, KFStoreUnlockInterface );
    }

    void KFStorePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFStoreConfig );
        __KF_ADD_CONFIG__( KFGoodsConfig );
        __KF_ADD_CONFIG__( KFStoreUnlockConfig );
        __KF_ADD_CONFIG__( KFCountCostConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
    }
}