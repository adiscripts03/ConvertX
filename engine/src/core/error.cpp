#include <convertor/error.hpp>

using namespace std;

namespace convertor {

Error::Error() : code_(ErrorCode::kOk) {}
Error::Error(ErrorCode code) : code_(code) {}
Error::Error(ErrorCode code, string message) : code_(code), message_(move(message)) {}
Error::Error(ErrorCode code, string message, string details)
    : code_(code), message_(move(message)), details_(move(details)) {}

bool Error::ok() const { return code_ == ErrorCode::kOk; }
ErrorCode Error::code() const { return code_; }
const string& Error::message() const { return message_; }
const string& Error::details() const { return details_; }

string Error::to_string() const {
    if (ok()) return "OK";
    return "Error(" + std::to_string(static_cast<uint32_t>(code_)) + "): " + message_;
}

Error Error::success() { return Error(); }

// --- Operator overloading implementations ---

Error::operator bool() const { return ok(); }
bool Error::operator!() const { return !ok(); }
bool Error::operator==(const Error& other) const { return code_ == other.code_; }
bool Error::operator!=(const Error& other) const { return code_ != other.code_; }

ostream& operator<<(ostream& os, const Error& err) {
    return os << err.to_string();
}

// Result<T>
template <typename T>
Result<T>::Result(T value) : data_(move(value)) {}

template <typename T>
Result<T>::Result(Error error) : data_(move(error)) {}

template <typename T>
bool Result<T>::ok() const { return holds_alternative<T>(data_); }

template <typename T>
const T& Result<T>::value() const { return get<T>(data_); }

template <typename T>
T& Result<T>::value() { return get<T>(data_); }

template <typename T>
const Error& Result<T>::error() const { return get<Error>(data_); }

template <typename T>
T Result<T>::value_or(T default_value) const {
    if (holds_alternative<T>(data_)) return get<T>(data_);
    return default_value;
}

// --- Result<T> operator overloading implementations ---

template <typename T>
const T& Result<T>::operator*() const { return value(); }

template <typename T>
T& Result<T>::operator*() { return value(); }

template <typename T>
const T* Result<T>::operator->() const { return &value(); }

template <typename T>
T* Result<T>::operator->() { return &value(); }

template <typename T>
Result<T>::operator bool() const { return ok(); }

// Result<void>
Result<void>::Result() : data_(monostate{}) {}
Result<void>::Result(Error error) : data_(move(error)) {}
bool Result<void>::ok() const { return holds_alternative<monostate>(data_); }
const Error& Result<void>::error() const { return get<Error>(data_); }

// Explicit template instantiations
template class Result<int>;
template class Result<int64_t>;
template class Result<double>;
template class Result<string>;

} // namespace convertor

