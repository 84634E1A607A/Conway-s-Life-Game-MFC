#pragma once

//#define REALTIME_NEW

extern int side_length;
extern unsigned int xpivot, ypivot;
extern bool started;
extern unsigned int selected_builtin, selected_direction, kbd_input_state, TIMER;
extern const unsigned int move_length;
extern char ids_help_about[256], ids_help_help[1024];
inline void redraw();
inline void change_xpivot();
inline void change_ypivot();

class Map {
private:
	struct node {
		unsigned int y;
		bool state;
		BYTE count;
		node* pnext;
	};

	struct head {
		unsigned int x;
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
	builtin builtins[10];
	const int SIZE = 50;
	ppool phead_pools, pnode_pools;

public:
	Map();
	~Map();
	head* change(unsigned int, unsigned int, int = 0, head* = nullptr);
	void calc();
	void clear();
	void add_builtin(const unsigned int&, const unsigned int&, const unsigned int& = selected_builtin, const unsigned int& = selected_direction);
	void add_delete_region(RECT&, bool, bool);
	void draw(CDC& hdc, RECT& rect);
	RECT get_builtin_info(int b = selected_builtin);
	void draw_builtin(CDialog*, const unsigned int& = selected_builtin, const unsigned int& = selected_direction);
	LPCTSTR get_size();
	//void load(const char*);
	void load(CString&);
	//void dump(const char*);
	void dump(CString&);
	void free_extra();
	//void trial_auto_release();

private:
	head* enlarge_head_pool();
	node* enlarge_node_pool();
	inline void refresh_headpool_usage();
	inline void refresh_nodepool_usage();
	head* insert(head*);
	node*insert(node*);
	void del(node*);
	void del(head*);
	head* add(unsigned int, unsigned int, head* = nullptr);
	void mark(unsigned int, unsigned int);
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
	BYTE state; // { 0: Click, 1: Pen, 2: Eraser, 3: Drag }
	CPoint pprev = { 0, 0 };
};

extern Map map;
extern AD ad;
extern MOUSEINFO mi;