#pragma once

#define EQ_IGNORE_GLEW
#include <eq/eq.h>

#if EQ_VERSION_GT( 1, 1, 0 )
#include <eq/fabric/error.h>
#else
#include <eq/error.h>
#endif

namespace eqEarth
{
    enum Error
    {
#if EQ_VERSION_GT( 1, 1, 0 )
        ERROR_EQEARTH_APPCONTEXT_FAILED = eq::fabric::ERROR_CUSTOM,
#else
        ERROR_EQEARTH_APPCONTEXT_FAILED = eq::ERROR_CUSTOM,
#endif
        ERROR_EQEARTH_LOADMODEL_FAILED,
        ERROR_EQEARTH_MAPOBJECT_FAILED
    };

    /** Set up eqEarth-specific error codes. */
    void initErrors( );

    /** Clear eqEarth-specific error codes. */
    void exitErrors( );
}
