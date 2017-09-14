#include "pipe.h"

#include "config.h"

namespace eqEarth
{
// ----------------------------------------------------------------------------

lunchbox::SpinLock Pipe::_pipeLock;

Pipe::Pipe( eq::Node* parent )
    : eq::Pipe( parent )
{
LBINFO << "=====> Pipe::Pipe(" << (void *)this << ")" << std::endl;
}

Pipe::~Pipe( )
{
LBINFO << "<===== Pipe::~Pipe(" << (void *)this << ")" << std::endl;
}

bool Pipe::configInit( const eq::uint128_t& initID )
{
LBINFO << "-----> Pipe::configInit(" << initID << ")" << std::endl;

    bool init = false;

    if( !eq::Pipe::configInit( initID ))
        goto out;

    {
        static_cast< Config* >( getConfig( ))->setThreadHint( isThreaded( ));
    }

    init = true;

out:
    if( !init )
        cleanup( );

LBINFO << "<----- Pipe::configInit(" << initID << ")" << std::endl;

    return init;
}

bool Pipe::configExit( )
{
LBINFO << "------ Pipe::configExit( )" << std::endl;

    lunchbox::ScopedWrite _mutex( Pipe::getPipeLock( ));

    cleanup( );

    return eq::Pipe::configExit( );
}

void Pipe::frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//LBINFO << "-----> Pipe<" << getName( ) << ">::frameStart("
//    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Pipe::frameStart( frameID, frameNumber );

//LBINFO << "<----- Pipe<" << getName( ) << ">::frameStart("
//    << frameID <<  ", " << frameNumber << ")" << std::endl;
}

void Pipe::frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//LBINFO << "-----> Pipe<" << getName( ) << ">::frameFinish("
//    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Pipe::frameFinish( frameID, frameNumber );

//LBINFO << "<----- Pipe<" << getName( ) << ">::frameFinish("
//    << frameID <<  ", " << frameNumber << ")" << std::endl;
}

void Pipe::frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//LBINFO << "-----> Pipe<" << getName( ) << ">::frameDrawFinish("
//    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Pipe::frameDrawFinish( frameID, frameNumber );

//LBINFO << "<----- Pipe<" << getName( ) << ">::frameDrawFinish("
//    << frameID <<  ", " << frameNumber << ")" << std::endl;
}

void Pipe::cleanup( )
{
}
}
