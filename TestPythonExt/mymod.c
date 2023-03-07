#include "Python.h"
#include "pch.h"
//#include <iostream>
//PyArg_ParseTuple(args, "s", &str); ����python����ģ�麯��ʱ���������ò���,����str

//1. ��չ����ں��� 
//	a.PyMODINIT_FUNC PyInit_mymod(void) { return PyModule_Create(&mymod_module); }
//	b.mymod ģ����
//	c.PyInit_ �̶���ͷ
//2. ģ�鴴��
//	a.���� PyModule_Create()
//3. ����ģ��
//	a.PyModuleDef mymod_module = { PyModuleDef_HEAD_INIT��"ģ����"��"ģ��˵��"��ģ��ռ䣬ģ�麯���б� }
//4. ����ģ�麯���б�
//	a.PyMethodDef mymod_funcs[] = { {"��������"������ָ�룬������ʶ��"����˵��"}, {0,0,0,0} }
//5. ����ģ�麯��
//	a.PyObject * testmod(PyObject * self) { ; }

//��װ��������˭������˭����
void SetDictLong(PyObject* dic, const char* key, const int* val)
{
	PyObject* k = PyUnicode_FromString(key);//���ü���+1
	PyObject* v = PyLong_FromLong(val);//���ü���+1
	PyDict_SetItem(dic, k, v);//��k��v ���ü���+1
	Py_XDECREF(k);//���ü���-1
	Py_XDECREF(v);//���ü���-1
}
void SetDictString(PyObject* dic, const char *key, const char *val)
{
	PyObject* k = PyUnicode_FromString(key);//���ü���+1
	PyObject* v = PyUnicode_FromString(val);//���ü���+1
	PyDict_SetItem(dic, k, v);//��k��v ���ü���+1
	Py_XDECREF(k);//���ü���-1
	Py_XDECREF(v);//���ü���-1
}
//��װPyObject��ӡ����
void printobj(PyObject* obj)
{
	wprintf(L" \n ");
	wchar_t buf[1024] = { 0 };  //����ռ�
	if (PyUnicode_Check(obj))//���ͼ��
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
// ����ģ�麯����ȡ������Dict
//PyUnicode_FromString�� PyDict_Keys�� PyDict_Values��PyDict_SetItem 
//���ü���+1  ʹ��Py_XDECREF(key);���ü���-1���ͷ�
static PyObject* test_dict(PyObject* self, PyObject* args)
{
	//��ȡdict����
	PyObject* dic = NULL;
	if (!PyArg_ParseTuple(args, "O", &dic))  //��д��O
	{
		return NULL;
	}
	if (!PyDict_Check(dic))
	{
		//����쳣��Ϣ
		PyErr_SetString(PyExc_Exception, "arg must be dict");
		return NULL; //�׳��쳣
	}
	//�ж��Ƿ��д˹ؼ���
	PyObject* key = PyUnicode_FromString("name"); //���ü���+1
	//if (!PyDict_Contains(dic, key)) // �޿ռ���������ͷŻ��ڴ�й©
	int re = PyDict_Contains(dic, key); 
	Py_XDECREF(key);//���ü���-1���ͷ�
	if(!re)
	{
		PyErr_SetString(PyExc_Exception, "name must be set");
		return NULL; //�׳��쳣
	}

	//=======��ȡ�ؼ��ֵ�ֵ  {}��������ֵ��������{}��==============
	{
		PyObject* key = PyUnicode_FromString("name");//���ü���+1  Create a Unicode object from a UTF-8 encoded null-terminated char buffer u.
		PyObject* val = PyDict_GetItem(dic, key);//ֻ������  Return value: Borrowed reference.
		wchar_t buf[1024] = { 0 };  //����ռ�
		PyUnicode_AsWideChar(val, buf, sizeof(buf));
		wprintf(L"PyDict_GetItem = %s\n", buf);
		Py_XDECREF(key);//���ü���-1���ͷ�

		//��ʽ2
		val = PyDict_GetItemString(dic, "name");
		PyUnicode_AsWideChar(val, buf, sizeof(buf));
		wprintf(L"PyDict_GetItem = %s\n", buf);
	}

	{
		//=============���� key �� value===============
		//�������йؼ���keys��PyList���ͣ������ռ�
		PyObject* keys = PyDict_Keys(dic);//���ü���+1  Return value: New reference.
		PyObject* vals = PyDict_Values(dic);//���ü���+1  Return value: New reference.
		int size = PyList_Size(keys);
		wchar_t buf[1024] = { 0 };  //����ռ�
		for (int i = 0; i < size; i++)
		{
			//ȡkeys
			PyObject* key = PyList_GetItem(keys, i);
			PyUnicode_AsWideChar(key, buf, sizeof(buf));
			wprintf(L"{ %s: ", buf);  //[ name ][ age ][ score ]

			//ȡvalues
			PyObject* val = PyList_GetItem(vals, i);
			if (PyUnicode_Check(val))//���ͼ��
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
		//�ռ�������ͷŽ�Գ���
		Py_XDECREF(keys);//���ü���-1
		Py_XDECREF(vals);//���ü���-1
	}

	{
		//===============������������dict=================
		/*PyAPI_FUNC(int) PyDict_Next(
			PyObject * mp, Py_ssize_t * pos, PyObject * *key, PyObject * *value);*/
		Py_ssize_t pos = 0;
		PyObject* key = 0;
		PyObject* value = 0;
		while (PyDict_Next(dic, &pos, &key, &value))  // int pos=0; int *pos = &pos;
		{	//û������+1���ڲ�ֱ�Ӹ���ָ��
			printobj(key);
			printobj(value);
		}
	}

	{
		//==============�����޸��ֵ�, �޸�==����================
		PyObject* k = PyUnicode_FromString("name");//���ü���+1
		PyObject* v = PyUnicode_FromString("changed_dict");//���ü���+1
		PyDict_SetItem(dic,k,v);//��k��v ���ü���+1
		Py_XDECREF(k);//���ü���-1
		Py_XDECREF(v);//���ü���-1

		SetDictString(dic, "path", "C:\windows");

	}//���ٵ�ʱ�򣬽�k��v ���ü���-1
	{
		//ɾ���ֵ�
		PyObject* k = PyUnicode_FromString("delkey");
		if (PyDict_Contains(dic,k))
		{
			PyDict_DelItem(dic, k);
		}
		Py_XDECREF(k);
	}

	{
		//===================�½�����ֵ=====================
		PyObject* redic = PyDict_New();

		SetDictString(redic, "das", "das");
		SetDictLong(redic, "d1s", 1);
		SetDictLong(redic, "d22", 42);

		return redic;
	}
	return PyLong_FromLong(0);
};


// ����ģ�麯����ȡ������list
static PyObject* test_list(PyObject* self, PyObject* args)
{
	//��ȡlist����
	PyObject* lis1 = NULL;
	if (!PyArg_ParseTuple(args, "O", &lis1))  //��д��O
	{
		return NULL;
	}
	if (!PyList_Check(lis1))
	{
		//����쳣��Ϣ
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
	//�޸Ĳ���list
	//��β������
	PyList_Append(lis1,PyLong_FromLong(11));
	//����
	PyList_Insert(lis1,4,PyFloat_FromDouble(3.1));
	//�޸�
	PyList_SetItem(lis1, 2, PyLong_FromLong(20));
	//ɾ��  [1,2)  ���������Ϊ0����ɾ��
	PyList_SetSlice(lis1,1,2,0);

	//�����´������б��ռ佻��python�ͷ�
	/*PyObject* relisr = PyList_New(0);
	PyList_Append(relisr, PyLong_FromLong(22));
	PyList_Append(relisr, PyLong_FromLong(33));
	PyList_Insert(relisr, 4, PyFloat_FromDouble(3.1));

	return relisr;*/

	//����lis1
	Py_XINCREF(lis1);
	return lis1;
};



// ����ģ�麯����ȡ������Ԫ��tuple
static PyObject* test_tuple(PyObject* self, PyObject* args)
{
	//��ȡԪ�����
	PyObject* tup = NULL;
	if (!PyArg_ParseTuple(args, "O", &tup))  //��д��O
	{
		return NULL;
	}
	//Ԫ���С
	Py_ssize_t size = PyTuple_Size(tup);
	for (int i = 0; i < size; i++)
	{
		//Ԫ���Ԫ��  
		//define PyAPI_FUNC(RTYPE) __declspec(dllimport) RTYPE
		//PyAPI_FUNC (PyObject *) PyTuple_GetItem(PyObject *, Py_ssize_t);
		PyObject* val = PyTuple_GetItem(tup, i);

		if (PyTuple_Check(val))//���Ԫ��Ԫ��val����һ��Ԫ�飬���������
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

	//����Ԫ��
	PyObject* return_tup = PyTuple_New(10);
	PyTuple_SetItem(return_tup, 0, PyLong_FromLong(102));  //����Ԫ��int
	PyTuple_SetItem(return_tup, 1, PyUnicode_FromString("test tuple return string "));

	PyObject* tup2 = PyTuple_New(3);
	PyTuple_SetItem(tup2, 0, PyLong_FromLong(122));  //����Ԫ��

	PyTuple_SetItem(return_tup, 3, tup2);  //����Ԫ��tup2
	return return_tup;
};

// ����ģ�麯��PyArg_ParseTupleAndKeywords
static PyObject* test_kw(PyObject* self, PyObject* args, PyObject* kw)
{
	//����value����python������ֵ������������Ĭ��ֵ
	const char* name = "";
	int age = 10;
	int score = 20;
	//����key������Ӧpython�еļ�
	static char* keys[] = { "name", "age", "score",NULL };
	//��������args��kw
	if (!PyArg_ParseTupleAndKeywords(args, kw, "s|ii", keys, &name, &age, &score))
	{
		return NULL;  //#define NULL ((void *)0)
	}
	printf("Name is %s age is %d score is %d\n", name, age, score);
	Py_RETURN_NONE;//�趨�ĺ� #define Py_RETURN_NONE return Py_INCREF(Py_None), Py_None
};

// ����ģ�麯��PyArg_ParseTuple  PyUnicode_FromString
static PyObject* teststring(PyObject* self, PyObject* args)
{
	char* str = 0;
	if (!PyArg_ParseTuple(args, "s", &str)) {
		return NULL;
	}
	//���ݹ������ַ�����utf - 8��ʽ������̨Ĭ��GBK����������
	printf("PyArg_ParseTuple str= %s  \n", str);

	/*/ ��������ģ�Ҫ�Ѵ�������Ϊutf - 8��ʽ
		/ ����ռ䣬���ü��� + 1������py thon�������ͷ�*/
	return 	PyLong_FromLong(0);
};

// ����ģ�麯��PyFloat_AsDouble
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


//5. ����ģ�麯��PyArg_ParseTuple  
static PyObject* testmod(PyObject* self,PyObject *args)
{
	int x = 0;
	int y = 0;
	float z = 0;
	if (!PyArg_ParseTuple(args,"iif",&x,&y,&z))
	{
		printf("PyArg_ParseTuple");
		return NULL;//���׳��쳣
	}
	printf("PyArg_ParseTuple x= %d y= %d z= %f \n",x,y,z);
	// ����python��Long���Σ�c���������ü���+1������ֵ��python�ͷ�
	return 	PyLong_FromLong(105);
};


//4. ����ģ�麯���б�
static PyMethodDef mymod_funcs[] = {
	{
		"testmod",  //"��������"��
		testmod, //����ָ�룬
		METH_VARARGS,//������ʶ���޲�METH_NOARGS,METH_VARARGS  
		"testmod function."  //"����˵��"
	},
	{
		"testfloat",  //"��������"��
		testfloat, //����ָ�룬
		METH_O,//������ʶ���޲�METH_NOARGS,METH_VARARGS  
		"testfloat function."  //"����˵��"
	},
	{
		"teststring",  //"��������"��
		teststring, //����ָ�룬
		METH_VARARGS,//������ʶ���޲�METH_NOARGS,METH_VARARGS  
		"teststring function."  //"����˵��"
	},
	{
		"test_kw",  //"��������"��
		test_kw, //����ָ�룬
		METH_VARARGS | METH_KEYWORDS,//������ʶ���޲�METH_NOARGS,METH_VARARGS  
		"test_kw function."  //"����˵��"
	},
	{
		"test_tuple",  //"��������"��
		test_tuple, //����ָ�룬
		METH_VARARGS,//������ʶ���޲�METH_NOARGS,METH_VARARGS  
		"test_tuple function."  //"����˵��"
	},
	{
		"test_list",  //"��������"��
		test_list, //����ָ�룬
		METH_VARARGS,//������ʶ���޲�METH_NOARGS,METH_VARARGS  
		"test_list function."  //"����˵��"
	},
	{
		"test_dict",  //"��������"��
		test_dict, //����ָ�룬
		METH_VARARGS,//������ʶ���޲�METH_NOARGS,METH_VARARGS  
		"test_dict function."  //"����˵��"
	},
	
	{0,0,0,0}
};

//3. ����ģ��
static PyModuleDef mymod_module = {
	PyModuleDef_HEAD_INIT,
	"TestPythonExt",                           //"ģ����"��
	"TestPythonExt is first module test",      //"ģ��˵��",    help(ģ����)
	-1,                               //ģ��ռ䣬
	mymod_funcs,                    //ģ�麯���б� 
};

//1. ��չ����ں���  PyMODINIT_FUNC�� extern c __declspec(dllexport)
PyMODINIT_FUNC PyInit_TestPythonExt(void)  // define PyMODINIT_FUNC __declspec(dllexport) PyObject*
{
	printf("PyInit_TestPythonExt\n");
	//2. ģ�鴴��
	return PyModule_Create(&mymod_module);

}