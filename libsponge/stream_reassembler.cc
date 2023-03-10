#include "stream_reassembler.hh"

#include <algorithm>
#include <numeric>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output{capacity}, _capacity{capacity} {
    _buffer.reserve(capacity);
    _map.reserve(capacity);
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (eof)
        _eof = index + data.size();
    auto read_pos = max(index, _output.bytes_written()) - index;
    if (read_pos > data.size())
        return;
    auto write_pos = max(index, _output.bytes_written()) - _output.bytes_written();
    if (write_pos > _capacity)
        return;
    auto len = min(data.size() - read_pos, _capacity - write_pos);
    if (write_pos + len > _buffer.size()) {
        _buffer.resize(write_pos + len);
        _map.resize(write_pos + len);
    }
    copy(data.begin() + read_pos, data.begin() + read_pos + len, _buffer.begin() + write_pos);
    fill(_map.begin() + write_pos, _map.begin() + write_pos + len, true);
    len = min(static_cast<size_t>(distance(_map.begin(), find(_map.begin(), _map.end(), false))),
              _output.remaining_capacity());
    _buffer.erase(_buffer.begin(), _buffer.begin() + _output.write(_buffer.substr(0, len)));
    _map.erase(_map.begin(), _map.begin() + len);
    if (_output.bytes_written() == _eof)
        _output.end_input();
}

size_t StreamReassembler::unassembled_bytes() const {
    return accumulate(_map.begin(), _map.end(), 0, [](auto lhs, bool rhs) { return lhs + rhs; });
}

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }
