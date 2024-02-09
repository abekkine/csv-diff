#ifndef CSV_FILE_HPP
#define CSV_FILE_HPP

#include "CsvColumn.hpp"
#include "TextUtility.hpp"

#include <string>
#include <memory>
#include <fstream>
#include <vector>
#include <functional>

typedef std::vector<std::string> ColumnNameList;

class CsvFile {
private:
    std::string filename_;
    std::ifstream f_csv_;
    std::string csv_header_;
    std::vector<std::string> csv_lines_;
    std::vector<CsvColumnPtr> csv_columns_;

public:
    CsvFile(const std::string& filename)
    : filename_(filename)
    {}
    ~CsvFile() {}
    size_t Parse(const bool dump=false) {
        Open();
        // Read CSV lines into column objects.
        auto line_count = ReadLines();
        Close();
        if (dump) {
            // If requested, dump column contents.
            DumpColumns();
        }
        return line_count;
    }
    CsvColumnPtr GetColumn(const std::string& name) {
        for (auto & col : csv_columns_) {
            if (col->GetName() == name) {
                return col;
            }
        }
        return nullptr;
    }
    void DumpFilename() {
        std::cout << "CsvFile: " << filename_ << "\n";
    }
    ColumnNameList GetColumnNames() {
        ColumnNameList result;
        for (auto & col : csv_columns_) {
            result.push_back(col->GetName());
        }
        return result;
    }

private:
    void Open() {
        f_csv_ = std::ifstream(filename_);
    }
    size_t ReadLines() {
        size_t line_count = 0;
        if (f_csv_.good()) {
            // If file is good, read the lines.
            std::string line;
            bool header_once = true;
            while (std::getline(f_csv_, line)) {
                // Strip whitespaced (space, \t, \r) in the line.
                auto strip_line = TextUtility::Strip(line);
                if (strip_line.empty()) {
                    // Skip empty lines.
                    continue;
                }
                if (header_once) {
                    // First line is the header.
                    header_once = false;
                    // Create CsvColumn objects from the header line.
                    IterateValues(strip_line, [this](const std::string& value){
                        csv_columns_.push_back(std::make_shared<CsvColumn>(value));
                    });
                } else {
                    size_t index = 0;
                    // Other lines are data lines; add values to the corresponding columns.
                    IterateValues(strip_line, [this, &index](const std::string& value){
                        csv_columns_[index]->AddValue(std::stod(value));
                        index++;
                    });
                }
            }
            line_count = csv_columns_[0]->GetSize();
        }
        return line_count;
    }
    void Close() {
        f_csv_.close();
    }
    void DumpColumns() {
        // Dump all values for all columns.
        for (auto & col : csv_columns_) {
            col->Dump();
        }
    }
    void IterateValues(const std::string& line, std::function<void(const std::string& value)> fun) {
        // Iterate over the comma separated values in a line.
        std::string value;
        for (auto c : line) {
            if (c == ',') {
                if (value != "") {
                    fun(value);
                    value.clear();
                }
            } else {
                value += c;
            }
        }
        // Omit trailing comma.
        if (value != "") {
            fun(value);
        }
    }
};

typedef std::shared_ptr<CsvFile> CsvFilePtr;

#endif // CSV_FILE_HPP
