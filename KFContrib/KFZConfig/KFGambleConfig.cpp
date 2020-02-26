#include "KFGambleConfig.hpp"

namespace KFrame
{
    void KFGambleConfig::ReadSetting( KFNode& xmlnode, KFGambleSetting* kfsetting )
    {
        kfsetting->_cost_item_id = xmlnode.GetUInt32( "CostItemId" );
        kfsetting->_init_count = xmlnode.GetUInt32( "Price" );
        kfsetting->_max_count = xmlnode.GetUInt32( "MaxPrice" );

        auto strparams = xmlnode.GetString( "Params" );
        KFUtility::SplitList( kfsetting->_params, strparams, __SPLIT_STRING__ );

        kfsetting->_show_count = xmlnode.GetUInt32( "ShowCount" );

        kfsetting->_normal_drop_id = xmlnode.GetUInt32( "NormalDrop" );
        kfsetting->_inner_drop_id = xmlnode.GetUInt32( "InnerDrop" );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFGambleConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            KFElementConfig::Instance()->FormatElemnt( kfsetting->_cost_item, __STRING__( item ), "1", kfsetting->_cost_item_id );
        }
    }

}