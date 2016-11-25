#pragma once

class CVerticalLayout : public CNode
{
public:
	CVerticalLayout( POINT = { 0, 0 } );

	virtual void onDraw( int = 0, int = 0 );
	virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

	virtual void SetPosition( POINT );
	virtual void SetSize( POINT );

	virtual bool AddChield( CNodeMenu*, std::string = "", bool = false );
	virtual void DelChield( CNodeMenu* );
	virtual void DelChield( std::string );

protected:
	void DrawLayout( int = 0, int = 0 );

private:
	bool _Init;
};