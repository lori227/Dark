#include "KFInitialProcessConfig.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFInitialProcessConfig::ReadSetting( KFNode& xmlnode, KFInitialProcessSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_parameter = xmlnode.GetUInt32( "parameter1" );
    }

    void KFInitialProcessConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            // 将章节点类型转化为PVE或realm类型
            auto kfsetting = iter.second;
            if ( kfsetting->_type == KFMsg::ProcessChapter )
            {
                auto kfchaptersetting = KFChapterConfig::Instance()->FindSetting( kfsetting->_parameter );
                if ( kfchaptersetting == nullptr )
                {
                    continue;
                }

                auto statusid = kfchaptersetting->_init_status_id;
                auto kfstatussetting = KFStatusConfig::Instance()->FindSetting( statusid );
                if ( kfstatussetting == nullptr )
                {
                    continue;
                }

                auto executedata = &kfstatussetting->_execute_data;
                if ( executedata->_name == "pve" )
                {
                    kfsetting->_type = KFMsg::ProcessPVE;
                    kfsetting->_parameter = executedata->_param_list._params[0]->_int_value;
                }
                else if ( executedata->_name == "realm" )
                {
                    kfsetting->_type = KFMsg::ProcessExplore;
                    kfsetting->_parameter = executedata->_param_list._params[0]->_int_value;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}