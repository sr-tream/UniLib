#define DLLEXPORT __declspec( dllexport )
#define DLLEXPORTC extern "C" DLLEXPORT

#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <assert.h>
#include <d3d9.h>
#include <d3dx9core.h>

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32")

extern IDirect3DDevice9* g_Device;
extern bool g_Initialize;
extern DWORD g_SampAddr;
extern struct stSAMP* g_SAMP;

template<typename T> void VectorErase( std::vector<T> &vec, T v );

#include "CMem.hpp"
#include "CHooks.hpp"
#include "d3drender.h"
#include "CreateTexture.h"
#include "FontInfo.h"
#include "DrawInfo.h"
#include "SampInfo.h"

#include "BitStream.h"
#include "RakClient.h"
#include "HookedRakClient.h"
#include "BSInfo.h"

#include "Interface.h"

void CALLBACK mainloop();
void CALLBACK D3DReset( IDirect3DDevice9* pDevice );
void CALLBACK D3DPresent( IDirect3DDevice9* pDevice );
bool HandleRPCPacketFunc( unsigned char id, RPCParameters *rpcParams );
bool OnSendRPC( int uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp );
bool OnReceivePacket( Packet *p );
bool OnSendPacket( BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel );


template<typename T>
inline void VectorErase( std::vector<T> &vec, T v )
{
	for ( int i = 0; i < vec.size(); ++i ){
		if ( vec[i] == v ){
			vec.erase( vec.begin() + i );
			break;
		}
	}
}