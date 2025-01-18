# include <bits/stdc++.h>
# define elemtype int

typedef struct linknode{
    elemtype data;
    struct linknode *next;
}linknode,*linklist;

linknode *initlist(){ //新建空链表
    linknode *p=(linknode *)malloc(sizeof(linknode)); //为头节点分配空间
    if (!p){
        exit(0);
    }

    p->data=0; //赋头节点初值
    p->next=NULL;
    return p;
}

void destroylist(linklist p){ //销毁链表
    while(p->next!=NULL){
        linknode *n=p->next; //从头节点所指向的节点开始删除
        p->next=n->next; 
        free(n);
    }
    free(p);
}

void clearlist(linklist p){ //清空链表
    while(p->next!=NULL){
        linknode *n=p->next; //从头节点所指向的节点开始删除
        p->next=n->next;
        free(n);
    }
    p->next=NULL;
    p->data=0;
}

int listempty(linklist p){ //判断链表是否为空
    if(p->next==NULL){
        return 1;
    }
    else{
        return 0;
    }
}

int listlength(linklist p){ //返回链表长度
    return p->data;
}

elemtype getelem(linklist p, int i){ //返回链表第i个元素
    if(i>listlength(p)) //防止i超出链表长度
    {
        printf("The target position exceeds the length of the list!\n");
        exit(0);
    }

    for(int k=0;k<i;k++){ //获取第i个元素地址
            p=p->next;
        }
    return p->data;
}

linknode *locateelem(linklist p, elemtype e){ //返回链表中首个值为e的元素地址
    p=p->next;
    while(p->next!=NULL){
        if(p->data==e){ //判定p的数据域与e是否相同
            return p;
        }
        p=p->next;
    }

    if(p->data==e){ //追加判断p在末尾的情形
        return p;
    }
    return NULL;
}

linknode *priorelem(linklist p, linknode *n){ //返回前驱的地址
    p=p->next;
    while(p->next!=NULL){ 
        if(p->next==n){ //判定p的指针域是否指向目标节点
            return p;
        }
        p=p->next;
    }
    printf("The target node is the head node or the target node does not exist!\n");
    exit(0);
}

linknode *nextelem(linklist p, linknode *n){ //返回后继的地址
    p=p->next;
    while(p->next!=NULL){
        if(p==n){ 
            return p->next; //当p抵达目标节点时返回其后继
        }
        p=p->next;
    }
    printf("The target node is the tail node or the target node does not exist!\n");
    exit(0);
}

void listinsert(linklist p, int i, elemtype e){ //插入值为e的元素于第i个位置
    if(i>listlength(p)+1)  //防止i超出链表长度+1
    {
        printf("The target position exceeds the length of the list!\n");
        exit(0);
    }

    linknode *n=(linknode *)malloc(sizeof(linknode)); //为新增节点分配空间
    if (!n){
        exit(0);
    }
    n->data=e; //为新增节点赋初值
    n->next=NULL;
    
    p->data++;

    for(int k=0;k<i-1;k++){ //找到目标位置
        p=p->next;
    }
    n->next=p->next; //修改相关指针
    p->next=n;
}

void listdelete(linklist p, int i){ //删除第i个元素
    if(i>listlength(p))  //防止i超出链表长度
    {
        printf("The target position exceeds the length of the list!\n");
        exit(0);
    }

    p->data--;
    for(int k=0;k<i-1;k++){ //获取第i-1个元素地址
        p=p->next;
    }

    linknode *n=p->next; 
    p->next=n->next; //修改相关指针与释放空间
    free(n);
}

void traverselist(linklist p,void (*func)(linknode *)){ //遍历链表
    while(p->next!=NULL){
        p=p->next;
        (*func)(p);
    }
    printf("\n");
}

void listprint(linknode *n){ //打印链表中各元素的值，用于函数指针
    printf("->%d", n->data);
}

void oddeven(linknode *n){ //打印链表中各元素的奇偶性，用于函数指针
    if(n->data%2==0){
        printf("->even");
    }
    else{
        printf("->odd");
    }
}

int main(){
    void (*func)(linknode *)=oddeven;
    linklist p=initlist();

    listinsert(p, 1, 1);
    printf("list");
    traverselist(p,func);
    printf("len=%d\n",listlength(p));
    printf("\n");

    listinsert(p, 2, 2);
    printf("list");
    traverselist(p,func);
    printf("empty=%d\n",listempty(p));
    printf("\n");

    func=listprint;
    listinsert(p, 1, 3);
    printf("list");
    traverselist(p,func);
    printf("next=%d\n",nextelem(p,p->next)->data);
    printf("\n");

    listdelete(p, 2);
    printf("list");
    traverselist(p,func);
    printf("locate=%d\n",locateelem(p,2)->data);
    printf("\n");

    listinsert(p, 2, 4);
    printf("list");
    traverselist(p,func);
    printf("prior=%d\n",priorelem(p,p->next->next->next)->data);
    printf("\n");

    listinsert(p, 3, 5);
    printf("list");
    traverselist(p,func);
    printf("get=%d\n",getelem(p,3));
    printf("\n");

    clearlist(p);
    printf("list");
    traverselist(p,func);
    printf("empty=%d\n",listempty(p));
    printf("\n");

    destroylist(p);
}