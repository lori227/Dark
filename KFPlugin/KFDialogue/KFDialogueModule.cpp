#include "KFDialogueModule.hpp"

namespace KFrame
{
    void KFDialogueModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_ADD_ELEMENT__( __STRING__( dialogue ), &KFDialogueModule::AddDialogueElement );
        __REGISTER_DROP_LOGIC__( __STRING__( dialogue ), &KFDialogueModule::OnDropDialogue );
        __REGISTER_EXECUTE__( __STRING__( dialogue ), &KFDialogueModule::OnExecuteDialogue );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_DIALOGUE_FINISH_REQ, &KFDialogueModule::HandleReceiveDialogueFinishReq );
    }

    void KFDialogueModule::BeforeShut()
    {
        __UN_EXECUTE__( __STRING__( dialogue ) );
        __UN_DROP_LOGIC__( __STRING__( dialogue ) );
        __UN_ADD_ELEMENT__( __STRING__( dialogue ) );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_DIALOGUE_FINISH_REQ );
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFDialogueModule::AddDialogueElement )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_data_name );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        player->UpdateData( __STRING__( dialogue ), __STRING__( id ), KFEnum::Set, kfelementobject->_config_id );
        return std::make_tuple( KFDataDefine::Show_None, nullptr );
    }

    bool KFDialogueModule::SendToClientDialogueStart( KFEntity* player, uint32 dialogid, const char* function, uint32 line )
    {
        // 允许同时触发两个对话，服务端只需要承认最后一个

        KFMsg::MsgDialogueStartAck ack;
        ack.set_dialogid( dialogid );
        if ( !_kf_player->SendToClient( player, KFMsg::MSG_DIALOGUE_START_ACK, &ack ) )
        {
            __LOG_ERROR_FUNCTION__( function, line, "dialogue send to client failed." );
            return false;
        }

        // 设置当前对话id
        player->Set( __STRING__( dialogue ), __STRING__( id ), dialogid );
        return true;
    }

    __KF_EXECUTE_FUNCTION__( KFDialogueModule::OnExecuteDialogue )
    {
        if ( executedata->_param_list._params.size() < 1u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "dialogue execute param size<1" );
            return false;
        }

        auto dialogid = executedata->_param_list._params[0]->_int_value;
        return SendToClientDialogueStart( player, dialogid, function, line );
    }

    __KF_DROP_LOGIC_FUNCTION__( KFDialogueModule::OnDropDialogue )
    {
        SendToClientDialogueStart( player, dropdata->_data_key, function, line );
    }

    __KF_MESSAGE_FUNCTION__( KFDialogueModule::HandleReceiveDialogueFinishReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDialogueFinishReq );

        auto kfdialogue = player->Find( __STRING__( dialogue ) );
        auto dialogueid = kfdialogue->Get<uint32>( __STRING__( id ) );
        if ( dialogueid == 0u || kfmsg.dialogueid() != dialogueid )
        {
            // 非合法对话id
            return;
        }

        kfdialogue->Set( __STRING__( id ), 0u );
        player->UpdateData( kfdialogue, __STRING__( id ), KFEnum::Set, kfmsg.dialogueid() );
    }

}