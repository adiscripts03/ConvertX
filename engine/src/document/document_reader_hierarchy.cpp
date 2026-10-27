#include "document_reader_hierarchy.hpp"

#include "ooxml_reader.hpp"
#include "pdf_document.hpp"
#include "xml_text.hpp"

#include <convertor/logging.hpp>

#include <algorithm>
#include <sstream>

using namespace std;

namespace convertor {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

vector<string> split_lines(const string& text) {
    vector<string> lines;
    istringstream stream(text);
    string line;
    while (getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        lines.push_back(line);
    }
    return lines;
}

} // namespace

// ---------------------------------------------------------------------------
// PdfDocumentReader — reads PDFs via poppler
// ---------------------------------------------------------------------------

Error PdfDocumentReader::read(const string& path, DocumentContent& out) {
    PDFDocument pdf(path);
    if (!pdf.is_open()) {
        return Error(ErrorCode::kPDFError, "Cannot open PDF: " + path);
    }

    string text;
    Error err = pdf.extract_text(text);
    if (!err.ok()) return err;

    out.lines = split_lines(text);
    return Error::success();
}

// ---------------------------------------------------------------------------
// DocxDocumentReader — reads OOXML (docx, xlsx, pptx)
// ---------------------------------------------------------------------------

Error DocxDocumentReader::read(const string& path, DocumentContent& out) {
    OOXMLReader reader(path);
    if (!reader.is_open()) {
        return Error(ErrorCode::kZipError,
                     "Not a readable Office file: " + path);
    }

    // Detect xlsx by checking for spreadsheet cells.
    const auto cells = reader.extract_cells();
    if (!cells.empty()) {
        int max_row = 0, max_col = 0;
        for (const auto& c : cells) {
            max_row = max(max_row, c.row);
            max_col = max(max_col, c.col);
        }
        out.rows.assign(max_row + 1, vector<string>(max_col + 1));
        for (const auto& c : cells) out.rows[c.row][c.col] = c.value;

        for (const auto& row : out.rows) {
            string line;
            for (size_t i = 0; i < row.size(); ++i) {
                if (i) line += '\t';
                line += row[i];
            }
            out.lines.push_back(line);
        }
        return Error::success();
    }

    const string text = reader.extract_text();
    if (text.empty()) {
        return Error(ErrorCode::kXmlParseError, "No text found in " + path);
    }
    out.lines = split_lines(text);
    return Error::success();
}

// ---------------------------------------------------------------------------
// DocumentReaderRegistry — polymorphic reader selection
// ---------------------------------------------------------------------------

DocumentReaderRegistry& DocumentReaderRegistry::instance() {
    static DocumentReaderRegistry registry;
    return registry;
}

void DocumentReaderRegistry::register_reader(
    unique_ptr<IDocumentReader> reader) {
    readers_.push_back(move(reader));
}

IDocumentReader* DocumentReaderRegistry::find_reader(
    const string& extension) const {
    for (const auto& reader : readers_) {
        if (reader->can_read(extension)) return reader.get();
    }
    return nullptr;
}

vector<IDocumentReader*> DocumentReaderRegistry::all_readers() const {
    vector<IDocumentReader*> result;
    result.reserve(readers_.size());
    for (const auto& reader : readers_) result.push_back(reader.get());
    return result;
}

} // namespace convertor
