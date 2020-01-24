#include "Engine/Core/WindowContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Engine/Renderer/DebugRender.hpp"

//------------------------------------------------------------------------
//
// DEFINES
//
//------------------------------------------------------------------------
static constexpr const TCHAR* GAME_WINDOW_CLASS_NAME = TEXT("GameWindowClass");


//------------------------------------------------------------------------
//
// INTERNAL FUNCTIONS
//
//------------------------------------------------------------------------
static void LockMouseToWindow(HWND hwnd)
{
	RECT client_rect;
	GetClientRect(hwnd, &client_rect);

	POINT client_origin = {0, 0};
	ClientToScreen(hwnd, &client_origin);

	client_rect.left += client_origin.x;
	client_rect.right += client_origin.x;
	client_rect.top += client_origin.y;
	client_rect.bottom += client_origin.y;

	ClipCursor(&client_rect);
}


//------------------------------------------------------------------------
// Get the instance of the running program.  Here so I don't need 
// to pass around an HINSTANCE from my WinMain to all my window creation
// functions.
static HINSTANCE GetCurrentHinstance()
{
	return ::GetModuleHandle(nullptr);
}

//------------------------------------------------------------------------
// Message Handler for the windows we create.
// Determines how it responds to specific events.
//LRESULT CALLBACK EngineWindowsHandlingProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
static LRESULT CALLBACK GameCommonWindowProc(HWND window_handle, UINT wm_message_code, WPARAM w_param, LPARAM l_param)
{
	WindowContext* window_context = reinterpret_cast<WindowContext*>(GetWindowLongPtr(window_handle, GWLP_USERDATA));

	switch (wm_message_code)
	{
		//------------------------------------------------------------------------
		// IMPORTANT/COMMON
		//------------------------------------------------------------------------
	case WM_CREATE:
		{
			// Window creation happened.
		}
		break;

	case WM_SHOWWINDOW:
		{
			// Window was shown/hidden
			// bool visible = (wparam == TRUE);
		}
		break;

	case WM_DESTROY:
		{
			// Window has been destroyed (actually closed)
			//ERROR_RECOVERABLE( "Window Destroyed" );
		}
		break;


		// Never do the default action for WM_CLOSE
	case WM_CLOSE:
		{
			// User has requested this window be closed
			// so actually destroy it
			// (note - you can intercept this by not calling DestroyWindow.
			// For example, if you wanted a custom in-game dialogue whent he user ALT-F4s
			DestroyWindow(window_handle);
			break;
		}

	case WM_PAINT:
		{
			// Ignore WM_PAINT messages
			// [c4] Must handle them otherwise they just keep coming
			PAINTSTRUCT ps;
			BeginPaint(window_handle, &ps);
			EndPaint(window_handle, &ps);
			break;
		}


		//------------------------------------------------------------------------
		// INPUT      
		//------------------------------------------------------------------------

	case WM_KEYUP:
		{
			// User has released a key
			// wparam - virtual key code
			// lparam - additional information
			break;
		}

	case WM_KEYDOWN:
		{
			// User has pressed a key
			// wparam - virtual key code
			// lparam - additional information

			// TEMP - just to show I can close with Escape
			break;
		}

	case WM_CHAR:
		{
			// User has typed a character
			// wparam - char code
			// lparam - additional information (like ALT state)
			break;
		}

	case WM_MOUSEMOVE:
		// mouse has moved.
		// wparam - mouse keys
		// lparam - mouse positions

		// Positions are relative to client area.
		// int mouse_x = LOWORD(lparam);
		// int mouse_y = HIWORD(lparam);
		break;

		//------------------------------------------------------------------------
		// POTENTIALLY USEFUL EVENTS - LOOK UP IN DOCUMENTATION 
		//------------------------------------------------------------------------
	case WM_ACTIVATE:
		{
			const WORD active_word = LOWORD(w_param);
			const bool is_active = (active_word != WA_INACTIVE);
			if (is_active && window_context->IsMouseLocked())
			{
				LockMouseToWindow(window_handle);
			}
			else
			{
				//UnlockMouse();
				ClipCursor(nullptr);
			}
			break;
		}

	case WM_MOVE:
	case WM_SIZE:
		{
			// Called when the window changes size or is moved.
			break;
		}

	case WM_DWMNCRENDERINGCHANGED:
		{
			// Rendering policy has changed (full screen deciated in OpenGL will cause this)
			break;
		}


	case WM_DPICHANGED:
		{
			// If the monitor DPI has changed.
			break;
		}

	case WM_ERASEBKGND:
		{
			// sent when the background needs to be erased (may potentially want to render)
			break;
		}

		//------------------------------------------------------------------------
		// NO-OP these ones - prevents windows key
		//------------------------------------------------------------------------
	case WM_INITMENUPOPUP:
		// case WM_SYSCOMMAND:
	case WM_ENTERIDLE:
		{
			// Consumes the event
			break;
		}

	default:
		// Trace( "wnd", "Unhandled Message: 0x%04X [0x%04X, 0x%04X]", msg, wparam, lparam );
		break;
	}

	bool handled = false;
	if (window_context != nullptr)
	{
		handled = window_context->m_gameCB(window_handle, wm_message_code, w_param, l_param);
	}
	if (!handled)
	{
		return DefWindowProc(window_handle, wm_message_code, w_param, l_param);
	}
	return 0;
}

//------------------------------------------------------------------------
static int gRegisterCount = 0;

//------------------------------------------------------------------------
static void RegisterGameWindowClass()
{
	// Define a window style/class
	WNDCLASSEX window_class_description;
	memset(&window_class_description, 0, sizeof(window_class_description));
	window_class_description.cbSize = sizeof(window_class_description);
	window_class_description.style = CS_OWNDC; // Redraw on move, request own Display Context
	window_class_description.lpfnWndProc = static_cast<WNDPROC>(GameCommonWindowProc);
	// Register our Windows message-handling function
	window_class_description.hInstance = GetModuleHandle(nullptr);
	window_class_description.hIcon = nullptr;
	window_class_description.hCursor = nullptr;
	window_class_description.lpszClassName = GAME_WINDOW_CLASS_NAME;
	RegisterClassEx(&window_class_description);
}

//------------------------------------------------------------------------
// Gets the window size needed for the desired client size.
static RECT WindowGetRect(const int width, const int height, const DWORD style)
{
	RECT window_rect;
	window_rect.left = 0;
	window_rect.top = 0;
	window_rect.right = width;
	window_rect.bottom = height;

	// Adjust the size so the window is big enough with this style to allow the client
	// to be the size of window_rect
	AdjustWindowRect(&window_rect, style, FALSE);

	return window_rect;
}

//------------------------------------------------------------------------
//
// EXTERNAL FUNCTIONS
//
//------------------------------------------------------------------------
WindowContext::WindowContext()
{
	m_hwnd = nullptr;

	if (gRegisterCount == 0)
	{
		RegisterGameWindowClass();
	}
	++gRegisterCount;
}

WindowContext::~WindowContext()
{
	Close();

	--gRegisterCount;
	if (gRegisterCount == 0)
	{
		UnregisterClass(GAME_WINDOW_CLASS_NAME, GetModuleHandle(nullptr));
	}
}

//------------------------------------------------------------------------
// Create the window at (x,y) with a client size of (res_x, res_y)
void WindowContext::CreateTheWindow(char const* title, int x, int y, int res_x, int res_y)
{
	const DWORD extended_style = WS_EX_APPWINDOW;
	DWORD style = 0U;

	wchar_t* w_title = new wchar_t[256];
	MultiByteToWideChar(CP_ACP, 0, title, -1, w_title, 256);

	// If you want a "window" - this gives it the caption bar, a sysmenu, and a border.
	style |= (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER);

	// this style will instead make it border-less - good for full-screen windows.
	// MUTUALLY EXCLUSIVE WITH ABOVE
	// style |= WS_POPUP;

	// Window starts invisible by default - this will allow it to be visible
	style |= WS_VISIBLE;

	const RECT win_rect = WindowGetRect(res_x, res_y, style);

	x += win_rect.left;
	y += win_rect.top;
	const int w = win_rect.right - win_rect.left;
	const int h = win_rect.bottom - win_rect.top;
	m_windowBounds = AABB2(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h));
	//m_clientBounds = AABB2(0.0f, 0.0f, static_cast<float>(x), static_cast<float>(y));

	DWORD error_first = GetLastError();
	error_first;

	const HWND hwnd = CreateWindowEx(extended_style, // Extended style (not used)
	                                 w_title, // Windows Class to use
	                                 L"TEST", // title      
	                                 style, // Style to start this with
	                                 x, y, // x, y of actual window (not client rect)
	                                 w, h, // width and height of actual window
	                                 nullptr, // my parent (not used in this example)
	                                 nullptr, // my menu (not used in this example)
	                                 GetCurrentHinstance(), // hinstance this window belongs too
	                                 nullptr);
	// user data associated with this window (good place to store a pointer to a window object if you class'ify this)

	if (nullptr == hwnd)
	{
		const DWORD error = GetLastError();
		ERROR_RECOVERABLE( Stringf("Failed to create window:  Error[%u]", error ));
	}

	m_hwnd = hwnd;
	SetWindowLongPtr(static_cast<HWND>(m_hwnd), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

//------------------------------------------------------------------------
void WindowContext::CreateTheWindow(std::string const& title, const float client_aspect, const float max_client_fraction_of_desktop,
									const WindowsProcCb app_proc)
{
	ASSERT_OR_DIE( !IsOpen(), "Window already open." );

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD window_style_flags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD window_style_ex_flags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktop_rect;
	const HWND desktop_window_handle = GetDesktopWindow();
	GetClientRect(desktop_window_handle, &desktop_rect);
	const float desktop_width = static_cast<float>(desktop_rect.right - desktop_rect.left);
	const float desktop_height = static_cast<float>(desktop_rect.bottom - desktop_rect.top);
	const float desktop_aspect = desktop_width / desktop_height;

	// Calculate maximum client size (as some % of desktop size)
	float client_width = desktop_width * max_client_fraction_of_desktop;
	float client_height = desktop_height * max_client_fraction_of_desktop;
	if (client_aspect > desktop_aspect)
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		client_height = client_width / client_aspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		client_width = client_height * client_aspect;
	}
	m_windowBounds = AABB2(0.0f, 0.0f, client_width, client_height);


	// Calculate client rect bounds by centering the client area
	const float client_margin_x = 0.5f * (desktop_width - client_width);
	const float client_margin_y = 0.5f * (desktop_height - client_height);
	//m_clientBounds = AABB2(0.0f, 0.0f, client_margin_x, client_margin_y);

	RECT client_rect;
	client_rect.left = static_cast<int>(client_margin_x);
	client_rect.right = client_rect.left + static_cast<int>(client_width);
	client_rect.top = static_cast<int>(client_margin_y);
	client_rect.bottom = client_rect.top + static_cast<int>(client_height);

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT window_rect = client_rect;
	AdjustWindowRectEx(&window_rect, window_style_flags, FALSE, window_style_ex_flags);

	WCHAR window_title[ 1024 ];
	MultiByteToWideChar(GetACP(), 0, title.c_str(), -1, window_title, sizeof(window_title) / sizeof(window_title[0]));
	const HWND hwnd = CreateWindowEx(
		window_style_ex_flags,
		GAME_WINDOW_CLASS_NAME,
		window_title,
		window_style_flags,
		window_rect.left,
		window_rect.top,
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top,
		nullptr,
		nullptr,
		::GetModuleHandle(nullptr),
		nullptr);

	ASSERT_OR_DIE( hwnd != nullptr, "Failed to create window" );

	m_hwnd = static_cast<void*>(hwnd);
	m_gameCB = app_proc;
	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	const HCURSOR cursor = LoadCursor(nullptr, IDC_ARROW);
	SetCursor(cursor);

	// Start the game with 0 mouse movement; 
	m_lastFrameMousePosition = GetWindowMousePosition();
	m_currentMousePosition = GetWindowMousePosition();
}

//------------------------------------------------------------------------
void WindowContext::Close()
{
	if (!IsOpen())
	{
		return;
	}

	CloseWindow(static_cast<HWND>(m_hwnd));
	m_hwnd = nullptr;
}

//------------------------------------------------------------------------
void WindowContext::BeginFrame()
{
	MSG queued_message;
	for (;;)
	{
		const BOOL was_message_present = PeekMessage(&queued_message, nullptr, 0, 0, PM_REMOVE);
		if (!was_message_present)
		{
			break;
		}

		TranslateMessage(&queued_message);
		DispatchMessage(&queued_message);
		// This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}

	// always save off old one;
	m_lastFrameMousePosition = m_currentMousePosition;
	m_currentMousePosition = GetWindowMousePosition();

	if ((m_mouseMode == MOUSE_MODE_RELATIVE) && HasFocus())
	{
		IntVec2 center = GetClientCenter();
		SetClientMousePosition(center);
		m_lastFrameMousePosition = GetWindowMousePosition();
	}
}

//------------------------------------------------------------------------
IntVec2 WindowContext::GetClientCenter() const
{
	RECT client_rect;
	GetClientRect(static_cast<HWND>(m_hwnd), &client_rect);

	IntVec2 center;
	center.x = (client_rect.left + client_rect.right) / 2;
	center.y = (client_rect.top + client_rect.bottom) / 2;

	return center;
}

//------------------------------------------------------------------------
IntVec2 WindowContext::GetWindowMousePosition() const
{
	POINT desktop_position;
	if (!GetCursorPos(&desktop_position))
	{
		desktop_position.x = 0;
		desktop_position.y = 0;
	}

	ScreenToClient(static_cast<HWND>(m_hwnd), &desktop_position);
	return IntVec2(desktop_position.x, desktop_position.y);
}

Vec2 WindowContext::GetMousePosition( const AABB2& camera_bounds ) const
{
	IntVec2 window_position = GetWindowMousePosition();
	float pos_x = RangeMapLinearFloat(static_cast<float>(window_position.x), m_windowBounds.mins.x, m_windowBounds.maxs.x, camera_bounds.mins.x, camera_bounds.maxs.x);
	float pos_y = RangeMapLinearFloat(static_cast<float>(window_position.y), m_windowBounds.mins.y, m_windowBounds.maxs.y, camera_bounds.maxs.y, m_windowBounds.mins.x);
	return Vec2(pos_x, pos_y);
}

//------------------------------------------------------------------------
void WindowContext::SetClientMousePosition(IntVec2& client_pos)
{
	POINT screen = {client_pos.x, client_pos.y};
	ClientToScreen(static_cast<HWND>(m_hwnd), &screen);

	SetCursorPos(screen.x, screen.y);
}

//------------------------------------------------------------------------
IntVec2 WindowContext::GetClientMouseRelativeMovement() const
{
	return m_currentMousePosition - m_lastFrameMousePosition;
}

//------------------------------------------------------------------------
void WindowContext::LockMouse()
{
	++m_lockCount;
	if (m_lockCount > 0)
	{
		LockMouseToWindow(static_cast<HWND>(m_hwnd));
	}

	// 0 out for a frame; 
	m_lastFrameMousePosition = GetWindowMousePosition();
	m_currentMousePosition = GetWindowMousePosition();
}

//------------------------------------------------------------------------
void WindowContext::UnlockMouse()
{
	--m_lockCount;
	if (m_lockCount <= 0)
	{
		ClipCursor(nullptr);
	}
}

//------------------------------------------------------------------------
void WindowContext::ShowMouse() const
{
	ShowCursor(TRUE);
}

//------------------------------------------------------------------------
void WindowContext::HideMouse() const
{
	ShowCursor(FALSE);
}

//------------------------------------------------------------------------
bool WindowContext::HasFocus() const
{
	return (GetActiveWindow() == static_cast<HWND>(m_hwnd));
}

//------------------------------------------------------------------------
void WindowContext::SetMouseMode(const eMouseMode mode)
{
	m_mouseMode = mode;
	switch(mode)
	{
		case MOUSE_MODE_ABSOLUTE:
		{
			break;	
		}

		case MOUSE_MODE_RELATIVE:
		{
			IntVec2 center = GetClientCenter();
			m_currentMousePosition = center;
			m_lastFrameMousePosition = center;
			SetClientMousePosition(center);
			HideMouse();
			break;
		}

	}
}
