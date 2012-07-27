#include "pipe.h"

namespace eqEarth
{
// ----------------------------------------------------------------------------

Pipe::Pipe( eq::Node* parent )
    : eq::Pipe( parent )
{
EQINFO << "=====> Pipe::Pipe(" << (void *)this << ")" << std::endl;
}

Pipe::~Pipe( )
{
EQINFO << "<===== Pipe::~Pipe(" << (void *)this << ")" << std::endl;
}

bool Pipe::configInit( const eq::uint128_t& initID )
{
EQINFO << "-----> Pipe::configInit(" << initID << ")" << std::endl;

    bool init = false;

    if( !eq::Pipe::configInit( initID ))
        goto out;

    init = true;

out:
    if( !init )
        cleanup( );

EQINFO << "<----- Pipe::configInit(" << initID << ")" << std::endl;

    return init;
}

bool Pipe::configExit( )
{
    cleanup( );

    return eq::Pipe::configExit( );
}

void Pipe::frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//EQINFO << "-----> Pipe<" << getName( ) << ">::frameStart("
//    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Pipe::frameStart( frameID, frameNumber );

//EQINFO << "<----- Pipe<" << getName( ) << ">::frameStart("
//    << frameID <<  ", " << frameNumber << ")" << std::endl;
}

void Pipe::frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//EQINFO << "-----> Pipe<" << getName( ) << ">::frameFinish("
//    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Pipe::frameFinish( frameID, frameNumber );

//EQINFO << "<----- Pipe<" << getName( ) << ">::frameFinish("
//    << frameID <<  ", " << frameNumber << ")" << std::endl;
}

void Pipe::frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//EQINFO << "-----> Pipe<" << getName( ) << ">::frameDrawFinish("
//    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Pipe::frameDrawFinish( frameID, frameNumber );

//EQINFO << "<----- Pipe<" << getName( ) << ">::frameDrawFinish("
//    << frameID <<  ", " << frameNumber << ")" << std::endl;
}

void Pipe::cleanup( )
{
}
}
