#pragma once

#include <eq/eq.h>

namespace eqEarth
{
class Pipe final : public eq::Pipe
{
public:
    explicit Pipe( eq::Node* parent );

    static lunchbox::SpinLock *getPipeLock( ) { return &_pipeLock; };

protected:
    virtual ~Pipe( );

protected:
    virtual bool configInit( const eq::uint128_t& initID ) override;
    virtual bool configExit( ) override;

    virtual void frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber ) override;
    virtual void frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber ) override;
    virtual void frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber ) override;

private:
    void cleanup( );

    static lunchbox::SpinLock _pipeLock;
};
}
