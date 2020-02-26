#include "KFInnerWorldConfig.hpp"

namespace KFrame
{
    void KFInnerWorldConfig::ReadSetting( KFNode& xmlnode, KFInnerWorldSetting* kfsetting )
    {
        kfsetting->_reduced_treatment = xmlnode.GetUInt32( "ReducedTreatment" );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////


}