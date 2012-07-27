#include "renderer.h"
#include "sceneView.h"

#include <osgViewer/View>
#include <osgDB/DatabasePager>

namespace eqEarth
{
// ----------------------------------------------------------------------------

Renderer::Renderer( osg::Camera* camera )
    : osgViewer::Renderer( camera )
{
    _availableQueue.takeFront( );
    _availableQueue.takeFront( );

    _sceneView[0] = new SceneView;
    _sceneView[1] = new SceneView;

    osg::Camera* masterCamera = _camera->getView( ) ?
        _camera->getView( )->getCamera( ) : camera;

    osg::StateSet* global_stateset = 0;
    osg::StateSet* secondary_stateset = 0;
    if( _camera != masterCamera )
    {
        global_stateset = masterCamera->getOrCreateStateSet( );
        secondary_stateset = _camera->getStateSet( );
    }
    else
    {
        global_stateset = _camera->getOrCreateStateSet( );
    }

    osgViewer::View* view =
        dynamic_cast< osgViewer::View* >( _camera->getView( ));
    osgViewer::ViewerBase* viewer = view ? view->getViewerBase( ) : 0;
    osgUtil::IncrementalCompileOperation* ico =
        viewer ? viewer->getIncrementalCompileOperation( ) : 0;
    bool automaticFlush = ( ico == NULL );

    osg::DisplaySettings* ds = _camera->getDisplaySettings( ) ?
        _camera->getDisplaySettings( ) :
        (( view && view->getDisplaySettings( )) ?
            view->getDisplaySettings( ) :
                osg::DisplaySettings::instance( ).get( ));

    _sceneView[0]->setAutomaticFlush( automaticFlush );
    _sceneView[0]->setGlobalStateSet( global_stateset );
    _sceneView[0]->setSecondaryStateSet( secondary_stateset );

    _sceneView[1]->setAutomaticFlush( automaticFlush );
    _sceneView[1]->setGlobalStateSet( global_stateset);
    _sceneView[1]->setSecondaryStateSet( secondary_stateset );

    _sceneView[0]->setDefaults( SceneView::COMPILE_GLOBJECTS_AT_INIT );
    _sceneView[1]->setDefaults( SceneView::COMPILE_GLOBJECTS_AT_INIT );

    _sceneView[0]->setDisplaySettings( ds );
    _sceneView[1]->setDisplaySettings( ds );

    _sceneView[0]->setCamera( _camera.get( ), false );
    _sceneView[1]->setCamera( _camera.get( ), false );

    _availableQueue.add( _sceneView[0].get( ));
    _availableQueue.add( _sceneView[1].get( ));

    //setGraphicsThreadDoesCull( false );
}

void Renderer::operator( )( osg::GraphicsContext* context )
{
    // cull/draw are called manually, don't let GraphicsContext::runOperations
    // (called in Window::frameFinish) run them
}
}
