#pragma once

#define EQ_IGNORE_GLEW
#include <eq/eq.h>

namespace eqEarth
{
class FrameData : public eq::fabric::Serializable
{
public:
    FrameData( );

    void setSimulationTime( double simTime );
    double getSimulationTime( ) const { return _simulationTime; }

    void setCalendarTime( time_t calendarTime );
    time_t getCalendarTime( ) const { return _calendarTime; }

    void setCurrentViewID( const eq::uint128_t& id );
    eq::uint128_t getCurrentViewID( ) const { return _currentViewID; }

    void toggleStatistics( );
    bool useStatistics( ) const { return _statistics; }

protected:
    virtual void serialize( co::DataOStream& os, const uint64_t dirtyBits );
    virtual void deserialize( co::DataIStream& is, const uint64_t dirtyBits );

    enum DirtyBits
    {
        DIRTY_TIME    = eq::fabric::Serializable::DIRTY_CUSTOM << 0,
        DIRTY_VIEW    = eq::fabric::Serializable::DIRTY_CUSTOM << 1,
        DIRTY_FLAGS   = eq::fabric::Serializable::DIRTY_CUSTOM << 2
    };

    virtual ChangeType getChangeType( ) const { return DELTA; }

private:
    double _simulationTime;
    time_t _calendarTime;
    eq::uint128_t _currentViewID;
    bool _statistics;
};
}
