#include "main.h"
IDirect3DDevice9* g_Device;
bool g_Initialize = false;

CHookCallSafe *HookMainloop;
CHookD3DReset *HookD3DReset;
CHookD3DPresent *HookD3DPresent;

std::vector<CFontInfo*> listFontInfo;
std::vector<CDrawInfo*> listDrawInfo;
std::vector<void(CALLBACK*)()> listMainloop;
std::vector<void(CALLBACK*)(IDirect3DDevice9*)> listPresent;
std::vector<void(CALLBACK*)(IDirect3DDevice9*)> listReset;
std::vector<CCreateTexture*> listCreateTexture;

template<typename T>
bool delete_s( T* pT )
{
	if ( pT == nullptr )
		return false;

	delete pT;
	pT = nullptr;

	return true;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved )
{
	switch ( dwReasonForCall )
	{
	case DLL_PROCESS_ATTACH:
		HookMainloop = new CHookCallSafe( 0x00748DA3, mainloop, 6 );
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		delete_s( HookD3DPresent );
		delete_s( HookD3DReset );
		delete_s( HookMainloop );
		break;
	}
	return TRUE;
}

void CALLBACK mainloop()
{
	static bool Init = false;

	if ( !Init ){
		if ( *(int*)0xC8D4C0 != 9 )
			return;

		HookD3DReset = new CHookD3DReset( D3DReset );
		HookD3DPresent = new CHookD3DPresent( D3DPresent );

		Init = true;
	} else {
		for ( int i = 0; i < listMainloop.size(); ++i )
			listMainloop[i]();
	}
}

void CALLBACK D3DReset( IDirect3DDevice9* pDevice )
{
	g_Device = pDevice;
	for ( int i = 0; i < listFontInfo.size(); ++i )
		listFontInfo[i]->Invalidate();
	for ( int i = 0; i < listDrawInfo.size(); ++i )
		listDrawInfo[i]->Invalidate();
	for ( int i = 0; i < listCreateTexture.size(); ++i )
		listCreateTexture[i]->Release();

	for ( int i = 0; i < listReset.size(); ++i )
		listReset[i]( pDevice );
}

void CALLBACK D3DPresent( IDirect3DDevice9* pDevice )
{
	g_Device = pDevice;
	if ( !g_Initialize )
		g_Initialize = true;

	for ( int i = 0; i < listPresent.size(); ++i )
		listPresent[i]( pDevice );
}