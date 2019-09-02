#ifndef __KF_RECORD_CLIENT_INTERFACE_H__
#define __KF_RECORD_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    typedef std::function< void( KFEntity*, uint64, uint64, MapString& ) > KFRecordValuesFunction;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFRecordClientInterface : public KFModule
    {
    public:
        // 添加营地记录(玩家 英雄 时间戳 建筑类型 数据)
        virtual void AddCampRecord( KFEntity* player, KFData* kfhero, uint64 time, uint32 type, uint64 value ) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterRecordValueFunction( T* object, void ( T::*handle )( KFEntity*, uint64, uint64, MapString& ) )
        {
            KFRecordValuesFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            AddRecordValueFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterRecordValueFunction( T* object )
        {
            RemoveRecordValueFunction( typeid( T ).name() );
        }

    protected:
        virtual void AddRecordValueFunction( const std::string& module, KFRecordValuesFunction& function ) = 0;
        virtual void RemoveRecordValueFunction( const std::string& module ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_record_client, KFRecordClientInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define  __KF_RECORD_VALUE_FUNCTION__( function ) void function( KFEntity* player, uint64 lasttime, uint64 nowtime, MapString& values )
#define  __REGISTER_RECORD_VALUE__( function ) _kf_record_client->RegisterRecordValueFunction( this, function )
#define  __UN_RECORD_VALUE__() _kf_record_client->UnRegisterRecordValueFunction( this )
}

#endif