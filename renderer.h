#pragma once

#include <osgViewer/Renderer>

namespace eqEarth
{
class Renderer : public osgViewer::Renderer
{
public:
    explicit Renderer( osg::Camera* camera );

    virtual void operator( )( osg::GraphicsContext* context );
};
}
