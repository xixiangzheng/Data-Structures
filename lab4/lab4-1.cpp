#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 200

typedef struct strtype{     //定长顺序结构存储
    char ch[MAXLEN];
    int len;
}strtype;

strtype *init(){    //初始化字符串
    strtype *str = (strtype*) malloc(sizeof(struct strtype));
    str->len = 0;
    return str;
}

int length(strtype *str){   //返回串长度
    return str->len;
}

void clear(strtype *str){   //清空串
    str->len = 0;
}

void destroy(strtype *str){     //销毁串
    free(str);
}

void print(strtype *str){       //遍历并打印串
    for(int i = 0; i < str->len; i++)
    {
        printf("%c", str->ch[i]);
    }
    printf("\n");
}

void insert(strtype *str, int pos, char input){     //在串的指定位置插入一个字符
    for(int i = str->len; i > pos; i--)     //指定位置后的元素依次向后平移一个单位
    {
        str->ch[i] = str->ch[i - 1];
    }
    str->ch[pos] = input;       //插入元素，并更新串长度
    str->len = str->len + 1;
}

void assign(strtype *str, char input[]){        //串赋值
    int strlen = 0;
    while(input[strlen] != '\0'){   //检测读入字符串的长度
        strlen++;
    }
    str->len = 0;
    for(int i = 0; i < strlen; i++)
    {
        insert(str, i, input[i]);   //逐个将字符串中的字符插入串中
    }
}

void concat(strtype *str1, strtype *str2){      //将str2拼接在str1后
    int len_tmp = str1->len;    //存储str1的原长
    for(int i = 0; i < str2->len; i++)
    {
        insert(str1, i + len_tmp, str2->ch[i]);
    }
}

strtype *substring(strtype *str, int subpos, int sublen){   //取出从suppos开始，长度为sublen的子串
    strtype *substr = init();
    for(int i = 0; i < sublen; i++)
    {
        insert(substr, i, str->ch[subpos + i]);
    }
    return substr;
}

void KMP_next(strtype *pat, int next[]){    //KMP算法，计算next
    int i = 0;
    int j = -1;
    next[0] = -1;
    while(i < pat->len){
        if(j == -1 || pat->ch[i] == pat->ch[j]){
            i++;
            j++;
            next[i] = j;
        }
        else{
            j = next[j];
        }
    }
}

int KMP(strtype *str, strtype *pat){    //KMP算法
    int i = 0;
    int j = 0;
    int next[MAXLEN];
    KMP_next(pat, next);

    while(i < str->len && j < pat->len){        //i指向主串，j指向模式串
        if (j == -1 || str->ch[i] == pat->ch[j]){
            i++;
            j++;
        }
        else{
            j = next[j];
        }
    }

    if (j >= pat->len)
    {
        return i - pat->len;    //返回模式串首次出现的位置
    }
    else{
        return -1;      //未找到主串中的模式串
    }
}

strtype *replace(strtype *str, strtype *pat, strtype *newstr){
    strtype *tmp = substring(str, 0, str->len);     //用tmp复制原本的str
    str = init();       //清空str
    int pos = KMP(tmp, pat);    //用pos记录模式串首次出现的位置
    while(pos != -1)
    {
        concat(str, substring(tmp, 0, pos));    //将模式串出现之前的部分拼接进str
        concat(str, newstr);        //将模式串替换为新串，并拼接进str
        tmp = substring(tmp, pos + pat->len, tmp->len - (pos + pat->len));      //从tmp中删去已被拼接进str的部分
        pos = KMP(tmp, pat);    //更新pos，找到模式串下一次出现的位置
    }
    concat(str, tmp);   //将tmp中多余的部分也拼接进str
    destroy(tmp);
    return str;
}



int main(){
    strtype* str1 = init();
    strtype* str2 = init();
    strtype* str3 = init();
    char chars1[50] = "hello world";
    char chars2[50] = "goodbyebye byebyebye by ";
    char chars3[50] = "byebye";
    assign(str1, chars1);
    print(str1);
    str1 = substring(str1, 6,5);
    print(str1);
    assign(str2, chars2);
    assign(str3, chars3);
    concat(str2, str1);
    print(str2);
    printf("%d\n", KMP(str2, str3));
    str2 = replace(str2, str3, str1);
    print(str2);
    clear(str3);
    printf("%d\n", length(str3));
}