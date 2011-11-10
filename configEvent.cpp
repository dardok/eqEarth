#include "configEvent.h"

namespace eqEarth
{
std::ostream& operator << ( std::ostream& os, const ConfigEvent* event )
{
    switch( event->data.type )
    {
        case ConfigEvent::INTERSECTION:
            os << "Hit @ " << event->hit;
            break;

        default:
            os << static_cast< const eq::ConfigEvent* >( event );
            return os;
    }

    return os;
}
}
