/*
* 作者: 李晟泽
* 时间：2020.04.01
* 作用: 
* 提供基于boost::asio 的调度工具，能够根据需求设置调度执行的优先级;
* 
* 技术点：
    模板编程；
    函数对象；
*/

#include <string>
#include <functional>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <memory>
#include <list>
#include <deque>
#include <mutex>

using std::string;
using std::list;
using std::deque;

enum CallType
{
    ET_MARKET,
    ET_TRADE
};

class HandlerBase
{
    public:
        virtual ~HandlerBase() {}
        virtual void execute() {}
        bool is_execute() { return m_isExecuted; }
    private:
        bool                m_isExecuted;
};

template<typename FuncType>
class Handler: public HandlerBase
{
    public:
        Handler(FuncType func): m_func(std::move(func)){ }
        void execute() { if (!m_isExecuted) m_func(); }
        void set_depth() {m_is_depth_data = true;}
    private:
        FuncType            m_func;
        bool                m_is_depth_data{false};
};

class Priority_IO_Service
{
    public:
        Priority_IO_Service()
        {
            m_io_service_holder = std::make_shared<boost::asio::io_service>();
            m_io_service_worker = std::make_shared<boost::asio::io_service::work>(*m_io_service_holder);
        }

        class executor
        {   
            public:

            // io_service 的 post
            template<typename FuncType>
            void post(FuncType func, int func_type = 0)
            {

                if (func_type == ET_MARKET)
                {

                }

                if (func_type == ET_TRADE)
                {

                }

            }

            template<typename FuncType>
            void set_timer(FuncType func, int millonsecds)
            {
                auto cur_timer = std::make_shared<boost::asio::deadline_timer>(get_io_service());

                m_io_service_holder.post(boost::bind(&Priority_IO_Service::on_timer, cur_timer, millonsecds));
            }

            private:
            
        };

        void schedule()
        {

        }

        void launch()
        {
            for (int i = 0; i < m_schedule_thread_count; ++i)
            {
                m_schedule_thread_group.push_back(std::make_shared<std::thread>(&Priority_IO_Service::schedule));
            }

            m_io_service_thread = std::make_shared<std::thread>(&boost::asio::io_service::run, *m_io_service_holder);
        }

        void block()
        {
            for (auto cur_thread:m_schedule_thread_group)
            {
                if (cur_thread->joinable())
                {
                    cur_thread->join();
                }
            }

            if (m_io_service_thread->joinable())
            {
                m_io_service_thread->join();
            }
        }

        void stop()
        {
            m_io_service_holder->stop();
            m_io_service_worker.reset();            
        }

        boost::asio::io_service& get_io_service() 
        {
            return *m_io_service_holder;
        }

    private:

        using handler_ptr = typename std::shared_ptr<HandlerBase>;

        // boost::deadline
        void on_timer(std::function<void()> func, std::shared_ptr<boost::asio::deadline_timer> timer, int microseconds)
        {
            auto time_handler = std::make_shared<HandlerBase>(func);
            m_timer_queue.push_back(time_handler);

            timer->expires_from_now(boost::posix_time::microseconds(microseconds));

            timer->async_wait(boost::bind(Priority_IO_Service::on_timer, func, timer, microseconds));            
        }

        // handler queue

        // using time_ptr = 

        deque<handler_ptr>                                      m_timer_queue;
        deque<handler_ptr>                                      m_market_queue;
        deque<handler_ptr>                                      m_trade_queue;
        
        int                                                     m_schedule_thread_count;
        std::list<std::shared_ptr<std::thread>>                 m_schedule_thread_group;

        std::shared_ptr<std::thread>                            m_io_service_thread;   

        std::shared_ptr<boost::asio::io_service>                m_io_service_holder;
        std::shared_ptr<boost::asio::io_service::work>          m_io_service_worker;
};