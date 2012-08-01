#include "error.h"

#include <eq/fabric/types.h>

namespace eqEarth
{
namespace
{
struct ErrorData
{
    const uint32_t code;
    const std::string text;
};

ErrorData _errors[] = {
    { ERROR_EQEARTH_APPCONTEXT_FAILED, "Can't create local app context" },
    { ERROR_EQEARTH_LOADMODEL_FAILED, "Can't load model" },
    { ERROR_EQEARTH_MAPOBJECT_FAILED,
      "Mapping data from application process failed" },

    { 0, "" } // last!
};
}

void initErrors( )
{
    eq::fabric::ErrorRegistry& registry =
        eq::fabric::Global::getErrorRegistry( );

    for( size_t i=0; _errors[i].code != 0; ++i )
        registry.setString( _errors[i].code, _errors[i].text );
}

void exitErrors( )
{
    eq::fabric::ErrorRegistry& registry =
        eq::fabric::Global::getErrorRegistry( );

    for( size_t i=0; _errors[i].code != 0; ++i )
        registry.eraseString( _errors[i].code );
}
}
