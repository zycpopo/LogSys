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
                while(!stop)
                {
                    //当缓冲区交换完毕后就解锁
                    {
                        std::unique_lock<std::mutex> lock(_mutex);
                        //判断生产缓冲区有没有数据，有则交换，无则阻塞
                        _cond_con.wait(lock,[&](){return !_pro_buf.empty() || _stop;});//若当前是退出前被唤醒，或者有数据被唤醒，则返回真，继续向下运行，否则重新陷入睡眠
                        _con_buf.swap(_pro_buf);
                        //唤醒生产者
                        if (_looper_type == AsyncType::ASYNC_SAFE)
                            _cond_pro.notify_all();
                    }

                    _callback(_con_buf);//唤醒后对消费缓冲区进行数据处理
                    _con_buf.reset();//初始化消费缓冲区
                }
            }
            Functor _callback;
        private:
            AsyncType _looper_type;
            std::atomic<bool> _stop;
            Buffer _pro_buf;
            Buffer _con_buf;
            std::mutex _mutex;
            std::condition_variable _cond_pro;
            std::condition_variable _cond_con;
            std::thread _thread;//异步工作器的工作线程
    };
}

#endif