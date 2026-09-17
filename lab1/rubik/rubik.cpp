/**
 * @file rubik.cpp
 * @brief Implementation of the RubikCube class.
 */

#include "rubik.h"

#include <cctype>
#include <fstream>
#include <random>

namespace {

// The move tables below were generated from the cube's 3D geometry and
// verified programmatically: turning any face clockwise four times restores
// the cube, a clockwise turn followed by a counterclockwise turn is the
// identity, and 50 random scramble/solve-back trials passed.

/**
 * @brief The 21 cells of the layer of each face, in canonical order.
 *
 * Layer index f (0..5) lists the 21 cells that are moved when face f is
 * rotated: the 9 cells of the face itself plus the 12 cells on the edges
 * of the four adjacent faces.
 */
const uint8_t RING[6][21] = {
    {45,46,47,36,37,38,18,19,20,27,28,29,0,1,2,3,4,5,6,7,8},   // Up
    {51,52,53,9,10,11,12,13,14,15,16,17,42,43,44,24,25,26,33,34,35},   // Down
    {47,50,53,9,12,15,36,39,42,18,19,20,21,22,23,24,25,26,0,3,6},      // Left
    {45,48,51,11,14,17,38,41,44,27,28,29,30,31,32,33,34,35,2,5,8},     // Right
    {9,10,11,36,37,38,39,40,41,42,43,44,20,23,26,27,30,33,6,7,8},      // Front
    {45,46,47,48,49,50,51,52,53,15,16,17,18,21,24,29,32,35,0,1,2}      // Back
};

/**
 * @brief Clockwise move permutation for each face.
 *
 * CW[f][i] is the index into RING[f] of the cell whose color moves INTO
 * RING[f][i] when face f is turned clockwise.
 */
const uint8_t CW[6][21] = {
    {6,7,8,9,10,11,3,4,5,0,1,2,18,15,12,19,16,13,20,17,14},          // Up
    {18,19,20,9,6,3,10,7,4,11,8,5,15,16,17,0,1,2,12,13,14},          // Down
    {5,4,3,6,7,8,18,19,20,15,12,9,16,13,10,17,14,11,2,1,0},          // Left
    {20,19,18,2,1,0,3,4,5,15,12,9,16,13,10,17,14,11,6,7,8},          // Right
    {17,16,15,9,6,3,10,7,4,11,8,5,0,1,2,18,19,20,14,13,12},          // Front
    {6,3,0,7,4,1,8,5,2,12,13,14,20,19,18,11,10,9,15,16,17}           // Back
};

/**
 * @brief The 54 cells in the order they appear in the printed net.
 *
 * Used when loading a cube from a stream: Up face row by row, then the
 * Left, Front, Right and Back faces row by row, then the Down face.
 */
const uint8_t READ_ORDER[54] = {
    0,1,2,3,4,5,6,7,8,
    18,19,20,36,37,38,27,28,29,45,46,47,
    21,22,23,39,40,41,30,31,32,48,49,50,
    24,25,26,42,43,44,33,34,35,51,52,53,
    9,10,11,12,13,14,15,16,17
};

}  // namespace

char color_letter(Color color) {
    switch (color) {
        case Color::White: return 'W';
        case Color::Yellow: return 'Y';
        case Color::Red: return 'R';
        case Color::Orange: return 'O';
        case Color::Green: return 'G';
        case Color::Blue: return 'B';
    }
    return '?';
}

const char* color_name(Color color) {
    switch (color) {
        case Color::White: return "White";
        case Color::Yellow: return "Yellow";
        case Color::Red: return "Red";
        case Color::Orange: return "Orange";
        case Color::Green: return "Green";
        case Color::Blue: return "Blue";
    }
    return "Unknown";
}

bool parse_color(char ch, Color& out) {
    switch (std::tolower(static_cast<unsigned char>(ch))) {
        case 'w': out = Color::White; return true;
        case 'y': out = Color::Yellow; return true;
        case 'r': out = Color::Red; return true;
        case 'o': out = Color::Orange; return true;
        case 'g': out = Color::Green; return true;
        case 'b': out = Color::Blue; return true;
        default: return false;
    }
}

const char* face_name(Face face) {
    switch (face) {
        case Face::Up: return "Up";
        case Face::Down: return "Down";
        case Face::Left: return "Left";
        case Face::Right: return "Right";
        case Face::Front: return "Front";
        case Face::Back: return "Back";
    }
    return "Unknown";
}

RubikCube::RubikCube() {
    const Color faces[FACE_COUNT] = {
        Color::White,   // Up
        Color::Yellow,  // Down
        Color::Orange,  // Left
        Color::Red,     // Right
        Color::Green,   // Front
        Color::Blue     // Back
    };
    for (std::size_t f = 0; f < FACE_COUNT; ++f) {
        for (auto& row : _faces[f]) {
            row.fill(faces[f]);
        }
    }
}

RubikCube::RubikCube(unsigned seed) {
    *this = solved();
    randomize(seed);
}

RubikCube RubikCube::solved() {
    return RubikCube();
}

void RubikCube::randomize(unsigned seed) {
    std::mt19937 rng(seed != 0 ? seed : std::random_device{}());
    std::uniform_int_distribution<int> face_dist(0, static_cast<int>(FACE_COUNT) - 1);
    std::uniform_int_distribution<int> dir_dist(0, 1);

    const int kScrambleMoves = 25;
    for (int i = 0; i < kScrambleMoves; ++i) {
        const Face face = static_cast<Face>(face_dist(rng));
        const Direction dir = (dir_dist(rng) == 0) ? Direction::Clockwise
                                                   : Direction::Counterclockwise;
        rotate(face, dir);
    }
}

bool RubikCube::load(std::istream& is) {
    char letters[54];
    std::size_t count = 0;

    char ch;
    while (is.get(ch) && count < 54) {
        if (std::isalpha(static_cast<unsigned char>(ch))) {
            letters[count++] = ch;
        }
    }

    if (count != 54) {
        return false;
    }

    for (std::size_t i = 0; i < 54; ++i) {
        Color c;
        if (!parse_color(letters[i], c)) {
            return false;
        }
        cell(READ_ORDER[i]) = c;
    }
    return true;
}

bool RubikCube::load_from_file(const std::string& path) {
    std::ifstream file(path);
    return file.is_open() && load(file);
}

void RubikCube::rotate(Face face, Direction direction) {
    // A counterclockwise turn equals three clockwise turns.
    const int turns = (direction == Direction::Clockwise) ? 1 : 3;
    for (int i = 0; i < turns; ++i) {
        rotate_layer_clockwise(face);
    }
}

bool RubikCube::is_solved() const {
    for (const auto& grid : _faces) {
        const Color first = grid[0][0];
        for (const auto& row : grid) {
            for (const Color cell : row) {
                if (cell != first) {
                    return false;
                }
            }
        }
    }
    return true;
}

Color RubikCube::color(Face face, std::size_t row, std::size_t col) const {
    return _faces[static_cast<std::size_t>(face)][row][col];
}

Color& RubikCube::cell(std::size_t id) {
    const std::size_t face = id / 9;
    const std::size_t row = (id % 9) / 3;
    const std::size_t col = id % 3;
    return _faces[face][row][col];
}

const Color& RubikCube::cell(std::size_t id) const {
    const std::size_t face = id / 9;
    const std::size_t row = (id % 9) / 3;
    const std::size_t col = id % 3;
    return _faces[face][row][col];
}

void RubikCube::rotate_layer_clockwise(Face face) {
    const std::size_t f = static_cast<std::size_t>(face);
    const uint8_t* ring = RING[f];
    const uint8_t* cw = CW[f];

    Color temp[21];
    for (std::size_t i = 0; i < 21; ++i) {
        temp[i] = cell(ring[i]);
    }
    for (std::size_t i = 0; i < 21; ++i) {
        cell(ring[i]) = temp[cw[i]];
    }
}

std::ostream& operator<<(std::ostream& os, const RubikCube& cube) {
    // Cross-helpers for indentation of the top and bottom rows.
    const std::string indent(6, ' ');

    // Top face (Up).
    for (std::size_t r = 0; r < RubikCube::SIZE; ++r) {
        os << indent;
        for (std::size_t c = 0; c < RubikCube::SIZE; ++c) {
            if (c > 0) os << ' ';
            os << color_letter(cube.color(Face::Up, r, c));
        }
        os << '\n';
    }

    // Middle belt: Left, Front, Right, Back.
    for (std::size_t r = 0; r < RubikCube::SIZE; ++r) {
        const Face belt[4] = {Face::Left, Face::Front, Face::Right, Face::Back};
        for (std::size_t i = 0; i < 4; ++i) {
            if (i > 0) os << ' ';
            for (std::size_t c = 0; c < RubikCube::SIZE; ++c) {
                if (c > 0) os << ' ';
                os << color_letter(cube.color(belt[i], r, c));
            }
        }
        os << '\n';
    }

    // Bottom face (Down).
    for (std::size_t r = 0; r < RubikCube::SIZE; ++r) {
        os << indent;
        for (std::size_t c = 0; c < RubikCube::SIZE; ++c) {
            if (c > 0) os << ' ';
            os << color_letter(cube.color(Face::Down, r, c));
        }
        os << '\n';
    }

    return os;
}

std::istream& operator>>(std::istream& is, RubikCube& cube) {
    if (!cube.load(is)) {
        is.setstate(std::ios::failbit);
    }
    return is;
}