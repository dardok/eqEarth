#pragma once

#include <eq/eq.h>

namespace eqEarth
{
struct ConfigEvent : public eq::ConfigEvent
{
enum Type
{
    INTERSECTION = eq::Event::USER
};

explicit ConfigEvent( )
{
    size = sizeof( ConfigEvent );
}

eq::Vector3d hit;
};

std::ostream& operator << ( std::ostream& os, const ConfigEvent* event );
}
