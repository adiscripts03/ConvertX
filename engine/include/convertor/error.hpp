#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <variant>

using namespace std;

namespace convertor {

enum class ErrorCode : uint32_t {
    kOk = 0,
    kUnknown = 1,

    kFileNotFound = 100,
    kFileExists = 101,
    kPermissionDenied = 102,
    kDiskFull = 103,

    kInvalidArgument = 200,
    kInvalidPath = 201,
    kUnsupportedFormat = 202,

    kFFmpegError = 300,
    kFFmpegInit = 301,
    kFFmpegOpen = 302,
    kFFmpegDecode = 303,
    kFFmpegEncode = 304,
    kFFmpegMux = 305,

    kPDFError = 400,
    kPDFRender = 401,
    kPDFTextExtract = 402,

    kZipError = 500,
    kXmlParseError = 501,

    kJobCancelled = 600,
    kJobFailed = 601,
    kJobTimeout = 602,

    kOutOfMemory = 700,
    kInternal = 800,
};

class Error {
public:
    Error();
    explicit Error(ErrorCode code);
    Error(ErrorCode code, string message);
    Error(ErrorCode code, string message, string details);

    bool ok() const;
    ErrorCode code() const;
    const string& message() const;
    const string& details() const;

    string to_string() const;

    static Error success();

    // --- Operator overloading (OOP: polymorphic operators) ---

    /// Truthy when the error is OK (no error), enabling idiomatic checks:
    ///   if (err) { /* success */ }
    explicit operator bool() const;

    /// Negation: true when an error is present.
    ///   if (!err) { /* handle failure */ }
    bool operator!() const;

    /// Equality: compare errors by code.
    bool operator==(const Error& other) const;
    bool operator!=(const Error& other) const;

    /// Stream insertion for logging and diagnostics.
    friend ostream& operator<<(ostream& os, const Error& err);

private:
    ErrorCode code_;
    string message_;
    string details_;
};

template <typename T>
class Result {
public:
    Result(T value);
    Result(Error error);
    Result(const Result&) = default;
    Result& operator=(const Result&) = default;
    Result(Result&&) noexcept = default;
    Result& operator=(Result&&) noexcept = default;

    bool ok() const;
    const T& value() const;
    T& value();
    const Error& error() const;

    T value_or(T default_value) const;

    // --- Operator overloading (OOP: polymorphic operators) ---

    /// Dereference: access the value directly, like a smart pointer.
    ///   Result<int> r = 42;
    ///   int v = *r;
    const T& operator*() const;
    T& operator*();

    /// Arrow operator for member access on the contained value.
    const T* operator->() const;
    T* operator->();

    /// Truthy when the result holds a value (not an error).
    explicit operator bool() const;

private:
    variant<T, Error> data_;
};

template <>
class Result<void> {
public:
    Result();
    Result(Error error);
    Result(const Result&) = default;
    Result& operator=(const Result&) = default;

    bool ok() const;
    const Error& error() const;

private:
    variant<monostate, Error> data_;
};

} // namespace convertor
