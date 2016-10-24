#include "main.h"


CCreateTexture::CCreateTexture(IDirect3DDevice9 *pDevice, const int width, const int height)
{
	this->pDevice = pDevice;
	textureSize = { width, height };
	isReleased = true;
	Init();
	isRenderToTexture = false;
	listCreateTexture.push_back( this );
}


CCreateTexture::~CCreateTexture()
{
	if (!isReleased){
		pSprite->Release();
		DS->Release();
	}
	pTexture->Release();
	VectorErase( listCreateTexture, this );
}


void CCreateTexture::Release()
{
	if (isReleased)
		return;

	pSprite->Release();
	pTexture->Release();
	DS->Release();
	PP1S->Release();
	OldRT->Release();
	OldDS->Release();
	isReleased = true;
}


void CCreateTexture::Init()
{
	if (!isReleased)
		return;

	isReleased = false;
	pDevice->CreateTexture(textureSize.x, textureSize.y, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexture, NULL);
	pTexture->GetSurfaceLevel(0, &PP1S);
	PP1S->Release();

	pDevice->CreateDepthStencilSurface(textureSize.x, textureSize.y, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, FALSE, &DS, NULL);
	D3DXCreateSprite(pDevice, &pSprite);
	pTexture->GetLevelDesc(0, &surfaceDesc);
}


void CCreateTexture::ReInit(const int width, const int height)
{
	Release();
	textureSize = { width, height };
	Init();
}


void CCreateTexture::Begin()
{
	if (isReleased){
		Init();
		return;
	}

	if (isRenderToTexture)
		return;

	pDevice->GetRenderTarget(0, &OldRT);
	pDevice->GetDepthStencilSurface(&OldDS);

	pDevice->SetDepthStencilSurface(DS);
	pDevice->SetRenderTarget(0, PP1S);

	isRenderToTexture = true;
}


void CCreateTexture::End()
{
	if (isReleased)
		return;

	if (!isRenderToTexture)
		return;

	pDevice->SetRenderTarget(0, OldRT);
	pDevice->SetDepthStencilSurface(OldDS);

	OldRT->Release();
	OldDS->Release();

	isRenderToTexture = false;
}


bool CCreateTexture::Clear(const D3DCOLOR color)
{
	if (isReleased)
		return false;

	if (!isRenderToTexture)
		return false;

	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
	return true;
}


bool CCreateTexture::Render(const int X, const int Y, int W, int H, const float R)
{
	if (isReleased)
		return false;

	if (isRenderToTexture)
		return false;

	if (W == -1)
		W = textureSize.x;
	if (H == -1)
		H = textureSize.y;

	D3DXMATRIX		mat; //Полигоны: https://dl.prime-hack.net/1r4t4.png (как понял)

	D3DXVECTOR2 axisPos = D3DXVECTOR2(X, Y);
	D3DXVECTOR2 size(1 / (float)surfaceDesc.Width * (float)W, 1 / (float)surfaceDesc.Height * (float)H);
	D3DXVECTOR2 axisCenter = D3DXVECTOR2((W / 2), (H / 2));
	D3DXMatrixTransformation2D(&mat, &axisCenter, 0.0f, &size, &axisCenter, R, &axisPos);

	pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);
	pSprite->SetTransform(&mat);
	pSprite->Draw(pTexture, NULL, NULL, NULL, -1);
	pSprite->End();
	return true;
}


ID3DXSprite* CCreateTexture::GetSprite()
{
	if (isReleased)
		return nullptr;

	return this->pSprite;
}


IDirect3DTexture9* CCreateTexture::GetTexture()
{
	if (isReleased)
		return nullptr;

	return this->pTexture;
}


D3DSURFACE_DESC CCreateTexture::GetSurfaceDesc()
{
	return this->surfaceDesc;
}


DLLEXPORTC CCreateTexture* CALLBACK CreateTexture( IDirect3DDevice9 *pDevice, int width, int height )
{
	return new CCreateTexture( pDevice, width, width );
}

DLLEXPORTC void CALLBACK DestoryTexture( CCreateTexture* &pTexture )
{
	delete pTexture;
	pTexture = nullptr;
}