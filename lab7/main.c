#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXVERTEX 25
#define MAXEDGE 100
#define INFTY 99999

//顶点结构体
typedef struct Vertex {
	int id;//顶点编号（唯一）
	int w;//顶点权值
	int degree;//顶点的度
	int edges[MAXVERTEX];//边的编号
} Vertex;

//边结构体
typedef struct Edge {
	int id;//边的编号（唯一）
	int h;//边头顶点编号
	int t;//边尾顶点编号
	int w;//权值
} Edge;

//图结构体
typedef struct Graph {
	Vertex v[MAXVERTEX];//顶点数组
	Edge e[MAXEDGE];//边数组
	int nv;//顶点数
	int ne;//边数
	bool dirctional;//t:有向图，f:无向图
	bool weighted;//t:带权图，f:等权图
} Graph ;

//保存图数据为html文件，供浏览器查看
void writeGraph(Graph *g, FILE *fp) {
	for (int i = 0; i < g->nv; ++i) // 为了显示独立结点
		fprintf(fp, "{source: '%d', target: '%d', 'rela': '', type: 'resolved'},\n", g->v[i].id, g->v[i].id);
	for (int i = 0; i < g->ne; ++i)
		fprintf(fp, "{source: '%d', target: '%d', 'rela': '%d', type: 'resolved'},\n", g->e[i].h, g->e[i].t, g->e[i].id);

}

//保存图为html文件，点击可展示图
void saveGraph(Graph *g, const char filename[]) {
	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		printf("Error opening file for writing!\n");
		exit(0);
	}
	//读取头部文件，二进制方式写入filename文件
	FILE *fh = fopen("head.txt", "rb");
	fseek(fh, 0, SEEK_END);
	unsigned long fsize = ftell(fh);
	rewind(fh);
	unsigned char *dataArray;
	dataArray = (unsigned char *)malloc(sizeof(unsigned char) * fsize);
	if (!dataArray) {
		printf("File is too large, not enough memory, read error! %d\n", fsize);
		exit(0);
	}
	fread(dataArray, sizeof(unsigned char), fsize, fh); //读取文件
	fwrite(dataArray, sizeof(unsigned char), fsize, fp); //写入文件
	free(dataArray);
	fclose(fh);

	//将图的信息写入filename文件
	fprintf(fp, "\n\nvar links = \n[\n");
	writeGraph(g, fp);
	fprintf(fp, "\n];");

	//读取尾部文件，二进制方式写入filename文件
	if (g->dirctional)//有向图
		fh = fopen("tail.txt", "rb");
	else //无向图
		fh = fopen("tail1.txt", "rb");
	fseek(fh, 0, SEEK_END);
	fsize = ftell(fh);
	rewind(fh);
	dataArray = (unsigned char *)malloc(sizeof(unsigned char) * fsize);
	if (!dataArray) {
		printf("File is too large, not enough memory, read error! %d\n", fsize);
		exit(0);
	}
	fread(dataArray, sizeof(unsigned char), fsize, fh); //读取文件
	fwrite(dataArray, sizeof(unsigned char), fsize, fp); //写入文件
	free(dataArray);
	fclose(fh);

	fclose(fp);

}

//生成一个随机图，包括n个顶点，每个顶点和其它顶点连边的概率为p
int randgengraph(int n, float p, bool directional, bool weighted, const char *filename, int seed) {
	srand(seed);
	char *content = (char *)malloc(5000000);
	if (!content) {
		printf("Failed to allocate storage space!\n");
		exit(0);
	}
	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		printf("Failed to generate graph file %s!\n", filename);
		exit(0);
	}
	long offset = 0;//content当前的写入位置
	int ne = 0;//生成边数
	for (int i = 0; i < n; i++)
		offset += sprintf(content + offset, "%3d %3d\n", i, rand() % 100);//写入顶点的编号和顶点随机权值
	int pn = (int)(10000 * p);
	if (directional) {//有向图
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				if (i != j && rand() % 10000 < pn) {
					offset += sprintf(content + offset, "%3d %3d %3d %3d\n", ne, i, j, weighted ? rand() % 100 : 1);
					ne++;
				}
	} else { //无向图
		for (int i = 0; i < n; i++)
			for (int j = i; j < n; j++)
				if (i != j && rand() % 10000 < pn) {
					offset += sprintf(content + offset, "%3d %3d %3d %3d\n", ne, i, j, weighted ? rand() % 100 : 1);
					ne++;
				}
	}
	char ch[40];
	int sz = sprintf(ch, "%3d %3d %3d %3d\n", n, ne, weighted ? 1 : 0, directional ? 1 : 0);
	fwrite(&ch, sz, 1, fp);//写入图的顶点数和边数
	fwrite(content, offset, 1, fp);//写入图的顶点和边的信息
	fclose(fp);
	free(content);
}

//从文件中读入图，在内存中生成图的表示
void initGraphFromFile(Graph *g, const char *gfile) {
	FILE *fp = fopen(gfile, "r");
	if (!fp) {
		printf("Error reading graph data file!\n");
		exit(0);
	}
	unsigned bv;
	//读取图的基本信息
	fscanf(fp, "%u%u%u", &(g->nv), &(g->ne), &bv);
	//printf("%d %d %s", g->ne, g->nv, bv);
	g->weighted = (bv == 1) ? true : false;
	fscanf(fp, "%u", &bv);
	g->dirctional = (bv == 1) ? true : false;
	//读取顶点信息
	for (int i = 0; i < g->nv; i++) {
		fscanf(fp, "%u%d", &(g->v[i].id), &(g->v[i].w));
		g->v[i].degree = 0;
		//printf("顶点%d-权%d\n", g->v[i].id, g->v[i].w);
	}
	//读取边信息
	for (unsigned long i = 0; i < g->ne; i++) {
		fscanf(fp, "%u%u%u%d", &(g->e[i].id), &(g->e[i].h), &(g->e[i].t), &(g->e[i].w));
		//printf("边:%u-%u=>%d\n", g->e[i].h, g->e[i].t, g->e[i].w);
		unsigned h, t;
		h = g->e[i].h;
		t = g->e[i].t;
		g->v[h].edges[g->v[h].degree++] = g->e[i].id;
		if (g->dirctional == false)
			g->v[t].edges[g->v[t].degree++] = g->e[i].id;
	}
	fclose(fp);
}

// 生成一个空图
Graph *initGraph(bool directional, bool weighted) {
	Graph *g = (Graph *)malloc(sizeof(Graph));
	g->nv = g->ne = 0;
	g->dirctional = directional;
	g->weighted = weighted;
	return g;
}

//查找给定编号的顶点，返回其顶点数组下标
int getVexIdx(Graph *g, int id) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id)
			return i;
	printf("input wrong vertex id in getVexIdx()!\n");
	exit(0);
}

//查找给定编号的顶点，返回其权值
int getVexW(Graph *g, int id) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id)
			return g->v[i].w;
	printf("input wrong vertex id in getVexW()!\n");
	exit(0);
}

//查找给定编号的顶点，返回其度
int getVexDegree(Graph *g, int id) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id)
			return g->v[i].degree;
	printf("input wrong vertex id in getVexDegree()!\n");
	exit(0);
}

//查找给定编号的顶点,返回其所有边
int *getEdgesByNode(Graph *g, int id, int *ne) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id) {
			*ne = g->v[i].degree;
			return g->v[i].edges;
		}
	printf("input wrong vertex id in getEdgesByNode()!\n");
	exit(0);
}

//查找给定编号的边，返回其边数组下标
int getEdgeIdx(Graph *g, int id) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].id == id)
			return i;
	printf("input wrong edge id in getEdgeIdx()!\n");
	exit(0);
}

//查找给定编号的边，返回其权值
int getEdgeW(Graph *g, int id) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].id == id)
			return g->e[i].w;
	printf("input wrong edge id in getEdgeW()!\n");
	exit(0);
}

//查找给定编号的边，返回其两个顶点
void getVexByEdge(Graph *g, int id, int *h, int *t) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].id == id) {
			*h = g->e[i].h;
			*t = g->e[i].t;
			return;
		}
	printf("input wrong edge id in getVexByEdge()!\n");
	exit(0);
}

//通过权值查找顶点编号
int locateVex(Graph *g, int w) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].w == w)
			return g->v[i].id;
	return -1;
}

//通过权值查找边编号
int locateEdge(Graph *g, int w) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].w == w)
			return g->e[i].id;
	return -1;
}

//设置顶点权值
void setVexW(Graph *g, int id, int w) {
	for (int i = 0; i < g->nv; i++) {
		if (g->v[i].id == id) {
			g->v[i].w = w;
			return;
		}
	}
	printf("input wrong vertex id in setVexW()!\n");
	exit(0);
}

//设置边权值
void setEdgeW(Graph *g, int id, int w) {
	for (int i = 0; i < g->ne; i++) {
		if (g->e[i].id == id) {
			g->e[i].w = w;
			return;
		}
	}
	printf("input wrong edge id in setEdgeW()!\n");
	exit(0);
}

//从给定编号的顶点的edges数组中删去给定编号的边
void deleteEdgeFromVex(Graph *g, int vid, int eid) {
	int vIdx = getVexIdx(g, vid);	//标记给定编号顶点的index
	for (int i = 0; i < g->v[vIdx].degree; i++) {
		if (g->v[vIdx].edges[i] == eid) {	//若找到给定编号边，则删去之，即degree-=1，并将当前edges[i]用最后一项覆盖
			g->v[vIdx].degree -= 1;
			g->v[vIdx].edges[i] = g->v[vIdx].edges[g->v[vIdx].degree];
			return;
		}
	}
	printf("input wrong edge id in deleteEdgeFromVex()!\n");
	exit(0);
}

//删除边
void deleteEdge(Graph *g, int id) {
	for (int i = 0; i < g->ne; i++) {
		if (g->e[i].id == id) {
			deleteEdgeFromVex(g, g->e[i].h, g->e[i].id);	//在这条边对应的头端点的edges中删除这条边
			if (g->dirctional == false) {	//无向图在边的两端点都需要删除
				deleteEdgeFromVex(g, g->e[i].t, g->e[i].id);
			}
			g->ne -= 1;		//图中边数-1，并将当前e[i]用最后一项覆盖
			g->e[i].id = g->e[g->ne].id;
			g->e[i].h = g->e[g->ne].h;
			g->e[i].t = g->e[g->ne].t;
			g->e[i].w = g->e[g->ne].w;
			return;
		}
	}
	printf("input wrong edge id in deleteEdge()!\n");
	exit(0);
}

//删除顶点，同时与其关联的边全部删除，记得调用 deleteEdge() 哟~
void deleteVex(Graph *g, int id) {
	for (int i = 0; i < g->nv; i++) {
		if (g->v[i].id == id) {		//删除所有与其关联的边
			while (g->v[i].degree > 0) {
				deleteEdge(g, g->v[i].edges[g->v[i].degree - 1]);
			}

			g->nv -= 1;		//图中顶点数-1，并将当前e[i]用最后一项覆盖
			g->v[i].id = g->v[g->nv].id;
			g->v[i].w = g->v[g->nv].w;
			g->v[i].degree = g->v[g->nv].degree;
			for (int j = 0; j < MAXVERTEX; j++) {
				g->v[i].edges[j] = g->v[g->nv].edges[j];
			}
			return;
		}
	}
	printf("input wrong vertex id in deleteVex()!\n");
	exit(0);
}


//增加顶点
void addVex(Graph *g, int id, int w) {
	g->v[g->nv].id = id;	//赋给对应的id与w
	g->v[g->nv].w = w;
	g->v[g->nv].degree = 0;
	for (int j = 0; j < MAXVERTEX; j++) {
		g->v[g->nv].edges[j] = 0;
	}
	g->nv += 1;		//图中顶点数+1
}

//增加边
void addEdge(Graph *g, int id, int h, int t, int w) {
	g->e[g->ne].id = id;	//赋给对应的id与w，连接首尾端点
	g->e[g->ne].h = h;
	g->e[g->ne].t = t;
	g->e[g->ne].w = w;
	g->ne += 1;

	int hIdx = getVexIdx(g, h);
	g->v[hIdx].edges[g->v[hIdx].degree] = id;	//相应端点的edges中存入该边
	g->v[hIdx].degree += 1;
	if (g->dirctional == false) {		//无向图需要同时存两端点
		int tIdx = getVexIdx(g, t);
		g->v[tIdx].edges[g->v[tIdx].degree] = id;
		g->v[tIdx].degree += 1;
	}
}

//打印邻接表的信息
void printgraph(Graph *g) {
	printf("Basic information of the graph: Number of vertices (%u) - Number of edges(%lu)-%s-%s\n", g->nv, g->ne,
		   g->weighted ? "Weighted graph" : "Unweighted graph", g->dirctional ? "Directed graph" : "Undirected graph");
	for (int i = 0; i < g->nv; i++) {
		printf("ID(%u)-Degree(%d)-Weight(%d)-Edge list: ", g->v[i].id, g->v[i].degree, g->v[i].w);
		for (int j = 0; j < g->v[i].degree; j++) {
			unsigned long e = getEdgeIdx(g, g->v[i].edges[j]);
			printf("(%2u:%u-%u,%2d) ", g->e[e].id, g->e[e].h, g->e[e].t, g->e[e].w);
		}
		printf("\n");
	}
	printf("\n");
}

//复杂操作，Dijkstra算法，有向图亦视作无向图
void Dijkstra(Graph *g, int id, int id1) {
	int idx, cnt, minidx, mindist;
	int w[MAXVERTEX][MAXVERTEX], dist[MAXVERTEX], pre[MAXVERTEX];	//w以两端点作为索引存边权值，dist,pre存当前到该顶点的最小距离及其前驱节点
	bool flag[MAXVERTEX];
	for (int i = 0; i < g->nv; i++) {		//初始化
		dist[i] = INFTY;
		pre[i] = -1;
		flag[i] = false;
		for (int j = 0; j < g->nv; j++) {
			w[i][j] = INFTY;
		}
	}
	idx = getVexIdx(g, id);
	cnt = 1;
	dist[idx] = 0;
	flag[idx] = true;

	for (int i = 0; i < g->ne; i++) {	//将边权值存入w
		w[g->e[i].h][g->e[i].t] = w[g->e[i].t][g->e[i].h] = g->e[i].w;
	}

	for (int i = 0; i < g->nv; i++) {
		printf("\nw[%d] = ", i);
		for (int j = 0; j < g->nv; j++) {
			printf("%6d", w[i][j]);
		}
	}

	while (cnt <= g->nv) {
		for (int i = 0; i < g->nv; i++) {
			for (int j = 0; j < g->nv; j++) {
				if (!flag[i] && flag[j] && dist[j] + w[i][j] < dist[i]) {	//Dijkstra算法，更新当前各节点最小长度dist
					dist[i] = dist[j] + w[i][j];
					pre[i] = j;
				}
			}
		}
		printf("\ncnt =%2d, dist =", cnt);
		for (int i = 0; i < g->nv; i++) {
			printf("%6d", dist[i]);
		}

		minidx = -1;
		mindist = INFTY;
		for (int i = 0; i < g->nv; i++) {		//将当前节点最小长度中最小者纳入flag=true，即已固定其最小长度及其路径
			if (!flag[i] && dist[i] < mindist) {
				mindist = dist[i];
				minidx = i;
			}
		}
		flag[minidx] = true;
		printf(", minidx =%2d", minidx);
		cnt++;
	}


	int pre_aim;
	cnt = 0;
	pre_aim = getVexIdx(g, id1);
	printf("\nminroute =");
	while (pre_aim != -1) {		//在图中用9999x为id的节点标记出最短路径，同时也调用了更多基本操作
		printf("%3d", g->v[pre_aim].id);	//通过pre依次回溯
		addVex(g, INFTY-cnt , -1);
		addEdge(g, INFTY-cnt, INFTY-cnt, pre_aim, 0);
		cnt += 1;
		pre_aim = pre[pre_aim];
	}
	printf("\n");
}

int main() {
	Graph *g = (Graph *)malloc(sizeof(Graph));
	if (!g) {
		printf("error\n");
		exit(0);
	}
	int k, h, t;

	randgengraph(10, 0.2, 1, 1, "graph.txt", 5);
	initGraphFromFile(g, "graph.txt");
	printgraph(g);
	saveGraph(g, "net.html");

	addVex(g, 88, 30);
	addEdge(g, 88, 1, 3, 50);
	getVexByEdge(g, k = 10, &h, &t);
	printf("edge %d, with vex %d -> %d\n", k, h, t);
	deleteEdge(g, 10);
	deleteVex(g, 8);
	printgraph(g);
	saveGraph(g, "net1.html");

	randgengraph(10, 0.2, 0, 0, "graph2.txt", 23000020);
	initGraphFromFile(g, "graph2.txt");
	printgraph(g);

	addVex(g, 88, 30);
	addEdge(g, 88, 1, 3, 50);
	getVexByEdge(g, k = 6, &h, &t);
	printf("edge %d, with vex %d -> %d\n", k, h, t);
	deleteEdge(g, 6);
	deleteVex(g, 1);
	printgraph(g);

	initGraphFromFile(g, "Dijkstra.txt");
	printgraph(g);
	saveGraph(g, "Dijkstra_before.html");
	Dijkstra(g, 3, 4);
	printgraph(g);
	saveGraph(g, "Dijkstra_after.html");
}