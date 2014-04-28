#pragma once

#include <osg/NodeCallback>

#include <osgEarthUtil/Sky>

#include "view.h"
#include "controls.h"

namespace eqEarth
{
// ----------------------------------------------------------------------------

struct SkyUpdateCallback : public osg::NodeCallback
{
SkyUpdateCallback( ) : _lastSec( -1 ) { }

void operator( )( osg::Node* node, osg::NodeVisitor* nv )
{
    if( osg::NodeVisitor::UPDATE_VISITOR == nv->getVisitorType( ))
    {
        osg::ref_ptr< const osg::FrameStamp > fs = nv->getFrameStamp( );

        struct tm now;
        fs->getCalendarTime( now );

        if( _lastSec != now.tm_sec )
        {
            static_cast< osgEarth::Util::SkyNode* >( node )->setDateTime(
                osgEarth::Util::DateTime( now.tm_year + 1900, now.tm_mon + 1, now.tm_mday,
                    now.tm_hour + ( now.tm_min / 60.0 ) + ( now.tm_sec / 3600.0 )));
            _lastSec = now.tm_sec;
        }
    }

    traverse( node, nv );
}

private:
    int _lastSec;
};

// ----------------------------------------------------------------------------

struct ControlUpdateCallback : public osg::NodeCallback
{
ControlUpdateCallback( View* view, LonLatLabelControl* ll )
    : _view( view ), _ll( ll ), _last_lat( 0. ), _last_lon( 0. ) { }

void operator( )( osg::Node* node, osg::NodeVisitor* nv )
{
    lunchbox::ScopedWrite _mutex( _update_lock );

    if( osg::NodeVisitor::UPDATE_VISITOR == nv->getVisitorType( ))
    {
#if 0
        osgEarth::Util::Controls::ControlCanvas* cc =
            static_cast< osgEarth::Util::Controls::ControlCanvas* >( node );
#endif

        double lat, lon;
        _view->getLatLon( lat, lon );
        if(( _last_lat != lat ) || ( _last_lon != lon ))
        {
            _ll->updateLonLat( lon, lat );
            _last_lat = lat; _last_lon = lon;
        }
    }

    traverse( node, nv );
}

private:
    View* _view;
    osg::ref_ptr< LonLatLabelControl > _ll;
    double _last_lat, _last_lon;
    static lunchbox::Lock _update_lock;
};

lunchbox::Lock ControlUpdateCallback::_update_lock;
};
