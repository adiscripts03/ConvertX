#pragma once

#include <string>
#include <functional>

#include <convertor/error.hpp>
#include <convertor/conversion_request.hpp>
#include <convertor/conversion_result.hpp>
#include <convertor/media_info.hpp>

namespace convertor {

using ProgressCallback = std::function<void(float)>;

/// Abstract base class for all converters (OOP: Abstraction + Inheritance).
///
/// Demonstrates the **Template Method** design pattern: `run()` defines a
/// fixed algorithm skeleton — validate, prepare, convert, finalize — and
/// derived classes override only the steps they need. Callers invoke `run()`
/// and get uniform pre/post processing for free.
class IConverter {
public:
    virtual ~IConverter() = default;

    virtual std::string name() const = 0;

    virtual bool can_handle(const ConversionRequest& request,
                            const MediaInfo& input_info) const = 0;

    /// The concrete conversion step. Subclasses MUST implement this.
    virtual Error convert(const ConversionRequest& request,
                          const MediaInfo& input_info,
                          ProgressCallback progress = nullptr) = 0;

    // -----------------------------------------------------------------
    // Template Method (OOP: defines the algorithm skeleton)
    // -----------------------------------------------------------------

    /// Runs the full conversion pipeline in a fixed order:
    ///   1. validate()  — check preconditions (overridable)
    ///   2. prepare()   — set up resources    (overridable)
    ///   3. convert()   — do the real work    (pure virtual)
    ///   4. finalize()  — clean up / verify   (overridable)
    ///
    /// Subclasses override individual hooks to inject behaviour without
    /// changing the overall flow.
    Error run(const ConversionRequest& request,
              const MediaInfo& input_info,
              ProgressCallback progress = nullptr) {
        Error err = validate(request, input_info);
        if (!err.ok()) return err;

        err = prepare(request, input_info);
        if (!err.ok()) return err;

        err = convert(request, input_info, progress);
        if (!err.ok()) return err;

        return finalize(request, input_info);
    }

protected:
    /// Hook 1: Validates that the conversion can proceed.
    /// Default implementation checks can_handle().
    virtual Error validate(const ConversionRequest& request,
                           const MediaInfo& input_info) {
        if (!can_handle(request, input_info)) {
            return Error(ErrorCode::kUnsupportedFormat,
                         name() + " cannot handle this conversion");
        }
        return Error::success();
    }

    /// Hook 2: Allocates resources or performs setup before conversion.
    /// Default implementation is a no-op.
    virtual Error prepare(const ConversionRequest& /*request*/,
                          const MediaInfo& /*input_info*/) {
        return Error::success();
    }

    /// Hook 3: Post-conversion cleanup or output validation.
    /// Default implementation is a no-op.
    virtual Error finalize(const ConversionRequest& /*request*/,
                           const MediaInfo& /*input_info*/) {
        return Error::success();
    }
};

} // namespace convertor

