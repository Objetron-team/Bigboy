#pragma once

#include "BasicTask.hpp"
#include "../Claw.hpp"

#define DISTANCE_THRESHOLD 5 // cm
#define TTL_FACTOR 300

class ClawTask : public BasicTask
{

private:
    
    Claw *myClaw;

    void _Update() override
    {
        
    }

    bool _IsDone() override
    {
        
        return true;
    }
    
    void _Debug() override
    {
      
    }

    void _Compute() override
    {
       myClaw->Close();
    }

public:
    ClawTask( Claw *claw)
    {
        myClaw = claw;
    }
    
        
    
};