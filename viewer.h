#pragma once

#include <osgViewer/View>
#include <osgViewer/CompositeViewer>

#include "frameData.h"

namespace eqEarth
{
class CompositeViewer : public osgViewer::CompositeViewer
{
public:
    void setGlobalContext( osg::GraphicsContext *context );

    static osgViewer::View* createOSGView( const eq::uint128_t& id,
        bool overrideRenderer = true );
    osgViewer::View* findOSGViewByID( const eq::uint128_t& id );

    void advance( const uint32_t frameNumber, const FrameData& frameData );

    // pre/postRender flag indicates whether the rendering is done by
    // eqChannel::frameDraw (true) or in the case of app rendering
    // with osgViewer::renderingTraversals (false)
    void frameStart( const uint32_t frameNumber, FrameData& frameData,
        bool preRender = true );
    void frameDrawFinish( bool postRender = true );

    virtual void realize( );
};
}
