#pragma once

#include <eq/eq.h>

#include "initData.h"
#include "frameData.h"
#include "view.h"
#include "viewer.h"

#include <osg/Node>
#include <osgEarthUtil/Controls>

namespace eqEarth
{
class Config final : public eq::Config
{
public:
    explicit Config( eq::ServerPtr parent );

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

    bool handleEvent( eq::EventType type, const eq::KeyEvent& event ) override;
    bool handleEvent( eq::EventType type, const eq::PointerEvent& event ) override;

public:
    void setThreadHint( bool thread_hint ) { _thread_hint = thread_hint; }

    osgUtil::IncrementalCompileOperation*
        getIncrementalCompileOperation( ) const { return _ico.get( ); }

    osgViewer::View* takeOrCreateOSGView( const eq::uint128_t& sceneID );
    void releaseOSGView( osgViewer::View* view );

    void createOverlay( osgEarth::Util::Controls::ControlCanvas* cc,
            eq::View* view );

#if 0
    CompositeViewer *getViewer( ) { return _viewer; }
    const CompositeViewer *getViewer( ) const { return _viewer; }
#endif

    void setZMode( bool zmode );
    void setZValue( float zvalue );

private:
    osg::Group* getScene( const eq::uint128_t& sceneID, osgViewer::View* view );

    friend class Window;
    void clearScene( ); // Must call with GL context (i.e. Window::configExitGL)

    void cleanup( );

protected:
    InitData _initData;
    FrameData _frameData;

    osg::ref_ptr< osg::Group > _scene;

    osg::Uniform *_zmode;
    osg::Uniform *_zvalue;

    osg::ref_ptr< osgUtil::IncrementalCompileOperation > _ico;
    osg::ref_ptr< osgDB::DatabasePager > _pager;

    lunchbox::Lock _viewer_lock;
    osg::ref_ptr< CompositeViewer > _viewer;
    osg::ref_ptr< osgGA::EventQueue > _eventQueue;

    osg::ref_ptr< osg::GraphicsContext > _gc;
    bool _thread_hint;

    uint32_t _appRenderTick;

    bool _shutdown;

private:
    View* selectCurrentView( const eq::uint128_t& viewID );
    void handleMouseEvent( eq::EventType type, const eq::PointerEvent& event, View* view,
            double time );
#if 0
    void updateCurrentWorldPointer( const eq::ConfigEvent& event );
#endif

    bool appInitGL( bool pbuffer = true );

    struct ViewCollector;
};
}
