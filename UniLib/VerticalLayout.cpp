#include "main.h"

CVerticalLayout::CVerticalLayout( POINT pos ) : CNode( pos )
{
	_height = 0;
	_width = 0;

	_Init = false;
}

void CVerticalLayout::onDraw( int so_V, int so_H )
{
	if ( !isInizialize() )
		return;

	if ( _Init )
		return DrawLayout( so_V, so_H );

	if ( !CNode::isInizialize() )
		return;

	for ( int i = 0; i < _nodes.size(); ++i ){
		POINT pos = _nodes[i].node->GetPosition();
		pos.y = _height;
		_nodes[i].node->SetPosition( pos );
		_height += _nodes[i].node->GetHeight() + 3;
		if ( _nodes[i].node->GetWidth() > _width )
			_width = _nodes[i].node->GetWidth();
	}

	CNode::SetSize( { _width, _height } );

	_Init = true;
	DrawLayout(so_V, so_H);
}

void CVerticalLayout::SetPosition( POINT pos )
{
	_pos.x = pos.x;
}

bool CVerticalLayout::AddChield( CNodeMenu* node, std::string name, bool deleteOnDestructor )
{
	if ( name.length() > 1 )
		for ( int i = 0; i < _nodes.size(); ++i )
			if ( _nodes[i].name == name )
				return false;

	POINT pos = node->GetPosition();
	pos.y = _height;
	node->SetPosition( pos );
	if ( _Init ){
		_height += node->GetHeight() + 3;
		if ( node->GetWidth() > _width )
			_width = node->GetWidth();
		CNode::SetSize( { _width, _height } );
	}

	_nodes.push_back( { node, deleteOnDestructor, name } );
	node->SetMenu( _menu );
	return true;
}
void CVerticalLayout::DelChield( CNodeMenu* node )
{
	int offset = -1;
	_width = 0;
	for ( int i = 0; i < _nodes.size(); ++i ){
		if ( _nodes[i].node == node ){
			offset = _nodes[i].node->GetHeight();
			if ( _nodes[i].autoRemove )
				delete node;
		}
		else if ( _Init ){
			if ( offset != -1 ){
				POINT pos = _nodes[i].node->GetPosition();
				pos.y -= offset;
				_nodes[i].node->SetPosition( pos );
			}

			if ( _nodes[i].node->GetWidth() > _width )
				_width = _nodes[i].node->GetWidth();
		}
	}
	if ( offset != -1 )
		_height -= offset;
	if ( _Init )
		CNode::SetSize( { _width, _height } );
}

void CVerticalLayout::DelChield( std::string name )
{
	if ( name.empty() )
		return;

	int offset = -1;
	_width = 0;
	for ( int i = 0; i < _nodes.size(); ++i ){
		if ( _nodes[i].name == name ){
			offset = _nodes[i].node->GetHeight();
			if ( _nodes[i].autoRemove )
				delete _nodes[i].node;
		}
		else if ( _Init ){
			if ( offset != -1 ){
				POINT pos = _nodes[i].node->GetPosition();
				pos.y -= offset;
				_nodes[i].node->SetPosition( pos );
			}

			if ( _nodes[i].node->GetWidth() > _width )
				_width = _nodes[i].node->GetWidth();
		}
	}
	if ( offset != -1 )
		_height -= offset;
	if ( _Init )
		CNode::SetSize( { _width, _height } );
}

void CVerticalLayout::SetSize( POINT sz )
{
	// Размеры считаются сами!
}

void CVerticalLayout::DrawLayout( int so_V, int so_H )
{
	// Все кроме скроллбаров скопипизженно из CNode
	if ( g_Device == nullptr )
		return;
	_texture->Begin();
	_texture->Clear( _colorBkg );

	for ( int i = 0; i < _nodes.size(); ++i ){
		if ( _nodes[i].node->isShowed() ){

			if ( isMouseInNode(so_V, so_H) )
				_nodes[i].node->SetMousePos( { _MP.x - _pos.x + so_H, _MP.y - _pos.y + so_V } );
			else _nodes[i].node->SetMousePos( { -1, -1 } );
			_nodes[i].node->onDraw( _scrollOffsetVertical, _scrollOffsetHorizontal );
		}
	}

	_texture->End();
	_texture->Render( _pos.x - so_H, _pos.y - so_V );
	CNodeMenu::onDraw( so_V, so_H );
}

bool CVerticalLayout::onEvents( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Тоже скопипизжено из CNode, и то же выпелины скроллбары
	if ( !isMouseInNode( _SO.y, _SO.x ) )
		return true;
	for ( int i = 0; i < _nodes.size(); ++i ){
		if ( _nodes[i].node->isShowed() )
			if ( !_nodes[i].node->onEvents( hwnd, uMsg, wParam, lParam ) )
				return false;
	}

	return CNodeMenu::onEvents( hwnd, uMsg, wParam, lParam );
}