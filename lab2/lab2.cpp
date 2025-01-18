#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define prec_l 400   // 整数位数
#define prec_r 400  // 小数精度
#define prec_o 100   // 小数输出精度
#define polynomial_max 100 // 多项式中单项式最多个数

typedef struct num
{
    int base;   // 基底
    int neg;    // 正0负1
    int data[prec_l + prec_r];
    int *dot;   // 指向小数点前的位置，即个位数
} num;

num* init()
{
    num* temp = (num*)malloc(sizeof(num));
    temp->base = 10;   // 基底初值
    temp->neg = 0;  // 符号赋0
    for (int i = 0; i < prec_l + prec_r; i++)   // data赋0
    {
        temp->data[i]=0;
    }
    temp->dot = temp->data + prec_r; // 初始化小数点指针
    return temp;
}

num* input(char *str, int base_in)
{
    num* temp = init();
    temp->base = base_in;   // 基底初值

    if (str[0] == '-') // 符号初值
    {
        temp->neg = 1;
        str++;  // 防止影响小数点位置计数
    }
    else
    {
        temp->neg = 0;
    }

    int dot_in = 0;
    // 查找小数点在str中位置
    while (str[dot_in] != '.' && str[dot_in] != '\0') // 保护越界
    {
        dot_in++;
    }

    // 整数部分初值
    for (int i = 0; i < dot_in; i++)
    {
        if (str[dot_in - i - 1] >= '0' && str[dot_in - i - 1] <= '9')
        {
            temp->dot[i] = str[dot_in - i - 1] - '0';
        }
        else
        {
            temp->dot[i] = str[dot_in - i - 1] - 'a' + 10;
        }
    }

    // 小数部分初值
    for (int i = 1; str[dot_in + i] != '\0'; i++)
    {
        if (str[dot_in + i] >= '0' && str[dot_in + i] <= '9')
        {
            temp->dot[-i] = str[dot_in + i] - '0';
        }
        else
        {
            temp->dot[-i] = str[dot_in + i] - 'a' + 10;
        }
    }
    return temp;
}

void output(num* A)
{
    if (A->neg == 1)    // 输出负号
    {
        printf("-");
    }

    int i = prec_l -1;
    while (i >= 1 && A->dot[i] == 0)   // 不输出前面多余的0
    {
        i--;
    }

    for (; i >= 0; i--)
    {
        if (A->dot[i] >= 0 && A->dot[i] <= 9)   // 判断是否在0-9
        {
            printf("%d", A->dot[i]);
        }
        else
        {
            printf("%c", A->dot[i] - 10 + 'a');
        }
    }

    printf(".");    // 输出小数点
    for (; i >= -prec_o; i--)
    {
        if (A->dot[i] >= 0 && A->dot[i] <= 9)   // 判断是否在0-9
        {
            printf("%d", A->dot[i]);
        }
        else
        {
            printf("%c", A->dot[i] - 10 + 'a');
        }
    }
    printf("\n");
}

num* conversion(num* A, int base_out)
{
    num *B = init();
    B->base = base_out;
    B->neg = A->neg;    //进制转换不改变符号

    // 整数部分
	for(int i = 0; i < prec_l; i++)
    {
		for(int j = 0; j < prec_l; j++)
        {
            B->dot[j] *= A->base;   //数值上相当于乘以A的进制
        }
		B->dot[0] += A->dot[prec_l - 1 - i];    //从最高位开始，每次循环逐位加入当前末位

		for(int j = 0; j < prec_l - 1; j++) //实现进位，保证B中各位不超过其进制，此步不影响按B进制下值的大小
		{
            if(B->dot[j] >= base_out)
            {
				B->dot[j + 1] += B->dot[j] / base_out;
				B->dot[j] = B->dot[j] % base_out;
			}
        }
	}

    // 小数部分
    int cut[prec_r];   //复制A的小数部分
	for(int i = 1; i < prec_r; i++)
    {
        cut[i] = A->dot[-i];
    }	

	for(int i = 1; i < prec_r; i++)
    {
        cut[0] = 0;
		for(int j = 1; j < prec_r; j++)
        {
            cut[j] *= base_out;   //数值上相当于乘以B的进制
        }

		for(int j = prec_r - 1; j > 0; j--) //实现进位，保证A的小数部分中各位不超过其进制，此步不影响按A进制下值的大小
        {
            cut[j - 1] += cut[j] / A->base;
            cut[j] = cut[j] % A->base;
        } 
		B->dot[-i] = cut[0];   //个位溢出部分即B的相应幂次位置的值
	}
    return B;
}

int abscompare(num * A, num* B) // 比较A与B的绝对值大小
{	
	for(int i = prec_r - 1; i >= prec_l; --i)
    {
        if(A->dot[i] > B->dot[i]){
			return 1;
		}
        else if(A->dot[i] < B->dot[i])
        {
            return -1;
        }
    }	
	return -1;	
}

num *plus(num *A, num* B)
{
    num *temp = init();
    temp->neg = A->neg;  //符号与之保持一致
    temp->base = A->base;
    for(int i = -prec_r; i < prec_l; ++i)
    {
        temp->dot[i] = A->dot[i];
    }	

	// 进位
	for(int i = -prec_r; i < prec_l; ++i){ // 从低位向高位遍历
		temp->dot[i] += B->dot[i];
		if(temp->dot[i] >= temp->base){
			temp->dot[i + 1] ++;
			temp->dot[i] -= temp->base;
		}
	}
	return temp;
}

num * minus(num * A, num* B ,int neg_out)
{	
    num *temp = init();
    temp->neg = neg_out;  //符号与之保持一致
    temp->base = A->base;
    for(int i = -prec_r; i < prec_l; ++i)
    {
        temp->dot[i]=A->dot[i];
    }	
    
	// 借位
	for(int i = -prec_r; i < prec_l; ++i){	// 从低位向高位遍历
        temp->dot[i] -= B->dot[i];
		if(temp->dot[i] < 0){
			temp->dot[i + 1] --;
			temp->dot[i] += temp->base;
		}
	}
	return temp;
}


num * multi(num* A,num* B){	
	num* temp = init();
    if (A->neg == B->neg) // 同号得正，异号为负
    {
        temp->neg = 0;
    }
    else
    {
        temp->neg = 1;
    }
	temp->base = A->base;
	
    // 逐位计算
	for(int i = -prec_r; i < prec_l; ++i)   //求第i位的值
    {
		for(int j = -prec_r; j < prec_l; ++j)
        {
            if(i - j >= -prec_r && i - j < prec_l)	//满足在精度范围内
            {
                temp->dot[i] += A->dot[j] * B->dot[i - j];	//保持下标和不变，类似于多项式乘法
            }
                    
            // 进位
            if(temp->dot[i] >= temp->base){
                temp->dot[i + 1] += temp->dot[i] / temp->base;
                temp->dot[i] = temp->dot[i] % temp->base;
            }
        }	
	}
	return temp;
}

num * judge(num* A, num* B, char c)
{
    if(A->base != B->base) //不同进制运算前需要先转换进制，否则报错
    {
        printf("base error!");
        return NULL;
    }

    if (c=='*')
    {
        return multi(A,B);
    }
    else
    {
        int temp_neg = B->neg;
        if (c=='-') //减法则B变号
        {
            temp_neg = 1 -temp_neg;
        }

        if (A->neg == temp_neg) //同号调用plus
        {
            return plus(A,B);
        }
        else if (A->neg != temp_neg && abscompare(A,B)==1) //异号调用minus，并保证前数更大
        {
            return minus(A,B,A->neg);
        }
        else if (A->neg != temp_neg && abscompare(A,B)!=1) 
        {
            return minus(B,A,temp_neg);
        }
    } 
}

num* polynomial()
{
    double coefficient[polynomial_max]; //单项式系数
    int monomial[polynomial_max]; //单项式次数
    for (int i=0; i < polynomial_max ;i++)  //单项式赋初值0
    {
        coefficient[i] = 0;
        monomial[i] = 0;
    }

    // 多项式初值
    num* A=input("1.4", 10);
    coefficient[0]=3.0/7;
    monomial[0]=3;
    coefficient[1]=-1.0/3;
    monomial[1]=2;
    coefficient[2]=4.0;
    monomial[2]=0;

    num* temp0 = init();
    num* temp1,*temp2;
    char str[20]; //系数未使用高精度
    for (int i = 0;  coefficient[i]!= 0 && i < polynomial_max; i++) //单项式系数为0时终止
    {
        temp1 = input("1.0", 10);
        temp2 = input("1.0", 10);
        for(int j = 0;  j < monomial[i]; j++)
        {
            temp2 = judge(temp2, A, '*');
        }
        sprintf(str, "%.15f", coefficient[i]);
        temp1 = judge(temp2, input(str,10),'*');    //计算得到单项式的值
        output(temp1);
        temp0 = judge(temp1, temp0,'+');
    }
    return temp0;
}

num* polynomial_copy()
{
    double coefficient[polynomial_max]; //单项式系数
    int monomial[polynomial_max]; //单项式次数
    for (int i=0; i < polynomial_max ;i++)  //单项式赋初值0
    {
        coefficient[i] = 0;
        monomial[i] = 0;
    }

    // 多项式初值
    num* A=input("1.612368921371923122414", 10);
    coefficient[0]=12.31379131;
    monomial[0]=2;
    coefficient[1]=1.0;
    monomial[1]=1;
    coefficient[2]=8.2137619836821388;
    monomial[2]=0;

    num* temp0 = init();
    num* temp1,*temp2;
    char str[20]; //系数未使用高精度
    for (int i = 0;  coefficient[i]!= 0 && i < polynomial_max; i++) //单项式系数为0时终止
    {
        temp1 = input("1.0", 10);
        temp2 = input("1.0", 10);
        for(int j = 0;  j < monomial[i]; j++)
        {
            temp2 = judge(temp2, A, '*');
        }
        sprintf(str, "%.15f", coefficient[i]);

        temp1 = judge(temp2, input(str,10),'*');    //计算得到单项式的值
        temp0 = judge(temp1, temp0,'+');
    }
    return temp0;
}

int main()
{
    output(conversion(input("204.2", 10),2));
    output(conversion(input("-11001100.00110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011", 2),10));
    output(conversion(input("114.5141919810", 10),5));
    output(conversion(input("-114.5141919810", 10),19));
    output(judge(input("1.14514",10),input("-1.919810",10),'+'));
    output(judge(input("1.14514",10),input("-1.919810",10),'-'));
    output(judge(input("1.14514",10),input("-1.919810",10),'*'));
    output(conversion(judge(input("1.14514",10),input("-1.919810",10),'+'),7));
    output(conversion(judge(input("1.14514",10),input("-1.919810",10),'-'),7));
    output(conversion(judge(input("1.14514",10),input("-1.919810",10),'*'),7));
    output(polynomial());
    output(polynomial_copy());
}
