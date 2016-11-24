#include "main.h"

std::vector<CMenu*> MenuList;

CMenu::CMenu( std::string title, POINT size, std::string name, CNode* node, bool deleteOnDestructor ) : CNodeMenu()
{
	_title = title;
	_size = size;
	_description = "";
	_move = false;
	_pEventMove = nullptr;
	_context = nullptr;
	_removeContext = false;

	if ( node != nullptr ){
		_node = node;
		_deleteNode = deleteOnDestructor;
	}
	else{
		_node = new CNode( { 0, 0 }, size );
		_deleteNode = true;
	}
	_node->SetMenu( this );

	_Init = false;
	MenuList.push_back( this );
}

CMenu::~CMenu()
{
	delete _texture;
	if ( _deleteNode )
		delete _node;
	VectorErase( MenuList, this );
}

bool CMenu::isInizialize()
{
	if ( _Init )
		return true;

	if ( g_Device == nullptr )
		return false;

	if ( !CNodeMenu::isInizialize() )
		return false;

	if ( !_node->isInizialize() )
		return false;

	_pos.x = (SCREEN_X - _size.x) / 2;
	_pos.y = (SCREEN_Y - _size.y) / 2;

	_header = _font->GetHeight() + 3;
	_height = _size.y + _header;
	_width = _size.x + 6;
	_texture = new CCreateTexture( g_Device, _size.x - (3 + _font->GetWidth( "X" )), _header );

	_Init = true;
	return true;
}

void CMenu::onDraw( int so_V, int so_H )
{
	if ( !isInizialize() )
		return;

	if ( _move ){
		SetMousePos( GetMousePos() );
		_pos.x = _MP.x - _mvOffset.x;
		_pos.y = _MP.y - _mvOffset.y;
	}
	else if ( isMouseOnHeader() )
		SetMenuHelper( _description, -1 );

	_texture->Begin();
	_texture->Clear( 0xFF000000 );
	_font->Print( -1, _title.c_str(), 1, 0 );
	_texture->End();
	_texture->Render( _pos.x, _pos.y );
	if ( isMouseOnClose() && IsForeground( this ) && _context == nullptr ){
		_draw->Box( _pos.x + (_size.x - (3 + _font->GetWidth( "X" ))),
					_pos.y, 9 + _font->GetWidth( "X" ), _header, 0xFFFF0000 );
		SetMenuHelper( "Close\nЗакрыть", -1 );
	}
	else{
		_draw->Box( _pos.x + (_size.x - (3 + _font->GetWidth( "X" ))),
					_pos.y, 9 + _font->GetWidth( "X" ), _header, 0xFFFF6060 );
	}
	_font->Print( -1, "X", _pos.x + (_size.x - _font->GetWidth( "X" ) + 1), _pos.y );
	_draw->Box( _pos.x, _pos.y + _header - 1, _size.x + 6, 1, -1 );

	_node->SetPosition( { _pos.x, _pos.y + _header } );
	POINT MP = GetMousePos();
	_node->SetMousePos( { MP.x - so_H, MP.y - so_V } );
	_node->onDraw( so_V, so_H );

	SetMousePos( GetMousePos() );

	if ( _context != nullptr ){
		p_helper.clear();
		if ( _context->isInizialize() )
			_context->onDraw();
	}

	if ( isMouseOnWidget() && !p_helper.empty() && IsForeground(this) ){
		POINT M = GetMousePos();
		float length = _font->GetWidth( p_helper[0].c_str() );
		for ( int i = 1; i < p_helper.size(); ++i ){
			if ( _font->GetWidth( p_helper[i].c_str() ) > length )
				length = _font->GetWidth( p_helper[i].c_str() );
		}
		int x_offset = 0;
		if ( M.x + 16 + length > SCREEN_X )
			x_offset = (M.x + 16 + length) - SCREEN_X;
		_draw->BorderBox( (M.x + 16) - x_offset, M.y + 16, length + 3, _font->GetHeight() * p_helper.size() + 3, p_color, InvertColor( p_color ) );
		for ( int i = 0; i < p_helper.size(); ++i )
			_font->Print( p_color, p_helper[i].c_str(), (M.x + 17) - x_offset, M.y + 16 + _font->GetHeight() * i );
	}
	p_helper.clear();
	//p_helper.shrink_to_fit();
}

bool CMenu::onEvents( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	SetMousePos(GetMousePos());

	if ( _context != nullptr ){
		if ( !_context->onEvents( hwnd, uMsg, wParam, lParam ) ){
			if ( _removeContext )
				delete _context;
			_context = nullptr;
		}
		return false;
	}

	if ( !isMouseOnWidget() )
		return true;

	switch ( uMsg )
	{
	case WM_LBUTTONDOWN:
		if ( !IsForeground( this ) ){
			SetForeground( this );
			if ( isMouseOnClose() )
				return false;
		}
		if ( isMouseOnHeader() ){
			if ( !_move ){
				_mvOffset.x = _MP.x - _pos.x;
				_mvOffset.y = _MP.y - _pos.y;
				_move = true;
			}
			return false;
		}
		else if ( isMouseOnClose() )
			SetShow( false );
		break;
	case WM_LBUTTONUP:
		if ( _move ){
			_pos.x = _MP.x - _mvOffset.x;
			_pos.y = _MP.y - _mvOffset.y;
			_move = false;
			if ( _pEventMove != nullptr )
				_pEventMove( _pos );
		}
		break;
	default:
		break;
	}

	if ( IsForeground( this ) )
		_node->onEvents( hwnd, uMsg, wParam, lParam );
	return false;
}

void CMenu::SetSize( POINT sz )
{
	delete _texture;
	_size = sz;
	_height = sz.y + _header;
	_width = sz.x + 6;
	_texture = new CCreateTexture( g_Device, sz.x - (3 + _font->GetWidth( "X" )), _header );
	_node->SetSize( _size );
}

POINT CMenu::GetSize()
{
	return _size;
}

CNode* CMenu::GetNode()
{
	return _node;
}

void CMenu::SetMenuHelper( std::string help, DWORD color )
{
	if ( help.empty() ){
		p_helper.clear();
		return;
	}
	while ( help.find( "\n" ) != std::string::npos ){
		p_helper.push_back( help );
		p_helper[p_helper.size() - 1].erase( help.find( "\n" ), help.length() - help.find( "\n" ) );
		help.erase( 0, help.find( "\n" ) + 1 );
	}
	p_helper.push_back( help );
	p_color = color;
}

bool CMenu::isMouseOnHeader()
{
	SetMousePos( GetMousePos() );
	if ( !isMouseOnWidget() )
		return false;
	if ( _MP.x < _pos.x + (_size.x - (3 + _font->GetWidth( "X" ))) && _MP.y < _pos.y + _header )
		return true;
	return false;
}

bool CMenu::isMouseOnClose()
{
	SetMousePos( GetMousePos() );
	if ( !isMouseOnWidget() )
		return false;
	if ( _MP.x > _pos.x + (_size.x - (3 + _font->GetWidth( "X" ))) && _MP.y < _pos.y + _header )
		return true;
	return false;
}

void CMenu::SetEventMove( void(CALLBACK* pEventMove)(POINT) )
{
	_pEventMove = pEventMove;
}

void CMenu::SetContextMenu( CContextMenu* contextMenu, bool autoRemove )
{
	_context = contextMenu;
	_removeContext = autoRemove;
	_context->SetPosition( GetMousePos() );
	_context->SetMenu( this );
}

void SetForeground( CMenu* menu )
{
	for ( int i = 0; i < MenuList.size() - 1; ++i ){
		if ( menu == MenuList[i] ){
			swap( MenuList[i], MenuList[MenuList.size() - 1] );
			break;
		}
	}
}

bool IsForeground( CMenu* menu )
{
	if ( menu == MenuList[MenuList.size() - 1] )
		return true;
	return false;
}