#include "viewer.h"

#include "renderer.h"
#include "util.h"

#include <osg/Version>
#include <osgViewer/View>
#include <osg/DeleteHandler>

namespace eqEarth
{
// ----------------------------------------------------------------------------

class osgView : public osgViewer::View
{
public:
    osgView( const eq::uint128_t& id )
        : osgViewer::View( )
        , _id( id ) { }

    const eq::uint128_t& getID( ) const { return _id; }

protected:
    osg::GraphicsOperation* createRenderer( osg::Camera* camera );

private:
    const eq::uint128_t _id;
};

osg::GraphicsOperation* osgView::createRenderer( osg::Camera* camera )
{
    return new Renderer( camera );
}

// ----------------------------------------------------------------------------

void CompositeViewer::setGlobalContext( osg::GraphicsContext *context )
{
    Views views;
    getViews( views );

    for( Views::iterator vitr = views.begin( ); vitr != views.end( ); ++vitr)
    {
        osg::ref_ptr< osg::Camera > camera =
            static_cast< osgView* >( *vitr )->getCamera( );
        if( !camera->getGraphicsContext( ))
        {
            camera->setGraphicsContext( context );

            camera->setViewport( 0, 0,
                context->getTraits( )->width, context->getTraits( )->height );
        }
    }
}

osgViewer::View* CompositeViewer::createOSGView( const eq::uint128_t& id )
{
    return new osgView( id );
}

osgViewer::View* CompositeViewer::findOSGViewByID( const eq::uint128_t& id )
{
    Views views;
    getViews( views );

    for( Views::iterator vitr = views.begin( ); vitr != views.end( ); ++vitr)
    {
        osg::ref_ptr< osgView > v = static_cast< osgView* >( *vitr );
        if( v->getID( ) == id )
            return v.get( );
    }
    return NULL;
}

void CompositeViewer::advance( const uint32_t frameNumber,
        const FrameData& frameData )
{
    _frameStamp->setFrameNumber( frameNumber );
    // reference time has to use osg::Timer because other OSG internals that
    // attempt to compute relative time based on the frame stamp also use it
    // (e.g. IncrementalCompileOperation)
    _frameStamp->setReferenceTime( osg::Timer::instance( )->delta_s( _startTick,
        osg::Timer::instance( )->tick( )));
    _frameStamp->setSimulationTime( frameData.getSimulationTime( ));
    const time_t calendar = frameData.getCalendarTime( );
    struct tm now;
    if( NULL != gmtime_r( &calendar, &now ))
        _frameStamp->setCalendarTime( now );

#if OSG_VERSION_GREATER_OR_EQUAL( 3, 0, 0 )
    for( RefViews::iterator vitr = _views.begin( );
            vitr != _views.end( ); ++vitr )
    {
        osgViewer::View *view = vitr->get( );
        view->getEventQueue( )->frame( _frameStamp->getReferenceTime( ));
    }
#endif

    if( osg::Referenced::getDeleteHandler( ))
    {
        osg::Referenced::getDeleteHandler( )->flush( );
        osg::Referenced::getDeleteHandler( )->setFrameNumber( frameNumber );
    }
}

void CompositeViewer::frameStart( const uint32_t frameNumber,
        const FrameData& frameData )
{
//LBINFO << "-----> Viewer::frameStart(" << frameNumber << ")" << std::endl;

    advance( frameNumber, frameData );

    eventTraversal( );
    updateTraversal( );

    Scenes scenes;
    getScenes( scenes );

    for( Scenes::iterator sitr = scenes.begin( );
            sitr != scenes.end( ); ++sitr)
    {
        osgViewer::Scene* scene = *sitr;

        osg::ref_ptr< osgDB::DatabasePager > dp = scene ?
            scene->getDatabasePager( ) : 0;
        if( dp.valid( ))
            dp->signalBeginFrame( getViewerFrameStamp( ));

        osg::ref_ptr< osg::Node > sceneData = scene ?
            scene->getSceneData( ) : 0;
        if( sceneData.valid( ))
            sceneData->getBound( );
    }

//LBINFO << "<----- Viewer::frameStart(" << frameNumber << ")" << std::endl;
}

void CompositeViewer::frameDrawFinish( )
{
//LBINFO << "-----> Viewer::frameDrawFinish( )" << std::endl;

    Scenes scenes;
    getScenes( scenes );

    for( Scenes::iterator sitr = scenes.begin( );
            sitr != scenes.end( ); ++sitr )
    {
        osgViewer::Scene* scene = *sitr;

        osg::ref_ptr< osgDB::DatabasePager > dp = scene ?
            scene->getDatabasePager( ) : 0;
        if( dp.valid( ))
            dp->signalEndFrame( );
    }

    _requestRedraw = false;

//LBINFO << "<----- Viewer::frameDrawFinish( )" << std::endl;
}

void CompositeViewer::renderingTraversals( bool needMakeCurrentInThisThread )
{
    Contexts contexts;
    getContexts( contexts );

    bool doneMakeCurrentInThisThread = false;

    for( Contexts::iterator itr = contexts.begin( );
            itr != contexts.end( ); ++itr )
    {
        if(( *itr )->valid( ))
        {
            if( needMakeCurrentInThisThread )
            {
                doneMakeCurrentInThisThread = true;
                makeCurrent( *itr );
            }
            ( *itr )->runOperations( );
        }
    }

    if( doneMakeCurrentInThisThread )
        releaseContext( );
}

void CompositeViewer::realize( )
{
    LBASSERT( false );
}
}
