#include "main.h"

CContextMenu::CContextMenu( void(CALLBACK* pCall)(CContextMenu*, int), DWORD color ) : CNodeMenu()
{
	_pCall = pCall;
	_color = color;
	_width = 0;
	_height = 0;
	_Init = false;
}

CContextMenu::~CContextMenu()
{
	for ( int i = 0; i < _vars.size(); ++i )
		delete _vars[i];
}

bool CContextMenu::isInizialize()
{
	if ( _Init )
		return true;

	if ( !CNodeMenu::isInizialize() )
		return false;

	if ( _menu == nullptr )
		return false;

	for ( int i = 0; i < _vars.size(); ++i ){
		_vars[i]->SetMenu( _menu );
		if ( !_vars[i]->isInizialize() )
			return false;
	}

	for ( int i = 0; i < _vars.size(); ++i ){
		if ( _vars[i]->GetWidth() + 3 > _width )
			_width = _vars[i]->GetWidth() + 3;
	}

	_height = _font->GetHeight() * _vars.size() + 5;

	if ( _pos.y + _height > SCREEN_Y )
		_pos.y = SCREEN_Y - _height;
	if ( _pos.x + _width > SCREEN_X )
		_pos.x = SCREEN_X - _width;

	_Init = true;
	return true;
}

void CContextMenu::AddVariant( std::string text, DWORD color )
{
	auto var = new CText( text, color );
	var->SetMenu( _menu );
	_vars.push_back( var );

	if ( _Init ){
		if ( var->GetWidth() + 3 > _width )
			_width = var->GetWidth() + 3;
		_height = _font->GetHeight() * _vars.size() + 5;
	}
}

void CContextMenu::SetDescription( std::string desc )
{
	_vars[_vars.size() - 1]->SetDescription( desc );
}

void CContextMenu::onDraw( int so_V, int so_H )
{
	if ( !isInizialize() )
		return;

	_draw->BorderBox( _pos.x - 1, _pos.y - 1, _width, _height, InvertColor( _color ), _color );

	for ( int i = 0; i < _vars.size(); ++i ){
		if ( isMouseOnVariant( i ) )
			_draw->Box( _pos.x + 1, _pos.y + i * _font->GetHeight() + 1,
			_vars[i]->GetWidth() - 1, _vars[i]->GetHeight() + 1, InvertColor( _color ) );

		_vars[i]->SetPosition( _pos );
		_vars[i]->onDraw( -(i * _font->GetHeight()) );
	}
}

bool CContextMenu::onEvents( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( !isInizialize() )
		return true;

	SetMousePos( GetMousePos() );
	if ( uMsg == WM_LBUTTONDOWN ){
		for ( int i = 0; i < _vars.size(); ++i ){

			if ( isMouseOnVariant(i) ){
				if ( _pCall != nullptr )
					_pCall( this, i );
				return false;
			}
		}
		if ( _pCall != nullptr && isMouseOnWidget() )
			_pCall( this, -1 );
		return false;
	}

	return true;
}

bool CContextMenu::isMouseOnVariant( int id )
{
	if ( _MP.x > _pos.x &&
		 _MP.x < _pos.x + _vars[id]->GetWidth() &&
		 _MP.y > _pos.y + id * _font->GetHeight() &&
		 _MP.y < (_pos.y + id * _font->GetHeight()) + _vars[id]->GetHeight() )
		return true;
	return false;
}

void CContextMenu::SetPosition( POINT pos )
{
	if ( pos.y + _height > SCREEN_Y )
		pos.y = SCREEN_Y - _height;
	if ( pos.x + _width > SCREEN_X )
		pos.x = SCREEN_X - _width;

	return CNodeMenu::SetPosition( pos );
}