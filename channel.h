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
/*
    virtual void frameAssemble( const eq::uint128_t& frameID );
    virtual void frameReadback( const eq::uint128_t& frameID );
*/

    virtual bool processEvent( eq::EventType type, eq::PointerEvent& event ) override;
    virtual bool processEvent( eq::EventType type, eq::KeyEvent& event ) override;

    virtual bool useOrtho( ) const { return false; }

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
    friend class Window;
    void clearCameras( ); // Must call with GL context (i.e. Window::configExitGL)

    void cleanup( );

    void connectCameraToScene( const eq::uint128_t& id );
    void connectCameraToOverlay( const eq::uint128_t& id );

    void __applyBuffer( osg::Camera* camera );
    void __applyColorMask( osg::Camera* camera ) const;
    void __applyViewport( osg::Camera* camera ) const;
    void __applyFrustum( osg::Camera* camera ) const;
    void __applyPerspective( osg::Camera* camera ) const;
    void __applyOrtho( osg::Camera* camera ) const;

    void __applyHeadTransform( osg::Camera* camera ) const;
    void __applyPerspectiveTransform( osg::Camera* camera ) const;
    void __applyOrthoTransform( osg::Camera* camera ) const;
    void __applyScreenFrustum( osg::Camera* camera ) const;
};
}
