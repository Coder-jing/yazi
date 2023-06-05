#pragma once
#include <string>
using namespace std;

#include "Context.h"

namespace yazi {
namespace engine {

class Plugin
{
public:
    Plugin() : m_name(""), m_switch(false) {}
    Plugin(const string & name, bool flag) : m_name(name), m_switch(flag) {}
    virtual ~Plugin() {}

    void set_name(const string & name) { m_name = name; }
    const string & get_name() const { return m_name; }

    void set_switch(bool flag) { m_switch = flag; }
    bool get_switch() const { return m_switch; }

    virtual bool run(Context & ctx) = 0;

protected:
    string m_name;
    bool m_switch;
};

/*
这段宏定义假设存在一个名为 classType 的插件类，其中：

extern "C" Plugin* create() 是一个导出函数，用于创建插件对象。它通过动态分配内存创建一个 classType 类的对象，并返回其指针。注意 extern "C" 是为了确保函数以 C 语言的方式进行导出。
extern "C" void destroy(Plugin *p) 是一个导出函数，用于销毁插件对象。它接受一个 Plugin 类的指针作为参数，并在销毁前将指针设为 NULL，然后释放内存。
*/
#define DEFINE_PLUGIN(classType)                \
extern "C" Plugin* create()                     \
{                                               \
    return new (std::nothrow) classType();      \
}                                               \
extern "C" void destroy(Plugin *p)              \
{                                               \
    delete p;                                   \
    p = NULL;                                   \
}

}}
