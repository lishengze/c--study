#pragma once

class RspQueueManager {    
public:
    RspQueueManager() {
        // m_size = size;
        // m_buffer = new char[m_size];
    }

    ~RspQueueManager() {
        Release();
    }

    void Release();

    void Init(const char* cstrSharedMemName);
};