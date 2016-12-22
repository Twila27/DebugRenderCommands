#pragma once


#include <vector>

void UpdateFromKeyboard();


//-----------------------------------------------------------------------------
//Forward declarations.
class TheGame;
class Entity2D;
class Camera3D;
class Vector3;


//-----------------------------------------------------------------------------
//Add constants required by header file, e.g. sizes of arrays on stack memory.
//const int MAX_NUMBER_OF_BULLETS = 400; //For Bullet* m_bullets[MNOB], etc.


//-----------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------
class TheGame
{
public:

	//Ship* m_ship; //Whatever represents the player, array if multiplayer.
	//Bullet* m_bullets[ MAX_NUMBER_OF_BULLETS ];

	std::vector< Entity2D* > m_entities;

	TheGame();
	~TheGame();
	void Update( float deltaSeconds );
	void Render();


private:

	void SetUpPerspectiveProjection();
	void ApplyCameraTransform();
	void SetupView3D();
	void Render3D();
	void RenderDebug3D();
	void SetupView2D();
	void Render2D();
	void RenderDebug2D();

	void RenderReticle();
	void RenderLeftDebugText2D();
	void RenderRightDebugText2D();

	void UpdateCamera( float deltaSeconds );
	void UpdateFromKeyboard( float deltaSeconds );
	void UpdateFromMouse();
	
	//Define any static class constants, e.g. 
	//static const int INITIAL_NUM_ASTEROIDS;

	Camera3D* m_playerCamera;
};
