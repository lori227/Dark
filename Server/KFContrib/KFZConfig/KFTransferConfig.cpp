#include "KFTransferConfig.hpp"

namespace KFrame
{
    void KFTransferConfig::ReadSetting( KFNode& xmlnode, KFTransferSetting* kfsetting )
    {
        kfsetting->_level = xmlnode.GetUInt32( "Level", true );

        auto strtransfer = xmlnode.GetString( "TransferId" );
        kfsetting->_transfer_list = KFUtility::SplitSet< std::set< uint32 > >( strtransfer, DEFAULT_SPLIT_STRING );

        auto stractive = xmlnode.GetString( "ActivePool" );
        kfsetting->_active_pool_list = KFUtility::SplitList< std::list< uint32 > >( stractive, DEFAULT_SPLIT_STRING );

        auto strcharacter = xmlnode.GetString( "CharacterPool" );
        kfsetting->_character_pool_list = KFUtility::SplitList< std::list< uint32 > >( strcharacter, DEFAULT_SPLIT_STRING );

        auto strinnate = xmlnode.GetString( "InnatePool" );
        kfsetting->_innate_pool_list = KFUtility::SplitList< std::list< uint32 > >( strinnate, DEFAULT_SPLIT_STRING );

        std::string strelement = xmlnode.GetString( "Cost", true );
        kfsetting->_cost.Parse( strelement, __FUNC_LINE__ );
    }
}