#ifndef __KF_ITEM_MOUDLE_H__
#define __KF_ITEM_MOUDLE_H__

/************************************************************************
//    @Module			:    物品系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-22
************************************************************************/
#include "KFrameEx.h"
#include "KFItemInterface.h"
#include "KFLua/KFLuaInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFItemTypeConfig.hpp"

namespace KFrame
{
#define __BEGIN_ITEM_INDEX__ 1u

    class KFItemIndex
    {
    public:
        // 初始化
        void InitMaxIndex( uint32 maxvalue )
        {
            _indexs.clear();
            for ( auto i = __BEGIN_ITEM_INDEX__; i <= maxvalue; ++i )
            {
                _indexs.insert( i );
            }

            _max_count = maxvalue;
        }

        // 添加索引
        void AddMaxIndex( uint32 count )
        {
            for ( auto i = __BEGIN_ITEM_INDEX__; i <= count; ++i )
            {
                _indexs.insert( i + _max_count );
            }

            _max_count += count;
        }

        uint32 FindEmpty()
        {
            auto iter = _indexs.begin();
            if ( iter == _indexs.end() )
            {
                return 0u;
            }

            auto index = *iter;
            _indexs.erase( iter );
            return index;
        }

        void AddEmpty( uint32 index )
        {
            _indexs.insert( index );
        }

        void RemoveEmpty( uint32 index )
        {
            _indexs.erase( index );
        }

        // 最大索引
        uint32 MaxIndex() const
        {
            return _max_count;
        }

    public:
        // 最大数量
        uint32 _max_count = 0u;

        // 空索引列表
        UInt32Set _indexs;
    };

    class KFItemModule : public KFItemInterface
    {
    public:
        KFItemModule() = default;
        ~KFItemModule() = default;

        // 逻辑
        virtual void BeforeRun();
        virtual void PrepareRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 创建一个道具
        virtual KFData* CreateItem( KFEntity* player, uint32 itemid, KFData* kfitem, const char* function, uint32 line );

        // 清空包裹
        virtual void CleanItem( KFEntity* player, const std::string& name, bool isauto = false );

        // 获取能添加物品数量
        virtual uint32 GetCanAddItemCount( KFEntity* player, uint32 itemid, uint32 num );

        // 获取物品自动操作类型
        virtual uint32 GetItemAutoType( uint32 itemid );

        // 查找空的格子存放道具
        virtual uint32 FindItemEmptyIndex( KFEntity* player, KFData* kfitemrecord );

        // 清空格子信息
        virtual void AddItemEmptyIndex( KFEntity* player, KFData* kfitem );

        // 查找道具
        virtual std::tuple<KFData*, KFData*> FindItem( KFEntity* player, uint64 itemuuid );

        // 判断包裹是否满了
        virtual bool IsItemRecordFull( KFEntity* player, KFData* kfitemrecord );
    protected:
        // 拆分道具
        __KF_MESSAGE_FUNCTION__( HandleSplitItemReq );

        // 合并道具
        __KF_MESSAGE_FUNCTION__( HandleMergeItemReq );

        // 移动道具(从背包到仓库 或者从仓库到背包)
        __KF_MESSAGE_FUNCTION__( HandleMoveItemReq );
        __KF_MESSAGE_FUNCTION__( HandleMoveAllItemReq );

        // 交换道具(从背包到仓库 或者从仓库到背包)
        __KF_MESSAGE_FUNCTION__( HandleExchangeItemReq );

        // 移动某包裹道具
        __KF_MESSAGE_FUNCTION__( HandleCleanItemReq );

        // 整理背包
        __KF_MESSAGE_FUNCTION__( HandleSortItemReq );

        // 删除道具
        __KF_MESSAGE_FUNCTION__( HandleRemoveItemReq );
    protected:
        // 获得道具数量
        __KF_GET_CONFIG_VALUE_FUNCTION__( GetItemTotalCount );

        // 判断是否能添加物品
        __KF_CHECK_ADD_ELEMENT_FUNCTION__( CheckAddItemElement );

        // 添加物品
        __KF_ADD_ELEMENT_FUNCTION__( AddItemElement );

        // 判断物品
        __KF_CHECK_REMOVE_ELEMENT_FUNCTION__( CheckRemoveItemElement );

        // 删除物品
        __KF_REMOVE_ELEMENT_FUNCTION__( RemoveItemElement );

        // 添加物品回调
        __KF_ADD_DATA_FUNCTION__( OnAddItemCallBack );

        // 物品数量更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnItemCountUpdateCallBack );

        // 物品时间更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnItemTimeUpdateCallBack );

        // 删除物品回调
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveItemCallBack );

        // 删除时间道具
        __KF_TIMER_FUNCTION__( OnTimerRemoveTimeItem );

        // 进入游戏
        __KF_AFTER_ENTER_PLAYER_FUNCTION__( OnEnterItemModule );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveItemModule );

        // 仓库数量
        __KF_EXECUTE_FUNCTION__( OnExecuteItemMaxCount );

    protected:
        virtual void BindInitItemFunction( uint32 itemtype, KFItemFunction& function );
        virtual void UnRegisteInitItemFunction( uint32 itemtype );
    protected:
        // 背包格子最大数量
        uint32 GetItemRecordMaxCount( KFEntity* player, KFData* kfitemrecord );

        //背包是否满了
        bool CheckItemRecordFull( KFEntity* player, KFData* kfitemrecord, const KFItemSetting* kfsetting, uint32 itemcount );

        // 获得背包
        KFData* FindItemRecord( KFEntity* player, const KFItemSetting* kfsetting, uint32 itemcount );

        // 添加时间叠加数量
        void AddOverlayTimeItem( KFEntity* player, KFData* kfparent, KFElementResult* kfresult, const KFItemSetting* kfsetting, uint32 count, uint32 time );

        // 添加数量叠加道具
        void AddOverlayCountItem( KFEntity* player, KFData* kfparent, KFElementResult* kfresult, const KFItemSetting* kfsetting, uint32 count );

        // 不能叠加的道具
        void AddNotOverlayItem( KFEntity* player, KFData* kfparent, KFElementResult* kfresult, const KFItemSetting* kfsetting, uint32 count );

        // 添加新物品
        KFData* AddNewItemData( KFEntity* player, KFData* kfparent, const KFItemSetting* kfsetting, uint32& count, uint32 time );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 调用函数lua函数
        bool CallItemLuaFunction( KFEntity* player, uint32 functiontype, const KFItemSetting* kfsetting, uint32 itemcount );

        // 启动检查时间道具
        void StartItemCheckTimer( KFEntity* player, KFData* kfitem );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化道具格子信息
        void InitItemEmptyIndexData( KFEntity* player, KFData* kfitemrecord );

        // 删除格子信息
        void UnInitItemEmptyIndexData( KFEntity* player, const std::string& name );

        // 设置道具格子信息
        void RemoveItemEmptyIndex( KFEntity* player, KFData* kfitem );

        // 查找索引的道具
        KFData* FindIndexItem( KFData* kfitemrecord, uint32 index );

        // 最大索引
        uint32 GetItemMaxIndex( KFEntity* player, KFData* kfitemrecord );

        // 添加最大索引
        void AddItemMaxIndex( KFEntity* player, KFData* kfitemrecord, uint32 count );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 判断是否能移动道具
        bool CheckItemCanMove( const KFItemSetting* kfsetting, const std::string& sourcename, const std::string& targetname );

        // 计算道具添加数量
        uint32 CalcItemAddCount( uint32 sourcecount, uint32 targetcount, uint32 maxcount );

        // 判断是否能交换
        bool CheckItemCanExchange( const KFItemSetting* kfsourcesetting, KFData* kfsourceitem, const KFItemSetting* kftargetsetting, KFData* kftargetitem );

        // 拆分道具
        uint32 SplitItem( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, uint32 splitcount, KFData* kftargetrecord, uint32 splitindex );

        // 移动道具
        uint32 MoveItem( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord, uint32 targetindex );
        void MoveItemDataToRecord( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord );
        void MoveItemCount( KFEntity* player, KFData* kfitem, uint32 operate, uint32 count );

        // 交换道具
        uint32 ExchangeItem( KFEntity* player, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord, KFData* kftargetitem );

        // 合并道具
        uint32 MergeItem( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, uint32 mergecount, KFData* kftargetrecord, KFData* kftargetitem );

        // 排序道具
        void SortItem( KFEntity* player, const std::string& name );
        void SortItem( KFEntity* player, KFData* kfitemrecord, KFItemIndex* kfindex, const KFItemSetting* kfsetting, std::set<KFData*>& itemlist );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加物品显示
        bool IsExtendItem( const std::string& name );

        // 移动物品显示
        void MoveItemDataToShow( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kftargetrecord, uint64 count );
        void MoveItemDataToShow( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kftargetrecord, KFData* kfsourceitem );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 物品属性列表
        StringVector _item_data_list;

        // 保存玩家的背包格子信息
        typedef std::pair<uint64, std::string> ItemIndexKey;
        KFMap< ItemIndexKey, const ItemIndexKey&, KFItemIndex > _player_item_index;

        // 初始化
        KFBind<uint32, uint32, KFItemFunction > _init_item_function;
    };
}

#endif