#include "window.h"
#include "node.h"

#include <osgEarth/Registry>

namespace eqEarth
{
// ----------------------------------------------------------------------------

struct WindowingSystem : public osg::GraphicsContext::WindowingSystemInterface
{
WindowingSystem( osg::GraphicsContext *context ) : _context( context ) { }

virtual unsigned int getNumScreens(
    const osg::GraphicsContext::ScreenIdentifier& screenIdentifier =
        osg::GraphicsContext::ScreenIdentifier( ))
{
    EQUNIMPLEMENTED;
    return 0;
}

virtual void getScreenSettings(
    const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
        osg::GraphicsContext::ScreenSettings& resolution )
{
    EQUNIMPLEMENTED;
}

virtual void enumerateScreenSettings(
    const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
        osg::GraphicsContext::ScreenSettingsList& resolutionList )
{
    EQUNIMPLEMENTED;
}

virtual osg::GraphicsContext* createGraphicsContext(
    osg::GraphicsContext::Traits* traits )
{
    return _context.get( );
}

void initCapabilities( )
{
    osg::ref_ptr< osg::GraphicsContext::WindowingSystemInterface > original =
        osg::GraphicsContext::getWindowingSystemInterface( );
    osg::GraphicsContext::setWindowingSystemInterface( this );
    EQ_GL_CALL( osgEarth::Registry::instance( )->getCapabilities( ));
    osg::GraphicsContext::setWindowingSystemInterface( original );
}

osg::observer_ptr< osg::GraphicsContext > _context;
};

// ----------------------------------------------------------------------------

Window::Window( eq::Pipe* parent )
    : eq::Window( parent )
{
EQINFO << "=====> Window::Window(" << (void *)this << ")" << std::endl;
}

Window::~Window( )
{
EQINFO << "<===== Window::~Window(" << (void *)this << ")" << std::endl;
}

void Window::initCapabilities( osg::GraphicsContext *context )
{
    static co::base::Lock _wsiLock;
    static bool _initialized = false;

    co::base::ScopedMutex<> mutex( _wsiLock );
    if( !_initialized )
    {
        // Hack to initialize osgEarth with an existing graphics context
        osg::ref_ptr< WindowingSystem > wsi = new WindowingSystem( context );
        wsi->initCapabilities( );
        _initialized = true;
    }
}

bool Window::configInit( const eq::uint128_t& initID )
{
    setIAttribute( IATTR_PLANES_ALPHA,   8 ); // enforce visual with alpha
    setIAttribute( IATTR_PLANES_STENCIL, 8 ); // enforce visual with stencil

    return eq::Window::configInit( initID );
}

bool Window::configInitGL( const eq::uint128_t& initID )
{
EQINFO << "-----> Window::configInitGL(" << initID <<
    ", " << getPixelViewport( ) <<
    ", " << getViewport( ) << ")" << std::endl;

    bool init = false;

    EQASSERT( !_window.valid( ));

    if( eq::Window::configInitGL( initID ))
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
        traits->pbuffer =
            ( getIAttribute( IATTR_HINT_DRAWABLE ) == eq::PBUFFER );

        const Window* sharedWindow =
            static_cast< const Window* >( getSharedContextWindow( ));
        if( sharedWindow && ( sharedWindow != this ))
            traits->sharedContext = sharedWindow->_window;

        _window = new osgViewer::GraphicsWindowEmbedded( traits );
	//_window->createGraphicsThread( );

	static_cast< Node* >( getNode( ))->addGraphicsContext( _window );

        unsigned int maxTexturePoolSize =
            osg::DisplaySettings::instance( )->getMaxTexturePoolSize( );
        unsigned int maxBufferObjectPoolSize =
            osg::DisplaySettings::instance( )->getMaxBufferObjectPoolSize( );

        if( maxTexturePoolSize > 0 )
            getState( )->setMaxTexturePoolSize( maxTexturePoolSize );
        if( maxBufferObjectPoolSize > 0 )
            getState( )->setMaxBufferObjectPoolSize( maxBufferObjectPoolSize );

        initCapabilities( _window );

        init = true;
    }

    if( !init )
        cleanup( );

EQINFO << "<----- Window::configInitGL(" << initID << ")" << std::endl;

    return init;
}

bool Window::configExitGL( )
{
    cleanup( );

    return eq::Window::configExitGL( );
}

void Window::frameStart( const eq::uint128_t& frameID,
    const uint32_t frameNumber )
{
//EQINFO << "-----> Window::frameStart(" << frameID << ", " << frameNumber << ")" << std::endl;

    EQASSERT( _window.valid( ) && _window->valid( ));

    // Ensures extension procs are initialized
    _window->makeCurrent( );

    // Runs the IncrementalCompileOperation if it is installed
    EQ_GL_CALL( _window->runOperations( ));

    eq::Window::frameStart( frameID, frameNumber );

//EQINFO << "<----- Window::frameStart(" << frameID << ")" << std::endl;
}

void Window::frameFinish( const eq::uint128_t& frameID,
    const uint32_t frameNumber )
{
//EQINFO << "-----> Window::frameFinish(" << frameID << ")" << std::endl;

    EQASSERT( _window.valid( ) && _window->valid( ));

    // For completeness
    _window->releaseContext( );

    eq::Window::frameFinish( frameID, frameNumber );

//EQINFO << "<----- Window::frameFinish(" << frameID << ")" << std::endl;
}

void Window::swapBuffers( )
{
    if( _window.valid( ) && _window->valid( ))
        // Calls clear which ensures _lastClearTick is set for the pager
        _window->swapBuffers( );

    eq::Window::swapBuffers( );
}

void Window::cleanup( )
{
    if( _window.valid( ))
    {
	static_cast< Node* >( getNode( ))->removeGraphicsContext( _window );

        if( _window->valid( ))
            _window->close( );
    }

    _window = 0;
}
}
