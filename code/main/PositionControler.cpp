
#include "DriveControler.cpp";

// Define the structure of a point.
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
                return sqrt(pow(point.x - current_point.x, 2) + pow(point.y - current_point.y, 2)) < 5;
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

class ValueConverter{

    private:
        int pulse_per_turn = 1000;
        float wheel_diameter = 1; // in meter
        float wheel_distance = 1; // in meter

    public:

    ValueConverter(int pulse_per_turn, float wheel_diameter, float wheel_distance) {
        this->pulse_per_turn = pulse_per_turn;
        this->wheel_diameter = wheel_diameter;
        this->wheel_distance = wheel_distance;
    }

    float PulseToDistanceCM(int pulse){
        return pulse * 100 * wheel_diameter * M_PI / pulse_per_turn;
    }

    float PulseToAngle(int pulse){
        // Tetha = tan-1( dd / L)

        float dd = PulseToDistanceCM(pulse);

        return atan2(dd, wheel_distance) * 180 / M_PI;
    }

    float AngleToPulse(float angle){

        // dd = L * tan(angle) if angle is small

        float pulse = 0;

        float angle_step = 10;
        float step_angle_rad = angle_step * M_PI / 180; 

        float pulse_per_step = wheel_distance * tan(step_angle_rad);

        float rest_angle = angle - (int)(angle / angle_step) * angle_step;
        float rest_angle_rad = rest_angle * M_PI / 180;

        pulse = (int)(angle / angle_step) * pulse_per_step;
        pulse += wheel_distance * tan(rest_angle_rad);

        return pulse;
    }

    float DistanceCMToPulse(float distance){
        return distance * pulse_per_turn / (wheel_diameter * 100 * M_PI);
    }

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

            float dt = (micros() - last_update_time) / 1000000.0;

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






