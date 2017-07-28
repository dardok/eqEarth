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
    void shutdown( );

    static osgViewer::View* createOSGView( const eq::uint128_t& id );
    osgViewer::View* findOSGViewByID( const eq::uint128_t& id );

    void advance( const uint32_t frameNumber, const FrameData& frameData );

    void frameStart( const uint32_t frameNumber, const FrameData& frameData );
    void frameDrawFinish( );

    // AppNode only
    virtual void renderingTraversals( bool needMakeCurrentInThisThread );
    virtual void realize( );
};
}
