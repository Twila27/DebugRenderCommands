#include "Game/TheGame.hpp"


#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Renderer/Texture.hpp" //For the textured DrawAABB call in Render2D().
#include "Engine/Input/TheInput.hpp" //For input polling.
#include "Engine/Audio/TheAudio.hpp"
#include "Engine/String/StringUtils.hpp"

#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp" 

//Be sure to #include all forward declared classes' .hpp's.
#include "Game/Entity2D.hpp" //And whatever represents player.
#include "Game/Camera3D.hpp"

#include <cstdlib>
#include <math.h>


//-----------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------
//Define class constants.
//STATIC const int TheGame::INITIAL_NUM_ASTEROIDS = 6; //Example, used in constructor.


//-----------------------------------------------------------------------------
TheGame::TheGame( )
	: m_playerCamera( new Camera3D( CAMERA_DEFAULT_POSITION ) )
{
	//Remember to initialize any pointers to initial values or to nullptr in here.
	// for ( int i = 0; i < TheGame::INITIAL_NUM_ASTEROIDS; i++ ) m_asteroids[ i ] = new Asteroid(...);
	// for ( int i = 0; i < MAX_NUMBER_OF_BULLETS; i++ ) m_bullets[ i ] = nullptr; //Etc.

	for ( int i = 0; i < 10; i++ ) {
		m_entities.push_back( new Entity2D( "" ) );
		m_entities[ i ]->SetPosition( Vector2( (float)( i * 72 ), (float)( i * 72 ) ) );
	}

	//Position player representation if camera isn't translated to player (i.e. if camera is fixed in world).
	//m_ship = new Ship( static_cast<float>( screenWidth ) / 2.f, static_cast<float>( screenHeight ) / 2.f ); 
}


//-----------------------------------------------------------------------------
TheGame::~TheGame( )
{
	delete m_playerCamera;
	//delete m_ship; //Whatever represents player.
	//for ( int i = 0; i < m_numBulletsAllocated; i++ ) delete m_bullets[ i ]; //Etc.
}


//-----------------------------------------------------------------------------
void TheGame::UpdateFromKeyboard( float deltaSeconds )
{
	//Super speed handling.
	float speed = 0.f;
	if ( g_theInput->IsKeyDown( VK_SHIFT ) )
		speed = FLYCAM_SPEED_SCALAR;
	else speed = 1.f;


	float deltaMove = ( speed * deltaSeconds ); //speed is then in blocks per second!

	Vector3 camForwardXY = m_playerCamera->GetForwardXY();
	Vector3 camLeftXY = m_playerCamera->GetLeftXY();

	if ( g_theInput->IsKeyDown( 'W' ) )
		m_playerCamera->m_worldPosition += camForwardXY * deltaMove;

	if ( g_theInput->IsKeyDown( 'S' ) )
		m_playerCamera->m_worldPosition -= camForwardXY * deltaMove;

	if ( g_theInput->IsKeyDown( 'A' ) )
		m_playerCamera->m_worldPosition += camLeftXY * deltaMove;

	if ( g_theInput->IsKeyDown( 'D' ) )
		m_playerCamera->m_worldPosition -= camLeftXY * deltaMove;

	if ( g_theInput->IsKeyDown( VK_SPACE ) )
		m_playerCamera->m_worldPosition.z += deltaMove; //Scaled by z-axis, so *1.0f.

	if ( g_theInput->IsKeyDown( 'Z' ) || g_theInput->IsKeyDown( 'X' ) || g_theInput->IsKeyDown( 'C' ) )
		m_playerCamera->m_worldPosition.z -= deltaMove; //Scaled by z-axis, so *1.0f.
}


//-----------------------------------------------------------------------------
void TheGame::UpdateFromMouse()
{
	const float mouseSensitivityYaw = 0.044f;
	m_playerCamera->m_orientation.m_yawDegreesAboutZ -= mouseSensitivityYaw * (float)g_theInput->GetCursorDeltaX();
	m_playerCamera->m_orientation.m_pitchDegreesAboutY += mouseSensitivityYaw * (float)g_theInput->GetCursorDeltaY();
	m_playerCamera->FixAndClampAngles();
}


//-----------------------------------------------------------------------------
void TheGame::UpdateCamera( float deltaSeconds )
{
	UpdateFromKeyboard( deltaSeconds );
	UpdateFromMouse();
}


//-----------------------------------------------------------------------------
float g_counter = 0.f;
void TheGame::Update( float deltaSeconds )
{
	g_counter++; //For 'animations'.
	g_counter = WrapNumberWithinCircularRange( g_counter, 0.f, 360.f );

	//m_ship->Update( deltaSeconds ); //Update player (or world!) representation.
	UpdateCamera( deltaSeconds );


	if ( g_theInput->WasKeyPressedOnce( KEY_TO_TOGGLE_DEBUG_INFO ) ) 
		g_renderDebugInfo = !g_renderDebugInfo;

	if ( g_theInput->IsKeyDown( 'A' ) && g_theInput->WasKeyJustPressed( 'A' ) ) 
		g_counter += 10.f;

	for ( auto entityIter = m_entities.begin(); entityIter != m_entities.end(); ++entityIter )
	{
		Entity2D* currentEntity2D = *entityIter;
		currentEntity2D->Update( deltaSeconds );
	}


	//-----------------------------------------------------------------------------
	//Engine updates should go here until a clear word comes back about creating an engine-side master update call.
	UpdateDebugCommands( deltaSeconds );
}


//-----------------------------------------------------------------------------
void TheGame::SetUpPerspectiveProjection()
{
	float aspect = ( 16.f / 9.f ); //Not speed-critical /'s because const expression handled during compilation.
	//float fovDegreesHorizontal = 180.f; //Lowering me zooms in! Raising me produces fisheye effect!
	float fovDegreesVertical = 60.f; // was ( fovDegreesHorizontal / aspect );
	float zNear = .1f;
	float zFar = 1000.f;

	g_theRenderer->SetPerspective( fovDegreesVertical, aspect, zNear, zFar ); // The math for how this works is very nuanced and above this class, will be in SD3. 
}


//-----------------------------------------------------------------------------
void TheGame::ApplyCameraTransform() //FIX AXES-BASIS AS PER GAME REQUIREMENTS HERE!
{
	//Change of basis detour application to move from OGL (y-up right-handed world-not-screen) to our coordinate system (z-up right-handed world) orientation.
	g_theRenderer->RotateViewByDegrees( -90.f, Vector3( 1.f, 0.f, 0.f ) ); //Rotate z-forward to z-up, a negative rotation about the x-axis.
	g_theRenderer->RotateViewByDegrees( 90.f, Vector3( 0.f, 0.f, 1.f ) ); //Rotate x-right to x-forward, a positive rotation about the z-axis.

	//Anti-rotation. (MP1 Recap: transposing rotation == inverse... so why does negating work?)
	g_theRenderer->RotateViewByDegrees( -1.f * m_playerCamera->m_orientation.m_rollDegreesAboutX, Vector3( 1.f, 0.f, 0.f ) ); //Anti-roll.
	g_theRenderer->RotateViewByDegrees( -1.f * m_playerCamera->m_orientation.m_pitchDegreesAboutY, Vector3( 0.f, 1.f, 0.f ) ); //Anti-pitch.
	g_theRenderer->RotateViewByDegrees( -1.f * m_playerCamera->m_orientation.m_yawDegreesAboutZ, Vector3( 0.f, 0.f, 1.f ) ); //Anti-yaw.

	//Anti-translation. (MP1 Recap: negating translation == inverse!)
	g_theRenderer->TranslateView( m_playerCamera->m_worldPosition * -1.f );
}


//-----------------------------------------------------------------------------
void TheGame::SetupView3D()
{
	g_theRenderer->ClearScreenToColor( Rgba::BLACK );
	g_theRenderer->ClearScreenDepthBuffer();

	SetUpPerspectiveProjection();

	ApplyCameraTransform();

	g_theRenderer->EnableDepthTesting( true );
	g_theRenderer->EnableBackfaceCulling( false );
	//g_theRenderer->EnableAlphaTesting( true );
}


//-----------------------------------------------------------------------------
void TheGame::Render3D() //Look below in RenderDebug3D for RenderCommand use.
{
	//Render 3D world/map/scene or player representation.

	//Render any 3D overlays.
}


//-----------------------------------------------------------------------------
void TheGame::RenderDebug3D() //RenderCommands from Engine, etc.
{
	float x = CosDegrees( g_counter );
	float y = SinDegrees( g_counter );
	Vector3 movingCirclePos = Vector3( x, y, 0.f );
	Vector3 camPos = m_playerCamera->m_worldPosition;

	//Box.
	AABB3 bounds;
	bounds.mins = Vector3( 4.f, 4.f, 4.f );
	bounds.maxs = Vector3( 5.f, 5.f, 5.f );
	AddDebugRenderCommand( new RenderCommandAABB3( bounds, true, 0.f, DepthMode::DEPTH_TEST_ON, 4.f, Rgba::CYAN, Rgba::GRAY ) );

	//Ball.
	AddDebugRenderCommand( new RenderCommandSphere( Vector3::ZERO, 1.f, 0.f, DepthMode::DEPTH_TEST_OFF, Rgba::WHITE, 4.f ) );

	//Axes.
	AddDebugRenderCommand( new RenderCommandBasis( Vector3::ZERO, 5.f, true, 0.f, DEPTH_TEST_DUAL, 1.f ) );

	//Lines and arrows and points.
	Vector3 lineStart = Vector3( -5.f, -5.f, -5.f );
	Vector3 lineEnd = Vector3( -6.f, -6.f, -6.f );
	AddDebugRenderCommand( new RenderCommandLine( lineStart, lineEnd, 0.f, DepthMode::DEPTH_TEST_DUAL, Rgba::CYAN, 4.f ) );
	Vector3 arrowStart = Vector3( 0.f, 0.f, 0.f );
	Vector3 arrowEnd = Vector3( -4.f, -4.f, -4.f );
	AddDebugRenderCommand( new RenderCommandArrow( arrowStart, arrowEnd - movingCirclePos, 0.f, DepthMode::DEPTH_TEST_DUAL, Rgba::YELLOW, 4.f ) );
	AddDebugRenderCommand( new RenderCommandPoint( movingCirclePos, 5.f, DepthMode::DEPTH_TEST_DUAL, Rgba::MAGENTA, 4.f ) );
	AddDebugRenderCommand( new RenderCommandPoint( arrowEnd - Vector3::ONE - movingCirclePos, 0.f, DepthMode::DEPTH_TEST_DUAL, Rgba::MAGENTA, 4.f ) );

	RenderThenExpireDebugCommands();
}


//-----------------------------------------------------------------------------
void TheGame::SetupView2D()
{
	g_theRenderer->EnableDepthTesting( false );
	g_theRenderer->EnableBackfaceCulling( false );

	g_theRenderer->SetOrtho( Vector2( 0.f, 0.f ),
							 Vector2( (float)g_theApp->GetScreenWidth(), (float)g_theApp->GetScreenHeight() ) );
}


//-----------------------------------------------------------------------------
void TheGame::Render2D()
{
	//Render 2D world/map/scene or player representation, and HUD.

	for ( auto entityIter = m_entities.begin(); entityIter != m_entities.end(); ++entityIter )
	{
		Entity2D* currentEntity2D = *entityIter;
		currentEntity2D->Render();
	}

	g_theRenderer->DrawAABB( TheRenderer::VertexGroupingRule::AS_QUADS, AABB2( 1000.f, 200.f, 1400.f, 600.f ), *Texture::CreateOrGetTexture( "Data/Images/Paused.png" ) ); //Uncomment #include Texture above to use.

	g_theRenderer->DrawPolygon( TheRenderer::VertexGroupingRule::AS_LINE_LOOP, Vector2( 100.f, 100.f ), 50.f, 20.f, 90.f );

	//g_theRenderer->SetOrtho( Vector2( 0.f, 0.f ), Vector2( 800.f, 450.f ) ); //Half-sizing the viewport to show how rendering to two scales things. Should go in SetupView2D.

	g_theRenderer->DrawAABB( TheRenderer::VertexGroupingRule::AS_LINE_LOOP, AABB2( 100.f + g_counter, 100.f, 400.f + g_counter, 400.f ), Rgba( 0.1f, 0.3f, 0.9f, 0.5f ) ); //Takes up half the height due to ortho call.

}


//-----------------------------------------------------------------------------
void TheGame::RenderDebug2D()
{
	RenderReticle(); //May later be considered part of HUD and moved to Render2D().

	RenderLeftDebugText2D();
	RenderRightDebugText2D();
}


//-----------------------------------------------------------------------------
void TheGame::RenderReticle()
{
	Vector2 screenCenter = g_theApp->GetScreenCenter();

	Vector2 crosshairLeft = Vector2( screenCenter.x - HUD_CROSSHAIR_RADIUS, screenCenter.y );
	Vector2 crosshairRight = Vector2( screenCenter.x + HUD_CROSSHAIR_RADIUS, screenCenter.y );
	Vector2 crosshairUp = Vector2( screenCenter.x, screenCenter.y - HUD_CROSSHAIR_RADIUS );
	Vector2 crosshairDown = Vector2( screenCenter.x, screenCenter.y + HUD_CROSSHAIR_RADIUS );

	g_theRenderer->SetBlendFunc( 0x0307, 0 ); //GL CONSTANTS. TODO: BE SMARTER. WHERE TO PUT?
	g_theRenderer->DrawLine( crosshairLeft, crosshairRight, Rgba(), Rgba(), HUD_CROSSHAIR_THICKNESS );
	g_theRenderer->DrawLine( crosshairUp, crosshairDown, Rgba(), Rgba(), HUD_CROSSHAIR_THICKNESS );
	g_theRenderer->SetBlendFunc( 0x0302, 0x0303 );
}


//-----------------------------------------------------------------------------
void TheGame::RenderRightDebugText2D()
{
	g_theRenderer->DrawText2D
	(
		Vector2( (float)g_theApp->GetScreenWidth() - 375.f, (float)g_theApp->GetScreenHeight() - 50.f ),
		Stringf( "Application Size: %.0f x %.0f", g_theApp->GetScreenWidth(), g_theApp->GetScreenHeight() ),
		18.f,
		Rgba::GREEN,
		nullptr,
		.65f
	);
}


//-----------------------------------------------------------------------------
void TheGame::RenderLeftDebugText2D()
{
	WorldCoords camPos = m_playerCamera->m_worldPosition;
	EulerAngles camOri = m_playerCamera->m_orientation;
	Vector3 camDir = m_playerCamera->GetForwardXYZ();

	g_theRenderer->DrawText2D
	( 
		Vector2( 100.f, (float)g_theApp->GetScreenHeight() - 50.f ),
		Stringf( "Camera Position: %f %f %f", camPos.x, camPos.y, camPos.z ), 
		18.f 
	);
	g_theRenderer->DrawText2D
	( 
		Vector2( 100.f, (float)g_theApp->GetScreenHeight() - 100.f ),
		Stringf( "Camera Orientation: %f %f %f", camOri.m_rollDegreesAboutX, camOri.m_pitchDegreesAboutY, camOri.m_yawDegreesAboutZ ), 18.f, Rgba( .7f, .7f, .7f ), 
		nullptr, 
		.65f
	);
	g_theRenderer->DrawText2D
	( 
		Vector2( 100.f, (float)g_theApp->GetScreenHeight() - 150.f ),
		Stringf( "Camera Forward XYZ: %f %f %f", camDir.x, camDir.y, camDir.z ), 
		18.f,
		Rgba( .7f, .7f, .7f ), 
		nullptr, 
		.65f 
	);
}


//-----------------------------------------------------------------------------
void TheGame::Render( )
{
	SetupView3D();
	Render3D();
	if ( g_renderDebugInfo ) RenderDebug3D();

	SetupView2D();
	Render2D();
	if ( g_renderDebugInfo ) RenderDebug2D();
}