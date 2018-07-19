#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class cinderPixelPusherExampleApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void cinderPixelPusherExampleApp::setup()
{
}

void cinderPixelPusherExampleApp::mouseDown( MouseEvent event )
{
}

void cinderPixelPusherExampleApp::update()
{
}

void cinderPixelPusherExampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( cinderPixelPusherExampleApp, RendererGl )
