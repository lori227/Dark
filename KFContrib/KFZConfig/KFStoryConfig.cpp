#include "KFStoryConfig.hpp"

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
    //////////////////////////////////////////////////////////////////////////////////////////////////
}