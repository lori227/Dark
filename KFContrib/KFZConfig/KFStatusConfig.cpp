#include "KFStatusConfig.hpp"
#include "KFZConfig/KFReadSetting.h"

namespace KFrame
{
    void KFStatusConfig::ReadSetting( KFNode& xmlnode, KFStatusSetting* kfsetting )
    {
        // 读取执行逻辑
        KFReadSetting::ReadExecuteData( xmlnode, &kfsetting->_execute_data );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
}