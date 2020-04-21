#ifndef __KF_STORY_MOUDLE_H__
#define __KF_STORY_MOUDLE_H__

/************************************************************************
//    @Moudle			:    剧情系统
//    @Author           :    erlking
//    @QQ				:    729159320
//    @Mail			    :    729159320@qq.com
//    @Date             :    2020-01-08
************************************************************************/

#include "KFrameEx.h"
#include "KFStoryInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDrop/KFDropInterface.h"
#include "KFExecute/KFExecuteInterface.h"
#include "KFTask/KFTaskInterface.h"
#include "KFZConfig/KFStoryConfig.hpp"
#include "KFZConfig/KFDialogueConfig.hpp"
#include "KFDialogue/KFDialogueInterface.h"

namespace KFrame
{
    class KFStoryModule : public KFStoryInterface
    {
    public:
        KFStoryModule() = default;
        ~KFStoryModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        // 添加剧情
        virtual void AddStory( KFEntity* player, uint32 storyid, uint32 childid, const std::string& modulename, uint64 moduleid );
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterStoryModule );

        // 添加剧情
        __KF_ADD_ELEMENT_FUNCTION__( AddStoryElement );

        // 掉落剧情段落回调
        __KF_DROP_LOGIC_FUNCTION__( OnDropStory );

        // 执行剧情
        __KF_EXECUTE_FUNCTION__( OnExecuteStory );

        // 删除对话
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveDialogueCallBack );

        // 剧情序列更新
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateSequenceCallBack );

        // PVE结算更新
        __KF_UPDATE_DATA_FUNCTION__( OnUpdatePVECallBack );

        // 探索结算更新
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateRealmCallBack );

        // 名字更新
        __KF_UPDATE_STRING_FUNCTION__( OnUpdateNameCallBack );

        // 开始剧情请求
        __KF_MESSAGE_FUNCTION__( HandleStartStoryReq );

        // 更新剧情序列请求
        __KF_MESSAGE_FUNCTION__( HandleUpdateStoryReq );

        // 执行剧情请求
        __KF_MESSAGE_FUNCTION__( HandleExecuteStoryReq );

    public:
        // 检查剧情
        void CheckStory( KFEntity* player );

        // 删除剧情
        void RemoveStory( KFEntity* player, uint32 storyid );

        // 增加序列
        void AddSequence( KFEntity* player, KFData* kfdata, uint32 type );

        // 执行剧情
        uint32 ExecuteStory( KFEntity* player, uint32 storyid );

        // 发送添加剧情数据
        void SendToClientAddStory( KFEntity* player, uint32 storyid, const std::string& modulename, uint64 moduleid );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif