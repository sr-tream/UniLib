#pragma once

struct stNodeChield{
	CNodeMenu*	node;
	bool		autoRemove;
	std::string name;
};

class CNode : public CNodeMenu
{
public:
	CNode( POINT = { 0, 0 }, POINT = { 100, 100 } );
	virtual ~CNode();

	virtual void onDraw( int = 0, int = 0 );
	virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

	virtual bool isInizialize();

	virtual void SetSize( POINT );
	virtual POINT GetSize();

	virtual int GetScrollVertical();
	virtual void SetScrollVertical( int );
	virtual int GetScrollHorizontal();
	virtual void SetScrollHorizontal( int );

	virtual void SetScrollStep( int );
	virtual void GetScrollColor( DWORD&, DWORD&, DWORD& );
	virtual void SetScrollColor( DWORD, DWORD, DWORD );

	virtual bool AddChield( CNodeMenu*, std::string = "", bool = false );
	virtual void DelChield( CNodeMenu* );
	virtual void DelChield( std::string );
	virtual CNodeMenu* GetChield( std::string );

	virtual DWORD GetColorBkg();
	virtual void SetColorBkg( DWORD );

protected:
	CCreateTexture *_texture;
	std::vector<stNodeChield> _nodes;

	POINT _size;
	int _scrollOffsetVertical;
	int _scrollOffsetHorizontal;
	int _scrollSizeVertical;
	int _scrollSizeHorizontal;
	int _scrollStep;
	DWORD _scrollFrame;
	DWORD _scrollMat;
	DWORD _scrollRoller;

	DWORD _colorBkg;

	void DrawScrollBarVertical( int = 0, int = 0 );
	void DrawScrollBarHorizontal( int = 0, int = 0 );
	bool isMouseInNode( int = 0, int = 0 );

private:
	bool _Init;
};