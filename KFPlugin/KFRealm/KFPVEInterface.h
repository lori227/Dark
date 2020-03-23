#ifndef __KF_PVE_INTERFACE_H__
#define __KF_PVE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRealmData;
    typedef std::function< void( KFEntity*, KFRealmData*, KFRealmData* ) > KFPVEStartFunction;
    typedef std::function< void( KFEntity*, KFRealmData*, KFRealmData*, uint32, uint32 ) > KFPVEFinishFunction;
    typedef std::function< void( KFEntity*, KFRealmData*, KFRealmData*, uint32 ) > KFTurnStartFunction;
    typedef std::function< void( KFEntity*, KFRealmData*, KFRealmData*, uint32 ) > KFTurnFinishFunction;

    class KFPVEInterface : public KFModule
    {
    public:
        // 操作(里世界)信仰值
        virtual void OperateFaith( KFEntity* player, uint32 operate, uint32 value ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////
        // 开始战斗
        template< class T >
        void RegisterPVEStartFunction( T* object, void ( T::*handle )( KFEntity*, KFRealmData*, KFRealmData* ) )
        {
            KFPVEStartFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
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
        void RegisterPVEFinishFunction( T* object, void ( T::*handle )( KFEntity*, KFRealmData*, KFRealmData*, uint32, uint32 ) )
        {
            KFPVEFinishFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5 );
            BindPVEFinishFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterPVEFinishFunction( T* object )
        {
            UnBindPVEFinishFunction( typeid( T ).name() );
        }
        //////////////////////////////////////////////////////////////////////////////////////
        // 回合开始
        template< class T >
        void RegisterTurnStartFunction( T* object, void ( T::*handle )( KFEntity*, KFRealmData*, KFRealmData*, uint32 ) )
        {
            KFTurnStartFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindTurnStartFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterTurnStartFunction( T* object )
        {
            UnBindTurnStartFunction( typeid( T ).name() );
        }
        //////////////////////////////////////////////////////////////////////////////////////
        // 回合结束
        template< class T >
        void RegisterTurnFinishFunction( T* object, void ( T::*handle )( KFEntity*, KFRealmData*, KFRealmData*, uint32 ) )
        {
            KFTurnFinishFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            BindTurnFinishFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterTurnFinishFunction( T* object )
        {
            UnBindTurnFinishFunction( typeid( T ).name() );
        }
    protected:
        virtual void BindPVEStartFunction( const std::string& module, KFPVEStartFunction& function ) = 0;
        virtual void UnBindPVEStartFunction( const std::string& module ) = 0;

        virtual void BindPVEFinishFunction( const std::string& module, KFPVEFinishFunction& function ) = 0;
        virtual void UnBindPVEFinishFunction( const std::string& module ) = 0;

        virtual void BindTurnStartFunction( const std::string& module, KFTurnStartFunction& function ) = 0;
        virtual void UnBindTurnStartFunction( const std::string& module ) = 0;

        virtual void BindTurnFinishFunction( const std::string& module, KFTurnFinishFunction& function ) = 0;
        virtual void UnBindTurnFinishFunction( const std::string& module ) = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_pve, KFPVEInterface );
    //////////////////////////////////////////////////////////////////////////
#define __KF_PVE_START_FUNCTION__( function ) void function( KFEntity* player, KFRealmData* kfrealmdata, KFRealmData* kfpvedata )
#define __REGISTER_PVE_START__( function ) _kf_pve->RegisterPVEStartFunction( this, function )
#define __UN_PVE_START__() _kf_pve->UnRegisterPVEStartFunction( this )

#define __KF_PVE_FINISH_FUNCTION__( function ) void function( KFEntity* player, KFRealmData* kfrealmdata, KFRealmData* kfpvedata, uint32 result, uint32 truns )
#define __REGISTER_PVE_FINISH__( function ) _kf_pve->RegisterPVEFinishFunction( this, function )
#define __UN_PVE_FINISH__() _kf_pve->UnRegisterPVEFinishFunction( this )

#define __KF_TURN_START_FUNCTION__( function ) void function( KFEntity* player, KFRealmData* kfrealmdata, KFRealmData* kfpvedata, uint32 trun )
#define __REGISTER_TURN_START__( function ) _kf_pve->RegisterTurnStartFunction( this, function )
#define __UN_TURN_START__() _kf_pve->UnRegisterTurnStartFunction( this )

#define __KF_TURN_FINISH_FUNCTION__( function ) void function( KFEntity* player, KFRealmData* kfrealmdata, KFRealmData* kfpvedata, uint32 trun )
#define __REGISTER_TURN_FINISH__( function ) _kf_pve->RegisterTurnFinishFunction( this, function )
#define __UN_TURN_FINISH__() _kf_pve->UnRegisterTurnFinishFunction( this )
}



#endif