#pragma once
#include "Map.h"
#include "framework.h"

class ClipBoard
{
private:
	Map::head* start;
	int style;
	enum
	{
		_and,
		_or,
		_xor
	};
public:
	ClipBoard();
	~ClipBoard();
	bool copy(RECT& rect, Map& _map);
	bool paste(RECT& rect,Map& _map);
	void set_paste_style(int _style);
private:
	void clear();
};

class Selector
{
private:
	struct SELECTINFO
	{
		POINT p1 = { 0,0 };
		POINT p2 = { 0,0 };
		bool is_active = false;
		int count = 0;
	};
private:
	//AD ad;
	SELECTINFO si;
	ClipBoard* pClipboard;
	Map* pMap;
	RECT rect;
	bool available;
public:
	Selector(Map* _map, ClipBoard* _clipboard);
	~Selector();
	void activate(bool a);
	bool is_active();
	void select_point(CPoint& pt, RECT& CliRect);
	void get_rgn(RECT& rgnrect);
	//void paint_rgn(RECT& CliRect, Graphics& g, Gdiplus::SolidBrush& brush, Gdiplus::Pen& pen);
	void select();
	void unselect();
	void add_delete_region(bool isadd,bool isrand);
	bool copy();
	bool paste();
	void set_paste_style(int _style);
	bool is_region_available();
};


