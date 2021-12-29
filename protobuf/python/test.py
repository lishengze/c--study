import  os
import  sys
# sys.path.append(os.getcwd()+"/proto/python")
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
    price = new_depth.price.add()
    price.value = 10000
    price.precise = 4
    
    

if __name__ == "__main__":
    # TestPrtPwd()
    test_market_data()