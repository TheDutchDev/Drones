#ifndef IMOTOR_H
#define IMOTOR_H

struct IMotorData;

class IMotor {
public:
    virtual ~IMotor() = default;

    virtual void Configure(float minDuty = 0.05f, float maxDuty = 0.10f,
                           float disarmedDuty = 0.0f) = 0;
    virtual void Initialize() = 0;
    virtual void Arm() = 0;
    virtual void Disarm() = 0;
    virtual void SetThrottle(float normalized) = 0;
    virtual bool IsArmed() const = 0;
};

#endif // IMOTOR_H
