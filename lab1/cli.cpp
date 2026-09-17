/**
 * @file cli.cpp
 * @brief Interactive command-line interface for the Set and RubikCube classes.
 *
 * The program shows a welcome screen and lets the user choose one of the two
 * libraries. Inside each library a small menu exposes the public API.
 */

#include <cctype>
#include <iostream>
#include <string>

#include "rubik/rubik.h"
#include "set/set.h"

namespace {

/**
 * @brief Removes leading and trailing whitespace from a string.
 * @param s The string to trim.
 * @return The trimmed string.
 */
std::string trim(const std::string& s) {
    const std::string whitespace = " \t\n\r\f\v";
    const std::size_t start = s.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    const std::size_t end = s.find_last_not_of(whitespace);
    return s.substr(start, end - start + 1);
}

/**
 * @brief Reads one line from standard input.
 * @param out Receives the trimmed line on success.
 * @return False on end-of-input (EOF), true otherwise.
 */
bool read_line(std::string& out) {
    std::cout << "> ";
    if (!std::getline(std::cin, out)) {
        return false;
    }
    out = trim(out);
    return true;
}

/**
 * @brief Parses an integer from a string.
 * @param s The string to parse.
 * @return The parsed value, or -1 if the string is not an integer.
 */
int parse_int(const std::string& s) {
    if (s.empty()) {
        return -1;
    }
    try {
        return std::stoi(s);
    } catch (...) {
        return -1;
    }
}

/**
 * @brief Converts the user's line into lowercase for case-insensitive input.
 * @param s The string to convert.
 * @return The lowercased string.
 */
std::string to_lower(const std::string& s) {
    std::string result = s;
    for (char& ch : result) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return result;
}

/**
 * @brief Reads a line and checks whether the user asked to quit.
 * @param out Receives the trimmed line on success.
 * @return True if the user wants to quit the whole program.
 */
bool read_quit_flag(std::string& out) {
    if (!read_line(out)) {
        return true;
    }
    const std::string lower = to_lower(out);
    return lower == "q" || lower == "quit";
}

}  // namespace

namespace setcli {

/**
 * @brief Prints the Set CLI menu.
 */
void print_menu() {
    std::cout << "\n--- Set CLI ---\n"
              << "1.  Print set\n"
              << "2.  Load set from string\n"
              << "3.  Insert element\n"
              << "4.  Remove element\n"
              << "5.  Check contains\n"
              << "6.  Cardinality\n"
              << "7.  Is empty\n"
              << "8.  Union with another set\n"
              << "9.  Intersection with another set\n"
              << "10. Difference with another set\n"
              << "11. Power set\n"
              << "0. Back to main menu\n"
              << "q. Quit\n";
}

/**
 * @brief Reads a set from the user (a string like "{a, b, {c}}").
 * @return The set, or an empty set if the user entered nothing.
 */
Set read_set() {
    std::string input;
    if (!read_line(input)) {
        return Set();
    }
    return Set(input);
}

/**
 * @brief Runs the interactive Set CLI.
 * @return False if the user quit the whole program, true to go back.
 */
bool run() {
    Set set;
    while (true) {
        std::cout << "\nCurrent set: " << set << '\n';
        print_menu();

        std::string line;
        if (read_quit_flag(line)) {
            return false;
        }
        if (line == "0") {
            return true;
        }

        switch (parse_int(line)) {
            case 1:
                std::cout << set << '\n';
                break;

            case 2:
                std::cout << "Enter set string, e.g. {a, b, {c}}:";
                set = read_set();
                std::cout << "Current set: " << set << '\n';
                break;

            case 3:
                std::cout << "Enter element to insert:";
                if (!read_line(line)) {
                    return false;
                }
                set.insert(line);
                std::cout << "Inserted '" << line << "'.\n";
                break;

            case 4:
                std::cout << "Enter element to remove:";
                if (!read_line(line)) {
                    return false;
                }
                set.remove(line);
                std::cout << "Removed '" << line << "'.\n";
                break;

            case 5:
                std::cout << "Enter element to check:";
                if (!read_line(line)) {
                    return false;
                }
                std::cout << (set.contains(line) ? "yes" : "no") << '\n';
                break;

            case 6:
                std::cout << "Cardinality: " << set.cardinality() << '\n';
                break;

            case 7:
                std::cout << (set.empty() ? "yes" : "no") << '\n';
                break;

            case 8:
                std::cout << "Enter the other set, e.g. {b, c, {d}}:";
                std::cout << (set + read_set()) << '\n';
                break;

            case 9:
                std::cout << "Enter the other set, e.g. {b, c, {d}}:";
                std::cout << (set * read_set()) << '\n';
                break;

            case 10:
                std::cout << "Enter the other set, e.g. {b, c, {d}}:";
                std::cout << (set - read_set()) << '\n';
                break;

            case 11: {
                const Set power = set.power_set();
                std::cout << "Power set: " << power << '\n';
                std::cout << "Cardinality: " << power.cardinality() << '\n';
                break;
            }

            default:
                std::cout << "Unknown option.\n";
                break;
        }
    }
}

}  // namespace setcli

namespace rubikcli {

/**
 * @brief Prints the Rubik's Cube CLI menu.
 */
void print_menu() {
    std::cout << "\n--- Rubik's Cube CLI ---\n"
              << "1. Print cube\n"
              << "2. Rotate face\n"
              << "3. Is solved\n"
              << "4. Randomize\n"
              << "5. Load from file\n"
              << "6. New cube from seed\n"
              << "0. Back to main menu\n"
              << "q. Quit\n";
}

/**
 * @brief Parses a face name (letter or full name) typed by the user.
 * @param s The user input, e.g. "f", "front", "U".
 * @param out Receives the face on success.
 * @return True if the input names a face.
 */
bool parse_face(const std::string& s, Face& out) {
    if (s.empty()) {
        return false;
    }
    const char ch = to_lower(s).front();
    switch (ch) {
        case 'u': out = Face::Up; return true;
        case 'd': out = Face::Down; return true;
        case 'l': out = Face::Left; return true;
        case 'r': out = Face::Right; return true;
        case 'f': out = Face::Front; return true;
        case 'b': out = Face::Back; return true;
        default: return false;
    }
}

/**
 * @brief Runs the interactive Rubik's Cube CLI.
 * @return False if the user quit the whole program, true to go back.
 */
bool run() {
    RubikCube cube;
    while (true) {
        std::cout << "\nSolved: " << (cube.is_solved() ? "yes" : "no") << '\n';
        print_menu();

        std::string line;
        if (read_quit_flag(line)) {
            return false;
        }
        if (line == "0") {
            return true;
        }

        switch (parse_int(line)) {
            case 1:
                std::cout << cube;
                break;

            case 2: {
                std::cout << "Enter face (U/D/L/R/F/B):";
                if (!read_line(line)) {
                    return false;
                }
                Face face;
                if (!parse_face(line, face)) {
                    std::cout << "Unknown face.\n";
                    break;
                }

                std::cout << "Enter direction (cw/ccw):";
                if (!read_line(line)) {
                    return false;
                }
                const std::string dir = to_lower(line);
                if (dir == "cw" || dir == "clockwise") {
                    cube.rotate(face, Direction::Clockwise);
                    std::cout << "Rotated " << face_name(face) << " clockwise.\n";
                } else if (dir == "ccw" || dir == "counterclockwise") {
                    cube.rotate(face, Direction::Counterclockwise);
                    std::cout << "Rotated " << face_name(face) << " counterclockwise.\n";
                } else {
                    std::cout << "Unknown direction.\n";
                }
                break;
            }

            case 3:
                std::cout << (cube.is_solved() ? "yes" : "no") << '\n';
                break;

            case 4:
                std::cout << "Enter seed (0 for random):";
                if (!read_line(line)) {
                    return false;
                }
                cube.randomize(static_cast<unsigned>(parse_int(line) < 0 ? 0 : parse_int(line)));
                std::cout << "Cube scrambled.\n";
                break;

            case 5: {
                std::cout << "Enter file path:";
                if (!read_line(line)) {
                    return false;
                }
                if (cube.load_from_file(line)) {
                    std::cout << "Loaded from '" << line << "'.\n";
                } else {
                    std::cout << "Failed to load from '" << line << "'.\n";
                }
                break;
            }

            case 6: {
                std::cout << "Enter seed (0 for random):";
                if (!read_line(line)) {
                    return false;
                }
                const unsigned seed = static_cast<unsigned>(parse_int(line) < 0 ? 0 : parse_int(line));
                cube = RubikCube(seed);
                std::cout << "Created cube from seed " << seed << ".\n";
                break;
            }

            default:
                std::cout << "Unknown option.\n";
                break;
        }
    }
}

}  // namespace rubikcli

/**
 * @brief Prints the welcome screen with the library choice.
 */
void print_welcome() {
    std::cout << "\nThis is lab1, choose lib: 1. set, 2. rubik's cube\n";
}

/**
 * @brief Program entry point: runs the interactive CLI.
 * @return 0 on success.
 */
int main() {
    print_welcome();

    while (true) {
        std::string line;
        if (read_quit_flag(line)) {
            break;
        }

        bool stay = true;
        switch (parse_int(line)) {
            case 1:
                stay = setcli::run();
                break;
            case 2:
                stay = rubikcli::run();
                break;
            default:
                std::cout << "Unknown option.\n";
                break;
        }

        if (!stay) {
            break;
        }
        print_welcome();
    }

    std::cout << "Bye!\n";
    return 0;
}