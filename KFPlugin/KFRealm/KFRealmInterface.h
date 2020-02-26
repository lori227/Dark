#ifndef __KF_REALM_INTERFACE_H__
#define __KF_REALM_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRealmData;
    class KFRealmInterface : public KFModule
    {
    public:
        // 获得秘境数据
        virtual KFRealmData* GetRealmData( KFEntity* player ) = 0;

        // 判断是否在里世界
        virtual bool IsInnerWorld( KFEntity* player ) = 0;

        // 获取生命回复比列
        virtual double GetAddHpRate( KFEntity* player ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_realm, KFRealmInterface );
    //////////////////////////////////////////////////////////////////////////
}



#endif