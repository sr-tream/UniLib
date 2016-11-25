#pragma once

class CText : public CNodeMenu
{
public:
	CText( std::string, DWORD, POINT = { 0, 0 }, CFontInfo* = nullptr, bool = false );

	virtual bool isInizialize();

	virtual void onDraw( int = 0, int = 0 );

protected:
	std::string _text;
	DWORD _color;

private:
	bool _Init;
};