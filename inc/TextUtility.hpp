#ifndef TEXT_UTILITY_HPP
#define TEXT_UTILITY_HPP

#include <string>
#include <algorithm>

class TextUtility {
public:
    static const std::string Strip(const std::string& str, const std::string& chars=" \t\r") {     
        std::string result("");
        for(auto & c : str) {
            size_t found = chars.find_first_of(c);
            if (found == std::string::npos) {
                result += c;
            }
        }
        return result;
    }
    static const int LevenshteinDistance(const std::string& s1, const std::string& s2) {
        // Calculate Levenshtein distance between two strings.
        int l1 = s1.length();
        int l2 = s2.length();
        std::vector<std::vector<int>> matrix(l1 + 1, std::vector<int>(l2 + 1));
        for (int i = 0; i <= l1; ++i) {
            matrix[i][0] = i;
        }
        for (int j = 0; j <= l2; ++j) {
            matrix[0][j] = j;
        }
        for (int i = 1; i <= l1; ++i) {
            for (int j = 1; j <= l2; ++j) {
                int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
                matrix[i][j] = std::min({matrix[i - 1][j] + 1, matrix[i][j - 1] + 1, matrix[i - 1][j - 1] + cost});
            }
        }
        return matrix[l1][l2];
    }
};

#endif // TEXT_UTILITY_HPP
