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

    void setKMLFileName( const std::string& filename );
    std::string getKMLFileName( ) const { return _kmlFileName; }

protected:
    virtual void getInstanceData( co::DataOStream& stream ) override;
    virtual void applyInstanceData( co::DataIStream& stream ) override;

public:
    bool parseCommandLine( char **argv, int argc );

private:
    std::string _parseCommandLineParam( int argc, char** argv,
        std::string param );

    eq::uint128_t _frameDataID;
    std::string _modelFileName;
    std::string _kmlFileName;
};
}
