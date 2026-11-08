#pragma once

class IMonitor
{
public:
    virtual ~IMonitor() = default;
    virtual bool Start() = 0;
    virtual void Stop() = 0;
};