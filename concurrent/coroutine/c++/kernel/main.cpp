#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>

void test_get_set_context()
{
    int i = 0;
	ucontext_t ctx, ctx_main;

    getcontext(&ctx_main);
    printf("After get main!\n");
	getcontext(&ctx);   // 在该位置保存上下文


    ctx.uc_link = &ctx_main;    // 无用：
	printf("i = %d\n", i++);
	sleep(2);
	// setcontext(&ctx);   //将上下文恢复至设置时的状态，完成死循环	
    swapcontext(&ctx, &ctx_main);
}

void fun( void ) 
{
	printf("fun()\n");
}

void test_make_context()
{
	int i = 1;
	char *stack = (char*)malloc(sizeof(char)*8192);
	ucontext_t ctx_main, ctx_fun;

	getcontext(&ctx_main);//保存ctx_main上下文
    printf("main get context over!\n");
	getcontext(&ctx_fun);//保存ctx_fun上下文
    printf("fun get context over!\n");

	printf("i=%d\n", i++);
	sleep(1);

	//设置上下文的栈信息
	ctx_fun.uc_stack.ss_sp    = stack;
	ctx_fun.uc_stack.ss_size  = 8192;
	ctx_fun.uc_stack.ss_flags = 0;

	ctx_fun.uc_link = &ctx_main;        // 设置ctx_main为ctx_fun的后继上下文
	makecontext(&ctx_fun, fun, 0);      // 修改上下文信息，设置入口函数与参数	

    printf("make over \n");

	setcontext(&ctx_fun);               // 恢复ctx_fun上下文
    // swapcontext(&ctx_fun, &ctx_main);
	
	printf("main exit\n");
}

int main() 
{
	// test_get_set_context();

    test_make_context();

	return 0;
}
