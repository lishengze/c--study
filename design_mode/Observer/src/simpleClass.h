#ifndef _SIMPLE_CLASS_H_
#define _SIMPLE_CLASS_H_

#include <list>
#include <iostream>
#include <memory>
#include <string>

class ObserverBase;
class InformerBase;

using ObserverPtr = std::shared_ptr<ObserverBase>;
using InformerPtr = std::shared_ptr<InformerBase>;
using State = std::string;


class InformerBase
{
public:
    InformerBase(std::string name):m_informer_name(name)
    {

    }

    ~InformerBase()
    {
        
    }

    void attach(ObserverPtr observer)
    {
        if (m_observer_list.find(observer) == m_observer_list.end())
        {
            m_observer_list.push_back(observer);
        }
    }

    void detach(ObserverPtr observer)
    {
        if (m_observer_list.find(observer) != m_observer_list.end())
        {
            m_observer_list.remove(observer);
        }
    }

    void set_state(State& state) 
    {
        m_state = state;
    }

    State get_state()
    {
        return m_state;
    }

    virtual void notify() = 0;

protected:
    std::list<ObserverPtr>          m_observer_list;
    std::string                     m_informer_name; 
    State                           m_state;

};

class ObserverBase
{
public:
    ObserverBase(std::string name, InformerPtr informer):
        m_observer_name(name),m_informer(informer)
    {
        if (nullptr != m_informer)
        {
            m_informer->attach(std::make_share);
        }
    }

    ~ObserverBase()
    {
        if (nullptr != m_informer)
        {
            m_informer->detach(this);
        }
    }

    virtual void update();

protected:
    std::string     m_observer_name;
    InformerPtr     m_informer;
    ObserverPtr     m_
};

class Informer:public InformerBase
{
public:
    virtual void notify()
    {
        for (ObserverPtr observer:m_observer_list)
        {
            observer->update();
        }
    }

};

class Observer:public ObserverBase
{
public:
    Observer(std::string name, InformerPtr informer):ObserverBase(name, informer)
    {
        
    }

    void update()
    {
        std::cout << "informer state: " << m_informer->get_state() 
             << ", " << m_observer_name << " update" << std::endl;
    }

};


#endif // !_SIMPLE_CLASS__H
