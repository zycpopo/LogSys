#ifndef __M_LOOPER_H__
#define __M_LOOPER_H__

#include "buffer.hpp"
#include <mutex>
#include <thread>
#include <memory>
#include <atomic>
#include <functional>
#include <condition_variable>

namespace popolog{
    using Functor = std::function<void(Buffer &)>;
    enum class AsyncType{
        ASYNC_SAFE,
        ASYNC_UNSAFE
    };
    class AsyncLooper{
        public:
            using ptr = std::shared_ptr<AsyncLooper>;
            AsyncLooper(const Functor &cb,AsyncType looper_type = AsyncType::ASYNC_SAFE):_stop(false),
                _looper_type(looper_type),
                _thread(std::thread(&AsyncLooper::threadEntry,this)),
                _callback(cb){}
            ~AsyncLooper(){stop();}
            void stop()
            {
                _stop = true;
                _cond_con.notify_all();//唤醒所有工作流程（就一个）
                _thread.join();//等待工作线程退出
            }
            void push(const char *data,size_t len)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                if(_looper_type == AsyncType::ASYNC_SAFE)
                    _cond_pro.wait(lock,[&](){ return _pro_buf.writeAbleSize() >= len;});
                //走到这说明满足条件，可以向缓冲区添加数据
                _pro_buf.push(data,len);
                _cond_con.notify_one();
            }
        private:
            void threadEntry()
            {
                
                while(!_stop)  // 直接使用 _stop，因为 std::atomic<bool> 支持隐式转换
                {
                    {
                        std::unique_lock<std::mutex> lock(_mutex);
                        _cond_con.wait(lock,[&](){return !_pro_buf.empty() || _stop;}); // 直接用 _stop
                        if(_stop && _pro_buf.empty()) {  // 直接用 _stop
                            break;
                        }
                        _con_buf.swap(_pro_buf);
                        if (_looper_type == AsyncType::ASYNC_SAFE)
                        _cond_pro.notify_all();
                    }

                    if(!_con_buf.empty()) {
                    _callback(_con_buf);
                    _con_buf.reset();
                    }
                }
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    if(!_pro_buf.empty()) {
                        _con_buf.swap(_pro_buf);
                    }
                }
    
                if(!_con_buf.empty()) {
                    _callback(_con_buf);
                    _con_buf.reset();
                }
            }
            
        private:
            std::atomic<bool> _stop;
            AsyncType _looper_type;
            Buffer _pro_buf;
            Buffer _con_buf;
            std::mutex _mutex;
            std::condition_variable _cond_pro;
            std::condition_variable _cond_con;
            std::thread _thread;//异步工作器的工作线程
            Functor _callback;
    };
}

#endif