#include "channel.h"

#include "config.h"
#include "configEvent.h"
#include "node.h"
#include "window.h"
#include "view.h"
#include "util.h"

#include <osgViewer/View>
#include <osg/CullSettings>
#include <osgEarth/MapNode>

#include <GL/glu.h>

namespace eqEarth
{
#if 0
static osg::Geode* hud = NULL;
static lunchbox::Lock _hud_lock;

static osg::Geode* createHUD()
{
    lunchbox::ScopedWrite _mutex( _hud_lock );
    if( hud )
        return hud;

        osg::Geode* geode = new osg::Geode();

        std::string timesFont("fonts/arial.ttf");

        // turn lighting off for the text and disable depth test to ensure it's always ontop.
        osg::StateSet* stateset = geode->getOrCreateStateSet();
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

        osg::Vec3 position(1650.0f,800.0f,0.0f);
        osg::Vec3 delta(0.0f,-120.0f,0.0f);

        {
            osgText::Text* text = new  osgText::Text;
            geode->addDrawable( text );

            text->setFont(timesFont);
            text->setPosition(position);
            text->setText("Head Up Displays are simple :-)");

            position += delta;
        }    


        {
            osgText::Text* text = new  osgText::Text;
            geode->addDrawable( text );

            text->setFont(timesFont);
            text->setPosition(position);
            text->setText("All you need to do is create your text in a subgraph.");

            position += delta;
        }    


        {
            osgText::Text* text = new  osgText::Text;
            geode->addDrawable( text );

            text->setFont(timesFont);
            text->setPosition(position);
            text->setText("Then place an osg::Camera above the subgraph\n"
                          "to create an orthographic projection.\n");

            position += delta;
        } 

        {
            osgText::Text* text = new  osgText::Text;
            geode->addDrawable( text );

            text->setFont(timesFont);
            text->setPosition(position);
            text->setText("Set the Camera's ReferenceFrame to ABSOLUTE_RF to ensure\n"
                          "it remains independent from any external model view matrices.");

            position += delta;
        } 

        {
            osgText::Text* text = new  osgText::Text;
            geode->addDrawable( text );

            text->setFont(timesFont);
            text->setPosition(position);
            text->setText("And set the Camera's clear mask to just clear the depth buffer.");

            position += delta;
        }    

        {
            osgText::Text* text = new  osgText::Text;
            geode->addDrawable( text );

            text->setFont(timesFont);
            text->setPosition(position);
            text->setText("And finally set the Camera's RenderOrder to POST_RENDER\n"
                          "to make sure it's drawn last.");

            position += delta;
        }    


        {
            osg::BoundingBox bb;
            for(unsigned int i=0;i<geode->getNumDrawables();++i)
            {
                bb.expandBy(geode->getDrawable(i)->getBound());
            }

            osg::Geometry* geom = new osg::Geometry;

            osg::Vec3Array* vertices = new osg::Vec3Array;
            float depth = bb.zMin()-0.1;
            vertices->push_back(osg::Vec3(bb.xMin(),bb.yMax(),depth));
            vertices->push_back(osg::Vec3(bb.xMin(),bb.yMin(),depth));
            vertices->push_back(osg::Vec3(bb.xMax(),bb.yMin(),depth));
            vertices->push_back(osg::Vec3(bb.xMax(),bb.yMax(),depth));
            geom->setVertexArray(vertices);

            osg::Vec3Array* normals = new osg::Vec3Array;
            normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
            geom->setNormalArray(normals);
            geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

            osg::Vec4Array* colors = new osg::Vec4Array;
            colors->push_back(osg::Vec4(1.0f,1.0,0.8f,0.2f));
            geom->setColorArray(colors);
            geom->setColorBinding(osg::Geometry::BIND_OVERALL);

            geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0,4));

            osg::StateSet* stateset = geom->getOrCreateStateSet();
            stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
            //stateset->setAttribute(new osg::PolygonOffset(1.0f,1.0f),osg::StateAttribute::ON);
            stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

            geode->addDrawable(geom);
        }

    return hud = geode;
}
#endif

// ----------------------------------------------------------------------------

Channel::Channel( eq::Window* parent )
    : eq::Channel( parent )
    , _sceneID( eq::UUID::ZERO )
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
        _camera->setComputeNearFarMode(
            osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
        _camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
        _camera->setAllowEventFocus( false );

#if 0
        _camera2d = new osg::Camera;
        _camera2d->setColorMask( new osg::ColorMask );
        _camera2d->setViewport( new osg::Viewport );
        _camera2d->setGraphicsContext( gc );
        _camera2d->setComputeNearFarMode(
            osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
        _camera2d->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
        _camera2d->setAllowEventFocus( false );

        _camera2d->setClearMask( GL_DEPTH_BUFFER_BIT );

        _camera2d->addChild( createHUD( ));
#endif
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
//LBINFO << "-----> Channel<" << getName( ) << ">::frameClear("
//    << frameID << ")" << std::endl;

    EQ_GL_CALL( glEnable( GL_SCISSOR_TEST ));

    eq::Channel::frameClear( frameID );

//LBINFO << "<----- Channel<" << getName( ) << ">::frameClear("
//    << frameID << ")" << std::endl;
}

void Channel::frameDraw( const eq::uint128_t& frameID )
{
//LBINFO << "-----> Channel<" << getName( ) << ">::frameDraw("
//    << frameID << ")" << std::endl;

    //eq::Channel::frameDraw( frameID );

    const View *view = static_cast< const View* >( getView( ));
    LBASSERT( view );

    if( view->getSceneID( ) != _sceneID )
        connectCameraToScene( view->getSceneID( ));

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

#if 0
    _applyView( );
#endif

    if( _renderer.valid( ))
    {
        const Node *node = static_cast< const Node* >( getNode( ));

        node->renderLocked( _renderer );
    }

    updateView( );

//LBINFO << "<----- Channel<" << getName( ) << ">::frameDraw("
//    << frameID << ")" << std::endl;
}

void Channel::frameViewStart( const eq::uint128_t& frameID )
{
//LBINFO << "-----> Channel<" << getName( ) << ">::frameViewStart("
//    << frameID << ")" << std::endl;

    eq::Channel::frameViewStart( frameID );

//LBINFO << "<----- Channel<" << getName( ) << ">::frameViewStart("
//    << frameID << ")" << std::endl;
}

void Channel::frameViewFinish( const eq::uint128_t& frameID )
{
//LBINFO << "-----> Channel<" << getName( ) << ">::frameViewFinish("
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

    if( frameData.useStatistics( ))
        drawStatistics( );

    eq::Channel::frameViewFinish( frameID );

//LBINFO << "<----- Channel<" << getName( ) << ">::frameViewFinish("
//    << frameID << ")" << std::endl;
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

LBWARN << "xy: " << x << ", " << y << " in pvp " << pvp << std::endl;

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
}

void Channel::connectCameraToScene( const eq::uint128_t& id )
{
    if( id != _sceneID )
    {
        Node *node = static_cast< Node* >( getNode( ));

        Window::initCapabilities( _camera->getGraphicsContext( ));

        if( eq::UUID::ZERO != _sceneID )
        {
            node->removeChannelFromOSGView( _sceneID, this );

            _camera->setRenderer( 0 );
            _renderer = 0;
        }

        _sceneID = id;

        if( eq::UUID::ZERO != _sceneID )
        {
            node->addChannelToOSGView( _sceneID, this );

            _renderer =
                static_cast< osgViewer::Renderer* >( _camera->getRenderer( ));
            LBASSERT( _renderer.valid( ));
            _renderer->setGraphicsThreadDoesCull( false );
        }
    }
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

#if 0
void Channel::_applyView( ) const
{
    EQ_TS_THREAD( _pipeThread );

    _applyBuffer( _camera2d );
    _applyViewport( _camera2d );
    _applyScreen( _camera2d );

    _applyScreenTransform( _camera2d );
}

void Channel::_applyScreen( osg::Camera* camera ) const
{
    const eq::Frustumf& screen = getScreenFrustum( );
    camera->setProjectionMatrixAsOrtho(
        screen.left( ), screen.right( ),
        screen.bottom( ), screen.top( ),
        screen.near_plane( ), screen.far_plane( ));
}

void Channel::_applyScreenTransform( osg::Camera* camera ) const
{
    camera->setViewMatrix( vmmlToOsg( eq::Matrix4d( getOrthoTransform( ))));
}
#endif
}
