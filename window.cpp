#include "channel.h"
#include "window.h"
#include "pipe.h"
#include "node.h"
#include "config.h"

#include <osgEarth/Registry>

namespace eqEarth
{
// ----------------------------------------------------------------------------

Window::Window( eq::Pipe* parent )
    : eq::Window( parent )
{
LBINFO << "=====> Window::Window(" << (void *)this << ")" << std::endl;
}

Window::~Window( )
{
LBINFO << "<===== Window::~Window(" << (void *)this << ")" << std::endl;
}

bool Window::configInit( const eq::uint128_t& initID )
{
#if 0
    setIAttribute( IATTR_PLANES_ALPHA,   8 ); // enforce visual with alpha
    setIAttribute( IATTR_PLANES_STENCIL, 8 ); // enforce visual with stencil
#endif

    return eq::Window::configInit( initID );
}

bool Window::configInitGL( const eq::uint128_t& initID )
{
LBINFO << "-----> Window::configInitGL(" << initID <<
    ", " << getPixelViewport( ) <<
    ", " << getViewport( ) << ")" << std::endl;

    bool init = false;

    LBASSERT( !_window.valid( ));

    if( !eq::Window::configInitGL( initID ))
        goto out;

    {
        const eq::PixelViewport& pvp = getPixelViewport( );
        const eq::DrawableConfig& dc = getDrawableConfig( );

        osg::ref_ptr< osg::GraphicsContext::Traits > traits =
            new osg::GraphicsContext::Traits( );
        traits->x = pvp.x;
        traits->y = pvp.y;
        traits->width = pvp.w;
        traits->height = pvp.h;
        traits->red = traits->blue = traits->green = dc.colorBits;
        traits->alpha = dc.alphaBits;
        traits->stencil = dc.stencilBits;
        traits->doubleBuffer = dc.doublebuffered;
#if 0
        traits->pbuffer =
            ( getIAttribute( IATTR_HINT_DRAWABLE ) == eq::PBUFFER );
#endif
        std::ostringstream version;
        version << dc.glVersion;
        traits->glContextVersion = version.str( );

#if 0
        Window* sharedWindow =
            static_cast< Window* >( getSharedContextWindow( ));
        if( sharedWindow && ( sharedWindow != this ))
            traits->sharedContext = sharedWindow->getGraphicsContext( );
#endif

        _window = new osgViewer::GraphicsWindowEmbedded( traits );

        static_cast< Node* >( getNode( ))->addGraphicsContext( _window );

        const unsigned int maxTexturePoolSize =
            osg::DisplaySettings::instance( )->getMaxTexturePoolSize( );
        const unsigned int maxBufferObjectPoolSize =
            osg::DisplaySettings::instance( )->getMaxBufferObjectPoolSize( );

        if( maxTexturePoolSize > 0 )
            getState( )->setMaxTexturePoolSize( maxTexturePoolSize );
        if( maxBufferObjectPoolSize > 0 )
            getState( )->setMaxBufferObjectPoolSize( maxBufferObjectPoolSize );

        osgEarth::Registry::instance( )->getCapabilities( _window );
    }

    init = true;

out:
    if( !init )
        cleanup( );

LBINFO << "<----- Window::configInitGL(" << initID << ")" << std::endl;

    return init;
}

bool Window::configExitGL( )
{
LBINFO << "------ Window::configExitGL( )" << std::endl;

    lunchbox::ScopedWrite _mutex( Pipe::getPipeLock( ));

    cleanup( );

    return eq::Window::configExitGL( );
}

void Window::frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
LBINFO << "-----> Window<" << getName( ) << ">::frameStart("
    << frameID << ", " << frameNumber << ")" << std::endl;

    LBASSERT( _window.valid( ) && _window->valid( ));

    // Ensures extension procs are initialized
    _window->makeCurrent( );

    // Runs the IncrementalCompileOperation if installed
    _window->runOperations( );

    eq::Window::frameStart( frameID, frameNumber );

LBINFO << "<----- Window<" << getName( ) << ">::frameStart("
    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Window::frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
LBINFO << "-----> Window<" << getName( ) << ">::frameFinish("
    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Window::frameFinish( frameID, frameNumber );

LBINFO << "<----- Window<" << getName( ) << ">::frameFinish("
    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Window::frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
LBINFO << "-----> Window<" << getName( ) << ">::frameDrawFinish("
    << frameID << ", " << frameNumber << ")" << std::endl;

    LBASSERT( _window.valid( ) && _window->valid( ));

    // For completeness
    _window->releaseContext( );

    eq::Window::frameDrawFinish( frameID, frameNumber );

LBINFO << "<----- Window<" << getName( ) << ">::frameDrawFinish("
    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Window::swapBuffers( )
{
    if( _window.valid( ) && _window->valid( ))
        // Calls clear which ensures _lastClearTick is set for the pager
        _window->swapBuffers( );

    eq::Window::swapBuffers( );
}

struct Window::ClearChannels : public eq::ConfigVisitor
{
virtual eq::VisitorResult visit( eq::Channel* channel )
{
    static_cast< Channel* >( channel )->clearCameras( );
}
};

void Window::cleanup( )
{
    if( _window.valid( ))
    {
        ClearChannels c;
	accept( c );

        static_cast< Config* >( getConfig( ))->clearScene( );

        static_cast< Node* >( getNode( ))->removeGraphicsContext( _window );

        _window->close( );
    }

    _window = 0;
}
}
