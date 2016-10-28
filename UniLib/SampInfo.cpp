#include "main.h"

DLLEXPORTC DWORD stGetSampChatInfo()
{
	return *(DWORD*)(g_SampAddr + SAMP_CHAT_INFO_OFFSET);
}

DLLEXPORTC void AddChatMessage( DWORD color, const char *szText )
{
	void( __thiscall *AddToChatWindowBuffer ) (void *, int, const char *, const char *, D3DCOLOR, D3DCOLOR) =
		(void( __thiscall * ) (void *_this, int Type, const char *szString, const char *szPrefix, D3DCOLOR TextColor, D3DCOLOR PrefixColor))
		(g_SampAddr + SAMP_FUNC_ADDTOCHATWND);

	AddToChatWindowBuffer( (void*)stGetSampChatInfo(), 8, szText, nullptr, color, 0 );
}