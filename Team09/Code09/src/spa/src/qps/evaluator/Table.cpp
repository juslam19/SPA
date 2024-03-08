#include "Table.h"

#include <unordered_set>

Table::Table(std::vector<Synonym> headers, std::vector<ColumnData> columns) : headers(headers) {
    if (columns.size() == 0) {
        return;
    }

    if (headers.size() != columns.size()) {
        throw "Number of columns and headers are not equal.";
    }

    for (int i = 0; i < columns.size(); i++) {
        if (columns[i].size() != columns[0].size()) {
            throw "Number of rows in columns are not equal.";
        }
    }

    for (int rowNo = 0; rowNo < columns[0].size(); rowNo++) {
        Row row;
        for (int colNo = 0; colNo < columns.size(); colNo++) {
            SynonymValue columnHeaderValue = headers[colNo].getValue();
            RowEntry rowEntryForColumn = columns[colNo][rowNo];
            row[columnHeaderValue] = rowEntryForColumn;
        }
        rows.push_back(row);
    }
}

Table::Table(std::vector<Synonym> headers, std::vector<Row> rows) : headers(headers), rows(rows) {
    for (int i = 0; i < rows.size(); i++) {
        if (rows[i].size() != headers.size()) {
            throw "Number of columns and headers are not equal.";
        }
    }
}

Table Table::join(const Table& other) const {
    if (this->isSentinel) {
        return other;
    }

    if (other.isSentinel) {
        return *this;
    }

    if (isEmpty() || other.isEmpty()) {
        std::vector<Row> emptyRows{};
        return Table{mergeHeaders(headers, other.headers), emptyRows};  // return empty table
    }

    std::vector<Synonym> commonHeaders{getCommonHeaders(other)};
    if (commonHeaders.empty()) {
        return cartesianProduct(other);
    }

    std::vector<Synonym> newHeaders{mergeHeaders(headers, other.headers)};

    // ai-gen start(copilot, 1, e)
    // prompt: find the rows that have the same values for the common headers
    std::vector<Row> newRows{};
    for (Row row : rows) {
        for (Row otherRow : other.rows) {
            if (areJoinableRows(row, otherRow, commonHeaders)) {
                newRows.push_back(combineRows(row, otherRow, other.headers));
            }
        }
    }
    // ai-gen end

    return Table{newHeaders, newRows};
}

bool Table::areJoinableRows(const Row& row, const Row& otherRow, const std::vector<Synonym>& commonHeaders) {
    for (Synonym header : commonHeaders) {
        if (row.at(header.getValue()) != otherRow.at(header.getValue())) {
            return false;
        }
    }
    return true;
}

Row Table::combineRows(const Row& row, const Row& otherRow, const std::vector<Synonym>& otherHeaders) {
    Row newRow{row};
    for (Synonym header : otherHeaders) {
        SynonymValue headerValue = header.getValue();
        newRow[headerValue] = otherRow.at(headerValue);
    }
    return newRow;
}

// ai-gen start(copilot, 0, e)
// prompt: create cross product of two tables
Table Table::cartesianProduct(const Table& other) const {
    std::vector<Synonym> newHeaders{mergeHeaders(headers, other.headers)};

    std::vector<Row> newRows{};
    for (Row row : rows) {
        for (Row otherRow : other.rows) {
            Row newRow{row};
            for (Synonym header : other.headers) {
                SynonymValue headerValue = header.getValue();
                newRow[headerValue] = otherRow.at(headerValue);
            }
            newRows.push_back(newRow);
        }
    }

    return Table{newHeaders, newRows};
}
// ai-gen ended

// ai-gen start(copilot, 0, e)
// prompt: using copilot
std::vector<Synonym> Table::getCommonHeaders(const Table& other) const {
    std::vector<Synonym> commonHeaders{};
    for (Synonym header : headers) {
        if (other.containsHeader(header)) {
            commonHeaders.push_back(header);
        }
    }
    return commonHeaders;
}
// ai-gen end

std::vector<Synonym> Table::mergeHeaders(const std::vector<Synonym>& firstHeaders,
                                         const std::vector<Synonym>& secondHeaders) {
    std::vector<Synonym> newHeaders{firstHeaders};
    std::unordered_set<SynonymValue> seen{};

    for (Synonym header : firstHeaders) {
        seen.insert(header.getValue());
    }

    for (Synonym header : secondHeaders) {
        if (seen.find(header.getValue()) == seen.end()) {
            newHeaders.push_back(header);
            seen.insert(header.getValue());
        }
    }
    return newHeaders;
}

// ai-gen start(copilot, 0, e)
// prompt: using copilot
bool Table::containsHeader(const Synonym& qe) const {
    for (Synonym header : headers) {
        if (header == qe) {
            return true;
        }
    }
    return false;
}
// ai-gen end

bool Table::headersIsSupersetOf(const std::vector<Synonym>& synonyms) const {
    for (Synonym synonym : synonyms) {
        if (!containsHeader(synonym)) {
            return false;
        }
    }
    return true;
}

bool Table::isEmpty() const {
    return !headers.empty() && rows.empty();
}

int Table::getHeaderIndex(const Synonym& qe) const {
    for (int i = 0; i < headers.size(); i++) {
        if (headers[i] == qe) {
            return i;
        }
    }
    return -1;
}

std::vector<Synonym> Table::getHeaders() const {
    return headers;
}

bool Table::operator==(const Table& other) const {
    if (this->headers.size() != other.headers.size() || this->rows.size() != other.rows.size()) {
        return false;
    }

    for (const Synonym& header : headers) {
        if (!other.containsHeader(header)) {
            return false;
        }
    }

    // For each row in this table, build its string representation and store it in a set
    std::unordered_set<std::string> rowStrings{};
    for (const Row& row : rows) {
        std::string rowString{};
        for (const Synonym& header : headers) {
            rowString += row.at(header.getValue()) + ",";
        }
        rowStrings.insert(rowString);
    }

    // For each row in the other table, build its string representation and check if it exists in the set
    for (const Row& row : other.rows) {
        std::string rowString{};
        // We use this' headers because we want to ensure the order of the columns is the same.
        for (const Synonym& header : headers) {
            rowString += row.at(header.getValue()) + ",";
        }

        if (auto it = rowStrings.find(rowString); it != rowStrings.end()) {
            rowStrings.erase(it);
        } else {
            return false;
        }
    }

    return true;
}

std::vector<Row> Table::getRows() const {
    return rows;
}
