#pragma once

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
virtual eq::Config* createConfig( eq::ServerPtr parent ) override
{ return new Config( parent ); }

virtual eq::Node* createNode( eq::Config* parent ) override
{ return new Node( parent ); }

virtual eq::Pipe* createPipe( eq::Node* parent ) override
{ return new Pipe( parent ); }

virtual eq::Window* createWindow( eq::Pipe* parent ) override
{ return new Window( parent ); }

virtual eq::Channel* createChannel( eq::Window* parent ) override
{ return new Channel( parent ); }

//virtual Observer* createObserver( Config* parent ) override;

//virtual Layout* createLayout( Config* parent ) override;

virtual eq::View* createView( eq::Layout* parent ) override
{ return new View( parent ); }

//virtual Canvas* createCanvas( Config* parent ) override;

//virtual Segment* createSegment( Canvas* parent ) override;
};
}
