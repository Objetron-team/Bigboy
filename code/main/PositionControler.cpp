
#include "DriveControler.cpp";

struct Point{
    float x;
    float y;
};

enum TaskType{
    MOVE,
    ROTATE,
};


class Task{

    private:
        TaskType type;
        Point point;

        float distance_to_travel = 0;
        float angle_to_rotate = 0;

        Task* next_task = NULL;

        float threshold = 0.05; // in %
    public:

        int id;

        Task(TaskType type, Point point){
            this->type = type;
            this->point = point;
            this->next_task = NULL;
        }

        TaskType GetType(){
            return type;
        }

        float GetDistance(){
            return distance_to_travel;
        }

        float GetAngle(){
            return angle_to_rotate;
        }

        void Compute(Point current_point){
            if(type == MOVE){
                // Compute the distance to travel
                distance_to_travel = sqrt(pow(point.x - current_point.x, 2) + pow(point.y - current_point.y, 2));
            }else if(type == ROTATE){

                // Compute the angle to rotate (angle between the current position and the target position) in degree
                float dx = point.x - current_point.x;
                float dy = point.y - current_point.y;

                angle_to_rotate = atan2(dy, dx) * 180 / M_PI;
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
                return sqrt(pow(point.x - current_point.x, 2) + pow(point.y - current_point.y, 2)) < threshold;
            }else if(type == ROTATE){
                return abs(atan2(point.y - current_point.y, point.x - current_point.x) * 180 / M_PI) < threshold;
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



class PositionControler{
    private:
        
        int pulse_per_turn = 1000;
        float wheel_diameter = 3.5; // in meter
        
        DriveControler* driveControler;

        Point current_point;
        Point target_point;

        Task* current_task;

        long last_update_time;

        float last_distance = 0;
        float last_angle = 0;

    public:
        PositionControler(DriveControler* driveControler){
            this->driveControler = driveControler;

            current_point.x = 0;
            current_point.y = 0;

            current_task = new Task(MOVE, {0,0});
            current_task->id = 0;
        }

        void Update(){


            if(current_task->IsDone(current_point) && current_task->HasNextTask()){
                driveControler->UrgentStop();
                driveControler->Reset();    // Reset all counter to avoid drift and overflows
                current_task = current_task->GetNextTask();
                current_task->Compute(current_point);
            }

            if(current_task->GetType() == MOVE){

                float distance_in_pulse = current_task->GetDistance() * pulse_per_turn / (wheel_diameter * M_PI);

                driveControler->SetDistance(distance_in_pulse);
                driveControler->SetAngle(0);


            }else if(current_task->GetType() == ROTATE){

                float angle_in_pulse = current_task->GetAngle() * pulse_per_turn / 360;

                driveControler->SetDistance(0);
                driveControler->SetAngle(angle_in_pulse);

            }

            float dt = (micros() - last_update_time) / 1000000.0;

            // integrate the position
            float distance = driveControler->GetDistance() * wheel_diameter * M_PI / pulse_per_turn;
            float angle = driveControler->GetAngle() * 360 / pulse_per_turn;    // in degree

            float d_distance = distance - last_distance;
            float d_angle = angle - last_angle;

            current_point.x += d_distance * cos(angle) * dt;
            current_point.y += d_angle * sin(angle) * dt;

            last_distance = distance;
            last_angle = angle;

            last_update_time = micros();

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

};






