﻿#include "KFChapterModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFChapterModule::BeforeRun()
    {
        __REGISTER_FINISH_TASK_CHAIN__( __STRING__( chapter ), &KFChapterModule::OnFinishTaskChainChapterLogic );
        /////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_CHAPTER_EXECUTE_STATUS_REQ, &KFChapterModule::HandleChapterExecuteStatusReq );
    }

    void KFChapterModule::BeforeShut()
    {
        __UN_FINISH_TASK_CHAIN__( __STRING__( chapter ) );
        /////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_CHAPTER_EXECUTE_STATUS_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFChapterModule::HandleChapterExecuteStatusReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChapterExecuteStatusReq );

        auto kfsetting = KFChapterConfig::Instance()->FindSetting( kfmsg.chapterid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ChapterIdError );
        }

        auto statusid = 0u;
        auto kfchapter = player->Find( __STRING__( chapter ), kfmsg.chapterid() );
        if ( kfchapter == nullptr )
        {
            if ( kfsetting->_init_status_id == 0u )
            {
                return _kf_display->SendToClient( player, KFMsg::ChapterNotOpen );
            }

            statusid = kfsetting->_init_status_id;
        }
        else
        {
            statusid = kfchapter->Get<uint32>( __STRING__( status ) );
            if ( statusid == 0u )
            {
                return _kf_display->SendToClient( player, KFMsg::ChapterEventError );
            }
        }

        auto kfstatussetting = KFStatusConfig::Instance()->FindSetting( statusid );
        if ( kfstatussetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ChapterStatusIdError );
        }

        auto ok = _kf_execute->Execute( player, &kfstatussetting->_execute_data, __STRING__( chapter ), kfmsg.chapterid(), __FUNC_LINE__ );
        if ( !ok )
        {
            return _kf_display->SendToClient( player, KFMsg::ChapterExecuteFailed );
        }
    }

    __KF_FINISH_TASK_CHAIN_FUNCTION__( KFChapterModule::OnFinishTaskChainChapterLogic )
    {
        auto kfrecord = player->Find( __STRING__( chapter ) );
        if ( kfrecord == nullptr )
        {
            return;
        }

        if ( statusid == 0u )
        {
            for ( auto logicid : logicids )
            {
                player->RemoveData( kfrecord, logicid, false );
            }
        }
        else
        {
            for ( auto logicid : logicids )
            {
                player->UpdateData( kfrecord, logicid, kfrecord->_data_setting->_value_key_name, KFEnum::Set, statusid );
            }
        }
    }
}