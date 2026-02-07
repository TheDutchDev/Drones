#ifndef RS2205BLDCMOTOR_H
#define RS2205BLDCMOTOR_H

#include <cstdint>
#include <memory>

#include "IMotor.h"
#include "IMotorData.h"
#include "IProperty.h"

class RS2205BLDCMotor : public IMotor {
public:
    explicit RS2205BLDCMotor(
        const std::shared_ptr<IMotorData>& motorData
    );
    ~RS2205BLDCMotor();
    void Initialize() override;
    void Update() override;

private:
    void OnPropertyModified(IPropertyBase *propertyBase);
    std::shared_ptr<IMotorData> _motorData;
    int _motorDataEventHandle {-1};
};

#endif // RS2205BLDCMOTOR_H
