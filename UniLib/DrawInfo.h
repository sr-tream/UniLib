#pragma once
class DLLEXPORT CDrawInfo;
extern std::vector<CDrawInfo*> listDrawInfo;

class DLLEXPORT CDrawInfo
{
public:
	CDrawInfo( int numVertices );
	~CDrawInfo();
	virtual void Box( int X, int Y, int W, int H, DWORD color );
	virtual void BorderBox( int X, int Y, int W, int H, DWORD boreder_color, DWORD color );
	virtual void Line( int X, int Y, int X2, int Y2, DWORD color );
	virtual void Initialize( IDirect3DDevice9* pDevice );
	virtual void Invalidate();
private:
	CD3DRender* _pDraw;
	IDirect3DDevice9* _pDevice;
	bool _init;
};

DLLEXPORTC CDrawInfo* CALLBACK CreateDrawInfo( int numVertices );
DLLEXPORTC void CALLBACK DestoryDrawInfo( CDrawInfo* &pDraw );