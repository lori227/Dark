#include "KFDialogueModule.hpp"

namespace KFrame
{
    void KFDialogueModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_EXECUTE__( __STRING__( dialogue ), &KFDialogueModule::OnExecuteDialogue );
        __REGISTER_DROP_LOGIC__( __STRING__( dialogue ), &KFDialogueModule::OnDropDialogue );
        __REGISTER_ADD_ELEMENT__( __STRING__( dialogue ), &KFDialogueModule::AddDialogueElement );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_DIALOGUE_REQ, &KFDialogueModule::HandleUpdateDialogueReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SELECT_DIALOGUE_BRANCH_REQ, &KFDialogueModule::HandleSelectDialogueBranchReq );
    }

    void KFDialogueModule::BeforeShut()
    {
        __UN_EXECUTE__( __STRING__( dialogue ) );
        __UN_DROP_LOGIC__( __STRING__( dialogue ) );
        __UN_ADD_ELEMENT__( __STRING__( dialogue ) );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_DIALOGUE_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SELECT_DIALOGUE_BRANCH_REQ );
    }

    void KFDialogueModule::AddDialogue( KFEntity* player, uint32 dialogid, uint32 type )
    {
        auto kfdialoguesetting = KFDialogueConfig::Instance()->FindSetting( dialogid );
        if ( kfdialoguesetting == nullptr )
        {
            return;
        }

        auto kfdialoguerecord = player->Find( __STRING__( dialogue ) );
        auto kfdialogue = kfdialoguerecord->Find( dialogid );
        auto sequence = 1u;		// 序列默认从1开始

        if ( kfdialogue == nullptr )
        {
            // 对话有分支，初始化条件，保存对话
            if ( kfdialoguesetting->_sequence > 0u )
            {
                kfdialogue = player->CreateData( kfdialoguerecord );
                kfdialogue->Set( __STRING__( sequence ), sequence );

                player->AddData( kfdialoguerecord, dialogid, kfdialogue );
            }
        }
        else
        {
            // 对话已存在，获取对话序列
            sequence = kfdialogue->Get<uint32>( __STRING__( sequence ) );
        }

        if ( kfdialogue != nullptr )
        {
            // 更换同步顺序, 先删除条件，再添加条件，再更新
            player->SyncDataSequence( KFEnum::Dec, KFEnum::Add, KFEnum::Set );

            // 清空当前的条件
            auto kfbranchrecord = kfdialogue->Find( __STRING__( dialoguebranch ) );
            player->CleanData( kfbranchrecord, false );

            for ( auto iter : kfdialoguesetting->_branch )
            {
                auto kfbranchsetting = KFDialogueBranchConfig::Instance()->FindSetting( iter );
                if ( kfbranchsetting == nullptr )
                {
                    continue;
                }

                auto kfbranch = kfbranchrecord->Find( iter );
                if ( kfbranch != nullptr )
                {
                    continue;
                }

                kfbranch = player->CreateData( kfbranchrecord );
                kfbranch->Set( __STRING__( id ), iter );
                kfbranchrecord->Add( iter, kfbranch );

                kfbranch = kfbranchrecord->Find( iter );
                auto kfconditionobject = kfbranch->Find( __STRING__( conditions ) );

                // 添加下一级的升级条件
                _kf_condition->AddCondition( player, kfconditionobject, kfbranchsetting->_condition, kfbranchsetting->_condition_type );

                // 初始化条件
                _kf_condition->InitCondition( player, kfconditionobject, KFConditionEnum::LimitNull, false );
            }

            // 数据同步到客户端
            player->SynAddRecordData( kfbranchrecord );
        }

        SendToClientDialogueStart( player, dialogid, sequence, type );
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFDialogueModule::AddDialogueElement )
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

        AddDialogue( player, kfelementobject->_config_id );
        return true;
    }

    bool KFDialogueModule::SendToClientDialogueStart( KFEntity* player, uint32 dialogid, uint32 sequence, uint32 type )
    {
        KFMsg::MsgStartDialogueAck ack;
        ack.set_dialogid( dialogid );
        ack.set_sequenceid( sequence );
        ack.set_type( type );
        return _kf_player->SendToClient( player, KFMsg::MSG_START_DIALOGUE_ACK, &ack, 10u );
    }

    __KF_EXECUTE_FUNCTION__( KFDialogueModule::OnExecuteDialogue )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "dialogue execute param size<1" );
            return false;
        }

        auto dialogid = executedata->_param_list._params[ 0 ]->_int_value;
        AddDialogue( player, dialogid );

        return true;
    }

    __KF_DROP_LOGIC_FUNCTION__( KFDialogueModule::OnDropDialogue )
    {
        AddDialogue( player, dropdata->_data_key );
    }

    __KF_MESSAGE_FUNCTION__( KFDialogueModule::HandleUpdateDialogueReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateDialogueReq );

        auto kfdialoguesetting = KFDialogueConfig::Instance()->FindSetting( kfmsg.dialogueid() );
        if ( kfdialoguesetting == nullptr )
        {
            // 对话配置不存在
            return _kf_display->SendToClient( player, KFMsg::DialogueSettingNotExist );
        }

        if ( kfdialoguesetting->_sequence == 0u )
        {
            // 对话没有分支
            return _kf_display->SendToClient( player, KFMsg::DialogueHaveNotBranch );
        }

        auto kfdialogue = player->Find( __STRING__( dialogue ), kfmsg.dialogueid() );
        if ( kfdialogue == nullptr )
        {
            // 对话不存在
            return _kf_display->SendToClient( player, KFMsg::DialogueNotExist );
        }

        player->UpdateData( kfdialogue, __STRING__( sequence ), KFEnum::Set, kfdialoguesetting->_sequence );
    }

    __KF_MESSAGE_FUNCTION__( KFDialogueModule::HandleSelectDialogueBranchReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSelectDialogueBranchReq );

        auto kfdialoguesetting = KFDialogueConfig::Instance()->FindSetting( kfmsg.dialogueid() );
        if ( kfdialoguesetting == nullptr )
        {
            // 对话配置不存在
            return _kf_display->SendToClient( player, KFMsg::DialogueSettingNotExist );
        }

        if ( kfdialoguesetting->_sequence == 0u )
        {
            // 对话没有分支
            return _kf_display->SendToClient( player, KFMsg::DialogueHaveNotBranch );
        }

        auto kfdialogue = player->Find( __STRING__( dialogue ), kfmsg.dialogueid() );
        if ( kfdialogue == nullptr )
        {
            // 对话不存在
            return _kf_display->SendToClient( player, KFMsg::DialogueNotExist );
        }

        auto kfbranchsetting = KFDialogueBranchConfig::Instance()->FindSetting( kfmsg.branchid() );
        if ( kfbranchsetting == nullptr )
        {
            // 对话分支配置不存在
            return _kf_display->SendToClient( player, KFMsg::BranchSettingNotExist );
        }

        auto kfbranch = kfdialogue->Find( __STRING__( dialoguebranch ), kfmsg.branchid() );
        if ( kfbranch == nullptr )
        {
            // 分支不存在
            return _kf_display->SendToClient( player, KFMsg::DialogueBranchNotExist );
        }

        auto kfconditionobject = kfbranch->Find( __STRING__( conditions ) );
        if ( !_kf_condition->CheckCondition( player, kfconditionobject ) )
        {
            // 分支条件不满足
            return _kf_display->SendToClient( player, KFMsg::DialogueBranchLackCondition );
        }

        // 消耗是否足够
        auto& dataname = player->RemoveElement( &kfbranchsetting->_cost, _default_multiple, __STRING__( dialoguebranch ), kfbranchsetting->_id, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::DataNotEnough, dataname );
        }

        // 执行
        _kf_execute->Execute( player, &kfbranchsetting->_execute_data, __FUNC_LINE__ );

        // 通知选择分支成功
        _kf_display->SendToClient( player, KFMsg::DialogueBranchSelectSuc );

        // 如果对话是剧情，则剧情跳到下一序列，删除对话
        player->RemoveData( __STRING__( dialogue ), kfmsg.dialogueid() );
    }

}