#pragma once

#define EQ_IGNORE_GLEW
#include <eq/eq.h>

namespace eqEarth
{
struct ConfigEvent : public eq::ConfigEvent
{
enum Type
{
    INTERSECTION = eq::Event::USER
};

ConfigEvent( )
{
    size = sizeof( ConfigEvent );
}

eq::Vector3d hit;
};

std::ostream& operator << ( std::ostream& os, const ConfigEvent* event );
}
