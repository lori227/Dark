#ifndef __KF_RECORD_CONFIG_H__
#define __KF_RECORD_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFRecordSetting : public KFIntSetting
    {
    public:
        // ��¼
        std::string _record = "";

        // �¼�¼
        std::string _newrecord = "";

        // ����ѯʱ��
        std::string _onlinetime = "";

        // ����Ϣ����
        uint32 _notice = 0u;

        // �Ƿ���Ҫ���߸���
        bool _login_update = false;
    };

    class KFRecordConfig : public KFIntConfigT< KFRecordSetting >, public KFInstance< KFRecordConfig >
    {
    public:
        KFRecordConfig()
        {
            _file_name = "record";
        }

    protected:

        // ��ȡ����
        virtual void ReadSetting( KFNode& xmlnode, KFRecordSetting* kfsetting );
    };
}

#endif