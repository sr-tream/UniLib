#pragma once
class DLLEXPORT CCreateTexture;
extern std::vector<CCreateTexture*> listCreateTexture;

class DLLEXPORT CCreateTexture
{
public:
	CCreateTexture(IDirect3DDevice9 *pDevice, int width, int height);
	~CCreateTexture();

	virtual void Begin();
	virtual void End();

	virtual bool Clear( D3DCOLOR color = 0 );
	virtual bool Render( int X, int Y, int W = -1, int H = -1, float R = 0.0f );
	//void Save(..........);

	virtual void Release();
	virtual void Init();
	virtual void ReInit( int width, int height );

	virtual ID3DXSprite* GetSprite();
	virtual IDirect3DTexture9* GetTexture();
	virtual D3DSURFACE_DESC GetSurfaceDesc();

protected:
	ID3DXSprite*			pSprite;
	IDirect3DTexture9*		pTexture;
	POINT					textureSize;
	IDirect3DDevice9*		pDevice;
	D3DSURFACE_DESC			surfaceDesc;

	bool					isRenderToTexture;
	bool					isReleased;

private:
	LPDIRECT3DSURFACE9      PP1S = NULL;
	LPDIRECT3DSURFACE9      DS = NULL;
	LPDIRECT3DSURFACE9		OldRT, OldDS;
};


DLLEXPORTC CCreateTexture* CALLBACK CreateCTexture( IDirect3DDevice9 *pDevice, int width, int height );
DLLEXPORTC void CALLBACK DestoryCTexture( CCreateTexture* &pTexture );