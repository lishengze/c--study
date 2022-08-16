import time
import datetime
import time
import os

def get_datetime_str():
    return datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')

def task_1():
    print("task_1 start!")
    while True:
        print("--This is task 1!--before " + get_datetime_str())
        yield
        print("--This is task 1!--after " + get_datetime_str())
        time.sleep(5)

def task_2():
    print("task_2 start!")
    while True:
        print("--This is task 2!--before " + get_datetime_str())
        yield
        print("--This is task 2!--after " + get_datetime_str())
        time.sleep(1)

def test_2_v():
    t1 = task_1()  # 生成器对象
    t2 = task_2()

    # next(t1)  # 1、唤醒生成器t1，执行到yield后，保存上下文，挂起任务；下次再次唤醒之后，从yield继续往下执行
    # print("\nThe main thread!\n")  # 2、继续往下执行
    # next(t2)  # 3、唤醒生成器t2，....    

    # print(t1, t2)

    while True:
        next(t1)  # 1、唤醒生成器t1，执行到yield后，保存上下文，挂起任务；下次再次唤醒之后，从yield继续往下执行
        print("\nThe main thread!\n")  # 2、继续往下执行
        next(t2)  # 3、唤醒生成器t2，....

        
if __name__ == "__main__":
    test_2_v()