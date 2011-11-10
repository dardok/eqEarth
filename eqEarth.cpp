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

    eq::ServerPtr server = new eq::Server( );
    if( !connectServer( server ))
        EQERROR << "Client::connectServer failed" << std::endl;
    else
    {
        eq::ConfigParams configParams;
        Config* config = static_cast< Config* >(
            server->chooseConfig( configParams ));
        if( !config )
            EQERROR << "Server::chooseConfig failed" << std::endl;
        else
        {
            config->setInitData( _initData );
            if( config->init( ))
            {
                if( config->getError( ))
                    EQWARN << "Config::init error : " << config->getError( ) <<
                        std::endl;

                while( config->isRunning( ))
                {
                    config->startFrame( );
                    config->finishFrame( );
                }
                config->finishAllFrames( );

                if( !config->exit( ))
                    EQWARN << "Config::exit failed" << std::endl;

                ret = EXIT_SUCCESS;
            }

            server->releaseConfig( config );
        }

        if( !disconnectServer( server ))
            EQWARN << "Client::disconnectServer failed" << std::endl;
    }

    server = 0;

    return ret;
}
}
