#pragma once

class CListing : public CNode
{
public:
	CListing( POINT, POINT );
	~CListing();

	virtual void onDraw( int = 0, int = 0 );
	virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

	virtual bool isInizialize();

	virtual void SetSize( POINT );

	virtual bool AddChield( CNodeMenu*, std::string = "", bool = false );
	virtual void DelChield( CNodeMenu* );
	virtual void DelChield( std::string );
	virtual CNodeMenu* GetChield( std::string );

protected:
	CVerticalLayout* _layout;

private:
	bool _Init;
};