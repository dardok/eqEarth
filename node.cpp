#include "node.h"

#include "config.h"
#include "error.h"
#include "util.h"
#include "pipe.h"

#include <osg/DeleteHandler>

#include <osgEarth/MapNode>
#include <osgEarth/NodeUtils>

#include <osgEarthUtil/AutoClipPlaneHandler>

namespace eqEarth
{
// ----------------------------------------------------------------------------

Node::Node( eq::Config* parent )
    : eq::Node( parent )
    , _frameNumber( 0UL )
{
LBINFO << "=====> Node::Node(" << (void *)this << ")" << std::endl;
}

Node::~Node( )
{
LBINFO << "<===== Node::~Node(" << (void *)this << ")" << std::endl;
}

void Node::addGraphicsContext( osg::GraphicsContext* context )
{
    LB_TS_NOT_THREAD( _nodeThread );

    Config* config = static_cast< Config* >( getConfig( ));
    osg::ref_ptr< osgUtil::IncrementalCompileOperation > ico =
        config->getIncrementalCompileOperation( );

    if( ico.valid( ))
    {
        const bool needViewerLock = ( getPipes( ).size( ) > 1 );
        lunchbox::ScopedWrite _mutex( needViewerLock ? &_viewer_lock : 0 );

        ico->addGraphicsContext( context );
    }
}

void Node::removeGraphicsContext( osg::GraphicsContext* context )
{
    LB_TS_NOT_THREAD( _nodeThread );

    Config* config = static_cast< Config* >( getConfig( ));
    osg::ref_ptr< osgUtil::IncrementalCompileOperation > ico =
        config->getIncrementalCompileOperation( );

    if( ico.valid( ))
    {
        const bool needViewerLock = ( getPipes( ).size( ) > 1 );
        lunchbox::ScopedWrite _mutex( needViewerLock ? &_viewer_lock : 0 );

        ico->removeGraphicsContext( context );
    }
}

void Node::addCameraToOSGView( const eq::uint128_t& id, osg::Camera* camera )
{
    LB_TS_NOT_THREAD( _nodeThread );

    Config* config = static_cast< Config* >( getConfig( ));

//LBINFO << "-----> Node::addCameraToView(" << id << ")" << std::endl;

    const bool needViewerLock = ( getPipes( ).size( ) > 1 );
    lunchbox::ScopedWrite _mutex( needViewerLock ? &_viewer_lock : 0 );
    bool needFrameStart = false;

    if( !_viewer.valid( ))
    {
        osg::ref_ptr< osgUtil::IncrementalCompileOperation > ico =
            config->getIncrementalCompileOperation( );

        _viewer = new CompositeViewer;
        // Just to make it *not* SingleThreaded
        _viewer->setThreadingModel( osgViewer::ViewerBase::ThreadPerContext );
        _viewer->setStartTick( osg::Timer::instance( )->getStartTick( ));
        if( ico.valid( ))
            _viewer->setIncrementalCompileOperation( ico );

        needFrameStart = true;
    }

    osg::ref_ptr< osgViewer::View > osgView = _viewer->findOSGViewByID( id );
    if( !osgView.valid( ))
    {
        osgView = config->takeOrCreateOSGView( id );

        _viewer->addView( osgView );
    }

    LBCHECK( osgView->addSlave( camera ));

    osg::ref_ptr< osgEarth::MapNode > mapNode =
        osgEarth::MapNode::findMapNode( osgView->getSceneData( ));
#if 0
    if( mapNode.valid( ) && mapNode->isGeocentric( ))
    {
        camera->addCullCallback(
            new osgEarth::Util::AutoClipPlaneCullCallback( mapNode ));
        camera->setComputeNearFarMode(
            osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES );
        camera->setNearFarRatio( 0.00002 );
    }
#else
    camera->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
#endif

    if( needFrameStart )
        _viewer->frameStart( _frameNumber, getFrameData( ));

//LBINFO << "<----- Node::addCameraToView(" << id << ")" << std::endl;
}

void Node::removeCameraFromOSGView( const eq::uint128_t& id,
        osg::Camera* camera )
{
    LB_TS_NOT_THREAD( _nodeThread );

    LBASSERT( _viewer.valid( ));

    const bool needViewerLock = ( getPipes( ).size( ) > 1 );
    lunchbox::ScopedWrite _mutex( needViewerLock ? &_viewer_lock : 0 );

    osg::ref_ptr< osgViewer::View > osgView = _viewer->findOSGViewByID( id );
    LBASSERT( osgView.valid( ));

    LBCHECK( osgView->removeSlave( osgView->findSlaveIndexForCamera( camera )));

    if( osgView->getNumSlaves( ) == 0 )
    {
        _viewer->removeView( osgView );

        static_cast< Config* >( getConfig( ))->releaseOSGView( osgView );
    }

    if( _viewer->getNumViews( ) == 0 )
    {
        _viewer->frameDrawFinish( );

        _viewer = 0;
    }
}

bool Node::configInit( const eq::uint128_t& initID )
{
LBINFO << "-----> Node::configInit(" << initID << ")" << std::endl;

    bool init = false;
    Config* config = static_cast< Config* >( getConfig( ));
    const InitData& initData = config->getInitData( );

    LBASSERT( !_viewer.valid( ));

    if( !eq::Node::configInit( initID ))
        goto out;

    // OSG is *not* multi-buffered
    setIAttribute( IATTR_THREAD_MODEL, eq::DRAW_SYNC );

    if( !config->mapInitData( initID ))
    {
        //setError( ERROR_EQEARTH_MAPOBJECT_FAILED );
        goto out;
    }

    if( !config->mapObject( &_frameData, initData.getFrameDataID( )))
    {
        //setError( ERROR_EQEARTH_MAPOBJECT_FAILED );
        goto out;
    }

    init = true;

out:
    if( !init )
        cleanup( );

LBINFO << "<----- Node::configInit(" << initID << ")" << std::endl;

    return init;
}

bool Node::configExit( )
{
    cleanup( );

    return eq::Node::configExit( );
}

void Node::frameStart( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
LBINFO << "-----> Node<" << getName( ) << ">::frameStart("
    << frameID << ", " << frameNumber << ")" << std::endl;

    _frameData.sync( frameID );
    _frameNumber = frameNumber;

    if( _viewer.valid( ))
    {
        LBASSERT( _viewer->getNumViews( ) > 0 );

        _viewer->frameStart( frameNumber, _frameData );
    }

    // aka "dispatch the rendering threads" - unlocks Channel::frameDraw!
    eq::Node::frameStart( frameID, frameNumber );

LBINFO << "<----- Node<" << getName( ) << ">::frameStart("
    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Node::frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
LBINFO << "-----> Node<" << getName( ) << ">::frameFinish("
    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Node::frameFinish( frameID, frameNumber );

LBINFO << "<----- Node<" << getName( ) << ">::frameFinish("
    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Node::frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
LBINFO << "-----> Node<" << getName( ) << ">::frameDrawFinish("
    << frameID << ", " << frameNumber << ")" << std::endl;

    if( _viewer.valid( ))
    {
        LBASSERT( _viewer->getNumViews( ) > 0 );

        _viewer->frameDrawFinish( );
    }

    eq::Node::frameDrawFinish( frameID, frameNumber );

LBINFO << "<----- Node<" << getName( ) << ">::frameDrawFinish("
    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Node::cleanup( )
{
    getConfig( )->unmapObject( &_frameData );

    LBASSERT( !_viewer.valid( ) || ( _viewer->getNumViews( ) == 0 ));

    _viewer = 0;
}

void Node::renderLocked( osgViewer::Renderer* renderer ) const
{
    LB_TS_NOT_THREAD( _nodeThread );

    LBASSERT( renderer );

    renderer->cull( );

    {
        const bool needViewerLock = ( getPipes( ).size( ) > 1 );
        lunchbox::ScopedWrite _mutex( needViewerLock ? &_viewer_lock : 0 );

        renderer->draw( );
    }
}
}
