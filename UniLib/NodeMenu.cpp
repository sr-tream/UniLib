#include "main.h"

CNodeMenu::CNodeMenu( POINT pos, CFontInfo* font, bool deleteOnDestructor )
{
	_pos = pos;
	_show = true;
	_pEventMove = nullptr;
	_pEventShow = nullptr;
	_pEventClick = nullptr;
	_menu = nullptr;
	_SO = { 0, 0 };
	_colorSelect = -1;

	if ( font != nullptr ){
		_font = font;
		_deleteFont = deleteOnDestructor;
	}
	else {
		_font = new CFontInfo( "Arial", 10, FCR_BORDER );
		_deleteFont = true;
	}
	_draw = new CDrawInfo( 40 );

	_Init = false;
}

CNodeMenu::~CNodeMenu()
{
	if ( _deleteFont )
		delete _font;
	delete _draw;
}
void CNodeMenu::onDraw( int so_V, int so_H )
{
	if ( !isInizialize() )
		return;

	_SO.x = so_H;
	_SO.y = so_V;

	if ( isMouseOnWidget( so_V, so_H ) && _menu != nullptr ){

		if ( ((CMenu*)_menu)->isActive() ){

			if ( _pEventClick != nullptr && _menu != this )
				_draw->BorderBox( _pos.x - so_H - 1, _pos.y - so_V - 1, _width + 2, _height + 2, _colorSelect, 0 );

			if ( !_description.empty() )
				((CMenu*)_menu)->SetMenuHelper( _description );
		}
	}
}

bool CNodeMenu::onEvents( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( !isMouseOnWidget( _SO.y, _SO.x ) )
		return true;

	switch ( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		if ( _pEventClick != nullptr )
			_pEventClick( this, uMsg );
		return false;
	default:
		break;
	}

	return true;
}

bool CNodeMenu::isInizialize()
{
	if ( _Init )
		return true;

	if ( g_Device == nullptr )
		return false;

	_font->Initialize( g_Device );
	_draw->Initialize( g_Device );

	_Init = true;
	return true;
}

void CNodeMenu::SetPosition( POINT pos )
{
	_pos = pos;
	if ( _pEventMove != nullptr )
		_pEventMove( this, _pos );
}

POINT CNodeMenu::GetPosition()
{
	return _pos;
}

bool CNodeMenu::isShowed()
{
	return _show;
}

void CNodeMenu::SetShow( bool show )
{
	_show = show;
	if ( _pEventShow != nullptr )
		_pEventShow( this, _show );
}

int CNodeMenu::GetHeight()
{
	return _height;
}

int CNodeMenu::GetWidth()
{
	return _width;
}

POINT CNodeMenu::GetMousePos()
{
	POINT M;
	GetCursorPos( &M );
	ScreenToClient( *(HWND*)0xC97C1C, &M );
	return M;
}

void CNodeMenu::SetMousePos( POINT MP )
{
	_MP = MP;
}

bool CNodeMenu::isMouseOnWidget( int so_V, int so_H )
{
	POINT M = _MP;
	int PX = _pos.x - so_H;
	int PY = _pos.y - so_V;
	if ( M.x > PX && M.x < PX + _width && M.y > PY && M.y < PY + _height )
		return true;
	return false;
}

void CNodeMenu::SetDescription( std::string description )
{
	_description = description;
}

std::string CNodeMenu::GetDescription()
{
	return _description;
}

DWORD CNodeMenu::InvertColor( DWORD color )
{
	bit32<DWORD> ARGB;
	ARGB.v = color;
	return D3DCOLOR_ARGB( ARGB.b[3], 0xFF - ARGB.b[2], 0xFF - ARGB.b[1], 0xFF - ARGB.b[0] );
}

void CNodeMenu::SetEventShow( void(CALLBACK*pEventShow)(CNodeMenu*, bool) )
{
	_pEventShow = pEventShow;
}

void CNodeMenu::SetEventMove( void(CALLBACK* pEventMove)(CNodeMenu*, POINT) )
{
	_pEventMove = pEventMove;
}

void CNodeMenu::SetEventClick( void(CALLBACK* pEventClick)(CNodeMenu*, UINT) )
{
	_pEventClick = pEventClick;
}

void CNodeMenu::SetMenu( CNodeMenu *menu )
{
	_menu = menu;
}