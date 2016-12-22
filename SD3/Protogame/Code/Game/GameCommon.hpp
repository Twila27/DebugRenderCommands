#pragma once


//-----------------------------------------------------------------------------
#include "Engine/Renderer/RenderCommand.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"


//-----------------------------------------------------------------------------
#define STATIC
#define VK_SHIFT 0x10
#define VK_SPACE 0x20
#define VK_F1 0x70
#define VK_F5 0x74


//-----------------------------------------------------------------------------
//Operating System
static const char* g_appName = "Protogame (Milestone 01) by Benjamin Gibson";


//-----------------------------------------------------------------------------
//Debug Flags
extern bool g_renderDebugInfo;
extern bool g_usePerspectiveCamera;


//-----------------------------------------------------------------------------
//Naming Keys
static char KEY_TO_TOGGLE_DEBUG_INFO = VK_F1;
static char KEY_TO_TOGGLE_CAMERA_PROJECTION = VK_F5;

static const char KEY_TO_MOVE_FORWARD = 'W';
static const char KEY_TO_MOVE_BACKWARD = 'S';
static const char KEY_TO_MOVE_LEFT = 'A';
static const char KEY_TO_MOVE_RIGHT = 'D';
static const char KEY_TO_MOVE_UP = VK_SPACE;
static const char KEY_TO_MOVE_DOWN = 'X'; //Be careful, also mutes BGM from Main_Win32.


//-----------------------------------------------------------------------------
//Coordinate System

typedef Vector3 WorldCoords; //May be negative.

static const Vector3 WORLD_UP = Vector3( 0.f, 0.f, 1.f );
static const Vector3 WORLD_DOWN = Vector3( 0.f, 0.f, -1.f );
static const Vector3 WORLD_LEFT = Vector3( 0.f, 1.f, 0.f );
static const Vector3 WORLD_RIGHT = Vector3( 0.f, -1.f, 0.f );
static const Vector3 WORLD_FORWARD = Vector3( 1.f, 0.f, 0.f );
static const Vector3 WORLD_BACKWARD = Vector3( -1.f, 0.f, 0.f );


//-----------------------------------------------------------------------------
//Camera
static const float FLYCAM_SPEED_SCALAR = 8.f;
static const Vector3 CAMERA_DEFAULT_POSITION = Vector3( 0.f, 0.f, 0.f );


 //-----------------------------------------------------------------------------
 //HUD

static const Vector2 HUD_BOTTOM_LEFT_POSITION = Vector2( 100.f, 27.f ); //In from left, up from bottom of screen.
static const float HUD_WIDTH_BETWEEN_ELEMENTS = 25.f;
static const float HUD_ELEMENT_WIDTH = 50.f;
static const float HUD_HEIGHT = HUD_ELEMENT_WIDTH; //Ensures HUD choices are square.
static const float HUD_CROSSHAIR_RADIUS = 20.f;
static const float HUD_CROSSHAIR_THICKNESS = 4.f;