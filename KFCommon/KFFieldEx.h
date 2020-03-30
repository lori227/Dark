﻿#ifndef __KF_FIELDEX_H__
#define __KF_FIELDEX_H__

#include "KFField.h"

namespace KFrame
{
    namespace KFField
    {
        __STRING_DEFINE__( rand );
        __STRING_DEFINE__( race );
        __STRING_DEFINE__( identity );
        __STRING_DEFINE__( profession );
        __STRING_DEFINE__( quality );
        __STRING_DEFINE__( weapon );
        __STRING_DEFINE__( age );
        __STRING_DEFINE__( active );
        __STRING_DEFINE__( passivity );
        __STRING_DEFINE__( innate );
        __STRING_DEFINE__( matchid );
        __STRING_DEFINE__( matchserverid );
        __STRING_DEFINE__( roomid );
        __STRING_DEFINE__( battle );
        __STRING_DEFINE__( battlelist );
        __STRING_DEFINE__( roomserverid );
        __STRING_DEFINE__( money );
        __STRING_DEFINE__( diamond );
        __STRING_DEFINE__( background );
        __STRING_DEFINE__( character );
        __STRING_DEFINE__( growth );
        __STRING_DEFINE__( fighter );
        __STRING_DEFINE__( attribute );
        __STRING_DEFINE__( divisor );
        __STRING_DEFINE__( usedivisor );
        __STRING_DEFINE__( recruitcount );
        __STRING_DEFINE__( recruittime );
        __STRING_DEFINE__( recruit );
        __STRING_DEFINE__( generateid );
        __STRING_DEFINE__( cost );
        __STRING_DEFINE__( usecost );
        __STRING_DEFINE__( head );
        __STRING_DEFINE__( weapontype );
        __STRING_DEFINE__( heroteam );
        __STRING_DEFINE__( heroteamcount );
        __STRING_DEFINE__( posflag )
        __STRING_DEFINE__( traincamplevel );
        __STRING_DEFINE__( train );
        __STRING_DEFINE__( endtime );
        __STRING_DEFINE__( calctime );
        __STRING_DEFINE__( exp );
        __STRING_DEFINE__( exprate );
        __STRING_DEFINE__( lock );
        __STRING_DEFINE__( clinic );
        __STRING_DEFINE__( daytime );
        __STRING_DEFINE__( dip );
        __STRING_DEFINE__( npc );
        __STRING_DEFINE__( explore );
        __STRING_DEFINE__( random );
        __STRING_DEFINE__( realm );
        __STRING_DEFINE__( realmid );
        __STRING_DEFINE__( realmtown );
        __STRING_DEFINE__( realmdata );
        __STRING_DEFINE__( activeindex );
        __STRING_DEFINE__( maincamp );
        __STRING_DEFINE__( itemid );
        __STRING_DEFINE__( itemnum );
        __STRING_DEFINE__( activetime );
        __STRING_DEFINE__( buildskinid );
        __STRING_DEFINE__( buildskin );
        __STRING_DEFINE__( info );
        __STRING_DEFINE__( hp );
        __STRING_DEFINE__( maxhp );
        __STRING_DEFINE__( ep );
        __STRING_DEFINE__( num );
        __STRING_DEFINE__( totalnum );
        __STRING_DEFINE__( herocount );
        __STRING_DEFINE__( maxherocount );
        __STRING_DEFINE__( chancel );
        __STRING_DEFINE__( transfer );
        __STRING_DEFINE__( other );
        __STRING_DEFINE__( movetype );
        __STRING_DEFINE__( maxdeadrecordid );
        __STRING_DEFINE__( deadrecordidcreater );
        __STRING_DEFINE__( deadrecord );
        __STRING_DEFINE__( deadrecordlist );
        __STRING_DEFINE__( lifeevent );
        __STRING_DEFINE__( playerdaily );
        __STRING_DEFINE__( clinicnum );
        __STRING_DEFINE__( record );
        __STRING_DEFINE__( fighterid );
        __STRING_DEFINE__( chapter );
        __STRING_DEFINE__( event );
        __STRING_DEFINE__( pveresult );
        __STRING_DEFINE__( realmresult );
        __STRING_DEFINE__( chapterid );
        __STRING_DEFINE__( pve );
        __STRING_DEFINE__( moduleid );
        __STRING_DEFINE__( modulename );
        __STRING_DEFINE__( group );
        __STRING_DEFINE__( eventgroup );
        __STRING_DEFINE__( durability );
        __STRING_DEFINE__( maxdurability );
        __STRING_DEFINE__( adddurability );
        __STRING_DEFINE__( affix );
        __STRING_DEFINE__( granary );
        __STRING_DEFINE__( granarynum );
        __STRING_DEFINE__( daynum );
        __STRING_DEFINE__( smithy );
        __STRING_DEFINE__( smithynum );
        __STRING_DEFINE__( fooditemid );
        __STRING_DEFINE__( levelversion );
        __STRING_DEFINE__( taskchain );
        __STRING_DEFINE__( chain );
        __STRING_DEFINE__( logicids );
        __STRING_DEFINE__( balance );
        __STRING_DEFINE__( addhp );
        __STRING_DEFINE__( dechp );
        __STRING_DEFINE__( addbuff );
        __STRING_DEFINE__( decbuff );
        __STRING_DEFINE__( totalsignin );
        __STRING_DEFINE__( signintime );
        __STRING_DEFINE__( continuoussignin );
        __STRING_DEFINE__( team );
        __STRING_DEFINE__( death );
        __STRING_DEFINE__( truns );
        __STRING_DEFINE__( refresh );
        __STRING_DEFINE__( refreshtaskchain );
        __STRING_DEFINE__( taskstatus );
        __STRING_DEFINE__( taskcondition );
        __STRING_DEFINE__( gift );
        __STRING_DEFINE__( useitem );
        __STRING_DEFINE__( smithymake );
        __STRING_DEFINE__( pveid );
        __STRING_DEFINE__( classlv );
        __STRING_DEFINE__( killrace );
        __STRING_DEFINE__( killprofession );
        __STRING_DEFINE__( killweapontype );
        __STRING_DEFINE__( killmovetype );
        __STRING_DEFINE__( interaction );
        __STRING_DEFINE__( field );
        __STRING_DEFINE__( pvevictory );
        __STRING_DEFINE__( consumemoney );
        __STRING_DEFINE__( preconditions );
        __STRING_DEFINE__( exploredrop );
        __STRING_DEFINE__( technology );
        __STRING_DEFINE__( unlocklevel );
        __STRING_DEFINE__( effect );
        __STRING_DEFINE__( recruitweight );
        __STRING_DEFINE__( recruitdiscount );
        __STRING_DEFINE__( recruitprofession );
        __STRING_DEFINE__( recruitlevel );
        __STRING_DEFINE__( recruitminlevel );
        __STRING_DEFINE__( recruitmaxlevel );
        __STRING_DEFINE__( recruitdivisor );
        __STRING_DEFINE__( recruitgrowth );
        __STRING_DEFINE__( mingrowth );
        __STRING_DEFINE__( maxgrowth );
        __STRING_DEFINE__( dialogue );
        __STRING_DEFINE__( cliniccdtime );
        __STRING_DEFINE__( clinicaddnum );
        __STRING_DEFINE__( clinicmaxnum );
        __STRING_DEFINE__( clinicsubmoneypercent );
        __STRING_DEFINE__( smithyweapon );
        __STRING_DEFINE__( buildgather );
        __STRING_DEFINE__( itemextend );
        __STRING_DEFINE__( explorebreakoffprob );
        __STRING_DEFINE__( buildupgrademaxnum );
        __STRING_DEFINE__( iconid );
        __STRING_DEFINE__( iconrecord );
        __STRING_DEFINE__( factionid );
        __STRING_DEFINE__( newversion );
        __STRING_DEFINE__( recruitrefresh );
        __STRING_DEFINE__( recruithero );
        __STRING_DEFINE__( taskreceive );
        __STRING_DEFINE__( buildupgrade );
        __STRING_DEFINE__( buildonekey );
        __STRING_DEFINE__( trainhero );
        __STRING_DEFINE__( trainonekey );
        __STRING_DEFINE__( supplies );
        __STRING_DEFINE__( retaskchain );
        __STRING_DEFINE__( tlog );
        __STRING_DEFINE__( mainstage );
        __STRING_DEFINE__( onelevel );
        __STRING_DEFINE__( twolevel );
        __STRING_DEFINE__( threelevel );
        __STRING_DEFINE__( smithycdtime );
        __STRING_DEFINE__( smithyaddnum );
        __STRING_DEFINE__( smithycollectmax );
        __STRING_DEFINE__( smithystoremax );
        __STRING_DEFINE__( smithycritrate );
        __STRING_DEFINE__( smithycritvalue );
        __STRING_DEFINE__( smithymakemax );
        __STRING_DEFINE__( smithymakelevel );
        __STRING_DEFINE__( smithyitemid );
        __STRING_DEFINE__( smithystorenum );
        __STRING_DEFINE__( traincostscaledec );
        __STRING_DEFINE__( traincount );
        __STRING_DEFINE__( trainunitexp );
        __STRING_DEFINE__( trainexpscale );
        __STRING_DEFINE__( exploretown );
        __STRING_DEFINE__( playerrecord );
        __STRING_DEFINE__( story );
        __STRING_DEFINE__( mainstory );
        __STRING_DEFINE__( sequence );
        __STRING_DEFINE__( dialoguebranch );
        __STRING_DEFINE__( parentid );
        __STRING_DEFINE__( childid );
        __STRING_DEFINE__( granaryaddnum );
        __STRING_DEFINE__( granarymaxnum );
        __STRING_DEFINE__( granarybuycount );
        __STRING_DEFINE__( granarybuyfactor );
        __STRING_DEFINE__( granarycritrate );
        __STRING_DEFINE__( granarycritvalue );
        __STRING_DEFINE__( granarycdtime );
        __STRING_DEFINE__( granaryfreebuycount );
        __STRING_DEFINE__( buycount );
        __STRING_DEFINE__( granarybuy );
        __STRING_DEFINE__( herolevel );
        __STRING_DEFINE__( maxlevel );
        __STRING_DEFINE__( campupgradelist );
        __STRING_DEFINE__( campdectime );
        __STRING_DEFINE__( campdecconsume );
        __STRING_DEFINE__( food );
        __STRING_DEFINE__( rune );
        __STRING_DEFINE__( faith );
        __STRING_DEFINE__( enterfaith );
        __STRING_DEFINE__( leavefaith );
        __STRING_DEFINE__( flee );
        __STRING_DEFINE__( foodpercent );
        __STRING_DEFINE__( moneypercent );
        __STRING_DEFINE__( suppliespercent );
        __STRING_DEFINE__( runeslot );
        __STRING_DEFINE__( runecompound );
        __STRING_DEFINE__( gamble );
        __STRING_DEFINE__( pvedata );
        __STRING_DEFINE__( exchange );
        __STRING_DEFINE__( select );
        __STRING_DEFINE__( jump );
        __STRING_DEFINE__( contracttimefid );
        __STRING_DEFINE__( contractpricefid );
        __STRING_DEFINE__( contracthero );
        __STRING_DEFINE__( technologycount );
        __STRING_DEFINE__( logicevent );
        __STRING_DEFINE__( remove );
        __STRING_DEFINE__( voice );
        __STRING_DEFINE__( custom );
    }
}

#endif
