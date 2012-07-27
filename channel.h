#pragma once

#include <eq/eq.h>

#include "viewer.h"

#include <osg/Camera>
#include <osgViewer/Renderer>

#define CONTROLS

namespace eqEarth
{
class Channel : public eq::Channel
{
public:
    Channel( eq::Window* parent );

protected:
    virtual ~Channel( );

protected:
    const FrameData& getFrameData( ) const;

    bool connectCameraToView( const eq::uint128_t& id );

    virtual bool configInit( const eq::uint128_t& initID );
    virtual bool configExit( );

    virtual void frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber );
    virtual void frameViewStart( const eq::uint128_t& frameID );
    virtual void frameClear( const eq::uint128_t& frameID );
    virtual void frameDraw( const eq::uint128_t& frameID );
    virtual void frameViewFinish( const eq::uint128_t& frameID );
    virtual void frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber );

    virtual bool processEvent( const eq::Event& event );

protected:
    osg::ref_ptr< osg::Camera > _camera;
    osg::ref_ptr< osgViewer::Renderer > _renderer;
    eq::uint128_t _sceneID;
    bool _newScene;

    void updateView( );
    void windowPick( uint32_t x, uint32_t y ) const;
    void worldPick( const eq::Vector3d& origin,
        const eq::Vector3d& direction ) const;

private:
    void cleanup( );

    void _applyBuffer( osg::Camera* camera ) const;
    void _applyViewport( osg::Camera* camera ) const;

    void _applyScene( ) const;
    void _applyPerspective( osg::Camera* camera ) const;
    void _applyPerspectiveTransform( osg::Camera* camera,
        const eq::Matrix4d& viewMatrix ) const;

    void _applyView( ) const;
    void _applyScreen( osg::Camera* camera ) const;
    void _applyScreenTransform( osg::Camera* camera,
        const eq::Matrix4d& viewMatrix ) const;
};
}
