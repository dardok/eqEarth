#include "channel.h"

#include "config.h"
#include "configEvent.h"
#include "node.h"
#include "window.h"
#include "view.h"
#include "viewer.h"
#include "util.h"

#include <GL/glu.h>

namespace eqEarth
{
// ----------------------------------------------------------------------------

Channel::Channel( eq::Window* parent )
    : eq::Channel( parent )
    , _sceneID( eq::UUID::ZERO )
    , _overlayID( eq::UUID::ZERO )
{
LBINFO << "=====> Channel::Channel(" << (void *)this << ")" << std::endl;
}

Channel::~Channel( )
{
LBINFO << "<===== Channel::~Channel(" << (void *)this << ")" << std::endl;
}

const FrameData& Channel::getFrameData( ) const
{
    return static_cast< const Node* >( getNode( ))->getFrameData( );
}

bool Channel::configInit( const eq::uint128_t& initID )
{
LBINFO << "-----> Channel::configInit(" << initID <<
    ", " << getPixelViewport( ) <<
    ", " << getViewport( ) <<
    ", " << (void *)getNativeView( ) <<
    ", " << (void *)getView( ) <<
    ", " << isDestination( ) << ")" << std::endl;

    bool init = false;

    if( !eq::Channel::configInit( initID ))
        goto out;

    {
        osg::ref_ptr< osg::GraphicsContext > gc =
            static_cast< const Window* >( getWindow( ))->getGraphicsContext( );

        _camera = new osg::Camera;
        _camera->setColorMask( new osg::ColorMask );
        _camera->setViewport( new osg::Viewport );
        _camera->setGraphicsContext( gc );
#if 1
        _camera->setComputeNearFarMode(
            osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
#endif
        _camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
        _camera->setAllowEventFocus( false );

        if( isDestination( ) &&
                ( std::string::npos == getNode( )->getName( ).find( "strad" )))
        {
#if 0
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
            traits->pbuffer = false;

            Window* sharedWindow =
                static_cast< Window* >( getWindow( ));
            traits->sharedContext = sharedWindow->getGraphicsContext( );

            osg::ref_ptr< osg::GraphicsContext > gc2 =
                new osgViewer::GraphicsWindowEmbedded( traits );
#else
            osg::ref_ptr< osg::GraphicsContext > gc2 = gc;
#endif

            _viewer2d = new osgViewer::Viewer;
            _viewer2d->setThreadingModel(
                osgViewer::ViewerBase::SingleThreaded );

            osg::ref_ptr< osg::Camera > camera = _viewer2d->getCamera( );
            camera->setColorMask( new osg::ColorMask );
            camera->setViewport( new osg::Viewport );
            camera->setGraphicsContext( gc2 );
#if 1
            camera->setComputeNearFarMode(
                osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
#endif
            camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
            camera->setAllowEventFocus( false );
            camera->setClearMask( GL_DEPTH_BUFFER_BIT );

            camera->setThreadSafeRefUnref( true );
        }
    }

    init = true;

out:
    if( !init )
        cleanup( );

LBINFO << "<----- Channel::configInit(" << initID << ")" << std::endl;

    return init;
}

bool Channel::configExit( )
{
    cleanup( );

    return eq::Channel::configExit( );
}

#if 0
void Channel::frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//LBINFO << "-----> Channel<" << getName( ) << ">::frameStart("
//    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Channel::frameStart( frameID, frameNumber );

//LBINFO << "<----- Channel<" << getName( ) << ">::frameStart("
//    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Channel::frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//LBINFO << "-----> Channel<" << getName( ) << ">::frameFinish("
//    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Channel::frameFinish( frameID, frameNumber );

//LBINFO << "<----- Channel<" << getName( ) << ">::frameFinish("
//    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Channel::frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//LBINFO << "-----> Channel<" << getName( ) << ">::frameDrawFinish("
//    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Channel::frameDrawFinish( frameID, frameNumber );

//LBINFO << "<----- Channel<" << getName( ) << ">::frameDrawFinish("
//    << frameID << ", " << frameNumber << ")" << std::endl;
}
#endif

void Channel::frameClear( const eq::uint128_t& frameID )
{
LBINFO << "-----> Channel<" << getName( ) << ">::frameClear("
    << frameID << ")" << std::endl;

/*
    if( isDestination( ))
    {
        const unsigned int contextID =
            static_cast< Window* >( getWindow( ))->getContextID( );
        osg::GL2Extensions::GL2Extensions* gl2e =
            osg::GL2Extensions::Get( contextID, true );
        gl2e->glUseProgram( 0 ); // Icky...
    }
*/
    glEnable( GL_SCISSOR_TEST );

    eq::Channel::frameClear( frameID );

LBINFO << "<----- Channel<" << getName( ) << ">::frameClear("
    << frameID << ")" << std::endl;
}

void Channel::frameDraw( const eq::uint128_t& frameID )
{
LBINFO << "-----> Channel<" << getName( ) << ">::frameDraw("
    << frameID << ")" << std::endl;

    const View* view = static_cast< const View* >( getView( ));
    LBASSERT( view );

    connectCameraToScene( view->getSceneID( ));

#if 1
    // Update near/far
    double near, far;
    view->getNearFar( near, far );
/*
    double r = far - near;

    const eq::Range &range = getRange( );
    near += r * range.start;
    far = near + ( r * range.end );
*/
    setNearFar( near, far );
#endif

    __applyBuffer( _camera );
    __applyViewport( _camera );

    __applyFrustum( _camera );

    __applyHeadTransform( _camera );

    if( _renderer.valid( ))
        static_cast< const Node* >( getNode( ))->renderLocked( _renderer );

    updateView( );

LBINFO << "<----- Channel<" << getName( ) << ">::frameDraw("
    << frameID << ")" << std::endl;
}

void Channel::frameViewStart( const eq::uint128_t& frameID )
{
LBINFO << "-----> Channel<" << getName( ) << ">::frameViewStart("
    << frameID << ")" << std::endl;

    eq::Channel::frameViewStart( frameID );

    if( _viewer2d.valid( ))
    {
        const View* view = static_cast< const View* >( getNativeView( ));
        LBASSERT( view );

        const FrameData& frameData =
            static_cast< const Node* >( getNode( ))->getFrameData( );

        connectCameraToOverlay( view->getOverlayID( ));

        osg::ref_ptr< osg::DisplaySettings > ds =
            osg::DisplaySettings::instance( );
        if( _viewer2d->getSceneData( ))
            _viewer2d->getSceneData( )->resizeGLObjectBuffers(
                ds->getMaxNumberOfGraphicsContexts( ));

        _viewer2d->advance( frameData.getSimulationTime( ));

        const time_t calendar = frameData.getCalendarTime( );
        struct tm now;
        if( NULL != gmtime_r( &calendar, &now ))
            _viewer2d->getViewerFrameStamp( )->setCalendarTime( now );

        _viewer2d->eventTraversal( );
        _viewer2d->updateTraversal( );
    }

LBINFO << "<----- Channel<" << getName( ) << ">::frameViewStart("
    << frameID << ")" << std::endl;
}

void Channel::frameViewFinish( const eq::uint128_t& frameID )
{
LBINFO << "-----> Channel<" << getName( ) << ">::frameViewFinish("
    << frameID << ")" << std::endl;

    if( _viewer2d.valid( ))
    {
        __applyBuffer( _camera2d );
        __applyViewport( _camera2d );

        __applyScreenFrustum( _camera2d );

        _viewer2d->renderingTraversals( );
    }

    applyBuffer( );
    applyViewport( );

    const FrameData& frameData =
        static_cast< const Node* >( getNode( ))->getFrameData( );
    if( frameData.useStatistics( ))
        drawStatistics( );

    eq::Channel::frameViewFinish( frameID );

LBINFO << "<----- Channel<" << getName( ) << ">::frameViewFinish("
    << frameID << ")" << std::endl;
}

void Channel::frameAssemble( const eq::uint128_t& frameID )
{
//LBINFO << "-----> Channel<" << getName( ) << ">::frameAssemble("
//    << frameID << ")" << std::endl;

    eq::Channel::frameAssemble( frameID );

//LBINFO << "<----- Channel<" << getName( ) << ">::frameAssemble("
//    << frameID << ")" << std::endl;
}

void Channel::frameReadback( const eq::uint128_t& frameID )
{
//LBINFO << "-----> Channel<" << getName( ) << ">::frameReadback("
//    << frameID << ")" << std::endl;

#if 0
    const eq::Frames& frames = getOutputFrames();
    for( eq::FramesCIter i = frames.begin(); i != frames.end(); ++i )
    {
        eq::Frame* frame = *i;
        // OPT: Drop alpha channel from all frames during network transport
        frame->setAlphaUsage( false );

        if( false )
            frame->useCompressor( eq::Frame::BUFFER_COLOR, EQ_COMPRESSOR_AUTO );
        else
            frame->useCompressor( eq::Frame::BUFFER_COLOR, EQ_COMPRESSOR_NONE );
    }
#endif

    eq::Channel::frameReadback( frameID );

//LBINFO << "<----- Channel<" << getName( ) << ">::frameReadback("
//    << frameID << ")" << std::endl;

}

bool Channel::processEvent( const eq::Event& event )
{
    if( _viewer2d.valid( ))
    {
        osg::ref_ptr< osgGA::EventQueue > eventQueue =
            _viewer2d->getEventQueue( );

        LBASSERT( isDestination( ));

        const double time =
            static_cast< double >( getConfig( )->getTime( )) / 1000.;

        const eq::PixelViewport& pvp = event.context.pvp;
        const eq::Viewport& vp = event.context.vp;
        const uint32_t x = event.pointer.x + pvp.x + ( vp.x * ( pvp.w / vp.w ));
        const uint32_t y =
            pvp.h - event.pointer.y + pvp.y + ( vp.y * ( pvp.h / vp.h ));

        switch( event.type )
        {
            case eq::Event::WINDOW_POINTER_WHEEL:
            {
                osgGA::GUIEventAdapter::ScrollingMotion sm =
                    osgGA::GUIEventAdapter::SCROLL_NONE;
                if( event.pointer.xAxis > 0 )
                    sm = osgGA::GUIEventAdapter::SCROLL_UP;
                else if( event.pointer.xAxis < 0 )
                    sm = osgGA::GUIEventAdapter::SCROLL_DOWN;
                else if( event.pointer.yAxis > 0 )
                    sm = osgGA::GUIEventAdapter::SCROLL_RIGHT;
                else if( event.pointer.yAxis < 0 )
                    sm = osgGA::GUIEventAdapter::SCROLL_LEFT;
                eventQueue->mouseScroll( sm, time );
                break;
            }
            case eq::Event::CHANNEL_POINTER_MOTION:
                eventQueue->mouseMotion( x, y, time );
                break;
            case eq::Event::CHANNEL_POINTER_BUTTON_PRESS:
            {
                const unsigned int b = eqButtonToOsg( event.pointer.button );
                if( b <= 3 )
                    eventQueue->mouseButtonPress( x, y, b, time );
                //windowPick( x, y );
                break;
            }
            case eq::Event::CHANNEL_POINTER_BUTTON_RELEASE:
            {
                const unsigned int b = eqButtonToOsg( event.pointer.button );
                if( b <= 3 )
                    eventQueue->mouseButtonRelease( x, y, b, time );
                break;
            }
            case eq::Event::KEY_PRESS:
            {
                const int osgKey = eqKeyToOsg( event.key.key );
                eventQueue->keyPress( osgKey, time );
            }
            case eq::Event::KEY_RELEASE:
            {
                const int osgKey = eqKeyToOsg( event.key.key );
                eventQueue->keyRelease( osgKey, time );
            }
            default:
                break;
        }
    }

    return eq::Channel::processEvent( event );
}

void Channel::updateView( )
{
#if 0
    View* v = static_cast< View* >( getView( ));
    if( v->getID( ) == getFrameData( ).getCurrentViewID( ))
    {
        eq::Vector3d origin, direction;
        v->getWorldPointer( origin, direction );
        if( origin != direction )
        {
            const eq::PixelViewport& pvp = getPixelViewport( );

            const eq::Matrix4d viewMatrix =
                eq::Matrix4d( getPerspectiveTransform( )) *
                    getFrameData( ).getViewMatrix( );

            const eq::Frustumf& frustum = getPerspective( );
            const eq::Matrix4d projectionMatrix = frustum.compute_matrix( );

            double x, y, z;
            LBCHECK( gluProject( origin.x( ), origin.y( ), origin.z( ),
                viewMatrix.array, projectionMatrix.array, &pvp.x,
                &x, &y, &z ));

            if( pvp.isInside( x, y ))
                worldPick( origin, direction );
        }
    }
#endif
}

void Channel::windowPick( uint32_t x, uint32_t y ) const
{
    using namespace osgUtil;

#if 0
    LBWARN << "window pick @ (" << x << ", " << y << ")" << std::endl;
#endif

    osg::ref_ptr< LineSegmentIntersector > picker =
        new LineSegmentIntersector( Intersector::WINDOW, x, y );
    picker->setIntersectionLimit( Intersector::LIMIT_NEAREST );
    IntersectionVisitor iv( picker.get( ));
    _camera->accept( iv );

    LineSegmentIntersector::Intersections intersections;
    if( picker->containsIntersections( ))
    {
        osg::Vec3d hit =
            picker->getFirstIntersection( ).getWorldIntersectPoint( );

#if 0
        LBWARN << "\thit @ (" << hit.x( )
            << ", " << hit.y( )
            << ", " << hit.z( )
            << ") " << std::endl;
#endif

        ConfigEvent event;
        event.data.originator = getID( );
        event.data.type = ConfigEvent::INTERSECTION;
        event.hit = osgToVmml( hit );
        eq::Config* config = const_cast< eq::Config* >( getConfig( ));
        config->sendEvent( event );
    }
}

void Channel::worldPick( const eq::Vector3d& origin,
        const eq::Vector3d& direction ) const
{
    using namespace osgUtil;

    osg::Vec3d start = vmmlToOsg( origin );
    osg::Vec3d end = vmmlToOsg( direction );

#if 0
    LBWARN << std::fixed << "world pick start @ (" << start << ")" << std::endl;
    LBWARN << std::fixed << "world pick   end @ (" << end << ")" << std::endl;
#endif

    osg::ref_ptr< LineSegmentIntersector > picker =
        new LineSegmentIntersector( Intersector::MODEL, start, end );
    picker->setIntersectionLimit( Intersector::LIMIT_NEAREST );
    IntersectionVisitor iv( picker.get( ));
    _camera->accept( iv );

    LineSegmentIntersector::Intersections intersections;
    if( picker->containsIntersections( ))
    {
        osg::Vec3d hit =
            picker->getFirstIntersection( ).getWorldIntersectPoint( );

#if 0
        LBWARN << std::fixed << "\thit @ (" << hit.x( )
            << ", " << hit.y( )
            << ", " << hit.z( )
            << ") in pvp " << getPixelViewport( ) << std::endl;
#endif

        ConfigEvent event;
        event.data.originator = getID( );
        event.data.type = ConfigEvent::INTERSECTION;
        event.hit = osgToVmml( hit );
        eq::Config* config = const_cast< eq::Config* >( getConfig( ));
        config->sendEvent( event );
    }
}

void Channel::cleanup( )
{
    if( _camera.valid( ))
        connectCameraToScene( eq::UUID::ZERO );
    _camera = 0;

    if( _camera2d.valid( ))
        connectCameraToOverlay( eq::UUID::ZERO );
    _viewer2d = 0;
}

void Channel::connectCameraToScene( const eq::uint128_t& id )
{
    LB_TS_THREAD( _pipeThread );

    // TODO : reusing the same camera between osgViews costs expensive
    // remove/add operations, make cameras per eq::View and find the
    // right one to use on every frameDraw.

    if( id != _sceneID )
    {
        Node* node = static_cast< Node* >( getNode( ));

        Window::initCapabilities( _camera->getGraphicsContext( ));

        if( eq::UUID::ZERO != _sceneID )
        {
            node->removeCameraFromOSGView( _sceneID, _camera );

            _camera->setRenderer( 0 );
            _renderer = 0;
        }

        _sceneID = id;

        if( eq::UUID::ZERO != _sceneID )
        {
            node->addCameraToOSGView( _sceneID, _camera );

            _renderer =
                static_cast< osgViewer::Renderer* >( _camera->getRenderer( ));
            LBASSERT( _renderer.valid( ));
            _renderer->setGraphicsThreadDoesCull( false );
        }
    }
}

void Channel::connectCameraToOverlay( const eq::uint128_t& id )
{
    LB_TS_THREAD( _pipeThread );
    LBASSERT( _viewer2d.valid( ));

    if( id != _overlayID )
    {
        if( eq::UUID::ZERO != _overlayID )
        {
            _camera2d = 0;

            _viewer2d->setSceneData( 0 );
        }

        _overlayID = id;

        if( eq::UUID::ZERO != _overlayID )
        {
            const eq::PixelViewport& pvp = getPixelViewport( );
            const eq::Viewport& vp = getViewport( );

            // Set the viewer's master camera viewport to cover the entire
            // canvas so osgEarth controls will position properly.
            osg::ref_ptr< osg::Camera > camera = _viewer2d->getCamera( );
            camera->setViewport( 0, 0, pvp.w / vp.w, pvp.h / vp.h );

            _camera2d =
                osgEarth::Util::Controls::ControlCanvas::get( _viewer2d, true );
            _camera2d->setColorMask( new osg::ColorMask );

            static_cast< Config* >( getConfig( ))->createOverlay( _camera2d,
                    getNativeView( ));
        }
    }
}

void Channel::__applyBuffer( osg::Camera* camera )
{
    if(( getFrameBufferObject( ) == 0 ) &&
        ( getWindow( )->getSystemWindow( )->getFrameBufferObject( ) == 0 ))
    {
        camera->setReadBuffer( getReadBuffer( ));
        camera->setDrawBuffer( getDrawBuffer( ));
    }

    __applyColorMask( camera );
}

void Channel::__applyColorMask( osg::Camera* camera ) const
{
    const eq::ColorMask& colorMask = getDrawBufferMask( );
    camera->getColorMask( )->setMask(
        colorMask.red, colorMask.green, colorMask.blue, true );
}

void Channel::__applyViewport( osg::Camera* camera ) const
{
    const eq::PixelViewport& pvp = getPixelViewport( );
    camera->setViewport( pvp.x, pvp.y, pvp.w, pvp.h );
}

void Channel::__applyFrustum( osg::Camera* camera ) const
{
#if 0
    if( useOrtho( ))
        __applyOrtho( camera );
    else
        __applyPerspective( camera );
#else
    eq::Frustumf frustum = getFrustum( );
    const eq::Vector2f jitter = getJitter( );

    frustum.apply_jitter( jitter );
    const eq::Matrix4f projection = useOrtho( ) ?
        frustum.compute_ortho_matrix( ):
        frustum.compute_matrix( );
    camera->setProjectionMatrix( vmmlToOsg( projection ));
#endif
}

void Channel::__applyPerspective( osg::Camera* camera ) const
{
    eq::Frustumf frustum = getPerspective( );
    const eq::Vector2f jitter = getJitter( );

    frustum.apply_jitter( jitter );
    camera->setProjectionMatrixAsFrustum(
        frustum.left( ), frustum.right( ),
        frustum.bottom( ), frustum.top( ),
        frustum.near_plane( ), frustum.far_plane( ));
}

void Channel::__applyOrtho( osg::Camera* camera ) const
{
    eq::Frustumf ortho = getOrtho( );
    const eq::Vector2f jitter = getJitter( );

    ortho.apply_jitter( jitter );
    camera->setProjectionMatrixAsOrtho(
        ortho.left( ), ortho.right( ),
        ortho.bottom( ), ortho.top( ),
        ortho.near_plane( ), ortho.far_plane( ));
}

void Channel::__applyHeadTransform( osg::Camera* camera ) const
{
#if 0
    if( useOrtho( ))
        __applyOrthoTransform( camera );
    else
        __applyPerspectiveTransform( camera );
#else
    osg::Matrix headView =
        vmmlToOsg( static_cast< const View* >( getView( ))->getViewMatrix( ));
    headView.postMult( vmmlToOsg( getHeadTransform( )));
    camera->setViewMatrix( headView );
#endif
}

void Channel::__applyPerspectiveTransform( osg::Camera* camera ) const
{
    osg::Matrix headView =
        vmmlToOsg( static_cast< const View* >( getView( ))->getViewMatrix( ));
    headView.postMult( vmmlToOsg( getPerspectiveTransform( )));
    camera->setViewMatrix( headView );
}

void Channel::__applyOrthoTransform( osg::Camera* camera ) const
{
    osg::Matrix headView =
        vmmlToOsg( static_cast< const View* >( getView( ))->getViewMatrix( ));
    headView.postMult( vmmlToOsg( getOrthoTransform( )));
    camera->setViewMatrix( headView );
}

void Channel::__applyScreenFrustum( osg::Camera* camera ) const
{
    const eq::Frustumf& screen = getScreenFrustum( );
    camera->setProjectionMatrixAsOrtho(
        screen.left( ), screen.right( ),
        screen.bottom( ), screen.top( ),
        screen.near_plane( ), screen.far_plane( ));
}
}
