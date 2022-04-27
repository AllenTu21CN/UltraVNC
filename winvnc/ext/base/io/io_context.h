#pragma once

#include <functional>

namespace base {
namespace io {

/**
 * Return true if the error is cancelled.
 */
bool errorIsCancelled(int error);

/**
 * The IOContext class provides the core I/O functionality for users of
 * the asynchronous I/O devices.
 */
class IOContext
{
public:
    /**
     * Constructor.
     */
    IOContext();

    /**
     * Destructor.
     */
    ~IOContext();

    // Non copyable
    IOContext(const IOContext &) = delete;
    IOContext &operator=(const IOContext &) = delete;

    /**
     * Ask the IOContext to execute the given callback.
     * The IOContext guarantees that the callback will only be called in a thread
     * in which the run(), runOnce() member functions is currently being invoked.
     * The callback may be executed inside this function if the guarantee can be met.
     */
    void dispatch(const std::function<void()> &callback);
    void dispatch(std::function<void()> &&callback);

    /**
     * @ref: https://stackoverflow.com/questions/2326588/boost-asio-io-service-dispatch-vs-post
     * @brief dispatch vs post
     * it depends on the context of the call, 
     * i.e. is it run from within the io_service or without:
     * - post will not call the function directly, ever, but postpone the call.
     * - dispatch will call it rightaway if the dispatch-caller was called from
     *   io_service itself, but queue it otherwise.
     *
     * ... 
     *
     * To incorporate some from @gimpf 's deleted answer, an older boost version had this
     * implementation of dispatch (my comments):
     *
     * ```
     * template <typename Handler>
     * void dispatch(Handler handler)
     * {
     *   if (call_stack<win_iocp_io_service>::contains(this)) // called from within io_service?
     *     boost_asio_handler_invoke_helpers::invoke(handler, &handler); // invoke rightaway
     *   else
     *    post(handler); // queue
     * }
     * ```
     */

    void post(const std::function<void()> &callback);
    void post(std::function<void()> &&callback);

    /**
     * Run the IOContext object's event processing loop.
     * The run() function blocks until all work has finished and there
     * are no more handlers to be dispatched, or until the IOContext has
     * been stopped.
     */
    void run();

    /**
     * Run the IOContext object's event processing loop to execute at most one handler.
     */
    void runOnce();

    /**
     * Stop the IOContext object's event processing loop.
     * This function does not block, but instead simply signals the
     * IOContext to stop.
     * All invocations of its run() member functions should return as soon
     * as possible.
     */
    void stop();

    /**
     * Determine whether the io_context object has been stopped.
     */
    bool stopped() const;

protected:
    void *getPriv();

private:
    void *m_priv;

    friend class IODevice;
    friend class Timer;

}; // End of class IOContext

} // End of namespace io
} // End of namespace base
