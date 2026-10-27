#include "document_writer_hierarchy.hpp"

#include "document_writers.hpp"

#include <convertor/logging.hpp>

using namespace std;

namespace convertor {

// ---------------------------------------------------------------------------
// Concrete writers — delegate to the existing writer functions
// ---------------------------------------------------------------------------

Error DocxDocumentWriter::write(const DocumentContent& content,
                                const string& path) {
    return writers::write_docx(content, path);
}

Error PdfDocumentWriter::write(const DocumentContent& content,
                               const string& path) {
    return writers::write_pdf(content, path);
}

Error XlsxDocumentWriter::write(const DocumentContent& content,
                                const string& path) {
    return writers::write_xlsx(content, path);
}

Error OdtDocumentWriter::write(const DocumentContent& content,
                               const string& path) {
    return writers::write_odt(content, path);
}

Error PlainTextDocumentWriter::write(const DocumentContent& content,
                                     const string& path) {
    // Determine the extension from the path for format selection.
    string ext;
    const size_t dot = path.rfind('.');
    if (dot != string::npos) {
        ext = path.substr(dot + 1);
        for (auto& c : ext) c = static_cast<char>(tolower(c));
    }

    if (ext == "html") {
        return writers::write_html(content, path);
    }
    return writers::write_plain(content, path, ext);
}

// ---------------------------------------------------------------------------
// DocumentWriterRegistry — polymorphic writer selection
// ---------------------------------------------------------------------------

DocumentWriterRegistry& DocumentWriterRegistry::instance() {
    static DocumentWriterRegistry registry;
    return registry;
}

void DocumentWriterRegistry::register_writer(
    unique_ptr<IDocumentWriter> writer) {
    writers_.push_back(move(writer));
}

IDocumentWriter* DocumentWriterRegistry::find_writer(
    const string& extension) const {
    for (const auto& writer : writers_) {
        if (writer->can_write(extension)) return writer.get();
    }
    return nullptr;
}

vector<IDocumentWriter*> DocumentWriterRegistry::all_writers() const {
    vector<IDocumentWriter*> result;
    result.reserve(writers_.size());
    for (const auto& writer : writers_) result.push_back(writer.get());
    return result;
}

} // namespace convertor
