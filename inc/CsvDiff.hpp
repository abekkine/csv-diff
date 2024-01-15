#ifndef CSV_DIFF_HPP
#define CSV_DIFF_HPP

#include "CsvFile.hpp"
#include "CsvStats.hpp"
#include "CsvReport.hpp"
#include "TextUtility.hpp"

#include <vector>
#include <string>
#include <limits>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <map>

typedef std::map<std::string, std::string> ColumnNameMap;

class CsvDiff {
private:
    double eps_;
    bool match_;
    bool hide_same_;
    bool hide_nan_;
    bool use_data_names_;
    bool group_by_result_;
    CsvFilePtr ref_;
    CsvFilePtr data_;
    std::unique_ptr<CsvReport> report_;

public:
    CsvDiff() {
        eps_ = 1e-8;
        match_ = false;
        hide_same_ = false;
        hide_nan_ = false;
        group_by_result_ = false;
    }
    ~CsvDiff() {}
    void SetEpsilon(const double& eps) {
        eps_ = eps;
    }
    void SetHideSame(const bool hide) {
        hide_same_ = hide;
    }
    void SetHideNan(const bool hide) {
        hide_nan_ = hide;
    }
    void SetMatchColumns(const bool match) {
        match_ = match;
    }
    void SetUseDataNames(const bool use) {
        use_data_names_ = use;
    }
    void SetGroupByResult(const bool group) {
        group_by_result_ = group;
    }
    void SetRefFile(CsvFilePtr ref) {
        ref_ = ref;
    }
    void SetDataFile(CsvFilePtr data) {
        data_ = data;
    }
    void Run() {
        if (ref_) {
            // Parse if reference file is set.
            ref_->Parse();
        } else {
            std::cerr << "No reference file set!\n";
        }
        if (data_) {
            // Parse if data file is set.
            data_->Parse();
        } else {
            std::cerr << "No data file set!\n";
        }
        report_ = std::make_unique<CsvReport>();
        // Set epsilon value for report.
        report_->SetEpsilon(eps_);
        report_->SetHideSame(hide_same_);
        report_->SetHideNan(hide_nan_);
        report_->SetGroupByResult(group_by_result_);
        report_->Init();

        // Get names for columns with matching names in both input files.
        ColumnNameList column_names;
        if (match_) {
            // TODO : implement
            column_names = MatchingColumns();
        } else {
            column_names = CommonColumns();
        }
        for (auto & name : column_names) {
            auto ref_column = ref_->GetColumn(name);
            auto data_column = data_->GetColumn(name);
            if (ref_column && data_column) {
                // If both columns are found, calculate statistics into a report line.
                auto stats = GetStats(ref_column, data_column);
                AddVariableStats(stats);
            } else {
                std::cerr << "Column: " << name << " not found!\n";
            }
        }
    }
    ColumnNameList CommonColumns() {
        // Return common names from both input files.
        auto ref_column_names = ref_->GetColumnNames();
        auto data_column_names = data_->GetColumnNames();
        std::sort(ref_column_names.begin(), ref_column_names.end());
        std::sort(data_column_names.begin(), data_column_names.end());
        ColumnNameList common_names;
        std::set_intersection(ref_column_names.begin(), ref_column_names.end()
            , data_column_names.begin(), data_column_names.end()
            , back_inserter(common_names));
    
        ReportNonMatchingColumns(common_names, ref_column_names, data_column_names);
        return common_names;
    }
    ColumnNameList MatchingColumns() {
        // TODO : Consider cases where levenshtein distance is same for multiple columns.
        // Return auto-matched names from both input files.
        auto ref_column_names = ref_->GetColumnNames();
        auto data_column_names = data_->GetColumnNames();
        ColumnNameMap strip_ref, strip_data;
        ColumnNameList list_ref, list_data;
        for (auto & name : ref_column_names) {
            auto stripped = TextUtility::Strip(name, "[]");
            strip_ref[stripped] = name;
            list_ref.push_back(stripped);
        }
        for (auto & name : data_column_names) {
            auto stripped = TextUtility::Strip(name, "[]");
            strip_data[stripped] = name;
            list_data.push_back(stripped);
        }
        auto matching_names = FindMatchingColumns(list_ref, list_data);
        for (auto & match : matching_names) {
            auto data_column = data_->GetColumn(strip_data[match.second]);
            data_column->SetName(strip_ref[match.first]);
        }
        return ref_column_names;
    }

    CsvStatPtr GetStats(CsvColumnPtr ref_column, CsvColumnPtr data_column) {
        // Calculate statistics for a single column.
        auto stats = std::make_unique<CsvStats>(eps_);
        if (use_data_names_) {
            stats->SetColumnName(data_column->GetNameHistory());
        } else {
            stats->SetColumnName(ref_column->GetName());
        }
        stats->SetReferenceColumn(ref_column);
        stats->SetDataColumn(data_column);
        stats->Calculate();
        return stats;
    }
    void AddVariableStats(CsvStatPtr stats) {
        if (! stats->Ready()) {
            return;
        }

        report_->SetColumnName(stats->GetName());

        double max_diff, min_diff;
        stats->GetMinMax(min_diff, max_diff);
        report_->SetMinMaxValues(min_diff, max_diff);

        double mean, sd, var;
        stats->GetStat(mean, sd, var);
        report_->SetNormalValues(mean, sd, var);

        double mean_abs, sd_abs, var_abs;
        stats->GetStatAbs(mean_abs, sd_abs, var_abs);
        report_->SetAbsValues(mean_abs, sd_abs, var_abs);

        report_->WriteVariableStats();
    }
    ColumnNameMap FindMatchingColumns(const ColumnNameList& ref, const ColumnNameList& data) {
        ColumnNameMap result;
        for (auto & r_name : ref) {
            int d_min = 1000;
            int d;
            std::string d_match = "";
            for (auto & d_name : data) {
                d = TextUtility::LevenshteinDistance(r_name, d_name);
                if (d < d_min) {
                    d_min = d;
                    d_match = d_name;
                }
            }
            result[r_name] = d_match;
        }
        return result;
    }
    void ReportNonMatchingColumns(const ColumnNameList& common, const ColumnNameList& ref, const ColumnNameList& data) {
        ColumnNameList work_ref = ref;
        ColumnNameList work_data = data;
        for (auto & name : common) {
            // Remove common names from the lists.
            work_ref.erase(std::remove(work_ref.begin(), work_ref.end(), name), work_ref.end());
            work_data.erase(std::remove(work_data.begin(), work_data.end(), name), work_data.end());
        }
        auto matching_names = FindMatchingColumns(work_ref, work_data);
        for (auto & match : matching_names) {
            report_->ColumnMatchingSuggestion(match.first, match.second);
        }
    }
    void ShowReport() {
        report_->Show();
    }
};

#endif // CSV_DIFF_HPP
