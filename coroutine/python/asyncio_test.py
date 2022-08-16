import asyncio
import time
import datetime
import time
import os

def get_datetime_str():
    return datetime.datetime.now().strftime('%Y-%m-%d_%H:%M:%S')

async def work1(count = 5):
    print("work1 start %d\n" % count)
    for i in range(count):
        print("work1: " + get_datetime_str())
        await asyncio.sleep(5)
    return ("work1 %d finished " % count)


async def work2(count = 6):
    print("work2 start %d \n" % count)
    for i in range(count):
        print("work2: " + get_datetime_str())
        await asyncio.sleep(3)
    return ("work2 %d finished " % count)

def test_asyncio():
    all_task = asyncio.gather(work1(), work2())
    loop = asyncio.get_event_loop()
    loop.run_until_complete(all_task)

async  def work_main():
    result = await asyncio.gather(work1(),work2())
    print(result)

def test_asyncio_run():
    asyncio.run(work_main())
        
if __name__ == "__main__":
    # test_asyncio()
    test_asyncio_run()