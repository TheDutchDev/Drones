#ifndef RS2205BLDCMOTOR_H
#define RS2205BLDCMOTOR_H

#include "IMotor.h"
#include "stm32f4xx_hal.h"

class RS2205BLDCMotor final : public IMotor {
public:
    RS2205BLDCMotor(TIM_HandleTypeDef *timer, uint32_t channel,
                    float minDuty = 0.05f, float maxDuty = 0.10f, float disarmedDuty = 0.0f);

    void Initialize() override;
    void Arm() override;
    void Disarm() override;
    void SetThrottle(float normalized) override;
    bool IsArmed() const override;

private:
    void WriteDuty(float duty);

    TIM_HandleTypeDef *_timer = nullptr;
    uint32_t _channel = 0;
    float _minDuty = 0.05f;
    float _maxDuty = 0.10f;
    float _disarmedDuty = 0.0f;
    bool _armed = false;
};

#endif // RS2205BLDCMOTOR_H
