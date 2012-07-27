#include "channel.h"

#include "config.h"
#include "configEvent.h"
#include "node.h"
#include "window.h"
#include "view.h"
#include "util.h"

#include <osgViewer/View>
#include <osgEarth/MapNode>

#include <GL/glu.h>

namespace eqEarth
{
// ----------------------------------------------------------------------------

Channel::Channel( eq::Window* parent )
    : eq::Channel( parent )
    , _sceneID( eq::UUID::ZERO )
    , _newScene( false )
{
EQINFO << "=====> Channel::Channel(" << (void *)this << ")" << std::endl;
}

Channel::~Channel( )
{
EQINFO << "<===== Channel::~Channel(" << (void *)this << ")" << std::endl;
}

const FrameData& Channel::getFrameData( ) const
{
    return static_cast< const Node* >( getNode( ))->getFrameData( );
}

bool Channel::connectCameraToView( const eq::uint128_t& id )
{
    bool isNew = false;

    if( id != _sceneID )
    {
        Node *node = static_cast< Node* >( getNode( ));

        Window::initCapabilities( _camera->getGraphicsContext( ));

        if( eq::UUID::ZERO != _sceneID )
        {
            EQCHECK( node->removeCameraFromView( _sceneID, _camera ));
            _camera->setRenderer( 0 );

            _renderer = 0;
        }

        _sceneID = id;

        if( eq::UUID::ZERO != _sceneID )
        {
            EQCHECK( node->addCameraToView( _sceneID, _camera ));
            isNew = true;

            _renderer =
                static_cast< osgViewer::Renderer* >( _camera->getRenderer( ));
            _renderer->setGraphicsThreadDoesCull( false );
        }
    }

    return isNew;
}

bool Channel::configInit( const eq::uint128_t& initID )
{
EQINFO << "-----> Channel::configInit(" << initID <<
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
        _camera->setComputeNearFarMode(
            osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
        _camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
        _camera->setAllowEventFocus( false );
    }

    init = true;

out:
    if( !init )
        cleanup( );

EQINFO << "<----- Channel::configInit(" << initID << ")" << std::endl;

    return init;
}

bool Channel::configExit( )
{
    cleanup( );

    return eq::Channel::configExit( );
}

void Channel::frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//EQINFO << "-----> Channel<" << getName( ) << ">::frameStart(" <<
//    << frameID << ", " << frameNumber << ")" << std::endl;

    _newScene = false;

    eq::Channel::frameStart( frameID, frameNumber );

//EQINFO << "<----- Channel<" << getName( ) << ">::frameStart(" <<
//    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Channel::frameViewStart( const eq::uint128_t& frameID )
{
//EQINFO << "-----> Channel<" << getName( ) << ">::frameViewStart(" <<
//    << frameID << ")" << std::endl;

    const View *view = static_cast< const View* >( getView( ));
    EQASSERT( view );

    double near, far;
    view->getNearFar( near, far );
    setNearFar( near, far );

    eq::Channel::frameViewStart( frameID );

//EQINFO << "<----- Channel<" << getName( ) << ">::frameViewStart(" <<
//    << frameID << ")" << std::endl;
}

void Channel::frameClear( const eq::uint128_t& frameID )
{
//EQINFO << "-----> Channel<" << getName( ) << ">::frameClear(" <<
//    << frameID << ")" << std::endl;

    EQ_GL_CALL( glEnable( GL_SCISSOR_TEST ));

    eq::Channel::frameClear( frameID );

//EQINFO << "<----- Channel<" << getName( ) << ">::frameClear(" <<
//    << frameID << ")" << std::endl;
}

void Channel::frameDraw( const eq::uint128_t& frameID )
{
//EQINFO << "-----> Channel<" << getName( ) << ">::frameDraw(" <<
//    << frameID << ")" << std::endl;

    //eq::Channel::frameDraw( frameID );

    const View *view = static_cast< const View* >( getView( ));
    EQASSERT( view );

    if( view->getSceneID( ) != _sceneID )
        _newScene = connectCameraToView( view->getSceneID( ));

    double near, far;
    view->getNearFar( near, far );
/*
    double r = far - near;

    const eq::Range &range = getRange( );
    near += r * range.start;
    far = near + ( r * range.end );
*/

    setNearFar( near, far );

    _applyScene( );

    if( !_newScene && _renderer.valid( ))
    {
        const Node *node = static_cast< const Node* >( getNode( ));
        node->renderLocked( _renderer );
    }

    updateView( );

//EQINFO << "<----- Channel<" << getName( ) << ">::frameDraw(" <<
//    << frameID << ")" << std::endl;
}

void Channel::frameViewFinish( const eq::uint128_t& frameID )
{
//EQINFO << "-----> Channel<" << getName( ) << ">::frameViewFinish(" <<
//    << frameID << ")" << std::endl;

#if 0
    unsigned int contextID =
        _camera->getGraphicsContext( )->getState( )->getContextID( );
    osg::GL2Extensions::GL2Extensions *gl2e =
        osg::GL2Extensions::Get( contextID, true );

    gl2e->glUseProgram( 0 );
#endif

    const FrameData& frameData =
        static_cast< const Node* >( getNode( ))->getFrameData( );

    _applyView( );

    if( frameData.useStatistics( ))
        drawStatistics( );

    eq::Channel::frameViewFinish( frameID );

//EQINFO << "<----- Channel<" << getName( ) << ">::frameViewFinish(" <<
//    << frameID << ")" << std::endl;
}

void Channel::frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//EQINFO << "-----> Channel<" << getName( ) << ">::frameDrawFinish(" <<
//    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Channel::frameDrawFinish( frameID, frameNumber );

//EQINFO << "<----- Channel<" << getName( ) << ">::frameDrawFinish(" <<
//    << frameID << ", " << frameNumber << ")" << std::endl;
}

bool Channel::processEvent( const eq::Event& event )
{
#if 0
    switch( event.type )
    {
        case eq::Event::CHANNEL_POINTER_MOTION:
        case eq::Event::CHANNEL_POINTER_BUTTON_PRESS:
        case eq::Event::CHANNEL_POINTER_BUTTON_RELEASE:
            const eq::PixelViewport& pvp = event.context.pvp;

            uint32_t x = event.pointer.x + pvp.x;
            uint32_t y = pvp.h - event.pointer.y + pvp.y;

EQWARN << "xy: " << x << ", " << y << " in pvp " << pvp << std::endl;

            windowPick( x, y );
            break;
    }
#endif

    return eq::Channel::processEvent( event );
}

void Channel::updateView( )
{
#if 0
    View *v = static_cast< View* >( getView( ));
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
            EQCHECK( gluProject( origin.x( ), origin.y( ), origin.z( ),
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
    EQWARN << "window pick @ (" << x << ", " << y << ")" << std::endl;
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
        EQWARN << "\thit @ (" << hit.x( )
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
    EQWARN << std::fixed << "world pick start @ (" << start << ")" << std::endl;
    EQWARN << std::fixed << "world pick   end @ (" << end << ")" << std::endl;
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
        EQWARN << std::fixed << "\thit @ (" << hit.x( )
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
        EQCHECK( !connectCameraToView( eq::UUID::ZERO ));
    _camera = 0;
}

void Channel::_applyBuffer( osg::Camera* camera ) const
{
    const Window *window = static_cast< const Window* >( getWindow( ));
    if(( const_cast< Channel* >( this )->getFrameBufferObject( ) == 0 ) &&
        ( window->getSystemWindow( )->getFrameBufferObject( ) == 0 ))
    {
        camera->setDrawBuffer( getDrawBuffer( ));
        camera->setReadBuffer( getReadBuffer( ));
    }
    const eq::ColorMask& colorMask = getDrawBufferMask( );
    camera->getColorMask( )->setMask(
        colorMask.red, colorMask.green, colorMask.blue, true );
}

void Channel::_applyViewport( osg::Camera* camera ) const
{
    const eq::PixelViewport& pvp = getPixelViewport( );
    camera->setViewport( pvp.x, pvp.y, pvp.w, pvp.h );
}

void Channel::_applyScene( ) const
{
    EQ_TS_THREAD( _pipeThread );

    _applyBuffer( _camera );
    _applyViewport( _camera );
    _applyPerspective( _camera );

    _applyPerspectiveTransform( _camera,
        static_cast< const View* >( getView( ))->getViewMatrix( ));
}

void Channel::_applyPerspective( osg::Camera* camera ) const
{
    const eq::Frustumf& frustum = getPerspective( );
    camera->setProjectionMatrixAsFrustum(
        frustum.left( ), frustum.right( ),
        frustum.bottom( ), frustum.top( ),
        frustum.near_plane( ), frustum.far_plane( ));
}

void Channel::_applyPerspectiveTransform( osg::Camera* camera,
        const eq::Matrix4d& viewMatrix ) const
{
    camera->setViewMatrix( vmmlToOsg(
        eq::Matrix4d( getPerspectiveTransform( )) * viewMatrix ));
}

void Channel::_applyView( ) const
{
    EQ_TS_THREAD( _pipeThread );

    _applyBuffer( _camera );
    _applyViewport( _camera );
    _applyScreen( _camera );

    _applyScreenTransform( _camera,
        static_cast< const View* >( getView( ))->getViewMatrix( ));
}

void Channel::_applyScreen( osg::Camera* camera ) const
{
    const eq::Frustumf& screen = getScreenFrustum( );
    camera->setProjectionMatrixAsOrtho(
        screen.left( ), screen.right( ),
        screen.bottom( ), screen.top( ),
        screen.near_plane( ), screen.far_plane( ));
}

void Channel::_applyScreenTransform( osg::Camera* camera,
    const eq::Matrix4d& viewMatrix ) const
{
    camera->setViewMatrix( vmmlToOsg(
        eq::Matrix4d( getOrthoTransform( )) * viewMatrix ));
}
}
