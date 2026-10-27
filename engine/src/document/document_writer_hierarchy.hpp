#pragma once

/// Document Writer class hierarchy (OOP: Abstraction + Inheritance + Polymorphism).
///
/// Mirrors the reader hierarchy: an abstract IDocumentWriter base class with
/// concrete implementations for each output format. A DocumentWriterRegistry
/// selects the right writer at runtime through the base pointer.

#include <memory>
#include <string>
#include <vector>

#include <convertor/error.hpp>

#include "document_content.hpp"

namespace convertor {

// ---------------------------------------------------------------------------
// Abstract base class for document writers
// ---------------------------------------------------------------------------

class IDocumentWriter {
public:
    virtual ~IDocumentWriter() = default;

    /// Human-readable name for logging and diagnostics.
    virtual std::string name() const = 0;

    /// True when this writer can produce the given file extension.
    virtual bool can_write(const std::string& extension) const = 0;

    /// Writes `content` to the file at `path`.
    virtual Error write(const DocumentContent& content,
                        const std::string& path) = 0;
};

// ---------------------------------------------------------------------------
// Concrete writers
// ---------------------------------------------------------------------------

/// Writes Word (.docx) documents.
class DocxDocumentWriter : public IDocumentWriter {
public:
    std::string name() const override { return "DocxDocumentWriter"; }

    bool can_write(const std::string& extension) const override {
        return extension == "docx";
    }

    Error write(const DocumentContent& content,
                const std::string& path) override;
};

/// Writes PDF documents.
class PdfDocumentWriter : public IDocumentWriter {
public:
    std::string name() const override { return "PdfDocumentWriter"; }

    bool can_write(const std::string& extension) const override {
        return extension == "pdf";
    }

    Error write(const DocumentContent& content,
                const std::string& path) override;
};

/// Writes Excel (.xlsx) spreadsheets.
class XlsxDocumentWriter : public IDocumentWriter {
public:
    std::string name() const override { return "XlsxDocumentWriter"; }

    bool can_write(const std::string& extension) const override {
        return extension == "xlsx";
    }

    Error write(const DocumentContent& content,
                const std::string& path) override;
};

/// Writes OpenDocument text (.odt).
class OdtDocumentWriter : public IDocumentWriter {
public:
    std::string name() const override { return "OdtDocumentWriter"; }

    bool can_write(const std::string& extension) const override {
        return extension == "odt";
    }

    Error write(const DocumentContent& content,
                const std::string& path) override;
};

/// Writes plain-text formats (txt, md, csv, html).
class PlainTextDocumentWriter : public IDocumentWriter {
public:
    std::string name() const override { return "PlainTextDocumentWriter"; }

    bool can_write(const std::string& extension) const override {
        return extension == "txt" || extension == "md" ||
               extension == "csv" || extension == "html";
    }

    Error write(const DocumentContent& content,
                const std::string& path) override;
};

// ---------------------------------------------------------------------------
// Writer registry — selects the right writer polymorphically
// ---------------------------------------------------------------------------

class DocumentWriterRegistry {
public:
    static DocumentWriterRegistry& instance();

    void register_writer(std::unique_ptr<IDocumentWriter> writer);

    /// Finds the first writer that can handle the extension.
    IDocumentWriter* find_writer(const std::string& extension) const;

    /// All registered writers.
    std::vector<IDocumentWriter*> all_writers() const;

private:
    DocumentWriterRegistry() = default;
    std::vector<std::unique_ptr<IDocumentWriter>> writers_;
};

} // namespace convertor
