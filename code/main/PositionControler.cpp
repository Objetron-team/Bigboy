
#include "DriveControler.cpp";
#include "ValueConverter.hpp";


#ifndef PositionControler_hpp

#define PositionControler_hpp

// Define the structure of a point.
struct Point{
    float x;
    float y;
};

class PositionControler{
    private:
        DriveControler* driveControler;

        Point current_point;
        Point target_point;

        float current_angle = 0;

        Task* current_task;

        long last_update_time;

        float last_distance = 0;
        float last_angle = 0;

        bool started = false;
        bool auto_mode = false;

        ValueConverter* valueConverter;

    public:
        PositionControler(DriveControler* driveControler, int pulse_per_turn, float wheel_diameter, float wheel_distance){
            this->driveControler = driveControler;

            valueConverter = new ValueConverter(pulse_per_turn, wheel_diameter, wheel_distance);

            current_point.x = 0;
            current_point.y = 0;

            current_task = new Task(MOVE, {0,0});
            current_task->id = 0;
        }

        void Update(){

            if(!started){
                return;
            }

            if(auto_mode && current_task->IsDone(current_point) && current_task->HasNextTask()){
                driveControler->UrgentStop();
                driveControler->Reset();    // Reset all counter to avoid drift and overflows
                current_task = current_task->GetNextTask();
                current_task->Compute(current_point, current_angle);

                last_angle = 0;
                last_distance = 0;

            }

            if(current_task->GetType() == MOVE){

                float distance_in_pulse = valueConverter->DistanceCMToPulse(current_task->GetDistance());
                driveControler->SetDistance(distance_in_pulse);
                driveControler->SetAngle(0);


            }else if(current_task->GetType() == ROTATE){

                float angle_in_pulse = valueConverter->AngleToPulse(current_task->GetAngle());
                driveControler->SetDistance(0);
                driveControler->SetAngle(angle_in_pulse);

            }

            // integrate the position
            float distance = valueConverter->PulseToDistanceCM(driveControler->GetDistance());  // in centimeter
            float angle = valueConverter->PulseToAngle(driveControler->GetAngle());             // in degree

            float d_distance = distance - last_distance;    // in centimeter
            float d_angle = angle - last_angle;             // in degree

            current_angle += d_angle;

            // convert into cm
            current_point.x += d_distance * cos(current_angle * M_PI / 180);
            current_point.y += d_distance * sin(current_angle * M_PI / 180);

            last_distance = distance;
            last_angle = angle;

            driveControler->Update();
        }

        void AddPoint(Point point){

            Serial.println("Add point");

            // add the rotate task then the move task

            Task* rotate_task = new Task(ROTATE, point);
            Task* move_task = new Task(MOVE, point);

            current_task->SetNextTask(rotate_task);
            rotate_task->SetNextTask(move_task);
        }

        int GetTaskId(){
            return current_task->id;
        }

        int GetTaskType(){
            return current_task->GetType();
        }

        int GetNumberOfTask(){
            Task* task = current_task;
            int count = 0;
            while(task != NULL){
                count++;
                task = task->GetNextTask();
            }
            return count;
        }

        Task* GetCurrentTask(){
            return current_task;
        }

        Point GetCurrentPoint(){
            return current_point;
        }

        float GetCurrentAngle(){
            return current_angle;
        }

        void Start(){
            started = true;
            last_update_time = micros();
        }

        void Stop(){
            started = false;
        }

        void SetAutoMode(bool auto_mode){
            this->auto_mode = auto_mode;
        }

        void NextTask(){
            if(current_task->HasNextTask()){
                driveControler->UrgentStop();
                driveControler->Reset();    // Reset all counter to avoid drift and overflows
                current_task = current_task->GetNextTask();
                current_task->Compute(current_point, current_angle);

                last_angle = 0;
                last_distance = 0;
            }
        }

        void Reset(){
            driveControler->UrgentStop();
            driveControler->Reset();    // Reset all counter to avoid drift and overflows

            current_point.x = 0;
            current_point.y = 0;

            current_angle = 0;
            last_angle = 0;
            last_distance = 0;

            current_task = new Task(MOVE, {0,0});
            current_task->id = 0;

            last_update_time = micros();
        }


};


#endif
