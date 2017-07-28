#pragma once

#include <eq/eq.h>

#include "frameData.h"
#include "viewer.h"
#include "channel.h"

#include <osgGA/GUIEventHandler>
#include <osgViewer/Renderer>

namespace eqEarth
{
class Node final : public eq::Node
{
public:
    explicit Node( eq::Config* parent );

protected:
    virtual ~Node( );

public:
    const FrameData& getFrameData( ) const { return _frameData; }

    void addGraphicsContext( osg::GraphicsContext* context );
    void removeGraphicsContext( osg::GraphicsContext* context );

    void addCameraToOSGView( const eq::uint128_t& id, osg::Camera* camera );
    void removeCameraFromOSGView( const eq::uint128_t& id,
        osg::Camera* camera );

#if 0
    CompositeViewer *getViewer( ) { return _viewer; }
    const CompositeViewer *getViewer( ) const { return _viewer; }
#endif

protected:
    virtual bool configInit( const eq::uint128_t& initID );
    virtual bool configExit( );

    virtual void frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber );
    virtual void frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber );
    virtual void frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber );

private:
    void cleanup( );

protected:
    FrameData _frameData;
    uint32_t _frameNumber;

    mutable lunchbox::Lock _viewer_lock;
    osg::ref_ptr< CompositeViewer > _viewer;

public:
    void renderLocked( osgViewer::Renderer* renderer ) const;
};
}
