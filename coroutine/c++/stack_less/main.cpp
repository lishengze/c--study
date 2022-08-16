#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>

int function_l(void) {
    static int i, state = 0;
    switch (state) {
        case 0: goto LABEL0;
        case 1: goto LABEL1;
    }
    LABEL0: // 函数开始执行
    for (i = 0; i < 10; i++) {
        state = 1; // 我们会跳转到 LABEL1
        return i;
        LABEL1:; // 从上一次返回之后的地方开始执行
    }
}

int function_d(void) {
    static int i, state = 0;
	printf("\ni = %d, state=%d \n", i, state);

    switch (state) {
        case 0: // 函数开始执行
        for (i = 0; i < 10; i++) {
            state = 1; // 我们会回到 "case 1" 的地方
            return i;
            case 1:{
				printf("case 1, state: %d\n", state);
			}; // 从上一次返回之后的地方开始执行
        }
    }
}

int test_base_func() {
	printf("Test Base Func");

	for (int i = 0; i < 10; ++i) {
		// printf("i = %d, function_l = %d,  function_d = %d, \n", i, function_l(), function_d());
		printf("i = %d, function_d = %d, \n", i, function_d());
	}
}


int main() 
{
	// test_get_set_context();

    test_base_func();

	return 0;
}
