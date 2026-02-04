#include "RS2205BLDCMotor.h"

static float Clamp01(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

RS2205BLDCMotor::RS2205BLDCMotor(TIM_HandleTypeDef *timer, uint32_t channel,
                                 float minDuty, float maxDuty, float disarmedDuty)
    : _timer(timer),
      _channel(channel),
      _minDuty(minDuty),
      _maxDuty(maxDuty),
      _disarmedDuty(disarmedDuty) {
}

void RS2205BLDCMotor::Initialize() {
    if (_timer) {
        HAL_TIM_PWM_Start(_timer, _channel);
    }
    Disarm();
}

void RS2205BLDCMotor::Arm() {
    _armed = true;
    WriteDuty(_minDuty);
}

void RS2205BLDCMotor::Disarm() {
    _armed = false;
    WriteDuty(_disarmedDuty);
}

void RS2205BLDCMotor::SetThrottle(float normalized) {
    if (!_armed) {
        return;
    }
    const float clamped = Clamp01(normalized);
    const float duty = _minDuty + ((_maxDuty - _minDuty) * clamped);
    WriteDuty(duty);
}

bool RS2205BLDCMotor::IsArmed() const {
    return _armed;
}

void RS2205BLDCMotor::WriteDuty(float duty) {
    if (!_timer) {
        return;
    }
    const uint32_t period = __HAL_TIM_GET_AUTORELOAD(_timer);
    const uint32_t compare = static_cast<uint32_t>(duty * static_cast<float>(period));
    __HAL_TIM_SET_COMPARE(_timer, _channel, compare);
}
