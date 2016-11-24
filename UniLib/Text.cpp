#include "main.h"

CText::CText( std::string text, DWORD color, POINT pos, CFontInfo* font, bool deleteOnDestructor ) : CNodeMenu( pos, font, deleteOnDestructor )
{
	_text = text;
	_color = color;
	_description = "";

	_Init = false;
}

bool CText::isInizialize()
{
	if ( _Init )
		return true;

	if ( g_Device == nullptr )
		return false;

	if ( !CNodeMenu::isInizialize() )
		return false;

	_height = _font->GetHeight();
	_width = _font->GetWidth( _text.c_str() );

	_Init = true;
	return false;
}

void CText::onDraw( int so_V, int so_H )
{
	if ( !isInizialize() )
		return;

	_font->Print( _color, _text.c_str(), _pos.x - so_H, _pos.y - so_V );

	if ( isMouseOnWidget( so_V, so_H ) && _menu != nullptr ){
		((CMenu*)_menu)->SetMenuHelper( _description, _color );
	}
}

bool CText::onEvents( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return true;
}