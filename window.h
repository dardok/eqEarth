#pragma once

#define EQ_IGNORE_GLEW
#include <eq/eq.h>

#include <osgViewer/GraphicsWindow>

namespace eqEarth
{
class Window : public eq::Window
{
public:
    Window( eq::Pipe* parent );

protected:
    virtual ~Window( );

public:
    osg::GraphicsContext *getGraphicsContext( ) { return _window.get( ); }
    const osg::GraphicsContext *getGraphicsContext( ) const
        { return _window.get( ); }

    osg::State* getState( ) { return _window->getState( ); }
    const osg::State* getState( ) const { return _window->getState( ); }

    unsigned int getContextID( ) const
        { return _window->getState( )->getContextID( ); }

    static void initCapabilities( osg::GraphicsContext *context );

protected:
    virtual bool configInit( const eq::uint128_t& initID );
    virtual bool configInitGL( const eq::uint128_t& initID );
    virtual bool configExitGL( );
    virtual void frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber );
    virtual void frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber );
    virtual void swapBuffers( );

private:
    void cleanup( );

protected:
    osg::ref_ptr< osgViewer::GraphicsWindow > _window;
};
}
