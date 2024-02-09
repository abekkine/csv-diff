#ifndef CSV_STATS_HPP
#define CSV_STATS_HPP

#include "CsvColumn.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

class CsvStats {
private:
    const double kEps;
    std::string name_;
    CsvColumnPtr ref_;
    CsvColumnPtr data_;

    double min_diff_abs_;
    double max_diff_abs_;
    double mean_diff_;
    double mean_diff_abs_;
    double sd_diff_;
    double sd_diff_abs_;
    double var_diff_;
    double var_diff_abs_;

    bool ready_ = false;

public:
    CsvStats(const double& eps)
    : kEps(eps)
    {}
    ~CsvStats() {}
    const bool Ready() const {
        return ready_;
    }
    void SetColumnName(const std::string& name) {
        name_ = name;
    }
    void SetReferenceColumn(CsvColumnPtr ref) {
        ref_ = ref;
    }
    void SetDataColumn(CsvColumnPtr data) {
        data_ = data;
    }
    void GetMinMax(double& min, double& max) const {
        min = min_diff_abs_;
        max = max_diff_abs_;
    }
    void GetStat(double& mean, double& sd, double& var) const {
        mean = mean_diff_;
        sd = sd_diff_;
        var = var_diff_;
    }
    void GetStatAbs(double& mean, double& sd, double& var) const {
        mean = mean_diff_abs_;
        sd = sd_diff_abs_;
        var = var_diff_abs_;
    }
    const std::string& GetName() const {
        return name_;
    }
    void Calculate() {
        if (ref_ && data_) {
            // Calculate statistics for a single column.
            size_t ref_size = ref_->GetSize();
            size_t data_size = data_->GetSize();
            size_t size;
            // Calculate data size to use. Use the smaller one if they differ.
            if (ref_size != data_size) {
                size = std::min(ref_size, data_size);
            } else {
                size = ref_size;
            }
            // Reset statistics variables for calculations.
            double sum = 0.0, sum_abs = 0.0;
            double sum_sq = 0.0, sum_abs_sq = 0.0;
            min_diff_abs_ = 1e30;
            max_diff_abs_ = -1e30;
            for (size_t i = 0; i < size; ++i) {
                auto ref_value = ref_->GetValue(i);
                auto data_value = data_->GetValue(i);
                // Calculate difference between reference and data values.
                double diff = ref_value - data_value;
                // Calculate absolute difference.
                double diff_abs = std::fabs(diff);
                // If difference is smaller than epsilon, assume it is zero.
                if (diff_abs < kEps) {
                    diff_abs = 0.0;
                    diff = 0.0;
                }
                // Find mix/max for difference (absolute) values.
                if (diff_abs < min_diff_abs_) {
                    min_diff_abs_ = diff_abs;
                }
                if (diff_abs > max_diff_abs_) {
                    max_diff_abs_ = diff_abs;
                }
                // Sums for mean values.
                sum += diff;
                sum_abs += diff_abs;
                // Square sums for standard deviation values.
                sum_sq += diff * diff;
                sum_abs_sq += diff_abs * diff_abs;
            }
            // Calculate mean.
            mean_diff_ = sum / size;
            // Calculate variance.
            var_diff_ = sum_sq / size - mean_diff_ * mean_diff_;
            // Calculate mean (absolute).
            mean_diff_abs_ = sum_abs / size;
            // Calculate variance (absolute).
            var_diff_abs_ = sum_abs_sq / size - mean_diff_abs_ * mean_diff_abs_;
            // Calculate standard deviation.
            sd_diff_ = std::sqrt(var_diff_);
            // Calculate standard deviation (absolute).
            sd_diff_abs_ = std::sqrt(var_diff_abs_);

            ready_ = true;
        } else {
            std::cerr << "CsvStats: reference or data column not set\n";
            ready_ = false;
        }
    }
};

typedef std::shared_ptr<CsvStats> CsvStatPtr;

#endif // CSV_STATS_HPP
