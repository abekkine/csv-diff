#ifndef CSV_REPORT_HPP
#define CSV_REPORT_HPP

#include <vector>
#include <string>
#include <limits>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdio>

class CsvReport {
private:
    bool hide_same_;
    bool hide_nan_;
    bool group_by_result_;
    bool brief_;
    size_t same_count_;
    size_t nan_count_;
    size_t column_count_;
    size_t ref_line_count_;
    size_t data_line_count_;
    double eps_ = std::numeric_limits<double>::quiet_NaN();
    std::vector<std::string> lines_;
    std::vector<std::string> var_lines_;
    std::vector<std::string> matching_suggestions_;

    double max_diff_, min_diff_;
    double mean_, sd_, var_;
    double mean_abs_, sd_abs_, var_abs_;
    std::string column_name_;

public:
    CsvReport() {
        hide_same_ = false;
        hide_nan_ = false;
        group_by_result_ = false;
        brief_ = false;
        same_count_ = 0;
        nan_count_ = 0;
    }
    ~CsvReport() {}
    void SetEpsilon(const double& eps) {
        eps_ = eps;
    }
    void SetBriefMode() {
        brief_ = true;
        hide_same_ = true;
        hide_nan_ = true;
    }
    void SetHideSame(const bool hide) {
        hide_same_ = hide;
    }
    void SetHideNan(const bool hide) {
        hide_nan_ = hide;
    }
    void SetGroupByResult(const bool group) {
        group_by_result_ = group;
    }
    void SetColumnCount(const size_t count) {
        column_count_ = count;
    }
    void SetLineCounts(const size_t c_ref, const size_t c_data) {
        ref_line_count_ = c_ref;
        data_line_count_ = c_data;
    }
    void Init() {
        lines_.clear();
        lines_.push_back(GetEpsLine());
        lines_.push_back(GetTableHeader());
    }
    void SetColumnName(const std::string& name) {
        column_name_ = name;
    }
    void SetMinMaxValues(const double& min, const double& max) {
        max_diff_ = max;
        min_diff_ = min;
    }
    void SetNormalValues(const double& mean, const double& sd, const double& var) {
        mean_ = mean;
        sd_ = sd;
        var_ = var;
    }
    void SetAbsValues(const double& mean, const double& sd, const double& var) {
        mean_abs_ = mean;
        sd_abs_ = sd;
        var_abs_ = var;
    }
    void ColumnMatchingSuggestion(const std::string& refName, const std::string& dataName) {
        char buf[1024];
        snprintf(buf, sizeof(buf)
            , "%-30s --> %-30s"
            , refName.c_str()
            , dataName.c_str()
        );
        matching_suggestions_.push_back(std::string(buf));
    }
    void WriteVariableStats() {
        char buf[1024];
        if (std::fabs(mean_) <= std::numeric_limits<double>::epsilon() &&
            sd_ <= std::numeric_limits<double>::epsilon() &&
            mean_abs_ <= std::numeric_limits<double>::epsilon() &&
            sd_abs_ <= std::numeric_limits<double>::epsilon()
        ) {
            ++same_count_;
            if (hide_same_) {
                return;
            }
            // If values are all zero, report as "same".
            snprintf(buf, sizeof(buf)
                , "99:%-32s : Values are same"
                , column_name_.c_str()
            );
        }
        else if (std::isnan(mean_)) {
            // If values contain NaN, report as "NaN".
            ++nan_count_;
            if (hide_nan_) {
                return;
            }
            snprintf(buf, sizeof(buf)
                , "98:%-32s : NaN values in column!"
                , column_name_.c_str()
            );
        } else {
            // Otherwise, report the statistics.
            snprintf(buf, sizeof(buf)
                , "00:%-32s : %20g %20g %20g %20g %20g %20g"
                , column_name_.c_str()
                , max_diff_
                , min_diff_
                , mean_
                , sd_
                , mean_abs_
                , sd_abs_
            );
        }
        var_lines_.push_back(std::string(buf));
    }
    void Show() {
        if (brief_) {
            std::cout << "\n";
            std::cout << "NaN : " << nan_count_ << "/" << column_count_ << " ";
            std::cout << "Same : " << same_count_ << "/" << column_count_ << " ";
            std::cout << "@ eps = " << eps_ << "\n";
        } else {
            
            if (ref_line_count_ != data_line_count_) {
                std::cerr << "\n== Line count mismatch; Ref(" << ref_line_count_ << ") != Data(" << data_line_count_ << "). ";
                std::cerr << "Smaller one will be used for comparison. ==\n";
            }
            
            if (! matching_suggestions_.empty()) {
                std::cout << "\n== Non-matching reference columns /w suggested (--match) data columns ==\n";
                for (const auto& line : matching_suggestions_) {
                    std::cout << line << "\n";
                }
                std::cout << "\n";
            }

            if (group_by_result_) {
                std::sort(var_lines_.begin(), var_lines_.end());
            }
            for (const auto& line : var_lines_) {
                lines_.push_back(line.substr(3));
            }

            for (const auto& line : lines_) {
                std::cout << line << "\n";
            }
            std::cout << "\n";

            if (hide_nan_ && nan_count_ > 0) {
                std::cout << "Number of NaN columns: " << nan_count_ << "\n";
            }

            if (hide_same_ && same_count_ > 0) {
                std::cout << "Number of same columns: " << same_count_ << "\n";
            }
        }
    }

private:
    const std::string GetEpsLine() {
        char buf[256];
        sprintf(buf, "Epsilon = %g\n", eps_);
        return std::string(buf);
    }
    const std::string GetTableHeader() {
        // Return header line for the report.
        char buf[1024];
        snprintf(buf, sizeof(buf)
            , "%-32s : %20s %20s %20s %20s %20s %20s"
            , "Variable (ref)"
            , "Max"
            , "Min"
            , "Mean"
            , "SD"
            , "Mean (abs)"
            , "SD (abs)"
        );
        return std::string(buf);
    }
};

#endif // CSV_REPORT_HPP
