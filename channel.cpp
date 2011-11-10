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
co::base::Lock Channel::_culllock;

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

bool Channel::connectToView( const eq::uint128_t& id )
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

    if( eq::Channel::configInit( initID ))
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

        init = true;
    }

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
    _newScene = false;

    eq::Channel::frameStart( frameID, frameNumber );
}

void Channel::frameViewStart( const eq::uint128_t& frameID )
{
    const View *view = static_cast< const View* >( getView( ));
    EQASSERT( view );

    double near, far;
    view->getNearFar( near, far );
    setNearFar( near, far );

    eq::Channel::frameViewStart( frameID );
}

void Channel::frameClear( const eq::uint128_t& frameID )
{
//EQINFO << "-----> Channel::frameClear(" << getName( ) << ":" << frameID << ")" << std::endl;

    EQ_GL_CALL( glEnable( GL_SCISSOR_TEST ));

    eq::Channel::frameClear( frameID );

//EQINFO << "<----- Channel::frameClear(" << getName( ) << ":" << frameID << ")" << std::endl;
}

void Channel::frameDraw( const eq::uint128_t& frameID )
{
//EQINFO << "-----> Channel::frameDraw(" << getName( ) << ":" << frameID << ")" << std::endl;

    //eq::Channel::frameDraw( frameID );

    const View *view = static_cast< const View* >( getView( ));
    EQASSERT( view );

    if( view->getSceneID( ) != _sceneID )
        _newScene = connectToView( view->getSceneID( ));

    double near, far;
    view->getNearFar( near, far );
    setNearFar( near, far );
    
    applyScene( );

    if( !_newScene && _renderer.valid( )) 
    {
        co::base::ScopedMutex<> mutex( _culllock ); // DAMMIT!
        _renderer->cull( );
        EQ_GL_CALL( _renderer->draw( ));

/*
        EQ_GL_CALL( _renderer->cull_draw( ));
*/
    }

    updateView( );

//EQINFO << "<----- Channel::frameDraw(" << frameID << ")" << std::endl;
}

void Channel::frameViewFinish( const eq::uint128_t& frameID )
{
//EQINFO << "-----> Channel::frameViewFinish(" << frameID << ")" << std::endl;

    const FrameData& frameData =
        static_cast< const Node* >( getNode( ))->getFrameData( );
    if( frameData.useStatistics( ))
        drawStatistics( );

    eq::Channel::frameViewFinish( frameID );

//EQINFO << "<----- Channel::frameViewFinish(" << frameID << ")" << std::endl;
}

void Channel::frameDrawFinish( const eq::uint128_t& frameID,
    const uint32_t frameNumber )
{
//EQINFO << "-----> Channel::frameDrawFinish(" << getName( ) << ":" << frameID << ")" << std::endl;

    eq::Channel::frameDrawFinish( frameID, frameNumber );

//EQINFO << "<----- Channel::frameDrawFinish(" << frameID << ")" << std::endl;
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

void Channel::applyScene( )
{
    EQ_TS_THREAD( _pipeThread );

    applyBuffer( _camera );
    applyViewport( _camera );
    applyPerspective( _camera );

    applyPerspectiveTransform( _camera,
        static_cast< View* >( getView( ))->getViewMatrix( ));
}

void Channel::applyBuffer( osg::Camera* camera ) const
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

void Channel::applyViewport( osg::Camera* camera ) const
{
    const eq::PixelViewport& pvp = getPixelViewport( );
    camera->setViewport( pvp.x, pvp.y, pvp.w, pvp.h );
}

void Channel::applyPerspective( osg::Camera* camera ) const
{
    const eq::Frustumf& frustum = getPerspective( );
    camera->setProjectionMatrixAsFrustum( 
        frustum.left( ), frustum.right( ),
        frustum.bottom( ), frustum.top( ),
        frustum.near_plane( ), frustum.far_plane( ));
}

void Channel::applyPerspectiveTransform( osg::Camera* camera,
        const eq::Matrix4d& viewMatrix ) const
{
    camera->setViewMatrix( vmmlToOsg( 
        eq::Matrix4d( getPerspectiveTransform( )) * viewMatrix ));
}

void Channel::applyScreen( osg::Camera* camera ) const
{
    const eq::Frustumf& screen = getScreenFrustum( );
    camera->setProjectionMatrixAsOrtho(
        screen.left( ), screen.right( ),
        screen.bottom( ), screen.top( ),
        screen.near_plane( ), screen.far_plane( ));
}

void Channel::applyScreenTransform( osg::Camera* camera,
    const eq::Matrix4d& viewMatrix ) const
{
    camera->setViewMatrix( vmmlToOsg( viewMatrix ));
}

const FrameData& Channel::getFrameData( ) const
{
    return static_cast< const Node* >( getNode( ))->getFrameData( );
}

void Channel::updateView( )
{
    View *v = static_cast< View* >( getView( ));
    if( v->getID( ) == getFrameData( ).getCurrentViewID( ))
    {
#if 0
        if( getID( ) == getFrameData( ).getMasterChannelID( ))
        {
            osg::ref_ptr< osgViewer::View > view =
               static_cast< osgViewer::View* >( _camera->getView( ));
            EQASSERT( view.valid( ));

            v->setViewMatrix( osgToVmml(
                view->getCameraManipulator( )->getInverseMatrix( )));

            const eq::Frustumf& frustum = getPerspective( );
            v->setNearFar( frustum.near_plane( ), frustum.far_plane( ));
        }
#endif

#if 0
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
#endif
    }
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
        EQCHECK( !connectToView( eq::UUID::ZERO ));
    _camera = 0;
}
}
