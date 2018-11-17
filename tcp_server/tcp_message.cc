#include "tcp_server/tcp_message.hh"
#include <unistd.h>
#include <cstring>

namespace server
{

//
// ############################################################################
//

tcp_message::tcp_message(int socket_fd, std::shared_ptr<std::mutex> mutex, std::shared_ptr<std::vector<uint8_t>> data)
    : socket_fd_(socket_fd),
      read_pointer_(0),
      data_lock_(mutex),
      data_(data)
{
}

//
// ############################################################################
//

bool tcp_message::peek_into(uint8_t* data, size_t size)
{
    if (data_lock_ == nullptr || data_ == nullptr)
    {
        throw std::runtime_error("Trying to read from tcp socket without an active connection.");
    }

    std::lock_guard<std::mutex> lock(*data_lock_);
    while (data_->size() - read_pointer_ < size)
    {
        read_from_socket();
    }

    void* data_ptr = &(*data_)[read_pointer_];
    std::memcpy(data, data_ptr, size);
    return true;
}

//
// ############################################################################
//

bool tcp_message::read_into(uint8_t* data, size_t size)
{
    bool success = peek_into(data, size);
    if (success)
    {
        read_pointer_ += size;
    }

    return success;
}

//
// ############################################################################
//

size_t tcp_message::size() const
{
    if (data_lock_ == nullptr || data_ == nullptr)
    {
        throw std::runtime_error("Trying to get size from tcp socket without an active connection.");
    }

    std::lock_guard<std::mutex> lock(*data_lock_);
    return data_->size() - read_pointer_;
}

//
// ############################################################################
//

void tcp_message::write_to_socket(std::string data)
{
    write(socket_fd_, data.c_str(), data.size());
}

//
// ############################################################################
//

void tcp_message::close_socket()
{
    close(socket_fd_);
}

//
// ############################################################################
//

size_t tcp_message::read_from_socket()
{
    if (data_lock_ == nullptr || data_ == nullptr)
    {
        throw std::runtime_error("Trying to read from tcp socket without an active connection.");
    }

    constexpr size_t RECV_BUF_SIZE = 100;
    uint8_t recv_buff[RECV_BUF_SIZE];
    size_t bytes_read = RECV_BUF_SIZE;

    size_t total_bytes_read = 0;

    //
    // Now while we continue to fill the buffer, push bytes into the final data vector
    //
    while (bytes_read == RECV_BUF_SIZE)
    {
        bytes_read = read(socket_fd_, &recv_buff, RECV_BUF_SIZE);
        total_bytes_read += bytes_read;
        std::copy(&recv_buff[0], &recv_buff[bytes_read], std::back_inserter(*data_));
    }

    return total_bytes_read;
}
}
