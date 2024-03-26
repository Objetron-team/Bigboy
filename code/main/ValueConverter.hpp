
#ifndef ValueConverter_hpp

#define ValueConverter_hpp

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

        float arc_length = PulseToDistanceCM(pulse) / 100; // in meter
        float angle_rad = arc_length / wheel_distance; // in radian

        return angle_rad * 180 / M_PI;
    }

    float AngleToPulse(float angle){
        // in degree to radian
        float angle_rad = angle * M_PI / 180;
        float arc_length = wheel_distance * angle_rad; // in meter (wheel_distance is in meter)

        return DistanceCMToPulse(arc_length * 100);
    }

    float DistanceCMToPulse(float distance){
        return distance * pulse_per_turn / (wheel_diameter * 100 * M_PI);
    }

};

#endif