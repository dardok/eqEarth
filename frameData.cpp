#include "frameData.h"

namespace eqEarth
{
// ----------------------------------------------------------------------------

FrameData::FrameData( )
    : _simulationTime( 0. )
    , _calendarTime( (time_t)0 )
    , _currentViewID( 0 )
    , _statistics( true )
    , _zmode( false )
    , _zvalue( 50.0f )
{
}

void FrameData::setSimulationTime( double simTime )
{
    if( _simulationTime != simTime )
    {
        _simulationTime = simTime;
        setDirty( DIRTY_TIME );
    }
}

void FrameData::setCalendarTime( time_t calendarTime )
{
    if( _calendarTime != calendarTime )
    {
        _calendarTime = calendarTime;
        setDirty( DIRTY_TIME );
    }
}

void FrameData::setCurrentViewID( const eq::uint128_t& id )
{
    if( id != _currentViewID )
    {
        _currentViewID = id;
        setDirty( DIRTY_VIEW );
    }
}

void FrameData::toggleStatistics( )
{
    _statistics = !_statistics;
    setDirty( DIRTY_FLAGS );
}

void FrameData::toggleZMode( )
{
    _zmode = !_zmode;
    setDirty( DIRTY_ZMODE );
}

void FrameData::setZValue( float zvalue )
{
    if( _zvalue != zvalue )
    {
        _zvalue = zvalue;
        setDirty( DIRTY_ZVALUE );
    }
}

void FrameData::serialize( co::DataOStream& os, const uint64_t dirtyBits )
{
    if( dirtyBits & DIRTY_TIME )
        os << _simulationTime << _calendarTime;
    if( dirtyBits & DIRTY_VIEW )
        os << _currentViewID;
    if( dirtyBits & DIRTY_FLAGS )
        os << _statistics;
    if( dirtyBits & DIRTY_ZMODE )
        os << _zmode;
    if( dirtyBits & DIRTY_ZVALUE )
        os << _zvalue;
}

void FrameData::deserialize( co::DataIStream& is, const uint64_t dirtyBits )
{
    if( dirtyBits & DIRTY_TIME )
        is >> _simulationTime >> _calendarTime;
    if( dirtyBits & DIRTY_VIEW )
        is >> _currentViewID;
    if( dirtyBits & DIRTY_FLAGS )
        is >> _statistics;
    if( dirtyBits & DIRTY_ZMODE )
        is >> _zmode;
    if( dirtyBits & DIRTY_ZVALUE )
        is >> _zvalue;
}
}
