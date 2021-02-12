#pragma once

//#define REALTIME_NEW

class Selector;
class ClipBoard;

extern int side_length;
extern int xpivot, ypivot;
extern bool started, headpool_usage_need_refresh, nodepool_usage_need_refresh,
			xpivot_need_refresh, ypivot_need_refresh,
			generation_need_refresh, population_need_refresh;
extern unsigned int selected_builtin, selected_direction, kbd_input_state, TIMER;
extern const unsigned int move_length;
void redraw();
void change_xpivot();
void change_ypivot();

class Map {
	friend class Selector;
	friend class ClipBoard;
private:
	struct node {
		int y;
		bool state;
		BYTE count;
		node* pnext;
	};

	struct head {
		int x;
		node* pnode;
		head* pnext;
	};

	struct builtin {
		const POINT* points;
		BYTE size;
		BYTE length;
		BYTE height;
	};

	struct ppool {
		head* phead;
		node* pnode;
		ppool* pnext;
	};

private:
	head cur;	//Current map
	head nxt;	//Next map
	head* head_pool;
	node* node_pool;
	int headpool_usage = 1, nodepool_usage = 1;
	int generation = 0;
	int population = 0;
	builtin builtins[10];
	const int SIZE = 50;
	ppool phead_pools, pnode_pools;

public:
	Map();
	~Map();
	head* change(int, int, int = 0, head* = nullptr);
	void calc();
	void clear();
	void reset();
	void add_builtin(const int&, const int&, const unsigned int& = selected_builtin, const unsigned int& = selected_direction);
	void add_delete_region(RECT& rect, bool isadd, bool isrand);
	void draw(CDC&, RECT&);
	//void draw(Graphics&, RECT&, Gdiplus::SolidBrush&);
	void draw(ID2D1HwndRenderTarget* ptheRenderTarget, int width, int height, float lnwidth);
	RECT get_builtin_info(int b = selected_builtin);
	void draw_builtin(CDialog*, const unsigned int& = selected_builtin, const unsigned int& = selected_direction);
	LPCTSTR get_size();
	//void load(const char*);
	void load(CString&);
	//void dump(const char*);
	void dump(CString&);
	void free_extra();
	inline void refresh_headpool_usage();
	inline void refresh_nodepool_usage();
	void refresh_generation();
	void refresh_population();


private:
	head* enlarge_head_pool();
	node* enlarge_node_pool();
	head* insert(head*);
	node* insert(node*);
	void del(node*);
	void del(head*);
	head* add(int, int, head* = nullptr);
	void mark(int, int);
	void clear(head*);
	void init_builtins();
};


struct AD {
	int count = 0;
	bool adstate = false;
	bool act = false;
	bool isrand = false;
	POINT p1 = { 0, 0 };
	POINT p2 = { 0, 0 };
};

struct MOUSEINFO {
	BYTE state = 0; // { 0: Click, 1: Pen, 2: Eraser, 3: Drag, 4:Select }
	CPoint pprev = { 0, 0 };
};

extern Map map;
extern AD ad;
extern MOUSEINFO mi;
