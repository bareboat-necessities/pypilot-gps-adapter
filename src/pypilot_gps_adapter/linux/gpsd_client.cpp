#include "gpsd_client.hpp"

#ifndef ARDUINO

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace pypilot_gps_adapter {
namespace linux_gpsd {

GpsdClient::GpsdClient() : fd_(-1) {
    last_error_[0] = 0;
}

GpsdClient::~GpsdClient() {
    close();
}

void GpsdClient::set_error(const char* message) {
    if (!message) message = "unknown gpsd error";
    strncpy(last_error_, message, sizeof(last_error_) - 1);
    last_error_[sizeof(last_error_) - 1] = 0;
}

const char* GpsdClient::last_error() const {
    return last_error_;
}

bool GpsdClient::connected() const {
    return fd_ >= 0;
}

bool GpsdClient::open(const char* host, const char* port) {
    close();
    if (!host) host = "127.0.0.1";
    if (!port) port = "2947";

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* results = 0;
    const int rc = getaddrinfo(host, port, &hints, &results);
    if (rc != 0) {
        set_error(gai_strerror(rc));
        return false;
    }

    for (struct addrinfo* ai = results; ai; ai = ai->ai_next) {
        const int fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd < 0) continue;
        if (::connect(fd, ai->ai_addr, ai->ai_addrlen) == 0) {
            fd_ = fd;
            int flags = fcntl(fd_, F_GETFL, 0);
            if (flags >= 0) fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
            freeaddrinfo(results);
            last_error_[0] = 0;
            return true;
        }
        ::close(fd);
    }

    freeaddrinfo(results);
    set_error(strerror(errno));
    return false;
}

void GpsdClient::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool GpsdClient::send_watch(bool enable) {
    if (fd_ < 0) {
        set_error("gpsd socket is not open");
        return false;
    }
    const char* watch_on = "?WATCH={\"enable\":true,\"json\":true};\n";
    const char* watch_off = "?WATCH={\"enable\":false};\n";
    const char* msg = enable ? watch_on : watch_off;
    const size_t len = strlen(msg);
    const ssize_t written = ::send(fd_, msg, len, 0);
    if (written < 0 || size_t(written) != len) {
        set_error(strerror(errno));
        return false;
    }
    return true;
}

int GpsdClient::read_line(char* out, unsigned capacity, int timeout_ms) {
    if (!out || capacity == 0) return -1;
    out[0] = 0;
    if (fd_ < 0) {
        set_error("gpsd socket is not open");
        return -1;
    }

    struct pollfd pfd;
    pfd.fd = fd_;
    pfd.events = POLLIN | POLLHUP | POLLERR;
    pfd.revents = 0;

    const int pr = poll(&pfd, 1, timeout_ms);
    if (pr == 0) return 0;
    if (pr < 0) {
        set_error(strerror(errno));
        return -1;
    }
    if (pfd.revents & (POLLHUP | POLLERR)) {
        set_error("gpsd socket closed");
        close();
        return -1;
    }

    unsigned n = 0;
    while (n + 1 < capacity) {
        char c = 0;
        const ssize_t r = recv(fd_, &c, 1, 0);
        if (r == 0) {
            set_error("gpsd socket closed");
            close();
            return -1;
        }
        if (r < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            set_error(strerror(errno));
            return -1;
        }
        if (c == '\n') break;
        if (c != '\r') out[n++] = c;
    }
    out[n] = 0;
    return int(n);
}

bool GpsdClient::read_tpv(GpsFixInput<float>& out, uint64_t now_us, int timeout_ms) {
    char line[512];
    const int n = read_line(line, sizeof(line), timeout_ms);
    if (n <= 0) return false;
    return parser_.parse_tpv(line, now_us, out, "gpsd");
}

} // namespace linux_gpsd
} // namespace pypilot_gps_adapter

#endif // ARDUINO
