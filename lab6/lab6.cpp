//哈夫曼树 ：分配连续存储空间存储树 

#include <stdio.h>
#include <stdlib.h>

//编码的最大长度 
#define MCL 30 
//定义叶节点数,8bit的unsigned char 
#define n 256
//定义结点总数，2n-1 
#define m 511 

#define codetype unsigned

//定义树结点，包括三个指针父节点，左右孩子
typedef struct HTNode{
	unsigned id;
	bool leaf;
	long w; 
  	struct HTNode * parent,* lchild, * rchild;		//改为三叉链表
} HTNode; 	


typedef struct {
	codetype code; //前面都是0，最后len位是编码 
	int len;   //编码长度 
} HCode;      //哈夫曼编码表

/*** 以下代码哈夫曼树生成 ***/
//构造哈夫曼树/二叉树,输入长度为n=256的权向量w（每个字符出现的次数）
HTNode *createHTree(long* w) {
	long allw = 0;    //allw为总权值
	int numZero = 0;  
	HTNode *node[m];   //用一个结点的指针数组寻找各个结点
	for (int i = 0; i < m; ++i) {
		node[i] = (HTNode *)malloc(sizeof(HTNode)); //创建结点
	    if (!node[i]) 
			exit(0);   //检验是否分配空间成功
		node[i]->id = i;
		node[i]->parent = node[i]->lchild = node[i]->rchild = NULL;  //初始化，双亲和孩子结点均指向空
		node[i]->w = 0;
		node[i]->leaf = i < n ? true : false;
		if (i < n) {
			node[i]->w = w[i];
			if (w[i] == 0) numZero++; //统计没出现的字符个数 
			allw += w[i];
		}
	} //初始化哈夫曼树，每个结点构成一棵树 

	int i = n, j;
	long min1, min2;
	while (i < m - numZero) {
		min1 = min2 = allw + 10; //设置最大的w值 
		unsigned pos1, pos2;
		for (j = 0; j < i; j++) {
			if (node[j]->w == 0) //忽略权值为0的结点 
				continue;
			if (node[j]->parent == NULL)   //只找某棵树的根结点进行比较
				if (node[j]->w < min1) {
					pos2 = pos1;
					min2 = min1;  //原本的最小变为第二小
					pos1 = j;
					min1 = node[j]->w;
				} //min1<min2,找到比min1更小的ht[j].w，那么min1替代min2， ht[j].w替代min1
				else {
					if (node[j]->w < min2) { //min1 <= node[j].w < min2 
						pos2 = j;
						min2 = node[j]->w;   //则min2变为node[j].w
					}
				}
		}//内层for结束，找到pos1,pos2最小的两个权值，准备构造非叶节点

		node[i]->lchild = node[pos1];  //小权值为左孩子
		node[pos1]->parent =node[i];
		node[i]->rchild = node[pos2];  //另一个为右孩子
		node[pos2]->parent =node[i];   //同时要改变子结点的父结点
		node[i]->w = min1 + min2;    //新的树根结点权值为2者相加
		i++;   //进入下一个循环
	}
	
	for(i = 0; i < m; i++)
		if(node[i]->w = 0)
			free(node[i]);
	return node[m-numZero-1]; //返回哈夫曼树的根结点
}      

//给定二叉树根结点，遍历二叉树 (先序遍历） 输出id和w
void printHtree0(HTNode *root) {
	if (!root) { //递归出口
		return;
	}
	if (root->leaf) {
		printf("(%c) -> %3d, %3d, %5ld\n", root->id, root->id, root->leaf, root->w);
	} else {
		printf("%3d -> %3d, %3d, %5ld\n", root->id, root->id, root->leaf, root->w);
	}

	printHtree0(root->lchild);
	printHtree0(root->rchild);
}

/*** 以下代码分析文件和从文件中生成哈夫曼编码表 ***/

//从filename中二进制读取字符，统计每个字符出现的次数，写入w并返回 
//w数组长度为256，如果字符不足256，那么没出现的字符权值为0  
unsigned char* parseFile(const char filename[], long* w, long* fsize) {
	FILE* fp = fopen(filename, "rb");  //二进制只读模式
	if (fp == NULL) {
		printf("Can not open the File!\n");
		exit(0);
	}
	fseek(fp, 0, SEEK_END);   //将指针移到文件结尾处
	*fsize = ftell(fp);       //获取指针当前位置（即获取文件字符数量）
	rewind(fp);               //将指针指向文件头
	unsigned char* dataArray;
	dataArray = (unsigned char*)malloc(sizeof(unsigned char) * (*fsize));
	if (!dataArray) {
		printf("Memory is not enough!\n");
		exit(0);
	}
	fread(dataArray, sizeof(unsigned char), *fsize, fp);   //读取待压缩文件，将二进制数据读取到dataArray
	fclose(fp);   //关闭文件

	for (int i = 0; i < n; ++i) //权值清空 
		w[i] = 0;
    
	for (int i = 0; i < *fsize; ++i) //权值+1，若对应字符出现 
		w[dataArray[i]]++;    //若读到某个字符，则其编码对应的w加一

	return dataArray;
}

//由结点的id（可以改成其它数据域的值）查找结点，返回结点指针 。(先序遍历） 
HTNode *locateByID(HTNode *root, int id) {
	if (!root)
		return NULL;
	if (root->id == id)
		return root;    //找到目标值时返回相应结点
	HTNode *t = locateByID(root->lchild, id); //否则在左右子树中递归查找
	if (t)
		return t;
	return locateByID(root->rchild, id);
}


//输入任何字符c，得到它的哈夫曼编码, 被genCodes()调用 
void getCode(HTNode *root, int c, codetype* code1, int* clen1) {
	int clen = 0;        //编码长度 
	codetype code = 0;   //编码 

	int size = sizeof(code);  //编码的最大bits数 
	codetype mask = 1ull << (size * 8 - 1); //最高位为1其它为0，用来设置最高位为1 (掩码？)

	HTNode *p = locateByID(root, c);  //定位代表c的结点的位置
	HTNode *p_parent = p->parent;
	while (p_parent) {   //当未到达根结点时不断寻找父结点
		code = code >> 1;  //右移一位，即此时最高位变为0
		(p_parent->lchild == p) ? code : (code = code | mask); //如果是右孩子，那么最高位设置为1，否则是左孩子最高位就是0 
		clen++;
		p = p->parent;
		p_parent = p_parent->parent;  //向根结点移动
	}
    
	code = code >> (size * 8 - clen);
	*code1 = code;  //返回值 
	*clen1 = clen;  //返回值 
}

//从哈夫曼树ht生成完整的编码表hc，输出hc为encode()的输入参数 
void getHCodes(HCode hc[], HTNode *root, long w[256]) {
	for (int i = 0; i < n; ++i)
	{
		if(w[i] > 0)  //当w！=0时求其编码
			getCode(root, i, &(hc[i].code), &(hc[i].len));
	}
}

/*** 以下为编解码实现 ***/

//编码,原长olen的内容orgi，压缩编码后为长nlen的内容newc，需要利用编码表hc  (这个部分没用上哈夫曼树)
void encode(unsigned char* orgi, long olen, unsigned char* newc, long* nlen, HCode hc[]) {
	int sizec = sizeof(unsigned int);  //要确保字符编码的最大长度要小于sizec*8-8 
	long i = 0; //orgi的下标 
	long j = 0; //newc的下标 
	newc[j] = 0;
	codetype code;
	int len;
	int usedbits = 0; //newc[j]中被占用的bits数 
	while (i < olen) {//对原始数据逐个字符处理 
		int idx = orgi[i]; //获得hc下标
		code = hc[idx].code;  //从编码表获取长度及编码
		len = hc[idx].len;
		while (len + usedbits >= 8) {//code需要左移usedbits长的一段，然后和newc[j]位或操作 （？）
			codetype code1 = code >> len + usedbits - 8;
			newc[j] = newc[j] | code1; //填满前一个编码字符 
			len = len + usedbits - 8;      //code现在有效长度
			usedbits = 0;
			newc[++j] = 0;
		}//循环，使得len+usedbits <8
		//将code的前面各位清0,因为有效位是len
		code = code & ((1ull << len) - 1); //code和后面k个1，其它全为0的数据进行与，保留后面len位 
		code = code << 8 - len - usedbits;//向前移动若干位 
		newc[j] = newc[j] | code;
		usedbits = len + usedbits;
		i++;
	}
	*nlen = j + 1;
}

//将哈夫曼树先序输入到文件中
void printHtree_file(HTNode *root, FILE *fp) {
	if (!root) { //递归出口
		return;
	}
	fwrite(&(root->id), sizeof(unsigned char), 1, fp);		//只需要保存id与leaf即可，先序遍历序列已经蕴含亲子节点关系
	fwrite(&(root->leaf), sizeof(bool), 1, fp);
	printf("we are writing, with id = %3d, leaf = %d\n", root->id, root->leaf);
	printHtree_file(root->lchild, fp);		//对于左右子树进行递归调用
	printHtree_file(root->rchild, fp);
}

HTNode *rebuildHtree(unsigned char ht_ch[], bool ht_leaf[], bool first) {
	HTNode *t;
	static int idx = 0;		//参照lab5中prelist的思想，使用static变量idx
	if(first){
		idx = 0;
	}
	if (!(t = (HTNode *)malloc(sizeof(HTNode))))	
		exit(0);

	t->id = ht_ch[idx];		//初始化赋值
	t->leaf = ht_leaf[idx];
	t->w = 0;
	t->parent = t->lchild = t->rchild = NULL;
	idx++;
	if (t->leaf == 1) {		//注意以下事实，HTree中的叶子节点必定是代表具体字符的节点，而非此类节点必同时有左右子树，因此可通过前序遍历唯一确定并重构出HTree
		return t;
	} else {
		t->lchild = rebuildHtree(ht_ch, ht_leaf,false);	//对于左右子树进行递归调用
		t->lchild->parent = t;
		t->rchild = rebuildHtree(ht_ch, ht_leaf,false);
		t->rchild->parent = t;
	}
	return t;
}

HTNode *fetchHtree_file(FILE *fp) {
	unsigned char ht_ch[m];
	bool ht_leaf[m];
	fread(&(ht_ch[0]), sizeof(unsigned char), 1, fp);	//读入id
	fread(&(ht_leaf[0]), sizeof(bool), 1, fp);			//读入leaf
	printf("we are reading, with id = %3d, leaf = %d\n", ht_ch[0], ht_leaf[0]);
	int len = 1;
	fread(&(ht_ch[len]), sizeof(unsigned char), 1, fp);
	fread(&(ht_leaf[len]), sizeof(bool), 1, fp);
	printf("we are reading, with id = %3d, leaf = %d\n", ht_ch[len], ht_leaf[len]);
	while (ht_ch[len] != ht_ch[0] || ht_leaf[len] != ht_leaf[0]) {	//检测到哨兵节点即停止
		len += 1;
		fread(&(ht_ch[len]), sizeof(unsigned char), 1, fp);
		fread(&(ht_leaf[len]), sizeof(bool), 1, fp);
		printf("we are reading, with id = %3d, leaf = %d\n", ht_ch[len], ht_leaf[len]);
	}
	return rebuildHtree(ht_ch, ht_leaf,true);	//据此重构HTree
}

//生成和保存压缩文件,被压缩文件fin，指定文件名fout，将所用的哈夫曼树存入文件 
void zip(char fin[], char fout[]) {
	HCode hc[n]={0};    //结构体数组，表示哈夫曼编码表 :code,len，用于编码文件 
	long wDist[256]; //保存字符的分布（字符在文件中出现的次数） 
	long fsize;      //文件长度 
	//获取文件内容，分析待压缩文件，返回权值向量wDist,文件内容content和长度fsize 
	unsigned char* content = parseFile(fin, wDist, &fsize);

	// 生成压缩树（H树）		 
	HTNode *ht = createHTree(wDist); //root是哈夫曼树ht的根结点的下标 
	getHCodes(hc, ht, wDist);	//生成完整的编码表,用于编码文件 
	
	// 开始压缩
	unsigned char* zipContent; //编码后的内容	
	long zipsize;//压缩后文件大小 
	zipContent = (unsigned char*)malloc(sizeof(unsigned char) * (fsize + 10000)); //压缩后的文件可能更大，考虑将fsize扩大一点 
	if (!zipContent) {
		printf("The memory is not enough!\n");
		exit(0);
	}
	encode(content, fsize, zipContent, &zipsize, hc); //编码后返回长度zipsize的内容zipContent 

	FILE* fp = fopen(fout, "wb");
	if (fp == NULL) {
		printf("Can not open the file!\n");
		exit(0);
	}
	
	fwrite(&fsize, sizeof(fsize), 1, fp); //保存原始内容的大小
	fwrite(&zipsize, sizeof(zipsize), 1, fp); //保存编码内容的大小
	fwrite(zipContent, sizeof(unsigned char), zipsize, fp); //保存编码后的内容
	printHtree_file(ht, fp);		//保存哈夫曼树，解码要用到
	fwrite(&(ht->id), sizeof(unsigned char), 1, fp);
	fwrite(&(ht->leaf), sizeof(bool), 1, fp);  	//哨兵节点，表示结束
	fclose(fp);


	free(zipContent);	//释放文件内容
	free(content);
	printf("The compressed file: %s has been generated!\n", fout);
}


//读取压缩文件，解压 
void unzip(char zfile[], char ofile[]) {
	FILE* fp = fopen(zfile, "rb");
	if (fp == NULL) {
		printf("Can not open the file to read!\n");
		exit(0);
	}
	long zipsize1, fsize1;
	fread(&fsize1, sizeof(fsize1), 1, fp);  //被压缩内容的大小
	fread(&zipsize1, sizeof(zipsize1), 1, fp); //压缩后内容的大小

	unsigned char *ocontent = (unsigned char *)malloc(fsize1);//解码后的数据
	unsigned char *zcontent = (unsigned char *)malloc(zipsize1);//存放读取的编码数据
	fread(zcontent, zipsize1, 1, fp); //读取编码数据
	HTNode *root = fetchHtree_file(fp);
	fclose(fp);
    
	fp = fopen(ofile, "wb");
	if (fp == NULL) {
		printf("Can not decompress!\n");
		exit(0);
	}

	HTNode *p = root; //从根结点开始 
	int j, k = 0, i = 0;//zcontent的下标 k,ocontent的下标i 
	while (i < fsize1) {//依次写入解压后数据的每个字节 
		j = 128; //10000000 
		while (j > 0) {
			if ((zcontent[k] & j) > 0) 
			{ //向右走 1
				if (p->rchild == NULL) 
				{
					ocontent[i++] = p->id;
					p = root;//解码了下一个字符的第一个bit 
					j = j << 1;
				}
				else
					p = p->rchild;
			}
			else
			{//向左走 0
				if (p->lchild == NULL) 
				{
					ocontent[i++] = p->id;
					p = root;//解码了下一个字符的第一个bit 
					j = j << 1;
				}
				else
					p = p->lchild;
			}
			j = j >> 1; //j控制while循环8次，求出zcontent[k]的每一位 
		}
		k++;//准备读取下一个字符 
	}

	fwrite(ocontent, fsize1, 1, fp);	// 将解压内容写入文件
	fclose(fp);
	free(ocontent);
	free(zcontent);
	printf("The file: %s has been decompressed to: %s !\n", zfile, ofile);
}

int main() {
    // 压缩一个图片
	zip("png_test.png","png.myzip");
	unzip("png.myzip","png_out.png");
	// 压缩一个pdf
	zip("pdf_test.pdf","pdf.myzip");
	unzip("pdf.myzip","pdf_out.pdf");
	// 压缩一个文件
	zip("txt_test.txt","txt.myzip");
	unzip("txt.myzip","txt_out.txt");
}
