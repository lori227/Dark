#ifndef __KF_PVE_INTERFACE_H__
#define __KF_PVE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRealmData;
    typedef std::function< void( KFEntity*, KFRealmData* ) > KFPVEStartFunction;
    typedef std::function< void( KFEntity*, KFRealmData*, uint32 ) > KFPVEFinishFunction;

    class KFPVEInterface : public KFModule
    {
    public:
        // 操作(里世界)信仰值
        virtual void OperateFaith( KFEntity* player, uint32 operate, uint32 value ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////
        // 开始战斗
        template< class T >
        void RegisterPVEStartFunction( T* object, void ( T::*handle )( KFEntity*, KFRealmData* ) )
        {
            KFPVEStartFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2 );
            BindPVEStartFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterPVEStartFunction( T* object )
        {
            UnBindPVEStartFunction( typeid( T ).name() );
        }
        //////////////////////////////////////////////////////////////////////////////////////
        // 结束战斗
        template< class T >
        void RegisterPVEFinishFunction( T* object, void ( T::*handle )( KFEntity*, KFRealmData*, uint32 ) )
        {
            KFPVEFinishFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindPVEFinishFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterPVEFinishFunction( T* object )
        {
            UnBindPVEFinishFunction( typeid( T ).name() );
        }
    protected:
        virtual void BindPVEStartFunction( const std::string& module, KFPVEStartFunction& function ) = 0;
        virtual void UnBindPVEStartFunction( const std::string& module ) = 0;

        virtual void BindPVEFinishFunction( const std::string& module, KFPVEFinishFunction& function ) = 0;
        virtual void UnBindPVEFinishFunction( const std::string& module ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_pve, KFPVEInterface );
    //////////////////////////////////////////////////////////////////////////
#define __KF_PVE_START_FUNCTION__( function ) void function( KFEntity* player, KFRealmData* kfpvedata )
#define __REGISTER_PVE_START__( function ) _kf_pve->RegisterPVEStartFunction( this, function )
#define __UN_PVE_START__() _kf_pve->UnRegisterPVEStartFunction( this )

#define __KF_PVE_FINISH_FUNCTION__( function ) void function( KFEntity* player, KFRealmData* kfpvedata, uint32 result )
#define __REGISTER_PVE_FINISH__( function ) _kf_pve->RegisterPVEFinishFunction( this, function )
#define __UN_PVE_FINISH__() _kf_pve->UnRegisterPVEFinishFunction( this )
}



#endif