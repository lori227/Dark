#ifndef __KF_PINCH_FACE_CONFIG_H__
#define __KF_PINCH_FACE_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFPinchFaceSetting : public KFIntSetting
    {
    public:
        // 名字
        std::string _name;

        KFRange<double> _range;
    };
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    class KFPinchFaceConfig : public KFConfigT< KFPinchFaceSetting >, public KFInstance< KFPinchFaceConfig >
    {
    public:
        KFPinchFaceConfig()
        {
            _file_name = "pinchface";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFPinchFaceSetting* kfsetting );
    };
}

#endif