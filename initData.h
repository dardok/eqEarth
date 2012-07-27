#pragma once

#include <eq/eq.h>

#define DEFAULT_MODEL "cow.osg"

namespace eqEarth
{
class InitData : public co::Object
{
public:
    InitData( ) ;
    virtual ~InitData( );

    void setFrameDataID( const eq::uint128_t& id );
    const eq::uint128_t& getFrameDataID( ) const { return _frameDataID; }

    void setModelFileName( const std::string& filename );
    std::string getModelFileName( ) const { return _modelFileName; }

protected:
    virtual void getInstanceData( co::DataOStream& stream );
    virtual void applyInstanceData( co::DataIStream& stream );

public:
    bool parseCommandLine( char **argv, int argc );

private:
    std::string _parseCommandLineParam( int argc, char** argv,
        std::string param );

    eq::uint128_t _frameDataID;
    std::string _modelFileName;
};
}
