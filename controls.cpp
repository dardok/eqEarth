#include "controls.h"

#include <iomanip>
#include <sstream>

namespace eqEarth
{
// ----------------------------------------------------------------------------

LonLatLabelControl::LonLatLabelControl( const std::string& value,
    float fontSize, const osg::Vec4f& foreColor )
    : LabelControl( value, fontSize, foreColor )
    , _lon( 0.0 ), _lat( 0.0 )
{
    updateLonLat( _lon, _lat );
};

LonLatLabelControl::LonLatLabelControl( const std::string& value,
    const osg::Vec4f& foreColor )
    : LabelControl( value, foreColor )
    , _lon( 0.0 ), _lat( 0.0 )
{
    updateLonLat( _lon, _lat );
};

void LonLatLabelControl::updateLonLat( const double lon, const double lat )
{
    if(( lon != _lon ) || ( lat != _lat ))
    {
        _lon = lon; _lat = lat;
        setText( lonlat2str( _lon, _lat ), osgText::String::ENCODING_UTF8 );
    }
};

const std::string LonLatLabelControl::deg2dms( double deg, const char c[2],
    int w )
{
    double min = 0.0, sec = 0.0;
    int degInt = 0, minInt = 0;

    char m = c[0];
    if (deg < 0.0)
    {
        deg = fabs( deg );
        m = c[1];
    }
    degInt = static_cast< int >( deg );
    min = ( deg - degInt ) * 60.0;
    minInt = static_cast< int >( min );
    sec = ( min - minInt ) * 60.0;
    std::stringstream buf;
    buf << std::setfill( '0' ) << std::setw( w ) << degInt << "°" <<
        std::setw( 2 ) << minInt << "'" << std::setw( 5 ) << std::fixed <<
        std::setprecision( 2 ) << sec << "\"" << m;
    return buf.str( );
};

const std::string LonLatLabelControl::lat2dms( double lat )
{
    return deg2dms( lat, "NS", 2 );
};

const std::string LonLatLabelControl::lon2dms( double lon )
{
    return deg2dms( lon, "EW", 3 );
};

const std::string LonLatLabelControl::lonlat2str( double lon, double lat )
{
    std::stringstream buf;
    buf << lon2dms( lon ) << "  " << lat2dms( lat );
    return buf.str( );
};
}
