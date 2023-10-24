#include <iostream>
#include <conio.h>
#include <windows.h>
#include <locale>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;

const int roadWidth = 15;
const int roadHeight = 15;
const int roadBorderMargin = 1; // Margin from max value
char road[roadHeight][roadWidth];
int carPosition = roadWidth / 2;
vector<int> enemyCarX(2, carPosition); // Initialize enemy cars' X coordinates
vector<int> enemyCarPosition(2, 0); // Initialize enemy cars' positions
bool gameOver = false; // Track game over state
vector<bool> enemyCarActive(2, false); // Track if the enemy cars are active
clock_t enemyCarTimer = 0; // Timer for enemy car appearance
clock_t enemyCarMoveTimer = 0; // Timer for enemy car movement
int enemyCarSpeed = 16; // Adjust this variable to control the enemy cars' speed
int score = 0; // Initialize the score

void initializeRoad() {
    for (int i = 0; i < roadHeight; i++) {
        for (int j = 0; j < roadWidth; j++) {
            road[i][j] = ' ';
        }
    }
}

void drawRoad(int carPosition, vector<int>& enemyCarX, vector<int>& enemyCarPosition) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = { 0, 0 };

    for (int i = 0; i < roadHeight; i++) {
        for (int j = 0; j < roadWidth; j++) {
            position.Y = i;
            position.X = j + roadBorderMargin; // Shift to create a border

            if (i == roadHeight - 1 && j == carPosition) {
                SetConsoleCursorPosition(console, position);
                wcout << "\033[1;32m" << u8"\u2593" << "\033[0m"; // Use "\u2593" for the player's car
            }
            else {
                bool enemyCarDrawn = false;
                for (int k = 0; k < enemyCarX.size(); k++) {
                    if (enemyCarActive[k] && i == enemyCarPosition[k] && j == enemyCarX[k]) {
                        SetConsoleCursorPosition(console, position);
                        wcout << "\033[1;31m" << '0' << "\033[0m"; // Use "X" for the enemy cars
                        enemyCarDrawn = true;
                    }
                }

                if (!enemyCarDrawn) {
                    if ((j == roadBorderMargin) || (j == roadWidth + roadBorderMargin - 1)) {
                        SetConsoleCursorPosition(console, position);
                        wcout << u8"\u2551";
                    }
                    else if ((j == roadBorderMargin) || (j == roadWidth - roadBorderMargin - 1)) {
                        SetConsoleCursorPosition(console, position);
                        wcout << u8"\u2551";
                    }
                    else {
                        cout << road[i][j - roadBorderMargin];
                    }
                }
            }
        }
    }
    
    // Display the score at the bottom of the screen
    position.Y = roadHeight + 1;
    position.X = roadBorderMargin;
    SetConsoleCursorPosition(console, position);
    wcout << "\033[1;32m" << "Score: " << score << "\033[0m";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    char key;
    bool quit = false;

    initializeRoad();

    while (!quit) {
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD position = { 0, 0 };
        SetConsoleCursorPosition(console, position);

        if (!gameOver) {
            // Update road
            for (int i = roadHeight - 1; i > 0; i--) {
                for (int j = 0; j < roadWidth; j++) {
                    road[i][j] = road[i - 1][j];
                }
            }

            // Handle key presses
            if (_kbhit()) {
                key = _getch();
                switch (key) {
                case 'a':
                    if (carPosition > 0) carPosition--;
                    break;
                case 'd':
                    if (carPosition < roadWidth - 1) carPosition++;
                    break;
                case 'q':
                    quit = true;
                    break;
                }
            }

            // Check and update enemy cars' appearance
            clock_t currentTime = clock();
            for (int k = 0; k < enemyCarX.size(); k++) {
                if (!enemyCarActive[k] && (currentTime - enemyCarTimer) >= CLOCKS_PER_SEC * 2) {
                    enemyCarActive[k] = true;
                    enemyCarX[k] = (rand() % (roadWidth - 6)) + 3; // Set enemy car X coordinate
                    enemyCarTimer = currentTime;
                }
            }

            // Update enemy cars' positions with a timer
            if ((currentTime - enemyCarMoveTimer) >= CLOCKS_PER_SEC / enemyCarSpeed) {
                for (int k = 0; k < enemyCarX.size(); k++) {
                    if (enemyCarActive[k]) {
                        enemyCarPosition[k]++;
                        enemyCarMoveTimer = currentTime;
                    }
                }
            }

            // Check for collision with player's car after updating the enemy cars' positions
            for (int k = 0; k < enemyCarX.size(); k++) {
                if (enemyCarPosition[k] == roadHeight - 1 && enemyCarX[k] == carPosition) {
                    gameOver = true;
                }
            }

            for (int k = 0; k < enemyCarX.size(); k++) {
                if (enemyCarPosition[k] == roadHeight - 1) {
                    enemyCarActive[k] = false;
                    enemyCarPosition[k] = 0;
                    score++; // Increment score for each car that reaches the bottom without collision
                }
            }

            if (carPosition == roadBorderMargin || carPosition == roadWidth - roadBorderMargin - 1) {
                gameOver = true;
            }
        }

        // Draw the road
        drawRoad(carPosition, enemyCarX, enemyCarPosition);

        if (gameOver) {
            wcout <<endl<< "\033[1;31m" << "END GAME" << "\033[0m";
            while (true) {
                if (_kbhit() && _getch() == 'q') {
                    quit = true;
                    break;
                }
            }
        }
    }

    return 0;
}
