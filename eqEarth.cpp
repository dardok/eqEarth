#include "eqEarth.h"

#include "config.h"

namespace eqEarth
{
// ----------------------------------------------------------------------------

EQEarth::EQEarth( const InitData& initData )
    : _initData( initData )
{
}

int EQEarth::run( )
{
    int ret = EXIT_FAILURE;
    eq::ServerPtr server;
    eq::fabric::ConfigParams configParams;
    Config* config = NULL;

    server = new eq::Server( );
    if( !connectServer( server ))
    {
        LBERROR << "Client::connectServer failed" << std::endl;
        goto out;
    }

    config = static_cast< Config* >( server->chooseConfig( configParams ));
    if( !config )
    {
        LBERROR << "Server::chooseConfig failed" << std::endl;
        goto out_disconnect;
    }

    config->setInitData( _initData );
    if( !config->init( ))
    {
        LBERROR << "Config::init failed" << std::endl;
        goto out_release;
    }

#if 0
    if( config->getError( ))
    {
        LBERROR << "Config::init error : " << config->getError( ) << std::endl;
        goto out_exit;
    }
#endif

    while( config->isRunning( ))
    {
        config->startFrame( );
        config->finishFrame( );
    }
    config->finishAllFrames( );

    ret = EXIT_SUCCESS;

out_exit:
    if( !config->exit( ))
        LBWARN << "Config::exit failed" << std::endl;

out_release:
    server->releaseConfig( config );

out_disconnect:
    if( !disconnectServer( server ))
        LBWARN << "Client::disconnectServer failed" << std::endl;

out:
    server = 0;

    return ret;
}
}
