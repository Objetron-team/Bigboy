#pragma once

#include "../Point.hpp"
#include "../DriveControler.hpp"
#include "../ValueConverter.hpp"
#include "../PositionControler.hpp"

class BasicTask
{
private:
    BasicTask *next_task = nullptr;

    virtual bool _IsDone();

    virtual void _Update();

public:
    int id = 0;

    long start_time = 0;
    long time_to_live = 5; // seconds

    virtual void Compute();

    virtual void Update()
    {
        // UpdateTTL();
        _Update();
    }

    bool IsDone()
    {

        if (millis() - start_time > time_to_live * 1000)
        {
            return true;
        }

        return _IsDone();
    }

    virtual void Debug();

    void SetTimeToLive(long time_to_live)
    {
        this->time_to_live = time_to_live;
    }

    void AddTask(BasicTask *next_task)
    {

        // add the next task to the end of the list
        BasicTask *last_task = GetLastTask();

        next_task->id = last_task->id + 1;

        last_task->next_task = next_task;
    }

    void InsertTask(BasicTask *next_task)
    {
        next_task->next_task = this->next_task;
        this->next_task = next_task;
    }

    bool HasNextTask()
    {
        return next_task != nullptr;
    }

    BasicTask *GetNextTask()
    {
        return next_task;
    }

    BasicTask *GetLastTask()
    {
        BasicTask *task = this;
        while (task->HasNextTask())
        {
            task = task->GetNextTask();
        }
        return task;
    }
};