#ifndef I_GPOUTPUT_H
#define I_GPOUTPUT_H

class IGpOutput {
public:
    virtual ~IGpOutput() = default;
    virtual bool Initialize() = 0;
    virtual void SetDuty(float duty) = 0;
    virtual bool IsValid() const = 0;
};

#endif // I_GPOUTPUT_H
