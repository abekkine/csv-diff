#include "CsvFile.hpp"
#include "CsvDiff.hpp"
#include "OptionParser.hpp"
#include "git.hpp"

#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    std::cout << "CsvDiff, Version: " << GitVersion::Describe() << '\n';

    // Parse command line options
    std::unique_ptr<OptionParser> opts;
    opts = std::make_unique<OptionParser>(argc, argv);

    // Two input files are required
    if (opts->NumParams() != 2) {
        std::cerr << "Two input parameters (files) required!\n";
        return 1;
    }
    // Default value for epsilon.
    double eps = 1e-8;
    // Or use the value from command line
    if (opts->HasValue("--eps")) {
        eps = opts->GetDouble("--eps");
    }

    bool match_columns = false;
    if (opts->HasFlag("--match")) {
        match_columns = true;
    }

    bool hide_same = false;
    if (opts->HasFlag("--hide-same")) {
        hide_same = true;
    }

    bool hide_nan = false;
    if (opts->HasFlag("--hide-nan")) {
        hide_nan = true;
    }

    bool use_data_names = false;
    if (opts->HasFlag("--use-data-names")) {
        use_data_names = true;
    }

    bool group_by_result = false;
    if (opts->HasFlag("--group")) {
        group_by_result = true;
    }

    // Read CsvFile instances from input files
    auto files = opts->GetParams();
    auto refFile = std::make_shared<CsvFile>(files[0]);
    auto dataFile = std::make_shared<CsvFile>(files[1]);

    // Compare the two files
    auto compare = std::make_unique<CsvDiff>();
    compare->SetEpsilon(eps);
    compare->SetHideSame(hide_same);
    compare->SetHideNan(hide_nan);
    compare->SetMatchColumns(match_columns);
    compare->SetUseDataNames(use_data_names);
    compare->SetGroupByResult(group_by_result);
    compare->SetRefFile(refFile);
    compare->SetDataFile(dataFile);
    compare->Run();
    // Show the report
    compare->ShowReport();

    return 0;
}