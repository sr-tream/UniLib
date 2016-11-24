#pragma once

class CTextClickable : public CText
{
public:
	CTextClickable( std::string, DWORD, void(CALLBACK*)(UINT), POINT = { 0, 0 }, CFontInfo* = nullptr, bool = false );

	virtual void onDraw( int = 0, int = 0 );
	virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

	virtual void SetCallback( void(CALLBACK*)(UINT) );

protected:
	void(CALLBACK* _pClick)(UINT);
};