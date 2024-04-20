#pragma once

#include "BasicTask.hpp"
#include "../Claw.hpp"

#define DISTANCE_THRESHOLD 5 // cm
#define TTL_FACTOR 300

enum ClawState
{
    OPEN,
    CLOSE
};

class ClawTask : public BasicTask
{

private:
    Claw *myClaw;
    ClawState request_state;

    void _Update() override
    {
        Serial.print("");
    }

    bool _IsDone() override
    {

        return true;
    }

    void _Debug() override
    {
        Serial.print("");
    }

    void _Compute() override
    {
        if (request_state == OPEN)
        {
            myClaw->Open();
        }
        else
        {
            myClaw->Close();
        }
    }

public:
    ClawTask(Claw *claw, ClawState state)
    {
        myClaw = claw;
        request_state = state;
    }
};