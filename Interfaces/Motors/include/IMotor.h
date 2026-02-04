#ifndef IMOTOR_H
#define IMOTOR_H

class IMotor {
public:
    virtual ~IMotor() = default;

    virtual void Initialize() = 0;
    virtual void Arm() = 0;
    virtual void Disarm() = 0;
    virtual void SetThrottle(float normalized) = 0;
    virtual bool IsArmed() const = 0;
};

#endif // IMOTOR_H
