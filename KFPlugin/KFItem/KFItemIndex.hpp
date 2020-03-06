#ifndef __KF_ITEM_INDEX_H__
#define __KF_ITEM_INDEX_H__
#include "KFrame.h"

namespace KFrame
{
#define __BEGIN_ITEM_INDEX__ 1u

    class KFItemIndex
    {
    public:
        // 初始化
        void InitMaxIndex( uint32 maxvalue )
        {
            _emptys.clear();
            _uuids.clear();
            for ( auto i = __BEGIN_ITEM_INDEX__; i <= maxvalue; ++i )
            {
                _emptys.insert( i );
            }

            _max_count = maxvalue;
        }

        // 添加索引
        void AddMaxIndex( uint32 count )
        {
            for ( auto i = __BEGIN_ITEM_INDEX__; i <= count; ++i )
            {
                _emptys.insert( i + _max_count );
            }

            _max_count += count;
        }

        uint32 FindEmpty( uint64 uuid )
        {
            auto iter = _emptys.begin();
            if ( iter == _emptys.end() )
            {
                return 0u;
            }

            auto index = *iter;
            _emptys.erase( iter );

            AddUUID( index, uuid );
            return index;
        }

        void AddEmpty( uint32 index )
        {
            if ( index >= __BEGIN_ITEM_INDEX__ )
            {
                _emptys.insert( index );
                RemoveUUID( index );
            }
        }

        void RemoveEmpty( uint32 index, uint64 uuid )
        {
            _emptys.erase( index );
            AddUUID( index, uuid );
        }

        // 最大索引
        uint32 MaxIndex() const
        {
            return _max_count;
        }

        bool IsEmpty( uint32 index )
        {
            return _emptys.find( index ) != _emptys.end();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        void AddUUID( uint32 index, uint64 uuid )
        {
            _uuids[ index ] = uuid;
        }

        void RemoveUUID( uint32 index )
        {
            _uuids.erase( index );
        }

        uint64 GetUUID( uint32 index )
        {
            auto iter = _uuids.find( index );
            if ( iter == _uuids.end() )
            {
                return 0u;
            }

            return iter->second;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
    public:
        // 最大数量
        uint32 _max_count = 0u;

        // 空索引列表
        UInt32Set _emptys;

        // 索引对应的物品uuid
        UInt64Map _uuids;
    };
}

#endif