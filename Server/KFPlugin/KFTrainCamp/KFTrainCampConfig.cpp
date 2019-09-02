#include "KFTrainCampConfig.hpp"

namespace KFrame
{
    void KFTrainCampConfig::ReadSetting( KFNode& xmlnode, KFTrainCampSetting* kfsetting )
    {
        kfsetting->_level = xmlnode.GetUInt32( "Id" );
        kfsetting->_count = xmlnode.GetUInt32( "Count" );
        kfsetting->_total_time = xmlnode.GetUInt64( "TotalTime" );
        kfsetting->_cd_time = xmlnode.GetUInt64( "CdTime" );
        kfsetting->_add_exp = xmlnode.GetUInt32( "AddExp" );
    }
}