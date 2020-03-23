#ifndef __KF_WEATHER_INTERFACE_H__
#define __KF_WEATHER_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFWeatherInterface : public KFModule
    {
    public:

    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_weather, KFWeatherInterface );
    //////////////////////////////////////////////////////////////////////////
}



#endif