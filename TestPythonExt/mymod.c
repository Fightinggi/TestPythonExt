#include "Python.h"
#include "pch.h"
//#include <iostream>
//PyArg_ParseTuple(args, "s", &str); 解析python调用模块函数时，传进来得参数,赋给str

//1. 扩展库入口函数 
//	a.PyMODINIT_FUNC PyInit_mymod(void) { return PyModule_Create(&mymod_module); }
//	b.mymod 模块名
//	c.PyInit_ 固定开头
//2. 模块创建
//	a.参数 PyModule_Create()
//3. 定义模块
//	a.PyModuleDef mymod_module = { PyModuleDef_HEAD_INIT，"模块名"，"模块说明"，模块空间，模块函数列表 }
//4. 定义模块函数列表
//	a.PyMethodDef mymod_funcs[] = { {"函数名称"，函数指针，参数标识，"函数说明"}, {0,0,0,0} }
//5. 定义模块函数
//	a.PyObject * testmod(PyObject * self) { ; }

//封装，参数，谁传进来谁清理
void SetDictLong(PyObject* dic, const char* key, const int* val)
{
	PyObject* k = PyUnicode_FromString(key);//引用计数+1
	PyObject* v = PyLong_FromLong(val);//引用计数+1
	PyDict_SetItem(dic, k, v);//将k和v 引用计数+1
	Py_XDECREF(k);//引用计数-1
	Py_XDECREF(v);//引用计数-1
}
void SetDictString(PyObject* dic, const char *key, const char *val)
{
	PyObject* k = PyUnicode_FromString(key);//引用计数+1
	PyObject* v = PyUnicode_FromString(val);//引用计数+1
	PyDict_SetItem(dic, k, v);//将k和v 引用计数+1
	Py_XDECREF(k);//引用计数-1
	Py_XDECREF(v);//引用计数-1
}
//封装PyObject打印函数
void printobj(PyObject* obj)
{
	wprintf(L" \n ");
	wchar_t buf[1024] = { 0 };  //申请空间
	if (PyUnicode_Check(obj))//类型检查
	{
		PyUnicode_AsWideChar(obj, buf, sizeof(buf));
		wprintf(L" {%s} ", buf);
	}
	else if (PyLong_Check(obj))
	{
		wprintf(L" {%d} ", PyLong_AsLong(obj));
	}
	else
	{
		wprintf(L" UnKnown ");
	}
}
// 定义模块函数获取并分析Dict
//PyUnicode_FromString； PyDict_Keys； PyDict_Values；PyDict_SetItem 
//引用计数+1  使用Py_XDECREF(key);引用计数-1，释放
static PyObject* test_dict(PyObject* self, PyObject* args)
{
	//获取dict对象
	PyObject* dic = NULL;
	if (!PyArg_ParseTuple(args, "O", &dic))  //大写的O
	{
		return NULL;
	}
	if (!PyDict_Check(dic))
	{
		//检查异常信息
		PyErr_SetString(PyExc_Exception, "arg must be dict");
		return NULL; //抛出异常
	}
	//判断是否有此关键字
	PyObject* key = PyUnicode_FromString("name"); //引用计数+1
	//if (!PyDict_Contains(dic, key)) // 无空间操作，不释放会内存泄漏
	int re = PyDict_Contains(dic, key); 
	Py_XDECREF(key);//引用计数-1，释放
	if(!re)
	{
		PyErr_SetString(PyExc_Exception, "name must be set");
		return NULL; //抛出异常
	}

	//=======读取关键字的值  {}括起来的值，作用在{}里==============
	{
		PyObject* key = PyUnicode_FromString("name");//引用计数+1  Create a Unicode object from a UTF-8 encoded null-terminated char buffer u.
		PyObject* val = PyDict_GetItem(dic, key);//只做引用  Return value: Borrowed reference.
		wchar_t buf[1024] = { 0 };  //申请空间
		PyUnicode_AsWideChar(val, buf, sizeof(buf));
		wprintf(L"PyDict_GetItem = %s\n", buf);
		Py_XDECREF(key);//引用计数-1，释放

		//方式2
		val = PyDict_GetItemString(dic, "name");
		PyUnicode_AsWideChar(val, buf, sizeof(buf));
		wprintf(L"PyDict_GetItem = %s\n", buf);
	}

	{
		//=============遍历 key 和 value===============
		//返回所有关键字keys，PyList类型，新增空间
		PyObject* keys = PyDict_Keys(dic);//引用计数+1  Return value: New reference.
		PyObject* vals = PyDict_Values(dic);//引用计数+1  Return value: New reference.
		int size = PyList_Size(keys);
		wchar_t buf[1024] = { 0 };  //申请空间
		for (int i = 0; i < size; i++)
		{
			//取keys
			PyObject* key = PyList_GetItem(keys, i);
			PyUnicode_AsWideChar(key, buf, sizeof(buf));
			wprintf(L"{ %s: ", buf);  //[ name ][ age ][ score ]

			//取values
			PyObject* val = PyList_GetItem(vals, i);
			if (PyUnicode_Check(val))//类型检查
			{
				PyUnicode_AsWideChar(val, buf, sizeof(buf));
				wprintf(L" %s} ", buf);  
			}
			else if (PyLong_Check(val))
			{
				wprintf(L" %d} ", PyLong_AsLong(val));
			}
			else
			{
				wprintf(L" UnKnown ");
			}
		}
		//空间申请和释放结对出现
		Py_XDECREF(keys);//引用计数-1
		Py_XDECREF(vals);//引用计数-1
	}

	{
		//===============迭代器来遍历dict=================
		/*PyAPI_FUNC(int) PyDict_Next(
			PyObject * mp, Py_ssize_t * pos, PyObject * *key, PyObject * *value);*/
		Py_ssize_t pos = 0;
		PyObject* key = 0;
		PyObject* value = 0;
		while (PyDict_Next(dic, &pos, &key, &value))  // int pos=0; int *pos = &pos;
		{	//没有引用+1，内部直接更改指向
			printobj(key);
			printobj(value);
		}
	}

	{
		//==============新增修改字典, 修改==覆盖================
		PyObject* k = PyUnicode_FromString("name");//引用计数+1
		PyObject* v = PyUnicode_FromString("changed_dict");//引用计数+1
		PyDict_SetItem(dic,k,v);//将k和v 引用计数+1
		Py_XDECREF(k);//引用计数-1
		Py_XDECREF(v);//引用计数-1

		SetDictString(dic, "path", "C:\windows");

	}//销毁的时候，将k和v 引用计数-1
	{
		//删除字典
		PyObject* k = PyUnicode_FromString("delkey");
		if (PyDict_Contains(dic,k))
		{
			PyDict_DelItem(dic, k);
		}
		Py_XDECREF(k);
	}

	{
		//===================新建返回值=====================
		PyObject* redic = PyDict_New();

		SetDictString(redic, "das", "das");
		SetDictLong(redic, "d1s", 1);
		SetDictLong(redic, "d22", 42);

		return redic;
	}
	return PyLong_FromLong(0);
};


// 定义模块函数获取并分析list
static PyObject* test_list(PyObject* self, PyObject* args)
{
	//获取list对象
	PyObject* lis1 = NULL;
	if (!PyArg_ParseTuple(args, "O", &lis1))  //大写的O
	{
		return NULL;
	}
	if (!PyList_Check(lis1))
	{
		//检查异常信息
		PyErr_SetString(PyExc_Exception,"arg must be list");
		return NULL;
	}
	Py_ssize_t size = PyList_Size(lis1);
	for (int i = 0; i < size; i++)
	{
		PyObject* val = PyList_GetItem(lis1, i);
		int re = PyLong_AsLong(val);
		printf("%d ", re);
	}
	printf("\n");
	//修改参数list
	//结尾处新增
	PyList_Append(lis1,PyLong_FromLong(11));
	//插入
	PyList_Insert(lis1,4,PyFloat_FromDouble(3.1));
	//修改
	PyList_SetItem(lis1, 2, PyLong_FromLong(20));
	//删除  [1,2)  区间的设置为0，即删除
	PyList_SetSlice(lis1,1,2,0);

	//返回新创建的列表，空间交给python释放
	/*PyObject* relisr = PyList_New(0);
	PyList_Append(relisr, PyLong_FromLong(22));
	PyList_Append(relisr, PyLong_FromLong(33));
	PyList_Insert(relisr, 4, PyFloat_FromDouble(3.1));

	return relisr;*/

	//返回lis1
	Py_XINCREF(lis1);
	return lis1;
};



// 定义模块函数获取并分析元组tuple
static PyObject* test_tuple(PyObject* self, PyObject* args)
{
	//获取元组对象
	PyObject* tup = NULL;
	if (!PyArg_ParseTuple(args, "O", &tup))  //大写的O
	{
		return NULL;
	}
	//元组大小
	Py_ssize_t size = PyTuple_Size(tup);
	for (int i = 0; i < size; i++)
	{
		//元组的元素  
		//define PyAPI_FUNC(RTYPE) __declspec(dllimport) RTYPE
		//PyAPI_FUNC (PyObject *) PyTuple_GetItem(PyObject *, Py_ssize_t);
		PyObject* val = PyTuple_GetItem(tup, i);

		if (PyTuple_Check(val))//如果元组元素val还是一个元组，则继续遍历
		{
			Py_ssize_t val_size = PyTuple_Size(val);
			printf("( ");
			for (int j = 0; j < val_size; j++)
			{
				PyObject* v = PyTuple_GetItem(val, j);
				int re = PyLong_AsLong(v);
				printf("%d ", re);
			}
			printf(") ");
			continue;
		}

		printf("%d ", PyLong_AsLong(val));
	}
	printf("\n");
	//Py_RETURN_NONE;

	//返回元组
	PyObject* return_tup = PyTuple_New(10);
	PyTuple_SetItem(return_tup, 0, PyLong_FromLong(102));  //插入元素int
	PyTuple_SetItem(return_tup, 1, PyUnicode_FromString("test tuple return string "));

	PyObject* tup2 = PyTuple_New(3);
	PyTuple_SetItem(tup2, 0, PyLong_FromLong(122));  //插入元素

	PyTuple_SetItem(return_tup, 3, tup2);  //插入元素tup2
	return return_tup;
};

// 定义模块函数PyArg_ParseTupleAndKeywords
static PyObject* test_kw(PyObject* self, PyObject* args, PyObject* kw)
{
	//定义value，接python传来的值，可以先设置默认值
	const char* name = "";
	int age = 10;
	int score = 20;
	//定义key键，对应python中的键
	static char* keys[] = { "name", "age", "score",NULL };
	//解析参数args，kw
	if (!PyArg_ParseTupleAndKeywords(args, kw, "s|ii", keys, &name, &age, &score))
	{
		return NULL;  //#define NULL ((void *)0)
	}
	printf("Name is %s age is %d score is %d\n", name, age, score);
	Py_RETURN_NONE;//设定的宏 #define Py_RETURN_NONE return Py_INCREF(Py_None), Py_None
};

// 定义模块函数PyArg_ParseTuple  PyUnicode_FromString
static PyObject* teststring(PyObject* self, PyObject* args)
{
	char* str = 0;
	if (!PyArg_ParseTuple(args, "s", &str)) {
		return NULL;
	}
	//传递过来的字符串是utf - 8格式，控制台默认GBK，中文乱码
	printf("PyArg_ParseTuple str= %s  \n", str);

	/*/ 如果有中文，要把代码设置为utf - 8格式
		/ 分配空间，引用计数 + 1，交给py thon解释器释放*/
	return 	PyLong_FromLong(0);
};

// 定义模块函数PyFloat_AsDouble
static PyObject* testfloat(PyObject* self, PyObject* arg)
{
	printf("testfloat\n");
	if (PyFloat_Check(arg))
	{
		double a = PyFloat_AsDouble(arg);
		printf("args type is float = %f\n",a);
	}
	else
	{
		printf("args type is not float\n");
	}

	return 	PyLong_FromLong(0);
};


//5. 定义模块函数PyArg_ParseTuple  
static PyObject* testmod(PyObject* self,PyObject *args)
{
	int x = 0;
	int y = 0;
	float z = 0;
	if (!PyArg_ParseTuple(args,"iif",&x,&y,&z))
	{
		printf("PyArg_ParseTuple");
		return NULL;//会抛出异常
	}
	printf("PyArg_ParseTuple x= %d y= %d z= %f \n",x,y,z);
	// 返回python的Long整形，c语言中引用计数+1，返回值由python释放
	return 	PyLong_FromLong(105);
};


//4. 定义模块函数列表
static PyMethodDef mymod_funcs[] = {
	{
		"testmod",  //"函数名称"，
		testmod, //函数指针，
		METH_VARARGS,//参数标识，无参METH_NOARGS,METH_VARARGS  
		"testmod function."  //"函数说明"
	},
	{
		"testfloat",  //"函数名称"，
		testfloat, //函数指针，
		METH_O,//参数标识，无参METH_NOARGS,METH_VARARGS  
		"testfloat function."  //"函数说明"
	},
	{
		"teststring",  //"函数名称"，
		teststring, //函数指针，
		METH_VARARGS,//参数标识，无参METH_NOARGS,METH_VARARGS  
		"teststring function."  //"函数说明"
	},
	{
		"test_kw",  //"函数名称"，
		test_kw, //函数指针，
		METH_VARARGS | METH_KEYWORDS,//参数标识，无参METH_NOARGS,METH_VARARGS  
		"test_kw function."  //"函数说明"
	},
	{
		"test_tuple",  //"函数名称"，
		test_tuple, //函数指针，
		METH_VARARGS,//参数标识，无参METH_NOARGS,METH_VARARGS  
		"test_tuple function."  //"函数说明"
	},
	{
		"test_list",  //"函数名称"，
		test_list, //函数指针，
		METH_VARARGS,//参数标识，无参METH_NOARGS,METH_VARARGS  
		"test_list function."  //"函数说明"
	},
	{
		"test_dict",  //"函数名称"，
		test_dict, //函数指针，
		METH_VARARGS,//参数标识，无参METH_NOARGS,METH_VARARGS  
		"test_dict function."  //"函数说明"
	},
	
	{0,0,0,0}
};

//3. 定义模块
static PyModuleDef mymod_module = {
	PyModuleDef_HEAD_INIT,
	"TestPythonExt",                           //"模块名"，
	"TestPythonExt is first module test",      //"模块说明",    help(模块名)
	-1,                               //模块空间，
	mymod_funcs,                    //模块函数列表 
};

//1. 扩展库入口函数  PyMODINIT_FUNC宏 extern c __declspec(dllexport)
PyMODINIT_FUNC PyInit_TestPythonExt(void)  // define PyMODINIT_FUNC __declspec(dllexport) PyObject*
{
	printf("PyInit_TestPythonExt\n");
	//2. 模块创建
	return PyModule_Create(&mymod_module);

}