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

DLLEXPORTC void CALLBACK RegisterHandleRPCPacket( bool(CALLBACK* pFunc)(unsigned char id, RPCParameters *rpcParams) )
{
	if ( pFunc == nullptr )
		return;

	listHandleRPCPacket.push_back( pFunc );
}
DLLEXPORTC void CALLBACK DestoryHandleRPCPacket( bool(CALLBACK* pFunc)(unsigned char id, RPCParameters *rpcParams) )
{
	if ( pFunc == nullptr )
		return;

	VectorErase( listHandleRPCPacket, pFunc );
}

DLLEXPORTC void CALLBACK RegisterSendRPC( bool(CALLBACK* pFunc)(int uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp) )
{
	if ( pFunc == nullptr )
		return;

	listSendRPC.push_back( pFunc );
}
DLLEXPORTC void CALLBACK DestorySendRPC( bool(CALLBACK* pFunc)(int uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp) )
{
	if ( pFunc == nullptr )
		return;

	VectorErase( listSendRPC, pFunc );
}

DLLEXPORTC void CALLBACK RegisterRecivePacket( bool(CALLBACK* pFunc)(Packet*p) )
{
	if ( pFunc == nullptr )
		return;

	listRecive.push_back( pFunc );
}
DLLEXPORTC void CALLBACK DestoryRecivePacket( bool(CALLBACK* pFunc)(Packet*p) )
{
	if ( pFunc == nullptr )
		return;

	VectorErase( listRecive, pFunc );
}

DLLEXPORTC void CALLBACK RegisterSendPacket( bool(CALLBACK* pFunc)(BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel) )
{
	if ( pFunc == nullptr )
		return;

	listSendPacket.push_back( pFunc );
}
DLLEXPORTC void CALLBACK DestorySendPacket( bool(CALLBACK* pFunc)(BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel) )
{
	if ( pFunc == nullptr )
		return;

	VectorErase( listSendPacket, pFunc );
}

DLLEXPORTC void CALLBACK RegisterWndProc( bool(CALLBACK* pFunc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) )
{
	if ( pFunc == nullptr )
		return;

	listWndProc.push_back( pFunc );
}
DLLEXPORTC void CALLBACK DestoryWndProc( bool(CALLBACK* pFunc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) )
{
	if ( pFunc == nullptr )
		return;

	VectorErase( listWndProc, pFunc );
}


DLLEXPORTC IDirect3DDevice9* CALLBACK GetD3DDevice()
{
	return g_Device;
}

DLLEXPORTC bool CALLBACK IsLoaded()
{
	return g_Initialize;
}

DLLEXPORTC RakClientInterface* CALLBACK GetRakClient()
{
	return (RakClientInterface*)g_SAMP->pRakClientInterface;
}


DLLEXPORTC BitStream* CALLBACK CreateBitStream()
{
	return new BitStream();
}
DLLEXPORTC BitStream* CALLBACK CreateBitStream2( int initialBytesToAllocate )
{
	return new BitStream( initialBytesToAllocate );
}
DLLEXPORTC BitStream* CALLBACK CreateBitStream3( unsigned char* _data, unsigned int lengthInBytes, bool _copyData )
{
	return new BitStream(_data, lengthInBytes, _copyData);
}
DLLEXPORTC void CALLBACK DestoryBitStream( BitStream *& pBS )
{
	delete pBS;
	pBS = nullptr;
}