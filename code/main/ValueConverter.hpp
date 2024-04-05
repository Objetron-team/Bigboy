#pragma once

class ValueConverter{

    private:
        int pulse_per_turn = 1000;
        double wheel_diameter = 1; // in meter
        double wheel_distance = 1; // in meter

    public:

    ValueConverter(int pulse_per_turn, double wheel_diameter, double wheel_distance) {
        this->pulse_per_turn = pulse_per_turn;
        this->wheel_diameter = wheel_diameter;
        this->wheel_distance = wheel_distance;
    }

    double PulseToDistanceCM(int pulse){
        return pulse * 100 * wheel_diameter * M_PI / pulse_per_turn;
    }

    double PulseToAngle(int pulse){

        double arc_length = PulseToDistanceCM(pulse) / 100; // in meter
        double angle_rad = arc_length / wheel_distance; // in radian

        return angle_rad * 180 / M_PI;
    }

    double AngleToPulse(double angle){
        // in degree to radian
        double angle_rad = angle * M_PI / 180;
        double arc_length = wheel_distance * angle_rad; // in meter (wheel_distance is in meter)

        return DistanceCMToPulse(arc_length * 100);
    }

    double DistanceCMToPulse(double distance){
        return distance * pulse_per_turn / (wheel_diameter * 100 * M_PI);
    }

};