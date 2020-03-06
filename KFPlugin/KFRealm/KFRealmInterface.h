#ifndef __KF_REALM_INTERFACE_H__
#define __KF_REALM_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRealmData;
    typedef std::function< void( KFEntity*, KFRealmData*, uint32 ) > KFEnterRealmFunction;

    class KFRealmInterface : public KFModule
    {
    public:
        // 获得秘境数据
        virtual KFRealmData* GetRealmData( KFEntity* player ) = 0;

        // 判断是否在里世界
        virtual bool IsInnerWorld( KFEntity* player ) = 0;

        // 获取生命回复比列
        virtual double GetAddHpRate( KFEntity* player ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterEnterRealmFunction( T* object, void ( T::*handle )( KFEntity*, KFRealmData*, uint32 ) )
        {
            KFEnterRealmFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindEnterRealmFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterEnterRealmFunction( T* object )
        {
            UnBindEnterRealmFunction( typeid( T ).name() );
        }

    protected:
        virtual void BindEnterRealmFunction( const std::string& module, KFEnterRealmFunction& function ) = 0;
        virtual void UnBindEnterRealmFunction( const std::string& module ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_realm, KFRealmInterface );
    //////////////////////////////////////////////////////////////////////////
#define __KF_ENTER_REALM_FUNCTION__( function ) void function( KFEntity* player, KFRealmData* kfrealmdata, uint32 entertype )
#define __REGISTER_ENTER_REALM__( function ) _kf_realm->RegisterEnterRealmFunction( this, function )
#define __UN_ENTER_REALM__() _kf_realm->UnRegisterEnterRealmFunction( this )

}



#endif