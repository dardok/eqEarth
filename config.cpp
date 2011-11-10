#include "config.h"

#include "error.h"
#include "util.h"
#include "vps.h"
#include "callbacks.h"
#include "window.h" // for Window::initCapabilities
#include "configEvent.h"

#include <osgGA/TrackballManipulator>
#include "earthManipulator.h"

#include <osg/DeleteHandler>
#include <osg/ImageStream>
#include <osg/TextureRectangle>
#include <osgDB/ReadFile>

#include <osgEarth/FindNode>

#include <GL/glu.h>

#include <osgEarthUtil/ObjectPlacer>

#define NFR_AT_RADIUS 0.00001
#define NFR_AT_DOUBLE_RADIUS 0.0049


osg::Geometry* myCreateTexturedQuadGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image, bool useTextureRectangle, bool xyPlane, bool option_flip)
{
    bool flip = image->getOrigin()==osg::Image::TOP_LEFT;
    if (option_flip) flip = !flip;

    if (useTextureRectangle)
    {
        osg::Geometry* pictureQuad = osg::createTexturedQuadGeometry(pos,
                                           osg::Vec3(width,0.0f,0.0f),
                                           xyPlane ? osg::Vec3(0.0f,height,0.0f) : osg::Vec3(0.0f,0.0f,height),
                                           0.0f, flip ? image->t() : 0.0, image->s(), flip ? 0.0 : image->t());

        osg::TextureRectangle* texture = new osg::TextureRectangle(image);
        texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
        texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);


        pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                                                                        texture,
                                                                        osg::StateAttribute::ON);

        return pictureQuad;
    }
    else
    {
        osg::Geometry* pictureQuad = osg::createTexturedQuadGeometry(pos,
                                           osg::Vec3(width,0.0f,0.0f),
                                           xyPlane ? osg::Vec3(0.0f,height,0.0f) : osg::Vec3(0.0f,0.0f,height),
                                           0.0f, flip ? 1.0f : 0.0f , 1.0f, flip ? 0.0f : 1.0f);

        osg::Texture2D* texture = new osg::Texture2D(image);
        texture->setResizeNonPowerOfTwoHint(false);
        texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
        texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
        texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);


        pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                    texture,
                    osg::StateAttribute::ON);

        return pictureQuad;
    }
}


namespace eqEarth
{
// ----------------------------------------------------------------------------

struct NullGraphicsContext : public osgViewer::GraphicsWindow
{
NullGraphicsContext( )
{
    _traits = new GraphicsContext::Traits;
    _traits->x = 0;
    _traits->y = 0;
    _traits->width = 0;
    _traits->height = 0;
}

void setPVP( uint32_t width, uint32_t height )
{
     _traits->width = width;
     _traits->height = height;
}

void clearCameras() { _cameras.clear( ); }
};

static osg::ref_ptr< NullGraphicsContext > ngc =
    new NullGraphicsContext;

// ----------------------------------------------------------------------------

struct ViewCollector : public eq::ConfigVisitor
{
ViewCollector( Config *config )
    : _config( config )
    , _lastViewID( eq::UUID::ZERO )
{
}

void configureAppView( osgViewer::View *view )
{
#if 0
    osgGA::CameraManipulator *m = new osgGA::TrackballManipulator;
/*
    m->setHomePosition( 
        osg::Vec3d( 0, 0, 10 ),
        osg::Vec3d( 0, 0, 0 ),
        osg::Vec3d( 0, 1, 0 ), false );
*/
#else
    osgGA::CameraManipulator *m = new EarthManipulator;
/*
    m->setHomePosition( 
        osg::Vec3d( 0, 0, 6000000 ),
        osg::Vec3d( 0, 0, 0 ),
        osg::Vec3d( 0, 1, 0 ), false );
*/

    osgEarth::MapNode* map = osgEarth::MapNode::findMapNode( view->getSceneData( ));
    if( map )
        m->setNode( map->getTerrainEngine( ));
#endif

    view->setCameraManipulator( m );

    view->getCamera( )->setComputeNearFarMode(
        osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );

    _config->releaseView( view );
}

virtual eq::VisitorResult visit( eq::View* view )
{
    View* v = static_cast< View* >( view );

    v->setSceneID( 1 );
    v->setView( _config->takeOrCreateView( 1 ));

    configureAppView( v->getView( ));

    _lastViewID = v->getID( );

    return eq::TRAVERSE_CONTINUE;
}

Config* _config;
eq::uint128_t _lastViewID;
};

// ----------------------------------------------------------------------------

Config::Config( eq::ServerPtr parent )
    : eq::Config( parent )
    , _appRenderTick( 0U )
{
EQINFO << "=====> Config::Config(" << (void *)this << ")" << std::endl;

    //srandom( time( NULL ));
    srandom( 1 );

    osg::Referenced::setThreadSafeReferenceCounting( true );

    //osg::Referenced::setDeleteHandler( new osg::DeleteHandler( 2 ));

    // initialize the global timer to be relative to the current time.
    osg::Timer::instance( )->setStartTick( );

    osg::ref_ptr< osg::DisplaySettings > ds =  
        osg::DisplaySettings::instance( );
    ds->setMaxTexturePoolSize( 100000000 );     // size in bytes (~100Mb) 
    ds->setMaxBufferObjectPoolSize( 200000000 );// size in bytes (~200Mb) 
    ds->setSerializeDrawDispatch( false  );
    ds->setCompileContextsHint( false  );

    _ico = new osgUtil::IncrementalCompileOperation( );
    _ico->setTargetFrameRate( 60.0f );

    _pager = osgDB::DatabasePager::create( );
    _pager->setIncrementalCompileOperation( _ico );
}

Config::~Config( )
{
EQINFO << "<===== Config::~Config(" << (void *)this << ")" << std::endl;
}

bool Config::init( )
{
EQINFO << "-----> Config::init( )" << std::endl;

    registerObject( &_frameData );
    _initData.setFrameDataID( _frameData.getID( ));
    registerObject( &_initData );

    bool init = false;

    _viewer = new CompositeViewer;
    _viewer->setThreadingModel( osgViewer::ViewerBase::ThreadPerCamera );
    _viewer->setStartTick( osg::Timer::instance( )->getStartTick( ));
    _viewer->setIncrementalCompileOperation( _ico );

    if( eq::Config::init( _initData.getID( )))
    {
        ViewCollector m( this );
        accept( m );

        if( NULL != selectCurrentView( m._lastViewID ))
            init = true;
    }

    if( !init )
        cleanup( );

EQINFO << "<----- Config::init( )" << std::endl;

    return init;
}

bool Config::exit( )
{
    bool ret = eq::Config::exit( );

    cleanup( );

    return ret;
}

uint32_t Config::startFrame( )
{
    const double t = static_cast< double >( getTime( )) / 1000.;
    _frameData.setSimulationTime( t );
    _frameData.setCalendarTime( time( NULL ));

    const eq::uint128_t& currentViewID = _frameData.getCurrentViewID( );
    View* view = static_cast< View* >( find< eq::View >( currentViewID ));
    if( view )
    {
        osgEarth::MapNode* map = osgEarth::MapNode::findMapNode(
            view->getView( )->getSceneData( ));
        if( map && map->isGeocentric( ))
        {
            osg::Vec3d eye, center, up;
            view->getView( )->getCamera( )->getViewMatrix( ).getLookAt(
                eye, center, up );
            double d = eye.length( );

            double rp = map->getMap( )->
                getProfile( )->getSRS( )->getEllipsoid()->getRadiusPolar( );

            if( d > rp )
            {
                double zf = ::sqrt( d * d - rp * rp ); 
                double nfr = NFR_AT_RADIUS + NFR_AT_DOUBLE_RADIUS *
                    (( d - rp ) / d ); 
                double zn = osg::clampAbove( zf * nfr, 1.0 );

                view->setNearFar( zn, zf );
            }
        }

        const osgGA::CameraManipulator* m =
            view->getView( )->getCameraManipulator( );
        view->setViewMatrix( osgToVmml( m->getInverseMatrix( )));
        //_frameData.setViewMatrix( osgToVmml( m->getInverseMatrix( )));
    }

    uint32_t ret = eq::Config::startFrame( _frameData.commit( ));

    if(( _viewer->getNumViews( ) > 0 ) && ( ++_appRenderTick > 1 ))
    {
        if( !_gc.valid( ) && !appInitGL( ))
            EQWARN << "Unable to create application GL context" << std::endl;
        
//EQWARN << "app render frame #" << getCurrentFrame( ) << std::endl;

        _viewer->setGlobalContext( _gc );
        _viewer->frameStart( getCurrentFrame( ), _frameData, false );
        _viewer->renderingTraversals( );
    }

    return ret;
}

uint32_t Config::finishFrame( )
{
    if( _viewer->getNumViews( ) > 0 )
        _viewer->frameFinish( false );
    else
        _appRenderTick = 0U;

    return eq::Config::finishFrame( );
}

bool Config::mapInitData( const eq::uint128_t& initDataID )
{
    bool mapped = false;
    if( !_initData.isAttached( ))
    {
        if( mapObject( &_initData, initDataID ));
        {
            unmapObject( &_initData );
            mapped = true;
        }
    }
    else
    {
        EQASSERT( _initData.getID( ) == initDataID );
        mapped = true;
    }
    return mapped;
}

bool Config::handleEvent( const eq::ConfigEvent* event )
{
    bool ret = false;

    const double time = static_cast< double >( getTime( )) / 1000.;

    switch( event->data.type )
    {
        case eq::Event::WINDOW_POINTER_WHEEL:
        case eq::Event::CHANNEL_POINTER_MOTION:
        case eq::Event::CHANNEL_POINTER_BUTTON_PRESS:
        case eq::Event::CHANNEL_POINTER_BUTTON_RELEASE:
            handleMouseEvent( event );
            ret = true;
            break;
        case eq::Event::KEY_PRESS:
            if( _eventQueue.valid( ))
                _eventQueue->keyPress( eqKeyToOsg( event->data.key.key ),
                    time );

            if( 's' == event->data.key.key )
                _frameData.toggleStatistics( );
            if( 't' == event->data.key.key )
            {
                const eq::uint128_t& currentViewID =
                    _frameData.getCurrentViewID( );
                View* view =
                    static_cast< View* >( find< eq::View >( currentViewID ));
                if( view )
                {
                    const eq::View::Mode mode = view->getMode( );
                    if( eq::View::MODE_MONO == mode )
                        view->changeMode( eq::View::MODE_STEREO );
                    else
                        view->changeMode( eq::View::MODE_MONO );
                }
            }
            if( '1' == event->data.key.key )
            {
                eq::Canvas* canvas = find< eq::Canvas >( "chunk" );
                if( canvas )
                {
                    int64_t index = canvas->getActiveLayoutIndex( );
                    canvas->useLayout( index ? 0 : 1 );
                }
            }
            break;

        case eq::Event::KEY_RELEASE:
            if( _eventQueue.valid( ))
                _eventQueue->keyRelease( eqKeyToOsg( event->data.key.key ),
                    time );
            break;

        case ConfigEvent::INTERSECTION:
        {
            const ConfigEvent* hitEvent =
                static_cast< const ConfigEvent* >( event );

            EQINFO << std::fixed << hitEvent << " from " <<
                hitEvent->data.originator << std::endl;
#if 0
if( _map.valid( ))
{

        double lat_rad, lon_rad, height;
        _map->getMap( )->getProfile( )->getSRS( )->getEllipsoid(
)->convertXYZToLatLongHeight( hitEvent->hit.x( ), hitEvent->hit.y( ),
hitEvent->hit.z( ), lat_rad, lon_rad, height );

        // query the elevation at the map point:
        double lat_deg = osg::RadiansToDegrees( lat_rad );
        double lon_deg = osg::RadiansToDegrees( lon_rad );

        EQWARN << "\tHIT(" << lat_deg << ", " << lon_deg << ")" << std::endl;
}
#endif
            ret = true;
            break;
        }
    }

    if( !ret )
        ret = eq::Config::handleEvent( event );

    return ret;
}

osg::Node* Config::getScene( const eq::uint128_t& sceneID )
{
    static co::base::Lock lock;
    co::base::ScopedMutex<> mutex( lock );

    if( !_scene.valid( ))
    {
        osg::Group* group = new osg::Group( );

        group->addChild( osgDB::readNodeFile( _initData.getModelFileName( )));

        osgEarth::MapNode* map = osgEarth::MapNode::findMapNode( group );
        if( map && map->getMap( )->getProfile( ) &&
            map->getMap()->isGeocentric( ))
        {
            osgEarth::Util::SkyNode* sky =
                new osgEarth::Util::SkyNode( map->getMap( ));

            sky->addUpdateCallback( new SkyUpdateCallback );

            group->addChild( sky );

            osgEarth::Util::ObjectPlacer o( map );

/*
            {
               osg::ref_ptr< osg::Image > i = osgDB::readImageFile( "134.207.0.141.gst" );
               if( i.valid( ))
               {
                  osg::ref_ptr< osg::ImageStream > is = dynamic_cast< osg::ImageStream* >( i.get( ));

                  osg::Vec3 pos(0.0f,0.0f,0.0f);
                  osg::ref_ptr<osg::Drawable> drawable = myCreateTexturedQuadGeometry(pos, 640, 480, i, true, false, true);

                  osg::ref_ptr<osg::Geode> geode = new osg::Geode;

                  osg::StateSet* stateset = geode->getOrCreateStateSet();
                  stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
                  geode->addDrawable(drawable.get());

                  group->addChild( o.placeNode( geode, 39.997, -74.6157, 10 ));
               }
            }
*/

/*
            {
               osg::ref_ptr< osg::Node > n = osgDB::readNodeFile(
                  "/lustre/site_x/data2/models/FtDix/archive.txp" );
               if( n.valid( ))
                  group->addChild( o.placeNode( n, 39.997, -74.6157, 5 ));
            }

            {
               osg::ref_ptr< osg::Node > n = osgDB::readNodeFile(
                  "/lustre/site_x/data2/models/FtOrdMOUT/Flythough/archive.txp" );
               if( n.valid( ))
                  group->addChild( o.placeNode( n, 36.5824, -121.8091, 100 ));
            }
*/
        }

        _scene = group;
    }
    return _scene.get( );
}

osgViewer::View* Config::takeOrCreateView( const eq::uint128_t& sceneID )
{
    osgViewer::View* view = NULL; // do *not* use ref_ptr here

    if( _viewer.valid( )) // i.e. isApplicationNode
    {
        view = _viewer->findViewByID( sceneID );
        if( view )
        {
            _viewer->removeView( view ); // eqEarth::View still has a ref

            view->getCamera( )->setGraphicsContext( 0 );
            view->getCamera( )->setViewport( 0 );
        }
    }

    if( !view )
    {
        view = CompositeViewer::createView( sceneID );

        view->setSceneData( getScene( sceneID ));
        view->setDatabasePager( getDatabasePager( )); 

        osgEarth::Util::SkyNode* sky =
            osgEarth::findTopMostNodeOfType< osgEarth::Util::SkyNode >(
                view->getSceneData( ));
        if( sky )
            sky->attach( view );
    }

    return view;
}

void Config::releaseView( osgViewer::View* view )
{   
    if( _viewer.valid( )) // i.e. isApplicationNode
    {
        view->getCamera( )->setGraphicsContext( 0 );
        view->getCamera( )->setViewport( 0 );

        _viewer->addView( view );
    }
}

void Config::cleanup( )
{
    deregisterObject( &_initData );
    deregisterObject( &_frameData );

    _initData.setFrameDataID( eq::UUID::ZERO );

    _viewer = 0;

    // Don't deref _scene until _pager threads are finished
    _pager = 0;
    _ico = 0;

    if( osg::Referenced::getDeleteHandler( ))
    {
        osg::Referenced::getDeleteHandler( )->setNumFramesToRetainObjects( 0 );
        osg::Referenced::getDeleteHandler( )->flushAll( );
    }

    _scene = 0;
}

View* Config::selectCurrentView( const eq::uint128_t& viewID )
{
    View* view = static_cast< View* >( find< eq::View >( viewID ));
    if( view )
    {
        _frameData.setCurrentViewID( viewID );
        _eventQueue = view->getView( )->getEventQueue( );
    }
    return view;
}

void Config::handleMouseEvent( const eq::ConfigEvent* event )
{
    const eq::uint128_t& viewID = event->data.context.view.identifier;

    View *view = NULL;

    if( NULL != ( view = selectCurrentView( viewID )))
    {
        const double time = static_cast< double >( getTime( )) / 1000.;
        const eq::PixelViewport& pvp = event->data.context.pvp;

        float x = static_cast< float >( event->data.pointer.x );
        float y = static_cast< float >( event->data.pointer.y );

        switch( event->data.type )
        {
            case eq::Event::WINDOW_POINTER_WHEEL:
            {
                osgGA::GUIEventAdapter::ScrollingMotion sm =
                    osgGA::GUIEventAdapter::SCROLL_NONE;
                if( event->data.pointer.xAxis > 0 )
                    sm = osgGA::GUIEventAdapter::SCROLL_UP;
                else if( event->data.pointer.xAxis < 0 )
                    sm = osgGA::GUIEventAdapter::SCROLL_DOWN;
                else if( event->data.pointer.yAxis > 0 )
                    sm = osgGA::GUIEventAdapter::SCROLL_RIGHT;
                else if( event->data.pointer.yAxis < 0 )
                    sm = osgGA::GUIEventAdapter::SCROLL_LEFT;
                _eventQueue->mouseScroll( sm, time );
                break;
            }
            case eq::Event::CHANNEL_POINTER_MOTION:
            {
                _eventQueue->setMouseInputRange( 0, 0, pvp.w, pvp.h );
                _eventQueue->mouseMotion( x, y, time );
                break;
            }
            case eq::Event::CHANNEL_POINTER_BUTTON_PRESS:
            {
                unsigned int b = eqButtonToOsg( event->data.pointer.button );
                if( b <= 3 )
                {
                    _eventQueue->setMouseInputRange( 0, 0, pvp.w, pvp.h );
                    _eventQueue->mouseButtonPress( x, y, b, time );
                }
                break;
            }
            case eq::Event::CHANNEL_POINTER_BUTTON_RELEASE:
            {
                unsigned int b = eqButtonToOsg( event->data.pointer.button );
                if( b <= 3 )
                {
                    _eventQueue->setMouseInputRange( 0, 0, pvp.w, pvp.h );
                    _eventQueue->mouseButtonRelease( x, y, b, time );
                }
                break;
            }
        }

        osgGA::EventQueue::Events events;
        _eventQueue->takeEvents( events );

        for( osgGA::EventQueue::Events::iterator itr = events.begin( );
            itr != events.end( ); ++itr)
        {
            osg::ref_ptr< osgGA::CameraManipulator > m =
                view->getView( )->getCameraManipulator( );
            if( m.valid( ))
            {
                osg::ref_ptr< osg::Camera > camera =
                    view->getView( )->getCamera( );

                const eq::Matrix4d viewMatrix = 
                    eq::Matrix4d( event->data.context.headTransform ) *
                        view->getViewMatrix( );

                const eq::Frustumf& frustum = event->data.context.frustum;

                ngc->setPVP( pvp.w, pvp.h );

                camera->setGraphicsContext( ngc );

                camera->setViewport( 0, 0, pvp.w, pvp.h );

                camera->setProjectionMatrixAsFrustum( 
                    frustum.left( ), frustum.right( ),
                    frustum.bottom( ), frustum.top( ),
                    frustum.near_plane( ), frustum.far_plane( )); 

                camera->setViewMatrix( vmmlToOsg( viewMatrix ));

                m->handleWithCheckAgainstIgnoreHandledEventsMask(
                    *itr->get( ), *view->getView( ));

                ngc->clearCameras( );

                camera->setGraphicsContext( 0 );
            }
        }
    }
}

void Config::updateCurrentWorldPointer( const eq::ConfigEvent* event )
{
#if 0
    const eq::PixelViewport& pvp = event->data.context.pvp;

    float x = static_cast< float >( event->data.pointer.x ) + pvp.x;
    float y = pvp.h - static_cast< float >( event->data.pointer.y ) + pvp.y;

#if 0
EQWARN << "xy: " << x << ", " << y << " in " << pvp << std::endl;
#endif

    const eq::Matrix4d viewMatrix = 
        eq::Matrix4d( event->data.context.headTransform ) *
            _frameData.getViewMatrix( );

    const eq::Frustumf& frustum = event->data.context.frustum;
    const eq::Matrix4d projectionMatrix = frustum.compute_matrix( );

    eq::Vector3d p1, p2;
    EQCHECK(
        gluUnProject( x, y, 0, // near plane
            viewMatrix.array, projectionMatrix.array, &pvp.x,
            &p1.x( ), &p1.y( ), &p1.z( ))
        &&
        gluUnProject( x, y, 1, // far plane
            viewMatrix.array, projectionMatrix.array, &pvp.x,
            &p2.x( ), &p2.y( ), &p2.z( )));

    const eq::uint128_t& viewID = event->data.context.view.identifier;
    View* view = static_cast< View* >( find< eq::View >( viewID ));
    EQASSERT( NULL != view );
    view->setWorldPointer( p1, p2 );
#endif
}

bool Config::appInitGL( bool pbuffer )
{
    osg::ref_ptr< osg::DisplaySettings > ds =
        osg::DisplaySettings::instance( );

    osg::ref_ptr< osg::GraphicsContext::Traits > traits =
        new osg::GraphicsContext::Traits( ds );

    traits->readDISPLAY( );
    if( traits->displayNum < 0 ) traits->displayNum = 0;
    if( traits->screenNum < 0 ) traits->screenNum = 0;

    traits->x = 0;
    traits->y = 0;
    traits->width = 10;
    traits->height = 10;
    traits->windowDecoration = false;
    traits->doubleBuffer = false;
    traits->sharedContext = 0;
    traits->pbuffer = pbuffer;

    _gc = osg::GraphicsContext::createGraphicsContext( traits );

    if( _gc.valid( ))
    {
        _gc->realize( );
        _gc->makeCurrent( );

        _ico->addGraphicsContext( _gc );

        unsigned int maxTexturePoolSize =
            osg::DisplaySettings::instance( )->getMaxTexturePoolSize( );
        unsigned int maxBufferObjectPoolSize =
            osg::DisplaySettings::instance( )->getMaxBufferObjectPoolSize( );

        if( maxTexturePoolSize > 0 )
            _gc->getState( )->setMaxTexturePoolSize( maxTexturePoolSize );
        if( maxBufferObjectPoolSize > 0 )
            _gc->getState( )->setMaxBufferObjectPoolSize(
                maxBufferObjectPoolSize );

        Window::initCapabilities( _gc );

        return true;
    }

    return false;
}
}
