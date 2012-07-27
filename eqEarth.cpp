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
    eq::ConfigParams configParams;
    Config* config = NULL;

    server = new eq::Server( );
    if( !connectServer( server ))
    {
        EQERROR << "Client::connectServer failed" << std::endl;
        goto out;
    }

    config = static_cast< Config* >( server->chooseConfig( configParams ));
    if( !config )
    {
        EQERROR << "Server::chooseConfig failed" << std::endl;
        goto out_disconnect;
    }

    config->setInitData( _initData );
    if( !config->init( ))
    {
        EQERROR << "Config::init failed" << std::endl;
        goto out_release;
    }

    if( config->getError( ))
    {
        EQERROR << "Config::init error : " << config->getError( ) << std::endl;
        goto out_exit;
    }

    while( config->isRunning( ))
    {
        config->startFrame( );
        config->finishFrame( );
    }
    config->finishAllFrames( );

    ret = EXIT_SUCCESS;

out_exit:
    if( !config->exit( ))
        EQWARN << "Config::exit failed" << std::endl;

out_release:
    server->releaseConfig( config );

out_disconnect:
    if( !disconnectServer( server ))
        EQWARN << "Client::disconnectServer failed" << std::endl;

out:
    server = 0;

    return ret;
}
}
