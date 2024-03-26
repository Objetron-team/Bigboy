
#include "PositionControler.hpp";

class BasicTask{
private:

    BasicTask* next_task = nullptr;

public:
    BasicTask();

    virtual void Update();

    virtual void Compute();

    virtual bool IsDone();

    void SetNextTask(BasicTask* next_task){
        
        // add the next task to the end of the list
        BasicTask* last_task = GetLastTask();

        next_task->id = last_task->id + 1;

        last_task->next_task = next_task;
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






/*
class Task{

    private:
        Task* next_task = nullptr;

        float threshold = 0.05; // in %
    public:

        int id;

        Task(TaskType type, Point point){
            this->type = type;
            this->point = point;
            this->next_task = NULL;
        }

        float GetDistance(){
            return distance_to_travel;
        }

        float GetAngle(){
            return angle_to_rotate;
        }

        void Compute(Point current_point, float current_angle){
            if(type == MOVE){
                // Compute the distance to travel
                distance_to_travel = sqrt(pow(point.x - current_point.x, 2) + pow(point.y - current_point.y, 2));
                angle_to_rotate = 0;
            }else if(type == ROTATE){

                // Compute the angle to rotate (angle between the current position and the target position) in degree
                float dx = point.x - current_point.x;
                float dy = point.y - current_point.y;

                angle_to_rotate = atan2(dy, dx) * 180 / M_PI;

                // Compute the shortest angle to rotate
                float angle_diff = angle_to_rotate - current_angle;

                if(angle_diff > 180){
                    angle_diff -= 360;

                }else if(angle_diff < -180){

                    angle_diff += 360;
                }

                angle_to_rotate = angle_diff;
                distance_to_travel = 0;

            }
        }

        float MoveError(Point current_point){
            return sqrt(pow(point.x - current_point.x, 2) + pow(point.y - current_point.y, 2));
        }

        float RotateError(Point current_point){
            return abs(atan2(point.y - current_point.y, point.x - current_point.x) * 180 / M_PI);
        }

        bool IsDone(Point current_point){

            if(type == MOVE){
                return sqrt(pow(point.x - current_point.x, 2) + pow(point.y - current_point.y, 2)) < 2;
            }else if(type == ROTATE){
                return abs(atan2(point.y - current_point.y, point.x - current_point.x) * 180 / M_PI) < 5;
            }

            return false;
        }

        void SetNextTask(Task* next_task){
            
            // add the next task to the end of the list
            Task* last_task = GetLastTask();

            next_task->id = last_task->id + 1;

            last_task->next_task = next_task;
        }

        bool HasNextTask(){
            return next_task != NULL;
        }

        Task* GetNextTask(){
            return next_task;
        }

        Task* GetLastTask(){
            Task* task = this;
            while(task->HasNextTask()){
                task = task->GetNextTask();
            }
            return task;
        }
};

*/