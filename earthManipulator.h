#pragma once

#include <osgEarthUtil/EarthManipulator>

namespace eqEarth
{
class EarthManipulator : public osgEarth::Util::EarthManipulator
{
public:
    EarthManipulator( );

protected:
    ~EarthManipulator( );

private:
    void overrideDefaultSettings( );
};
}
