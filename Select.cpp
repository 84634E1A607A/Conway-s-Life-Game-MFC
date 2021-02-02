#include "pch.h"
#include "Select.h"

ClipBoard::ClipBoard()
{
	start = nullptr;
	style = _or ;
}

ClipBoard::~ClipBoard()
{
	clear();
}

bool ClipBoard::copy(RECT& rect, Map& _map)
{
	clear();
	Map::head* ptrh = &_map.cur,*phnow = start;
	Map::node* ptrn = nullptr,*pnnow = nullptr;
	while (ptrh->x < rect.left && ptrh->pnext != nullptr);
	if (ptrh->x > rect.right) return false;
	while (ptrh->x >= rect.left && ptrh->x <= rect.right && ptrh->pnext != nullptr)
	{
		ptrn = ptrh->pnode;
		if (ptrn == nullptr)continue;
		phnow->pnext = new Map::head;
		phnow = phnow->pnext;
		phnow->pnext = nullptr;
		phnow->pnode = nullptr;
		phnow->x = ptrh->x - rect.left;
		while (ptrn != nullptr)
		{
			while (ptrn->y < rect.top && ptrn->pnext != nullptr);
			if (ptrn->y > rect.bottom)break;
			phnow->pnode = new Map::node;
			pnnow = phnow->pnode;
			pnnow->pnext = nullptr;
			pnnow->y = ptrn->y - rect.left;
			pnnow->state = ptrn->state;
		}
	}
	return start != nullptr;
}

bool ClipBoard::paste(RECT& rect, Map& _map)
{
	if (start == nullptr) return false;
	Map::head* phnow = start, * phmap = &_map.cur, * phmapprev = nullptr;
	Map::node* pnnow = nullptr, * pnmap = nullptr;
	bool can_findm,can_findc;
	
	for (int i = 0; i <= rect.right - rect.left; i++)
	{
		while (phnow->pnext != nullptr && phnow->x < i) phnow = phnow->pnext;
		while (phmap->pnext != nullptr && phmap->x < i + rect.left) 
		{
			phmapprev = phmap; phmap = phmap->pnext;
		}
		if (phnow->pnext != nullptr) pnnow = phnow->pnode;
		if (phmap->pnext != nullptr) pnmap = phmap->pnode;
		for (int j = 0; j <= rect.bottom - rect.top; j++)
		{
			while (pnnow != nullptr && pnnow->pnext != nullptr && pnnow->y < i) pnnow = pnnow->pnext;
			while (pnmap != nullptr && pnmap->pnext != nullptr && pnmap->y < i + rect.top) pnmap = pnmap->pnext;
			can_findc = pnnow != nullptr && phnow->x == i && pnnow->y == j;
			can_findm = pnmap != nullptr && phmap->x == i + rect.left && pnmap->y == j + rect.top;
			switch (style)
			{
			case _and:
			{
				_map.change(i + rect.left, j + rect.top, (can_findc && can_findm) ? 1 : 2, phmapprev);
				break;
			}
			case _or:
			{
				_map.change(i + rect.left, j + rect.top, (can_findc || can_findm) ? 1 : 2, phmapprev);
				break;
			}
			case _xor:
			{
				_map.change(i + rect.left, j + rect.top, (can_findc == can_findm) ? 1 : 2, phmapprev);
				break;
			}
			default:
				break;
			}
		}
	}
	
	return true;
}

void ClipBoard::set_paste_style(int _style)
{
	style = _style;
}

void ClipBoard::clear()
{
	if (start == nullptr)return;
	auto dptrh = start,fptrh = start;
	while (fptrh != nullptr)
	{
		fptrh = fptrh->pnext;
		auto dptrn = dptrh->pnode, fptrn = fptrh->pnode;
		while (fptrn != nullptr)
		{
			fptrn = fptrn->pnext;
			delete dptrn;
			dptrn = fptrn;
		}
		delete dptrh;
		dptrh = fptrh;
	}
}



Selector::Selector(Map* _map, ClipBoard* _clipboard)
{
	rect = { 0,0,0,0 };
	pMap = _map;
	pClipboard = _clipboard;
	available = 0;
}

Selector::~Selector()
{
}

void Selector::activate(bool a)
{
	si.is_active = a;
	if (!a) si.count = 0;
}

bool Selector::is_active()
{
	return si.is_active;
}

void Selector::select_point(CPoint& pt, RECT& CliRect)
{
	if (!is_active())return;
	CPoint midpt((CliRect.right-CliRect.left)/2,(CliRect.bottom-CliRect.top)/2);
	if (si.count == 0)
	{
		si.p1.x = (int)((pt.x - midpt.x + 0x1000 * side_length) / side_length - 0x1000 + xpivot), si.p1.y = (int)((si.p2.y - midpt.y + 0x1000 * side_length) / side_length - 0x1000 + ypivot);
		si.count++;
	}
	else
	{
		si.p2.x = (int)((pt.x - midpt.x + 0x1000 * side_length) / side_length - 0x1000 + xpivot), si.p2.y = (int)((si.p2.y - midpt.y + 0x1000 * side_length) / side_length - 0x1000 + ypivot);
		si.count++;
	}
}



void Selector::paint_rgn(RECT& CliRect, Graphics& g, Gdiplus::SolidBrush& brush, Gdiplus::Pen& pen)
{
	int mid_x = CliRect.right / 2, mid_y = CliRect.bottom / 2;
	CPoint p1, p2;
	p1.x = min(si.p1.x, si.p2.x);
	p1.y = min(si.p1.y, si.p2.y);
	p2.x = max(si.p1.x, si.p2.x);
	p2.y = max(si.p1.y, si.p2.y);
	int left = (p1.x - xpivot) * side_length + mid_x;
	int top = (p1.y - ypivot) * side_length + mid_y;
	int width = (p2.x - p1.x) * side_length;
	int height = (p2.y - p1.y) * side_length;

	g.DrawRectangle(&pen, left, top, width, height);
	g.FillRectangle(&brush, left, top, width, height);

}

void Selector::select()
{
	rect = CRect(si.p1, si.p2);
	available = true;
}

void Selector::unselect()
{
	rect = { 0,0,0,0 };
	available = false;
}

void Selector::add_delete_region(bool isadd, bool isrand)
{
	if (is_region_available())
		pMap->add_delete_region(rect, isadd, isrand);
}


bool Selector::copy()
{
	if (is_region_available())
	{
		return pClipboard->copy(rect, *pMap);
	}
	return false;
}

bool Selector::paste()
{
	if (is_region_available())
	{
		return pClipboard->paste(rect, *pMap);
	}
	return false;
}

void Selector::set_paste_style(int _style)
{
	pClipboard->set_paste_style(_style);
}

bool Selector::is_region_available()
{
	return available;
}


