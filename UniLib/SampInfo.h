#pragma once
#define SAMP_CHAT_INFO_OFFSET						0x21A0E4
#define SAMP_INFO_OFFSET							0x21A0F8
#define SAMP_FUNC_ADDTOCHATWND						0x64010
#define SAMP_FUNC_CNETGAMEDESTRUCTOR				0x9380
#define SAMP_HOOKENTER_HANDLE_RPC					0x3743D
#define SAMP_HOOKENTER_HANDLE_RPC2					0x373C9
#define SAMP_HOOKENTER_CNETGAME_DESTR				0xB2893
#define SAMP_HOOKENTER_CNETGAME_DESTR2				0xB3A32
#define SAMP_HOOKEXIT_HANDLE_RPC					0x37443
#define SAMP_HOOKEXIT_HANDLE_RPC2					0x37451

#pragma pack(push, 1)
struct stSAMP
{
	void					*pUnk0[2]; // 8
	uint8_t					byteSpace[24]; // 24
	char					szIP[257]; // 257
	char					szHostname[259]; // 259
	uint8_t					byteUnk1; // 1
	uint32_t				ulPort; // 4
	uint32_t				ulMapIcons[100]; // 400
	int						iLanMode; // 4
	int						iGameState; // 4
	uint32_t				ulConnectTick; // 4
	struct stServerPresets	*pSettings; // 4
	void					*pRakClientInterface; // 4
	void					*pPools; // 4
};

struct stServerPresets
{
	uint8_t byteCJWalk; // 1
	uint8_t byteUnk0[4]; // 4
	float	fWorldBoundaries[4]; // 16
	uint8_t byteUnk1; // 1
	float	fGravity; // 4
	uint8_t byteDisableInteriorEnterExits; // 1
	uint32_t ulVehicleFriendlyFire; // 4
	uint8_t byteUnk2[4]; // 4
	int		iClassesAvailable; // 4
	float	fNameTagsDistance; // 4
	uint8_t byteUnk3; // 1
	uint8_t byteWorldTime_Hour; // 1
	uint8_t byteWorldTime_Minute; // 1
	uint8_t byteWeather; // 1
	uint8_t byteNoNametagsBehindWalls; // 1
	uint8_t bytePlayerMarkersMode; // 1
	uint8_t byteUnk4[3]; // 3
	float	fGlobalChatRadiusLimit; // 4
	uint8_t byteShowNameTags; // 1
};
#pragma pack(pop)

template<typename T> inline T GetSAMPPtrInfo( uint32_t offset );

DLLEXPORTC DWORD stGetSampChatInfo();

DLLEXPORTC void AddChatMessage( DWORD color, const char *szText );

template<typename T>
inline T GetSAMPPtrInfo( uint32_t offset )
{
	if ( g_SampAddr == NULL )
		return NULL;
	return *(T *)(g_SampAddr + offset);
}