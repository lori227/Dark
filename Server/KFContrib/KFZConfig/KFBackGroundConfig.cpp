#include "KFBackGroundConfig.hpp"

namespace KFrame
{
    void KFBackGroundConfig::ReadSetting( KFNode& xmlnode, KFBackGroundSetting* kfsetting )
    {
        auto strracelimit = xmlnode.GetString( "RaceLimit", true );
        if ( !strracelimit.empty() )
        {
            kfsetting->_race_list = KFUtility::SplitSet< std::set<uint32> >( strracelimit, DEFAULT_SPLIT_STRING );
        }

        auto strsexlimit = xmlnode.GetString( "SexLimit", true );
        if ( !strsexlimit.empty() )
        {
            kfsetting->_sex_list = KFUtility::SplitSet< std::set<uint32> >( strsexlimit, DEFAULT_SPLIT_STRING );
        }

    }
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    bool KFBackGroundSetting::IsValid( uint32 race, uint32 sex ) const
    {
        if ( !_race_list.empty() )
        {
            if ( _race_list.find( race ) == _race_list.end() )
            {
                return false;
            }
        }

        if ( !_sex_list.empty() )
        {
            if ( _sex_list.find( sex ) == _sex_list.end() )
            {
                return false;
            }
        }

        return true;
    }
}