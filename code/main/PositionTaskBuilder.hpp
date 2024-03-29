#pragma once

#include "Point.hpp"
#include "ValueConverter.hpp"
#include "DriveControler.hpp"
#include "PositionControler.hpp"

#include "Tasks/BasicTask.hpp"
#include "Tasks/MoveTask.hpp"
#include "Tasks/RotateTask.hpp"


class PositionTaskBuilder{
private:

    PositionControler* positionControler;
    DriveControler* driveControler;
    ValueConverter* valueConverter;
    
public:

    PositionTaskBuilder(PositionControler* positionControler, DriveControler* driveControler, ValueConverter* valueConverter){
        
        this->positionControler = positionControler;
        this->driveControler = driveControler;
        this->valueConverter = valueConverter;
    }

    MoveTask* CreateMoveTask(Point target){
        return new MoveTask(positionControler, driveControler, valueConverter, target);
    }

    RotateTask* CreateRotateTask(Point target){
        return new RotateTask(positionControler, driveControler, valueConverter, target);
    }

    BasicTask* CreateTasksFromPoints(Point* points, int size){
        BasicTask* first_task = CreateMoveTask(points[0]);

        for(int i = 1; i < size; i++){
            Point target = points[i];

            RotateTask* rotate_task = CreateRotateTask(target);
            MoveTask* move_task = CreateMoveTask(target);

            first_task->AddTask(rotate_task);
            rotate_task->AddTask(move_task);
        }

        return first_task;
    }   
};