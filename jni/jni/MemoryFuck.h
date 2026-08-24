//
// Created by Wh3baby on 20.02.2023.
//

#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <sys/syscall.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


class MemoryFuck {
private:
    void Routine();

    std::mutex m_Mutex;
    std::vector<pid_t> m_Threads;
public:
    MemoryFuck();
    ~MemoryFuck();

    void PushThread(pid_t tid);


};