#include "KFRecordConfig.hpp"

namespace KFrame
{
    uint32 KFRecordConfig::GetExpireNum( uint32 recordtype )
    {
        auto setting = FindSetting( recordtype );
        if ( setting == nullptr )
        {
            return 0u;
        }

        return setting->_expire_num;
    }

    void KFRecordConfig::ReadSetting( KFNode& xmlnode, KFRecordSetting* kfsetting )
    {
        kfsetting->_record = xmlnode.GetString( "Record", true );
        kfsetting->_newrecord = xmlnode.GetString( "NewRecord", true );
        kfsetting->_onlinetime = xmlnode.GetString( "OnlineTime", true );
        kfsetting->_notice = xmlnode.GetUInt32( "Notice", true );
        kfsetting->_login_update = xmlnode.GetUInt32( "LoginUpdate", true );
        kfsetting->_expire_type = xmlnode.GetUInt32( "ExpireType", true );
        kfsetting->_expire_num = xmlnode.GetUInt32( "ExpireNum", true );
    }
}