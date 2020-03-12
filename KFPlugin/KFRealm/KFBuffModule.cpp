#include "KFBuffModule.hpp"

namespace KFrame
{
    void KFBuffModule::BeforeRun()
    {
        __REGISTER_PVE_START__( &KFBuffModule::OnPVEStartBuffModule );
        __REGISTER_DROP_LOGIC__( __STRING__( addbuff ), &KFBuffModule::OnDropHeroAddBuff );
        __REGISTER_DROP_LOGIC__( __STRING__( decbuff ), &KFBuffModule::OnDropHeroDecBuff );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_ADD_TEAM_BUFF_REQ, &KFBuffModule::HandleAddTeamBuffReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_TEAM_BUFF_REQ, &KFBuffModule::HandleRemoveTeamBuffReq );
    }

    void KFBuffModule::BeforeShut()
    {
        __UN_PVE_START__();
        __UN_DROP_LOGIC__( __STRING__( addbuff ) );
        __UN_DROP_LOGIC__( __STRING__( decbuff ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_ADD_TEAM_BUFF_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REMOVE_TEAM_BUFF_REQ );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
#define __MAX_BUFF_COUNT__ 100000

    __KF_PVE_START_FUNCTION__( KFBuffModule::OnPVEStartBuffModule )
    {
        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return;
        }

        // 所有buff数量-1
        UInt32Set bufflist;
        auto pbbuffdata = kfrealmdata->_data.mutable_buffdata();
        auto pbteambuff = &pbbuffdata->teambuff();
        for ( auto iter = pbteambuff->begin(); iter != pbteambuff->end(); ++iter )
        {
            bufflist.insert( iter->first );
        }

        for ( auto buffid : bufflist )
        {
            RemoveTeamBuff( kfrealmdata, buffid, 1u );
        }
        SendBuffMessage( player, kfrealmdata );
    }

    __KF_DROP_LOGIC_FUNCTION__( KFBuffModule::OnDropHeroAddBuff )
    {
        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return;
        }

        auto buffid = dropdata->_data_key;
        auto count = dropdata->GetValue();

        AddTeamBuff( kfrealmdata, buffid, count );
        SendBuffMessage( player, kfrealmdata );
        AddBuffToShow( player, dropdata->_logic_name, buffid, count );
    }

    __KF_DROP_LOGIC_FUNCTION__( KFBuffModule::OnDropHeroDecBuff )
    {
        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return;
        }

        auto buffid = dropdata->_data_key;
        auto count = dropdata->GetValue();

        RemoveTeamBuff( kfrealmdata, buffid, count );
        SendBuffMessage( player, kfrealmdata );
        AddBuffToShow( player, dropdata->_logic_name, buffid, count );
    }

    __KF_MESSAGE_FUNCTION__( KFBuffModule::HandleAddTeamBuffReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgAddTeamBuffReq );

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return;
        }

        auto pbbufflist = &kfmsg.bufflist();
        for ( auto iter = pbbufflist->begin(); iter != pbbufflist->end(); ++iter )
        {
            AddTeamBuff( kfrealmdata, iter->first, iter->second );
            AddBuffToShow( player, __STRING__( addbuff ), iter->first, iter->second );
        }
        SendBuffMessage( player, kfrealmdata );
    }

    __KF_MESSAGE_FUNCTION__( KFBuffModule::HandleRemoveTeamBuffReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveTeamBuffReq );

        auto kfrealmdata = _kf_realm->GetRealmData( player );
        if ( kfrealmdata == nullptr )
        {
            return;
        }

        auto pbbufflist = &kfmsg.bufflist();
        for ( auto iter = pbbufflist->begin(); iter != pbbufflist->end(); ++iter )
        {
            RemoveTeamBuff( kfrealmdata, iter->first, iter->second );
            AddBuffToShow( player, __STRING__( decbuff ), iter->first, iter->second );
        }
        SendBuffMessage( player, kfrealmdata );
    }

    void KFBuffModule::AddTeamBuff( KFRealmData* kfrealmdata, uint32 buffid, uint32 count )
    {
        count = ( count == 0u ? __MAX_BUFF_COUNT__ : count );

        auto pbbuffdata = kfrealmdata->_data.mutable_buffdata();
        auto& pbbuff = ( *pbbuffdata->mutable_teambuff() )[ buffid ];
        pbbuff.set_id( buffid );
        pbbuff.set_count( pbbuff.count() + count );
    }

    void KFBuffModule::RemoveTeamBuff( KFRealmData* kfrealmdata, uint32 buffid, uint32 count )
    {
        count = ( count == 0u ? __MAX_BUFF_COUNT__ : count );

        auto pbbuffdata = kfrealmdata->_data.mutable_buffdata();
        auto pbteambuff = pbbuffdata->mutable_teambuff();
        auto iter = pbteambuff->find( buffid );
        if ( iter == pbteambuff->end() )
        {
            return;
        }

        auto oldcount = iter->second.count();
        iter->second.set_count( oldcount - __MIN__( oldcount, count ) );
        if ( iter->second.count() == 0u )
        {
            pbteambuff->erase( iter );
        }
    }

    void KFBuffModule::SendBuffMessage( KFEntity* player, KFRealmData* kfrealmdata )
    {
        KFMsg::MsgUpdateExploreBuffAck ack;
        ack.mutable_buffdata()->CopyFrom( kfrealmdata->_data.buffdata() );
        _kf_player->SendToClient( player, KFMsg::MSG_UPDATE_EXPLORE_BUFF_ACK, &ack );
    }

    void KFBuffModule::AddBuffToShow( KFEntity* player, const std::string& logicname, uint32 buffid, uint32 count )
    {
        KeyValue values;
        values[ __STRING__( id ) ] = buffid;
        values[ __STRING__( count ) ] = count;
        player->AddDataToShow( logicname, buffid, values, false );
    }

    void KFBuffModule::AddBuffData( KFRealmData* kfrealmdata, uint64 uuid, uint32 value )
    {
        auto buff = kfrealmdata->_data.mutable_buffdata()->mutable_herobuff();

        KFMsg::PBBuffListData* bufflistdata = nullptr;
        auto iter = buff->find( uuid );
        if ( iter != buff->end() )
        {
            bufflistdata = &iter->second;
        }
        else
        {
            bufflistdata = &( ( *buff )[ uuid ] );
        }

        auto buffdata = bufflistdata->add_bufflist();
        buffdata->set_id( value );
    }

    void KFBuffModule::RemoveBuffData( KFRealmData* kfrealmdata, uint64 uuid, uint32 value )
    {
        auto buff = kfrealmdata->_data.mutable_buffdata()->mutable_herobuff();
        auto iter = buff->find( uuid );
        if ( iter == buff->end() )
        {
            return;
        }

        // 删除该buff
        auto bufflistdata = &iter->second;
        auto buffsize = bufflistdata->bufflist_size();
        for ( auto index = 0; index < buffsize; ++index )
        {
            auto buff = &bufflistdata->bufflist( index );
            if ( buff->id() == value )
            {
                bufflistdata->mutable_bufflist()->DeleteSubrange( index, 1 );
                break;
            }
        }
    }
}