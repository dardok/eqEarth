#include "earthManipulator.h"

namespace eqEarth
{
// ----------------------------------------------------------------------------

EarthManipulator::EarthManipulator( )
{
    overrideDefaultSettings( );
}

EarthManipulator::~EarthManipulator( )
{
}

void EarthManipulator::overrideDefaultSettings( )
{
    //osg::ref_ptr< Settings > settings = new Settings( );
    osg::ref_ptr< Settings > settings = getSettings( );

    // install default action bindings:
    ActionOptions options;

    settings->bindKey( ACTION_HOME, osgGA::GUIEventAdapter::KEY_Space );

    // pan as you hold the left button:
    options.clear( );
    settings->bindMouse( ACTION_PAN,
        osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON, 0L, options);

    // zoom as you hold the right button:
    options.clear( );
    options.add( OPTION_CONTINUOUS, true );
    settings->bindMouse( ACTION_ZOOM,
        osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON, 0L, options );

    // rotate with either the middle button or the left+right buttons:
    options.clear( );
    settings->bindMouse( ACTION_ROTATE,
        osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON, 0L, options );
    settings->bindMouse( ACTION_ROTATE,
        osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON |
        osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON );

    // zoom with the scroll wheel:
    settings->bindScroll( ACTION_ZOOM, osgGA::GUIEventAdapter::SCROLL_2D );
    settings->bindScroll( ACTION_ZOOM_IN, osgGA::GUIEventAdapter::SCROLL_DOWN );
    settings->bindScroll( ACTION_ZOOM_OUT, osgGA::GUIEventAdapter::SCROLL_UP );

    // pan around with arrow keys:
    settings->bindKey( ACTION_PAN_LEFT, osgGA::GUIEventAdapter::KEY_Left );
    settings->bindKey( ACTION_PAN_RIGHT, osgGA::GUIEventAdapter::KEY_Right );
    settings->bindKey( ACTION_PAN_UP, osgGA::GUIEventAdapter::KEY_Up );
    settings->bindKey( ACTION_PAN_DOWN, osgGA::GUIEventAdapter::KEY_Down );
    
    // double click the left button (or CTRL-left button) to zoom in on a point:
    options.clear( );
    options.add( OPTION_GOTO_RANGE_FACTOR, 0.4 );
    settings->bindMouseDoubleClick( ACTION_GOTO,
        osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON, 0L, options );
    settings->bindMouseClick( ACTION_GOTO,
        osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON,
        osgGA::GUIEventAdapter::MODKEY_CTRL, options );
    
    // double click the right button (or CTRL-right button) to zoom out to a point
    options.clear( );
    options.add( OPTION_GOTO_RANGE_FACTOR, 2.5 );
    settings->bindMouseDoubleClick( ACTION_GOTO,
        osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON, 0L, options );
    settings->bindMouseClick( ACTION_GOTO,
        osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON,
        osgGA::GUIEventAdapter::MODKEY_CTRL, options );

    settings->setThrowingEnabled( true );
    settings->setLockAzimuthWhilePanning( true );
    settings->setArcViewpointTransitions( true );
    settings->setMouseSensitivity( 2.0 );

    //applySettings( settings );
}
}
