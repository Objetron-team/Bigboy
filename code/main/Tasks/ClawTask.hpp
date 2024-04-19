#pragma once

#include "BasicTask.hpp"
#include "../Claw.hpp"

#define DISTANCE_THRESHOLD 5 // cm
#define TTL_FACTOR 300

class ClawTask : public BasicTask
{

private:
    
    Claw *myClaw;
int idd;
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
        if (idd == 0){
            myClaw->Open();
        }
        else{
            myClaw->Close();
        }
    }

public:
    ClawTask( Claw *claw, int id)
    {
        myClaw = claw;
        this->idd = id;
    }
    
        
    
};