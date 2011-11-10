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

    co::base::ScopedMutex<> mutex( _viewerlock );
    ico->addGraphicsContext( context );
}

void Node::removeGraphicsContext( osg::GraphicsContext *context )
{
    Config *config = static_cast< Config* >( getConfig( ));
    osg::ref_ptr< osgUtil::IncrementalCompileOperation > ico =
        config->getIncrementalCompileOperation( );

    co::base::ScopedMutex<> mutex( _viewerlock );
    ico->removeGraphicsContext( context );
}

bool Node::addCameraToView( const eq::uint128_t& id, osg::Camera *camera )
{
    Config *config = static_cast< Config* >( getConfig( ));

    co::base::ScopedMutex<> mutex( _viewerlock );
    if( !_viewer.valid( ))
    {
        osg::ref_ptr< osgUtil::IncrementalCompileOperation > ico =
            config->getIncrementalCompileOperation( );

        _viewer = new CompositeViewer;
	// Just to make it *not* SingleThreaded
        _viewer->setThreadingModel( osgViewer::ViewerBase::ThreadPerContext );
        _viewer->setStartTick( osg::Timer::instance( )->getStartTick( ));
        _viewer->setIncrementalCompileOperation( ico );
    }

    osg::ref_ptr< osgViewer::View > view = _viewer->findViewByID( id );
    if( !view.valid( ))
    {
        view = config->takeOrCreateView( id );
        _viewer->addView( view );
    }

    return view->addSlave( camera );
}

bool Node::removeCameraFromView( const eq::uint128_t& id, osg::Camera *camera )
{
    co::base::ScopedMutex<> mutex( _viewerlock );
    osg::ref_ptr< osgViewer::View > view = _viewer->findViewByID( id );
    EQASSERT( view.valid( ));

    bool ret = view->removeSlave( view->findSlaveIndexForCamera( camera ));

    if( view->getNumSlaves( ) == 0 )
    {
        _viewer->removeView( view );

        static_cast< Config* >( getConfig( ))->releaseView( view );
    }

    if( _viewer->getNumViews( ) == 0 )
        _viewer = 0;

    return ret;
}

bool Node::configInit( const eq::uint128_t& initID )
{
EQINFO << "-----> Node::configInit(" << initID << ")" << std::endl;

    bool init = false;

    EQASSERT( !_viewer.valid( ));

    if( eq::Node::configInit( initID ))
    {
        // OSG is *not* multi-buffered
        setIAttribute( IATTR_THREAD_MODEL, eq::DRAW_SYNC );

        Config* config = static_cast< Config* >( getConfig( ));
        if( !config->mapInitData( initID ))
            setError( ERROR_EQEARTH_MAPOBJECT_FAILED );
        else
        {
            const InitData& initData = config->getInitData( );
            if( !config->mapObject( &_frameData, initData.getFrameDataID( )))
                setError( ERROR_EQEARTH_MAPOBJECT_FAILED );
            else
            {
                init = true;
            }
        }
    }

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
//EQINFO << "-----> Node::frameStart(" << frameID << ", " << frameNumber << ")" << std::endl;

    _frameData.sync( frameID );

    if( _viewer.valid( ))
    {
        EQASSERT( _viewer->getNumViews( ) > 0 );

        _viewer->advance( frameNumber, _frameData );

        _viewer->frameStart( frameNumber, _frameData );
    }

    // aka "dispatch the rendering threads" - unlocks Channel::frameDraw!
    eq::Node::frameStart( frameID, frameNumber );

//EQINFO << "<----- Node::frameStart(" << frameID << ")" << std::endl;
}

void Node::frameFinish( const eq::uint128_t& frameID,
    const uint32_t frameNumber )
{
    if( _viewer.valid( ))
    {
        EQASSERT( _viewer->getNumViews( ) > 0 );

        _viewer->frameFinish( );
    }

    eq::Node::frameFinish( frameID, frameNumber );
}

void Node::cleanup( )
{
    getConfig( )->unmapObject( &_frameData );

    EQASSERT( !_viewer.valid( ) || ( _viewer->getNumViews( ) == 0 ));

    _viewer = 0;
}
}
