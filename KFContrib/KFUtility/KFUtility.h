﻿#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "KFInclude.h"
#include "KFDefine.h"

namespace KFrame
{
    class KFUtility
    {
    public:
        // 算术操作
        template< class T = uint64 > static T Operate( uint32 operate, T basevalue, T value );

        // 判断是否满足条件
        template< class T = uint64 > static bool CheckOperate( T basevalue, uint32 operate, T operatevalue );

        // 转换成字符串
        template< class T = uint64 > static std::string ToString( const T& value );

        // 转换成数值
        template< class T = uint64 > static T ToValue( const char* srcstring );
        template< class T = uint64 > static T ToValue( const std::string& srcstring );

        // 切割字符串
        template< class T = uint64 > static T SplitValue( std::string& srcstring, const std::string& split );

        // 标记
        template< class T = uint32 > static bool HaveBitMask( T value, T bitmask );
        template< class T = uint32 > static void ClearBitMask( T& value, T bitmask );
        template< class T = uint32 > static void AddBitMask( T& value, T bitmask );

        template< class T > static void SplitList( T& outlist, std::string& srcstring, const std::string& split );
        template< class T > static void SplitSet( T& outlist, std::string& srcstring, const std::string& split );

        template< class T > static bool ParseArrayList( T& arraylist, const std::string& srcstring );
        template< class T > static bool ParseArraySet( T& arrayset, const std::string& srcstring );

        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        // 判断float double 是否为0
        static bool IsZeroFloat( float value, float epsilon = 1e-6 );
        static bool IsZeroDouble( double value, double epsilon = 1e-15 );
        static bool IsNumber( const std::string& value );

        // 计算两点之间距离
        static uint32 CalcDistance( uint32 nowx, uint32 nowy, uint32 targetx, uint32 targety );

        // 判断是否在距离范围内
        static bool CheckInDistance( uint32 nowx, uint32 nowy, uint32 targetx, uint32 targety, uint32 distance );

        // 计算hash值
        static uint32 GetHashValue( const std::string& data );

        // 切割字符串
        static std::string SplitString( std::string& srcstring, const std::string& split );

        // 替换字串
        static void ReplaceString( std::string& srcstring, const std::string& strold, const std::string& strnew );
        static void ReplaceString( std::string& srcstring, const std::string& strold, const StringVector& params );

        // 删除一个字符串中的指定字符串
        static bool DelString( std::string& srcstring, const std::string& delstr, std::string& strnew );

        // 格式化标题
        static std::string FormatTitleText( const std::string& appname, const std::string& apptype, const std::string& appid );

        ////////////////////////////////////////////////////////////////////////////////////////////////
        static std::string FormatConfigFile( const std::string& filename, uint32 channel, uint32 service );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 获得map中最大的值
        static uint32 GetMaxMapValue( const UInt32Map& mapvalues, uint32 value );

        // 按权重随机map
        static uint32 RandMapValue( const UInt32Map& mapvalues, uint32 totalweight, uint32 rand );

        // 随机一个值
        static uint32 RandVectorValue( const UInt32Vector& vectorvalues, const UInt32Set& excludes );

    };
}

#include "KFUtility.inl"

#endif