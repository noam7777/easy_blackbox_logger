#include "black_box_logger.h"
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <regex>

namespace fs = std::filesystem;

BlackBoxLogger::BlackBoxLogger(const std::string& output_dir)
    : output_directory(output_dir)
{
    createOutputDirectoryIfNeeded();
    file_path = output_directory + "/" + generateFilename();
    csv_file.open(file_path);
}

BlackBoxLogger::~BlackBoxLogger() {
    if (!saved) {
        save();
    }
    if (csv_file.is_open()) {
        csv_file.close();
    }
}

void BlackBoxLogger::insert(const std::string& field_name, double value) {
    if (columns.find(field_name) == columns.end()) {
        headers.push_back(field_name);
        columns[field_name] = std::vector<double>(current_row_index, 0.0);
    }
    current_row_values[field_name] = value;
}

void BlackBoxLogger::newRow() {
    for (const auto& header : headers) {
        if (current_row_values.find(header) != current_row_values.end()) {
            columns[header].push_back(current_row_values[header]);
        } else if (!columns[header].empty()) {
            columns[header].push_back(columns[header].back());
        } else {
            columns[header].push_back(0.0);
        }
    }

    if (current_row_index == 0) {
        writeHeaderIfNeeded();
    }

    flushRowToDisk();
    current_row_values.clear();
    current_row_index++;
}

void BlackBoxLogger::save() {
    if (!csv_file.is_open()) return;

    csv_file.seekp(0);
    writeHeaderIfNeeded();

    for (size_t i = 0; i < current_row_index; ++i) {
        for (size_t j = 0; j < headers.size(); ++j) {
            csv_file << columns[headers[j]][i];
            if (j + 1 < headers.size()) csv_file << ",";
        }
        csv_file << "\n";
    }

    csv_file.flush();
    saved = true;
}

std::string BlackBoxLogger::generateFilename() {
    size_t max_test_number = 0;
    std::regex filename_pattern(R"(test(\d+)-.*\.csv)");

    for (const auto& entry : fs::directory_iterator(output_directory)) {
        if (!entry.is_regular_file()) continue;

        std::smatch match;
        std::string name = entry.path().filename().string();
        if (std::regex_match(name, match, filename_pattern)) {
            size_t num = std::stoul(match[1]);
            if (num > max_test_number) {
                max_test_number = num;
            }
        }
    }

    max_test_number++;

    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << "test" << max_test_number << "-"
        << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".csv";

    return oss.str();
}

void BlackBoxLogger::createOutputDirectoryIfNeeded() {
    if (!fs::exists(output_directory)) {
        fs::create_directories(output_directory);
    }
}

void BlackBoxLogger::writeHeaderIfNeeded() {
    for (size_t i = 0; i < headers.size(); ++i) {
        csv_file << headers[i];
        if (i + 1 < headers.size()) csv_file << ",";
    }
    csv_file << "\n";
    csv_file.flush();
}

void BlackBoxLogger::flushRowToDisk() {
    for (size_t i = 0; i < headers.size(); ++i) {
        csv_file << columns[headers[i]].back();
        if (i + 1 < headers.size()) csv_file << ",";
    }
    csv_file << "\n";
    csv_file.flush();
}