#ifndef __KF_ITEM_INTERFACE_H__
#define __KF_ITEM_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFItemSetting;
    typedef std::function< void( KFEntity*, KFData*, const KFItemSetting* ) > KFItemFunction;

    class KFItemInterface : public KFModule
    {
    public:
        // 创建一个道具
        virtual KFData* CreateItem( KFEntity* player, uint32 itemid, KFData* kfitem, const char* function, uint32 line ) = 0;

        // 清空包裹
        virtual void CleanItem( KFEntity* player, const std::string& name, bool isauto = false ) = 0;

        // 获取能添加物品数量
        virtual uint32 GetCanAddItemCount( KFEntity* player, uint32 itemid, uint32 num ) = 0;

        // 获取物品自动操作类型
        virtual uint32 GetItemAutoType( uint32 itemid ) = 0;

        // 查找空的格子存放道具
        virtual uint32 FindItemEmptyIndex( KFEntity* player, KFData* kfitemrecord ) = 0;

        // 清空格子信息
        virtual void AddItemEmptyIndex( KFEntity* player, KFData* kfitem ) = 0;

        // 查找道具
        virtual std::tuple<KFData*, KFData*> FindItem( KFEntity* player, uint64 itemuuid ) = 0;

        // 判断包裹是否满了
        virtual bool IsItemRecordFull( KFEntity* player, KFData* kfitemrecord ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 初始化数据
        template< class T >
        void RegisteInitItemFunction( uint32 itemtype, T* object, void ( T::*handle )( KFEntity*, KFData*, const KFItemSetting* ) )
        {
            KFItemFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            BindInitItemFunction( itemtype, function );
        }
        virtual void UnRegisteInitItemFunction( uint32 itemtype ) = 0;

    protected:
        virtual void BindInitItemFunction( uint32 itemtype, KFItemFunction& function ) = 0;
    };
    __KF_INTERFACE__( _kf_item, KFItemInterface );

#define __KF_INIT_ITEM_FUNCTION__( function )\
    void function( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
#define __REGISTER_INIT_ITEM__( itemtype, function )\
    _kf_item->RegisteInitItemFunction( itemtype, this, function )
#define __UN_INIT_ITEM__( itemtype )\
    _kf_item->UnRegisteInitItemFunction( itemtype )

    //////////////////////////////////////////////////////////////////////////
    class KFItemUseInterface : public KFModule
    {
    public:

    };
    __KF_INTERFACE__( _kf_item_use, KFItemUseInterface );

    //////////////////////////////////////////////////////////////////////////
    class KFItemWeaponInterface : public KFModule
    {
    public:
    };
    __KF_INTERFACE__( _kf_weapon, KFItemWeaponInterface );
    //////////////////////////////////////////////////////////////////////////
}

#endif