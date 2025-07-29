#include <iostream>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <vector>
#include <ctime>
using namespace std;

bool gameOver;
bool paused = false;
const int width = 20;
const int height = 20;
int x, y, fruitX, fruitY, score, highScore;
int tailX[100], tailY[100];
int nTail;
int speed = 100; // Initial speed (lower is faster)

vector<pair<int, int>> obstacles;

enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void LoadHighScore() {
    ifstream inFile("highscore.txt");
    if (inFile.is_open()) {
        inFile >> highScore;
        inFile.close();
    }
    else {
        highScore = 0;
    }
}

void SaveHighScore() {
    ofstream outFile("highscore.txt");
    if (outFile.is_open()) {
        outFile << highScore;
        outFile.close();
    }
}

void GenerateObstacles() {
    obstacles.clear();
    for (int i = 0; i < 5; ++i) {
        int ox = rand() % width;
        int oy = rand() % height;
        if ((ox != x || oy != y) && (ox != fruitX || oy != fruitY))
            obstacles.push_back({ ox, oy });
    }
}

void Setup() {
    srand(time(0));
    gameOver = false;
    paused = false;
    dir = RIGHT; // <-- Snake will move immediately to the right
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
    speed = 100;
    LoadHighScore();
    GenerateObstacles();
}

void MoveCursorToTop() {
    COORD cursorPosition;
    cursorPosition.X = 0;
    cursorPosition.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

void Draw() {
    MoveCursorToTop();
    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) setColor(6), cout << "#";

            if (i == y && j == x) setColor(2), cout << "0";
            else if (i == fruitY && j == fruitX) setColor(4), cout << "F";
            else {
                bool printed = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        setColor(2), cout << "o";
                        printed = true; break;
                    }
                }
                if (!printed) {
                    bool isObs = false;
                    for (auto& ob : obstacles) {
                        if (ob.first == j && ob.second == i) {
                            setColor(12), cout << "X";
                            isObs = true; break;
                        }
                    }
                    if (!isObs) cout << " ";
                }
            }

            if (j == width - 1) setColor(6), cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++) cout << "#";
    setColor(3);

    cout << "\nScore: " << score << "  High Score: " << highScore;
    if (paused) cout << "\nGame Paused. Press 'P' to resume.";
    setColor(7);
}

void Input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a': if (dir != RIGHT) dir = LEFT; break;
        case 'd': if (dir != LEFT)  dir = RIGHT; break;
        case 'w': if (dir != DOWN)  dir = UP; break;
        case 's': if (dir != UP)    dir = DOWN; break;
        case 'p': case 'P': paused = !paused; break;
        case 'x': gameOver = true; break;
        }
    }
}

void Logic() {
    int prevX = tailX[0], prevY = tailY[0];
    tailX[0] = x; tailY[0] = y;
    int prev2X, prev2Y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i]; prev2Y = tailY[i];
        tailX[i] = prevX; tailY[i] = prevY;
        prevX = prev2X; prevY = prev2Y;
    }

    switch (dir) {
    case LEFT:  x--; break;
    case RIGHT: x++; break;
    case UP:    y--; break;
    case DOWN:  y++; break;
    default: break;
    }

    // Wrap-around
    if (x >= width) x = 0;
    else if (x < 0) x = width - 1;
    if (y >= height) y = 0;
    else if (y < 0) y = height - 1;

    // Check self collision
    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y) gameOver = true;

    // Check obstacle collision
    for (auto& ob : obstacles)
        if (ob.first == x && ob.second == y) gameOver = true;

    // Fruit eaten
    if (x == fruitX && y == fruitY) {
        score += 10;
        if (score > highScore) highScore = score;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
        GenerateObstacles();

        if (speed > 20) speed -= 5;
    }
}

int main() {
    Setup();
    while (!gameOver) {
        Input();
        if (!paused) {
            Draw();
            Logic();
        }
        else {
            Draw();
            Sleep(200);
        }
        Sleep(speed);
    }
    SaveHighScore();
    return 0;
}
