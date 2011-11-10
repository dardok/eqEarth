#pragma once

#define EQ_IGNORE_GLEW
#include <eq/eq.h>

#include "frameData.h"
#include "viewer.h"
#include "channel.h"

#include <osgGA/GUIEventHandler>

namespace eqEarth
{
class Node : public eq::Node
{
public:
    Node( eq::Config* parent );

protected:
    virtual ~Node( );

public:
    void addGraphicsContext( osg::GraphicsContext *context );
    void removeGraphicsContext( osg::GraphicsContext *context );

    bool addCameraToView( const eq::uint128_t& id, osg::Camera *camera );
    bool removeCameraFromView( const eq::uint128_t& id, osg::Camera *camera );

    const FrameData& getFrameData( ) const { return _frameData; }

    CompositeViewer *getViewer( ) { return _viewer; }
    const CompositeViewer *getViewer( ) const { return _viewer; }

protected:
    virtual bool configInit( const eq::uint128_t& initID );
    virtual bool configExit( );

    virtual void frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber );
    virtual void frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber );

private:
    void cleanup( );

protected:
    FrameData _frameData;

    osg::ref_ptr< CompositeViewer > _viewer;

private:
    co::base::Lock _viewerlock;
};
}
