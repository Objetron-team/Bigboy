#pragma once

#include "BasicTask.hpp"


#define DISTANCE_THRESHOLD 0.5;


class MoveTask : public BasicTask{

private:
    PositionControler* positionControler;
    DriveControler* driveControler;
    ValueConverter* valueConverter;

    Point target_position;

    float distance_to_target_cm = 0;
    float distance_to_target_pulse = 0;

public:

    MoveTask(PositionControler* positionControler,DriveControler* driveControler ,ValueConverter* valueConverter ,Point target_position)  {
        this->positionControler = positionControler;
        this->driveControler = driveControler;
        this->valueConverter = valueConverter;

        this->target_position = target_position;
    }

    void Update() override {
        driveControler->SetDistance(distance_to_target_pulse);
        driveControler->SetAngle(0);
    }

    void Compute() override {
        Point current_position = positionControler->GetCurrentPoint();

        distance_to_target_cm = sqrt(pow(target_position.x - current_position.x, 2) + pow(target_position.y - current_position.y, 2));
    
        distance_to_target_pulse = valueConverter->DistanceCMToPulse(distance_to_target_cm);
    }

    bool IsDone() override {
        // check if the current position is close enough to the target position
        Point current_position = positionControler->GetCurrentPoint();

        float distance_to_target = sqrt(pow(target_position.x - current_position.x, 2) + pow(target_position.y - current_position.y, 2));

        return distance_to_target < DISTANCE_THRESHOLD;
    }

};