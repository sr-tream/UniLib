#pragma once

class CContextMenu : public CNodeMenu
{
public:
	CContextMenu( void(CALLBACK*)(CContextMenu*, int), DWORD = 0xE8283848 );
	virtual ~CContextMenu();

	virtual void onDraw( int = 0, int = 0 );
	virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

	virtual bool isInizialize();

	virtual void SetPosition( POINT );

	virtual void AddVariant( std::string, DWORD = -1 );
	virtual void SetDescription( std::string );

protected:
	std::vector<CText*> _vars;
	void(CALLBACK* _pCall)(CContextMenu*, int);

	DWORD _color;

	bool isMouseOnVariant( int );

private:
	bool _Init;
};