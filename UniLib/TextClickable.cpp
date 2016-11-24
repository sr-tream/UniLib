#include "main.h"

CTextClickable::CTextClickable( std::string text,
								DWORD color,
								void(CALLBACK* pClick)(UINT),
								POINT pos,
								CFontInfo* font,
								bool deleteOnDestructor )
								: CText(text, color, pos, font, deleteOnDestructor)
{
	_description = "Clickable text";
	_pClick = pClick;
}


void CTextClickable::onDraw( int so_V, int so_H )
{
	_SO.x = so_H;
	_SO.y = so_V;

	if ( isMouseOnWidget( so_V, so_H ) )
		_draw->BorderBox( _pos.x - so_H - 1, _pos.y - so_V, _width + 3, _height + 3, _color, InvertColor( _color ) );
	CText::onDraw( so_V, so_H );
}

bool CTextClickable::onEvents( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( !isMouseOnWidget( _SO.y, _SO.x ) )
		return true;

	switch ( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		if ( _pClick != nullptr )
			_pClick( uMsg );
		return false;
	default:
		break;
	}

	return true;
}

void CTextClickable::SetCallback( void(CALLBACK* pClick)(UINT) )
{
	_pClick = pClick;
}