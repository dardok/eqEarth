#pragma once

#include <osgViewer/View>
#include <osgViewer/CompositeViewer>

#include "frameData.h"

namespace eqEarth
{
class CompositeViewer : public osgViewer::CompositeViewer
{
public:
    // AppNode only
    void setGlobalContext( osg::GraphicsContext *context );

    static osgViewer::View* createOSGView( const eq::uint128_t& id,
        bool overrideRenderer = true );
    osgViewer::View* findOSGViewByID( const eq::uint128_t& id );

    void advance( const uint32_t frameNumber, const FrameData& frameData );

    void frameStart( const uint32_t frameNumber, const FrameData& frameData );
    void frameDrawFinish( );

    virtual void renderingTraversals( bool needMakeCurrent );
    virtual void realize( );
};
}
