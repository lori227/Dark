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

        __REGISTER_REMOVE_DATA_1__( __STRING__( dialogue ), &KFStoryModule::OnRemoveDialogueCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( story ), __STRING__( sequence ), &KFStoryModule::OnUpdateSequenceCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( balance ), __STRING__( pveresult ), &KFStoryModule::OnUpdatePVECallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( balance ), __STRING__( realmresult ), &KFStoryModule::OnUpdateRealmCallBack );
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

        __UN_REMOVE_DATA_1__( __STRING__( dialogue ) );
        __UN_UPDATE_DATA_2__( __STRING__( story ), __STRING__( sequence ) );
        __UN_UPDATE_DATA_2__( __STRING__( balance ), __STRING__( pveresult ) );
        __UN_UPDATE_DATA_2__( __STRING__( balance ), __STRING__( realmresult ) );
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

                CheckStory( player );
            }
            else
            {
                ExecuteStory( player, mainstory );
            }
        }
        else
        {
            CheckStory( player );
        }
    }

    void KFStoryModule::CheckStory( KFEntity* player )
    {
        auto kfstoryrecord = player->Find( __STRING__( story ) );
        for ( auto kfstory = kfstoryrecord->First(); kfstory != nullptr; kfstory = kfstoryrecord->Next() )
        {
            auto sequence = kfstory->Get<uint32>( __STRING__( sequence ) );
            if ( sequence == 0u )
            {
                // 该剧情已结束
                continue;
            }

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
            ExecuteStory( player, storyid );

            break;
        }
    }

    void KFStoryModule::RemoveStory( KFEntity* player, uint32 storyid )
    {
        auto kfstoryrecord = player->Find( __STRING__( story ) );
        auto kfstory = kfstoryrecord->Find( storyid );
        if ( kfstory == nullptr )
        {
            return;
        }

        // 删除剧情列表
        UInt32Set removeset;
        removeset.insert( storyid );

        auto parentid = kfstory->Get<uint32>( __STRING__( parentid ) );
        while ( parentid != 0u )
        {
            auto kfparentstory = kfstoryrecord->Find( parentid );
            if ( kfparentstory == nullptr )
            {
                break;
            }

            if ( removeset.find( parentid ) != removeset.end() )
            {
                // 防止剧情链循环
                break;
            }

            removeset.insert( parentid );;

            parentid = kfparentstory->Get<uint32>( __STRING__( parentid ) );
        }

        for ( auto iter : removeset )
        {
            player->RemoveData( kfstoryrecord, iter );
        }

        auto mainstory = player->Get<uint32>( __STRING__( mainstory ) );
        if ( removeset.find( mainstory ) != removeset.end() )
        {
            // 当主剧情结束后，自动跳转到下一个剧情id
            player->UpdateData( __STRING__( mainstory ), KFEnum::Set, 0u );

            CheckStory( player );
        }
    }

    void KFStoryModule::AddSequence( KFEntity* player, KFData* kfdata, uint32 type )
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

        if ( kfsetting->_type != type )
        {
            return;
        }

        auto id = kfdata->Get<uint32>( __STRING__( id ) );
        if ( kfsetting->_parameter1 == id )
        {
            player->UpdateData( kfstory, __STRING__( sequence ), KFEnum::Add, 1u );
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

    __KF_REMOVE_DATA_FUNCTION__( KFStoryModule::OnRemoveDialogueCallBack )
    {
        AddSequence( player, kfdata, KFMsg::BubbleDialogue );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFStoryModule::OnUpdateSequenceCallBack )
    {
        if ( newvalue == 0u )
        {
            return;
        }

        auto kfstory = kfdata->GetParent();
        auto storyid = kfstory->Get<uint32>( __STRING__( id ) );

        auto kfsetting = KFStoryConfig::Instance()->FindSetting( storyid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        auto count = kfsetting->_sequences.size();
        if ( newvalue > count )
        {
            auto childid = kfstory->Get<uint32>( __STRING__( childid ) );
            if ( childid == 0u )
            {
                // 没有子剧情，则删除剧情
                RemoveStory( player, storyid );
            }
            else
            {
                // 有子剧情，则继续执行子剧情
                player->UpdateData( kfstory, __STRING__( sequence ), KFEnum::Set, 0u );

                player->UpdateData( __STRING__( mainstory ), KFEnum::Set, childid );
                ExecuteStory( player, childid );
            }

            return;
        }

        ExecuteStory( player, storyid );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFStoryModule::OnUpdatePVECallBack )
    {
        if ( newvalue != KFMsg::Victory )
        {
            return;
        }

        AddSequence( player, kfdata->GetParent(), KFMsg::ProcessPVE );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFStoryModule::OnUpdateRealmCallBack )
    {
        if ( newvalue != KFMsg::Victory )
        {
            return;
        }

        AddSequence( player, kfdata->GetParent(), KFMsg::ProcessExplore );
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

        AddStory( player, kfmsg.storyid(), kfmsg.childid() );
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
        auto storysequence = kfstorysetting->FindStorySequence( sequenceid );
        if ( storysequence == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StorySequenceIsError );
        }

        if ( storysequence->_type == KFMsg::ProcessPVE ||
                storysequence->_type == KFMsg::ProcessExplore ||
                storysequence->_type == KFMsg::ProcessTask )
        {
            return _kf_display->SendToClient( player, KFMsg::StorySequenceNotFinish );
        }

        if ( storysequence->_type == KFMsg::BubbleDialogue )
        {
            // 剧情序列对话有分支，不能跳过
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

    void KFStoryModule::AddStory( KFEntity* player, uint32 storyid, uint32 childid )
    {
        auto kfstorysetting = KFStoryConfig::Instance()->FindSetting( storyid );
        if ( kfstorysetting == nullptr )
        {
            __LOG_ERROR__( "storyid [{}] is not exist", storyid );
            return;
        }

        auto kfstoryrecord = player->Find( __STRING__( story ) );
        auto kfstory = kfstoryrecord->Find( storyid );

        if ( kfstory == nullptr )
        {
            kfstory = player->CreateData( kfstoryrecord );
            kfstory->Set( __STRING__( sequence ), 1u );

            if ( childid != 0u )
            {
                auto kfchildstory = kfstoryrecord->Find( childid );
                if ( kfchildstory == nullptr )
                {
                    childid = 0u;
                }
                else
                {
                    player->UpdateData( kfchildstory, __STRING__( parentid ), KFEnum::Set, storyid );
                }
            }
            kfstory->Set( __STRING__( childid ), childid );

            player->AddData( kfstoryrecord, storyid, kfstory );
        }

        UInt32Set storyset;
        storyset.insert( storyid );

        auto sequence = kfstory->Get<uint32>( __STRING__( sequence ) );
        while ( sequence == 0u )
        {
            storyid = kfstory->Get<uint32>( __STRING__( childid ) );
            auto kfchildstory = kfstoryrecord->Find( storyid );
            if ( kfchildstory == nullptr )
            {
                return;
            }

            if ( storyset.find( storyid ) != storyset.end() )
            {
                // 防止剧情链循环
                RemoveStory( player, storyid );
                return;
            }
            storyset.insert( storyid );

            sequence = kfchildstory->Get<uint32>( __STRING__( sequence ) );
        }

        player->UpdateData( __STRING__( mainstory ), KFEnum::Set, storyid );
        ExecuteStory( player, storyid );
    }

    void KFStoryModule::ExecuteStory( KFEntity* player, uint32 storyid )
    {
        auto kfstory = player->Find( __STRING__( story ), storyid );
        if ( kfstory == nullptr )
        {
            return;
        }
        auto sequence = kfstory->Get<uint32>( __STRING__( sequence ) );

        auto storysequence = KFStoryConfig::Instance()->FindStorySequence( storyid, sequence );
        if ( storysequence == nullptr )
        {
            return;
        }

        if ( storysequence->_type == KFMsg::ProcessTask )
        {
            // 如果当前序列为任务，接取任务
            _kf_task->OpenTask( player, storysequence->_parameter1, KFMsg::ExecuteStatus, 0u );

            player->UpdateData( kfstory, __STRING__( sequence ), KFEnum::Add, 1u );
        }
        else if ( storysequence->_type == KFMsg::UIDialogue || storysequence->_type == KFMsg::BubbleDialogue )
        {
            // 当前序列为对话，添加对话
            _kf_dialogue->AddDialogue( player, storysequence->_parameter1, storysequence->_type, storyid );
        }
    }
}