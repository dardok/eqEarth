#pragma once

#define EQ_IGNORE_GLEW
#include <eq/eq.h>

namespace eqEarth
{
class Pipe : public eq::Pipe
{
public:
    Pipe( eq::Node* parent );

protected:
    virtual ~Pipe( );

protected:
    virtual bool configInit( const eq::uint128_t& initID );
    virtual bool configExit( );

    virtual void frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber );
    virtual void frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber );

private:
    void cleanup( );
};
}
