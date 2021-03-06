#pragma once

#include <eq/eq.h>

#include <osgViewer/GraphicsWindow>

namespace eqEarth
{
class Window final : public eq::Window
{
public:
    explicit Window( eq::Pipe* parent );

protected:
    virtual ~Window( );

public:
    osg::GraphicsContext* getGraphicsContext( ) { return _window.get( ); }
    const osg::GraphicsContext* getGraphicsContext( ) const
        { return _window.get( ); }

    osg::State* getState( ) { return _window->getState( ); }
    const osg::State* getState( ) const { return _window->getState( ); }

    unsigned int getContextID( ) const
        { return _window->getState( )->getContextID( ); }

protected:
    virtual bool configInit( const eq::uint128_t& initID ) override;
    virtual bool configInitGL( const eq::uint128_t& initID ) override;
    virtual bool configExitGL( ) override;
    virtual void frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber ) override;
    virtual void frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber ) override;
    virtual void frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber ) override;
    virtual void swapBuffers( ) override;

private:
    struct ClearChannels;

    void cleanup( );

protected:
    osg::ref_ptr< osgViewer::GraphicsWindow > _window;
};
}
