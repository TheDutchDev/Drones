#include "RS2205BLDCMotor.h"

RS2205BLDCMotor::RS2205BLDCMotor(const std::shared_ptr<IMotorData>& motorData)
    : _motorData(motorData) {
    _motorDataEventHandle = _motorData->OnPropertyModified->Subscribe(this, &RS2205BLDCMotor::OnPropertyModified);
}

RS2205BLDCMotor::~RS2205BLDCMotor() {
    _motorData->OnPropertyModified->Unsubscribe(_motorDataEventHandle);
}

void RS2205BLDCMotor::Initialize() {
}

void RS2205BLDCMotor::Update() {
}

void RS2205BLDCMotor::OnPropertyModified(IPropertyBase *property) {

}
