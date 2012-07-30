#include "channel.h"

#include "config.h"
#include "configEvent.h"
#include "node.h"
#include "window.h"
#include "view.h"
#include "viewer.h"
#include "util.h"

#include <osgViewer/View>
#include <osgDB/ReadFile>

#include <osgEarthUtil/Controls>
#include <osgEarthSymbology/Color>

#include <GL/glu.h>

namespace eqEarth
{
using namespace osgEarth::Symbology;
using namespace osgEarth::Util::Controls;


void createControls( ControlCanvas* );

static osg::Geode* createHUD()
{
    osg::Geode* geode = new osg::Geode();

    std::string timesFont("/afs/cmf/project/dc/sys/share/OpenSceneGraph-Data/fonts/arial.ttf");

    osg::StateSet* stateset = geode->getOrCreateStateSet();
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

    osg::Vec3 position(5000.0f,400.0f,0.0f);
    osg::Vec3 delta(0.0f,-120.0f,0.0f);

    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont(timesFont);
        text->setPosition(position);
        text->setText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

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
        stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

        geode->addDrawable(geom);
    }

    return geode;
}

// ----------------------------------------------------------------------------

Channel::Channel( eq::Window* parent )
    : eq::Channel( parent )
    , _sceneID( eq::UUID::ZERO )
    , _first( true )
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

        if( isDestination( ))
        {
            _viewer = new osgViewer::Viewer;
            _viewer->setThreadingModel( osgViewer::ViewerBase::SingleThreaded );

            _camera2d = _viewer->getCamera( );
            _camera2d->setColorMask( new osg::ColorMask );
            _camera2d->setViewport( new osg::Viewport );
            _camera2d->setGraphicsContext( gc );
            _camera2d->setComputeNearFarMode(
                osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
            _camera2d->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
            _camera2d->setAllowEventFocus( false );

            _camera2d->setClearMask( GL_DEPTH_BUFFER_BIT );
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

    const View* view = static_cast< const View* >( getView( ));
    LBASSERT( view );

    connectCameraToScene( view->getSceneID( ));

    _applyScene( _camera );

    if( _renderer.valid( ))
        static_cast< const Node* >( getNode( ))->renderLocked( _renderer );

    updateView( );

//LBINFO << "<----- Channel<" << getName( ) << ">::frameDraw("
//    << frameID << ")" << std::endl;
}

void Channel::frameViewStart( const eq::uint128_t& frameID )
{
//LBINFO << "-----> Channel<" << getName( ) << ">::frameViewStart("
//    << frameID << ")" << std::endl;

    eq::Channel::frameViewStart( frameID );

    if( _viewer.valid( ))
    {
        if( _first )
        {
            _applyView( _camera2d );

#if 0
            _viewer->setSceneData( createHUD( ));
#else
            ControlCanvas* cs = ControlCanvas::get( _viewer );

            createControls( cs );

            _viewer->setSceneData( cs );
#endif

            _first = false;
        }

        const FrameData& frameData =
            static_cast< const Node* >( getNode( ))->getFrameData( );

        _viewer->advance( frameData.getSimulationTime( ));

        const time_t calendar = frameData.getCalendarTime( );
        struct tm now;
        if( NULL != gmtime_r( &calendar, &now ))
            _viewer->getViewerFrameStamp( )->setCalendarTime( now );

        _viewer->eventTraversal( );
        _viewer->updateTraversal( );
    }

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
    osg::GL2Extensions::GL2Extensions* gl2e =
        osg::GL2Extensions::Get( contextID, true );

    gl2e->glUseProgram( 0 );
#endif

    if( _viewer.valid( ))
    {
        _applyView( _camera2d );

        _viewer->renderingTraversals( );
    }

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
    if( isDestination( ) && _viewer.valid( ))
    {
        osg::ref_ptr< osgGA::EventQueue > eventQueue =
            _viewer->getEventQueue( );

        const double time =
            static_cast< double >( getConfig( )->getTime( )) / 1000.;

        const eq::PixelViewport& pvp = event.context.pvp;
        const eq::Viewport& vp = event.context.vp;
        const uint32_t x = event.pointer.x + pvp.x + ( vp.x * ( pvp.w / vp.w ));
        const uint32_t y = pvp.h - event.pointer.y + pvp.y;

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
LBWARN << "CHANNEL_POINTER_MOTION(" << x << ", " << y << ")" << std::endl;
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

    _camera2d = 0;
    _viewer = 0;
}

void Channel::connectCameraToScene( const eq::uint128_t& id )
{
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

void Channel::_applyScene( osg::Camera* camera )
{
    EQ_TS_THREAD( _pipeThread );

    _applyBuffer( camera );
    _applyViewport( camera );
    _applyPerspective( camera );
    _applyPerspectiveTransform( camera );
}

void Channel::_applyView( osg::Camera* camera )
{
    EQ_TS_THREAD( _pipeThread );

    _applyBuffer( camera );
    _applyViewport( camera );
    _applyScreen( camera );
    _applyScreenTransform( camera );
}

void Channel::_applyBuffer( osg::Camera* camera )
{
    const Window* window = static_cast< const Window* >( getWindow( ));
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

void Channel::_applyViewport( osg::Camera* camera )
{
    const eq::PixelViewport& pvp = getPixelViewport( );
    camera->setViewport( pvp.x, pvp.y, pvp.w, pvp.h );
}

void Channel::_applyViewport2d( osg::Camera* camera )
{
    const eq::PixelViewport& pvp = getPixelViewport( );
    const eq::Viewport& vp = getViewport( );

    camera->setViewport( -( pvp.w / vp.w ) * vp.x, -( pvp.h / vp.h ) * vp.y,
        pvp.w / vp.w, pvp.h / vp.h );

#if 0
    const osg::Viewport* v2 = camera->getViewport( );
    LBWARN << "vp = " << vp << ", pvp = " << pvp << std::endl;
    LBWARN << "v2 = " << v2->x() << ", " << v2->y() << ", " << v2->width() <<
           ", " << v2->height() << std::endl;
#endif
}

void Channel::_applyPerspective( osg::Camera* camera )
{
    const View* view = static_cast< const View* >( getView( ));
    double near, far;
    view->getNearFar( near, far );
/*
    double r = far - near;

    const eq::Range &range = getRange( );
    near += r * range.start;
    far = near + ( r * range.end );
*/
    setNearFar( near, far );

    const eq::Frustumf& frustum = getPerspective( );
    camera->setProjectionMatrixAsFrustum(
        frustum.left( ), frustum.right( ),
        frustum.bottom( ), frustum.top( ),
        frustum.near_plane( ), frustum.far_plane( ));
}

void Channel::_applyPerspectiveTransform( osg::Camera* camera )
{
    const eq::Matrix4d& viewMatrix =
        static_cast< const View* >( getView( ))->getViewMatrix( );
    const eq::Matrix4d& headTransform = getPerspectiveTransform( );
    camera->setViewMatrix( vmmlToOsg( headTransform * viewMatrix ));
}

void Channel::_applyScreen( osg::Camera* camera )
{
    const eq::Frustumf& screen = getScreenFrustum( );
    //LBWARN << "screen = " << screen << std::endl;
    camera->setProjectionMatrixAsOrtho2D(
        screen.left( ), screen.right( ),
        screen.bottom( ), screen.top( ));
}

void Channel::_applyScreenTransform( osg::Camera* camera )
{
    camera->setViewMatrix( vmmlToOsg( getOrthoTransform( )));
}




struct MyClickHandler : public ControlEventHandler
{
    void onClick( Control* control, const osg::Vec2f& pos, int mouseButtonMask )
    {
        OE_NOTICE << "You clicked at (" << pos.x() << ", " << pos.y() << ") within the control."
            << std::endl;
    }
};

static LabelControl* s_sliderLabel;

struct MySliderHandler : public ControlEventHandler
{
    void onValueChanged( Control* control, float value )
    {
        std::stringstream buf;
        buf << (int)value;
        std::string str;
        str = buf.str();
        s_sliderLabel->setText( str );
    }
};

#if 0
struct RotateImage : public ControlEventHandler
{
    void onValueChanged( Control* control, float value )
    {
        s_imageControl->setRotation( Angular(value) );
    }
};
#endif

void
createControls( ControlCanvas* cs )
{
    static lunchbox::Lock lock;
    lunchbox::ScopedWrite _mutex( lock );

    // a container centered on the screen, containing an image and a text label.
    {
        VBox* center = new VBox();
        center->setFrame( new RoundedFrame() );
        center->getFrame()->setBackColor( 1,1,1,0.5 );
        center->setPadding( 10 );
        center->setHorizAlign( Control::ALIGN_CENTER );
        center->setVertAlign( Control::ALIGN_CENTER );

        // Add an image:
        osg::ref_ptr<osg::Image> image = osgDB::readImageFile("http://demo.pelicanmapping.com/rmweb/readymap_logo.png");
        if ( image.valid() )
        {
            ImageControl*imageControl = new ImageControl( image.get() );
            imageControl->setHorizAlign( Control::ALIGN_CENTER );
            imageControl->setFixSizeForRotation( true );
            //imageCon->addEventHandler( new ImageRotationHandler );
            center->addControl( imageControl );
            center->setHorizAlign( Control::ALIGN_CENTER );
        }

        // Add a text label:
        LabelControl* label = new LabelControl( "osgEarth Controls Toolkit" );
        label->setFont( osgText::readFontFile(
                    "/afs/cmf/project/dc/sys/share/OpenSceneGraph-Data/fonts/arialbd.ttf" ) );
        label->setFontSize( 24.0f );
        label->setHorizAlign( Control::ALIGN_CENTER );
        label->setMargin( 5 );
        center->addControl( label );

        // Rotation slider
        HBox* rotateBox = new HBox();
        rotateBox->setChildVertAlign( Control::ALIGN_CENTER );
        rotateBox->setHorizFill( true );
        rotateBox->setBackColor( Color::Blue );
        {
            rotateBox->addControl( new LabelControl("Rotate: ") );

            HSliderControl* rotateSlider = new HSliderControl( -180.0, 180.0, 0.0 );
            //rotateSlider->addEventHandler( new RotateImage() );
            rotateSlider->setHeight( 8.0f );
            rotateSlider->setHorizFill( true );
            rotateBox->addControl( rotateSlider );
        }
        center->addControl( rotateBox );

        cs->addControl( center );
    }

    // a simple vbox with absolute positioning in the upper left with two text labels.
    {
        VBox* ul = new VBox();
        ul->setPosition( 20, 20 );
        ul->setPadding( 10 );
        {
            LabelControl* title = new LabelControl( "Upper left control", 22, osg::Vec4f(1,1,0,1) );
            ul->addControl( title );

            LabelControl* content = new LabelControl( "Here is some text in the upper left control" );
            ul->addControl( content );

            HBox* c2 = new HBox();
            c2->setChildSpacing( 10 );
            {
                HSliderControl* slider = new HSliderControl( 0, 100 );
                slider->setBackColor( .6,0,0,1 );
                slider->setHeight( 25 );
                slider->setWidth( 300 );
                slider->addEventHandler( new MySliderHandler() );
                c2->addControl( slider );

                s_sliderLabel = new LabelControl();
                s_sliderLabel->setVertAlign( Control::ALIGN_CENTER );
                c2->addControl( s_sliderLabel );        
            }
            ul->addControl( c2 );

            HBox* c3 = new HBox();
            c3->setHorizAlign( Control::ALIGN_CENTER );
            c3->setChildSpacing( 10 );
            {
                HBox* c4 = new HBox();
                c4->setChildSpacing( 5 );
                {
                    c4->addControl( new CheckBoxControl( true ) );
                    c4->addControl( new LabelControl( "Checkbox 1" ) );
                }
                c3->addControl( c4 );

                HBox* c5 = new HBox();
                c5->setChildSpacing( 5 );
                {
                    c5->addControl( new CheckBoxControl( false ) );
                    c5->addControl( new LabelControl( "Checkbox 2" ) );
                }
                c3->addControl( c5 );
            }
            ul->addControl( c3 );
        }
        cs->addControl( ul );

        ul->addEventHandler( new MyClickHandler );
    }

    // a centered hbox container along the bottom on the screen.
    {
        HBox* bottom = new HBox();
        bottom->setFrame( new RoundedFrame() );
        bottom->getFrame()->setBackColor(0,0,0,0.5);
        bottom->setMargin( 10 );
        bottom->setChildSpacing( 145 );
        bottom->setVertAlign( Control::ALIGN_BOTTOM );
        bottom->setHorizAlign( Control::ALIGN_CENTER );

        for( int i=0; i<4; ++i )
        {
            LabelControl* label = new LabelControl();
            std::stringstream buf;
            buf << "Label_" << i;
            std::string str;
            str = buf.str();
            label->setText( str );
            label->setMargin( 10 );
            label->setBackColor( 1,1,1,0.4 );
            bottom->addControl( label );

            label->setActiveColor(1,.3,.3,1);
            label->addEventHandler( new MyClickHandler );
        }

        cs->addControl( bottom );
    }
}
}
