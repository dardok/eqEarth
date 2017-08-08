#include "config.h"

#include "error.h"
#include "util.h"
#include "vps.h"
#include "callbacks.h"
#include "window.h" // for Window::initCapabilities
#if 0
#include "configEvent.h"
#endif
#include "controls.h"

#include "earthManipulator.h"
#include <osgEarth/TerrainEngineNode>
#include <osgGA/TrackballManipulator>

#include <osg/DeleteHandler>
#include <osg/ImageStream>
#include <osg/TextureRectangle>
#include <osgDB/ReadFile>

#include <osgEarth/NodeUtils>

//#include <GL/glu.h>

#include <osgEarth/MapNode>
#include <osgEarth/Registry>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/Sky>
#include <osgEarthUtil/Ocean>

#if 0
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ViewDependentShadowMap>
#include <osgEarthUtil/ObjectPlacer>
#include <osgEarthUtil/ShadowUtils>
#include <osgEarthAnnotation/AnnotationRegistry>
#include <osgEarthAnnotation/Decluttering>

#include <osgEarthDrivers/kml/KML>
#include <osgEarthDrivers/ocean_surface/OceanSurface>
#include <osgEarth/ShaderComposition>
#include <osgEarthSymbology/Color>
#include <osgEarthDrivers/engine_osgterrain/OSGTerrainOptions>
#include <osgEarthUtil/ObjectPlacer>
#endif

#define NFR_AT_RADIUS 0.00001
#define NFR_AT_DOUBLE_RADIUS 0.0049

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

void clearCameras( ) { _cameras.clear( ); }
};

static osg::ref_ptr< NullGraphicsContext > ngc =
    new NullGraphicsContext;

// ----------------------------------------------------------------------------

struct Config::ViewInitializer : public eq::ConfigVisitor
{
ViewInitializer( Config* config, bool release = false )
    : _config( config ),
      _release( release )
{
}

virtual eq::VisitorResult visit( eq::View* view )
{
    View* v = static_cast< View* >( view );
    osgViewer::View* osgView;

    if( _release ) {
        v->setOSGView( 0 );

        return eq::TRAVERSE_CONTINUE;
    }

    lunchbox::uint128_t id( 1 );

    v->setSceneID( id );
    v->setOverlayID( id );

    osgView = _config->takeOrCreateOSGView( v->getSceneID( ));

    v->setOSGView( osgView );

    if( !osgView->getCameraManipulator( ))
    {
        osgGA::CameraManipulator* m;

        osgEarth::MapNode* map =
            osgEarth::MapNode::findMapNode( osgView->getSceneData( ));
        if( map )
        {
            EarthManipulator* em = new EarthManipulator;
#if 0
            if( !map->isGeocentric( ))
                em->setCameraProjection(
                    EarthManipulator::PROJ_ORTHOGRAPHIC );
#endif
            em->setNode( map->getTerrainEngine( ));
            m = em;
        }
        else
        {
            m = new osgGA::TrackballManipulator;
            m->setHomePosition(
                osg::Vec3d( 0, 0, 100 ),
                osg::Vec3d( 0, 0, 0 ),
                osg::Vec3d( 0, 1, 0 ), false );
            m->setNode( osgView->getSceneData( ));
        }

        osgView->setCameraManipulator( m );
    }

    _config->releaseOSGView( osgView );

    LBCHECK( _config->selectCurrentView( v->getID( )));

    return eq::TRAVERSE_CONTINUE;
}

public:
    Config* _config;

private:
    bool _release;
};

// ----------------------------------------------------------------------------

struct ViewUpdater : public eq::ConfigVisitor
{
virtual eq::VisitorResult visit( eq::View* view )
{
    View* v = static_cast< View* >( view );
    osgViewer::View* osgView = v->getOSGView( );
    LBASSERT( osgView );
    const osgGA::CameraManipulator* m = osgView->getCameraManipulator( );
    LBASSERT( m );
    const osg::Matrixd& viewMatrix = m->getInverseMatrix( );
    //const osg::Matrixd& viewMatrix = osgView->getCamera( )->getViewMatrix( );

    /* VIEW MATRIX */
    v->setViewMatrix( osgToVmml( viewMatrix ));

    /* NEAR/FAR */
    osgEarth::MapNode* map = osgEarth::MapNode::findMapNode(
        osgView->getSceneData( ));
    if( map )
    {
        if( map->isGeocentric( ))
        {
            osg::Vec3d eye, center, up;
            viewMatrix.getLookAt( eye, center, up );
            double d = eye.length( );

            double rp = map->getMap( )->
                getProfile( )->getSRS( )->getEllipsoid()->getRadiusPolar( );

            if( d > rp )
            {
                double zf = ::sqrt( d * d - rp * rp );
                double nfr = NFR_AT_RADIUS + NFR_AT_DOUBLE_RADIUS *
                    (( d - rp ) / d );
                double zn = osg::clampAbove( zf * nfr, 1.0 );

                v->setNearFar( zn, zf );

                //LBWARN << "nf = " << zn << ", " << zf << std::endl;
            }
        }
        else
        {
            double d, zn, zf;
            viewMatrix.getOrtho( d, d, d, d, zn, zf );

            v->setNearFar( zn, zf );

            //LBWARN << "nf = " << zn << ", " << zf << std::endl;
        }
    }

    /* LAT/LON */
    osg::ref_ptr< const osgEarth::Util::EarthManipulator > em =
        dynamic_cast< const osgEarth::Util::EarthManipulator* >( m );
    if( em.valid( ))
    {
        const osgEarth::optional<osgEarth::GeoPoint> vp = em->getViewpoint( ).focalPoint( );
        v->setLatLon( vp.get( ).y( ), vp.get( ).x( ));
    }

    return eq::TRAVERSE_CONTINUE;
}
};

// ----------------------------------------------------------------------------

Config::Config( eq::ServerPtr parent )
    : eq::Config( parent )
    , _thread_hint( true )
    , _appRenderTick( 0U )
    , _shutdown( false )
    , _zmode( NULL )
    , _zvalue( NULL )
{
LBINFO << "=====> Config::Config(" << (void *)this << ")" << std::endl;

    //srandom( time( NULL ));
    srandom( 1 );

    osg::Referenced::setThreadSafeReferenceCounting( true );

    //osg::Referenced::setDeleteHandler( new osg::DeleteHandler( 2 ));

    // initialize the global timer to be relative to the current time.
    osg::Timer::instance( )->setStartTick( );

    osg::ref_ptr< osg::DisplaySettings > ds =
        osg::DisplaySettings::instance( );
    ds->setMaxTexturePoolSize( 100000000 );      // size in bytes (~100Mb)
    ds->setMaxBufferObjectPoolSize( 200000000 ); // size in bytes (~200Mb)
    ds->setSerializeDrawDispatch( false  );
    ds->setCompileContextsHint( false  );
#if 1
    ds->setNumOfDatabaseThreadsHint( 2 );
    ds->setNumOfHttpDatabaseThreadsHint( 2 );
#endif

#if 0
    _ico = new osgUtil::IncrementalCompileOperation( );
    _ico->setTargetFrameRate( 60.0f );
#endif

    _pager = osgDB::DatabasePager::create( );
    _pager->setUnrefImageDataAfterApplyPolicy( false, false );
    if( _ico.valid( ))
        _pager->setIncrementalCompileOperation( _ico );

    osgDB::Registry::instance()->getObjectWrapperManager()->findWrapper("osg::Image");
}

Config::~Config( )
{
LBINFO << "<===== Config::~Config(" << (void *)this << ")" << std::endl;
}

bool Config::init( )
{
LBINFO << "-----> Config::init( )" << std::endl;

    registerObject( &_frameData );
    _initData.setFrameDataID( _frameData.getID( ));
    registerObject( &_initData );

    bool init = false;

    _viewer = new CompositeViewer;
    _viewer->setThreadingModel( osgViewer::ViewerBase::SingleThreaded );
    _viewer->setStartTick( osg::Timer::instance( )->getStartTick( ));
    if( _ico.valid( ))
        _viewer->setIncrementalCompileOperation( _ico );

    if( !eq::Config::init( _initData.getID( )))
        goto out;

    _accept( ViewInitializer( this ));

    init = true;

out:
    if( !init )
        cleanup( );

LBINFO << "<----- Config::init( )" << std::endl;

    return init;
}

bool Config::exit( )
{
LBINFO << "-----> Config::exit( )" << std::endl;

    // Ignore any further events
    _shutdown = true;

    // Stop database pagers before GL shutdown
    _viewer->shutdown( );
    _pager->cancel( );

    // Clear view references before GL shutdown
    _accept( ViewInitializer( this, true ));

    cleanup( );

    // Shutdown GL (destroys View, Channel, Window, Pipe, Node - basically everything)
    bool ret = eq::Config::exit( );

LBINFO << "<----- Config::exit( ) = " << ret << std::endl;
    return ret;
}

uint32_t Config::startFrame( )
{
LBINFO << "-----> Config<" << getName( ) << ">::startFrame( )" << std::endl;

    _accept( ViewUpdater( ));

    const double t = static_cast< double >( getTime( )) / 1000.;
    _frameData.setSimulationTime( t );
    _frameData.setCalendarTime( time( NULL ));

    uint32_t ret = eq::Config::startFrame( _frameData.commit( ));

    // Need to wait one frame to see if Nodes/Channels take all the views away
    if(( _viewer->getNumViews( ) > 0 ) && ( ++_appRenderTick > 1 ))
    {
        if( !_gc.valid( ) && !appInitGL( ))
            LBWARN << "Unable to create application GL context" << std::endl;

        if( _gc.valid( ))
        {
            //LBWARN << "app render frame #" << getCurrentFrame( ) << std::endl;

            _viewer->setGlobalContext( _gc );

            _viewer->frameStart( getCurrentFrame( ), _frameData );

            _viewer->renderingTraversals( _thread_hint );
        }
    }

LBINFO << "<----- Config<" << getName( ) << ">::startFrame( )" << std::endl;

    return ret;
}

uint32_t Config::finishFrame( )
{
LBINFO << "-----> Config<" << getName( ) << ">::finishFrame( )" << std::endl;

    if( _viewer->getNumViews( ) > 0 )
    {
        if( _gc.valid( ))
            _viewer->frameDrawFinish( );
    }
    else
        _appRenderTick = 0U;

LBINFO << "<----- Config<" << getName( ) << ">::finishFrame( )" << std::endl;

    return eq::Config::finishFrame( );
}

bool Config::mapInitData( const eq::uint128_t& initDataID )
{
    bool mapped = false;
    if( !_initData.isAttached( ))
    {
        const uint32_t request = mapObjectNB( &_initData, initDataID,
            co::VERSION_OLDEST, getApplicationNode( ));
        if( mapObjectSync( request ))
        {
            unmapObject( &_initData );
            mapped = true;
        }
    }
    else
    {
        LBASSERT( _initData.getID( ) == initDataID );
        mapped = true;
    }
    return mapped;
}

bool Config::handleEvent( eq::EventType type, const eq::KeyEvent& event )
{
    bool ret = false;
    const double time = static_cast< double >( getTime( )) / 1000.;

    switch( type )
    {
        case eq::EVENT_KEY_PRESS:
        {
            const int osgKey = eqKeyToOsg( event.key );
            if( _eventQueue.valid( ))
                _eventQueue->keyPress( osgKey, time );

            if( 's' == event.key )
            {
                _frameData.toggleStatistics( );
                ret = true;
            }

            if( 't' == event.key )
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
                ret = true;
            }

            if( 'z' == event.key )
            {
                _frameData.toggleZMode( );
                ret = true;
            }

            if( 'q' == event.key )
            {
                _frameData.setZValue( _frameData.getZValue( ) - 10.0f );
                ret = true;
            }

            if( 'w' == event.key )
            {
                _frameData.setZValue( _frameData.getZValue( ) + 10.0f );
                ret = true;
            }

#if 0
            if( '1' == event.key )
            {
                eq::Canvas* canvas = find< eq::Canvas >( "clove" );
                if( canvas )
                {
                    int64_t index = canvas->getActiveLayoutIndex( );
                    canvas->useLayout( index ? 0 : 1 );
                }
                ret = true;
            }
#endif
            break;
        }
        case eq::EVENT_KEY_RELEASE:
        {
            //const eq::Event& event = command.get< eq::Event >();
            //const eq::KeyEvent& keyEvent = event.keyPress;
            //const int osgKey = eqKeyToOsg( keyEvent.key );
            const int osgKey = eqKeyToOsg( event.key );
            if( _eventQueue.valid( ))
                _eventQueue->keyRelease( osgKey, time );
            break;
        }
    }

    if( !ret )
        ret = eq::Config::handleEvent( type, event );

    return ret;
}

bool Config::handleEvent( eq::EventType type, const eq::PointerEvent& event )
{
    bool ret = false;
    const double time = static_cast< double >( getTime( )) / 1000.;

    if( _shutdown )
        return true;

    switch( type )
    {
        case eq::EVENT_WINDOW_POINTER_WHEEL:
        case eq::EVENT_CHANNEL_POINTER_MOTION:
        case eq::EVENT_CHANNEL_POINTER_BUTTON_PRESS:
        case eq::EVENT_CHANNEL_POINTER_BUTTON_RELEASE:
        {
            //const eq::Event& event = command.get< eq::Event >();
            View* view =
                selectCurrentView( event.context.view.identifier );
            if( view && _eventQueue.valid( ))
            {
                handleMouseEvent( type, event, view, time );
                ret = true;
            }
            break;
        }
    }

    if( !ret )
        ret = eq::Config::handleEvent( type, event );

    return ret;
}

osgViewer::View* Config::takeOrCreateOSGView( const eq::uint128_t& sceneID )
{
    const bool needViewerLock = ( getNodes( ).size( ) > 1 );
    lunchbox::ScopedWrite _mutex( needViewerLock ? &_viewer_lock : 0 );

    osgViewer::View* osgView = NULL; // do *not* use ref_ptr here

    if( _viewer.valid( )) // i.e. isApplicationNode
    {
        osgView = _viewer->findOSGViewByID( sceneID );
        if( osgView )
        {
            _viewer->removeView( osgView ); // eqEarth::View still has a ref

            osgView->getCamera( )->setGraphicsContext( 0 );
            osgView->getCamera( )->setViewport( 0 );
        }
    }

    if( !osgView )
    {
        osgView = CompositeViewer::createOSGView( sceneID );

        osgView->getCamera( )->setComputeNearFarMode(
            osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );

        osgView->setSceneData( getScene( sceneID, osgView ));
        osgView->setDatabasePager( _pager );
    }

    return osgView;
}

void Config::releaseOSGView( osgViewer::View* osgView )
{
    const bool needViewerLock = ( getNodes( ).size( ) > 1 );
    lunchbox::ScopedWrite _mutex( needViewerLock ? &_viewer_lock : 0 );

    if( _viewer.valid( )) // i.e. isApplicationNode
    {
        osgView->getCamera( )->setGraphicsContext( 0 );
        osgView->getCamera( )->setViewport( 0 );

        _viewer->addView( osgView );
    }
}

void Config::createOverlay( osgEarth::Util::Controls::ControlCanvas* cc,
        eq::View* view )
{
    // osgDB is not thread safe
    static lunchbox::Lock lock;
    lunchbox::ScopedWrite _mutex( lock );

    View* v = static_cast< View* >( view );

    LBASSERT( v->getOverlayID( ) == 1 ); // TODO : multiple overlays

#if 0
    {
        using namespace osgEarth::Symbology;
        using namespace osgEarth::Util::Controls;

        // Logo bottom-left
        VBox* center = new VBox( );
        center->setFrame( new RoundedFrame( ));
        center->getFrame()->setBackColor( 0, 0, 0, 0.5 );
        center->setPadding( 10 );
        center->setMargin( 10 );
        center->setVertAlign( Control::ALIGN_BOTTOM );
        center->setHorizAlign( Control::ALIGN_LEFT );

        osg::ref_ptr< osg::Image > image =
            osgDB::readImageFile("/afs/cmf/users/dkleiner/nrl-dc-logo.png");
        if ( image.valid( ))
        {
            ImageControl* imageControl = new ImageControl( image.get( ));
            imageControl->setHorizAlign( Control::ALIGN_CENTER );

            center->addControl( imageControl );
        }

        cc->addControl( center );

        // Lat/Lon bottom-center
        HBox* bottom = new HBox( );
        bottom->setFrame( new RoundedFrame( ));
        bottom->getFrame( )->setBackColor( 0, 0, 0, 0.5 );
        bottom->setMargin( 10 );
        bottom->setVertAlign( Control::ALIGN_BOTTOM );
        bottom->setHorizAlign( Control::ALIGN_CENTER );

        LonLatLabelControl* lonlat = new LonLatLabelControl( );
        lonlat->setMargin( 10 );
        lonlat->setBackColor( 0, 0, 0, 0.5 );
        lonlat->setActiveColor( 0, 0, 0, 0.5 );
        bottom->addControl( lonlat );

        cc->addControl( bottom );

        cc->addUpdateCallback( new ControlUpdateCallback( v, lonlat ));

        // Classification top-center
        HBox* top = new HBox( );
        top->setFrame( new Frame( ));
        top->getFrame( )->setBackColor( 0, 0, 0, 0.5 );
        top->setMargin( 10 );
        top->setVertAlign( Control::ALIGN_TOP );
        top->setHorizAlign( Control::ALIGN_CENTER );

        LabelControl* classification = new LabelControl( );
        classification->setMargin( 0 );
        classification->setBackColor( 1, 1, 0, 0.5 );
        classification->setForeColor( 0, 0, 0, 0.75 );
        classification->setText( "        Unclassified     //     FOUO        " );
        classification->setEncoding( osgText::String::ENCODING_UTF8 );
        classification->setFont( osgText::readFontFile(
            "/afs/cmf/project/dc/sys/share/OpenSceneGraph-Data/fonts/VeraMono.ttf"
        ));
        top->addControl( classification );


        cc->addControl( top );
    }
#endif
}

void Config::setZMode( bool zmode )
{
    if( _zmode )
        _zmode->set( zmode );
}

void Config::setZValue( float zvalue )
{
    if( _zvalue )
        _zvalue->set( zvalue );
}

osg::Group* Config::getScene( const eq::uint128_t& sceneID,
        osgViewer::View* view )
{
    LBASSERT( sceneID == 1 ); // TODO : multiple scenes

    if( !_scene.valid( ))
    {
        using namespace osgEarth;
        //using namespace osgEarth::Drivers;
        using namespace osgEarth::Util;
        //using namespace osgEarth::Annotation;

        osg::ref_ptr<osg::Node> loadedModel;
        osg::Group* group = new osg::Group( );

        Map* map = NULL;
        MapNode* mapNode = NULL;

        osgEarth::Config c;
        c.add( "elevation_smoothing", false );
        TerrainOptions to( c );

        MapNodeOptions defMNO;
        defMNO.setTerrainOptions( to );

        osg::ref_ptr<osgDB::Options> dbo = Registry::instance( )->cloneOrCreateOptions( );
        dbo->setPluginStringData( "osgEarth.defaultOptions", defMNO.getConfig( ).toJSON( ));

        loadedModel = osgDB::readNodeFile( _initData.getModelFileName( ), dbo.get( ));

        group->addChild( loadedModel );
        mapNode = osgEarth::MapNode::findMapNode( group );
        if( mapNode )
            map = mapNode->getMap( );

        if( mapNode && map->getProfile( ) && map->isGeocentric( ))
        {
#if 1
            SkyNode* sky = osgEarth::Util::SkyNode::create( mapNode );

            sky->addUpdateCallback( new SkyUpdateCallback );
            sky->setSunVisible( true );
            sky->setMoonVisible( true );
            sky->attach( view );

            group->addChild( sky );
#endif

            const osgEarth::Util::Config& externals = mapNode->externalConfig( );

            const osgEarth::Util::Config& annoConf = externals.child( "annotations" );

#if 0
            OceanNode* ocean = osgEarth::Util::OceanNode::create( mapNode );
            group->addChild( ocean );
#endif
#if 0
            OceanSurfaceNode* ocean = new OceanSurfaceNode( mapNode, externals.child( "ocean" ));
            group->addChild( ocean );
#endif

#if 0
            if ( !annoConf.empty() )
            {
                osg::Group* annotations = NULL;
                AnnotationRegistry::instance( )->create(
                    mapNode, annoConf, dbOptions.get( ), annotations );
                if( annotations )
                {
                    group->addChild( annotations );
                }
            }

            const osgEarth::Util::Config& declutterConf = externals.child( "decluttering" );
            if ( !declutterConf.empty() )
            {
                Decluttering::setOptions( DeclutteringOptions( declutterConf ));
            }
#endif

        } else if( loadedModel ) {
          osg::StateSet *ss = dynamic_cast<osg::Geode *>( loadedModel.get( ))->getOrCreateStateSet( );
          ss->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

static const char *zshaderVertSource = {
"uniform bool zmode;                                                         \n"
"uniform float zvalue;                                                       \n"
"out float z;                                                                \n"
"out vec4 color;                                                             \n"
"void main()                                                                 \n"
"{                                                                           \n"
"        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;             \n"
"        z = 1.0 / (zvalue / gl_Vertex.z);                                   \n"
"        color = gl_Color;                                                   \n"
"}                                                                           \n"
};

static const char *zshaderFragSource = {
"uniform bool zmode;                                                         \n"
"uniform float zvalue;                                                       \n"
"in float z;                                                                 \n"
"in vec4 color;                                                              \n"
"void main()                                                                 \n"
"{                                                                           \n"
"        if (zmode) {                                                        \n"
"          gl_FragColor = vec4 (z, z, z, 1.0);                               \n"
"        } else {                                                            \n"
"          gl_FragColor = color;                                             \n"
"        }                                                                   \n"
"}                                                                           \n"
};

          osg::Program* program = new osg::Program;
          program->setName( "zshader" );
          program->addShader( new osg::Shader( osg::Shader::VERTEX, zshaderVertSource ));
          program->addShader( new osg::Shader( osg::Shader::FRAGMENT, zshaderFragSource ));
          ss->setAttributeAndModes( program, osg::StateAttribute::ON );

          ss->addUniform( _zmode = new osg::Uniform( "zmode", true ));
          ss->addUniform( _zvalue = new osg::Uniform( "zvalue", 50.0f ));
        } else {
          LBWARN << "No model loaded!" << std::endl;
        }

        const std::string &kmlFile = _initData.getKMLFileName( );
        if( endsWith( kmlFile, ".kml" ))
        {
            osg::ref_ptr<osgDB::Options> options = new osgDB::Options( );
            options->setPluginData( "osgEarth::MapNode", mapNode );
            osg::Node* kml = osgDB::readNodeFile( kmlFile, options.get( ));
            if ( kml )
                group->addChild( kml );
        }

        _scene = group;

        // make sure that existing scene graph objects are allocated with
        // thread safe ref/unref
        _scene->setThreadSafeRefUnref( true );
    }

    osg::ref_ptr< osg::DisplaySettings > ds =
        osg::DisplaySettings::instance( );
    _scene->resizeGLObjectBuffers( ds->getMaxNumberOfGraphicsContexts( ));

    return _scene.get( );
}

void Config::clearScene( )
{
    if( _scene.valid( ))
        _scene->removeChildren( 0, _scene->getNumChildren( ));
}

void Config::cleanup( )
{
    deregisterObject( &_initData );
    deregisterObject( &_frameData );

    _initData.setFrameDataID( eq::uint128_t( ));

    _eventQueue = 0;
    _viewer = 0;

    _pager = 0;
    _ico = 0;

    if( osg::Referenced::getDeleteHandler( ))
    {
        osg::Referenced::getDeleteHandler( )->setNumFramesToRetainObjects( 0 );
        osg::Referenced::getDeleteHandler( )->flushAll( );
    }

    _scene = 0;

    _gc = 0;
}

View* Config::selectCurrentView( const eq::uint128_t& viewID )
{
    View* view = static_cast< View* >( find< eq::View >( viewID ));
    if( view )
    {
        _frameData.setCurrentViewID( viewID );
        _eventQueue = view->getOSGView( )->getEventQueue( );
    }
    return view;
}

void Config::handleMouseEvent( eq::EventType type, const eq::PointerEvent& event, View* view,
          double time )
{
    const eq::PixelViewport& pvp = event.context.pvp;
    const uint32_t x = event.x;
    const uint32_t y = event.y;

    LBASSERT( _eventQueue.valid( ));

    switch( type )
    {
        case eq::EVENT_WINDOW_POINTER_WHEEL:
        {
            osgGA::GUIEventAdapter::ScrollingMotion sm =
                osgGA::GUIEventAdapter::SCROLL_NONE;
            if( event.xAxis > 0 )
                sm = osgGA::GUIEventAdapter::SCROLL_UP;
            else if( event.xAxis < 0 )
                sm = osgGA::GUIEventAdapter::SCROLL_DOWN;
            else if( event.yAxis > 0 )
                sm = osgGA::GUIEventAdapter::SCROLL_RIGHT;
            else if( event.yAxis < 0 )
                sm = osgGA::GUIEventAdapter::SCROLL_LEFT;
            _eventQueue->mouseScroll( sm, time );
            break;
        }
        case eq::EVENT_CHANNEL_POINTER_MOTION:
            _eventQueue->setMouseInputRange( 0, 0, pvp.w, pvp.h );
            _eventQueue->mouseMotion( x, y, time );
            break;
        case eq::EVENT_CHANNEL_POINTER_BUTTON_PRESS:
        {
            const unsigned int b = eqButtonToOsg( event.button );
            if( b <= 3 )
            {
                _eventQueue->setMouseInputRange( 0, 0, pvp.w, pvp.h );
                _eventQueue->mouseButtonPress( x, y, b, time );
            }
            break;
        }
        case eq::EVENT_CHANNEL_POINTER_BUTTON_RELEASE:
        {
            const unsigned int b = eqButtonToOsg( event.button );
            if( b <= 3 )
            {
                _eventQueue->setMouseInputRange( 0, 0, pvp.w, pvp.h );
                _eventQueue->mouseButtonRelease( x, y, b, time );
            }
            break;
        }
        default:
            break;
    }

    osgGA::EventQueue::Events events;
    _eventQueue->takeEvents( events );

    for( osgGA::EventQueue::Events::iterator itr = events.begin( );
            itr != events.end( ); ++itr)
    {
        osgViewer::View* osgView = view->getOSGView( );
        LBASSERT( osgView );

        osg::ref_ptr< osgGA::CameraManipulator > m =
            osgView->getCameraManipulator( );
        if( m.valid( ))
        {
            osg::ref_ptr< osg::Camera > camera = osgView->getCamera( );

            ngc->setPVP( pvp.w, pvp.h );

            camera->setGraphicsContext( ngc );

            // viewport
            camera->setViewport( 0, 0, pvp.w, pvp.h );

            osgEarth::MapNode* map =
                osgEarth::MapNode::findMapNode( osgView->getSceneData( ));

            if( map )
            {
                double near, far;
                view->getNearFar( near, far );
                const eq::Matrix4f& headView = view->getViewMatrix( );

                if( map->isGeocentric( ))
                {
                    eq::Frustumf frustum = event.context.frustum;
                    frustum.adjustNearPlane( near );
                    frustum.farPlane( ) = far;
                    camera->setProjectionMatrixAsFrustum(
                        frustum.left( ), frustum.right( ),
                        frustum.bottom( ), frustum.top( ),
                        frustum.nearPlane( ), frustum.farPlane( ));

                    const eq::Matrix4f& headTransform =
                        event.context.headTransform;
                    camera->setViewMatrix( vmmlToOsg( headTransform *
                            headView ));
                }
                else
                {
                    eq::Frustumf frustum = event.context.ortho;
                    frustum.adjustNearPlane( near );
                    frustum.farPlane( ) = far;
                    camera->setProjectionMatrixAsOrtho(
                        frustum.left( ), frustum.right( ),
                        frustum.bottom( ), frustum.top( ),
                        frustum.nearPlane( ), frustum.farPlane( ));

                    const eq::Matrix4f& orthoTransform =
                        event.context.orthoTransform;
                    camera->setViewMatrix( vmmlToOsg( orthoTransform *
                            headView ));
                }
            }

            osgGA::GUIEventAdapter& ea( *(( *itr )->asGUIEventAdapter( )));
            {
                    // Code stolen from GUIEventHandler::handleWithCheckAgainstIgnoreHandledEventsMask,
                    // which was deprecated approximately OSG v3.3.1.
                    bool handled = m->handle( ea, *osgView );
                    if (handled) ea.setHandled( true );
            }

            ngc->clearCameras( );

            camera->setGraphicsContext( 0 );
            camera->setViewport( 0 );
        }
    }
}

#if 0
void Config::updateCurrentWorldPointer( const eq::ConfigEvent& event )
{
    const eq::PixelViewport& pvp = event.context.pvp;

    float x = static_cast< float >( event.pointer.x ) + pvp.x;
    float y = pvp.h - static_cast< float >( event.pointer.y ) + pvp.y;

#if 0
LBWARN << "xy: " << x << ", " << y << " in " << pvp << std::endl;
#endif

    const eq::Matrix4f viewMatrix =
        eq::Matrix4f( event.context.headTransform ) *
            _frameData.getViewMatrix( );

    const eq::Frustumf& frustum = event.context.frustum;
    const eq::Matrix4f projectionMatrix = frustum.computePerspectiveMatrix( );

    eq::Vector3d p1, p2;
    LBCHECK(
        gluUnProject( x, y, 0, // near plane
            viewMatrix.array, projectionMatrix.array, &pvp.x,
            &p1.x( ), &p1.y( ), &p1.z( ))
        &&
        gluUnProject( x, y, 1, // far plane
            viewMatrix.array, projectionMatrix.array, &pvp.x,
            &p2.x( ), &p2.y( ), &p2.z( )));

    const eq::uint128_t& viewID = event.context.view.identifier;
    View* view = static_cast< View* >( find< eq::View >( viewID ));
    LBASSERT( NULL != view );
    view->setWorldPointer( p1, p2 );
}
#endif

bool Config::appInitGL( bool pbuffer )
{
LBINFO << "-----> Config<" << getName( ) << ">::appInitGL( )" << std::endl;

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

    if( !_gc.valid( ))
        return false;

    LBCHECK( _gc->realize( ));

    if( _ico.valid( ))
        _ico->addGraphicsContext( _gc );

    const unsigned int maxTexturePoolSize =
        osg::DisplaySettings::instance( )->getMaxTexturePoolSize( );
    const unsigned int maxBufferObjectPoolSize =
        osg::DisplaySettings::instance( )->getMaxBufferObjectPoolSize( );

    if( maxTexturePoolSize > 0 )
        _gc->getState( )->setMaxTexturePoolSize( maxTexturePoolSize );
    if( maxBufferObjectPoolSize > 0 )
        _gc->getState( )->setMaxBufferObjectPoolSize(
            maxBufferObjectPoolSize );

    Window::initCapabilities( _gc );

LBINFO << "<----- Config<" << getName( ) << ">::appInitGL( )" << std::endl;

    return true;
}
}
