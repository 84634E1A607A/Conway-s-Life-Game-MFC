#include "pch.h"
#include "Map.h"
#include "Life-MFC.h"
#include "MainFrm.h"
#include "ChildView.h"
#include "DlgOptions.h"

int side_length = 10;
unsigned int xpivot = 0x08000000, ypivot = 0x08000000;
bool needErase = true, started;
unsigned int selected_builtin, selected_direction, kbd_input_state, TIMER = 500;
const unsigned int move_length = 30;
char ids_help_about[256], ids_help_help[1024];
extern DlgOptions theDlg;


#ifdef DEBUG
unsigned int insert_count, del_count, id;
#endif

inline void redraw_erase() {
	RECT rect;
	theApp.m_pMainWnd->GetClientRect(&rect);
	theApp.m_pMainWnd->RedrawWindow(&rect, 0, RDW_ERASE | RDW_INVALIDATE);
}

inline void change_xpivot() {
	WCHAR xpivot_c[16];
	wsprintf(xpivot_c, L"%08x", xpivot);
	theDlg.GetDlgItem(IDC_XPIVOT)->SendMessage(WM_SETTEXT, 0, (LPARAM)xpivot_c);
}

inline void change_ypivot() {
	WCHAR ypivot_c[16];
	wsprintf(ypivot_c, L"%08x", ypivot);
	theDlg.GetDlgItem(IDC_YPIVOT)->SendMessage(WM_SETTEXT, 0, (LPARAM)ypivot_c);
}

Map::Map() {
#ifdef DEBUG
	head_pool = new head, node_pool = new node;
	cur.pnext = pre.pnext = nxt.pnext = nullptr;
#else // DEBUG
	head_pool = new Map::head[SIZE];
	node_pool = new Map::node[SIZE];
	memset(head_pool, 0, SIZE * sizeof(Map::head));
	memset(node_pool, 0, SIZE * sizeof(Map::node));
	for (int i = 0; i < SIZE; i++) (head_pool + i)->pnext = head_pool + i + 1;
	for (int i = 0; i < SIZE; i++) (node_pool + i)->pnext = node_pool + i + 1;
	(head_pool + SIZE - 1)->pnext = nullptr;  (node_pool + SIZE - 1)->pnext = nullptr;
	cur.pnext = nullptr;
#endif
	phead_pools = { head_pool, nullptr, nullptr };
	pnode_pools = { nullptr, node_pool, nullptr };
	init_builtins();
}

//type: {0: 1->0, 0->1; 1: 0,1->1; 2: 0,1->0}
void Map::change(unsigned int xpos, unsigned int ypos, int type) {
	Map::head* px = &cur;
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
}

void Map::calc() {
	Map::head* px = cur.pnext;
	while (px) {
		Map::node* py = px->pnode->pnext;
		for (; py; py = py->pnext) {
			if (!py->state) continue;
			int x = px->x;
			int y = py->y;
			for (int i = x - 1; i <= x + 1; i++)
				for (int j = y - 1; j <= y + 1; j++) add(i, j);
			mark(x, y);
		}
		px = px->pnext;
	}
	clear(&pre);
	pre.pnext = cur.pnext;
	cur.pnext = nullptr;
	px = nxt.pnext;
	while (px) {
		Map::node* py = px->pnode->pnext;
		for (; py; py = py->pnext) if ((py->count == 3) || py->state && py->count == 4) change(px->x, py->y);
		px = px->pnext;
	}
	clear(&nxt);
}

void Map::clear() {
	Sleep(TIMER);
#ifdef DEBUG
	clear(&pre);
	clear(&cur);
	cur.pnext = nullptr, nxt.pnext = nullptr, pre.pnext = nullptr, ppre = nullptr;
	redraw_erase();
#else
	memset(head_pool, 0, SIZE * sizeof(Map::head));
	memset(node_pool, 0, SIZE * sizeof(Map::node));
	for (int i = 0; i < SIZE; i++) (head_pool + i)->pnext = head_pool + i + 1;
	for (int i = 0; i < SIZE; i++) (node_pool + i)->pnext = node_pool + i + 1;
	(head_pool + SIZE - 1)->pnext = nullptr;  (node_pool + SIZE - 1)->pnext = nullptr;
	cur.pnext = nullptr, nxt.pnext = nullptr, pre.pnext = nullptr, ppre = nullptr;
	
	redraw_erase();

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
#endif

	headpool_usage = nodepool_usage = 1;
	refresh_headpool_usage(), refresh_nodepool_usage();
	change_xpivot(), change_ypivot();
}

void Map::add_builtin(const unsigned int& xpos, const unsigned int& ypos, const unsigned int& b, const unsigned int& d) {
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
	for (int x = rect.left; x <= rect.right; x++) {
		for (int y = rect.top; y <= rect.bottom; y++) {
			if (isadd) change(x, y, (isrand ? (rand() % 2 ? 1 : 2) : 1));
			else change(x, y, 2);
		}
	}
}

void Map::draw(HDC hdc, RECT rect, bool erase, HBRUSH hBlackBrush, HBRUSH hWhiteBrush) {
	needErase = false;
	unsigned int left = rect.left / side_length + xpivot, right = rect.right / side_length + xpivot;
	unsigned int top = rect.top / side_length + ypivot, bottom = rect.bottom / side_length + ypivot;
	Map::head* pl = &cur, * ppl = &pre;
	while (pl->pnext && pl->pnext->x < left) pl = pl->pnext;
	while (ppl->pnext && ppl->pnext->x < left) ppl = ppl->pnext;
	Map::head* px = pl, * ppx = ppl;
	while (px->pnext && px->pnext->x <= right) {
		if (!erase) while (ppx->pnext && ppx->pnext->x < px->pnext->x) ppx = ppx->pnext;
		Map::node* py = px->pnext->pnode, * ppy = (ppx->pnext && ppx->pnext->x == px->pnext->x) ? ppx->pnext->pnode : nullptr;
		while (py->pnext && py->pnext->y < top) py = py->pnext;
		if (!erase && ppy) while (ppy->pnext && ppy->pnext->y < top) ppy = ppy->pnext;
		while (py->pnext && py->pnext->y <= bottom) {
			if (ppy && !erase) {
				while (ppy->pnext && ppy->pnext->y < py->pnext->y) ppy = ppy->pnext;
				if (ppy->pnext && ppy->pnext->y == py->pnext->y) { py = py->pnext; continue; }
			}
			RECT fill_rect = {
				(LONG)((px->pnext->x - xpivot) * side_length + 1), //left
				(LONG)((py->pnext->y - ypivot) * side_length + 1), //top
				(LONG)((px->pnext->x - xpivot + 1) * side_length), //right
				(LONG)((py->pnext->y - ypivot + 1) * side_length)  //bottom
			};
			FillRect(hdc, &fill_rect, hBlackBrush);
			py = py->pnext;
		}
		px = px->pnext;
	}
	if (erase) return;
	px = pl, ppx = ppl;
	while (ppx->pnext && ppx->pnext->x <= right) {
		while (px->pnext && px->pnext->x < ppx->pnext->x) px = px->pnext;
		Map::node* ppy = ppx->pnext->pnode, * py = (px->pnext && px->pnext->x == ppx->pnext->x) ? px->pnext->pnode : nullptr;
		while (ppy->pnext && ppy->pnext->y < top) ppy = ppy->pnext;
		if (py) while (py->pnext && py->pnext->y < top) py = py->pnext;
		while (ppy->pnext && ppy->pnext->y <= bottom) {
			if (py) {
				while (py->pnext && py->pnext->y < ppy->pnext->y) py = py->pnext;
				if (py->pnext && py->pnext->y == ppy->pnext->y) { ppy = ppy->pnext; continue; }
			}
			RECT fill_rect = {
				(LONG)((ppx->pnext->x - xpivot) * side_length + 1), //left
				(LONG)((ppy->pnext->y - ypivot) * side_length + 1), //top
				(LONG)((ppx->pnext->x - xpivot + 1) * side_length), //right
				(LONG)((ppy->pnext->y - ypivot + 1) * side_length)  //bottom
			};
			FillRect(hdc, &fill_rect, hWhiteBrush);
			ppy = ppy->pnext;
		}
		ppx = ppx->pnext;
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
	const static HBRUSH black_brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	const static HBRUSH white_brush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	const static HBRUSH red_brush = CreateSolidBrush(RGB(255, 0, 0));
	const builtin& bi = builtins[b];
	HDC hdc = hPreview->GetDC()->m_hDC;

	//Clear
	RECT erase_rect = { 1, 1, xl, yl };
	FillRect(hdc, &erase_rect, white_brush);
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
		FillRect(hdc, &rect, black_brush);
	}
	RECT click_rect = { xs, ys, xs + r, ys + r };
	FillRect(hdc, &click_rect, red_brush);
}

LPCTSTR Map::get_size() {
	static WCHAR size[5];
	_itow_s(SIZE, size, 10);
	return size;
}

void Map::load(const char* fname) {
	try {
		FILE* fin;
		fopen_s(&fin, fname, "rb");
		if (!fin) throw L"Cannot open file";
		clear();
		fread_s(&xpivot, sizeof(xpivot), sizeof(xpivot), 1, fin);
		fread_s(&ypivot, sizeof(ypivot), sizeof(ypivot), 1, fin);
		unsigned int tmp;
		fread_s(&tmp, sizeof(tmp), sizeof(tmp), 1, fin);
		if (tmp != 0xffffffff) {
			clear();
			fclose(fin);
			throw L"Bad file! Map reset";
		}
		while (true) {
			unsigned int x, y;
			fread_s(&x, sizeof(x), sizeof(x), 1, fin);
			fread_s(&y, sizeof(y), sizeof(y), 1, fin);
			if (x == 0xffffffff && y == 0xfffffffd) break;
			if (ferror(fin)) {
				clear();
				fclose(fin);
				throw L"Bad file! Map reset";
			}
			change(x, y);
		}
		fclose(fin);
		redraw_erase();
	}
	catch (const LPCTSTR msg) {
		theApp.m_pMainWnd->MessageBox(msg, L"Error", MB_OK);
	}
}

void Map::dump(const char* fname) {
	started = false;
	FILE* fout;
	fopen_s(&fout, fname, "wb");
	if (!fout) {
		theApp.m_pMainWnd->MessageBox(L"Cannot Open File", L"Error", MB_OK);
		return;
	}
	unsigned int tmp[2] = { 0xffffffff, 0xfffffffd };
	fwrite(&xpivot, sizeof(xpivot), 1, fout);
	fwrite(&ypivot, sizeof(ypivot), 1, fout);
	fwrite(tmp, sizeof(tmp[0]), 1, fout);
	Map::head* ph = &cur;
	while (ph->pnext) {
		ph = ph->pnext;
		unsigned int x = ph->x;
		Map::node* pn = ph->pnode;
		while (pn->pnext) {
			pn = pn->pnext;
			if (!pn->state) continue;
			fwrite(&x, sizeof(x), 1, fout);
			fwrite(&(pn->y), sizeof(pn->y), 1, fout);
		}
	}
	fwrite(tmp, sizeof(tmp), 1, fout);
	fclose(fout);
}

Map::head* Map::enlarge_head_pool() {
	Map::head* nhead_pool = new Map::head[SIZE];
	memset(nhead_pool, 0, SIZE * sizeof(Map::head));
	for (int i = 0; i < SIZE; i++) (nhead_pool + i)->pnext = nhead_pool + i + 1;
	(nhead_pool + SIZE - 1)->pnext = nullptr;
	head_pool->pnext = nhead_pool;

	ppool* nphead_pool = new ppool;
	nphead_pool->phead = nhead_pool;
	nphead_pool->pnext = phead_pools.pnext;
	phead_pools.pnext = nphead_pool;

	headpool_usage++;
	refresh_headpool_usage();

	return nhead_pool;
}

Map::node* Map::enlarge_node_pool() {
	Map::node* nnode_pool = new Map::node[SIZE];
	memset(nnode_pool, 0, SIZE * sizeof(Map::node));
	for (int i = 0; i < SIZE; i++) (nnode_pool + i)->pnext = nnode_pool + i + 1;
	(nnode_pool + SIZE - 1)->pnext = nullptr;
	node_pool->pnext = nnode_pool;

	ppool* npnode_pool = new ppool;
	npnode_pool->pnode = nnode_pool;
	npnode_pool->pnext = pnode_pools.pnext;
	pnode_pools.pnext = npnode_pool;

	nodepool_usage++;
	refresh_nodepool_usage();

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
#ifdef DEBUG
	insert_count++;
	head* pn = new head;
	pn->pnext = p->pnext;
	p->pnext = pn;
	node* pnode = new node;
	pn->pnode = pnode;
	memset(pnode, 0, sizeof(node));
	pnode->id = id++;
	return pn;
#else // DEBUG
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

Map::node* Map::insert(Map::node* p) {
#ifdef DEBUG
	insert_count++;
	node* pn = new node;
	memset(pn, 0, sizeof(node));
	pn->pnext = p->pnext;
	p->pnext = pn;
	pn->id = id++;
	return pn;
#else // DEBUG
	Map::node* pn = node_pool->pnext ? node_pool->pnext : enlarge_node_pool();
	node_pool->pnext = pn->pnext;
	pn->pnext = p->pnext;
	p->pnext = pn;
	return pn;
#endif
}

void Map::del(Map::node* p) {
#ifdef DEBUG
	del_count++;
	node* pd = p->pnext;
	p->pnext = pd->pnext;
	delete pd;
#else // DEBUG
	Map::node* pd = p->pnext;
	p->pnext = pd->pnext;
	pd->pnext = node_pool->pnext;
	node_pool->pnext = pd;
	pd->count = pd->state = pd->y = 0;
#endif
}

void Map::del(Map::head* h) {
#ifdef DEBUG
	del_count++;
	head* pd = h->pnext;
	delete pd->pnode;
	h->pnext = pd->pnext;
	delete pd;
#else // DEBUG
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

void Map::add(unsigned int xpos, unsigned int ypos) {
#ifdef DEBUG
	ppre = nullptr;
#endif
	if (!ppre) ppre = &nxt;
	Map::head* px = &nxt;
	if (ppre->pnext && ppre->pnext->x <= xpos) px = ppre;
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
	ppre = px;
}

void Map::mark(unsigned int xpos, unsigned int ypos) {
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
		{0, 0}, {0, 1}, {1, 0}, {1, 2}, {2, 0}
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
		{0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 3}, {2, 3}, {3, 3}, {4, 0}, {4, 2}
	};
	builtins[1].points = builtin1;
	builtins[1].size = 9;
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
		{34, 2}, {34, 3}, {35, 2}, {35, 3}
	};
	builtins[2].points = builtin2;
	builtins[2].size = 36;
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
		{9, 6}, {10, 5}, {10, 7}, {11, 5}, {11, 7}, {12, 6}
	};
	builtins[3].points = builtin3;
	builtins[3].size = 24;
	builtins[3].length = 13;
	builtins[3].height = 13;
}

Map::~Map() {
#ifdef DEBUG
	clear(&pre);
	clear(&cur);
	delete phead_pools.phead;
	delete pnode_pools.pnode;
#else // DEBUG
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