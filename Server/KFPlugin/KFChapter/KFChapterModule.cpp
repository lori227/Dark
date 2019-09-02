#include "KFChapterModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFChapterModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
    }

    void KFChapterModule::BeforeShut()
    {
    }

}