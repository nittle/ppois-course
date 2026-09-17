/**
 * @file rubik.h
 * @brief Declaration of the RubikCube class.
 *
 * Models the Rubik's Cube puzzle with 6 faces of 3x3 colored cells.
 * The class supports a random initial arrangement, loading an arrangement
 * from a file, rotating any face of the cube and checking whether the
 * cube is solved.
 */

#ifndef RUBIK_H
#define RUBIK_H

#include <array>
#include <cstdint>
#include <istream>
#include <string>

/**
 * @brief The color of a cell on a cube face.
 */
enum class Color : uint8_t {
    White,   ///< White cells (the Up face on a solved cube).
    Yellow,  ///< Yellow cells (the Down face on a solved cube).
    Red,     ///< Red cells (the Right face on a solved cube).
    Orange,  ///< Orange cells (the Left face on a solved cube).
    Green,   ///< Green cells (the Front face on a solved cube).
    Blue     ///< Blue cells (the Back face on a solved cube).
};

/**
 * @brief A face of the cube.
 */
enum class Face : uint8_t {
    Up = 0,     ///< The Up face (normal +Y).
    Down = 1,   ///< The Down face (normal -Y).
    Left = 2,   ///< The Left face (normal -X).
    Right = 3,  ///< The Right face (normal +X).
    Front = 4,  ///< The Front face (normal +Z).
    Back = 5    ///< The Back face (normal -Z).
};

/**
 * @brief The direction of a face rotation.
 */
enum class Direction : uint8_t {
    Clockwise,        ///< 90 degrees clockwise, seen from outside the face.
    Counterclockwise  ///< 90 degrees counterclockwise, seen from outside the face.
};

/**
 * @brief Returns the case-insensitive color letter used in files and printing.
 *
 * White->'W', Yellow->'Y', Red->'R', Orange->'O', Green->'G', Blue->'B'.
 *
 * @param color The color.
 * @return The single-letter representation of the color.
 */
char color_letter(Color color);

/**
 * @brief Returns the human-readable name of a color.
 * @param color The color.
 * @return The color name, for example "White".
 */
const char* color_name(Color color);

/**
 * @brief Converts a letter into a Color.
 *
 * Accepts both upper- and lowercase letters.
 *
 * @param ch The letter to convert.
 * @param out Receives the color on success.
 * @return True if the letter is a valid color letter, false otherwise.
 */
bool parse_color(char ch, Color& out);

/**
 * @brief Returns the human-readable name of a cube face.
 * @param face The face.
 * @return The face name, for example "Front".
 */
const char* face_name(Face face);

/**
 * @brief Compares two colors for equality.
 * @param lhs Left-hand color.
 * @param rhs Right-hand color.
 * @return True if the colors are equal.
 */
inline bool operator==(Color lhs, Color rhs) {
    return static_cast<uint8_t>(lhs) == static_cast<uint8_t>(rhs);
}

/**
 * @brief Compares two colors for inequality.
 * @param lhs Left-hand color.
 * @param rhs Right-hand color.
 * @return True if the colors differ.
 */
inline bool operator!=(Color lhs, Color rhs) {
    return !(lhs == rhs);
}

/**
 * @brief A class implementing the Rubik's Cube puzzle.
 *
 * The cube stores 6 faces (Up, Down, Left, Right, Front, Back); every face
 * is a 3x3 grid of colors. A solved cube has every face filled with a
 * single color (Up=White, Down=Yellow, Left=Orange, Right=Red,
 * Front=Green, Back=Blue).
 *
 * The rotation geometry is implemented with precomputed move tables derived
 * from the cube's 3D coordinates, so all face turns are consistent with a
 * physical cube.
 */
class RubikCube {
    public:
    /**
     * @brief The edge length of a face, in cells.
     */
    static constexpr std::size_t SIZE = 3;

    /**
     * @brief The number of faces of the cube.
     */
    static constexpr std::size_t FACE_COUNT = 6;

    /**
     * @brief The color grid of a single face.
     */
    using Grid = std::array<std::array<Color, SIZE>, SIZE>;

    /**
     * @brief Constructs a solved cube.
     */
    RubikCube();

    /**
     * @brief Constructs a cube with a random arrangement of colors.
     *
     * The cube is solved first and then scrambled with a sequence of
     * random face turns, so the resulting arrangement is always reachable
     * from a solved cube.
     *
     * @param seed The random seed; 0 selects a non-deterministic seed.
     */
    explicit RubikCube(unsigned seed);

    /**
     * @brief Returns a new cube in the solved state.
     * @return A solved cube.
     */
    static RubikCube solved();

    /**
     * @brief Scrambles the cube into a random arrangement of colors.
     *
     * Random face turns are applied to the current cube. With the same
     * seed the result is reproducible.
     *
     * @param seed The random seed; 0 selects a non-deterministic seed.
     */
    void randomize(unsigned seed = 0);

    /**
     * @brief Loads the cube arrangement from a stream.
     *
     * The stream must contain the 54 cell colors in the order they appear
     * in the printed net (see operator<<): the Up face row by row, then
     * the Left, Front, Right and Back faces row by row, then the Down face
     * row by row. Any non-letter characters are ignored and color letters
     * are matched case-insensitively. On failure the cube state is
     * unspecified.
     *
     * @param is The input stream.
     * @return True if the arrangement was loaded successfully, false otherwise.
     */
    bool load(std::istream& is);

    /**
     * @brief Loads the cube arrangement from a file.
     *
     * See load(std::istream&) for the file format.
     *
     * @param path The path of the file to read.
     * @return True if the arrangement was loaded successfully, false otherwise.
     */
    bool load_from_file(const std::string& path);

    /**
     * @brief Rotates a face of the cube by 90 degrees.
     *
     * The affected face and the four faces adjacent to it are updated so
     * that the turn matches a physical cube.
     *
     * @param face The face to rotate.
     * @param direction The rotation direction (clockwise by default).
     */
    void rotate(Face face, Direction direction = Direction::Clockwise);

    /**
     * @brief Checks whether the cube is solved.
     *
     * The cube is solved when every face is filled with a single color
     * (the exact color of each face is not restricted).
     *
     * @return True if all faces are monochrome, false otherwise.
     */
    bool is_solved() const;

    /**
     * @brief Returns the color of a single cell.
     * @param face The face of the cell.
     * @param row The row of the cell within the face (0-based).
     * @param col The column of the cell within the face (0-based).
     * @return The color of the requested cell.
     */
    Color color(Face face, std::size_t row, std::size_t col) const;

    private:
    /**
     * @brief The grids of all six faces, indexed by Face.
     */
    std::array<Grid, FACE_COUNT> _faces;

    /**
     * @brief Returns a reference to a cell addressed by a flat cell id.
     *
     * A cell id encodes the face, row and column: id = face*9 + row*3 + col.
     *
     * @param id The flat cell id in the range [0, 54).
     * @return A reference to the requested cell.
     */
    Color& cell(std::size_t id);

    /**
     * @brief Returns a const reference to a cell addressed by a flat cell id.
     * @param id The flat cell id in the range [0, 54).
     * @return A const reference to the requested cell.
     */
    const Color& cell(std::size_t id) const;

    /**
     * @brief Rotates the layer of the given face 90 degrees clockwise.
     *
     * Applies the precomputed move table for the face. A counterclockwise
     * turn is performed by applying this operation three times.
     *
     * @param face The face whose layer is rotated.
     */
    void rotate_layer_clockwise(Face face);
};

/**
 * @brief Prints the cube in the flat net layout.
 *
 * The output is a 9x12 net of color letters:
 * the Up face centered on top, the Down face centered below, and the
 * Left, Front, Right and Back faces side by side in the middle row.
 *
 * @param os The output stream.
 * @param cube The cube to print.
 * @return The output stream, for chaining.
 */
std::ostream& operator<<(std::ostream& os, const RubikCube& cube);

/**
 * @brief Reads a cube from a stream.
 *
 * Delegates to RubikCube::load; on failure the stream is left in a failed
 * state and the cube is unspecified.
 *
 * @param is The input stream.
 * @param cube The cube to fill.
 * @return The input stream, for chaining.
 */
std::istream& operator>>(std::istream& is, RubikCube& cube);

#endif  // RUBIK_H