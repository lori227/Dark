#include "KFStoryModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFStoryModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ENTER_PLAYER__( &KFStoryModule::OnEnterStoryModule );
        __REGISTER_EXECUTE__( __STRING__( story ), &KFStoryModule::OnExecuteStory );
        __REGISTER_DROP_LOGIC__( __STRING__( story ), &KFStoryModule::OnDropStory );
        __REGISTER_ADD_ELEMENT__( __STRING__( story ), &KFStoryModule::AddStoryElement );

        __REGISTER_ADD_DATA_1__( __STRING__( story ), &KFStoryModule::OnAddStoryCallBack );
        __REGISTER_REMOVE_DATA_1__( __STRING__( story ), &KFStoryModule::OnRemoveStoryCallBack );
        __REGISTER_REMOVE_DATA_1__( __STRING__( dialogue ), &KFStoryModule::OnRemoveDialogueCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( story ), __STRING__( sequence ), &KFStoryModule::OnUpdateSequenceCallBack );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_START_STORY_REQ, &KFStoryModule::HandleStartStoryReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_STORY_REQ, &KFStoryModule::HandleUpdateStoryReq );
    }

    void KFStoryModule::BeforeShut()
    {
        __UN_ENTER_PLAYER__();
        __UN_EXECUTE__( __STRING__( story ) );
        __UN_DROP_LOGIC__( __STRING__( story ) );
        __UN_ADD_ELEMENT__( __STRING__( story ) );

        __UN_ADD_DATA_1__( __STRING__( story ) );
        __UN_REMOVE_DATA_1__( __STRING__( story ) );
        __UN_REMOVE_DATA_1__( __STRING__( dialogue ) );
        __UN_UPDATE_DATA_2__( __STRING__( story ), __STRING__( sequence ) );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_START_STORY_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_STORY_REQ );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFStoryModule::OnEnterStoryModule )
    {
        auto mainstory = player->Get<uint32>( __STRING__( mainstory ) );
        if ( mainstory != 0u )
        {
            auto kfstorysetting = KFStoryConfig::Instance()->FindSetting( mainstory );
            if ( kfstorysetting == nullptr || kfstorysetting->_trigger_type != 0u )
            {
                // 如果主剧情不存在或主剧情不为被动触发，则清空
                player->UpdateData( __STRING__( mainstory ), KFEnum::Set, 0u );

                CheckMainStory( player );
            }

        }
        else
        {
            CheckMainStory( player );
        }
    }

    void KFStoryModule::CheckMainStory( KFEntity* player )
    {
        auto kfstoryrecord = player->Find( __STRING__( story ) );
        for ( auto kfstory = kfstoryrecord->First(); kfstory != nullptr; kfstory = kfstoryrecord->Next() )
        {
            auto storyid = kfstory->Get<uint32>( __STRING__( id ) );
            auto kfstorysetting = KFStoryConfig::Instance()->FindSetting( storyid );
            if ( kfstorysetting == nullptr )
            {
                continue;
            }

            if ( kfstorysetting->_trigger_type != 0u )
            {
                continue;
            }

            player->UpdateData( __STRING__( mainstory ), KFEnum::Set, storyid );

            auto sequence = kfstory->Get<uint32>( __STRING__( sequence ) );
            OnExecuteSequence( player, storyid, sequence );

            break;
        }
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFStoryModule::AddStoryElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", kfelement->_data_name );
            return false;
        }

        AddStory( player, kfelementobject->_config_id );
        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFStoryModule::OnExecuteStory )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            return false;
        }

        auto storyid = executedata->_param_list._params[ 0 ]->_int_value;
        AddStory( player, storyid );

        return true;
    }

    __KF_DROP_LOGIC_FUNCTION__( KFStoryModule::OnDropStory )
    {
        AddStory( player, dropdata->_data_key );
    }

    __KF_ADD_DATA_FUNCTION__( KFStoryModule::OnAddStoryCallBack )
    {
        auto mainstory = player->Get<uint32>( __STRING__( mainstory ) );
        auto storyid = kfdata->Get<uint32>( __STRING__( id ) );
        if ( mainstory == 0u )
        {
            player->UpdateData( __STRING__( mainstory ), KFEnum::Set, storyid );
        }

        auto sequence = kfdata->Get<uint32>( __STRING__( sequence ) );
        OnExecuteSequence( player, storyid, sequence );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFStoryModule::OnRemoveStoryCallBack )
    {
        auto storyid = kfdata->Get<uint32>( __STRING__( id ) );
        auto mainstory = player->Get( __STRING__( mainstory ) );
        if ( mainstory == storyid )
        {
            // 当主剧情结束后，自动跳转到下一个自动执行的剧情id
            player->UpdateData( __STRING__( mainstory ), KFEnum::Set, 0u );

            CheckMainStory( player );
        }
    }

    __KF_REMOVE_DATA_FUNCTION__( KFStoryModule::OnRemoveDialogueCallBack )
    {
        auto mainstory = player->Get( __STRING__( mainstory ) );
        if ( mainstory == 0u )
        {
            return;
        }

        auto kfstory = player->Find( __STRING__( story ), mainstory );
        if ( kfstory == nullptr )
        {
            return;
        }

        auto sequence = kfstory->Get<uint32>( __STRING__( sequence ) );
        auto kfsetting = KFStoryConfig::Instance()->FindStorySequence( mainstory, sequence );
        if ( kfsetting == nullptr )
        {
            return;
        }

        if ( kfsetting->_type != KFMsg::UIDialogue && kfsetting->_type != KFMsg::BubbleDialogue )
        {
            return;
        }

        auto dialogueid = kfdata->Get<uint32>( __STRING__( id ) );
        if ( kfsetting->_parameter1 == dialogueid )
        {
            player->UpdateData( kfstory, __STRING__( sequence ), KFEnum::Add, 1u );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFStoryModule::OnUpdateSequenceCallBack )
    {
        if ( newvalue == 0u )
        {
            return;
        }

        auto storyid = kfdata->GetParent()->Get<uint32>( __STRING__( id ) );

        auto kfsetting = KFStoryConfig::Instance()->FindSetting( storyid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 剧情序列数
        auto count = kfsetting->_sequences.size();
        if ( newvalue > count )
        {
            player->RemoveData( __STRING__( story ), storyid );
            return;
        }

        OnExecuteSequence( player, storyid, newvalue );
    }

    __KF_MESSAGE_FUNCTION__( KFStoryModule::HandleStartStoryReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgStartStoryReq );

        auto kfstorysetting = KFStoryConfig::Instance()->FindSetting( kfmsg.storyid() );
        if ( kfstorysetting == nullptr )
        {
            // 剧情配置不存在
            return _kf_display->SendToClient( player, KFMsg::StorySettingNotExist );
        }

        if ( kfstorysetting->_trigger_type == 0u )
        {
            // 触发类型限制
            return _kf_display->SendToClient( player, KFMsg::StoryTriggerTypeLimit );
        }

        // 更换同步顺序, 先删除条件，再添加条件，再更新
        player->SyncDataSequence( KFEnum::Dec, KFEnum::Add, KFEnum::Set );

        auto kfstoryrecord = player->Find( __STRING__( story ) );
        auto kfstory = kfstoryrecord->Find( kfmsg.storyid() );
        if ( kfstory == nullptr )
        {
            kfstory = player->CreateData( kfstoryrecord );
            kfstory->Set( __STRING__( sequence ), 1u );		// 默认序列1开始

            player->AddData( kfstoryrecord, kfmsg.storyid(), kfstory );
        }

        player->UpdateData( __STRING__( mainstory ), KFEnum::Set, kfmsg.storyid() );

        auto sequence = kfstory->Get<uint32>( __STRING__( sequence ) );
        OnExecuteSequence( player, kfmsg.storyid(), sequence );
    }

    __KF_MESSAGE_FUNCTION__( KFStoryModule::HandleUpdateStoryReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateStoryReq );

        auto kfstorysetting = KFStoryConfig::Instance()->FindSetting( kfmsg.storyid() );
        if ( kfstorysetting == nullptr )
        {
            // 剧情配置不存在
            return _kf_display->SendToClient( player, KFMsg::StorySettingNotExist );
        }

        auto kfstoryrecord = player->Find( __STRING__( story ) );
        auto kfstory = kfstoryrecord->Find( kfmsg.storyid() );
        if ( kfstory == nullptr )
        {
            // 剧情不存在
            return _kf_display->SendToClient( player, KFMsg::StoryNotExist );
        }

        auto sequenceid = kfstory->Get<uint32>( __STRING__( sequence ) );

        // 剧情序列对话有分支，不能跳过
        auto storysequence = kfstorysetting->FindStorySequence( sequenceid );
        if ( storysequence != nullptr &&
                ( storysequence->_type == KFMsg::UIDialogue || storysequence->_type == KFMsg::BubbleDialogue ) )
        {
            auto dialogueid = storysequence->_parameter1;
            auto kfdialoguesetting = KFDialogueConfig::Instance()->FindSetting( dialogueid );
            if ( kfdialoguesetting != nullptr && kfdialoguesetting->_sequence > 0u )
            {
                return _kf_display->SendToClient( player, KFMsg::StoryHaveDialogueBranch );
            }
        }

        // 剧情序列更新
        player->UpdateData( kfstory, __STRING__( sequence ), KFEnum::Add, 1u );
    }

    void KFStoryModule::AddStory( KFEntity* player, uint32 storyid )
    {
        auto kfstorysetting = KFStoryConfig::Instance()->FindSetting( storyid );
        if ( kfstorysetting == nullptr )
        {
            return;
        }

        auto kfstoryrecord = player->Find( __STRING__( story ) );
        auto kfstory = kfstoryrecord->Find( storyid );
        if ( kfstory == nullptr )
        {
            kfstory = player->CreateData( kfstoryrecord );
            kfstory->Set( __STRING__( sequence ), 1u );		// 默认序列1开始

            player->AddData( kfstoryrecord, storyid, kfstory );
        }
    }

    void KFStoryModule::OnExecuteSequence( KFEntity* player, uint32 storyid, uint32 sequence )
    {
        auto kfsetting = KFStoryConfig::Instance()->FindSetting( storyid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        auto storysequence = kfsetting->FindStorySequence( sequence );
        if ( storysequence == nullptr )
        {
            return;
        }

        auto kfstoryrecord = player->Find( __STRING__( story ) );
        auto kfstory = kfstoryrecord->Find( storyid );

        if ( storysequence->_type == KFMsg::ProcessTask )
        {
            // 如果当前序列为任务，接取任务
            _kf_task->OpenTask( player, storysequence->_parameter1, KFMsg::ExecuteStatus, 0u );

            player->UpdateData( kfstory, __STRING__( sequence ), KFEnum::Add, 1u );
        }
        else if ( storysequence->_type == KFMsg::UIDialogue || storysequence->_type == KFMsg::BubbleDialogue )
        {
            // 当前序列为对话，添加对话
            _kf_dialogue->AddDialogue( player, storysequence->_parameter1, storysequence->_type );
        }
    }
}