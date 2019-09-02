#include "KFRecordConfig.hpp"

namespace KFrame
{
    void KFRecordConfig::ReadSetting( KFNode& xmlnode, KFRecordSetting* kfsetting )
    {
        kfsetting->_record = xmlnode.GetString( "Record", true );
        kfsetting->_newrecord = xmlnode.GetString( "NewRecord", true );
        kfsetting->_onlinetime = xmlnode.GetString( "OnlineTime", true );
        kfsetting->_notice = xmlnode.GetUInt32( "Notice", true );
        kfsetting->_login_update = xmlnode.GetUInt32( "LoginUpdate", true );
    }
}