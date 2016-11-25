#include "main.h"

CListing::CListing( POINT pos, POINT size ) : CNode( pos, size )
{
	_height = size.y;
	_width = size.x + 6;
	_size = size;
	_Init = false;
	_layout = new CVerticalLayout();
}

CListing::~CListing()
{
	delete _layout;
}

void CListing::onDraw( int so_V, int so_H )
{
	if ( !isInizialize() )
		return;

	_texture->Begin();
	_texture->Clear( _colorBkg );

	int sd = _layout->GetPosition().y + _layout->GetHeight();
	if ( sd > _scrollSizeVertical )
		_scrollSizeVertical = sd;

	sd = _layout->GetPosition().x + _layout->GetWidth();
	if ( sd > _width - 6 ){
		_width = sd + 6;
		SetSize( { _width, _height } );
	}

	if ( isMouseInNode( so_V, so_H ) )
		_layout->SetMousePos( { _MP.x - _pos.x + so_H, _MP.y - _pos.y + so_V } );
	else _layout->SetMousePos( { -1, -1 } );

	_layout->onDraw( _scrollOffsetVertical, _scrollOffsetHorizontal );

	_texture->End();
	_texture->Render( _pos.x - so_H, _pos.y - so_V );
	DrawScrollBarVertical( so_V, so_H );
	CNodeMenu::onDraw( so_V, so_H );
}

bool CListing::onEvents( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( !isMouseInNode( _SO.y, _SO.x ) )
		return true;

	if ( !_layout->onEvents( hwnd, uMsg, wParam, lParam ) )
		return false;

	switch ( uMsg )
	{
	case WM_MOUSEWHEEL:
		if ( GET_WHEEL_DELTA_WPARAM( wParam ) < 0 ){
			_scrollOffsetVertical += _scrollStep;
			if ( _scrollOffsetVertical + _size.y > _scrollSizeVertical )
				_scrollOffsetVertical = _scrollSizeVertical - _size.y;
		}
		else{
			_scrollOffsetVertical -= _scrollStep;
			if ( _scrollOffsetVertical < 0 )
				_scrollOffsetVertical = 0;
		}
		return false;
	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_UP:
			_scrollOffsetVertical -= _scrollStep;
			if ( _scrollOffsetVertical < 0 )
				_scrollOffsetVertical = 0;
			return false;
		case VK_DOWN:
			_scrollOffsetVertical += _scrollStep;
			if ( _scrollOffsetVertical + _size.y > _scrollSizeVertical )
				_scrollOffsetVertical = _scrollSizeVertical - _size.y;
			return false;
		case VK_PRIOR:
			_scrollOffsetVertical = 0;
			return false;
		case VK_NEXT:
			_scrollOffsetVertical = _scrollSizeVertical - _size.y;
			return false;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return CNodeMenu::onEvents( hwnd, uMsg, wParam, lParam );
}

bool CListing::isInizialize()
{
	if ( _Init )
		return true;

	if ( !CNode::isInizialize() )
		return false;

	if ( _menu == nullptr )
		return false;

	_layout->SetMenu( _menu );

	_Init = true;
	return true;
}

void CListing::SetSize( POINT size )
{
	size.x = _width - 6;
	CNode::SetSize( size );
}

bool CListing::AddChield( CNodeMenu* node, std::string name, bool deleteOnDestructor )
{
	return _layout->AddChield( node, name, deleteOnDestructor );
}

void CListing::DelChield( CNodeMenu* node )
{
	_layout->DelChield( node );
}

void CListing::DelChield( std::string name )
{
	_layout->DelChield( name );
}

CNodeMenu* CListing::GetChield( std::string name )
{
	return _layout->GetChield( name );
}