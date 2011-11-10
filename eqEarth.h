#pragma once

#include "initData.h"

#define EQ_IGNORE_GLEW
#include <eq/eq.h>

namespace eqEarth
{
class EQEarth : public eq::Client
{
public:
    EQEarth( const InitData& initData );

    int run( );

private:
    const InitData& _initData;
};
}
