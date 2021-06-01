/*----------苏州大学独墅湖校区地图导览系统----------*/
/*姓名：杜冰                    完成日期：2020-06-29*/
#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <graphics.h>
using namespace std;
#define MaxViewNum 50	//最大建筑个数
#define Inf 32767	//定义无穷大
#define ViewNum 14	//目前建筑个数
#define LoadNum 35  //目前路径个数
static const int M = 1 << (ViewNum - 1);    //建筑集合所有子集的个数

/*----------以下定义邻接矩阵类型----------*/
typedef struct
{
    char name[30];          //建筑名称
    char number[10];        //建筑代号
    char introduce[200];    //建筑介绍
}Elemtype;                  //建筑信息
typedef struct
{
    int num;              //建筑编号
    Elemtype date;        //建筑信息
}View;                    //定义建筑
typedef struct
{
    View view[MaxViewNum];         //存放顶点的一维数组
    int length[MaxViewNum][MaxViewNum];  //存放路径长度
    int n, e;               //图中顶点数n和边数e
}MGraph;

/*----------以下定义邻接表类型-----------*/
typedef struct ANode
{
    int adjvex;					//该边的邻接点编号
    struct ANode* nextarc;		//指向下一条边的指针
    int weight;					//该边的相关信息，如权值（用整型表示）
} ArcNode;						//边结点类型
typedef struct Vnode
{
    Elemtype data;				//顶点其他信息
    int count;					//存放顶点入度,仅仅用于拓扑排序
    ArcNode* firstarc;			//指向第一条边
} VNode;						//邻接表头结点类型
typedef struct
{
    VNode adjlist[MaxViewNum];		//邻接表头结点数组
    int n, e;					//图中顶点数n和边数e
} AGraph;						//完整的图邻接表类型

/*----------全局变量----------*/
MGraph MGr;     //定义邻接矩阵
AGraph *AGr;     //定义邻接表
int shortest[MaxViewNum][MaxViewNum];   //定义全局变量存贮最小路径
int path[MaxViewNum][MaxViewNum];       //定义存储路径
int pathall[MaxViewNum];                //定义存储两点之间所有路径
int visited[MaxViewNum];                //定义访问标记
char filename[100];             //保存文件名称
int road[MaxViewNum][MaxViewNum];       //保存道路信息
int pathadvice[MaxViewNum];   //定义存储全部建筑建议浏览路径
int dp[ViewNum][M];     //保存动态规划公式d(i,V)的值
vector<int> apath;       //保存全部建筑建议浏览路径

/*----------函数声明----------*/
void Start();   //开始界面
void System(int n, int &fl);    //菜单系统
void Menu();            //输出菜单
void Init();            //保存建筑信息
void map();     //输出地图
void Output();          //输出建筑名称及编号
void Introduce();        //建筑介绍
void Floyd();       //Floyd算法
void Display(int i, int j);     //打印两个建筑的路径及最短路径
void ShortDistance();        //查找的两建筑的最短路径
void CreateAdj(AGraph*& G, int A[ViewNum][ViewNum], int n, int e);  //创建图的邻接表
void MatToList(MGraph g, AGraph*& G);   //将邻接矩阵g转换成邻接表G
void PathAll(AGraph* G, int u, int v, int path[], int d);  //求顶点u到顶点v的所有的简单路径
void AllDistance();  //查找两建筑之间的所有路径
void TSP(); //求出动态规划dp数组
bool IsVisited(bool visited[]); //判断结点是否都以访问,不包括0号结点
void GetPath(); //获取最优路径并保存在pathadvice中
void DispPath();    //输出路径
void AdvicePath();  //游览全程道路推荐
void ModSet(int w, int h);	//设置窗口大小
void setColor(unsigned short ForeColor, unsigned short BackGroundColor); //设置字体颜色
void SetPos(int x, int y);   //设置光标位置
void GetPos(int &x, int &y);     //获取当前控制台光标位置

/*----------主函数----------*/
int main()
{
    ModSet(79, 40);
    Init();
    MatToList(MGr, AGr);
    int flag = 1, x;
    Start();
    while (flag)
    {
        cin >> x;
        cin.clear();
        System(x, flag);
    }
    return 0;
}

/*----------开始界面----------*/
void Start()
{
    SetPos(25, 15); //设置光标位置，就是需要显示的位置
    setColor(10, 0);    //设置颜色
    cout << "苏州大学独墅湖校区地图导览系统" << endl;
    SetPos(27, 17); //设置光标位置，就是需要显示的位置
    setColor(12, 0);
    cout << "密码:";
    setColor(6, 6);
    cout << "                    ";
    SetPos(32, 17);
}

/*----------菜单系统----------*/
void System(int n, int& fl)
{
    setColor(7, 0);
    system("cls");
    if (n != 2020612)   //密码输入不正确，无法进入系统
    {
        Start();
        SetPos(27, 18);
        setColor(4, 0);
        cout << "密码输入有误，请重新输入！" << endl;
        SetPos(32, 17);
        setColor(6, 6);
        return;
    }
    SetPos(0, 0);
    setColor(7, 0);
    cout << "--------------------欢迎来到苏州大学独墅湖校区地图导览系统--------------------" << endl;
    Floyd();
    int x = 6, flag = 1;
    while (flag)
    {
        switch (x)
        {
        case 0:
            flag = 0;
            system("cls");
            SetPos(11, 18);
            setColor(10, 0);
            cout << "感谢使用苏州大学独墅湖校区地图导览系统，欢迎下次使用！" << endl;
            SetPos(0, 35);
            setColor(7, 0);
            break;
        case 1:
            Output();
            break;
        case 2:
            cout << "---------------------------------建筑介绍查询---------------------------------" << endl;
            Introduce();
            cout << "------------------------------------------------------------------------------" << endl;
            break;
        case 3:
            cout << "---------------------------------最短路径查询---------------------------------" << endl;
            ShortDistance();
            cout << "------------------------------------------------------------------------------" << endl;
            break;
        case 4:
            cout << "---------------------------------所有路径查询---------------------------------" << endl;
            AllDistance();
            cout << "------------------------------------------------------------------------------" << endl;
            break;
        case 5:
            cout << "-------------------------------游览全程道路推荐-------------------------------" << endl;
            AdvicePath();
            cout << "------------------------------------------------------------------------------" << endl;
            break;
        case 6:
            Menu();
            break;
        case 7:
            system("cls");
            cout << "--------------------欢迎来到苏州大学独墅湖校区地图导览系统--------------------" << endl;
            Menu();
            break;
        default:
            cout << "输入有误，请重新输入！" << endl;
        }
        if (x != 0)
        {
            cout << "请选择你要进行的操作：";
            cin >> x;
        }
    }
    fl = 0;
}

/*----------输出菜单----------*/
void Menu()
{
    cout << "------------------------------------菜单--------------------------------------" << endl;
    cout << "-      1.输出地图及所有建筑编号                    2.建筑信息查询            -" << endl;
    cout << "-      3.建筑之间最短路径查询                      4.建筑之间所有路径查询    -" << endl;
    cout << "-      5.从某建筑出发游览全部建筑路径推荐          6.显示菜单                -" << endl;
    cout << "-      7.清屏                                      0.结束程序                -" << endl;
    cout << "-----------------------------请选择你要进行的操作-----------------------------" << endl;
}

/*----------保存建筑信息----------*/
void Init()
{
    int i, j;
    MGr.view[0].num = 1;
    strcpy_s(MGr.view[0].date.name, "材料大楼");
    strcpy_s(MGr.view[0].date.number, "001");
    strcpy_s(MGr.view[0].date.introduce, "苏州大学纳米科学技术学院。");

    MGr.view[1].num = 2;
    strcpy_s(MGr.view[1].date.name, "蚕学实验室");
    strcpy_s(MGr.view[1].date.number, "002");
    strcpy_s(MGr.view[1].date.introduce, "附近有苏州市动物实验重点研究室。");

    MGr.view[2].num = 3;
    strcpy_s(MGr.view[2].date.name, "生化楼");
    strcpy_s(MGr.view[2].date.number, "003");
    strcpy_s(MGr.view[2].date.introduce, "南边为苏州大学高分子化学与物理研究所，东边为炳麟图书馆。");

    MGr.view[3].num = 4;
    strcpy_s(MGr.view[3].date.name, "炳麟图书馆");
    strcpy_s(MGr.view[3].date.number, "004");
    strcpy_s(MGr.view[3].date.introduce, "大佬们埋头苦学的地方。");

    MGr.view[4].num = 5;
    strcpy_s(MGr.view[4].date.name, "医学楼");
    strcpy_s(MGr.view[4].date.number, "005");
    strcpy_s(MGr.view[4].date.introduce, "苏州大学医学部。");

    MGr.view[5].num = 6;
    strcpy_s(MGr.view[5].date.name, "文科艺术楼");
    strcpy_s(MGr.view[5].date.number, "006");
    strcpy_s(MGr.view[5].date.introduce, "紧邻苏州大学政治与公共管理学院.");

    MGr.view[6].num = 7;
    strcpy_s(MGr.view[6].date.name, "公共教学楼");
    strcpy_s(MGr.view[6].date.number, "007");
    strcpy_s(MGr.view[6].date.introduce, "学生日常上课的地方。");

    MGr.view[7].num = 8;
    strcpy_s(MGr.view[7].date.name, "专家楼");
    strcpy_s(MGr.view[7].date.number, "008");
    strcpy_s(MGr.view[7].date.introduce, "紧邻苏州大学独墅湖校区教工餐厅。");

    MGr.view[8].num = 9;
    strcpy_s(MGr.view[8].date.name, "文科综合楼");
    strcpy_s(MGr.view[8].date.number, "009");
    strcpy_s(MGr.view[8].date.introduce, "在苏州大学凤凰传媒学院附近哦。");

    MGr.view[9].num = 10;
    strcpy_s(MGr.view[9].date.name, "文轩楼");
    strcpy_s(MGr.view[9].date.number, "010");
    strcpy_s(MGr.view[9].date.introduce, "药学院。");

    MGr.view[10].num = 11;
    strcpy_s(MGr.view[10].date.name, "金螳螂商学院");
    strcpy_s(MGr.view[10].date.number, "011");
    strcpy_s(MGr.view[10].date.introduce, "对面就是金螳螂建筑学院。");

    MGr.view[11].num = 12;
    strcpy_s(MGr.view[11].date.name, "金螳螂建筑学院");
    strcpy_s(MGr.view[11].date.number, "012");
    strcpy_s(MGr.view[11].date.introduce, "秉承“江南古典园林意蕴、苏州香山匠人精神”，肩负延续中国现代建筑教育发端的历史使命。");

    MGr.view[12].num = 13;
    strcpy_s(MGr.view[12].date.name, "公共体育楼");
    strcpy_s(MGr.view[12].date.number, "013");
    strcpy_s(MGr.view[12].date.introduce, "体育楼，健身放松的地方。");

    MGr.view[13].num = 14;
    strcpy_s(MGr.view[13].date.name, "唐仲英医学研究院大楼");
    strcpy_s(MGr.view[13].date.number, "014");
    strcpy_s(MGr.view[13].date.introduce, "为江苏乃至全国的科教事业和医疗卫生事业做出巨大贡献。");

    for (i = 0; i < ViewNum; i++)
    {
        for (j = 0; j < ViewNum; j++)
        {
            if (i == j)
                MGr.length[i][j] = 0;
            else
                MGr.length[i][j] = Inf;
        }
    }
    MGr.length[0][1] = MGr.length[1][0] = 100;
    MGr.length[0][2] = MGr.length[2][0] = 300;
    MGr.length[0][5] = MGr.length[5][0] = 400;
    MGr.length[0][7] = MGr.length[7][0] = 420;
    MGr.length[1][2] = MGr.length[2][1] = 200;
    MGr.length[2][3] = MGr.length[3][2] = 150;
    MGr.length[2][5] = MGr.length[5][2] = 250;
    MGr.length[2][7] = MGr.length[7][2] = 425;
    MGr.length[3][4] = MGr.length[4][3] = 150;
    MGr.length[3][5] = MGr.length[5][3] = 250;
    MGr.length[3][6] = MGr.length[6][3] = 250;
    MGr.length[4][5] = MGr.length[5][4] = 350;
    MGr.length[4][6] = MGr.length[6][4] = 200;
    MGr.length[4][12] = MGr.length[12][4] = 450;
    MGr.length[5][6] = MGr.length[6][5] = 250;
    MGr.length[5][7] = MGr.length[7][5] = 250;
    MGr.length[5][8] = MGr.length[8][5] = 400;
    MGr.length[5][9] = MGr.length[9][5] = 250;
    MGr.length[5][10] = MGr.length[10][5] = 350;
    MGr.length[6][9] = MGr.length[9][6] = 400;
    MGr.length[6][10] = MGr.length[10][6] = 220;
    MGr.length[6][12] = MGr.length[12][6] = 300;
    MGr.length[7][8] = MGr.length[8][7] = 200;
    MGr.length[7][9] = MGr.length[9][7] = 250;
    MGr.length[8][9] = MGr.length[9][8] = 200;
    MGr.length[8][11] = MGr.length[11][8] = 540;
    MGr.length[8][13] = MGr.length[13][8] = 310;
    MGr.length[9][10] = MGr.length[10][9] = 300;
    MGr.length[9][11] = MGr.length[11][9] = 360;
    MGr.length[9][13] = MGr.length[13][9] = 400;
    MGr.length[10][11] = MGr.length[11][10] = 180;
    MGr.length[10][12] = MGr.length[12][10] = 230;
    MGr.length[10][13] = MGr.length[13][10] = 650;
    MGr.length[11][12] = MGr.length[12][11] = 280;
    MGr.length[11][13] = MGr.length[13][11] = 590;
    MGr.n = ViewNum; MGr.e = LoadNum;

    for (i = 0; i < ViewNum; i++)
    {
        for (j = 0; j < ViewNum; j++)
        {
            road [i][j] = MGr.length[i][j];
        }
    }   //获得邻接矩阵
}

/*----------输出地图----------*/
void map()
{
    int x, y;
    initgraph(640, 480);//新建窗口
    IMAGE img;  //准备图片
    loadimage(&img, L"map.jpg", 640, 480); //加载图片  L"背景.jpg"是图片的相对路径,也可以使用绝对路径
    putimage(0, 0, &img);   //在指定位置贴图  前面的(0,0)是坐标  后面的&img是要贴的图片
    cin.get();
    cin.get();
    closegraph();   //关闭窗口
    GetPos(x, y);
    SetPos(x, y - 1);
    return;
}

/*----------输出地图，建筑名称及其编号----------*/
void Output()
{
    map();
    cout << "-----------------苏州大学独墅湖校区所有建筑及其编号如下所示-------------------" << endl;
    cout << "-       1.材料大楼                                  2.蚕学实验室             -" << endl;
    cout << "-       3.生化楼                                    4.炳麟图书馆             -" << endl;
    cout << "-       5.医学楼                                    6.文科艺术楼             -" << endl;
    cout << "-       7.公共教学楼                                8.专家楼                 -" << endl;
    cout << "-       9.文科综合楼                               10.文轩楼                 -" << endl;
    cout << "-      11.金螳螂商学院                             12.金螳螂建筑学院         -" << endl;
    cout << "-      13.公共体育楼                               14.唐仲英医学研究院大楼   -" << endl;
    cout << "------------------------------------------------------------------------------" << endl;
}

/*----------建筑介绍----------*/
void Introduce()
{
    int m;
    cout << "请输入查询建筑的编号(1->" << ViewNum << ")：";
    cin>>m;
    while (m > ViewNum || m <= 0)
    {
        cout << "输入信息错误！" << endl;
        cout << "请输入查询建筑编号(1->" << ViewNum << ")：";
        cin >> m;
    }
    cout << "建筑编号：" << MGr.view[m - 1].date.number << "\t建筑名称:" << MGr.view[m - 1].date.name << endl;
    cout << "建筑介绍:" << MGr.view[m - 1].date.introduce << endl;
}

/*----------Floyd算法----------*/
void Floyd()
{
    int i, j, k;
    for (i = 0; i < ViewNum; i++)
    {
        for (j = 0; j < ViewNum; j++)
        {
            shortest[i][j] = MGr.length[i][j];
            path[i][j] = 0;
        }
    }   /*初始化数组*/
    for (k = 0; k < ViewNum; k++)
    {
        for (i = 0; i < ViewNum; i++)
        {
            for (j = 0; j < ViewNum; j++)
            {
                if (shortest[i][j] > (shortest[i][k] + shortest[k][j]))
                {
                    shortest[i][j] = shortest[i][k] + shortest[k][j];
                    path[i][j] = k;
                    path[j][i] = k;   /*记录经过的路径*/
                }
            }
        }
    }
}

/*----------打印两个建筑的路径及最短路径----------*/
void Display(int i, int j)
{         
    int a, b;
    a = i;
    b = j;
    cout<<"您要查询的两建筑间最短路径是："<<endl;
    if (i < j)
    {
        int k = 0, sort[ViewNum];     //反序记录
        sort[k] = b;
        while (path[i][j] != 0)
        {
            k++;
            sort[k] = path[i][j];
            if (i < j)
                j = path[i][j];
            else
                i = path[j][i];
        }
        cout << MGr.view[a].date.name;
        for (k; k >= 0; k--)
            cout << "-->" << MGr.view[sort[k]].date.name;
        cout << endl;
        cout << MGr.view[a].date.name << "-->" << MGr.view[b].date.name << "的最短路径是：" << shortest[a][b] << "米" << endl;
    }
    else
    {
        cout << MGr.view[a].date.name;
        while (path[i][j] != 0)
        {
            cout << "-->" << MGr.view[path[i][j]].date.name;
            if (i < j)
                j = path[i][j];
            else
                i = path[j][i];
        }
        cout<<"-->" << MGr.view[b].date.name<<endl;
        cout << MGr.view[a].date.name << "-->" << MGr.view[b].date.name << "的最短路径是："<<shortest[a][b]<<"米" << endl;
    }
}

/*----------查找两建筑之间的最短路径----------*/
void ShortDistance()
{
    int i, j;
    cout << "请输入要查询的两个建筑的数字编号(1->" << ViewNum << ")中间用空格间隔开：";
    cin>>i>>j;
    while (i > ViewNum || i <= 0 || j > ViewNum || j <= 0)
    {
        cout << "输入信息错误！" << endl;
        cout << "请输入要查询的两个建筑的数字编号(1->" << ViewNum << ")中间用空格间隔开：";
        cin >> i >> j;
    }
    Display(i-1, j-1);
}

/*----------创建图的邻接表----------*/
void CreateAdj(AGraph*& G, int A[ViewNum][ViewNum], int n, int e) 
{
    int i, j;
    ArcNode* p;
    G = (AGraph*)malloc(sizeof(AGraph));
    for (i = 0; i < n; i++)								//给邻接表中所有头结点的指针域置初值
        G->adjlist[i].firstarc = NULL;
    for (i = 0; i < n; i++)								//检查邻接矩阵中每个元素
        for (j = n - 1; j >= 0; j--)
            if (A[i][j] != 0 && A[i][j] != Inf)			//存在一条边
            {
                p = (ArcNode*)malloc(sizeof(ArcNode));	//创建一个结点p
                p->adjvex = j;
                p->weight = A[i][j];
                p->nextarc = G->adjlist[i].firstarc;	//采用头插法插入结点p
                G->adjlist[i].firstarc = p;
            }
    G->n = n; G->e = n;
}

/*----------将邻接矩阵g转换成邻接表G----------*/
void MatToList(MGraph g, AGraph*& G)
{
    int A[ViewNum][ViewNum];
    int i, j;
    for (i = 0; i < ViewNum; i++)
    {
        for (j = 0; j < ViewNum; j++)
        {
            A[i][j] = MGr.length[i][j];
        }
    }   //获得邻接矩阵
    CreateAdj(G, A, g.n, g.e);  //创建邻接表

}

/*----------求顶点u到顶点v的所有的简单路径----------*/
void PathAll(AGraph* G, int u, int v, int path[], int d)//求顶点u到顶点v的所有的简单路径
{
    ArcNode* p;
    int j, w;
    d++; path[d] = u;		//路径长度d增加1，将当前顶点添加到路径中
    visited[u] = 1;
    if (u == v && d > 0)        //找到终点
    {
        ofstream outfile(filename, ios::app);
        outfile << MGr.view[path[0]].date.name;
        for (j = 1; j <= d; j++)
            outfile << "-->" << MGr.view[path[j]].date.name;
        outfile << endl;
        outfile.close();
    }
    p = G->adjlist[u].firstarc;     //p指向第一个顶底u的第一个相邻点
    while (p != NULL)
    {
        w = p->adjvex;          //w为u的相邻点编号
        if (visited[w] == 0)    //若该顶点未标记访问，则递归访问之
            PathAll(G, w, v, path, d);
        p = p->nextarc;     //找u的下一个相邻点
    }
    visited[u] = 0;
}

/*----------查找两建筑之间的所有路径----------*/
void AllDistance()
{
    int i, j, k;
    cout << "请输入要查询的两个建筑的数字编号(1->" << ViewNum << ")中间用空格间隔开：";
    cin >> i >> j;
    while (i > ViewNum || i <= 0 || j > ViewNum || j <= 0)
    {
        cout << "输入信息错误！" << endl;
        cout << "请输入要查询的两个建筑的数字编号(1->" << ViewNum << ")中间用空格间隔开：";
        cin >> i >> j;
    }
    cout << "请等待10s左右...";
    strcpy_s(filename, MGr.view[i - 1].date.name);
    strcat_s(filename, "至");
    strcat_s(filename, MGr.view[j - 1].date.name);
    strcat_s(filename, ".txt");
    ofstream outfile(filename);
    outfile << "你可以选择的所有路径:" << endl;
    outfile.close();
    for (k = 0; k < MaxViewNum; k++)
        visited[k] = 0;
    PathAll(AGr, i - 1, j - 1, pathall, -1);
    cout << "\r所有路径已保存到 "<<filename<<" 文件中，请查阅。" << endl;
}

/*----------求出动态规划dp数组----------*/
void TSP()
{
    for (int i = 0; i < ViewNum; i++) //初始化dp[i][0]
        dp[i][0] = road[i][0];
    for (int j = 1; j < M; j++) //求解dp[i][j],先跟新列再更新行
    {
        for (int i = 0; i < ViewNum; i++) {
            dp[i][j] = Inf;
            if (((j >> (i - 1)) & 1) == 1) //如果集和j(或状态j)中包含结点i,则不符合条件退出
                continue;
            for (int k = 1; k < ViewNum; k++)
            {
                if (((j >> (k - 1)) & 1) == 0)
                    continue;
                if (dp[i][j] > road[i][k] + dp[k][j ^ (1 << (k - 1))])
                    dp[i][j] = road[i][k] + dp[k][j ^ (1 << (k - 1))];
            }
        }
    }

}

/*----------判断结点是否都以访问,不包括0号结点----------*/
bool IsVisited(bool visited[])
{
    for (int i = 1; i < ViewNum; i++)
        if (visited[i] == false)
            return false;
    return true;
}

/*----------获取最优路径并保存在pathadvice中----------*/
void GetPath()
{
    bool visited[ViewNum] = { false };  //标记访问数组
    int pioneer = 0, min = Inf, S = M - 1, temp;    //前驱节点编号
    apath.push_back(0);      //把起点结点编号加入容器

    while (!IsVisited(visited))
    {
        for (int i = 1; i < ViewNum; i++)
            if (visited[i] == false && (S & (1 << (i - 1))) != 0)
                if (min > road[i][pioneer] + dp[i][(S ^ (1 << (i - 1)))])
                {
                    min = road[i][pioneer] + dp[i][(S ^ (1 << (i - 1)))];
                    temp = i;
                }
        pioneer = temp;
        apath.push_back(pioneer);
        visited[pioneer] = true;
        S = S ^ (1 << (pioneer - 1));
        min = Inf;
    }
    vector<int>::iterator  it = apath.begin();
    int i = 0;
    for (it; it != apath.end(); it++)
    {
        pathadvice[i] = *it;
        i++;
    }           //将其保存到pathadvice中
}

/*----------输出路径----------*/
void DispPath()
{
    int x,i;
    cout << "请输入出发点的建筑的数字编号(1->" << ViewNum << ")：";
    cin >> x;
    while (x > ViewNum || x<=0)
    {
        cout << "输入信息错误！" << endl;
        cout << "请输入出发点的建筑的数字编号(1->" << ViewNum << ")：";
        cin >> x;
    }
    cout << "推荐路径为：" << endl;
    for (i = 0; i < ViewNum && pathadvice[i] != x-1; i++);
    cout << MGr.view[pathadvice[i]].date.name << "-->";
    i = (i + 1) % ViewNum;
    do
    {
        cout << MGr.view[pathadvice[i]].date.name << "-->";
        i = (i + 1) % ViewNum;
    } while (pathadvice[i] != x-1);
    cout << MGr.view[pathadvice[i]].date.name << endl;
    cout << "该路径的长度为:" << dp[0][M - 1] << "米" << endl;
}

/*----------游览全程道路推荐----------*/
void AdvicePath()
{
    TSP();
    GetPath();
    DispPath();
}

/*----------设置窗口大小----------*/
void ModSet(int w, int h)
{
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD size = { w,h };
    SetConsoleScreenBufferSize(hout, size);
    SMALL_RECT rc = { 1,1,w,h };
    SetConsoleWindowInfo(hout, true, &rc);
    system("cls");
    return;
}

/*----------设置字体颜色----------*/
void setColor(unsigned short ForeColor, unsigned short BackGroundColor)

{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);//获取当前窗口句柄
    SetConsoleTextAttribute(handle, ForeColor + BackGroundColor * 0x10);//设置颜色
}

/*----------设置光标位置----------*/
void SetPos(int x, int y)

{
    COORD pos;
    HANDLE handle;
    pos.X = x;
    pos.Y = y;
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(handle, pos);
}

/*----------获取当前控制台光标位置----------*/
void GetPos(int &x, int &y)     //获取当前控制台光标位置
{
    CONSOLE_SCREEN_BUFFER_INFO c;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(h, &c);
    x = c.dwCursorPosition.X;
    y = c.dwCursorPosition.Y;
}
