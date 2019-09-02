#ifndef __KF_CONFIG_H__
#define __KF_CONFIG_H__

#include "KFrame.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFSetting
    {
    public:
        // 类型
        typedef T Type;

        // 配置id
        T _id;
    };

    class KFIntSetting : public KFSetting< uint32 >
    {
    public:
        typedef uint32 ParamType;
    };

    class KFStrSetting : public KFSetting< std::string >
    {
    public:
        typedef const std::string& ParamType;
    };
    ///////////////////////////////////////////////////////////////
    class KFConfig
    {
    public:
        // 加载配置
        virtual bool LoadConfig( const std::string& file ) = 0;
    public:
        // 配置文件
        std::string _file;
    };
    ///////////////////////////////////////////////////////////////
    template< class T >
    class KFConfigT : public KFConfig
    {
    public:
        // 加载配置
        bool LoadConfig( const std::string& file )
        {
            ClearSetting();

            KFXml kfxml( file );
            auto config = kfxml.RootNode();
            auto xmlnode = config.FindNode( "item" );
            while ( xmlnode.IsValid() )
            {
                auto kfsetting = CreateSetting( xmlnode );
                if ( kfsetting != nullptr )
                {
                    ReadSetting( xmlnode, kfsetting );
                }
                xmlnode.NextNode();
            }

            return true;
        }

        // 获取配置
        const T* FindSetting( typename T::ParamType id )
        {
            return _settings.Find( id );
        }

    protected:
        // 清空配置
        virtual  void ClearSetting()
        {
            _settings.Clear();
        }

        // 创建配置
        virtual T* CreateSetting( KFNode& xmlnode )
        {
            auto id = xmlnode.GetUInt32( "Id" );
            auto kfsetting = _settings.Create( id );
            kfsetting->_id = id;

            return kfsetting;
        }

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, T* kfsetting )
        {
        }

    public:
        // 列表
        KFHashMap< typename T::Type, typename T::ParamType, T > _settings;
    };
}

#endif