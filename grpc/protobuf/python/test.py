import  os
import  sys

import time
import  datetime

def get_grandfather_dir():
    parent = os.path.dirname(os.path.realpath(__file__))
    garder = os.path.dirname(parent)    
    return garder

g_proto_dir = get_grandfather_dir() +"\proto\python"

print(g_proto_dir)
# sys.path.append(g_proto_dir)

from market_data_pb2 import *



def TestPrtPwd():
    print("获取当前文件路径——" + os.path.realpath(__file__)) # 获取当前文件路径
    parent = os.path.dirname(os.path.realpath(__file__))
    print("获取其父目录——" + parent) # 从当前文件路径中获取目录
    garder = os.path.dirname(parent)
    print("获取父目录的父目录——" + garder)
    print("获取文件名" + os.path.basename(os.path.realpath(__file__))) # 获取文件名
    # 当前文件的路径
    pwd = os.getcwd()
    print("当前运行文件路径" + pwd)
    # 当前文件的父路径
    father_path = os.path.abspath(os.path.dirname(pwd) + os.path.sep + ".")
    print("运行文件父路径" + father_path)
    # 当前文件的前两级目录
    grader_father = os.path.abspath(os.path.dirname(pwd) + os.path.sep + "..")
    print("运行文件父路径的父路径" + grader_father)
    return garder


def test_market_data():
    new_depth = Depth()
    price = new_depth.price
    price.value = 10000
    price.precise = 4
    
    new_quote = DepthQuote()
    new_quote.asks.append(new_depth)

    se_string = new_quote.SerializeToString()

    print("se_string: %s" % (se_string))
    
    trans_quote = DepthQuote()
    trans_quote.ParseFromString(se_string)
    print(trans_quote.asks[0].price.value)
    print(trans_quote.asks[0].price.precise)

def get_utc_nano_time():
    ori_time = time.time()
    nano_time = ori_time - int(ori_time)

    utc_time_str = datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')
    time_array = time.strptime(utc_time_str,"%Y-%m-%d %H:%M:%S.%f")
    utc_time_sec_int = time.mktime(time_array)
    utc_time_sec_float = utc_time_sec_int + nano_time
    utc_time_nano = utc_time_sec_float * 1000000000

    print(utc_time_nano)
    print(ori_time*1000000000)


def test_time():
    print(time.time())
    print(datetime.datetime.utcnow())

    utc_time = datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')

    timeArray = time.strptime(utc_time,"%Y-%m-%d %H:%M:%S.%f")
    print(timeArray)
    # 转为时间戳
    timeStamp = time.mktime(timeArray)
    print(timeStamp)

if __name__ == "__main__":
    # TestPrtPwd()
    # test_market_data()
    # test_time()

    get_utc_nano_time()