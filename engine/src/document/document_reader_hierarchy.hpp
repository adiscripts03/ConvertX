#pragma once

/// Document Reader class hierarchy (OOP: Abstraction + Inheritance + Polymorphism).
///
/// Demonstrates:
///  - Abstract base class: IDocumentReader defines the interface
///  - Inheritance: PdfDocumentReader, DocxDocumentReader, etc. extend it
///  - Polymorphism: DocumentReaderRegistry selects the right reader at runtime
///    and calls it through the base pointer
///
/// The existing free-function API (convertor::readers::read_document) continues
/// to work and now delegates to this hierarchy internally.

#include <memory>
#include <string>
#include <vector>

#include <convertor/error.hpp>

#include "document_content.hpp"

namespace convertor {

// ---------------------------------------------------------------------------
// Abstract base class for document readers
// ---------------------------------------------------------------------------

class IDocumentReader {
public:
    virtual ~IDocumentReader() = default;

    /// Human-readable name for logging and diagnostics.
    virtual std::string name() const = 0;

    /// True when this reader can handle the given file extension.
    virtual bool can_read(const std::string& extension) const = 0;

    /// Reads the file at `path` and populates `out` with its content.
    virtual Error read(const std::string& path, DocumentContent& out) = 0;
};

// ---------------------------------------------------------------------------
// Concrete readers
// ---------------------------------------------------------------------------

/// Reads PDF documents via poppler.
class PdfDocumentReader : public IDocumentReader {
public:
    std::string name() const override { return "PdfDocumentReader"; }

    bool can_read(const std::string& extension) const override {
        return extension == "pdf";
    }

    Error read(const std::string& path, DocumentContent& out) override;
};

/// Reads OOXML documents (Word .docx, Excel .xlsx, PowerPoint .pptx).
class DocxDocumentReader : public IDocumentReader {
public:
    std::string name() const override { return "DocxDocumentReader"; }

    bool can_read(const std::string& extension) const override {
        return extension == "docx" || extension == "pptx" || extension == "xlsx";
    }

    Error read(const std::string& path, DocumentContent& out) override;
};

// ---------------------------------------------------------------------------
// Reader registry — selects the right reader polymorphically
// ---------------------------------------------------------------------------

class DocumentReaderRegistry {
public:
    static DocumentReaderRegistry& instance();

    void register_reader(std::unique_ptr<IDocumentReader> reader);

    /// Finds the first reader that can handle the extension.
    IDocumentReader* find_reader(const std::string& extension) const;

    /// All registered readers.
    std::vector<IDocumentReader*> all_readers() const;

private:
    DocumentReaderRegistry() = default;
    std::vector<std::unique_ptr<IDocumentReader>> readers_;
};

} // namespace convertor
