#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/params/Params.h"
#include "cinder/Perlin.h"

#include "LiveParam.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace live;

struct Disk {
	Param<float>	mRadius;
	Param<Color>	mColor;
	vec2			mPos, mVel;
};

class DemoApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown( KeyEvent event ) override;
	
	Disk					mDisk;
	Perlin					mPerlin;
	Param<float>			mPerlinAmplitude, mPerlinSpeed, mPerlinScale, mFriction, mSpringK;
};

void DemoApp::setup()
{
	JsonBag::add( &mDisk.mRadius, "disk_radius",  []() { app::console() << "Updated disk radius!" << endl; } );
	JsonBag::add( &mDisk.mColor, "disk_color" );
	JsonBag::add( &mPerlinScale, "perlin_scale" );
	JsonBag::add( &mPerlinAmplitude, "perlin_amplitude" );
	JsonBag::add( &mPerlinSpeed, "perlin_speed" );
	JsonBag::add( &mFriction, "friction" );
	JsonBag::add( &mSpringK, "springk" );
	
	mDisk.mPos = vec2( app::getWindowSize() / 2 );
	
//	bag()->load(); //called by watchdog automatically
}

void DemoApp::update()
{
	float time = app::getElapsedSeconds();
	auto acc = mPerlinAmplitude() * vec2( mPerlin.dfBm( vec3( mPerlinScale() * vec2( mDisk.mPos.x, mDisk.mPos.y ), mPerlinSpeed * time ) ) );
	mDisk.mVel += acc + mSpringK() * ( vec2( app::getWindowSize() / 2 ) - mDisk.mPos );
	mDisk.mVel *= mFriction();
	mDisk.mPos += mDisk.mVel;
}

void DemoApp::draw()
{
	gl::clear( Color::gray( 0.5f ) );
	
	gl::ScopedColor col;
	gl::color( mDisk.mColor );
	gl::drawSolidCircle( mDisk.mPos, mDisk.mRadius );
}

void DemoApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_s ) {
		bag()->save();
	}
	else if( event.getCode() == KeyEvent::KEY_l ) {
		bag()->load();
	}
	else if( event.getCode() == KeyEvent::KEY_r ) {
		mDisk.mPos = vec2( app::getWindowSize() / 2 );
	}
}

CINDER_APP( DemoApp, RendererGl )
