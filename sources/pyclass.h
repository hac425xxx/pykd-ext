#pragma once

#include "pyapi.h"


#include <comutil.h>

#include <string>
#include <vector>


class convert_python_exception : public std::exception
{
public:
    convert_python_exception(const char* message) : std::exception(message)
    {}
};


struct convert_from_python
{
    convert_from_python(PyObject* obj) : m_obj(obj){}

    operator std::wstring()
    {
        if ( PyUnicode_Check(m_obj) )
        {
            std::vector<wchar_t>  buf(0x10000);
            size_t  len = buf.size();
            len = PyUnicode_AsWideChar(m_obj, &buf[0], len);
            return std::wstring(&buf[0], len);
        }

        if (!IsPy3() )
        {
            if (PyString_Check(m_obj))
            {
                return std::wstring(_bstr_t(PyString_AsString(m_obj)));
            }
        }
        else
        {
            if (PyBytes_Check(m_obj))
            {
                return std::wstring(_bstr_t(PyBytes_AsString(m_obj)));
            }
        }

        throw convert_python_exception("failed convert argument");
    }


    operator std::string()
    {
        if (PyUnicode_Check(m_obj))
        {
            std::vector<wchar_t>  buf(0x10000);
            size_t  len = buf.size();
            len = PyUnicode_AsWideChar(m_obj, &buf[0], len);
            std::wstring  str(&buf[0], len);
            return std::string(_bstr_t(str.c_str()));
        }

        if (!IsPy3())
        {
            if (PyString_Check(m_obj))
            {
                return std::string(PyString_AsString(m_obj));
            }
        }
        else
        {
            if (PyBytes_Check(m_obj))
            {
                return std::string(PyBytes_AsString(m_obj));
            }
        }

        throw convert_python_exception("failed convert argument");
    }

    PyObject* m_obj;
};

struct convert_to_python
{
    operator PyObject* (){
        return m_obj;
    }

    convert_to_python()
    {

    }

    convert_to_python(const std::wstring& str)
    {
        m_obj = PyUnicode_FromWideChar(str.c_str(), str.size());
    }

    convert_to_python(bool v)
    {
        m_obj = PyBool_FromLong(v == true ? 1 : 0);
    }    

    PyObject* m_obj;
};



#define BEGIN_PYTHON_METHOD_MAP(classType, className) \
    template <typename TRet> \
    PyObject* callMethod0( \
        TRet (classType::*method)()) \
      { \
          TRet r = (this->*method)(); \
          return convert_to_python(r); \
      } \
    PyObject* callMethod0(\
        void (classType::*method)()) \
    { \
        (this->*method)(); \
        Py_IncRef(Py_None()); \
        return Py_None(); \
    } \
    template <typename TRet, typename V1> \
    PyObject* callMethod1( \
        TRet (classType::*method)(V1& v1), \
        convert_from_python& v1)\
      { \
          return (this->*method)(v1); \
      } \
    template <typename V1> \
    PyObject* callMethod1(\
      void(classType::*method)(V1& v1), \
      convert_from_python& v1)\
    { \
      (this->*method)(v1); \
      Py_IncRef(Py_None()); \
      return Py_None(); \
    } \
template<typename T = classType> \
static PyObject* getPythonClass() { \
        PyObject*  args = PyTuple_New(3); \
        PyTuple_SetItem(args, 0, IsPy3() ? PyUnicode_FromString(className) : PyString_FromString(className)); \
        PyTuple_SetItem(args, 1, PyTuple_New(0)); \
        PyTuple_SetItem(args, 2, PyDict_New()); \
        PyObject*  classTypeObj = PyObject_CallObject(PyType_Type(), args); \
        Py_DecRef(args);

#define END_PYTHON_METHOD_MAP  \
        return classTypeObj; \
    }

#define PYTHON_METHOD0(name, fn, doc) \
    struct Call_##fn { \
        static PyObject* pycall(PyObject *s, PyObject *args) \
         { \
            try { \
                PyObject*  self = PyTuple_GetItem(args, 0); \
                PyObject*  cppobj =  PyObject_GetAttrString(self, "cppobject"); \
                T*  _this = reinterpret_cast<T*>(PyCapsule_GetPointer(cppobj, "cppobject")); \
                Py_DecRef(cppobj); \
                return _this->callMethod0(&fn); \
            } \
            catch(convert_python_exception& exc) \
            { PyErr_SetString(PyExc_TypeError(), exc.what()); } \
            return Py_None(); \
        } \
    };  \
    {\
    static PyMethodDef methodDef = { name, Call_##fn::pycall, METH_VARARGS }; \
    PyObject*  cFuncObj = PyCFunction_NewEx(&methodDef, NULL, NULL); \
    PyObject*  methodObj = IsPy3() ? PyInstanceMethod_New(cFuncObj) : PyMethod_New(cFuncObj, NULL, classTypeObj); \
    PyObject_SetAttrString(classTypeObj, name, methodObj); \
    Py_DecRef(cFuncObj), Py_DecRef(methodObj); \
    }

#define PYTHON_METHOD1(name, fn, doc) \
    struct Call_##fn { \
        static PyObject* pycall(PyObject *s, PyObject *args) \
        { \
            PyObject*  self = PyTuple_GetItem(args, 0); \
            PyObject*  cppobj =  PyObject_GetAttrString(self, "cppobject"); \
            T*  _this = reinterpret_cast<T*>(PyCapsule_GetPointer(cppobj, "cppobject")); \
            Py_DecRef(cppobj); \
            PyObject* v1 = PyTuple_GetItem(args, 1); \
            return _this->callMethod1(&fn, convert_from_python(v1)); \
        } \
    };  \
    {\
    static PyMethodDef methodDef = { name, Call_##fn::pycall, METH_VARARGS }; \
    PyObject*  cFuncObj = PyCFunction_NewEx(&methodDef, NULL, NULL); \
    PyObject*  methodObj = IsPy3() ? PyInstanceMethod_New(cFuncObj) : PyMethod_New(cFuncObj, NULL, classTypeObj); \
    PyObject_SetAttrString(classTypeObj, name, methodObj); \
    Py_DecRef(cFuncObj), Py_DecRef(methodObj); \
    }

#define PYTHON_PROPERTY(name, fn, doc) \
    struct Call_##fn{ \
            static PyObject* pycall(PyObject *s, PyObject *args) \
             { \
                PyObject*  self = PyTuple_GetItem(args, 0); \
                PyObject*  cppobj =  PyObject_GetAttrString(self, "cppobject"); \
                T*  _this = reinterpret_cast<T*>(PyCapsule_GetPointer(cppobj, "cppobject")); \
                Py_DecRef(cppobj); \
                return _this->callMethod0(&fn); \
            } \
        };  \
        {\
        static PyMethodDef methodDef = { name, Call_##fn::pycall, METH_VARARGS }; \
        PyObject*  cFuncObj = PyCFunction_NewEx(&methodDef, NULL, NULL); \
        PyObject*  methodObj = IsPy3() ? PyInstanceMethod_New(cFuncObj) : PyMethod_New(cFuncObj, NULL, classTypeObj); \
        PyObject*  args = PyTuple_New(4); \
        Py_IncRef(PyProperty_Type()); \
        PyTuple_SetItem(args, 0, methodObj); \
        Py_IncRef(Py_None()); \
        PyTuple_SetItem(args, 1, Py_None()); \
        Py_IncRef(Py_None()); \
        PyTuple_SetItem(args, 2, Py_None()); \
        PyTuple_SetItem(args, 3, IsPy3() ? PyUnicode_FromString(doc) : PyString_FromString(doc));\
        PyObject*  propertyObj = PyObject_CallObject(PyProperty_Type(), args); \
        PyObject_SetAttrString(classTypeObj, name, propertyObj); \
        Py_DecRef(cFuncObj), Py_DecRef(propertyObj), Py_DecRef(args); \
        }



template<typename T1>
void delete_pyobject(PyObject* obj)
{
    T1*  cppobj = reinterpret_cast<T1*>(PyCapsule_GetPointer(obj, "cppobject"));
    delete cppobj;
}

template<typename T1, typename T2>
PyObject*  make_pyobject(const T2& var)
{
    PyObject* cls = T1::getPythonClass();
    PyObject* p1 = PyObject_CallObject(cls, NULL);
    Py_DecRef(cls);

    T1*  t1 = new T1(var);
    PyObject*  p2 = PyCapsule_New(t1, "cppobject", delete_pyobject<T1>);

    PyObject_SetAttrString(p1, "cppobject", p2);

    Py_DecRef(p2);

    return p1;
}



