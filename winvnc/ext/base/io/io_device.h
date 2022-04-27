#pragma once

#include <functional>

namespace base {
namespace io {

class IOContext;

/**
 * The IODevice class is the base interface class of all I/O devices.
 */
class IODevice
{
public:
    /**
     * This enum is used with open() to describe the mode in which a device
     * is opened.
     */
    enum OpenModeFlag
    {
        /**
         * The device is not open.
         */
        NotOpen = 0x0000,

        /**
         * The device is open for reading.
         */
        ReadOnly = 0x0001,

        /**
         * The device is open for writing.
         * Note that this mode implies Truncate.
         */
        WriteOnly = 0x0002,

        /**
         * The device is open for reading and writing.
         */
        ReadWrite = ReadOnly | WriteOnly,

        /**
         * The device is opened in append mode so that all data is written
         * to the end of the file.
         */
        Append = 0x0004,

        /**
         * If possible, the device is truncated before it is opened.
         * All earlier contents of the device are lost.
         */
        Truncate = 0x0008,

        /**
         * When reading, the end-of-line terminators are translated to '\n'.
         * When writing, the end-of-line terminators are translated to the
         * local encoding, for example '\r\n' for Win32.
         */
        Text = 0x0010,
#if 0
        /**
         * Any buffer in the device is bypassed.
         */
        Unbuffered  = 0x0020
#endif
    };

    /**
     * The OpenMode type stores an OR combination of OpenModeFlag values.
     */
    typedef int OpenMode;

    /**
     * Constructs an IODevice with given io_context.
     */
    IODevice(IOContext &io_context);

    /**
     * Move constructor.
     */
    IODevice(IODevice &&other);

    /**
     * The destructor is virtual, and IODevice is an abstract base class.
     * This destructor does not call close(), but the subclass destructor
     * might. If you are in doubt, call close() before destroying the
     * IODevice.
     */
    virtual ~IODevice();

    /**
     * Get the IOContext associated with the device.
     * @return
     */
    IOContext &getIOContext() const;

    /**
     * Opens the device and sets its OpenMode to mode. This function should
     * be called from any reimplementations of open() or other functions
     * that open the device.
     * Returns true if successful; otherwise returns false.
     */
    virtual bool open(OpenMode mode);

    /**
     * Close the device and sets its OpenMode to NotOpen. This function
     * should be called from any reimplementations of close() or other
     * functions that close the device.
     */
    virtual void close();

    /**
     * Start an asynchronous operation to read from device. The callback
     * to be called when the device ready for read. The function call
     * always returns immediately.
     */
    virtual void asyncRead(const std::function<void(int error)> &callback) = 0;
    virtual void asyncRead(std::function<void(int error)> &&callback) = 0;

    /**
     * Start an asynchronous operation to read at most max_length bytes
     * from device into data. The callback to be called when the read
     * operation completes. The function call always returns immediately.
     *
     * Note:
     * Although the data may be copied as necessary, ownership of the
     * underlying memory blocks is retained by the caller, which must
     * guarantee that they remain valid until the callback is called.
     */
    virtual void asyncRead(char *data, int64_t max_length,
                           const std::function<void(int error, int64_t bytes_read)> &callback) = 0;
    virtual void asyncRead(char *data, int64_t max_length,
                           std::function<void(int error, int64_t bytes_read)> &&callback) = 0;

    /**
     * Start an asynchronous operation to write into device. The callback
     * to be called when the device ready for write. The function call
     * always returns immediately.
     */
    virtual void asyncWrite(const std::function<void(int error)> &callback) = 0;
    virtual void asyncWrite(std::function<void(int error)> &&callback) = 0;

    /**
     * Start an asynchronous operation to write at most max_length bytes
     * from data into device. The callback to be called when the write
     * operation completes. The function call always returns immediately.
     *
     * Note:
     * Although the data may be copied as necessary, ownership of the
     * underlying memory blocks is retained by the caller, which must
     * guarantee that they remain valid until the callback is called.
     */
    virtual void asyncWrite(const char *data, int64_t max_length,
                        const std::function<void(int error, int64_t bytes_written)> &callback) = 0;
    virtual void asyncWrite(const char *data, int64_t max_length,
                        std::function<void(int error, int64_t bytes_written)> &&callback) = 0;

    /**
     * Reads at most max_length bytes from the device into data, and
     * returns the number of bytes read. If an error occurs, such as when
     * attempting to read from a device opened in WriteOnly mode, this
     * function returns -1.
     * 0 is returned when no more data is available for reading.
     * However, reading past the end of the stream is considered an error,
     * so this function returns -1 in those cases (that is, reading on
     * a closed socket or after a process has died).
     */
    virtual int64_t read(char *data, int64_t max_length) = 0;

    /**
     * Writes at most max_length bytes of data from data to the device.
     * Returns the number of bytes that were actually written,
     * or -1 if an error occurred.
     */
    virtual int64_t write(const char *data, int64_t max_length) = 0;

    /**
     * For random-access devices, this function sets the current position
     * to pos, returning true on success, or false if an error occurred.
     * For sequential devices, the default behavior is to produce a warning
     * and return false.
     */
    virtual bool seek(int64_t pos);

    /**
     * Returns the mode in which the device has been opened.
     */
    OpenMode openMode() const;

    /**
     * Returns true if the device is open; otherwise returns false.
     * A device is open if it can be read from and/or written to.
     * By default, this function returns false if openMode() returns
     * NotOpen.
     */
    bool isOpen() const;

    /**
     * Returns true if data can be read from the device;
     * otherwise returns false.
     * This is a convenience function which checks if the OpenMode of the
     * device contains the ReadOnly flag.
     */
    bool isReadable() const;

    /**
     * Returns true if this device is sequential; otherwise returns false.
     * Sequential devices, as opposed to a random-access devices, have no
     * concept of a start, an end, a size, or a current position, and they
     * do not support seeking. You can only read from the device when it
     * reports that data is available. The most common example of a
     * sequential device is a network socket. On Unix, special files such
     * as /dev/zero and fifo pipes are sequential.
     * Regular files, on the other hand, do support random access.
     * They have both a size and a current position, and they also support
     * seeking backwards and forwards in the data stream.
     * Regular files are non-sequential.
     */
    virtual bool isSequential() const = 0;

    /**
     * Returns true if the Text flag is enabled; otherwise returns false.
     */
    bool isTextModeEnabled() const;

    /**
     * Returns true if data can be written to the device;
     * otherwise returns false.
     * This is a convenience function which checks if the OpenMode of the
     * device contains the WriteOnly flag.
     */
    bool isWritable() const;

protected:
    void *getIOContextPriv();

private:
    void *m_priv;

}; // End of class IODevice

} // End of namespace io
} // End of namespace base
