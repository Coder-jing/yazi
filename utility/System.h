#pragma once

#include <string.h>
#include <string>
using namespace std;

namespace yazi {
namespace utility {

class System
{
public:
    System();
    ~System();

    void init();
    string get_root_path();

private:
    void core_dump();

private:
    string m_root_path;
};

}}
