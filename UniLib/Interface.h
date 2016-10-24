#pragma once
extern std::vector<void(CALLBACK*)()> listMainloop;
extern std::vector<void(CALLBACK*)(IDirect3DDevice9*)> listPresent;
extern std::vector<void(CALLBACK*)(IDirect3DDevice9*)> listReset;

DLLEXPORTC void CALLBACK RegisterMainloop( void(CALLBACK* pFunc)() );
DLLEXPORTC void CALLBACK DestoryMainloop( void(CALLBACK* pFunc)() );

DLLEXPORTC void CALLBACK RegisterPresent( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) );
DLLEXPORTC void CALLBACK DestoryPresent( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) );

DLLEXPORTC void CALLBACK RegisterReset( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) );
DLLEXPORTC void CALLBACK DestoryReset( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) );

DLLEXPORTC IDirect3DDevice9* CALLBACK GetD3DDevice();
DLLEXPORTC bool CALLBACK IsLoaded();