#ifndef IMOTOR_H
#define IMOTOR_H

struct IMotorData;

class IMotor {
public:
    virtual ~IMotor() = default;
    virtual void Initialize() = 0;
    virtual void Update() = 0;
};

#endif // IMOTOR_H
