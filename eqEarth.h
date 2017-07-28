#pragma once

#include "initData.h"

#include <eq/eq.h>

namespace eqEarth
{
class EQEarth final : public eq::Client
{
public:
    explicit EQEarth( const InitData& initData );

    int run( );

private:
    const InitData& _initData;
};
}
