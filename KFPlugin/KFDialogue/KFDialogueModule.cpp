#include "KFDialogueModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFDialogueModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );

        __REGISTER_EXECUTE__( __STRING__( dialogue ), &KFDialogueModule::OnExecuteDialogue );
        __REGISTER_DROP_LOGIC__( __STRING__( dialogue ), &KFDialogueModule::OnDropDialogue );
        __REGISTER_ADD_ELEMENT__( __STRING__( dialogue ), &KFDialogueModule::AddDialogueElement );
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

        player->UpdateData( __STRING__( dialogue ), __STRING__( id ), KFEnum::Set, kfelementobject->_config_id );
        return true;
    }

    bool KFDialogueModule::SendToClientDialogueStart( KFEntity* player, uint32 dialogid, uint32 delaytime, const char* function, uint32 line )
    {
        player->Set( __STRING__( dialogue ), __STRING__( id ), dialogid );

        KFMsg::MsgDialogueStartAck ack;
        ack.set_dialogid( dialogid );
        return _kf_player->SendToClient( player, KFMsg::MSG_DIALOGUE_START_ACK, &ack, __MAX__( delaytime, 10u ) );
    }

    __KF_EXECUTE_FUNCTION__( KFDialogueModule::OnExecuteDialogue )
    {
        if ( executedata->_param_list._params.size() < 2u )
        {
            __LOG_ERROR_FUNCTION__( function, line, "dialogue execute param size<2" );
            return false;
        }

        auto dialogid = executedata->_param_list._params[ 0 ]->_int_value;
        auto delaytime = executedata->_param_list._params[ 1 ]->_int_value;
        return SendToClientDialogueStart( player, dialogid, delaytime, function, line );
    }

    __KF_DROP_LOGIC_FUNCTION__( KFDialogueModule::OnDropDialogue )
    {
        SendToClientDialogueStart( player, dropdata->_data_key, dropdata->_min_value, function, line );
    }

    __KF_MESSAGE_FUNCTION__( KFDialogueModule::HandleReceiveDialogueFinishReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDialogueFinishReq );

        auto kfdialogue = player->Find( __STRING__( dialogue ) );
        auto dialogueid = kfdialogue->Get<uint32>( __STRING__( id ) );
        if ( dialogueid == 0u || kfmsg.dialogueid() != dialogueid )
        {
            return;
        }

        player->UpdateData( kfdialogue, __STRING__( id ), KFEnum::Set, kfmsg.dialogueid() );
    }

}