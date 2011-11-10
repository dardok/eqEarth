#pragma once

#define EQ_IGNORE_GLEW
#include <eq/eq.h>

#include "initData.h"
#include "frameData.h"
#include "view.h"
#include "viewer.h"
#include "controls.h"

#include <osg/Node>

namespace eqEarth
{
class Config : public eq::Config
{
public:
    Config( eq::ServerPtr parent );

protected:
    virtual ~Config( );

public:
    virtual bool init( );
    virtual bool exit( );

    virtual uint32_t startFrame( );
    virtual uint32_t finishFrame( );

    void setInitData( const InitData& initData ) { _initData = initData; }
    const InitData& getInitData( ) const { return _initData; }
    bool mapInitData( const eq::uint128_t& initDataID );

    virtual bool handleEvent( const eq::ConfigEvent* event );

public:
    CompositeViewer *getViewer( ) { return _viewer; }
    const CompositeViewer *getViewer( ) const { return _viewer; }

    osg::Node* getScene( const eq::uint128_t& sceneID );

    osgViewer::View* takeOrCreateView( const eq::uint128_t& sceneID );
    void releaseView( osgViewer::View* view );

private:
    void cleanup( );

protected:
    InitData _initData;
    FrameData _frameData;

    osg::ref_ptr< osg::Node > _scene;
    
    osg::ref_ptr< osgDB::DatabasePager > _pager;
    osg::ref_ptr< osgUtil::IncrementalCompileOperation > _ico;

    osg::ref_ptr< CompositeViewer > _viewer;
    osg::ref_ptr< osgGA::EventQueue > _eventQueue;

    osg::ref_ptr< osg::GraphicsContext > _gc;
    uint32_t _appRenderTick;

public:
    osgUtil::IncrementalCompileOperation*
        getIncrementalCompileOperation( ) const { return _ico; }
    osgDB::DatabasePager* getDatabasePager( ) const { return _pager; }

private:
    View* selectCurrentView( const eq::uint128_t& viewID );
    void handleMouseEvent( const eq::ConfigEvent* event );
    void updateCurrentWorldPointer( const eq::ConfigEvent* event );

    bool appInitGL( bool pbuffer = true );
};
}
