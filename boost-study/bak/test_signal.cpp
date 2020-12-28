#include "test_signal.h"

#include <boost/signals2.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <iostream>
using std::cout;
using std::endl;

template<int N>
class SlotA
{
    public:
        void operator()()
        {
            cout << "slot: " << N << endl;
        }

};

void basic_test_signal()
{
    boost::signals2::signal<void()> sig1;

    sig1.connect(SlotA<1>(), boost::signals2::at_back);
    sig1.connect(SlotA<2>(), boost::signals2::at_front);

    sig1.connect(5, SlotA<51>(), boost::signals2::at_front);
    sig1.connect(5, SlotA<55>(), boost::signals2::at_front);

    sig1.connect(3, SlotA<31>(), boost::signals2::at_front);
    sig1.connect(3, SlotA<35>(), boost::signals2::at_front);

    sig1.connect(10, SlotA<100>(), boost::signals2::at_front);

    sig1();
}

void test_slot_track()
{
    typedef boost::signals2::signal<void()> signal_t;
    typedef signal_t::slot_type slot_t;

    signal_t sig2;
    auto p1 = boost::make_shared<SlotA<10>>();
    auto p2 = boost::make_shared<SlotA<20>>();
    boost::function<void()> func = boost::ref(*p1);

    sig2.connect(slot_t(func).track(p1));
    sig2.connect(slot_t(*p1).track(p1));

    sig2.connect(slot_t(*p2).track(p2));
    
    cout << "Init: sig2.numb: " << sig2.num_slots() << endl;
    sig2();

    p1.reset();

    cout << "After P1 reset, sig2.numb: " << sig2.num_slots() << endl;
    sig2();

}

class Door
{
public:
    void open() {cout << "Door Open" << endl;}
};

void open_door()
{
    cout << "Function Open Door" << endl;
}

class Ring
{
private:

    typedef boost::signals2::signal<void()> ring_signal_t;
    typedef ring_signal_t::slot_type        ring_slot_t;

    // boost::shared_ptr<boost::signals2::signal<void()>>    m_ring_signal;

    ring_signal_t   m_ring_signal;
    
    Door    m_door;

public:
    Ring()
    {
        m_ring_signal.connect(&open_door);

        // auto open_door_inner = boost::bind(&Door::open, m_door);
        // m_ring_signal.connect(open_door_inner);

        m_ring_signal.connect(ring_slot_t(&Door::open, m_door));
    }

    void Alarm()
    {
        cout << "Ring Alarm " << endl;
        // m_ring_signal->operator();
        // (*m_ring_signal)();

        m_ring_signal();
    }

    void Connect(const ring_slot_t& slot)
    {
        m_ring_signal.connect(slot);
    }
};

void test_copy_signal()
{

    typedef boost::signals2::signal<void()> signal_t;
    typedef signal_t::slot_type        slot_t;

    Door door_obj;
    signal_t ring;
    ring.connect(slot_t(&Door::open, door_obj));

    ring();
}

void test_class_inner_connect()
{
    Ring ring_obj;

    ring_obj.Alarm();
}

void test_class_outter_connect()
{
    Ring ring_obj;
    Door door_obj;

    ring_obj.Connect(boost::bind(open_door));
    ring_obj.Connect(boost::bind(&Door::open, door_obj));

    ring_obj.Alarm();    
}

void TestSignal()
{
    // basic_test_signal();

    // test_slot_track();

    // test_copy_signal();

    test_class_inner_connect();

    // test_class_outter_connect();
}