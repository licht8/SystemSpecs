#pragma once

// Class for reading and printing the contents of a file
class FileReader {
private:
    std::string filename;  // File to be read

public:
    // Constructor to set the filename
    FileReader(const std::string& filename) : filename(filename) {}

    // Read the file and print its contents
    void readFileAndPrint() {
        // Open the file for reading
        std::ifstream inputFile(filename);

        // Check if the file is opened successfully
        if (!inputFile.is_open()) {
            std::cerr << "Can't read the file :( " << filename << std::endl;
            return;
        }

        std::string line;

        // Read and print each line of the file
        while (std::getline(inputFile, line)) {
            // Set console color to green
            SetConsoleColor(FOREGROUND_GREEN);
            std::cout << line << std::endl;
        }

        // Close the file
        inputFile.close();
    }
};
