#pragma once

#include "../Point.hpp"
#include "../DriveControler.hpp"
#include "../ValueConverter.hpp"
#include "../PositionControler.hpp"


class BasicTask{
private:

    BasicTask* next_task = nullptr;

public:
    int id = 0;

    virtual void Update();

    virtual void Compute();

    virtual bool IsDone();

    
    void AddTask(BasicTask* next_task){
        
        // add the next task to the end of the list
        BasicTask* last_task = GetLastTask();

        next_task->id = last_task->id + 1;

        last_task->next_task = next_task;
    }

    void InsertTask(BasicTask* next_task){
        next_task->next_task = this->next_task;
        this->next_task = next_task;
    }

    bool HasNextTask(){
        return next_task != nullptr;
    }

    BasicTask* GetNextTask(){
        return next_task;
    }

    BasicTask* GetLastTask(){
        BasicTask* task = this;
        while(task->HasNextTask()){
            task = task->GetNextTask();
        }
        return task;
    }

};