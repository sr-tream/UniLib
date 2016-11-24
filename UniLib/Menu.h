#pragma once

#define SCREEN_X *(int*)0x00C9C040
#define SCREEN_Y *(int*)0x00C9C044

class CMenu;
extern std::vector<CMenu*> MenuList;

struct stMenuInfo{
	CMenu*		menu;
	bool		autoRemove;
	std::string name;
};

class CMenu : public CNodeMenu
{
public:
	CMenu( std::string, POINT, std::string = "", CNode* = nullptr, bool = false );
	~CMenu();

	virtual void onDraw( int = 0, int = 0 );
	virtual bool onEvents( HWND, UINT, WPARAM, LPARAM );

	virtual bool isInizialize();

	virtual void SetSize( POINT );
	virtual POINT GetSize();

	virtual CNode* GetNode();

	virtual void SetEventMove( void(CALLBACK*)(POINT) );

	virtual void SetMenuHelper( std::string, DWORD );

protected:
	CCreateTexture *_texture;
	CNode *_node;
	
	std::string _title;
	POINT _size;
	int _header;

	bool isMouseOnHeader();
	bool isMouseOnClose();

	void(CALLBACK* _pEventMove)(POINT);

	std::vector<std::string> p_helper;
	DWORD p_color;

private:
	bool _Init;
	bool _deleteNode;

	bool _move;
	POINT _mvOffset;
};

void SetForeground( CMenu* menu );
bool IsForeground( CMenu*menu );