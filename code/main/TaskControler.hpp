#pragma once

#include "Point.hpp"
#include "DriveControler.hpp"
#include "ValueConverter.hpp"
#include "PositionControler.hpp"

#include "Tasks/BasicTask.hpp"
#include "Tasks/MoveTask.hpp"
#include "Tasks/RotateTask.hpp"
#include "Tasks/WaitTask.hpp"
#include "Tasks/ReverseTask.hpp"
#include "Tasks/ClawTask.hpp"
#include "Tasks/ForwardTask.hpp"
#include "Tasks/RotationTask.hpp"

class TaskControler
{
private:
    PositionControler *positionControler;
    DriveControler *driveControler;
    ValueConverter *valueConverter;

    BasicTask *current_task;

    bool started = false;
    bool auto_mode = false;

    long start_time = 0;

    void GoNextTask()
    {
        positionControler->SoftReset();

        current_task = current_task->GetNextTask();
        current_task->Compute();
    }

public:
    TaskControler(PositionControler *positionControler, DriveControler *driveControler, ValueConverter *valueConverter)
    {

        this->positionControler = positionControler;
        this->driveControler = driveControler;
        this->valueConverter = valueConverter;

        current_task = new MoveTask(positionControler, driveControler, valueConverter, {0, 0});
        current_task->id = 0;
    }

    void Update()
    {

        if (!started)
        {
            Stop();
            return;
        }

        if (millis() - start_time > DEATH_TIME * 1000)
        {
            Stop();
            return;
        }

        if (auto_mode && current_task->IsDone() && current_task->HasNextTask())
        {
            GoNextTask();
        }
        else if (auto_mode && current_task->IsDone() && !current_task->HasNextTask())
        {
            Stop();
        }

        if (!current_task->IsDone())
        {
            current_task->Update();
        }

        positionControler->Update();
    }

    void NextTask()
    {
        if (current_task->HasNextTask())
        {
            GoNextTask();
        }
    }

    BasicTask *GetCurrentTask()
    {
        return current_task;
    }

    void AddTask(BasicTask *next_task)
    {
        current_task->AddTask(next_task);
    }

    int GetTaskId()
    {
        return current_task->id;
    }

    int GetNumberOfTask()
    {
        BasicTask *task = current_task;
        int count = 0;
        while (task != NULL)
        {
            count++;
            task = task->GetNextTask();
        }
        return count;
    }

    void Start()
    {
        started = true;
        start_time = millis();
    }

    void Stop()
    {
        positionControler->SoftReset();
        started = false;
    }

    void SetAutoMode(bool auto_mode)
    {
        this->auto_mode = auto_mode;
    }

    void Reset()
    {
        Stop();

        positionControler->Reset();

        current_task = new MoveTask(positionControler, driveControler, valueConverter, {0, 0});
        current_task->id = 0;
    }

    void Debug()
    {
        Serial.print("Tasks:");
        Serial.print(GetNumberOfTask());
        Serial.print(",");

        // Serial.print("X:");
        // Serial.print(positionControler->GetCurrentPoint().x);
        // Serial.print(",");
        //
        // Serial.print("Y:");
        // Serial.println(positionControler->GetCurrentPoint().y);
        // Serial.print(",")//;
        //
        // Serial.print("cur_angle:");
        // Serial.print(positionControler->GetCurrentAngle());
        // Serial.print(",");
        //
        current_task->Debug();
    }
};
