#pragma once

#include <osgUtil/SceneView>

namespace eqEarth
{
class SceneView : public osgUtil::SceneView
{
protected:
    virtual void draw() override;
};
}
