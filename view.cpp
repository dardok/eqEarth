#include "view.h"

namespace eqEarth
{
// ----------------------------------------------------------------------------

View::View( eq::Layout* parent )
    : eq::View( parent )
    , _proxy( this )
    , _sceneID( 0 )
    , _overlayID( 0 )
    , _viewMatrix( eq::Matrix4f( ))
    , _near( 0.01 ), _far( 100.0 )
    , _lat( 0.0 ), _lon( 0.0 )
    , _origin( )
    , _direction( )
{
LBINFO << "=====> View::View(" << (void *)this << ")" << std::endl;

    setUserData( &_proxy );
}

View::~View( )
{
    setUserData( 0 );

LBINFO << "<===== View::~View(" << (void *)this << ")" << std::endl;
}

void View::setSceneID( const eq::uint128_t& id )
{
    if( id != _sceneID )
    {
        _sceneID = id;
        _proxy.setDirty( Proxy::DIRTY_SCENE );
    }
}

void View::setOverlayID( const eq::uint128_t& id )
{
    if( id != _overlayID )
    {
        _overlayID = id;
        _proxy.setDirty( Proxy::DIRTY_OVERLAY );
    }
}

void View::setViewMatrix( const eq::Matrix4f& viewMatrix )
{
    _viewMatrix = viewMatrix;
    _proxy.setDirty( Proxy::DIRTY_CAMERA );
}

void View::setNearFar( double near, double far )
{
    if(( near != _near ) || ( far != _far ))
    {
        _near = near;
        _far = far;
        _proxy.setDirty( Proxy::DIRTY_NEARFAR );
    }
}

void View::getNearFar( double& near, double& far ) const
{
    near = _near;
    far = _far;
}

void View::setLatLon( double lat, double lon )
{
    if(( lat != _lat ) || ( lon != _lon ))
    {
        _lat = lat;
        _lon = lon;
        _proxy.setDirty( Proxy::DIRTY_LATLON );
    }
}

void View::getLatLon( double& lat, double& lon ) const
{
    lat = _lat;
    lon = _lon;
}

void View::setWorldPointer( const eq::Vector3f& origin,
    const eq::Vector3f& direction )
{
    if(( origin != _origin ) || ( direction != _direction ))
    {
        _origin = origin;
        _direction = direction;
        _proxy.setDirty( Proxy::DIRTY_POINTER );
    }
}

void View::getWorldPointer( eq::Vector3f& origin,
    eq::Vector3f& direction ) const
{
    origin = _origin;
    direction = _direction;
}

void View::Proxy::serialize( co::DataOStream& os, const uint64_t dirtyBits )
{
    if( dirtyBits & DIRTY_SCENE )
        os << _view->_sceneID;
    if( dirtyBits & DIRTY_OVERLAY )
        os << _view->_overlayID;
    if( dirtyBits & DIRTY_CAMERA )
        os << _view->_viewMatrix;
    if( dirtyBits & DIRTY_NEARFAR )
        os << _view->_near << _view->_far;
    if( dirtyBits & DIRTY_LATLON )
        os << _view->_lat << _view->_lon;
    if( dirtyBits & DIRTY_POINTER )
        os << _view->_origin << _view->_direction;
}

void View::Proxy::deserialize( co::DataIStream& is, const uint64_t dirtyBits )
{
    if( dirtyBits & DIRTY_SCENE )
        is >> _view->_sceneID;
    if( dirtyBits & DIRTY_OVERLAY )
        is >> _view->_overlayID;
    if( dirtyBits & DIRTY_CAMERA )
    {
        is >> _view->_viewMatrix;
        if( isMaster( ))
            setDirty( DIRTY_CAMERA ); // redistribute
    }
    if( dirtyBits & DIRTY_NEARFAR )
    {
        is >> _view->_near >> _view->_far;
        if( isMaster( ))
            setDirty( DIRTY_NEARFAR ); // redistribute
    }
    if( dirtyBits & DIRTY_LATLON )
    {
        is >> _view->_lat >> _view->_lon;
        if( isMaster( ))
            setDirty( DIRTY_LATLON ); // redistribute
    }
    if( dirtyBits & DIRTY_POINTER )
    {
        is >> _view->_origin >> _view->_direction;
        if( isMaster( ))
            setDirty( DIRTY_POINTER ); // redistribute
    }
}
}
