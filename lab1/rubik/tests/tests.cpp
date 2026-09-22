#include "UnitTest++/UnitTest++.h"
#include <UnitTest++/CheckMacros.h>
#include <UnitTest++/TestMacros.h>
#include <UnitTest++/TestRunner.h>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include "../rubik.h"

namespace {

// Compares every cell of two cubes.
bool cubes_equal(const RubikCube& a, const RubikCube& b) {
    for (int f = 0; f < 6; ++f) {
        const Face face = static_cast<Face>(f);
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                if (a.color(face, r, c) != b.color(face, r, c)) {
                    return false;
                }
            }
        }
    }
    return true;
}

}  // namespace

SUITE(RubikCubeTest) {
    TEST(Sanity) {
        CHECK_EQUAL(0, 0);
    }

    // A default cube is solved: every face is monochrome
    TEST(DefaultCubeIsSolved) {
        RubikCube cube;
        CHECK(cube.is_solved());
    }

    // On a solved cube each face is filled with its center color
    TEST(SolvedCenterColors) {
        RubikCube cube;
        CHECK(cube.color(Face::Up, 1, 1) == Color::White);
        CHECK(cube.color(Face::Down, 1, 1) == Color::Yellow);
        CHECK(cube.color(Face::Left, 1, 1) == Color::Orange);
        CHECK(cube.color(Face::Right, 1, 1) == Color::Red);
        CHECK(cube.color(Face::Front, 1, 1) == Color::Green);
        CHECK(cube.color(Face::Back, 1, 1) == Color::Blue);

        // Every cell, not just the centers, has the face color
        CHECK(cube.color(Face::Front, 0, 2) == Color::Green);
        CHECK(cube.color(Face::Back, 2, 0) == Color::Blue);
        CHECK(cube.color(Face::Up, 2, 2) == Color::White);
    }

    // Rotating any face clockwise four times restores the cube
    TEST(FourClockwiseRotationsRestore) {
        for (int f = 0; f < 6; ++f) {
            RubikCube cube;
            const Face face = static_cast<Face>(f);
            for (int i = 0; i < 4; ++i) {
                cube.rotate(face, Direction::Clockwise);
            }
            CHECK(cube.is_solved());
        }
    }

    // A clockwise turn followed by a counterclockwise turn changes nothing
    TEST(CwThenCcwRestoresState) {
        for (int f = 0; f < 6; ++f) {
            RubikCube cube(12345);
            const RubikCube before = cube;
            const Face face = static_cast<Face>(f);
            cube.rotate(face, Direction::Clockwise);
            cube.rotate(face, Direction::Counterclockwise);
            CHECK(cubes_equal(cube, before));
        }
    }

    // A counterclockwise turn equals three clockwise turns
    TEST(CcwEqualsThreeCw) {
        RubikCube a(42);
        RubikCube b(42);
        a.rotate(Face::Front, Direction::Counterclockwise);

        for (int i = 0; i < 3; ++i) {
            b.rotate(Face::Front, Direction::Clockwise);
        }
        CHECK(cubes_equal(a, b));
    }

    // Randomizing a cube produces a non-solved arrangement
    TEST(RandomizeProducesScrambled) {
        RubikCube cube;
        cube.randomize(1337);
        CHECK(!cube.is_solved());
    }

    // Randomizing with the same seed is reproducible
    TEST(RandomizeIsDeterministic) {
        RubikCube a(2024);
        RubikCube b(2024);
        CHECK(cubes_equal(a, b));

        RubikCube c(2025);
        CHECK(!cubes_equal(a, c));
    }

    // The constructed-from-seed cube is also scrambled and reproducible
    TEST(SeedConstructorScrambles) {
        RubikCube cube(999);
        CHECK(!cube.is_solved());

        RubikCube other(999);
        CHECK(cubes_equal(cube, other));
    }

    // Turning the Front face clockwise transfers colors as on a real cube
    TEST(FrontTurnTransfersCorrectColors) {
        RubikCube cube;
        cube.rotate(Face::Front, Direction::Clockwise);

        // The old bottom row of Up (white) moves to the left column of Right.
        for (int r = 0; r < 3; ++r) {
            CHECK(cube.color(Face::Right, r, 0) == Color::White);
        }
        // The old right column of Left (orange) moves to the bottom row of Up.
        for (int c = 0; c < 3; ++c) {
            CHECK(cube.color(Face::Up, 2, c) == Color::Orange);
        }
        // The old left column of Right (red) moves to the top row of Down.
        for (int c = 0; c < 3; ++c) {
            CHECK(cube.color(Face::Down, 0, c) == Color::Red);
        }
        // The old top row of Down (yellow) moves to the right column of Left.
        for (int r = 0; r < 3; ++r) {
            CHECK(cube.color(Face::Left, r, 2) == Color::Yellow);
        }
        // The Front face itself stays green (still monochrome).
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                CHECK(cube.color(Face::Front, r, c) == Color::Green);
            }
        }
        CHECK(!cube.is_solved());
    }

    // A solved cube can be loaded from its printed net
    TEST(LoadSolvedNetFromStream) {
        RubikCube cube;
        std::ostringstream os;
        os << cube;

        RubikCube loaded;
        std::istringstream is(os.str());
        CHECK(loaded.load(is));
        CHECK(loaded.is_solved());
    }

    // A scrambled cube round-trips through printing and loading
    TEST(PrintLoadRoundTrip) {
        RubikCube cube(777);
        CHECK(!cube.is_solved());

        std::ostringstream os;
        os << cube;

        RubikCube loaded;
        std::istringstream is(os.str());
        CHECK(is >> loaded);
        CHECK(cubes_equal(cube, loaded));
    }

    // Loading a stream with an invalid color letter fails
    TEST(LoadInvalidLetterFails) {
        RubikCube cube;
        std::ostringstream os;
        os << cube;
        std::string text = os.str();

        // Corrupt the first letter of the net with an invalid color 'Z'.
        const std::size_t pos = text.find_first_of("WYRGOB");
        CHECK(pos != std::string::npos);
        text[pos] = 'Z';

        RubikCube loaded;
        std::istringstream is(text);
        CHECK(!loaded.load(is));
    }

    // Loading a stream with too few cells fails
    TEST(LoadTooFewCellsFails) {
        std::istringstream is("W W W W W W W W W");
        RubikCube loaded;
        CHECK(!loaded.load(is));
    }

    // An arrangement can be loaded from a file
    TEST(LoadFromFile) {
        const std::filesystem::path dir = std::filesystem::temp_directory_path();
        std::filesystem::create_directories(dir);  // ensure the temp dir exists
        const std::filesystem::path path = dir / "rubik_state_test.txt";

        RubikCube cube(321);
        std::ostringstream os;
        os << cube;
        {
            std::ofstream file(path);
            file << os.str();
        }

        RubikCube loaded;
        CHECK(loaded.load_from_file(path.string()));
        CHECK(cubes_equal(cube, loaded));

        std::filesystem::remove(path);
    }

    // Loading from a missing file fails
    TEST(LoadMissingFileFails) {
        RubikCube loaded;
        CHECK(!loaded.load_from_file("/nonexistent/path/to/cube.txt"));
    }

    // The net printed for a solved cube matches the expected layout
    TEST(PrintSolvedNetLayout) {
        RubikCube cube;
        std::ostringstream os;
        os << cube;
        const std::string out = os.str();

        CHECK(out.find("      W W W") != std::string::npos);
        CHECK(out.find("O O O G G G R R R B B B") != std::string::npos);
        CHECK(out.find("      Y Y Y") != std::string::npos);
    }
}

int main() {
    std::cout << "Running RubikCube tests!" << std::endl;
    return UnitTest::RunAllTests();
}