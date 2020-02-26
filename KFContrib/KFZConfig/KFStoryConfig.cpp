#include "KFStoryConfig.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFStoryConfig::ReadSetting( KFNode& xmlnode, KFStorySetting* kfsetting )
    {
        auto triggertype = xmlnode.GetUInt32( "TriggerType" );
        if ( triggertype != 0u )
        {
            kfsetting->_trigger_type = triggertype;
        }

        KFStorySequence sequence;
        sequence._sequence = xmlnode.GetUInt32( "Array", true );
        sequence._type = xmlnode.GetUInt32( "Type", true );
        sequence._parameter1 = xmlnode.GetUInt32( "Parameter1" );
        sequence._parameter2 = xmlnode.GetUInt32( "Parameter2" );

        kfsetting->_sequences[sequence._sequence] = sequence;
    }

    void KFStoryConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            // 将章节点类型转化为PVE或realm类型
            for ( auto& it : iter.second->_sequences )
            {
                if ( it.second._type == KFMsg::ProcessChapter )
                {
                    auto kfchaptersetting = KFChapterConfig::Instance()->FindSetting( it.second._parameter1 );
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
                        it.second._type = KFMsg::ProcessPVE;
                        it.second._parameter1 = executedata->_param_list._params[0]->_int_value;
                    }
                    else if ( executedata->_name == "realm" )
                    {
                        it.second._type = KFMsg::ProcessExplore;
                        it.second._parameter1 = executedata->_param_list._params[0]->_int_value;
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}