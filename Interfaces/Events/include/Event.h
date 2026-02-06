#ifndef EVENT_H
#define EVENT_H

#include <memory>
#include "Callback.h"

#define INVALID_EVENT_ID (-1)

template<int MaxSize, typename... Args>
class Event {
public:
    ~Event() {
        Reset();
    }

    void Reset() {
        for(auto &callback: _callbacks) {
            if(callback == nullptr)
                continue;

            delete callback;
            callback = nullptr;
        }
    }

    /**
     * @brief Subscribe to a given event (for member functions)
     * @param instance The instance of the class subscribing to the event
     * @param callback The member function pointer of the instance that should be called when the event is triggered
     * @returns INVALID_EVENT_ID if no more callbacks can be registered to this event. Or a positive value with the event ID if succeeded
     */
    template<typename T>
    int Subscribe(T *instance, void (T::*method)(Args...)) {
        for(int idx = 0; idx < MaxSize; idx++) {
            if(_callbacks[idx] == nullptr) {
                _callbacks[idx] = new Callback<T, Args...>(instance, method);
                return idx;
            }
        }
        return INVALID_EVENT_ID;
    }

    /**
     * @brief Subscribe to a given event (for member functions)
     * @param owner The event owner of this callback
     * @param instance The instance of the class subscribing to the event
     * @param callback The member function pointer of the instance that should be called when the event is triggered
     * @returns false if another class has already registered to this event with same owner. True otherwise.
     */
    template<typename T>
    bool Subscribe(const EEventOwner &owner, T *instance, void (T::*method)(Args...)) {
        if(Exists(owner))
            return false;

        for(int idx = 0; idx < MaxSize; idx++) {
            if(_callbacks[idx] == nullptr) {
                _callbacks[idx] = new Callback<T, Args...>(owner, instance, method);
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Unsubscribe to a given event
     * @param eventId The event ID to unsubscribe
     */
    void Unsubscribe(int eventId) {
        if(eventId < 0 || eventId >= MaxSize)
            return;

        delete _callbacks[eventId];
        _callbacks[eventId] = nullptr;
    }

    /**
     * @brief Unsubscribe using the owner identifier
     * @param owner The event owner
     */
    void Unsubscribe(const EEventOwner &owner) {
        for(auto &callback: _callbacks) {
            if(callback != nullptr && callback->Owner == owner) {
                delete callback;
                callback = nullptr;
            }
        }
    }

    /**
     * @brief Trigger all callbacks for this event
     * @param args The arguments to pass to the callbacks
     */
    void Emit(Args... args) {
        for(const auto &callback: _callbacks) {
            if(callback != nullptr)
                callback->Invoke(args...);
        }
    }

    static std::shared_ptr<Event<MaxSize, Args...>> Make() {
        return std::make_shared<Event<MaxSize, Args...>>();
    }

private:
    ICallbackBase<Args...> *_callbacks[MaxSize] = {};

    bool Exists(const EEventOwner &owner) const {
        for(const auto &callback: _callbacks) {
            if(callback != nullptr && callback->Owner == owner)
                return true;
        }
        return false;
    }
};

#endif // EVENT_H
