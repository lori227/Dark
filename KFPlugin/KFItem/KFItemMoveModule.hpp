#ifndef __KF_ITEM_MOVE_MOUDLE_H__
#define __KF_ITEM_MOVE_MOUDLE_H__

/************************************************************************
//    @Module			:    物品移动模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2020-2-28
************************************************************************/
#include "KFrameEx.h"
#include "KFItemIndex.hpp"
#include "KFItemInterface.h"
#include "KFItemMoveInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFItemTypeConfig.hpp"

namespace KFrame
{
    class KFItemMoveModule : public KFItemMoveInterface
    {
    public:
        KFItemMoveModule() = default;
        ~KFItemMoveModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 清空包裹
        virtual void CleanItem( KFEntity* player, const std::string& name, bool isauto = false );

        // 移动道具
        virtual uint32 MoveItem( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord, uint32 targetindex );
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

    protected:
        // 进入游戏
        __KF_AFTER_ENTER_PLAYER_FUNCTION__( OnEnterItemMoveModule );

        // 离开游戏
        __KF_LEAVE_PLAYER_FUNCTION__( OnLeaveItemMoveModule );

        // 添加物品回调
        __KF_ADD_DATA_FUNCTION__( OnAddItemMoveLogic );

        // 删除物品回调
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveItemMoveLogic );

        // 更新道具索引回调
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateItemMoveCallBack );
        ///////////////////////////////////////////////////////////////////////////
        // 包裹数量
        __KF_EXECUTE_FUNCTION__( OnExecuteItemMaxCount );
    protected:
        // 初始化道具格子信息
        void InitItemEmptyIndexData( KFEntity* player, KFData* kfitemrecord );

        // 删除格子信息
        void UnInitItemEmptyIndexData( KFEntity* player, const std::string& name );

        // 查找空的格子存放道具
        uint32 GetItemEmptyIndex( KFEntity* player, KFData* kfitemrecord, uint64 uuid );

        // 清空格子信息
        void AddItemEmptyIndex( KFEntity* player, KFData* kfitemrecord, uint32 index );

        // 查找索引的道具
        KFData* FindIndexItem( KFEntity* player, KFData* kfitemrecord, uint32 index, const KFItemSetting* kfsetting );

        // 最大索引
        uint32 GetItemMaxIndex( KFEntity* player, KFData* kfitemrecord );

        // 添加最大索引
        void AddItemMaxIndex( KFEntity* player, KFData* kfitemrecord, uint32 count );

        // 索引是否为空
        bool IsIndexEmpty( KFEntity* player, KFData* kfitemrecord, uint32 index );

        // 删除索引
        void RemoveItemEmptyIndex( KFEntity* player, KFData* kfitemrecord, uint32 index, uint64 uuid );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 计算可以叠加物品数量
        uint32 CalcItemAddCount( uint32 sourcecount, uint32 targetcount, uint32 maxcount );

        // 判断是否能移动道具
        bool CheckItemCanMove( const KFItemSetting* kfsetting, const std::string& sourcename, const std::string& targetname );

        // 找到可以移动的背包
        KFData* FindItemMoveRecord( KFEntity* player, const KFItemSetting* kfsetting, const std::string& sourcename, const std::string& excludename );

        // 判断是否能合并
        bool CheckItemCanMerge( const KFItemSetting* kfsourcesetting, KFData* kfsourceitem, const KFItemSetting* kftargetsetting, KFData* kftargetitem );

        // 拆分道具
        uint32 SplitItem( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, uint32 splitcount, KFData* kftargetrecord, uint32 splitindex );

        // 移动道具
        bool MoveItemDataToRecord( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord );
        void MoveItemCount( KFEntity* player, KFData* kfitem, uint32 operate, uint32 count );

        // 交换道具
        uint32 ExchangeItem( KFEntity* player, KFData* kfsourcerecord, KFData* kfsourceitem, const KFItemSetting* kfsourcesetting, KFData* kftargetrecord, KFData* kftargetitem, const KFItemSetting* kftargetsetting );

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
    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;

        // 保存玩家的背包格子信息
        typedef std::pair<uint64, std::string> ItemIndexKey;
        KFMap< ItemIndexKey, const ItemIndexKey&, KFItemIndex > _player_item_index;
    };
}

#endif