#ifndef __KF_REALM_INTERFACE_H__
#define __KF_REALM_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRealmData;
    typedef std::function< void( KFEntity*, KFRealmData*, uint32 ) > KFRealmEnterFunction;
    typedef std::function< void( KFEntity*, KFRealmData*, uint32 ) > KFRealmMoveFunction;

    class KFRealmInterface : public KFModule
    {
    public:
        // 获得秘境数据
        virtual KFRealmData* GetRealmData( KFEntity* player ) = 0;

        // 判断是否在里世界
        virtual bool IsInnerWorld( KFEntity* player ) = 0;

        // 获取生命回复减少比列
        virtual int GetAddHpReduceRate( KFEntity* player ) = 0;

        // 获取里世界参数
        virtual int GetInnerWorldDecHpParam( KFEntity* player ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////
        // 进入秘境
        template< class T >
        void RegisterRealmEnterFunction( T* object, void ( T::*handle )( KFEntity*, KFRealmData*, uint32 ) )
        {
            KFRealmEnterFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindRealmEnterFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterRealmEnterFunction( T* object )
        {
            UnBindRealmEnterFunction( typeid( T ).name() );
        }
        ////////////////////////////////////////////////////////////////////////////////////
        // 玩家移动
        template< class T >
        void RegisterRealmMoveFunction( T* object, void ( T::*handle )( KFEntity*, KFRealmData*, uint32 ) )
        {
            KFRealmMoveFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindRealmMoveFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterRealmMoveFunction( T* object )
        {
            UnBindRealmMoveFunction( typeid( T ).name() );
        }
    protected:
        virtual void BindRealmEnterFunction( const std::string& module, KFRealmEnterFunction& function ) = 0;
        virtual void UnBindRealmEnterFunction( const std::string& module ) = 0;

        virtual void BindRealmMoveFunction( const std::string& module, KFRealmMoveFunction& function ) = 0;
        virtual void UnBindRealmMoveFunction( const std::string& module ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_realm, KFRealmInterface );
    //////////////////////////////////////////////////////////////////////////
#define __KF_REALM_ENTER_FUNCTION__( function ) void function( KFEntity* player, KFRealmData* kfrealmdata, uint32 entertype )
#define __REGISTER_REALM_ENTER__( function ) _kf_realm->RegisterRealmEnterFunction( this, function )
#define __UN_REALM_ENTER__() _kf_realm->UnRegisterRealmEnterFunction( this )

#define __KF_REALM_MOVE_FUNCTION__( function ) void function( KFEntity* player, KFRealmData* kfrealmdata, uint32 movestep )
#define __REGISTER_REALM_MOVE__( function ) _kf_realm->RegisterRealmMoveFunction( this, function )
#define __UN_REALM_MOVE__() _kf_realm->UnRegisterRealmMoveFunction( this )

}



#endif