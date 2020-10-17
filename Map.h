#pragma once

//#define REALTIME_NEW

extern int side_length;
extern int xpivot, ypivot;
extern bool started;
extern unsigned int selected_builtin, selected_direction, kbd_input_state, TIMER;
extern const unsigned int move_length;
extern char ids_help_about[256], ids_help_help[1024];
inline void redraw();
inline void change_xpivot();
inline void change_ypivot();
UINT __cdecl theMapProcessingThread(LPVOID lpParameter);

enum jobtype {
	t_calc,
	t_exit
};

struct MapProcessingData {
	CWnd* cv;
	jobtype type;
	CEvent threadEvent;
	bool busy;
};

class Map {
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

public:
	MapProcessingData data = {};

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
	head* change(int, int, int = 0, head* = nullptr);
	void calc();
	void clear();
	void add_builtin(const int&, const int&, const unsigned int& = selected_builtin, const unsigned int& = selected_direction);
	void add_delete_region(RECT&, bool, bool);
	void draw(CDC&, RECT&);
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
};

struct MOUSEINFO {
	BYTE state = 0; // { 0: Click, 1: Pen, 2: Eraser, 3: Drag }
	CPoint pprev = { 0, 0 };
};

extern Map map;
extern AD ad;
extern MOUSEINFO mi;