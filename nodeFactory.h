#pragma once

#define EQ_IGNORE_GLEW
#include <eq/eq.h>

#include "config.h"
#include "node.h"
#include "pipe.h"
#include "window.h"
#include "channel.h"
#include "view.h"

namespace eqEarth
{
class NodeFactory : public eq::NodeFactory
{
public:
    virtual eq::Config* createConfig( eq::ServerPtr parent )
    { return new Config( parent ); }

    virtual eq::Node* createNode( eq::Config* parent )
    { return new Node( parent ); }

    virtual eq::View* createView( eq::Layout* parent )
    { return new View( parent ); }

    virtual eq::Pipe* createPipe( eq::Node* parent )
    { return new Pipe( parent ); }

    virtual eq::Window* createWindow( eq::Pipe* parent )
    { return new Window( parent ); }

    virtual eq::Channel* createChannel( eq::Window* parent )
    { return new Channel( parent ); }
};
}
