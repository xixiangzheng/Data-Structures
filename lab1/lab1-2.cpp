# include <bits/stdc++.h>
# define maxsize 100

#define DEFINE_STACK(elemtype) \
    typedef struct {/*顺序栈*/\
        elemtype *base;\
        elemtype *top;\
        int stacksize;\
    } stack_##elemtype;\
\
    stack_##elemtype initstack_##elemtype() {/*创建新顺序栈*/\
        stack_##elemtype s;\
        s.base = new elemtype[maxsize];\
        if (!s.base) {/*分配空间不成功*/\
            exit(0);\
        }\
        s.top = s.base;\
        s.stacksize = maxsize;\
        return s;\
    }\
\
    void destroystack_##elemtype(stack_##elemtype &s){ /*销毁栈并释放空间*/\
        delete[] s.base;\
    }\
\
    bool stackempty_##elemtype(stack_##elemtype &s){ /*判断是否为空栈*/\
        return s.base == s.top;\
    }\
\
    int stacklength_##elemtype(stack_##elemtype &s){ /*返回栈的长度*/\
        return s.top - s.base;\
    }\
\
    elemtype *gettop_##elemtype(stack_##elemtype &s){ /*返回栈顶元素*/\
        if (s.base == s.top){/*判断是否为空栈*/\
            exit(0);\
        }\
        return (s.top - 1);\
    }\
\
    void push_##elemtype(stack_##elemtype &s, elemtype e){ /*元素入栈*/\
        if (s.top - s.base >= s.stacksize) {/*栈满防溢出*/\
            exit(0);\
        }\
\
        *s.top = e;\
        s.top += 1;\
    }\
\
    elemtype *pop_##elemtype(stack_##elemtype &s){ /*元素出栈*/\
        if (s.base == s.top){\
            exit(0);\
        }\
        elemtype *e = s.top - 1;/*保存当前栈顶元素的指针*/\
        s.top -= 1;\
        return e;\
    }\
\
    void stacktraverse_##elemtype(stack_##elemtype &s, void (*func)(elemtype)){ /*从栈底至栈顶遍历栈*/\
        for(elemtype *e = s.base; e < s.top; e++) {\
            (*func)(*e);\
        }\
    }

struct map{/*定义结构体*/
    char name;
    int data;
};

DEFINE_STACK(int)
DEFINE_STACK(char)
DEFINE_STACK(map)

void stackprint_int(int e){ /*对int类型，打印栈中各元素的值，用于函数指针*/\
        std::cout << e << "<-";\
    }

void stackprint_char(char e){ /*对char类型，打印栈中各元素的值，用于函数指针*/\
        std::cout << e << "<-";\
    }

void stackprint_map(map e){ /*对map类型，打印栈中各元素的值，用于函数指针*/\
        std::cout << "(" << e.name << "," << e.data << ") <- ";\
    }


int main(){
    void (*func1)(int) = stackprint_int;
    stack_int s = initstack_int();
    std::cout << "empty: " << stackempty_int(s) << std::endl;
    push_int(s, 1);
    push_int(s, 2);
    push_int(s, 3);
    std::cout << "length: " << stacklength_int(s) << std::endl;
    stacktraverse_int(s, func1);
    std::cout << std::endl;
    push_int(s, 4);
    push_int(s, 5);
    std::cout << "pop: " << *pop_int(s) << std::endl;
    std::cout << "gettop: " << *gettop_int(s) << std::endl;
    stacktraverse_int(s, func1);
    destroystack_int(s);
    std::cout << std::endl << std::endl;

    void (*func2)(char) = stackprint_char;
    stack_char t = initstack_char();
    std::cout << "empty: " << stackempty_char(t) << std::endl;
    push_char(t, 'a');
    push_char(t, 'b');
    push_char(t, 'c');
    std::cout << "length: " << stacklength_char(t) << std::endl;
    stacktraverse_char(t, func2);
    std::cout << std::endl;
    push_char(t, 'd');
    push_char(t, 'e');
    std::cout << "pop: " << *pop_char(t) << std::endl;
    std::cout << "gettop: " << *gettop_char(t) << std::endl;
    stacktraverse_char(t, func2);
    destroystack_char(t);
    std::cout << std::endl << std::endl;

    void (*func3)(map) = stackprint_map;
    stack_map r = initstack_map();
    std::cout << "empty: " << stackempty_map(r) << std::endl;
    push_map(r, {'a',1});
    push_map(r, {'b',2});
    push_map(r, {'c',3});
    std::cout << "length: " << stacklength_map(r) << std::endl;
    stacktraverse_map(r, func3);
    std::cout << std::endl;
    push_map(r, {'d',4});
    push_map(r, {'e',5});  
    map e=*pop_map(r); /*注：不能调用两次pop，否则删去两个节点！！！*/
    std::cout << "pop: " << "(" << e.name << "," << e.data << ")" << std::endl;
    std::cout << "gettop: " << "(" << (*gettop_map(r)).name << "," << (*gettop_map(r)).data << ")"<< std::endl;
    stacktraverse_map(r, func3);
    destroystack_map(r);
    return 0;
}