#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <random>

#define len 1000
#define seed 23000020

int *init() //初始化sol，其中sol[i]=j表示第i行的皇后位于第j列
{
    int *sol;
    sol = new int[len];
    if (!sol)
    {
        exit(0);
    }

    for(int i = 0; i < len; i++) //赋初值sol[i]=i，保证各行各列上恰有一个皇后
    {
        sol[i] = i;
    }
    return sol;
}

void shuffle(int *sol) //根据种子进行打乱
{
    std::mt19937 g(seed); 
    std::shuffle(sol, sol + len, g);
    return;
}

void output(int *sol) //输出当前解
{
    for(int i = 0; i < len; i++)
    {
        printf("%d,",sol[i]);
    }
    printf("\n");
}

void output_graph(int *sol) //以图像形式输出当前解，其中O处表示皇后
{
    for(int i = 1; i < len; i++)
    {
        for(int j = 1; j < len; j++)
        {
            if (sol[i] == j)
            {
                printf("O");
            }
            else
            {
                printf("x");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void swap(int *sol, int i, int j) //交换第i行与第j行
{
    int tmp;
    tmp = sol[i];
    sol[i] = sol[j];
    sol[j] = tmp;
}

int evaluate(int *sol) //评价函数，value越小表示越优
{
    int value = 0;
    for(int i = 1; i < len; i++)
    {
        for(int j = i + 1; j < len; j++)
        {
            if (sol[j] - sol[i] == j - i || sol[j] - sol[i] == i - j) //若两个皇后在同一对角线，则value++
            {
                value++;
            }
        }
    }
    return value;
}

int neighbors(int *sol) //遍历所有微小扰动的邻居
{
    int value = evaluate(sol);
    int value_new;
    if (value == 0) //初始状态已经是解
    {
        return 0;
    }

    for(int i = 0; i < len; i++) 
    {
        for(int j = i + 1; j < len; j++)
        {
            swap(sol, i, j); //微小扰动即交换第i行与第j行
            value_new = evaluate(sol);
            if (value_new < value) //如果该邻居的评价函数值更优，则更新
            {
                return value_new;
            }
            swap(sol, i, j); //否则再次交换，还原为原本的sol
        }
    }
    return -1; //未能找到更好的邻居，需要进行restart
}

void restart(int *sol){ //进行restart，即增加扰动范围
	int times = 3;
	for(int i = 0; i < times; i++)
    {
        swap(sol, rand()%len, rand()%len);
    }
    printf("restart!\n");
}

int main()
{
    int *sol = init();
    shuffle(sol);
    printf("value = %d\n", evaluate(sol));

    int flag = -1; //存储neighbors函数的返回值，-1表示需要restart，0表示已找到解
    while(flag != 0)
    {
        flag = neighbors(sol);
        printf("value = %d\n", flag);
        if (flag == -1)
        {
            restart(sol);
        }
    }

    output(sol);
}
