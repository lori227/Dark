#include "KFGranaryConfig.hpp"

namespace KFrame
{
    void KFGranaryConfig::ReadSetting( KFNode& xmlnode, KFGranarySetting* kfsetting )
    {
        kfsetting->_cd_time = xmlnode.GetUInt64( "CdTime", true );
        kfsetting->_add_num = xmlnode.GetUInt32( "AddNum", true );
        kfsetting->_max_num = xmlnode.GetUInt32( "MaxNum", true );
    }
}