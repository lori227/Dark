#include "KFChapterConfig.hpp"

namespace KFrame
{
    void KFChapterConfig::ReadSetting( KFNode& xmlnode, KFChapterSetting* kfsetting )
    {
        kfsetting->_init_status_id = xmlnode.GetUInt32( "InitStatus" );
        kfsetting->_finish_status_id = xmlnode.GetUInt32( "FinishStatus" );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////

}