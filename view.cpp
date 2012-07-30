#include "view.h"

namespace eqEarth
{
// ----------------------------------------------------------------------------

View::View( eq::Layout* parent )
    : eq::View( parent )
    , _proxy( this )
    , _sceneID( eq::UUID::ZERO )
    , _viewMatrix( eq::Matrix4d::IDENTITY )
    , _near( 0.01 ), _far( 100.0 )
    , _origin( eq::Vector3d::ZERO )
    , _direction( eq::Vector3d::ZERO )
{
EQINFO << "=====> View::View(" << (void *)this << ")" << std::endl;

    setUserData( &_proxy );
}

View::~View( )
{
    setUserData( 0 );

EQINFO << "<===== View::~View(" << (void *)this << ")" << std::endl;
}

void View::setSceneID( const eq::uint128_t& id )
{
    if( id != _sceneID )
    {
        _sceneID = id;
        _proxy.setDirty( Proxy::DIRTY_SCENE );
    }
}

void View::setViewMatrix( const eq::Matrix4d& viewMatrix )
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

void View::setWorldPointer( const eq::Vector3d& origin,
    const eq::Vector3d& direction )
{
    if(( origin != _origin ) || ( direction != _direction ))
    {
        _origin = origin;
        _direction = direction;
        _proxy.setDirty( Proxy::DIRTY_POINTER );
    }
}

void View::getWorldPointer( eq::Vector3d& origin,
    eq::Vector3d& direction ) const
{
    origin = _origin;
    direction = _direction;
}

void View::Proxy::serialize( co::DataOStream& os, const uint64_t dirtyBits )
{
    if( dirtyBits & DIRTY_SCENE )
        os << _view->_sceneID;
    if( dirtyBits & DIRTY_CAMERA )
        os << _view->_viewMatrix;
    if( dirtyBits & DIRTY_NEARFAR )
        os << _view->_near << _view->_far;
    if( dirtyBits & DIRTY_POINTER )
        os << _view->_origin << _view->_direction;
}

void View::Proxy::deserialize( co::DataIStream& is, const uint64_t dirtyBits )
{
    if( dirtyBits & DIRTY_SCENE )
        is >> _view->_sceneID;
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
    if( dirtyBits & DIRTY_POINTER )
    {
        is >> _view->_origin >> _view->_direction;
        if( isMaster( ))
            setDirty( DIRTY_POINTER ); // redistribute
    }
}
}
