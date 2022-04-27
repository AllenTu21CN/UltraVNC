#pragma once

#include "base/thread.h"

#include <string>
#include <chrono>

namespace base {

namespace io {
class IOContext;
}

/**
 * Class used to run a event loop for a thread.
 */
class EventLoop : public Thread
{
public:
    typedef struct Event
    {
        Event() : what(-1), arg1(-1), arg2(-1), obj(nullptr) {}
        Event(int what) : what(what), arg1(-1), arg2(-1), obj(nullptr) {}
        Event(int what, int arg1) : what(what), arg1(arg1), arg2(-1), obj(nullptr) {}
        Event(int what, int arg1, int arg2) : what(what), arg1(arg1), arg2(arg2), obj(nullptr) {}
        Event(int what, int arg1, int arg2, void *obj) : what(what), arg1(arg1), arg2(arg2), obj(obj) {}
        Event(const Event &e) : what(e.what), arg1(e.arg1), arg2(e.arg2), obj(e.obj) {}

        /**
         * User-defined event code so that the recipient can identify what this event is about.
         */
        int what;

        /**
         * arg1 and arg2 are lower-cost arguments if you only need to store a few integer values.
         */
        int arg1;
        int arg2;

        /**
         * Raw pointer for passing big object as argument.
         */
        void *obj;
    } Event;

    /**
     * Construct an EventLoop object.
     */
    EventLoop();

    /**
     * Destroy the object.
     */
    virtual ~EventLoop();

    /**
     * Post an event to the thread.
     */
    void postEvent(const Event &event);

    /**
     * Post an event to the thread, sync or async post
     * NOTE:
     * - sync: @event will be referenced, you use it as handle back.
     *         The method will wait @onEvent over.
     * - async: @event will be copied.
     *
     * @return, 
     * - sync: 0 is suc to post, or failed.
     * - async: always 0(suc).
     */
    int postEvent(Event &event, bool sync,
        const std::chrono::milliseconds &timeout = std::chrono::milliseconds(300000));

    /**
     * Get the default IOContext for the thread.
     */
    io::IOContext &getIOContext();

    /**
     * Quit the event loop and terminate the thread execution.
     */
    void quit();

protected:
    /**
     * Thread entry.
     */
    void run() override;

    virtual void onEvent(const Event &event) = 0;

    //virtual void onSystemSignal(int signal) = 0;

private:
    void *m_priv;

}; // End of class EventLoop

} // End of namespace base
