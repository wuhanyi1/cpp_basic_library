#ifndef __WHY_FIBER_H__
#define __WHY_FIBER_H__

#include <memory>
#include <functional>
#include <stdint.h>
#include <ucontext.h>

namespace why {

class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    using ptr = std::shared_ptr<Fiber>;

    template<typename Func, typename ...Args>
    Fiber(Func&& f, Args&& ...args, uint64_t stk_size) : Fiber(nullptr, stk_size) {
        std::function cb = std::bind(std::forward(f), std::forward(args)...);
        m_cb = cb;
    }

    Fiber(std::function<void()>& cb, uint64_t stk_size);

    enum class State {
        INIT, READY, EXEC, PAUSE, TERM, EXCEPT
    };

    ~Fiber();

    void Reset(std::function<void()>& cb);

    void SwapIn();
    void SwapOut();

    void SwapMainIn();
    // ？？
    void SwapMianOut();

    uint64_t GetId() const { return m_id; }

    State GetState() const { return m_state; }

public:
    /**
     * @description: 将 f 设置为当前线程的运行协程
     */
    static void SetCurFiber(Fiber* f);

    /**
     * @description: 获取当前正在执行的协程
     */
    static Fiber::ptr GetCurFiber(); 

    /**
     * @description: 将当前正在运行的协程切换到后台，设置为 READY 状态
     */
    static void YieldToReady();

    /**
     * @description: 将当前正在运行的协程切换到后台，设置为 PAUSE 状态
     */
    static void YieldToPause();

    static uint64_t TotalFibers();

    /**
     * @description: 所有协程的执行函数
     */
    static void Run();

private:
    /**
     * @description: 仅用以初始化 main_fiber
     */
    Fiber();

private:
    uint64_t m_stkSize{0};
    uint64_t m_id{-1};
    void* m_stkPtr{nullptr};
    State m_state{State::INIT};
    ucontext_t m_ctx{};
    std::function<void()> m_cb;
};

}

#endif