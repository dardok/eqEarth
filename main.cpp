#include "nodeFactory.h"
#include "error.h"
#include "initData.h"
#include "eqEarth.h"

int main( const int argc, char** argv )
{
    int ret = EXIT_FAILURE;

    eqEarth::initErrors( );
    eqEarth::NodeFactory nodeFactory;
    if( !eq::init( argc, argv, &nodeFactory ))
        EQERROR << "eq::init failed" << std::endl;
    else
    {
        eqEarth::InitData initData;
        if( !initData.parseCommandLine( argv, argc ))
            EQERROR << "InitData::parseCommandLine failed" << std::endl;
        else
        {
            co::base::RefPtr< eqEarth::EQEarth > client =
                new eqEarth::EQEarth( initData );
            if( !client->initLocal( argc, argv ))
                EQERROR << "Client::initLocal failed" << std::endl;
            else
            {
                ret = client->run( );
                client->exitLocal( );
            }

            EQASSERTINFO( client->getRefCount( ) == 1,
                "Client still referenced by " << client->getRefCount( ) - 1 );
            client = 0;

            eq::exit( );
        }
    }

    eqEarth::exitErrors( );
    return ret;
}
