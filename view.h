#pragma once

#include <eq/eq.h>

#include <osgViewer/View>

namespace eqEarth
{
class View : public eq::View
{
public:
    View( eq::Layout* parent );

protected:
    virtual ~View( );

public:
    void setSceneID( const eq::uint128_t& id );
    eq::uint128_t getSceneID( ) const { return _sceneID; }

    void setOverlayID( const eq::uint128_t& id );
    eq::uint128_t getOverlayID( ) const { return _overlayID; }

    void setViewMatrix( const eq::Matrix4f& viewMatrix );
    const eq::Matrix4f& getViewMatrix( ) const { return _viewMatrix; }

    void setNearFar( double near, double far );
    void getNearFar( double& near, double& far ) const;

    void setLatLon( double lat, double lon );
    void getLatLon( double& lat, double& lon ) const;

    void setWorldPointer( const eq::Vector3f& origin,
        const eq::Vector3f& direction );
    void getWorldPointer( eq::Vector3f& origin, eq::Vector3f& direction ) const;

    // AppNode only
    void setOSGView( osgViewer::View* osgView ) { _osgView = osgView; }
    osgViewer::View* getOSGView( ) { return _osgView; }
    const osgViewer::View* getOSGView( ) const { return _osgView; }

private:
    class Proxy : public co::Serializable
    {
    public:
        Proxy( View* view ) : _view( view ) {}

    protected:
        /** The changed parts of the view. */
        enum DirtyBits
        {
            DIRTY_SCENE   = co::Serializable::DIRTY_CUSTOM << 0,
            DIRTY_OVERLAY = co::Serializable::DIRTY_CUSTOM << 1,
            DIRTY_CAMERA  = co::Serializable::DIRTY_CUSTOM << 2,
            DIRTY_NEARFAR = co::Serializable::DIRTY_CUSTOM << 3,
            DIRTY_LATLON  = co::Serializable::DIRTY_CUSTOM << 4,
            DIRTY_POINTER = co::Serializable::DIRTY_CUSTOM << 5
        };

        virtual void serialize( co::DataOStream& os,
            const uint64_t dirtyBits );
        virtual void deserialize( co::DataIStream& is,
            const uint64_t dirtyBits );
        virtual void notifyNewVersion( ) { sync( ); }

    private:
        View* const _view;
        friend class View;
    };

    Proxy _proxy;
    friend class Proxy;
    eq::uint128_t _sceneID;
    eq::uint128_t _overlayID;
    eq::Matrix4f _viewMatrix;
    double _near, _far;
    double _lat, _lon;
    eq::Vector3f _origin;
    eq::Vector3f _direction;

    osg::ref_ptr< osgViewer::View > _osgView;
};
}
