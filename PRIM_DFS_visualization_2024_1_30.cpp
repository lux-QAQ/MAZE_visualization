#include <graphics.h>
#include <Windows.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <time.h>

using namespace std;

//#define WIDTH 1080//分辨率(分辨率大了迷宫会很大，生成和探索速度会很慢)
//#define HEIGHT 720
#define WIDTH 600//分辨率
#define HEIGHT 420
#define BLOCK_XY 10//小方格边长
const int BLOCK_WIDTH = WIDTH / (BLOCK_XY);//小方格x轴数量
const int BLOCK_HEIGHT = HEIGHT / (BLOCK_XY);//小方格y轴数量

#define road COLORREF RGB(255,255,255)//空气颜色
#define wall COLORREF RGB(50,50,50)//墙壁颜色
#define wait COLORREF RGB(0,255,0)//待选颜色
#define current_point COLORREF RGB(255,0,0)//选中颜色
#define explored_point COLORREF RGB(255,165,0)//已探索颜色
#define target COLORREF RGB(0,0,255)//终点颜色
#define start_pioint COLORREF RGB(0,205,102)//起点颜色
#define path COLORREF RGB(255,0,0)//路径颜色

#define ROAD 0//路（空气）
#define WALL 1//墙壁
#define WAIT 2//待选路点
#define EXPLORED_POINT 3//已探索地方
#define TARGET 4//终点
#define START_POINT 5//起点	


struct block {
public:
	int x;
	int y;
	block(int x, int y) {
		this->x = x;
		this->y = y;
	}
};

int map[BLOCK_WIDTH][BLOCK_HEIGHT];//0代表空气，1代表墙壁，2代表待选路点，3代表选中路点（但是并没有用到）
vector<block*> openlist;//待选列表
vector<block*> list;
vector<block*> pathlist; // 路径列表

/*函数声明*/
void clearscreen(COLORREF RGB);//清空地图为RGB颜色(带有网格)
void gamedrawpixal(int x, int y, COLORREF RGB);//在x，y位置画颜色为RGB的方格
void init();//初始化
void join(int x, int y);//将x,y四周是墙壁的路点加入待选列表
void display();//刷新屏幕
void prim_main();//prim主循环
void search(int x, int y);//DFS探索
void through(int x, int y);//将x,y与它四周一个随机已经变成路的路点打通

void display2();//显示DFS探索
void target_maker();//设置终点
/*********/
int main() {
	srand((unsigned)time(NULL));//随机数种子
	initgraph(WIDTH, HEIGHT);//创建绘图窗口
	BeginBatchDraw();//开始批量绘图
	cleardevice();//清空显示

	init();

	display();//显示迷宫生成结果
	FlushBatchDraw();//将缓冲输出到屏幕
	//以下是探索迷宫
	target_maker();//设置终点
	search(0, 0);//这个里面的参数代表DFS探索的起点
	
	display2();//显示DFS探索
	FlushBatchDraw();
	while (1);
}
void search(int x, int y) {
	if (map[x][y] == TARGET) {
		printf("Found the target:(%d,%d)!\n",x, y);
		system("pause");
		return; // 找到目标停止搜索
	}

	map[x][y] = EXPLORED_POINT; // 标记当前探索的点为已探索
	display2();
	Sleep(1); // 延迟是为了可视化

	// 探索相邻的点
	if (x - 1 >= 0 && (map[x - 1][y] == ROAD||map[x - 1][y] == TARGET)) 
	{
		search(x - 1, y); // 左
	}
	if (x + 1 < BLOCK_WIDTH && (map[x + 1][y] == ROAD||map[x + 1][y] == TARGET)) 
	{
		search(x + 1, y); // 右
	}
	if (y - 1 >= 0 && (map[x][y - 1] == ROAD||map[x][y - 1] == TARGET)) 
	{
		search(x, y - 1); // 上
	}
	if (y + 1 < BLOCK_HEIGHT && (map[x][y + 1] == ROAD||map[x][y + 1] == TARGET)) 
	{
		search(x, y + 1); // 下
	}

	if (map[x][y] != TARGET) 
	{
		map[x][y] = ROAD; // 标记当前点为路，这样在相邻的点探索完后就能体现出回溯的效果
	}
	display2();
	Sleep(2); // 延迟是为了可视化
}
void init() {
	openlist.clear();
	pathlist.clear(); // 清空路径列表
	/*地图初始化为墙*/
	for (int y = 0; y < BLOCK_HEIGHT; y++) {
		for (int x = 0; x < BLOCK_WIDTH; x++) {
			map[x][y] = WALL;
		}
	}
	/*随机选一个路点，将它变成路*/
	map[40][40] = ROAD;
	/*将路点四周不是路的路点加入待选列表*/
	join(40, 40);
	prim_main();
}
void prim_main() {
	while (openlist.size() > 0) {
		/*从待选路点随机选一个路点A*/
		int AIndexea = rand() % openlist.size();
		block* A = openlist[AIndexea];
		/*将A与它四周一个随机已经变成路的路点打通*/
		through(A->x, A->y);
		/*将A变成路*/
		map[A->x][A->y] = ROAD;
		/*将A四周不是路的路点加入待选列表*///(注意，不要重复添加路点！)
		join(A->x, A->y);
		/*从待选路点中移除A*/
		openlist.erase(openlist.begin() + AIndexea);

		gamedrawpixal(A->x, A->y, current_point);
		FlushBatchDraw();
		display();
	}
	openlist.clear();
}
void through(int x, int y) {
	list.clear();
	//上
	if (y - 2 >= 0 && map[x][y - 2] == ROAD) {
		block* a = new block(x, y - 1);
		list.push_back(a);
	}
	//下
	if (y + 2 <= BLOCK_HEIGHT - 1 && map[x][y + 2] == ROAD) {
		block* b = new block(x, y + 1);
		list.push_back(b);
	}
	//左
	if (x - 2 >= 0 && map[x - 2][y] == ROAD) {
		block* c = new block(x - 1, y);
		list.push_back(c);
	}
	//右
	if (x + 2 <= BLOCK_WIDTH - 1 && map[x + 2][y] == ROAD) {
		block* d = new block(x + 1, y);
		list.push_back(d);
	}
	int BIndexea = rand() % list.size();
	block* B = list[BIndexea];
	/*将x，y与B打通*/
	map[B->x][B->y] = ROAD;
}
void join(int x, int y) {
	//上
	if (y - 2 >= 0 && map[x][y - 2] == WALL) {
		block* a = new block(x, y - 2);
		openlist.push_back(a);
		map[x][y - 2] = WAIT;
	}
	//下
	if (y + 2 <= BLOCK_HEIGHT - 1 && map[x][y + 2] == WALL) {
		block* b = new block(x, y + 2);
		openlist.push_back(b);
		map[x][y + 2] = WAIT;
	}
	//左
	if (x - 2 >= 0 && map[x - 2][y] == WALL) {
		block* c = new block(x - 2, y);
		openlist.push_back(c);
		map[x - 2][y] = WAIT;
	}
	//右
	if (x + 2 <= BLOCK_WIDTH - 1 && map[x + 2][y] == WALL) {
		block* d = new block(x + 2, y);
		openlist.push_back(d);
		map[x + 2][y] = WAIT;
	}
}
void display() {
	for (int y = 0; y < BLOCK_HEIGHT; y++) {
		for (int x = 0; x < BLOCK_WIDTH; x++) {
			if (map[x][y] == WALL) {
				gamedrawpixal(x, y, wall);
			}
			else if (map[x][y] == ROAD) {
				gamedrawpixal(x, y, road);
			}
			else if (map[x][y] == WAIT) {
				gamedrawpixal(x, y, wait);
			}
			else if (map[x][y] == START_POINT) {
				gamedrawpixal(x, y, start_pioint);
			}
			else if (map[x][y] == TARGET) {
				gamedrawpixal(x, y, target);
			}
			else if (map[x][y] == EXPLORED_POINT) {
				gamedrawpixal(x, y, explored_point);
			}
			else if (map[x][y] == 9) {
				gamedrawpixal(x, y, current_point);
			}
			else if (map[x][y] == 6) {
				gamedrawpixal(x, y, path);
			}
		}
	}
	FlushBatchDraw();
}
void display2() {
	for (int y = 0; y < BLOCK_HEIGHT; y++) {
		for (int x = 0; x < BLOCK_WIDTH; x++) {
			if (map[x][y] == WALL) {
				gamedrawpixal(x, y, wall);
			}
			else if (map[x][y] == ROAD) {
				gamedrawpixal(x, y, road);
			}
			else if (map[x][y] == EXPLORED_POINT) {
				gamedrawpixal(x, y, explored_point);
			}
			else if (map[x][y] == TARGET) {
				gamedrawpixal(x, y, target);
			}
			else if (map[x][y] == START_POINT) {
				gamedrawpixal(x, y, start_pioint);
			}
			else if (map[x][y] == 9) {
				gamedrawpixal(x, y, current_point);
			}
			else if (map[x][y] == 6) {
				gamedrawpixal(x, y, path);
			}
		}
	}
	FlushBatchDraw();
}
void clearscreen(COLORREF RGB) {
	for (int y = 0; y < BLOCK_HEIGHT; y++) {
		for (int x = 0; x < BLOCK_WIDTH; x++) {
			gamedrawpixal(x, y, RGB);
		}
	}
}
void gamedrawpixal(int x, int y, COLORREF RGB) {
	setfillcolor(RGB); setlinecolor(COLORREF RGB(0, 0, 0));
	fillrectangle(x * BLOCK_XY, y * BLOCK_XY, BLOCK_XY + x * BLOCK_XY, BLOCK_XY + y * BLOCK_XY);
}

void target_maker()
{
	int tmp = (BLOCK_WIDTH > BLOCK_HEIGHT ? (BLOCK_HEIGHT / 2) - 1 : (BLOCK_WIDTH / 2) - 1),a,b;
	
	
		do {
			a = rand() % BLOCK_WIDTH , b = rand() % BLOCK_HEIGHT ;//BLOCK_HEIGHT
		} while (((BLOCK_WIDTH / 2 - a) * (BLOCK_WIDTH / 2 - a) + (BLOCK_HEIGHT / 2 - b) * (BLOCK_HEIGHT / 2 - b) <= tmp * tmp) ||a-1<0||b-1<0|| map[a][b] != ROAD && (map[a + 1][b] != ROAD || map[a - 1][b] != ROAD || map[a][b + 1] != ROAD || map[a][b - 1] != ROAD) || map[a][b] == 4);
		map[a][b] = TARGET;
	
		
		
	
}
/*2024.1.30*/
/* 
.　　          ▃▆█▇▄▖
　　     　▟◤▖　　　◥█▎
　　　◢◤　   ▐　　　　▐▉
　▗◤　　　▂　▗▖　　▕█▎
　◤　▗▅▖◥▄　▀◣　　█▊
▐　▕▎◥▖◣◤　　　　◢██
█◣　◥▅█▀　　　　▐██◤
▐█▙▂　　　      ◢██◤
　◥██◣　　　　◢▄◤
　　　▀██▅▇▀ 
*/


