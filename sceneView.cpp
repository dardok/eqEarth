#include "sceneView.h"

#include <osg/ContextData>

using namespace osg;
using namespace osgUtil;

namespace eqEarth
{
// ----------------------------------------------------------------------------

void SceneView::draw( )
{
    if( _camera->getNodeMask( ) == 0 )
        return;

    osg::State* state = _renderInfo.getState( );

    // we in theory should be able to be able to bypass reset, but we'll call it just incase.
    //_state->reset();
    state->setFrameStamp(_frameStamp.get());

    if (_displaySettings.valid())
    {
        state->setDisplaySettings(_displaySettings.get());
    }

    state->initializeExtensionProcs();

    osg::get<ContextData>(state->getContextID())->newFrame(state->getFrameStamp());

    if( !_initCalled )
        init( );

    // note, to support multi-pipe systems the deletion of OpenGL display list
    // and texture objects is deferred until the OpenGL context is the correct
    // context for when the object were originally created.  Here we know what
    // context we are in so can flush the appropriate caches.

    if( _requiresFlush )
    {
        double availableTime = 0.005;
        flushDeletedGLObjects( availableTime );
    }

    // assume the the draw which is about to happen could generate GL objects
    // that need flushing in the next frame.
    _requiresFlush = _automaticFlush;

    RenderLeaf* previous = NULL;

    if( 0 == ( _camera->getInheritanceMask() & DRAW_BUFFER ))
    {
        _renderStage->setDrawBuffer( _camera->getDrawBuffer( ));
        _renderStage->setReadBuffer( _camera->getDrawBuffer( ));
    }

    if( 0 == ( _camera->getInheritanceMask() & READ_BUFFER ))
    {
        _renderStage->setReadBuffer( _camera->getReadBuffer( ));
    }

    _localStateSet->setAttribute( getViewport( ));

    _localStateSet->setAttribute( _camera->getColorMask( ));

    _renderStage->setColorMask( _camera->getColorMask( ));

    // bog standard draw.
    _renderStage->drawPreRenderStages( _renderInfo, previous );
    _renderStage->draw( _renderInfo, previous );

    // re apply the defalt OGL state.
    state->popAllStateSets( );
    state->apply( );

#if 0
    if( _camera->getPostDrawCallback( ))
    {
        (*( _camera->getPostDrawCallback( )))( *_camera );
    }
#endif

    if ( state->getCheckForGLErrors( ) != State::NEVER_CHECK_GL_ERRORS )
    {
        if( state->checkGLErrors( "end of SceneView::draw( )" ))
        {
            // go into debug mode of OGL error in a fine grained way to help
            // track down OpenGL errors.
            state->setCheckForGLErrors( State::ONCE_PER_ATTRIBUTE );
        }
    }

// #define REPORT_TEXTURE_MANAGER_STATS
#ifdef REPORT_TEXTURE_MANAGER_STATS
    tom->reportStats( );
    bom->reportStats( );
#endif

    // osg::notify(osg::NOTICE)<<"SceneView  draw() DynamicObjectCount"<<getState()->getDynamicObjectCount()<<std::endl;
}
}
