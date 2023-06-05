#include <sstream>
#include <iostream>

#include "Element.h"
using namespace yazi::xml;
using namespace std;

Element::Element()
{
}

Element::Element(const string & name) : m_name(name)
{
}

Element::Element(const string & name, const string & text) : m_name(name), m_text(text)
{
}

Element::~Element()
{
}

const string & Element::name() const
{
    return m_name;
}

const string & Element::text() const
{
    return m_text;
}

void Element::name(const string & name)
{
    m_name = name;
}

void Element::text(const string & text)
{
    m_text = text;
}

string & Element::attr(const string & key)
{
    return m_attrs[key];
}

void Element::attr(const string & key, const string & value)
{
    if (value != "")
    {
        m_attrs[key] = value;
        return;
    }

    std::map<string, string>::iterator it = m_attrs.find(key);
    if (it != m_attrs.end())
    {
        m_attrs.erase(it);
    }
}

const Element & Element::operator [] (int index) const
{
    int size = m_children.size();
    if (index >= 0 && index < size)
    {
        return m_children.at(index);
    }
    return null();
}

const Element & Element::operator [] (const string & name) const
{
    for (Element::const_iterator it = m_children.begin(); it != m_children.end(); it++)
    {
        if ((*it).name() == name)
        {
            return (*it);
        }
    }
    return null();
}

void Element::append(const Element & child)
{
    m_children.push_back(child);
}

int Element::size()
{
    return m_children.size();
}

void Element::clear()
{
    m_name.clear();
    m_text.clear();
    m_attrs.clear();
    m_children.clear();
}

string Element::toString() const
{
    if (m_name == "")
    {
        return "";
    }
    std::ostringstream os;
    os << "<" << m_name;
    for (std::map<string, string>::const_iterator it = m_attrs.begin(); it != m_attrs.end(); it++)
    {
        os << " " << it->first << "=\"" << it->second << "\"";  
    }
    os << ">";
    if (m_children.size() > 0)
    {
        for (std::vector<Element>::const_iterator it = m_children.begin(); it != m_children.end(); it++)
        {
            os << (*it).toString();
        }
    }
    else
    {
        os << m_text;
    }
    os << "</" << m_name << ">";
    return os.str();
}

void Element::print(Element element)
{
    /*
    string m_name;  //xml元素
    string m_text;  //xml元素文本
    std::vector<Element> m_children;    //XML 元素的子元素
    std::map<string, string> m_attrs;   //XML 元素的属性
    */
    cout<<"m_name:"<<element.m_name<<endl;
    cout<<"m_text:"<<element.m_text<<endl;
    for(auto it=element.m_children.begin();it!=element.m_children.end();it++)
    {
        print(*it);
    }
    cout<<endl;
    for(auto it=element.m_attrs.begin();it!=element.m_attrs.end();it++)
    {
        cout<<it->first<<" : "<<it->second<<endl;
    }
}

void Element::print()
{
    /*
    string m_name;  //xml元素
    string m_text;  //xml元素文本
    std::vector<Element> m_children;    //XML 元素的子元素
    std::map<string, string> m_attrs;   //XML 元素的属性
    */
    cout<<"m_name:"<<m_name<<endl;
    cout<<"m_text:"<<m_text<<endl;
    for(auto it=m_children.begin();it!=m_children.end();it++)
    {
        print(*it);
    }
    cout<<endl;
    for(auto it=m_attrs.begin();it!=m_attrs.end();it++)
    {
        cout<<it->first<<" : "<<it->second<<endl;
    }
}


Element const & Element::null()
{
    static const Element null;
    return null;
}
