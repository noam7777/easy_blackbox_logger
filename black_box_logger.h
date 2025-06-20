#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>

class BlackBoxLogger {
public:
    // Constructor - creates output directory and opens a CSV file with an auto-generated name
    explicit BlackBoxLogger(const std::string& output_dir = "./output");

    // Insert a value for a specific field into the current row
    void insert(const std::string& field_name, double value);

    // Commit the current row to the file and start a new row
    void newRow();

    // Manually save the entire dataset to file (optional)
    void save();

    // Destructor - automatically saves if not already saved
    ~BlackBoxLogger();

private:
    std::string file_path;
    std::ofstream csv_file;
    std::vector<std::string> headers;
    std::map<std::string, std::vector<double>> columns;
    std::map<std::string, double> current_row_values;
    size_t current_row_index = 0;
    std::string output_directory;
    bool saved = false;

    // Internal helpers
    std::string generateFilename();
    void createOutputDirectoryIfNeeded();
    void writeHeaderIfNeeded();
    void flushRowToDisk();
};