#pragma once

#include <osg/NodeCallback>

#include <osgEarthUtil/SkyNode>

namespace eqEarth
{
// ----------------------------------------------------------------------------

struct SkyUpdateCallback : public osg::NodeCallback
{
SkyUpdateCallback( ) : _last( -1 ) { }

void operator( )( osg::Node* node, osg::NodeVisitor* nv )
{
    if( osg::NodeVisitor::UPDATE_VISITOR == nv->getVisitorType( ))
    {
        osg::ref_ptr< const osg::FrameStamp > fs = nv->getFrameStamp( );

        struct tm now;
        fs->getCalendarTime( now );

        if( _last != now.tm_sec )
        {
            static_cast< osgEarth::Util::SkyNode* >( node )->setDateTime(
                now.tm_year + 1900, now.tm_mon + 1, now.tm_mday,
                now.tm_hour + ( now.tm_min / 60.0 ) + ( now.tm_sec / 3600.0 ));
            _last = now.tm_sec;
        }
    }

    traverse( node, nv );
}

int _last;
};
};
