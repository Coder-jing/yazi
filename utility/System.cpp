#include "System.h"
using namespace yazi::utility;

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>

#include "Logger.h"
#include "IniFile.h"
#include "Singleton.h"
using namespace yazi::utility;

#include "Workflow.h"
using namespace yazi::engine;


System::System()
{
}

System::~System()
{
}

void System::init()
{
    core_dump();

    m_root_path = get_root_path();

    const string& logdir = m_root_path + "/log";
    DIR * dp = opendir(logdir.c_str());                                                                                                      
    if (dp == NULL)
    {
        mkdir(logdir.c_str(), 0755);
    }
    else
    {
        closedir(dp);
    }

    // init logger
    Logger::instance()->open(m_root_path + "/log/main.log");

    // init inifile
    IniFile * ini = Singleton<IniFile>::instance();
    ini->load(get_root_path() + "/config/main.ini");

    // init workflow
    Workflow * workflow = Singleton<Workflow>::instance();
    workflow->load(get_root_path() + "/config/workflow.xml");
}

/*
这段代码涉及了对进程资源限制的设置。具体来说，它设置了核心转储文件的大小限制和数据段的大小限制。

首先，通过getrlimit函数获取当前的核心转储文件大小限制，即通过RLIMIT_CORE参数指定。获取的结果保存在结构体变量x中。
然后，将当前限制的软限制和硬限制都设置为相同的值，即x.rlim_cur = x.rlim_max，表示将限制设置为最大值。接着，使用setrlimit函数将新的核心转储文件大小限制应用于进程。

接下来，通过getrlimit函数获取当前的数据段大小限制，即通过RLIMIT_DATA参数指定。
同样，获取的结果保存在结构体变量x中。然后，将数据段的软限制设置为768000000，即x.rlim_cur = 768000000。最后，使用setrlimit函数将新的数据段大小限制应用于进程。

这段代码的目的是设置核心转储文件大小限制和数据段大小限制，可能是为了处理核心转储文件或控制进程的内存使用情况。
请注意，对于修改进程资源限制，可能需要适当的权限和操作系统支持，否则可能会导致操作失败。在实际应用中，应该根据具体的需求和环境进行设置，并进行错误处理来确保设置正确生效。
*/
void System::core_dump()
{
    // core dump
    struct rlimit x;
    int ret = getrlimit(RLIMIT_CORE, &x);
    x.rlim_cur = x.rlim_max;
    ret = setrlimit(RLIMIT_CORE, &x);

    ret = getrlimit(RLIMIT_DATA, &x);
    x.rlim_cur = 768000000;
    ret = setrlimit(RLIMIT_DATA, &x);
}

string System::get_root_path()
{
    if (m_root_path != "")
    {
        return m_root_path;
    }
    char path[1024];
    memset(path, 0, 1024);
    if(getcwd(path,sizeof(path))==NULL)
        return "";
    return string(path);
}
