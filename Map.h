#pragma once

extern int side_length;
extern unsigned int xpivot, ypivot;
extern bool needErase, started;
extern unsigned int selected_builtin, selected_direction, kbd_input_state, TIMER;
extern const unsigned int move_length;
extern char ids_help_about[256], ids_help_help[1024];
inline void redraw_erase();
inline void change_xpivot();
inline void change_ypivot();

class Map {
public:
	Map();
	~Map();
	void change(unsigned int xpos, unsigned int ypos, int type = 0);
	void calc();
	void clear();
	void add_builtin(const unsigned int& xpos, const unsigned int& ypos, const unsigned int& b = selected_builtin, const unsigned int& d = selected_direction);
	void add_delete_region(RECT&, bool, bool);
	void draw(HDC hdc, RECT rect, bool erase = needErase, HBRUSH hBlackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH), HBRUSH hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH));
	RECT get_builtin_info(int b = selected_builtin);
	void draw_builtin(CDialog* Dlg, const unsigned int& b = selected_builtin, const unsigned int& d = selected_direction);
	LPCTSTR get_size();
	void load(const char*);
	void dump(const char*);
private:
	struct node {
		unsigned int y;
		bool state;
		BYTE count;
		node* pnext;
#ifdef DEBUG
		unsigned int id;
#endif // DEBUG

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

	head* enlarge_head_pool();
	node* enlarge_node_pool();
	inline void refresh_headpool_usage();
	inline void refresh_nodepool_usage();
	head* insert(head*);
	node*insert(node*);
	void del(node*);
	void del(head*);
	void add(unsigned int, unsigned int);
	void mark(unsigned int, unsigned int);
	void clear(head*);
	void init_builtins();
	head pre;	//Previous map
	head cur;	//Current map
	head nxt;	//Next map
	head* ppre;
	head* head_pool;
	node* node_pool;
	int headpool_usage = 1, nodepool_usage = 1;
	builtin builtins[10];
	const int SIZE = 50;
	ppool phead_pools, pnode_pools;
};


struct AD {
	int count = 0;
	bool adstate = false;
	bool act = false;
	bool isrand = false;
	POINT p1 = { 0, 0 };
	POINT p2 = { 0, 0 };
};

extern Map map;
extern AD ad;