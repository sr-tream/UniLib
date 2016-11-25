#pragma once

#define SCREEN_X *(int*)0x00C9C040
#define SCREEN_Y *(int*)0x00C9C044

class CMenu;
extern std::vector<CMenu*> MenuList;

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

	virtual void SetMenuHelper( std::string );

	virtual void SetContextMenu( CContextMenu*, bool = false );

	virtual bool isActive();

protected:
	CCreateTexture *_texture;
	CNode *_node;
	CContextMenu *_context;
	bool _removeContext;
	
	std::string _title;
	POINT _size;
	int _header;

	bool isMouseOnHeader();
	bool isMouseOnClose();

	std::vector<std::string> p_helper;

private:
	bool _Init;
	bool _deleteNode;

	bool _move;
	POINT _mvOffset;
};

void SetForeground( CMenu* menu );
bool IsForeground( CMenu*menu );