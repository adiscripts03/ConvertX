#pragma once

/// Exception class hierarchy (OOP: Inheritance + Polymorphism).
///
/// Demonstrates:
///  - Inheritance: a hierarchy of exception types derived from std::runtime_error
///  - Polymorphism: catch-by-base-class handles any derived exception
///  - Encapsulation: each exception carries its ErrorCode and optional details
///
/// Usage:
///   try {
///       throw FileNotFoundException("/path/to/file");
///   } catch (const ConvertorException& e) {
///       // Catches any engine exception polymorphically.
///       std::cerr << e.what() << " [code " << (int)e.code() << "]\n";
///   }

#include <stdexcept>
#include <string>

#include <convertor/error.hpp>

namespace convertor {

// ---------------------------------------------------------------------------
// Base exception — all engine exceptions inherit from this.
// ---------------------------------------------------------------------------

class ConvertorException : public std::runtime_error {
public:
    explicit ConvertorException(const Error& err)
        : std::runtime_error(err.to_string()),
          code_(err.code()),
          details_(err.details()) {}

    ConvertorException(ErrorCode code, const std::string& message)
        : std::runtime_error(message),
          code_(code) {}

    ConvertorException(ErrorCode code, const std::string& message,
                       const std::string& details)
        : std::runtime_error(message),
          code_(code),
          details_(details) {}

    virtual ~ConvertorException() = default;

    /// The machine-readable error category.
    ErrorCode code() const { return code_; }

    /// Optional engine diagnostics (FFmpeg stderr tail, etc.).
    const std::string& details() const { return details_; }

private:
    ErrorCode code_;
    std::string details_;
};

// ---------------------------------------------------------------------------
// File-system exceptions
// ---------------------------------------------------------------------------

class FileNotFoundException : public ConvertorException {
public:
    explicit FileNotFoundException(const std::string& path)
        : ConvertorException(ErrorCode::kFileNotFound,
                             "File not found: " + path),
          path_(path) {}

    const std::string& path() const { return path_; }

private:
    std::string path_;
};

class PermissionDeniedException : public ConvertorException {
public:
    explicit PermissionDeniedException(const std::string& path)
        : ConvertorException(ErrorCode::kPermissionDenied,
                             "Permission denied: " + path),
          path_(path) {}

    const std::string& path() const { return path_; }

private:
    std::string path_;
};

// ---------------------------------------------------------------------------
// Format / conversion exceptions
// ---------------------------------------------------------------------------

class UnsupportedFormatException : public ConvertorException {
public:
    explicit UnsupportedFormatException(const std::string& format)
        : ConvertorException(ErrorCode::kUnsupportedFormat,
                             "Unsupported format: " + format),
          format_(format) {}

    const std::string& format() const { return format_; }

private:
    std::string format_;
};

// ---------------------------------------------------------------------------
// Encoding / decoding exceptions
// ---------------------------------------------------------------------------

class EncodingException : public ConvertorException {
public:
    explicit EncodingException(const std::string& message)
        : ConvertorException(ErrorCode::kFFmpegEncode, message) {}

    EncodingException(const std::string& message, const std::string& details)
        : ConvertorException(ErrorCode::kFFmpegEncode, message, details) {}
};

class DecodingException : public ConvertorException {
public:
    explicit DecodingException(const std::string& message)
        : ConvertorException(ErrorCode::kFFmpegDecode, message) {}

    DecodingException(const std::string& message, const std::string& details)
        : ConvertorException(ErrorCode::kFFmpegDecode, message, details) {}
};

// ---------------------------------------------------------------------------
// Document exceptions
// ---------------------------------------------------------------------------

class DocumentException : public ConvertorException {
public:
    explicit DocumentException(const std::string& message)
        : ConvertorException(ErrorCode::kPDFError, message) {}

    DocumentException(ErrorCode code, const std::string& message)
        : ConvertorException(code, message) {}
};

// ---------------------------------------------------------------------------
// Helper: throw from an Error value when it is not OK.
// ---------------------------------------------------------------------------

/// Converts an Error into the most specific exception type available.
///
///   Error err = some_operation();
///   throw_if_error(err);   // no-op when err.ok()
inline void throw_if_error(const Error& err) {
    if (err.ok()) return;

    switch (err.code()) {
        case ErrorCode::kFileNotFound:
            throw FileNotFoundException(err.message());
        case ErrorCode::kPermissionDenied:
            throw PermissionDeniedException(err.message());
        case ErrorCode::kUnsupportedFormat:
            throw UnsupportedFormatException(err.message());
        case ErrorCode::kFFmpegEncode:
            throw EncodingException(err.message(), err.details());
        case ErrorCode::kFFmpegDecode:
            throw DecodingException(err.message(), err.details());
        case ErrorCode::kPDFError:
        case ErrorCode::kPDFRender:
        case ErrorCode::kPDFTextExtract:
            throw DocumentException(err.message());
        default:
            throw ConvertorException(err);
    }
}

} // namespace convertor
