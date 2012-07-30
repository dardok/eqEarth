#pragma once

//#include "Controls"
#include <osgEarthUtil/Controls>

namespace eqEarth
{
class LonLatLabelControl : public osgEarth::Util::Controls::LabelControl
{
public:
    LonLatLabelControl( const std::string& value = "",
        float fontSize = 18.0f,
        const osg::Vec4f& foreColor = osg::Vec4f( 1, 1, 1, 1 ));

    LonLatLabelControl( const std::string& value,
        const osg::Vec4f& foreColor );

    void updateLonLat( const double lon, const double lat );

private:
    const std::string deg2dms( double deg, const char c[2], int w );
    const std::string lat2dms( double lat );
    const std::string lon2dms( double lon );
    const std::string lonlat2str( double lon, double lat );

    double _lon, _lat;
};
}
