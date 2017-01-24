#pragma once

#include <eq/eq.h>

#include <osg/Matrix>
#include <vmmlib/matrix.hpp>

#if EQ_VERSION_GE( 1, 8, 0 )
#include <eq/fabric/eventEnums.h>
#elif EQ_VERSION_GT( 1, 1, 0 )
#include <eq/client/event.h>
#else
#include <eq/event.h>
#endif
#include <osgGA/GUIEventAdapter>

#include <osgViewer/Viewer>
#include "renderer.h"

// ----------------------------------------------------------------------------

/**
 * Converts a matrix from the VMML library to a matrix of the OSG library.
 * @param matrix a vmml matrix.
 * @return the converted osg matrix.
 */
inline osg::Matrix vmmlToOsg( const eq::Matrix4f& matrix )
{
    return osg::Matrix( matrix( 0, 0 ), matrix( 1, 0 ),
                        matrix( 2, 0 ), matrix( 3, 0 ),
                        matrix( 0, 1 ), matrix( 1, 1 ),
                        matrix( 2, 1 ), matrix( 3, 1 ),
                        matrix( 0, 2 ), matrix( 1, 2 ),
                        matrix( 2, 2 ), matrix( 3, 2 ),
                        matrix( 0, 3 ), matrix( 1, 3 ),
                        matrix( 2, 3 ), matrix( 3, 3 ));
}

inline eq::Matrix4f osgToVmml( const osg::Matrix& matrix )
{
#if 1
    eq::Matrix4f M;

    M( 0,0 ) = matrix( 0,0 );
    M( 0,1 ) = matrix( 1,0 );
    M( 0,2 ) = matrix( 2,0 );
    M( 0,3 ) = matrix( 3,0 );

    M( 1,0 ) = matrix( 0,1 );
    M( 1,1 ) = matrix( 1,1 );
    M( 1,2 ) = matrix( 2,1 );
    M( 1,3 ) = matrix( 3,1 );

    M( 2,0 ) = matrix( 0,2 );
    M( 2,1 ) = matrix( 1,2 );
    M( 2,2 ) = matrix( 2,2 );
    M( 2,3 ) = matrix( 3,2 );

    M( 3,0 ) = matrix( 0,3 );
    M( 3,1 ) = matrix( 1,3 );
    M( 3,2 ) = matrix( 2,3 );
    M( 3,3 ) = matrix( 3,3 );

    return M;
#else
    eq::Matrix4f m;
    m.set( matrix.ptr( ), matrix.ptr( ) + 4 * 4, false );
    return m;
#endif
}

inline osg::Vec3d vmmlToOsg( const eq::Vector3f& vector )
{
    return osg::Vec3d( vector.x( ), vector.y( ), vector.z( ));
}

inline eq::Vector3f osgToVmml( const osg::Vec3d& vector )
{
    return eq::Vector3f( vector.x( ), vector.y( ), vector.z( ));
}

inline unsigned int eqButtonToOsg( uint32_t button )
{
    switch( button )
    {
        case eq::PTR_BUTTON1: return 1;
        case eq::PTR_BUTTON2: return 2;
        case eq::PTR_BUTTON3: return 3;
        case eq::PTR_BUTTON4: return 4;
        case eq::PTR_BUTTON5: return 5;
        case eq::PTR_BUTTON6: return 6;
        case eq::PTR_BUTTON7: return 7;
    }

    return 0;
}

// undo <linux/input.h> #defines
#ifdef EV_VERSION
#undef KEY_F1
#undef KEY_F2
#undef KEY_F3
#undef KEY_F4
#undef KEY_F5
#undef KEY_F6
#undef KEY_F7
#undef KEY_F8
#undef KEY_F9
#undef KEY_F10
#undef KEY_F11
#undef KEY_F12
#undef KEY_F13
#undef KEY_F14
#undef KEY_F15
#undef KEY_F16
#undef KEY_F17
#undef KEY_F18
#undef KEY_F19
#undef KEY_F20
#undef KEY_F21
#undef KEY_F22
#undef KEY_F23
#undef KEY_F24
#endif

inline int eqKeyToOsg( uint32_t key )
{
    int k;
    switch (key) {
        case eq::KC_ESCAPE: k = osgGA::GUIEventAdapter::KEY_Escape; break;
        case eq::KC_BACKSPACE: k = osgGA::GUIEventAdapter::KEY_BackSpace; break;
        case eq::KC_RETURN: k = osgGA::GUIEventAdapter::KEY_Return; break;
        case eq::KC_TAB: k = osgGA::GUIEventAdapter::KEY_Tab; break;
        case eq::KC_HOME: k = osgGA::GUIEventAdapter::KEY_Home; break;
        case eq::KC_LEFT: k = osgGA::GUIEventAdapter::KEY_Left; break;
        case eq::KC_UP: k = osgGA::GUIEventAdapter::KEY_Up; break;
        case eq::KC_RIGHT: k = osgGA::GUIEventAdapter::KEY_Right; break;
        case eq::KC_DOWN: k = osgGA::GUIEventAdapter::KEY_Down; break;
        case eq::KC_PAGE_UP: k = osgGA::GUIEventAdapter::KEY_Page_Up; break;
        case eq::KC_PAGE_DOWN: k = osgGA::GUIEventAdapter::KEY_Page_Down; break;
        case eq::KC_END: k = osgGA::GUIEventAdapter::KEY_End; break;
        case eq::KC_F1: k = osgGA::GUIEventAdapter::KEY_F1; break;
        case eq::KC_F2: k = osgGA::GUIEventAdapter::KEY_F2; break;
        case eq::KC_F3: k = osgGA::GUIEventAdapter::KEY_F3; break;
        case eq::KC_F4: k = osgGA::GUIEventAdapter::KEY_F4; break;
        case eq::KC_F5: k = osgGA::GUIEventAdapter::KEY_F5; break;
        case eq::KC_F6: k = osgGA::GUIEventAdapter::KEY_F6; break;
        case eq::KC_F7: k = osgGA::GUIEventAdapter::KEY_F7; break;
        case eq::KC_F8: k = osgGA::GUIEventAdapter::KEY_F8; break;
        case eq::KC_F9: k = osgGA::GUIEventAdapter::KEY_F9; break;
        case eq::KC_F10: k = osgGA::GUIEventAdapter::KEY_F10; break;
        case eq::KC_F11: k = osgGA::GUIEventAdapter::KEY_F11; break;
        case eq::KC_F12: k = osgGA::GUIEventAdapter::KEY_F12; break;
        case eq::KC_F13: k = osgGA::GUIEventAdapter::KEY_F13; break;
        case eq::KC_F14: k = osgGA::GUIEventAdapter::KEY_F14; break;
        case eq::KC_F15: k = osgGA::GUIEventAdapter::KEY_F15; break;
        case eq::KC_F16: k = osgGA::GUIEventAdapter::KEY_F16; break;
        case eq::KC_F17: k = osgGA::GUIEventAdapter::KEY_F17; break;
        case eq::KC_F18: k = osgGA::GUIEventAdapter::KEY_F18; break;
        case eq::KC_F19: k = osgGA::GUIEventAdapter::KEY_F19; break;
        case eq::KC_F20: k = osgGA::GUIEventAdapter::KEY_F20; break;
        case eq::KC_F21: k = osgGA::GUIEventAdapter::KEY_F21; break;
        case eq::KC_F22: k = osgGA::GUIEventAdapter::KEY_F22; break;
        case eq::KC_F23: k = osgGA::GUIEventAdapter::KEY_F23; break;
        case eq::KC_F24: k = osgGA::GUIEventAdapter::KEY_F24; break;
        case eq::KC_SHIFT_L: k = osgGA::GUIEventAdapter::KEY_Shift_L; break;
        case eq::KC_SHIFT_R: k = osgGA::GUIEventAdapter::KEY_Shift_R; break;
        case eq::KC_CONTROL_L: k = osgGA::GUIEventAdapter::KEY_Control_L; break;
        case eq::KC_CONTROL_R: k = osgGA::GUIEventAdapter::KEY_Control_R; break;
        case eq::KC_ALT_L: k = osgGA::GUIEventAdapter::KEY_Alt_L; break;
        case eq::KC_ALT_R: k = osgGA::GUIEventAdapter::KEY_Alt_R; break;
        default: k = key; break;
    }
    return k;
}
