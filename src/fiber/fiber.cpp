#include "fiber.h"
#include "common.h"
#include "config.h"
#include <atomic>

namespace why {

static std::atomic<uint64_t> s_fiber_id{0};
static std::atomic<uint64_t> s_fiber_num{0};

static thread_local Fiber* t_cur_fiber{nullptr};
static thread_local Fiber::ptr t_main_fiber{nullptr};

static constexpr uint64_t kKeyDefaultStackSize = 128 * 1024;

ConfigVar<uint64_t>::ptr g_stack_size_config = 
    ConfigVarManager::LookUp("fiber.stack_size", kKeyDefaultStackSize, "fiber stack size");

class MallocStackAllocator {
public:
    static void* Allocate(uint32_t size) {
        return malloc(size);
    }

    static void Dealloc(void* ptr) {
        free(ptr);
    }
};

using StackAllocator = MallocStackAllocator;

Fiber::Fiber(std::function<void()>& cb, uint64_t stk_size) : 
    m_cb(cb), 
    m_id(s_fiber_id++) {
    
    s_fiber_num++;
    uint32_t c_stk_size = g_stack_size_config->GetValue();
    stk_size = stk_size ? stk_size : c_stk_size;
    m_stkSize = stk_size ? stk_size : kKeyDefaultStackSize;

    if (getcontext(&m_ctx)) {
        CHECK_THROW(false, "Fiber::Fiber failed");
    }

    m_stkPtr = StackAllocator::Allocate(m_stkSize);

}

Fiber::Fiber() {
    m_state = State::EXEC;
    SetCurFiber(this);
    
    if (getcontext(&m_ctx)) {
        CHECK_THROW(false, "Fiber::Fiber() getcontext failed");
    }

    ++s_fiber_num;
}

void Fiber::SetCurFiber(Fiber* f) {
    t_cur_fiber = f;
}

uint64_t Fiber::TotalFibers() {
    return s_fiber_num;
}

}