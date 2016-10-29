#pragma once
extern std::vector<void(CALLBACK*)()> listMainloop;
extern std::vector<void(CALLBACK*)(IDirect3DDevice9*)> listPresent;
extern std::vector<void(CALLBACK*)(IDirect3DDevice9*)> listReset;
extern std::vector<bool(CALLBACK*)(unsigned char, RPCParameters *)> listHandleRPCPacket;
extern std::vector<bool(CALLBACK*)(int, BitStream*, PacketPriority, PacketReliability, char, bool)> listSendRPC;
extern std::vector<bool(CALLBACK*)(Packet*)> listRecive;
extern std::vector<bool(CALLBACK*)(BitStream*, PacketPriority, PacketReliability, char)> listSendPacket;

DLLEXPORTC void CALLBACK RegisterMainloop( void(CALLBACK* pFunc)() );
DLLEXPORTC void CALLBACK DestoryMainloop( void(CALLBACK* pFunc)() );

DLLEXPORTC void CALLBACK RegisterPresent( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) );
DLLEXPORTC void CALLBACK DestoryPresent( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) );

DLLEXPORTC void CALLBACK RegisterReset( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) );
DLLEXPORTC void CALLBACK DestoryReset( void(CALLBACK* pFunc)(IDirect3DDevice9* pDevice) );

DLLEXPORTC void CALLBACK RegisterHandleRPCPacket( bool(CALLBACK* pFunc)(unsigned char id, RPCParameters *rpcParams) );
DLLEXPORTC void CALLBACK DestoryHandleRPCPacket( bool(CALLBACK* pFunc)(unsigned char id, RPCParameters *rpcParams) );

DLLEXPORTC void CALLBACK RegisterSendRPC( bool(CALLBACK* pFunc)(int uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp) );
DLLEXPORTC void CALLBACK DestorySendRPC( bool(CALLBACK* pFunc)(int uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp) );

DLLEXPORTC void CALLBACK RegisterRecivePacket( bool(CALLBACK* pFunc)(Packet*p) );
DLLEXPORTC void CALLBACK DestoryRecivePacket( bool(CALLBACK* pFunc)(Packet*p) );

DLLEXPORTC void CALLBACK RegisterSendPacket( bool(CALLBACK* pFunc)(BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel) );
DLLEXPORTC void CALLBACK DestorySendPacket( bool(CALLBACK* pFunc)(BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel) );


DLLEXPORTC IDirect3DDevice9* CALLBACK GetD3DDevice();
DLLEXPORTC bool CALLBACK IsLoaded();
DLLEXPORTC RakClientInterface* CALLBACK GetRakClient();


DLLEXPORTC BitStream* CALLBACK CreateBitStream();
DLLEXPORTC BitStream* CALLBACK CreateBitStream2( int initialBytesToAllocate );
DLLEXPORTC BitStream* CALLBACK CreateBitStream3( unsigned char* _data, unsigned int lengthInBytes, bool _copyData );
DLLEXPORTC void CALLBACK DestoryBitStream( BitStream *& pBS );