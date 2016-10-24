#include "main.h"

DLLEXPORTC void CALLBACK RegisterMainloop( void(CALLBACK* pFunc)() )
{
	if ( pFunc == nullptr )
		return;

	listMainloop.push_back( pFunc );
}

DLLEXPORTC void CALLBACK DestoryMainloop( void(CALLBACK* pFunc)() )
{
	if ( pFunc == nullptr )
		return;

	VectorErase( listMainloop, pFunc );
}


DLLEXPORTC void CALLBACK RegisterPresent( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) )
{
	if ( pFunc == nullptr )
		return;

	listPresent.push_back( pFunc );
}

DLLEXPORTC void CALLBACK DestoryPresent( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) )
{
	if ( pFunc == nullptr )
		return;

	VectorErase( listPresent, pFunc );
}


DLLEXPORTC void CALLBACK RegisterReset( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) )
{
	if ( pFunc == nullptr )
		return;

	listReset.push_back( pFunc );
}

DLLEXPORTC void CALLBACK DestoryReset( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) )
{
	if ( pFunc == nullptr )
		return;

	VectorErase( listReset, pFunc );
}


DLLEXPORTC IDirect3DDevice9* CALLBACK GetD3DDevice()
{
	return g_Device;
}

DLLEXPORTC bool CALLBACK IsLoaded()
{
	return g_Initialize;
}