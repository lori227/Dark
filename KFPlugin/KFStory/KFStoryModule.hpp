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

        // 添加剧情
        __KF_ADD_DATA_FUNCTION__( OnAddStoryCallBack );

        // 删除剧情
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveStoryCallBack );

        // 删除对话
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveDialogueCallBack );

        // 剧情序列更新
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateSequenceCallBack );

        // 更新剧情序列请求
        __KF_MESSAGE_FUNCTION__( HandleUpdateStoryReq );

        // 开始剧情请求
        __KF_MESSAGE_FUNCTION__( HandleStartStoryReq );

    public:
        // 添加剧情
        void AddStory( KFEntity* player, uint32 storyid );

        // 检查主剧情
        void CheckMainStory( KFEntity* player );

        // 序列更新
        void OnExecuteSequence( KFEntity* player, uint32 storyid, uint32 sequence );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };
}



#endif