# coding:utf-8
print("Test TestPythonExt Module ")
print("testmymod.py IN F:\WIN10SDK\py_lesson\src\TestPythonExt\TestPythonExt ")
import TestPythonExt

print(help(TestPythonExt))
try:
    print(TestPythonExt.testmod(999, 111, 22.33))

    TestPythonExt.testfloat("xx")
    TestPythonExt.testfloat(11.22)

    a = TestPythonExt.teststring("python to c++")
    print("teststring return type is ", type(a))
    print("teststring return  is = ", a)

    TestPythonExt.test_kw(name="XiaoMing2")

    tup = (1, 2, 3, 4, 5, 9, (7, 6, 5), 3)
    res = TestPythonExt.test_tuple(tup)
    print("=========C++ return Tuple to Python=============")
    print(res)

    print("=========C++ return list to Python=============")
    try:
        TestPythonExt.test_list(111)  # err 异常
    except Exception as e:
        print("list Exception", type(e), e)

    list1 = [0, 1, 2, 3, 4, 5, 6, 8, 9, 10]
    print("\nbegin list:", list1)
    re = TestPythonExt.test_list(list1)
    print("\nend list:", list1)
    print("\n接收c++扩展库修改后返回的list列表", re)

    dic = {"name": "test_dict", "age": 124, "score": 12, "delkey": "delstring"}
    re = TestPythonExt.test_dict(dic)

    print("\n=======changed===========")
    print(dic)

    print("\n=======re dict===========")
    print(re)


except Exception as e:
    print(" Exception", type(e), e)

input()