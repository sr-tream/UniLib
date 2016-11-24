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

	virtual void onDraw( int, int ) = 0;
	virtual bool onEvents( HWND, UINT, WPARAM, LPARAM ) = 0;

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

	virtual void SetEventShow( void(CALLBACK*)(bool) );

	virtual void SetMenu( CNodeMenu *menu );

protected:
	CFontInfo *_font;
	CDrawInfo *_draw;
	CNodeMenu *_menu;

	std::string _description;
	POINT _pos;
	int _height; // Высота элемента, задается из наследников
	int _width; // Длина элемента, задается из наследников
	POINT _MP;
	POINT _SO;
	void(CALLBACK* _pEventShow)(bool);

	POINT GetMousePos();
	bool isMouseOnWidget( int = 0, int = 0 );
	DWORD InvertColor( DWORD );

private:
	bool _Init;
	bool _show;
	bool _deleteFont;
};