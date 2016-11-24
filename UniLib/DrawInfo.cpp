#include "main.h"


CDrawInfo::CDrawInfo( int numVertices )
{
	_pDraw = new CD3DRender( numVertices );
	_pDevice = nullptr;
	_init = false;

	listDrawInfo.push_back( this );
}


CDrawInfo::~CDrawInfo()
{
	delete _pDraw;
	VectorErase( listDrawInfo, this );
}


void CDrawInfo::Box( int X, int Y, int W, int H, DWORD color )
{
	if ( !_init )
		Initialize( _pDevice );

	_pDraw->D3DBox( X, Y, W, H, color );
}


void CDrawInfo::BorderBox( int X, int Y, int W, int H, DWORD boreder_color, DWORD color )
{
	if ( !_init )
		Initialize( _pDevice );

	_pDraw->D3DBoxBorder( X, Y, W, H, boreder_color, color );
}


void CDrawInfo::Line( int X, int Y, int X2, int Y2, DWORD color )
{
	if ( !_init )
		Initialize( _pDevice );

	_pDraw->DrawLine( X, Y, X2, Y2, 1, color );
}


void CDrawInfo::Initialize( IDirect3DDevice9* pDevice )
{
	if ( _init )
		return;

	if ( pDevice == nullptr )
		return;

	_pDraw->Initialize( pDevice );
	_init = true;

	if ( _pDevice == nullptr )
		_pDevice = pDevice;
}


void CDrawInfo::Invalidate()
{
	if ( !_init )
		return;

	_pDraw->Invalidate();
	_init = false;
}


DLLEXPORTC CDrawInfo* CALLBACK CreateDrawInfo( int numVertices )
{
	CDrawInfo *pDraw = new CDrawInfo( numVertices );
	pDraw->Initialize( g_Device );
	return pDraw;
}

DLLEXPORTC void CALLBACK DestoryDrawInfo( CDrawInfo* &pDraw )
{
	delete pDraw;
	pDraw = nullptr;
}