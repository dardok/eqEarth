#include "initData.h"

namespace eqEarth
{
// ----------------------------------------------------------------------------

InitData::InitData( )
        : _frameDataID( eq::UUID::ZERO )
        , _modelFileName( DEFAULT_MODEL )
{
}

InitData::~InitData( )
{
    setFrameDataID( eq::UUID::ZERO );
}

void InitData::setFrameDataID( const eq::uint128_t& id )
{
    _frameDataID = id;
}

void InitData::setModelFileName( const std::string &fileName )
{
    _modelFileName = fileName;
}

void InitData::getInstanceData( co::DataOStream& stream )
{
    stream << _frameDataID << _modelFileName;
}

void InitData::applyInstanceData( co::DataIStream& stream )
{
    stream >> _frameDataID >> _modelFileName;
}

bool InitData::parseCommandLine( char **argv, int argc )
{
    std::string model = _parseCommandLineParam( argc, argv, "--model" );
    if( !model.empty( ))
    {
        setModelFileName( model );
    }

    return true;
}

std::string InitData::_parseCommandLineParam( int argc, char** argv,
    std::string param )
{
    for ( int i = 1; i < argc; i++ )
    {
        if( strcmp( argv[i], param.c_str( )) == 0 )
        {
            ++i;
            if( i < argc )
                return argv[i];
        }
    }

    return "";
}
}
