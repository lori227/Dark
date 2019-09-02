﻿#ifndef __KF_GENERATE_PLUGIN_H__
#define __KF_GENERATE_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
    class KFGeneratePlugin : public KFPlugin
    {
    public:
        // 注册
        virtual void Install();

        // 卸载
        virtual void UnInstall();

        // 加载
        virtual void LoadModule();


        // 配置
        virtual void AddConfig();
    };
}


#endif