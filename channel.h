#pragma once

#define EQ_IGNORE_GLEW
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
    bool connectToView( const eq::uint128_t& id );

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

public:
    osg::Camera* getCamera( ) { return _camera; }

protected:
    osg::ref_ptr< osg::Camera > _camera;
    osg::ref_ptr< osgViewer::Renderer > _renderer;
    eq::uint128_t _sceneID;
    bool _newScene;
    virtual void applyScene( );

    virtual void applyBuffer( osg::Camera* camera ) const;
    virtual void applyViewport( osg::Camera* camera ) const;
    virtual void applyPerspective( osg::Camera* camera ) const;
    virtual void applyPerspectiveTransform( osg::Camera* camera,
        const eq::Matrix4d& viewMatrix ) const;
    virtual void applyScreen( osg::Camera* camera ) const;
    virtual void applyScreenTransform( osg::Camera* camera,
        const eq::Matrix4d& viewMatrix ) const;

    inline const FrameData& getFrameData( ) const;

    void updateView( );
    void windowPick( uint32_t x, uint32_t y ) const;
    void worldPick( const eq::Vector3d& origin,
        const eq::Vector3d& direction ) const;

private:
    void cleanup( );

private:
    static co::base::Lock _culllock;
};
}
