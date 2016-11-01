#include "main.h"
IDirect3DDevice9* g_Device;
bool g_Initialize = false;
DWORD g_SampAddr = NULL;
struct stSAMP* g_SAMP;
WNDPROC hOrigProc = NULL;

CHookCallSafe *HookMainloop;
CHookD3DReset *HookD3DReset;
CHookD3DPresent *HookD3DPresent;
CHookJmp *HookRPC1;
CHookJmp *HookRPC2;
CHookCallNoSafe *HookCNetDestructor1;
CHookCallNoSafe *HookCNetDestructor2;

std::vector<CFontInfo*> listFontInfo;
std::vector<CDrawInfo*> listDrawInfo;
std::vector<void(CALLBACK*)()> listMainloop;
std::vector<void(CALLBACK*)(IDirect3DDevice9*)> listPresent;
std::vector<void(CALLBACK*)(IDirect3DDevice9*)> listReset;
std::vector<CCreateTexture*> listCreateTexture;
std::vector<bool(CALLBACK*)(unsigned char, RPCParameters *)> listHandleRPCPacket;
std::vector<bool(CALLBACK*)(int, BitStream*, PacketPriority, PacketReliability, char, bool)> listSendRPC;
std::vector<bool(CALLBACK*)(Packet*)> listRecive;
std::vector<bool(CALLBACK*)(BitStream*, PacketPriority, PacketReliability, char)> listSendPacket;
std::vector<bool(CALLBACK*)(HWND, UINT, WPARAM, LPARAM)> listWndProc;;

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
		//MessageBoxA( nullptr, std::string( "PlayerID=" + std::to_string( sizeof( PlayerID ) ) ).c_str(), "UniLib", MB_OK );
		HookMainloop = new CHookCallSafe( 0x00748DA3, mainloop, 6 );
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		SetWindowLong( *(HWND*)0xC97C1C, GWL_WNDPROC, (LONG)(UINT_PTR)hOrigProc );
		delete_s( HookCNetDestructor2 );
		delete_s( HookCNetDestructor1 );
		delete_s( HookRPC2 );
		delete_s( HookRPC1 );
		delete_s( HookD3DPresent );
		delete_s( HookD3DReset );
		delete_s( HookMainloop );
		break;
	}
	return TRUE;
}

DWORD hook_handle_rpc_packet_jmp;
void _declspec (naked) hook_handle_rpc_packet( void )
{
	static RPCParameters *pRPCParams = nullptr;
	static RPCNode *pRPCNode = nullptr;
	static DWORD dwTmp = 0;
	static bool result = true;

	__asm pushad;
	__asm mov pRPCParams, eax;
	__asm mov pRPCNode, edi;

	result = HandleRPCPacketFunc( pRPCNode->uniqueIdentifier, pRPCParams );
	dwTmp = g_SampAddr + SAMP_HOOKEXIT_HANDLE_RPC;

	__asm popad;
	if ( !result )
		__asm jmp dwTmp;
	__asm jmp hook_handle_rpc_packet_jmp;
}

DWORD hook_handle_rpc_packet2_jmp;
void _declspec (naked) hook_handle_rpc_packet2( void )
{
	static RPCParameters *pRPCParams = nullptr;
	static RPCNode *pRPCNode = nullptr;
	static DWORD dwTmp = 0;
	static bool result = true;

	__asm pushad;
	__asm mov pRPCParams, ecx;
	__asm mov pRPCNode, edi;

	result = HandleRPCPacketFunc( pRPCNode->uniqueIdentifier, pRPCParams );
	dwTmp = g_SampAddr + SAMP_HOOKEXIT_HANDLE_RPC2;

	__asm popad;
	if ( !result )
		__asm jmp dwTmp;
	__asm jmp hook_handle_rpc_packet_jmp;
}

void __stdcall CNetGame__destructor( void )
{
	// release hooked rakclientinterface, restore original rakclientinterface address and call CNetGame destructor
	if ( g_SAMP->pRakClientInterface != NULL )
		delete g_SAMP->pRakClientInterface;
	g_SAMP->pRakClientInterface = g_RakClient->GetInterface();
	return ((void( __thiscall * ) (void *)) (g_SampAddr + SAMP_FUNC_CNETGAMEDESTRUCTOR))(g_SAMP);
}

void CALLBACK mainloop()
{
	static bool Init = false;

	if ( !Init ){
		static bool preloadHooks = false;
		if ( !preloadHooks ){
			g_SampAddr = (DWORD)GetModuleHandle( "samp.dll" );
			if ( g_SampAddr == NULL )
				return;

			g_SAMP = GetSAMPPtrInfo<stSAMP *>( SAMP_INFO_OFFSET );
			if ( g_SAMP == nullptr )
				return;

			DWORD oldProtect;
			VirtualProtect( g_SAMP->pRakClientInterface, 4, PAGE_EXECUTE_READWRITE, &oldProtect );
			g_RakClient = new RakClient( g_SAMP->pRakClientInterface );
			g_SAMP->pRakClientInterface = new HookedRakClientInterface();
			VirtualProtect( g_SAMP->pRakClientInterface, 4, oldProtect, nullptr );

			HookRPC1 = new CHookJmp( g_SampAddr + SAMP_HOOKENTER_HANDLE_RPC, hook_handle_rpc_packet, 6 );
			HookRPC2 = new CHookJmp( g_SampAddr + SAMP_HOOKENTER_HANDLE_RPC2, hook_handle_rpc_packet, 8 );

			hook_handle_rpc_packet_jmp = HookRPC1->GetExitAddress();
			hook_handle_rpc_packet2_jmp = HookRPC2->GetExitAddress();

			HookCNetDestructor1 = new CHookCallNoSafe( g_SampAddr + SAMP_HOOKENTER_CNETGAME_DESTR, CNetGame__destructor );
			HookCNetDestructor2 = new CHookCallNoSafe( g_SampAddr + SAMP_HOOKENTER_CNETGAME_DESTR2, CNetGame__destructor );

			preloadHooks = true;
		}

		if ( *(int*)0xC8D4C0 != 9 )
			return;

		HookD3DReset = new CHookD3DReset( D3DReset );
		HookD3DPresent = new CHookD3DPresent( D3DPresent );
		hOrigProc = (WNDPROC)SetWindowLong( *(HWND*)0xC97C1C, GWL_WNDPROC, (LONG)(UINT_PTR)WndProc );

		//AddChatMessage( -1, "Hello UniLib" );

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

bool HandleRPCPacketFunc( unsigned char id, RPCParameters *rpcParams ) // in RPC
{
	bool result = true;
	for ( int i = 0; i < listHandleRPCPacket.size(); ++i ){
		if ( !listHandleRPCPacket[i]( id, rpcParams ) )
			result = false;
	}
	return result;
}

bool OnSendRPC( int uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp ) // out RPC
{
	bool result = true;
	for ( int i = 0; i < listSendRPC.size(); ++i ){
		if ( !listSendRPC[i]( uniqueID, parameters, priority, reliability, orderingChannel, shiftTimestamp ) )
			result = false;
	}
	return result;
}

bool OnReceivePacket( Packet *p ) // in Packet
{
	bool result = true;
	for ( int i = 0; i < listRecive.size(); ++i ){
		if ( !listRecive[i]( p ) )
			result = false;
	}
	return result;
}

bool OnSendPacket( BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel ) // out Packet
{
	bool result = true;
	for ( int i = 0; i < listSendPacket.size(); ++i ){
		if ( !listSendPacket[i]( bitStream, priority, reliability, orderingChannel ) )
			result = false;
	}
	return result;
}

LRESULT APIENTRY WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	bool result = true;
	for ( int i = 0; i < listWndProc.size(); ++i ){
		if ( !listWndProc[i]( hwnd, uMsg, wParam, lParam ) )
			result = false;
	}
	if ( result )
		return CallWindowProc( hOrigProc, hwnd, uMsg, wParam, lParam );
	else return FALSE;
}