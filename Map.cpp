#include "pch.h"
#include "Map.h"
#include "Life-MFC.h"
#include "MainFrm.h"
#include "ChildView.h"
#include "DlgOptions.h"
#include "CCalcThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int side_length = 10;
int xpivot = 0x08000000, ypivot = 0x08000000;
bool started, headpool_usage_need_refresh, nodepool_usage_need_refresh;
unsigned int selected_builtin, selected_direction, kbd_input_state, TIMER = 500;
const unsigned int move_length = 30;
char ids_help_about[256], ids_help_help[1024];
extern DlgOptions theDlg;
CWinThread* pCalcThread;


#ifdef REALTIME_NEW
unsigned int head_count, hstate, node_count, nstate;
#endif

inline void redraw() {
	theApp.m_pMainWnd->RedrawWindow(nullptr, nullptr, RDW_INVALIDATE);
}

inline void change_xpivot() {
	WCHAR xpivot_c[16];
	if (xpivot >= 0x10000000) xpivot = 0xfffffff;
	if (xpivot <= 0) xpivot = 0;
	wsprintf(xpivot_c, L"%0x", xpivot);
	theDlg.SetDlgItemText(IDC_XPIVOT, xpivot_c);
}

inline void change_ypivot() {
	WCHAR ypivot_c[16];
	if (ypivot >= 0x10000000) ypivot = 0xfffffff;
	if (ypivot <= 0) ypivot = 0;
	wsprintf(ypivot_c, L"%0x", ypivot);
	theDlg.SetDlgItemText(IDC_YPIVOT, ypivot_c);
}

Map::Map() {
#ifdef REALTIME_NEW
	head_pool = new head, node_pool = new node;
	cur.pnext = pre.pnext = nxt.pnext = nullptr;
#else
	head_pool = new Map::head[SIZE];
	node_pool = new Map::node[SIZE];
	memset(head_pool, 0, SIZE * sizeof(Map::head));
	memset(node_pool, 0, SIZE * sizeof(Map::node));
	for (int i = 0; i < SIZE; i++) head_pool[i].pnext = &head_pool[i + 1];
	for (int i = 0; i < SIZE; i++) node_pool[i].pnext = &node_pool[i + 1];
	head_pool[SIZE - 1].pnext = nullptr;  node_pool[SIZE - 1].pnext = nullptr;
	cur.pnext = nullptr;
#endif
	phead_pools = { head_pool, nullptr, nullptr };
	pnode_pools = { nullptr, node_pool, nullptr };
	init_builtins();
	pCalcThread = AfxBeginThread(RUNTIME_CLASS(CCalcThread));
}

//type: {0: 1->0, 0->1; 1: 0,1->1; 2: 0,1->0}
Map::head* Map::change(int xpos, int ypos, int type, head* pacceh) {
	Map::head* px = &cur;
	if (pacceh && pacceh->x <= xpos) px = pacceh;
	while (px->pnext && px->pnext->x <= xpos) px = px->pnext;
	if (px->x == xpos && px->pnode) {
		Map::node* py = px->pnode;
		while (py->pnext && py->pnext->y < ypos) py = py->pnext;
		if (py->pnext && py->pnext->y == ypos) { if (type != 1) del(py); }	//If the Map::node already exists: destroy the Map::node
		else if (type != 2) {										//If the Map::node doesn't exist: insert a Map::node
			Map::node* pn = insert(py);
			pn->y = ypos;
			pn->state = true;
		}
	}
	else if (type != 2) {											//If the row doesn't exist: insert Map::head and Map::node
		Map::head* pn = insert(px);
		Map::node* pnode = insert(pn->pnode);
		pn->x = xpos;
		pnode->y = ypos;
		pnode->state = true;
	}
	return px;
}

void Map::calc() {
	clock_t ts = clock();
	//Sleep(10000);//test for long time calculating
	Map::head* px = cur.pnext, *pacce = nullptr, *ptmp = nullptr;
	while (px) {
		Map::node* py = px->pnode->pnext;
		for (; py; py = py->pnext) {
			if (!py->state) continue;
			int x = px->x;
			int y = py->y;
			ptmp = pacce;
			ptmp = add(x-1, y-1, ptmp);
			pacce = ptmp;
			add(x-1, y  , ptmp);
			add(x-1, y+1, ptmp);
			ptmp = add(x  , y-1, ptmp);
			add(x  , y  , ptmp);
			add(x  , y+1, ptmp);
			ptmp = add(x+1, y-1, ptmp);
			add(x+1, y  , ptmp);
			add(x+1, y+1, ptmp);
			mark(x, y);
		}
		px = px->pnext;
	}
	clear(&cur);
	px = nxt.pnext;
	while (px) {
		Map::node* py = px->pnode->pnext;
		for (; py; py = py->pnext) if ((py->count == 3) || py->state && py->count == 4) change(px->x, py->y);
		px = px->pnext;
	}
	clear(&nxt);
	clock_t te = clock();
	clock_t t = te - ts;
	TCHAR s[16];
	_itow_s(t, s, 10);
	if (theDlg.GetSafeHwnd())
		theDlg.SetDlgItemText(IDC_CALC_TIME, s);
}

void Map::clear() {
	Sleep(TIMER);
	xpivot = ypivot = 0x08000000;
#ifdef REALTIME_NEW
	clear(&pre);
	clear(&cur);
	cur.pnext = nullptr, nxt.pnext = nullptr, ppre = nullptr;
	head_count = node_count = 0;
	theDlg.SetDlgItemText(IDC_HEADPOOL_USAGE, L"0");
	theDlg.SetDlgItemText(IDC_NODEPOOL_USAGE, L"0");
	redraw_erase();
#else
	memset(head_pool, 0, SIZE * sizeof(Map::head));
	memset(node_pool, 0, SIZE * sizeof(Map::node));
	for (int i = 0; i < SIZE; i++) head_pool[i].pnext = head_pool + i + 1;
	for (int i = 0; i < SIZE; i++) node_pool[i].pnext = node_pool + i + 1;
	head_pool[SIZE - 1].pnext = nullptr;  node_pool[SIZE - 1].pnext = nullptr;
	cur.pnext = nullptr, nxt.pnext = nullptr;
	
	redraw();

	ppool* phead = phead_pools.pnext, * pdel = phead;
	while (phead != nullptr) {
		delete[] phead->phead;
		phead = phead->pnext;
		delete pdel;
		pdel = phead;
	}
	phead_pools.pnext = nullptr;

	ppool* pnode = pnode_pools.pnext; pdel = pnode;
	while (pnode != nullptr) {
		delete[] pnode->pnode;
		pnode = pnode->pnext;
		delete pdel;
		pdel = pnode;
	}
	pnode_pools.pnext = nullptr;
	headpool_usage = nodepool_usage = 1;
	refresh_headpool_usage(), refresh_nodepool_usage();
#endif
	change_xpivot(), change_ypivot();
}

void Map::add_builtin(const int& xpos, const int& ypos, const unsigned int& b, const unsigned int& d) {
	if (b >= 10 || d >= 8) return;
	unsigned int s = builtins[b].size, l = builtins[b].length - 1, h = builtins[b].height - 1;
	const POINT* cur = builtins[b].points;
	switch (d) {
	case 0:
		for (unsigned int i = 0; i < s; i++) change(xpos + cur[i].x, ypos + cur[i].y, 1);
		break;
	case 1:
		for (unsigned int i = 0; i < s; i++) change(xpos + cur[i].x, ypos + h - cur[i].y, 1);
		break;
	case 2:
		for (unsigned int i = 0; i < s; i++) change(xpos + l - cur[i].x, ypos + cur[i].y, 1);
		break;
	case 3:
		for (unsigned int i = 0; i < s; i++) change(xpos + l - cur[i].x, ypos + h - cur[i].y, 1);
		break;
	case 4:
		for (unsigned int i = 0; i < s; i++) change(xpos + cur[i].y, ypos + cur[i].x, 1);
		break;
	case 5:
		for (unsigned int i = 0; i < s; i++) change(xpos + cur[i].y, ypos + l - cur[i].x, 1);
		break;
	case 6:
		for (unsigned int i = 0; i < s; i++) change(xpos + h - cur[i].y, ypos + cur[i].x, 1);
		break;
	case 7:
		for (unsigned int i = 0; i < s; i++) change(xpos + h - cur[i].y, ypos + l - cur[i].x, 1);
		break;
	}
}

void Map::add_delete_region(RECT& rect, bool isadd, bool isrand) {
	head* pacce = nullptr;
	if (isadd) {
		if (isrand) {
			for (int x = rect.left; x <= rect.right; x++)
				for (int y = rect.bottom; y >= rect.top; y--)
					pacce = change(x, y, rand() % 3, pacce);
		}
		else {
			for (int x = rect.left; x <= rect.right; x++)
				for (int y = rect.bottom; y >= rect.top; y--)
					pacce = change(x, y, 1, pacce);
		}
	}
	else {
		for (int x = rect.left; x <= rect.right; x++)
			for (int y = rect.bottom; y >= rect.top; y--)
				pacce = change(x, y, 2, pacce);
	}
}

void Map::draw(CDC& cdc, RECT& rect) {
	int mid_x = rect.right / 2, mid_y = rect.bottom / 2;
	int left = (-mid_x) / side_length + xpivot - 1, right = (rect.right - mid_x) / side_length + xpivot + 1;
	int top = (-mid_y) / side_length + ypivot - 1, bottom = (rect.bottom - mid_y) / side_length + ypivot + 1;
	Map::head* pl = &cur;
	while (pl->pnext && pl->pnext->x < left) pl = pl->pnext;
	Map::head* px = pl;
	while (px->pnext && px->pnext->x <= right) {
		Map::node* py = px->pnext->pnode;
		while (py->pnext && py->pnext->y < top) py = py->pnext;
		while (py->pnext && py->pnext->y <= bottom) {
			RECT fill_rect = {
				(LONG)((px->pnext->x - xpivot) * side_length + mid_x + 1), //left
				(LONG)((py->pnext->y - ypivot) * side_length + mid_y + 1), //top
				(LONG)((px->pnext->x - xpivot + 1) * side_length + mid_x), //right
				(LONG)((py->pnext->y - ypivot + 1) * side_length + mid_y)  //bottom
			};
			cdc.FillSolidRect(&fill_rect, RGB(0, 0, 0));
			py = py->pnext;
		}
		px = px->pnext;
	}
}

RECT Map::get_builtin_info(int b) {
	if (b < 0 || b > 10) return { 0, 0, 0, 0 };
	return { 0, 0, builtins[b].length, builtins[b].height };
}

void Map::draw_builtin(CDialog* Dlg, const unsigned int& b, const unsigned int& d) {
	//Init
	const int xl = 71, yl = 58;
	CWnd* hPreview = Dlg->GetDlgItem(IDC_PREVIEW);
	if (!hPreview) return;
	const builtin& bi = builtins[b];
	CDC* dc = hPreview->GetDC();

	//Clear
	RECT erase_rect = { 1, 1, xl, yl };
	dc->FillSolidRect(&erase_rect, RGB(255, 255, 255));
	if (b >= 10 || d >= 8 || !bi.size) return;

	//Calc
	int length = d & 4 ? bi.height : bi.length, height = (d & 4 ? bi.length : bi.height);
	int xr = xl / length, yr = yl / height, r = xr < yr ? xr : yr;
	if (!r) return;
	int xs = ((xl - r * length) >> 1) + 1, ys = ((yl - r * height) >> 1) + 1;

	//Draw
	for (int i = 0; i < bi.size; i++) {
		int x = bi.points[i].x, y = bi.points[i].y;
		int l = length - 1, h = height - 1;
		if (d & 4) x = y, y = bi.points[i].x;
		switch (d) {
		case 0:
			break;
		case 2:
		case 6:
			x = l - x;
			break;
		case 1:
		case 5:
			y = h - y;
			break;
		case 3:
		case 7:
			x = l - x, y = h - y;
			break;
		}
		RECT rect = { x * r + xs, y * r + ys, (x + 1) * r + xs, (y + 1) * r + ys };
		dc->FillSolidRect(&rect, RGB(0, 0, 0));
	}
	RECT click_rect = { xs, ys, xs + r, ys + r };
	dc->FillSolidRect(&click_rect, RGB(255, 0, 0));
}

LPCTSTR Map::get_size() {
	static WCHAR size[5];
	_itow_s(SIZE, size, 10);
	return size;
}


void Map::load(CString& fname)
{
	started = false;
	try
	{
		CFile file;
		if (!file.Open(fname.GetString(), CFile::modeRead))
		{
			throw L"Cannot open file";
		}
		clear();
		file.Read(&xpivot, sizeof(xpivot));
		file.Read(&ypivot, sizeof(ypivot));
		unsigned int tmp;
		file.Read(&tmp, sizeof(tmp));
		if (tmp != 0xffffffff)
		{
			clear();
			throw L"Bad file! Map reset";
		}
		while (true) {
			unsigned int x, y, m, n;
			m = file.Read(&x, sizeof(x));
			n = file.Read(&y, sizeof(y));
			if (x == 0xffffffff && y == 0xfffffffd) break;
			if (!n) {
				clear();
				throw L"Bad file! Map reset";
			}
			change(x, y);
		}
		redraw();
	}
	catch (const LPCTSTR msg) 
	{
		theApp.m_pMainWnd->MessageBox(msg, L"Error", MB_OK);
	}
}


void Map::dump(CString& fname)
{
	started = false;
	CFile file;
	if (!file.Open(fname.GetString(), CFile::modeWrite|CFile::modeCreate))
	{
		theApp.m_pMainWnd->MessageBox(L"Save File Failed!", L"Error", MB_OK);
		return;
	}
	unsigned int tmp[2] = { 0xffffffff, 0xfffffffd };
	file.Write(&xpivot, sizeof(xpivot));
	file.Write(&ypivot, sizeof(ypivot));
	file.Write(tmp, sizeof(tmp[0]));
	Map::head* ph = &cur;
	while (ph->pnext) {
		ph = ph->pnext;
		unsigned int x = ph->x;
		Map::node* pn = ph->pnode;
		while (pn->pnext) {
			pn = pn->pnext;
			if (!pn->state) continue;
			file.Write(&x, sizeof(x));
			file.Write(&(pn->y), sizeof(pn->y));
		}
	}
	file.Write(tmp, sizeof(tmp));
}

void Map::free_extra() {
	struct POINT_LINK { POINT point; POINT_LINK* pnext; } 
		point_link_head = { {0,0}, nullptr }, * point_link_end = &point_link_head, * point_link_node = &point_link_head, *point_link_del;
	head* ph = &cur;
	while (ph->pnext) {
		ph = ph->pnext;
		unsigned int x = ph->x;
		node* pn = ph->pnode;
		while (pn->pnext) {
			pn = pn->pnext;
			if (!pn->state) continue;
			point_link_end = point_link_end->pnext = new POINT_LINK;
			point_link_end->point = { (long)x, (long)pn->y }, point_link_end->pnext = nullptr;
		}
	}
	unsigned int x = xpivot, y = ypivot;
	clear();
	xpivot = x, ypivot = y;
	while (point_link_node->pnext) {
		point_link_node = point_link_node->pnext;
		change(point_link_node->point.x, point_link_node->point.y);
	}
	point_link_node = point_link_head.pnext;
	while (point_link_node) {
		point_link_del = point_link_node;
		point_link_node = point_link_node->pnext;
		delete point_link_del;
	}
}


Map::head* Map::enlarge_head_pool() {
	Map::head* nhead_pool = new Map::head[SIZE];
	memset(nhead_pool, 0, SIZE * sizeof(Map::head));
	for (int i = 0; i < SIZE; i++) nhead_pool[i].pnext = &nhead_pool[i + 1];
	nhead_pool[SIZE - 1].pnext = nullptr;
	head_pool->pnext = nhead_pool;

	ppool* nphead_pool = new ppool;
	nphead_pool->phead = nhead_pool;
	nphead_pool->pnext = phead_pools.pnext;
	phead_pools.pnext = nphead_pool;

	headpool_usage++;
	headpool_usage_need_refresh = true;

	return nhead_pool;
}

Map::node* Map::enlarge_node_pool() {
	Map::node* nnode_pool = new Map::node[SIZE];
	memset(nnode_pool, 0, SIZE * sizeof(Map::node));
	for (int i = 0; i < SIZE; i++) nnode_pool[i].pnext = nnode_pool + i + 1;

	nnode_pool[SIZE - 1].pnext = nullptr;
	node_pool->pnext = nnode_pool;

	ppool* npnode_pool = new ppool;
	npnode_pool->pnode = nnode_pool;
	npnode_pool->pnext = pnode_pools.pnext;
	pnode_pools.pnext = npnode_pool;

	nodepool_usage++;
	nodepool_usage_need_refresh = true;

	return nnode_pool;
}

inline void Map::refresh_headpool_usage() {
	WCHAR headpool_usage_c[8] = L"";
	_itow_s(headpool_usage, headpool_usage_c, 10);
	theDlg.GetDlgItem(IDC_HEADPOOL_USAGE)->SendMessage(WM_SETTEXT, 0, (LPARAM)headpool_usage_c);
}
//
inline void Map::refresh_nodepool_usage() {
	WCHAR nodepool_usage_c[8] = L"";
	_itow_s(nodepool_usage, nodepool_usage_c, 10);
	theDlg.GetDlgItem(IDC_NODEPOOL_USAGE)->SendMessage(WM_SETTEXT, 0, (LPARAM)nodepool_usage_c);
}

Map::head* Map::insert(Map::head* p) {
#ifdef REALTIME_NEW
	head* pn = new head;
	pn->pnext = p->pnext;
	p->pnext = pn;
	node* pnode = new node;
	pn->pnode = pnode;
	memset(pnode, 0, sizeof(node));
	WCHAR c[16];
	if (!hstate) {
		_itow_s(head_count, c, 10);
		theDlg.SetDlgItemText(IDC_HEADPOOL_USAGE, c);
		hstate = 1;
	}
	head_count++;
	if (!nstate) {
		_itow_s(node_count, c, 10);
		theDlg.SetDlgItemText(IDC_NODEPOOL_USAGE, c);
		nstate = 1;
	}
	node_count++;
	return pn;
#else
	Map::head* pn = head_pool->pnext ? head_pool->pnext : enlarge_head_pool();
	head_pool->pnext = pn->pnext;
	pn->pnext = p->pnext;
	p->pnext = pn;

	Map::node* pnode = node_pool->pnext ? node_pool->pnext : enlarge_node_pool();
	node_pool->pnext = pnode->pnext;
	pn->pnode = pnode;
	pnode->pnext = nullptr;
	return pn;
#endif
}

Map::node* Map::insert(node* p) {
#ifdef REALTIME_NEW
	node* pn = new node;
	memset(pn, 0, sizeof(node));
	pn->pnext = p->pnext;
	p->pnext = pn;
	WCHAR c[16];
	if (!nstate) {
		_itow_s(node_count, c, 10);
		theDlg.SetDlgItemText(IDC_NODEPOOL_USAGE, c);
		nstate = 1;
	}
	node_count++;
	return pn;
#else
	Map::node* pn = node_pool->pnext ? node_pool->pnext : enlarge_node_pool();
	node_pool->pnext = pn->pnext;
	pn->pnext = p->pnext;
	p->pnext = pn;
	return pn;
#endif
}

void Map::del(Map::node* p) {
#ifdef REALTIME_NEW
	node* pd = p->pnext;
	p->pnext = pd->pnext;
	delete pd;
	WCHAR c[16];
	if (nstate) {
		_itow_s(node_count, c, 10);
		theDlg.SetDlgItemText(IDC_NODEPOOL_USAGE, c);
		nstate = 0;
	}
	node_count--;
#else
	Map::node* pd = p->pnext;
	p->pnext = pd->pnext;
	pd->pnext = node_pool->pnext;
	node_pool->pnext = pd;
	pd->count = pd->state = pd->y = 0;
#endif
}

void Map::del(Map::head* h) {
#ifdef REALTIME_NEW
	head* pd = h->pnext;
	delete pd->pnode;
	h->pnext = pd->pnext;
	delete pd;
	WCHAR c[16];
	if (hstate) {
		_itow_s(head_count, c, 10);
		theDlg.SetDlgItemText(IDC_HEADPOOL_USAGE, c);
		hstate = 0;
	}
	head_count--;
	if (nstate) {
		_itow_s(node_count, c, 10);
		theDlg.SetDlgItemText(IDC_NODEPOOL_USAGE, c);
		nstate = 0;
	}
	node_count--;
#else 
	Map::head* pd = h->pnext;

	Map::node* pdn = pd->pnode;
	pd->pnode = nullptr;
	pdn->pnext = node_pool->pnext;
	node_pool->pnext = pdn;
	pdn->count = pdn->state = pdn->y = 0;

	h->pnext = pd->pnext;
	pd->pnext = head_pool->pnext;
	head_pool->pnext = pd;
	pd->x = 0;
#endif
}

Map::head* Map::add(int xpos, int ypos, head* pacceh) {
#ifdef REALTIME_NEW
#endif
	Map::head* px = &nxt;
	if (pacceh &&  pacceh->x <= xpos) px = pacceh;
	while (px->pnext && px->pnext->x <= xpos) px = px->pnext;
	if (px->x == xpos && px->pnode) {
		Map::node* py = px->pnode;
		while (py->pnext && py->pnext->y <= ypos) py = py->pnext;
		if (py->y == ypos) py->count++;				//If the Map::node already exists: add 1 to count
		else {										//If the Map::node doesn't exist: insert a Map::node
			Map::node* pn = insert(py);
			pn->y = ypos;
			pn->count = 1;
		}
	}
	else {											//If the row doesn't exist: insert Map::head and Map::node
		Map::head* pn = insert(px);
		Map::node* pnode = insert(pn->pnode);

		pn->x = xpos;
		pnode->y = ypos;
		pnode->count = 1;
	}
	return px;
}

void Map::mark(int xpos, int ypos) {
	Map::head* px = &nxt;
	while (px->pnext && px->pnext->x <= xpos) px = px->pnext;
	Map::node* py = px->pnode;
	while (py->pnext && py->pnext->y <= ypos) py = py->pnext;
	py->state = true;
}

void Map::clear(Map::head* h) {
	while (h->pnext) {
		Map::node* hn = h->pnext->pnode;
		while (hn->pnext)
			del(hn);
		del(h);
	}
}

void Map::init_builtins() {
	//{1,1,1},
	//{1,0,0},
	//{0,1,0}

	const static POINT builtin0[] = {
		{0, 0}, {0, 1}, {1, 0}, {1, 2}, {2, 0},
	};
	builtins[0].points = builtin0;
	builtins[0].size = 5;
	builtins[0].length = 3;
	builtins[0].height = 3;

	//{0,1,0,0,1},
	//{1,0,0,0,0},
	//{1,0,0,0,1},
	//{1,1,1,1,0}	

	const static POINT builtin1[] = {
		{0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 3}, {2, 3}, {3, 3}, {4, 0}, {4, 2},
	};
	builtins[1].points = builtin1;
	builtins[1].size = sizeof(builtin1) / sizeof(POINT);
	builtins[1].length = 5;
	builtins[1].height = 4;

	//
	//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
	//{0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
	//{1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	//{1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
	//

	const static POINT builtin2[] = {
		{0, 4}, {0, 5}, {1, 4}, {1, 5},
		{10, 4}, {10, 5}, {10, 6}, {11, 3}, {11, 7}, {12, 2}, {12, 8}, {13, 2}, {13, 8},
		{14, 5}, {15, 3}, {15, 7}, {16, 4}, {16, 5}, {16, 6}, {17, 5},
		{20, 2}, {20, 3}, {20, 4}, {21, 2}, {21, 3}, {21, 4}, {22, 1}, {22, 5},
		{24, 0}, {24, 1}, {24, 5}, {24, 6},
		{34, 2}, {34, 3}, {35, 2}, {35, 3},
	};
	builtins[2].points = builtin2;
	builtins[2].size = sizeof(builtin2) / sizeof(POINT);
	builtins[2].length = 36;
	builtins[2].height = 9;

	//
	//{0,0,0,0,0,0,1,0,0,0,0,0,0},
	//{0,0,0,0,0,1,0,1,0,0,0,0,0},
	//{0,0,0,0,0,1,0,1,0,0,0,0,0},
	//{0,0,0,0,0,0,1,0,0,0,0,0,0},
	//{0,0,0,0,0,0,0,0,0,0,0,0,0},
	//{0,1,1,0,0,0,0,0,0,0,1,1,0},
	//{1,0,0,1,0,0,0,0,0,1,0,0,1},
	//{0,1,1,0,0,0,0,0,0,0,1,1,0},
	//{0,0,0,0,0,0,0,0,0,0,0,0,0},
	//{0,0,0,0,0,0,1,0,0,0,0,0,0},
	//{0,0,0,0,0,1,0,1,0,0,0,0,0},
	//{0,0,0,0,0,1,0,1,0,0,0,0,0},
	//{0,0,0,0,0,0,1,0,0,0,0,0,0}
	//

	const static POINT builtin3[] = {
		{0, 6}, {1, 5}, {1, 7}, {2, 5}, {2, 7}, {3, 6},
		{5, 1}, {5, 2}, {6, 0}, {6, 3}, {7, 1}, {7, 2},
		{5, 10}, {5, 11}, {6, 9}, {6, 12}, {7, 10}, {7, 11},
		{9, 6}, {10, 5}, {10, 7}, {11, 5}, {11, 7}, {12, 6},
	};
	builtins[3].points = builtin3;
	builtins[3].size = sizeof(builtin3) / sizeof(POINT);
	builtins[3].length = 13;
	builtins[3].height = 13;

	//{1, 1, 0, 0}
	//{1, 1, 0, 0}
	//{0, 0, 1, 1}
	//{0, 0, 1, 1}

	const static POINT builtin4[] = {
		{0, 0}, {0, 1}, {1, 0}, {1, 1},
		{2, 2}, {2, 3}, {3, 2}, {3, 3},
	};
	builtins[4].points = builtin4;
	builtins[4].size = sizeof(builtin4) / sizeof(POINT);
	builtins[4].length = 4;
	builtins[4].height = 4;

	//
	// {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0}
	// {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	// {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1}
	// {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1}
	// {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1}
	// {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0}
	// {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	// {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0}
	// {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1}
	// {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1}
	// {1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1}
	// {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	// {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0}
	//

	const static POINT builtin5[] = {
		{0, 2}, {0, 3}, {0, 4}, {2, 0}, {3, 0}, {4, 0}, {5, 2}, {5, 3}, {5, 4}, {2, 5}, {3, 5}, {4, 5},
		{0, 8}, {0, 9}, {0, 10}, {2, 7}, {3, 7}, {4, 7}, {5, 8}, {5, 9}, {5, 10}, {2, 12}, {3, 12}, {4, 12},
		{7, 2}, {7, 3}, {7, 4}, {8, 0}, {9, 0}, {10, 0}, {12, 2}, {12, 3}, {12, 4}, {8, 5}, {9, 5}, {10, 5},
		{7, 8}, {7, 9}, {7, 10}, {8, 7}, {9, 7}, {10, 7}, {12, 8}, {12, 9}, {12, 10}, {8, 12}, {9, 12}, {10, 12},
	};
	builtins[5].points = builtin5;
	builtins[5].size = sizeof(builtin5) / sizeof(POINT);
	builtins[5].length = 13;
	builtins[5].height = 13;
}

Map::~Map() {
	DWORD threadcode;
	::GetExitCodeThread(pCalcThread->m_hThread, &threadcode);
	if(threadcode == STILL_ACTIVE)
		pCalcThread->PostThreadMessageW(UM_CLOSETHREAD, 0, (LPARAM)0);
#ifdef REALTIME_NEW
	clear(&pre);
	clear(&cur);
	delete phead_pools.phead;
	delete pnode_pools.pnode;
#else
	ppool* phead = phead_pools.pnext, * pdel = phead;
	while (phead != nullptr) {
		delete[] phead->phead;
		phead = phead->pnext;
		delete pdel;
		pdel = phead;
	}
	ppool* pnode = pnode_pools.pnext; pdel = pnode;
	while (pnode != nullptr) {
		delete[] pnode->pnode;
		pnode = pnode->pnext;
		delete pdel;
		pdel = pnode;
	}
	delete[] phead_pools.phead;
	delete[] pnode_pools.pnode;
#endif
}

Map map;
AD ad;
MOUSEINFO mi;
CALCINFO ci;

