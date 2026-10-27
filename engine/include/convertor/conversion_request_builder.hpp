#pragma once

/// Builder pattern for ConversionRequest (OOP: creational design pattern).
///
/// Demonstrates:
///  - Builder: step-by-step construction of a complex object
///  - Method chaining: fluent interface via returning *this
///  - Encapsulation: the partially-built state is private
///
/// Usage:
///   auto request = ConversionRequestBuilder()
///       .from("input.mp4")
///       .to("output.mkv")
///       .with_video_codec("h264")
///       .with_video_crf(23)
///       .with_audio_bitrate(192000)
///       .build();

#include <optional>
#include <stdexcept>
#include <string>

#include <convertor/conversion_request.hpp>
#include <convertor/conversion_settings.hpp>

namespace convertor {

class ConversionRequestBuilder {
public:
    ConversionRequestBuilder() = default;

    // --- Required fields ---

    /// Sets the input file path.
    ConversionRequestBuilder& from(const std::string& input_path) {
        input_path_ = input_path;
        return *this;
    }

    /// Sets the output file path.
    ConversionRequestBuilder& to(const std::string& output_path) {
        output_path_ = output_path;
        return *this;
    }

    // --- Optional: full settings object ---

    /// Replaces the entire settings block.
    ConversionRequestBuilder& with_settings(ConversionSettings settings) {
        settings_ = std::move(settings);
        return *this;
    }

    // --- Optional: video settings (fluent) ---

    ConversionRequestBuilder& with_video_codec(const std::string& codec) {
        settings_.video.codec = codec;
        return *this;
    }

    ConversionRequestBuilder& with_video_resolution(int width, int height) {
        settings_.video.width = width;
        settings_.video.height = height;
        return *this;
    }

    ConversionRequestBuilder& with_video_crf(int crf) {
        settings_.video.crf = crf;
        return *this;
    }

    ConversionRequestBuilder& with_video_bitrate(int64_t bitrate) {
        settings_.video.bit_rate = bitrate;
        return *this;
    }

    ConversionRequestBuilder& with_frame_rate(double fps) {
        settings_.video.frame_rate = fps;
        return *this;
    }

    // --- Optional: audio settings (fluent) ---

    ConversionRequestBuilder& with_audio_codec(const std::string& codec) {
        settings_.audio.codec = codec;
        return *this;
    }

    ConversionRequestBuilder& with_audio_bitrate(int64_t bitrate) {
        settings_.audio.bit_rate = bitrate;
        return *this;
    }

    ConversionRequestBuilder& with_sample_rate(int rate) {
        settings_.audio.sample_rate = rate;
        return *this;
    }

    ConversionRequestBuilder& with_channels(int channels) {
        settings_.audio.channels = channels;
        return *this;
    }

    // --- Optional: image settings (fluent) ---

    ConversionRequestBuilder& with_image_quality(int quality) {
        settings_.image.quality = quality;
        return *this;
    }

    ConversionRequestBuilder& with_image_size(int width, int height) {
        settings_.image.width = width;
        settings_.image.height = height;
        return *this;
    }

    // --- Optional: document settings (fluent) ---

    ConversionRequestBuilder& with_dpi(int dpi) {
        settings_.document.dpi = dpi;
        return *this;
    }

    ConversionRequestBuilder& with_page_range(const std::string& range) {
        settings_.document.page_range = range;
        return *this;
    }

    // --- Optional: global flags ---

    ConversionRequestBuilder& overwrite(bool value = true) {
        settings_.overwrite = value;
        return *this;
    }

    ConversionRequestBuilder& fast_start(bool value = true) {
        settings_.fast_start = value;
        return *this;
    }

    ConversionRequestBuilder& temp_dir(const std::string& dir) {
        settings_.temp_dir = dir;
        return *this;
    }

    // --- Build ---

    /// Constructs the ConversionRequest. Throws if required fields are missing.
    ConversionRequest build() const {
        if (input_path_.empty()) {
            throw std::invalid_argument(
                "ConversionRequestBuilder: input path is required");
        }
        if (output_path_.empty()) {
            throw std::invalid_argument(
                "ConversionRequestBuilder: output path is required");
        }
        return ConversionRequest(input_path_, output_path_, settings_);
    }

    /// Constructs the ConversionRequest, returning an error instead of throwing.
    ConversionRequest build(Error& err) const {
        if (input_path_.empty()) {
            err = Error(ErrorCode::kInvalidArgument, "Input path is required");
            return ConversionRequest();
        }
        if (output_path_.empty()) {
            err = Error(ErrorCode::kInvalidArgument, "Output path is required");
            return ConversionRequest();
        }
        err = Error::success();
        return ConversionRequest(input_path_, output_path_, settings_);
    }

private:
    std::string input_path_;
    std::string output_path_;
    ConversionSettings settings_;
};

} // namespace convertor
