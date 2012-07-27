#include "node.h"

#include "config.h"
#include "error.h"
#include "util.h"
#include "pipe.h"

#include <osg/DeleteHandler>

namespace eqEarth
{
// ----------------------------------------------------------------------------

Node::Node( eq::Config* parent )
    : eq::Node( parent )
{
EQINFO << "=====> Node::Node(" << (void *)this << ")" << std::endl;
}

Node::~Node( )
{
EQINFO << "<===== Node::~Node(" << (void *)this << ")" << std::endl;
}

void Node::addGraphicsContext( osg::GraphicsContext *context )
{
    Config *config = static_cast< Config* >( getConfig( ));
    osg::ref_ptr< osgUtil::IncrementalCompileOperation > ico =
        config->getIncrementalCompileOperation( );

    if( ico.valid( ))
    {
        const bool needViewerLock = ( getPipes( ).size( ) > 1 );
        lunchbox::ScopedWrite _mutex( needViewerLock ? &_viewer_lock : 0 );

        ico->addGraphicsContext( context );
    }
}

void Node::removeGraphicsContext( osg::GraphicsContext *context )
{
    Config *config = static_cast< Config* >( getConfig( ));
    osg::ref_ptr< osgUtil::IncrementalCompileOperation > ico =
        config->getIncrementalCompileOperation( );

    if( ico.valid( ))
    {
        const bool needViewerLock = ( getPipes( ).size( ) > 1 );
        lunchbox::ScopedWrite _mutex( needViewerLock ? &_viewer_lock : 0 );

        ico->removeGraphicsContext( context );
    }
}

bool Node::addCameraToView( const eq::uint128_t& id, osg::Camera *camera )
{
    Config *config = static_cast< Config* >( getConfig( ));

//EQINFO << "-----> Node::addCameraToView(" << id << ")" << std::endl;

    const bool needViewerLock = ( getPipes( ).size( ) > 1 );
    lunchbox::ScopedWrite _mutex( needViewerLock ? &_viewer_lock : 0 );

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
    }

    osg::ref_ptr< osgViewer::View > osgView = _viewer->findOSGViewByID( id );
    if( !osgView.valid( ))
    {
        osgView = config->takeOrCreateOSGView( id );
        _viewer->addView( osgView );
    }

//EQINFO << "<----- Node::addCameraToView(" << id << ")" << std::endl;

    return osgView->addSlave( camera );
}

bool Node::removeCameraFromView( const eq::uint128_t& id, osg::Camera *camera )
{
    const bool needViewerLock = ( getPipes( ).size( ) > 1 );
    lunchbox::ScopedWrite _mutex( needViewerLock ? &_viewer_lock : 0 );

    osg::ref_ptr< osgViewer::View > osgView = _viewer->findOSGViewByID( id );
    EQASSERT( osgView.valid( ));

    bool ret =
        osgView->removeSlave( osgView->findSlaveIndexForCamera( camera ));

    if( osgView->getNumSlaves( ) == 0 )
    {
        _viewer->removeView( osgView );

        static_cast< Config* >( getConfig( ))->releaseOSGView( osgView );
    }

    if( _viewer->getNumViews( ) == 0 )
        _viewer = 0;

    return ret;
}

bool Node::configInit( const eq::uint128_t& initID )
{
EQINFO << "-----> Node::configInit(" << initID << ")" << std::endl;

    bool init = false;
    Config* config = static_cast< Config* >( getConfig( ));
    const InitData& initData = config->getInitData( );

    EQASSERT( !_viewer.valid( ));

    if( !eq::Node::configInit( initID ))
        goto out;

    // OSG is *not* multi-buffered
    setIAttribute( IATTR_THREAD_MODEL, eq::DRAW_SYNC );

    if( !config->mapInitData( initID ))
    {
        setError( ERROR_EQEARTH_MAPOBJECT_FAILED );
        goto out;
    }

    if( !config->mapObject( &_frameData, initData.getFrameDataID( )))
    {
        setError( ERROR_EQEARTH_MAPOBJECT_FAILED );
        goto out;
    }

    init = true;

out:
    if( !init )
        cleanup( );

EQINFO << "<----- Node::configInit(" << initID << ")" << std::endl;

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
//EQWARN << "-----> Node<" << getName( ) << ">::frameStart(" <<
//    << frameID << ", " << frameNumber << ")" << std::endl;

    _frameData.sync( frameID );

    if( _viewer.valid( ))
    {
        EQASSERT( _viewer->getNumViews( ) > 0 );

        _viewer->frameStart( frameNumber, _frameData );
    }

    // aka "dispatch the rendering threads" - unlocks Channel::frameDraw!
    eq::Node::frameStart( frameID, frameNumber );

//EQINFO << "<----- Node<" << getName( ) << ">::frameStart(" <<
//    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Node::frameFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//EQWARN << "-----> Node<" << getName( ) << ">::frameFinish(" <<
//    << frameID << ", " << frameNumber << ")" << std::endl;

    eq::Node::frameFinish( frameID, frameNumber );

//EQINFO << "<----- Node<" << getName( ) << ">::frameFinish(" <<
//    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Node::frameDrawFinish( const eq::uint128_t& frameID,
        const uint32_t frameNumber )
{
//EQWARN << "-----> Node<" << getName( ) << ">::frameDrawFinish(" <<
//    << frameID << ", " << frameNumber << ")" << std::endl;

    if( _viewer.valid( ))
    {
        EQASSERT( _viewer->getNumViews( ) > 0 );

        _viewer->frameDrawFinish( );
    }

    eq::Node::frameDrawFinish( frameID, frameNumber );

//EQINFO << "<----- Node<" << getName( ) << ">::frameDrawFinish(" <<
//    << frameID << ", " << frameNumber << ")" << std::endl;
}

void Node::cleanup( )
{
    getConfig( )->unmapObject( &_frameData );

    EQASSERT( !_viewer.valid( ) || ( _viewer->getNumViews( ) == 0 ));

    _viewer = 0;
}

void Node::renderLocked( osgViewer::Renderer* renderer ) const
{
#if 1
    renderer->cull( );

    const bool needRenderLock = ( getPipes( ).size( ) > 1 );
    lunchbox::ScopedWrite _mutex( needRenderLock ? &_render_lock : 0 );

    EQ_GL_CALL( renderer->draw( ));
#else
    const bool needRenderLock = ( getPipes( ).size( ) > 1 );
    lunchbox::ScopedWrite _mutex( needRenderLock ? &_render_lock : 0 );

    EQ_GL_CALL( renderer->cull_draw( ));
#endif
}
}
