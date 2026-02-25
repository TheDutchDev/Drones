#ifndef I_GPINPUT_H
#define I_GPINPUT_H

class IGpInput {
public:
    virtual ~IGpInput() = default;
    virtual bool Initialize() = 0;
    virtual bool Read() const = 0;
    virtual bool IsValid() const = 0;
};

#endif // I_GPINPUT_H
