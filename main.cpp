#include "nodeFactory.h"
#include "error.h"
#include "initData.h"
#include "eqEarth.h"

int main( const int argc, char** argv )
{
    int ret = EXIT_FAILURE;
    eqEarth::NodeFactory nodeFactory;
    eqEarth::InitData initData;
    co::base::RefPtr< eqEarth::EQEarth > client;

    eqEarth::initErrors( );

    if( !eq::init( argc, argv, &nodeFactory ))
    {
        LBERROR << "eq::init failed" << std::endl;
        goto err;
    }

    if( !initData.parseCommandLine( argv, argc ))
    {
        LBERROR << "InitData::parseCommandLine failed" << std::endl;
        goto err;
    }

    client = new eqEarth::EQEarth( initData );
    if( !client->initLocal( argc, argv ))
    {
        LBERROR << "Client::initLocal failed" << std::endl;
        goto out;
    }

    ret = client->run( );

    client->exitLocal( );

    LBASSERTINFO( client->getRefCount( ) == 1,
        "Client still referenced by " << client->getRefCount( ) - 1 );

out:
    client = 0;

    if( !eq::exit( ))
        LBWARN << "eq::exit failed" << std::endl;

err:
    eqEarth::exitErrors( );

    return ret;
}
