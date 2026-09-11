//
// Created by Wh3baby on 20.02.2023.
//

#include "MemoryFuck.h"
static void setThreadAffinityMask(pid_t tid, uint32_t mask)
{
    syscall(__NR_sched_setaffinity, tid, sizeof(mask), &mask);
}
void MemoryFuck::PushThread(int tid)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_Threads.push_back(tid);
}
void MemoryFuck::Routine()
{
    while (true)
    {
        m_Mutex.lock();
        for (auto& i : m_Threads)
        {
            uint32_t mask = 0xff;

            setThreadAffinityMask(i, mask);
        }
        m_Mutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
MemoryFuck::MemoryFuck()
{
    std::thread(&MemoryFuck::Routine, this).detach();
}
MemoryFuck::~MemoryFuck()
{

}