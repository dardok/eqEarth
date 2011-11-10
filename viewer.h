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

    static osgViewer::View* createView( const eq::uint128_t& id,
        bool overrideRenderer = true );
    osgViewer::View* findViewByID( const eq::uint128_t& id );

    void advance( const uint32_t frameNumber, const FrameData& frameData );
    void frameStart( const uint32_t frameNumber, FrameData& frameData,
        bool preRender = true );
    void frameFinish( bool postRender = true );

    virtual void realize( );
};
}
