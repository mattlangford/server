#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

namespace server
{
// TODO: Should this be tcp_connection?
class tcp_message
{
public: ///////////////////////////////////////////////////////////////////////
    tcp_message() = default;
    tcp_message(int socket_fd, std::shared_ptr<std::mutex> mutex, std::shared_ptr<std::vector<uint8_t>> data);

public: ///////////////////////////////////////////////////////////////////////
    ///
    /// Grab a pointer to where we are in the data but don't move the read pointer.
    /// This may cause a read to happen.
    /// TODO: Timeouts
    ///
    bool peek_into(uint8_t* data, size_t size);

    ///
    /// Like peek, but will advance the read_pointer
    ///
    bool read_into(uint8_t* data, size_t size);

    ///
    /// Get the current remaining size of the data buffer
    ///
    size_t size() const;

    ///
    /// Writes the data to this communications channel
    /// TODO: This kind of breaks down when there is multiple messages alive at the same time
    ///
    void write_to_socket(std::string data);

    ///
    /// Close this connection
    /// TODO: This kind of breaks down when there is multiple messages alive at the same time
    ///
    void close_socket();

private: //////////////////////////////////////////////////////////////////////
    ///
    /// Reads the specified number of bytes
    ///
    size_t read_from_socket();

private: //////////////////////////////////////////////////////////////////////
    int socket_fd_ = -1;
    size_t read_pointer_ = 0;

    std::shared_ptr<std::mutex> data_lock_;
    std::shared_ptr<std::vector<uint8_t>> data_;
};
}
