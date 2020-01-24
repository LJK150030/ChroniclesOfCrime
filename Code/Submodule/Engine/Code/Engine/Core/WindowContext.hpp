#pragma once

#define WIN32_LEAN_AND_MEAN
#include <string>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"


enum eMouseMode
{
	MOUSE_MODE_ABSOLUTE, 
	MOUSE_MODE_RELATIVE, 
};

typedef unsigned int uint; 
typedef bool (*WindowsProcCb)( void*, uint msg, uintptr_t wparam, uintptr_t lparam ); 

class WindowContext
{
public:
	WindowContext();
	~WindowContext();

	// Create the window at (x,y) with a client size of (res_x, res_y)
	void CreateTheWindow(char const* title, int x, int y, int res_x, int res_y);
	void CreateTheWindow( std::string const &title, float client_aspect, float max_client_fraction_of_desktop, WindowsProcCb app_proc );
	void Close();

	void BeginFrame(); 
	IntVec2 GetClientCenter() const;

	IntVec2 GetWindowMousePosition() const;
	Vec2 GetMousePosition( const AABB2& camera_bounds ) const;
	void SetClientMousePosition( IntVec2 &client_pos );

	IntVec2 GetClientMouseRelativeMovement() const; 

	void LockMouse(); 
	void UnlockMouse(); 
	inline bool IsMouseLocked() const   { return m_lockCount > 0; }

	void ShowMouse() const; 
	void HideMouse() const;

	bool HasFocus() const; 

	void SetMouseMode( eMouseMode mode ); 

	inline bool IsOpen() const       { return m_hwnd != nullptr; }

public:
	void *m_hwnd; // void* if you're avoiding Windows.h
	WindowsProcCb m_gameCB;

	eMouseMode m_mouseMode     = MOUSE_MODE_ABSOLUTE; 
	IntVec2 m_lastFrameMousePosition; 
	IntVec2 m_currentMousePosition;
	AABB2 m_windowBounds;
	//AABB2 m_clientBounds;
	int m_lockCount            = 0; 
};
