#pragma once

#include <eq/eq.h>

#include "viewer.h"

#include <osg/Camera>
#include <osgViewer/Viewer>
#include <osgViewer/Renderer>

#include <osgEarthUtil/Controls>

#define CONTROLS

namespace eqEarth
{
class Channel : public eq::Channel
{
public:
    Channel( eq::Window* parent );

protected:
    virtual ~Channel( );

public:
    osg::Camera* getCamera( ) { return _camera.get( ); }
    const osg::Camera* getCamera( ) const { return _camera.get( ); }

protected:
    const FrameData& getFrameData( ) const;

    virtual bool configInit( const eq::uint128_t& initID );
    virtual bool configExit( );

#if 0
    virtual void frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber );
    virtual void frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber );
    virtual void frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber );
#endif
    virtual void frameClear( const eq::uint128_t& frameID );
    virtual void frameDraw( const eq::uint128_t& frameID );
    virtual void frameViewStart( const eq::uint128_t& frameID );
    virtual void frameViewFinish( const eq::uint128_t& frameID );
    virtual void frameAssemble( const eq::uint128_t& frameID );
    virtual void frameReadback( const eq::uint128_t& frameID );

    virtual bool processEvent( const eq::Event& event );

protected:
    eq::uint128_t _sceneID;
    osg::ref_ptr< osg::Camera > _camera;
    osg::ref_ptr< osgViewer::Renderer > _renderer;

    eq::uint128_t _overlayID;
    osg::ref_ptr< osgViewer::Viewer > _viewer2d;
    osg::ref_ptr< osgEarth::Util::Controls::ControlCanvas > _camera2d;

    void updateView( );
    void windowPick( uint32_t x, uint32_t y ) const;
    void worldPick( const eq::Vector3d& origin,
        const eq::Vector3d& direction ) const;

private:
    void cleanup( );

    void connectCameraToScene( const eq::uint128_t& id );
    void connectCameraToOverlay( const eq::uint128_t& id );

    void _applyScene( osg::Camera* camera );
    void _applyView( osg::Camera* camera );

    void _applyBuffer( osg::Camera* camera );
    void _applyViewport( osg::Camera* camera );

    void _applyPerspective( osg::Camera* camera );
    void _applyPerspectiveTransform( osg::Camera* camera );

    void _applyScreen( osg::Camera* camera );
    void _applyScreenTransform( osg::Camera* camera );
};
}
