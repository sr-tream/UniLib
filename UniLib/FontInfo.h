#pragma once
class DLLEXPORT CFontInfo;
extern std::vector<CFontInfo*> listFontInfo;

class DLLEXPORT CFontInfo
{
public:
	CFontInfo( const char *szFontName, int fontHeight, DWORD dwCreateFlags );
	~CFontInfo();
	virtual void Initialize( IDirect3DDevice9* pDevice );
	virtual void Invalidate();
	virtual void Print( DWORD color, const char* szText, int X, int Y );
	virtual void PrintShadow( DWORD color, const char* szText, int X, int Y );
	virtual float GetHeight();
	virtual float GetWidth( const char* szText );
private:
	CD3DFont *_pFont;
	IDirect3DDevice9* _pDevice;
	bool _init = false;
};

DLLEXPORTC CFontInfo* CALLBACK CreateFontInfo( const char *szFontName, int fontHeight, DWORD dwCreateFlags );
DLLEXPORTC void CALLBACK DestoryFontInfo( CFontInfo* &pFont );