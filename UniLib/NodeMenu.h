#pragma once

template <class T>
union bit32{
	T v;
	byte b[sizeof( T )];
};

class CNodeMenu
{
public:
	CNodeMenu( POINT = { 0, 0 }, CFontInfo* = nullptr, bool = false );

	virtual ~CNodeMenu();

	virtual void onDraw( int, int );
	virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

	virtual bool isInizialize();

	virtual void SetPosition( POINT );
	virtual POINT GetPosition();

	virtual bool isShowed();
	virtual void SetShow( bool );

	virtual int GetHeight();
	virtual int GetWidth();

	virtual void SetDescription( std::string );
	virtual std::string GetDescription();

	virtual void SetMousePos( POINT );

	virtual void SetEventShow( void(CALLBACK*)(CNodeMenu*, bool) );
	virtual void SetEventMove( void(CALLBACK*)(CNodeMenu*, POINT) );
	virtual void SetEventClick( void(CALLBACK*)(CNodeMenu*, UINT) );

	virtual void SetMenu( CNodeMenu *menu );

protected:
	CFontInfo *_font;
	CDrawInfo *_draw;
	CNodeMenu *_menu;

	std::string _description;
	POINT _pos;
	int _height; // ������ ��������, �������� �� �����������
	int _width; // ����� ��������, �������� �� �����������
	POINT _MP;
	POINT _SO;

	void(CALLBACK* _pEventShow)(CNodeMenu*, bool);
	void(CALLBACK* _pEventMove)(CNodeMenu*, POINT);
	void(CALLBACK* _pEventClick)(CNodeMenu*, UINT);

	DWORD _colorSelect;

	POINT GetMousePos();
	bool isMouseOnWidget( int = 0, int = 0 );
	DWORD InvertColor( DWORD );

private:
	bool _Init;
	bool _show;
	bool _deleteFont;
};