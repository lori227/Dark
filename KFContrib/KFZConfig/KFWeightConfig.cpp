#include "KFWeightConfig.hpp"

namespace KFrame
{
    // 读取配置
    void KFWeightConfig::ReadSetting( KFNode& xmlnode, KFWeightSetting* kfsetting )
    {
        auto value = xmlnode.GetUInt32( "Value" );
        auto weight = xmlnode.GetUInt32( "Weight", true );
        kfsetting->_weight_list.Create( value, weight );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 查找权重池
    const KFWeightList< KFWeight >* KFWeightConfig::FindWeightPool( uint32 id )
    {
        auto kfsetting = FindSetting( id );
        if ( kfsetting == nullptr )
        {
            return nullptr;
        }

        return &kfsetting->_weight_list;
    }

    const KFWeight* KFWeightConfig::RandWeight( uint32 poolid )
    {
        auto pooldata = FindWeightPool( poolid );
        if ( pooldata == nullptr )
        {
            return nullptr;
        }

        return pooldata->Rand();
    }
}
