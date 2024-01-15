#ifndef CSV_COLUMN_HPP
#define CSV_COLUMN_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>

class CsvColumn {
private:
    // Csv header name
    std::string name_;
    std::string name_history_;
    // Csv column values
    std::vector<double> values_;

public:
    CsvColumn(const std::string& name)
    : name_(name)
    , name_history_(name)
    {}
    ~CsvColumn() {}
    void SetName(const std::string& name) {
        name_history_ = name_;
        name_ = name;
    }
    const std::string& GetName() const {
        return name_;
    }
    const std::string& GetNameHistory() const {
        return name_history_;
    }
    const size_t GetSize() const {
        return values_.size();
    }
    const double& GetValue(const size_t index) const {
        return values_[index];
    }
    void AddValue(const double& value) {
        // Add new value to the column.
        values_.push_back(value);
    }
    void Dump() {
        // Dump all values in the column, /w name at the top.
        std::cout << "Column: " << name_ << " [ ";
        for (auto& value : values_) {
            std::cout << value << " ";
        }
        std::cout << "]\n";
    }
};

typedef std::shared_ptr<CsvColumn> CsvColumnPtr;

#endif // CSV_COLUMN_HPP
