#include "main.h"


CFontInfo::CFontInfo( const char *szFontName, int fontHeight, DWORD dwCreateFlags )
{
	_pFont = new CD3DFont( szFontName, fontHeight, dwCreateFlags );
	_pDevice = nullptr;
	listFontInfo.push_back( this );
}


CFontInfo::~CFontInfo()
{
	delete _pFont;
	VectorErase( listFontInfo, this );
}


void CFontInfo::Initialize( IDirect3DDevice9* pDevice )
{
	if ( _init )
		return;

	if ( pDevice == nullptr )
		return;

	_pFont->Initialize( pDevice );

	if ( _pDevice == nullptr )
		_pDevice = pDevice;

	_init = true;
}


void CFontInfo::Invalidate()
{
	if ( !_init )
		return;

	_pFont->Invalidate();
	_init = false;
}


void CFontInfo::Print( DWORD color, const char* szText, int X, int Y )
{
	if ( !_init )
		Initialize( _pDevice );

	_pFont->Print( szText, color, X, Y, false, false );
}


void CFontInfo::PrintShadow( DWORD color, const char* szText, int X, int Y )
{
	if ( !_init )
		Initialize( _pDevice );

	_pFont->Print( szText, color, X, Y, true, false );
}


float CFontInfo::GetHeight()
{
	if ( !_init )
		Initialize( _pDevice );

	return _pFont->DrawHeight();
}


float CFontInfo::GetWidth( const char* szText )
{
	if ( !_init )
		Initialize( _pDevice );

	return _pFont->DrawLength(szText);
}


DLLEXPORTC CFontInfo* CALLBACK CreateFontInfo( const char *szFontName, int fontHeight, DWORD dwCreateFlags )
{
	CFontInfo *pFont = new CFontInfo( szFontName, fontHeight, dwCreateFlags );
	pFont->Initialize( g_Device );
	return pFont;
}
DLLEXPORTC void CALLBACK DestoryFontInfo( CFontInfo* &pFont )
{
	delete pFont;
	pFont = nullptr;
}