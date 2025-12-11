// ProjectTwo.cpp
// Author: Andres Cepeda
// Description: ABCU Course Planner program. This program loads course data
// from a file into a binary search tree, prints a sorted course list, and
// shows information for an individual course, including its prerequisites.

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// -----------------------------
// Data structures
// -----------------------------

// This struct holds the information for one course.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// This struct is a node in the binary search tree.
struct TreeNode {
    Course courseData;
    TreeNode* leftChild;
    TreeNode* rightChild;

    TreeNode(const Course& course)
        : courseData(course), leftChild(nullptr), rightChild(nullptr) {}
};

// This class stores Course objects in a binary search tree ordered
// by course number so they can be printed in alphanumeric order.
class CourseBST {
public:
    CourseBST() : root(nullptr) {}

    // Insert a course into the tree.
    void insert(const Course& newCourse) {
        insertHelper(root, newCourse);
    }

    // Search for a course by course number.
    Course* search(const string& targetNumber) {
        return searchHelper(root, targetNumber);
    }

    // Print all courses in alphanumeric order.
    void printInOrder() const {
        if (!root) {
            cout << "No courses loaded." << endl;
            return;
        }
        inOrderHelper(root);
    }

    // Clear all nodes from the tree.
    void clear() {
        clearHelper(root);
        root = nullptr;
    }

private:
    TreeNode* root;

    // Helper function to insert a course into the tree.
    void insertHelper(TreeNode*& node, const Course& newCourse) {
        if (node == nullptr) {
            node = new TreeNode(newCourse);
            return;
        }

        if (newCourse.courseNumber < node->courseData.courseNumber) {
            insertHelper(node->leftChild, newCourse);
        }
        else if (newCourse.courseNumber > node->courseData.courseNumber) {
            insertHelper(node->rightChild, newCourse);
        }
        else {
            // If the course already exists, update its data.
            node->courseData.courseTitle = newCourse.courseTitle;
            node->courseData.prerequisites = newCourse.prerequisites;
        }
    }

    // Helper function to search for a course in the tree.
    Course* searchHelper(TreeNode* node, const string& targetNumber) {
        if (node == nullptr) {
            return nullptr;
        }

        if (targetNumber == node->courseData.courseNumber) {
            return &(node->courseData);
        }
        else if (targetNumber < node->courseData.courseNumber) {
            return searchHelper(node->leftChild, targetNumber);
        }
        else {
            return searchHelper(node->rightChild, targetNumber);
        }
    }

    // Helper function to print the tree in order.
    void inOrderHelper(TreeNode* node) const {
        if (node == nullptr) {
            return;
        }

        inOrderHelper(node->leftChild);
        cout << node->courseData.courseNumber << ", "
             << node->courseData.courseTitle << endl;
        inOrderHelper(node->rightChild);
    }

    // Helper function to delete all nodes in the tree.
    void clearHelper(TreeNode* node) {
        if (node == nullptr) {
            return;
        }
        clearHelper(node->leftChild);
        clearHelper(node->rightChild);
        delete node;
    }
};

// -----------------------------
// Utility functions
// -----------------------------

// Trim whitespace from the beginning and end of a string.
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Split a line into tokens using a single-character delimiter.
vector<string> split(const string& line, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

// Convert a string to uppercase. This helps keep course lookups consistent.
string toUpper(const string& s) {
    string result = s;
    for (char& c : result) {
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    }
    return result;
}

// -----------------------------
// File loading
// -----------------------------

// Load course data from a CSV file and store it in the tree.
// Returns true if the load is successful.
bool loadCoursesFromFile(const string& fileName, CourseBST& tree) {
    ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        cout << "Error opening file: " << fileName << endl;
        return false;
    }

    // Clear any existing data before loading new courses.
    tree.clear();

    string line;
    int lineNumber = 0;

    while (getline(inputFile, line)) {
        lineNumber++;

        // Skip empty lines so they do not cause errors.
        if (trim(line).empty()) {
            continue;
        }

        vector<string> tokens = split(line, ',');

        // Each line should have at least a course number and a course title.
        if (tokens.size() < 2) {
            cout << "File format error on line " << lineNumber
                 << ": fewer than two fields." << endl;
            cout << "Offending line: " << line << endl;
            // Skip this line and continue with the rest.
            continue;
        }

        Course course;
        course.courseNumber = trim(tokens[0]);
        course.courseTitle = trim(tokens[1]);

        // Any remaining tokens are prerequisites.
        for (size_t i = 2; i < tokens.size(); ++i) {
            string prereqId = trim(tokens[i]);
            if (!prereqId.empty()) {
                course.prerequisites.push_back(prereqId);
            }
        }

        // Only insert the course if it has both a number and a title.
        if (!course.courseNumber.empty() && !course.courseTitle.empty()) {
            tree.insert(course);
        }
        else {
            cout << "File format warning on line " << lineNumber
                 << ": missing course number or title." << endl;
        }
    }

    inputFile.close();
    cout << "Courses successfully loaded from file: " << fileName << endl;
    return true;
}

// -----------------------------
// Printing functions
// -----------------------------

// Print detailed information for one course, including its prerequisites.
void printCourseInformation(CourseBST& tree, const string& targetNumber) {
    string searchNumber = toUpper(targetNumber);
    Course* found = tree.search(searchNumber);

    if (found == nullptr) {
        cout << "Course " << searchNumber << " not found." << endl;
        return;
    }

    cout << endl;
    cout << found->courseNumber << ", " << found->courseTitle << endl;

    if (found->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites:" << endl;

        // For each prerequisite, try to print its number and title.
        for (const string& prereqIdRaw : found->prerequisites) {
            string prereqId = toUpper(prereqIdRaw);
            Course* prereqCourse = tree.search(prereqId);

            if (prereqCourse != nullptr) {
                cout << "  " << prereqCourse->courseNumber
                     << ", " << prereqCourse->courseTitle << endl;
            }
            else {
                // If the prerequisite is not in the tree, at least show its ID.
                cout << "  " << prereqId << " (course not found in data)" << endl;
            }
        }
    }
}

// -----------------------------
// Menu and main program
// -----------------------------

// Print the main menu for the user.
void printMenu() {
    cout << endl;
    cout << "*******************************" << endl;
    cout << "Welcome to the ABCU Course Planner" << endl;
    cout << "*******************************" << endl;
    cout << "1. Load Data Structure" << endl;
    cout << "2. Print Course List" << endl;
    cout << "3. Print Course" << endl;
    cout << "9. Exit" << endl;
    cout << "Please enter your choice: ";
}

int main() {
    CourseBST courseTree;
    bool dataLoaded = false;
    string fileName;
    string userChoice;

    // Loop until the user chooses to exit.
    while (true) {
        printMenu();
        getline(cin, userChoice);

        if (userChoice == "1") {
            cout << "Enter course data file name: ";
            getline(cin, fileName);

            if (fileName.empty()) {
                cout << "File name cannot be empty." << endl;
                continue;
            }

            dataLoaded = loadCoursesFromFile(fileName, courseTree);
        }
        else if (userChoice == "2") {
            if (!dataLoaded) {
                cout << "Please load the data structure first (option 1)." << endl;
            }
            else {
                cout << endl;
                cout << "Here is the list of courses:" << endl;
                courseTree.printInOrder();
            }
        }
        else if (userChoice == "3") {
            if (!dataLoaded) {
                cout << "Please load the data structure first (option 1)." << endl;
            }
            else {
                string searchNumber;
                cout << "Please enter the course number (for example, CS200): ";
                getline(cin, searchNumber);

                if (searchNumber.empty()) {
                    cout << "Course number cannot be empty." << endl;
                }
                else {
                    printCourseInformation(courseTree, searchNumber);
                }
            }
        }
        else if (userChoice == "9") {
            cout << "Thank you for using the ABCU Course Planner. Goodbye!" << endl;
            break;
        }
        else {
            // Handle any menu choices that are not valid.
            cout << "Invalid choice. Please enter 1, 2, 3, or 9." << endl;
        }
    }

    return 0;
}
