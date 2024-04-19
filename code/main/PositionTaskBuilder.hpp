#pragma once

#include "Point.hpp"
#include "ValueConverter.hpp"
#include "DriveControler.hpp"
#include "PositionControler.hpp"

#include "Tasks/BasicTask.hpp"
#include "Tasks/MoveTask.hpp"
#include "Tasks/RotateTask.hpp"
#include "Tasks/WaitTask.hpp"
#include "Tasks/ReverseTask.hpp"

class PositionTaskBuilder
{
private:
    PositionControler *positionControler;
    DriveControler *driveControler;
    ValueConverter *valueConverter;

public:
    PositionTaskBuilder(PositionControler *positionControler, DriveControler *driveControler, ValueConverter *valueConverter)
    {

        this->positionControler = positionControler;
        this->driveControler = driveControler;
        this->valueConverter = valueConverter;
    }

    MoveTask *CreateMoveTask(Point target)
    {
        return new MoveTask(positionControler, driveControler, valueConverter, target);
    }

    RotateTask *CreateRotateTask(Point target)
    {
        return new RotateTask(positionControler, driveControler, valueConverter, target);
    }

    ReverseTask *CreateReverseTask(double distance_cm)
    {
        return new ReverseTask(driveControler, valueConverter, distance_cm);
    }

    BasicTask *MoveToPointTask(Point target)
    {
        BasicTask *first_task = CreateRotateTask(target);
        MoveTask *move_task = CreateMoveTask(target);

        first_task->AddTask(move_task);

        return first_task;
    }

    BasicTask *CreateTasksFromPoints(Point *points, int size)
    {
        WaitTask *first_task = new WaitTask(driveControler);

        for (int i = 1; i < size; i++)
        {
            Point target = points[i];

            WaitTask *wait_task_1 = new WaitTask(driveControler);
            WaitTask *wait_task_2 = new WaitTask(driveControler);

            RotateTask *rotate_task = CreateRotateTask(target);
            MoveTask *move_task = CreateMoveTask(target);

            first_task->AddTask(rotate_task);
            first_task->AddTask(wait_task_1);
            first_task->AddTask(move_task);
            first_task->AddTask(wait_task_2);
        }

        return first_task;
    }
};