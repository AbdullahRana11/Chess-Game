#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace sf;
using namespace std;
const int TILE_SIZE = 80;
const int BOARD_SIZE = 8;
const int WINDOW_SIZE = TILE_SIZE * BOARD_SIZE;
const int MAX_MOVES = 64; // Maximum possible moves for a piece
int moveCount = 0;
int whiteScore = 0;
int blackScore = 0;
bool enPassantAvailable = false;
int enPassantX = -1;
int enPassantY = -1;
bool enPassantIsWhitePawn = false;
SoundBuffer moveSoundBuffer, captureSoundBuffer, startSoundBuffer, mateSoundBuffer, backmusicBuffer, promotionSoundBuffer, typingBuffer, clickBuffer, menuSoundBuffer;
Sound moveSound, checkSound, startSound, mateSound, backmusic, captureSound, promotionSound;
struct Move {
    int x, y;
};
// Load a texture for the board
Texture loadTexture(const string& filePath) {
    Texture texture;
    if (!texture.loadFromFile(filePath)) {
        cerr << "Failed to load " << filePath << endl;
        exit(1);
    }
    return texture;
}
//starting interface
bool startMenu(string& player1, string& player2) {
    RenderWindow menuWindow(VideoMode(800, 800), "Game - Start Menu");
    Font font;
    if (!font.loadFromFile("D:/assets/font.otf")) { // Load a font
        cerr << "Failed to load font!" << endl;
        return false;
    }
    Texture buttonTexture;
    if (!buttonTexture.loadFromFile("D:/assets/New/start.png")) {
        cerr << "Failed to load button texture!" << endl;
        return false;
    }
    Sprite startButton(buttonTexture);
    startButton.setPosition(300, 600);
    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("D:/assets/New/RonnyPic1 (1).jpg")) {
        cerr << "Failed to load background texture!" << endl;
        return false;
    }
    Sprite background(backgroundTexture);
    background.setColor(Color(255, 255, 255, 125));
    background.setScale(800.0f / backgroundTexture.getSize().x, 800.0f / backgroundTexture.getSize().y);

    Text title("Chess", font, 75);
    title.setPosition(400 - title.getGlobalBounds().width / 2, 180);

    Text prompt1("Player A= ", font, 42.5);
    prompt1.setPosition(50, 350);
    Text prompt2("Player B= ", font, 42.5);
    prompt2.setPosition(50, 420);

    Text player1Text("", font, 42.5);
    player1Text.setPosition(350, 352.5);
    Text player2Text("", font, 42.5);
    player2Text.setPosition(350, 422.5);
    if (!typingBuffer.loadFromFile("D:/assets/New/typing.mp3") || !clickBuffer.loadFromFile("D:/assets/New/click.mp3") || (!menuSoundBuffer.loadFromFile("D:/assets/New/Background Music.mp3"))) {
        cerr << "Failed to load sound effects!" << endl;
        return false;
    }
    Sound typingSound(typingBuffer);
    Sound clickSound(clickBuffer);
    Sound menuSound(menuSoundBuffer);
    menuSound.setVolume(30.f);
    menuSound.setLoop(true); // Loop the menu sound
    menuSound.play(); // Play the menu sound
    bool isPlayer1Input = true;
    while (menuWindow.isOpen()) {
        Event event;
        while (menuWindow.pollEvent(event)) {
            if (event.type == Event::Closed) {
                menuWindow.close();
                return false; // Exit if the menu window is closed
            }
            if (isPlayer1Input && event.type == Event::TextEntered) {
                if (event.text.unicode < 128) {
                    if (event.text.unicode == '\b' && !player1.empty()) {
                        player1.pop_back();
                    }
                    else if (player1.length() < 20) {
                        player1 += static_cast<char>(event.text.unicode);
                        typingSound.play();
                    }
                    player1Text.setString(player1);
                }
            }
            else if (!isPlayer1Input && event.type == Event::TextEntered) {
                if (event.text.unicode < 128) {
                    if (event.text.unicode == '\b' && !player2.empty()) {
                        player2.pop_back();
                    }
                    else if (player2.length() < 20) {
                        player2 += static_cast<char>(event.text.unicode);
                        typingSound.play();
                    }
                    player2Text.setString(player2);
                }
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                if (isPlayer1Input && !player1.empty()) {
                    isPlayer1Input = false;
                }
                else if (!isPlayer1Input && !player2.empty()) {
                    cout << "Game started!!!" << endl;
                    menuWindow.close();
                    clickSound.play();
                    return true; // Start the game
                }
            }
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                if (startButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && !player1.empty() && !player2.empty()) {
                    cout << "Game started!!!" << endl;
                    clickSound.play();
                    menuWindow.close();
                    return true; // Start the game
                }
            }
        }
        menuWindow.clear(Color::Black);
        menuWindow.draw(background);
        menuWindow.draw(title);
        menuWindow.draw(prompt1);
        menuWindow.draw(prompt2);
        menuWindow.draw(player1Text);
        menuWindow.draw(player2Text);
        menuWindow.draw(startButton);
        menuWindow.display();
    }
    return false;
}
//standard piece values
int getPieceValue(int piece) {
    switch (piece) {
    case 1: case 7: return 1;  // Pawn
    case 2: case 8: return 5;  // Rook
    case 3: case 9: return 3;  // Knight
    case 4: case 10: return 3; // Bishop
    case 5: case 11: return 9; // Queen
    case 6: case 12: return 0; // King (not counted)
    default: return 0;         // Empty square
    }
}
// Initialize the chessboard and pieces
void initializeBoard(int board[BOARD_SIZE][BOARD_SIZE]) {
    int initialBoard[BOARD_SIZE][BOARD_SIZE] = {
        { 2, 3, 4, 5, 6, 4, 3, 2 }, // Black back rank
        { 1, 1, 1, 1, 1, 1, 1, 1 }, // Black pawns
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 7, 7, 7, 7, 7, 7, 7, 7 }, // White pawns
      { 8, 9, 10, 11, 12, 10, 9, 8 } // White back rank
    };
    // Manual copy using nested loops
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = initialBoard[i][j];
        }
    }
}
// Check if a position is within bounds
bool isWithinBounds(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}
// Add a move if valid
void addMove(int x, int y, int board[BOARD_SIZE][BOARD_SIZE], Move legalMoves[], int& moveCount, bool isWhite) {
    if (isWithinBounds(x, y)) {
        if (board[y][x] == 0 || (isWhite && board[y][x] <= 6) || (!isWhite && board[y][x] >= 7)) {
            legalMoves[moveCount++] = { x, y };
        }
    }
}
// Get moves for a pawn
void getPawnMoves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE], Move legalMoves[], int& moveCount, bool isWhite) {
    int direction = isWhite ? -1 : 1; // Direction of pawn movement
    int startRow = isWhite ? 6 : 1;  // Starting row for double moves
    // Move forward one square
    if (isWithinBounds(x, y + direction) && board[y + direction][x] == 0) {
        legalMoves[moveCount++] = { x, y + direction };
        // Move forward two squares if on initial position
        if (y == startRow && isWithinBounds(x, y + 2 * direction) && board[y + 2 * direction][x] == 0) {
            legalMoves[moveCount++] = { x, y + 2 * direction };
        }
    }
    // Capture diagonally
    if (isWithinBounds(x - 1, y + direction) && board[y + direction][x - 1] != 0 && ((isWhite && board[y + direction][x - 1] <= 6) || (!isWhite && board[y + direction][x - 1] >= 7))) {
        legalMoves[moveCount++] = { x - 1, y + direction };
    }
    if (isWithinBounds(x + 1, y + direction) && board[y + direction][x + 1] != 0 && ((isWhite && board[y + direction][x + 1] <= 6) || (!isWhite && board[y + direction][x + 1] >= 7))) {
        legalMoves[moveCount++] = { x + 1, y + direction };
    }
    // En passant capture
    if (enPassantAvailable && enPassantIsWhitePawn != isWhite && y == enPassantY && abs(x - enPassantX) == 1) {
        int targetY = y + direction;
        if (isWithinBounds(enPassantX, targetY) && board[targetY][enPassantX] == 0) {
            legalMoves[moveCount++] = { enPassantX, targetY };
        }
    }
}
// Get moves for a rook
void getRookMoves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE], Move legalMoves[], int& moveCount, bool isWhite) {
    int directions[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
    for (int d = 0; d < 4; ++d) {
        int nx = x, ny = y;
        while (true) {
            nx += directions[d][0];
            ny += directions[d][1];
            if (!isWithinBounds(nx, ny) || (board[ny][nx] != 0 &&
                ((isWhite && board[ny][nx] > 6) || (!isWhite && board[ny][nx] < 7)))) {
                break;
            }
            legalMoves[moveCount++] = { nx, ny };
            if (board[ny][nx] != 0) break;
        }
    }
}
// Get moves for a knight
void getKnightMoves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE], Move legalMoves[], int& moveCount, bool isWhite) {
    int jumps[8][2] = { {1, 2}, {2, 1}, {-1, 2}, {-2, 1}, {1, -2}, {2, -1}, {-1, -2}, {-2, -1} };
    for (int i = 0; i < 8; ++i) {
        addMove(x + jumps[i][0], y + jumps[i][1], board, legalMoves, moveCount, isWhite);
    }
}
// Get moves for a bishop
void getBishopMoves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE], Move legalMoves[], int& moveCount, bool isWhite) {
    for (int dx = -1; dx <= 1; dx += 2) {
        for (int dy = -1; dy <= 1; dy += 2) {
            for (int i = 1; i < BOARD_SIZE; i++) {
                int nx = x + dx * i;
                int ny = y + dy * i;
                if (isWithinBounds(nx, ny)) {
                    if (board[ny][nx] == 0) {
                        legalMoves[moveCount++] = { nx, ny };
                    }
                    else {
                        if ((isWhite && board[ny][nx] <= 6) || (!isWhite && board[ny][nx] >= 7)) {
                            legalMoves[moveCount++] = { nx, ny };
                        }
                        break;
                    }
                }
            }
        }
    }
}
// Get moves for a queen
void getQueenMoves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE], Move legalMoves[], int& moveCount, bool isWhite) {
    getRookMoves(x, y, board, legalMoves, moveCount, isWhite);
    getBishopMoves(x, y, board, legalMoves, moveCount, isWhite);
}
// Get moves for a king
void getKingMoves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE], Move legalMoves[], int& moveCount, bool isWhite) {
    int directions[8][2] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
    for (int i = 0; i < 8; ++i) {
        addMove(x + directions[i][0], y + directions[i][1], board, legalMoves, moveCount, isWhite);
    }
}
// Get all possible legal moves for a piece
void getLegalMoves(int piece, int x, int y, int board[BOARD_SIZE][BOARD_SIZE], Move legalMoves[], int& moveCount) {
    moveCount = 0;
    bool isWhite = (piece >= 7);
    switch (piece % 6) {
    case 1: getPawnMoves(x, y, board, legalMoves, moveCount, isWhite); break;
    case 2: getRookMoves(x, y, board, legalMoves, moveCount, isWhite); break;
    case 3: getKnightMoves(x, y, board, legalMoves, moveCount, isWhite); break;
    case 4: getBishopMoves(x, y, board, legalMoves, moveCount, isWhite); break;
    case 5: getQueenMoves(x, y, board, legalMoves, moveCount, isWhite); break;
    case 0: getKingMoves(x, y, board, legalMoves, moveCount, isWhite); break;
    }
}
//pawn promotion function
void promotePawn(int x, int y, int board[BOARD_SIZE][BOARD_SIZE], bool& isWhiteTurn, Texture whitePromotionTextures[], Texture blackPromotionTextures[]) {
    // Check if the piece at the given position is a pawn
    int pawnValue = (isWhiteTurn) ? 7 : 1; // Assuming 7 for white pawns and 1 for black pawns
    if (board[y][x] == pawnValue) { // Ensure the piece is a pawn
        if ((isWhiteTurn && y == 0) || (!isWhiteTurn && y == 7)) {
            // Display promotion options
            RenderWindow window_1(VideoMode(320, 80), "Choose the piece for promotion");
            bool promoting = true;
            while (promoting) {
                Event event;
                while (window_1.pollEvent(event)) {
                    if (event.type == Event::Closed) {
                        window_1.close();
                        promoting = false; // Exit the promotion loop if the window is closed
                    }
                    if (event.type == Event::MouseButtonPressed) {
                        if (event.mouseButton.button == Mouse::Left) {
                            Vector2i mousePos = Mouse::getPosition(window_1);
                            // Check which promotion image was clicked
                            Texture* currentPromotionTextures = isWhiteTurn ? whitePromotionTextures : blackPromotionTextures;
                            for (int i = 0; i < 4; ++i) {
                                // Calculate the position based on the window size
                                int imageWidth = 80; // Width of each image
                                int imageHeight = 80; // Height of each image
                                int imageX = 5 + i * imageWidth; // X position of the image
                                int imageY = (window_1.getSize().y - imageHeight) / 2; // Centered vertically
                                // Check if the mouse is within the bounds of the image
                                if (mousePos.x >= imageX && mousePos.x <= imageX + imageWidth && mousePos.y >= imageY && mousePos.y <= imageY + imageHeight) {
                                    promotionSound.play();
                                    if (i == 0) {
                                        board[y][x] = (isWhiteTurn) ? 8 : 2; // Rook promotion
                                        if (isWhiteTurn) whiteScore += getPieceValue(8) - 1; // Add rook value to white score
                                        else blackScore += getPieceValue(2) - 1; // Add rook value to black score
                                    }
                                    else if (i == 1) {
                                        board[y][x] = (isWhiteTurn) ? 9 : 3; // Knight promotion
                                        if (isWhiteTurn) whiteScore += getPieceValue(9) - 1; // Add knight value to white score
                                        else blackScore += getPieceValue(3) - 1; // Add knight value to black score
                                    }
                                    else if (i == 2) {
                                        board[y][x] = (isWhiteTurn) ? 10 : 4; // Bishop promotion
                                        if (isWhiteTurn) whiteScore += getPieceValue(10) - 1; // Add bishop value to white score
                                        else blackScore += getPieceValue(4) - 1; // Add bishop value to black score
                                    }
                                    else if (i == 3) {
                                        board[y][x] = (isWhiteTurn) ? 11 : 5; // Queen promotion
                                        if (isWhiteTurn) whiteScore += getPieceValue(11) - 1; // Add queen value to white score
                                        else blackScore += getPieceValue(5) - 1; // Add queen value to black score
                                    }
                                    promoting = false; // Exit the promotion loop
                                    break;
                                }
                            }
                        }
                    }
                }
                // Draw promotion options
                window_1.clear(Color::White);
                Texture* currentPromotionTextures = isWhiteTurn ? whitePromotionTextures : blackPromotionTextures;
                for (int i = 0; i < 4; ++i) {
                    Sprite promotionSprite(currentPromotionTextures[i]);
                    // Calculate the position based on the window size
                    int imageX = 5 + i * 80; // X position of the image
                    int imageY = (window_1.getSize().y - 80) / 2; // Centered vertically
                    promotionSprite.setPosition(imageX, imageY); // Position
                    window_1.draw(promotionSprite);
                }
                window_1.display();
            }
        }
    }
}
//identifies if king is in check
bool isKingInCheck(bool isWhiteKing, int board[BOARD_SIZE][BOARD_SIZE]) {
    int kingX = -1, kingY = -1;
    // Find the king's position
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            int piece = board[y][x];
            if ((isWhiteKing && piece == 12) || (!isWhiteKing && piece == 6)) {
                kingX = x;
                kingY = y;
                break;
            }
        }
    }
    if (kingX == -1 || kingY == -1) { return false; } // King not found (shouldn't happen in a valid game)
    // Check if any opponent piece can attack the king
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            int piece = board[y][x];
            if ((isWhiteKing && piece > 0 && piece <= 6) || (!isWhiteKing && piece >= 7))
            {
                Move legalMoves[MAX_MOVES];
                int moveCount = 0;
                getLegalMoves(piece, x, y, board, legalMoves, moveCount);
                for (int i = 0; i < moveCount; ++i)
                {
                    if (legalMoves[i].x == kingX && legalMoves[i].y == kingY)
                    {
                        return true; // King is in check
                    }
                }
            }
        }
    }
    return false;
}
//enemy controlled squares
bool isSquareInCheck(bool isWhite, int targetX, int targetY, int board[BOARD_SIZE][BOARD_SIZE]) {
    // Iterate through the board to find all opponent pieces
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            int piece = board[y][x];
            // Check if the piece belongs to the opponent
            if ((isWhite && piece > 0 && piece <= 6) || (!isWhite && piece >= 7)) {
                Move legalMoves[MAX_MOVES];
                int moveCount = 0;
                getLegalMoves(piece, x, y, board, legalMoves, moveCount);
                // Check if any of the legal moves can attack the target square
                for (int i = 0; i < moveCount; ++i) {
                    if (legalMoves[i].x == targetX && legalMoves[i].y == targetY) {
                        return true; // Square is in check
                    }
                }
            }
        }
    }
    return false; // Square is not in check
}
//restritct moves of other pieces
void filterMovesToProtectKing(int board[BOARD_SIZE][BOARD_SIZE], Move legalMoves[], int& moveCount, int pieceX, int pieceY, bool isWhiteTurn) {
    Move filteredMoves[MAX_MOVES];
    int filteredCount = 0;
    for (int i = 0; i < moveCount; ++i) {
        int newX = legalMoves[i].x;
        int newY = legalMoves[i].y;
        // Simulate the move
        int temp = board[newY][newX];
        board[newY][newX] = board[pieceY][pieceX];
        board[pieceY][pieceX] = 0;
        // Check if the king is still in check after this move
        if (!isKingInCheck(isWhiteTurn, board)) {
            filteredMoves[filteredCount++] = legalMoves[i];
        }
        // Undo the move
        board[pieceY][pieceX] = board[newY][newX];
        board[newY][newX] = temp;
    }
    // Update the legal moves to only include the filtered moves
    moveCount = filteredCount;
    copy(filteredMoves, filteredMoves + filteredCount, legalMoves);
}
//checks for checkmate
bool isCheckmate(bool isWhiteKing, int board[BOARD_SIZE][BOARD_SIZE]) {
    if (!isKingInCheck(isWhiteKing, board)) return false;
    // Try all moves for all pieces of the current player
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            int piece = board[y][x];
            if ((isWhiteKing && piece >= 7) || (!isWhiteKing && piece > 0 && piece <= 6)) {
                Move legalMoves[MAX_MOVES];
                int moveCount = 0;
                getLegalMoves(piece, x, y, board, legalMoves, moveCount);
                for (int i = 0; i < moveCount; ++i) {
                    int newX = legalMoves[i].x;
                    int newY = legalMoves[i].y;
                    // Simulate the move
                    int temp = board[newY][newX];
                    board[newY][newX] = piece;
                    board[y][x] = 0;
                    bool stillInCheck = isKingInCheck(isWhiteKing, board);
                    // Undo the move
                    board[y][x] = piece;
                    board[newY][newX] = temp;
                    if (!stillInCheck) {
                        return false; // Found a move that gets out of check
                    }
                }
            }
        }
    }
    return true; // No moves to escape check -> Checkmate
}
// Validate if a move is legal
bool isMoveLegal(int x, int y, Move legalMoves[], int moveCount) {
    for (int i = 0; i < moveCount; ++i) {
        if (legalMoves[i].x == x && legalMoves[i].y == y) {
            return true;
        }
    }
    return false;
}
//genera function to add a new window
int new_window(int x, int y, int z, const string& name, string filepath) {
    RenderWindow window(VideoMode(x, y), name);
    Texture texture;
    if (!texture.loadFromFile(filepath)) {
        cerr << "Error: Could not load the image!" << endl;
        return -1;
    }
    Sprite sprite;
    sprite.setTexture(texture);
    Clock clock;
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }
        if (clock.getElapsedTime().asSeconds() > z) {
            window.close();
        }
        window.clear();
        window.draw(sprite);
        window.display();
    }
}
// Function to save the game state to a file
void saveGame(const string& filePath, const string& player1, const string& player2, int board[BOARD_SIZE][BOARD_SIZE], int whiteScore, int blackScore) {
    ofstream outFile(filePath);
    if (!outFile) {
        cerr << "Error opening file for saving!" << endl;
        return;
    }
    // Save player names
    outFile << player1 << endl;
    outFile << player2 << endl;
    // Save board state
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            outFile << board[y][x] << " ";
        }
        outFile << endl;
    }
    // Save scores and move count
    outFile << whiteScore << endl;
    outFile << blackScore << endl;
    outFile.close();
    cout << "Game saved successfully!" << endl;
}
// Function to load the game state from a file
bool loadGame(const string& filePath, string& player1, string& player2, int board[BOARD_SIZE][BOARD_SIZE], int& whiteScore, int& blackScore) {
    ifstream inFile(filePath);
    if (!inFile) {
        cerr << "Error opening file for loading!" << endl;
        return false;
    }
    // Load player names
    getline(inFile, player1);
    getline(inFile, player2);
    // Load board state
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            inFile >> board[y][x];
        }
    }
    // Load scores and move count
    inFile >> whiteScore;
    inFile >> blackScore;
    inFile.close();
    cout << "Game loaded successfully!" << endl;
    return true;
}
//main function
int main() {
    Font font;
    if (!font.loadFromFile("D:/assets/New/Campus A.ttf")) { // Load a font
        cerr << "Failed to load font!" << endl;
        return false;
    }
    bool isWhiteTurn = true;
    Text whiteScoreText("White Score: 0", font, 30);
    Text blackScoreText("Black Score: 0", font, 30);
    whiteScoreText.setPosition(50, 650); // Position for white score
    blackScoreText.setPosition(360, 650); // Position for black score
    string player1, player2;
    if (!startMenu(player1, player2)) {
        cout << "Game cannot start. Exiting..." << endl;
        return 0;
    }
    RenderWindow window(VideoMode(WINDOW_SIZE, WINDOW_SIZE + 50), "Chess by DANYAL & ABDULLAH");
    // Load promotion textures in main
    Texture whitePromotionTextures[4];
    Texture blackPromotionTextures[4];
    string whitePromotionFiles[4] = { "D:/assets/New/white_rook.png","D:/assets/New/white_knight.png","D:/assets/New/white_bishop.png","D:/assets/New/white_queen.png" };
    string blackPromotionFiles[4] = { "D:/assets/New/black_rook.png","D:/assets/New/black_knight.png","D:/assets/New/black_bishop.png","D:/assets/New/black_queen.png" };
    // Load white & black promotion textures
    for (int i = 0; i < 4; ++i) {
        if (!whitePromotionTextures[i].loadFromFile(whitePromotionFiles[i]) || (!blackPromotionTextures[i].loadFromFile(blackPromotionFiles[i]))) {
            cerr << "Failed to load " << whitePromotionFiles[i] << endl;
            return 1;
        }
    }
    Texture boardTexture = loadTexture("D:/assets/New/board3.png");
    Sprite boardSprite(boardTexture);
    boardSprite.setScale(WINDOW_SIZE / 640.0f, WINDOW_SIZE / 640.0f);
    Texture pieceTextures[12];
    string pieceFiles[12] = {
        "D:/assets/New/black_pawn.png", "D:/assets/New/black_rook.png", "D:/assets/New/black_knight.png", "D:/assets/New/black_bishop.png", "D:/assets/New/black_queen.png", "D:/assets/New/black_king.png",
        "D:/assets/New/white_pawn.png", "D:/assets/New/white_rook.png", "D:/assets/New/white_knight.png", "D:/assets/New/white_bishop.png", "D:/assets/New/white_queen.png", "D:/assets/New/white_king.png"
    };
    for (int i = 0; i < 12; ++i) {
        if (!pieceTextures[i].loadFromFile(pieceFiles[i])) {
            cerr << "Failed to load " << pieceFiles[i] << endl;
            return 1;
        }
    }
    // Load the sound buffers
    if ((!moveSoundBuffer.loadFromFile("D:/assets/New/piece_sound.mp3")) || (!backmusicBuffer.loadFromFile("D:/assets/New/Chess Loop Sound.mp3")) || (!startSoundBuffer.loadFromFile("D:/assets/New/start.mp3")) ||
        (!mateSoundBuffer.loadFromFile("D:/assets/New/mate.mp3")) || (!moveSoundBuffer.loadFromFile("D:/assets/New/piece_sound.mp3")) || (!promotionSoundBuffer.loadFromFile("D:/assets/New/promote.mp3")) ||
        (!captureSoundBuffer.loadFromFile("D:/assets/New/capture.mp3"))) {
        cerr << "Failed to load move sound" << endl;
        return 1;
    }
    captureSound.setBuffer(captureSoundBuffer);
    promotionSound.setBuffer(promotionSoundBuffer);
    startSound.setBuffer(startSoundBuffer);
    moveSound.setBuffer(moveSoundBuffer);
    checkSound.setBuffer(captureSoundBuffer);
    mateSound.setBuffer(mateSoundBuffer);
    backmusic.setBuffer(backmusicBuffer);
    backmusic.setVolume(20.f);
    backmusic.setLoop(true); // Set the music to loop
    backmusic.play(); // Start playing the background music
    int board[BOARD_SIZE][BOARD_SIZE];
    initializeBoard(board);
    bool isDragging = false;
    Vector2i dragPiece(-1, -1);
    Vector2i mouseOffset;
    Move legalMoves[MAX_MOVES];
    Text movecount("Black Score: 0", font, 30);
    movecount.setPosition(50, 650); // Position for white score
    startSound.play();
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            // Check for save game
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::S) {
                saveGame("D:/Chess Project 1st Semester/Save Game/chess-game.txt", player1, player2, board, whiteScore, blackScore);
            }
            // Check for load game
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::L) {
                if (loadGame("D:/Chess Project 1st Semester/Save Game/chess-game.txt", player1, player2, board, whiteScore, blackScore)) {
                    // After loading the game
                    whiteScoreText.setString("White Score: " + to_string(whiteScore));
                    blackScoreText.setString("Black Score: " + to_string(blackScore));
                    // Reset move history if needed
                    moveCount = 0; // Reset move count if you want to start fresh
                    // You may also want to reset the move history array if you are keeping track of moves
                }
            }
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePos = Mouse::getPosition(window);
                int x = mousePos.x / TILE_SIZE;
                int y = mousePos.y / TILE_SIZE;
                // Update score text
                whiteScoreText.setString("White Score: " + to_string(whiteScore));
                blackScoreText.setString("Black Score: " + to_string(blackScore));
                if (isWithinBounds(x, y) && board[y][x] != 0) {
                    bool isWhitePiece = board[y][x] >= 7;
                    if ((isWhiteTurn && isWhitePiece) || (!isWhiteTurn && !isWhitePiece)) {
                        isDragging = true;
                        dragPiece = { x, y };
                        mouseOffset = mousePos - Vector2i(x * TILE_SIZE, y * TILE_SIZE);
                        getLegalMoves(board[y][x], x, y, board, legalMoves, moveCount);
                        if (isKingInCheck(isWhiteTurn, board)) {
                            filterMovesToProtectKing(board, legalMoves, moveCount, x, y, isWhiteTurn);

                        }
                    }
                }
            }
            if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left) {
                if (isDragging) {
                    int fromX = 0; // Replace with actual logic to get fromX
                    int fromY = 0; // Replace with actual logic to get fromY
                    int toX = 1;   // Replace with actual logic to get toX
                    int toY = 1;   // Replace with actual logic to get toY
                    Vector2i mousePos = Mouse::getPosition(window);
                    int newX = mousePos.x / TILE_SIZE;
                    int newY = mousePos.y / TILE_SIZE;
                    if (isMoveLegal(newX, newY, legalMoves, moveCount)) {
                        // Simulate the move
                        int tempPiece = board[newY][newX];
                        if (tempPiece != 0) { // If a piece is captured
                            if (tempPiece > 6) { // Black piece
                                blackScore += getPieceValue(tempPiece);
                            }
                            else { // White piece
                                whiteScore += getPieceValue(tempPiece);
                            }
                            captureSound.play(); // Play capture sound
                        }
                        board[newY][newX] = board[dragPiece.y][dragPiece.x];
                        board[dragPiece.y][dragPiece.x] = 0;
                        promotePawn(newX, newY, board, isWhiteTurn, whitePromotionTextures, blackPromotionTextures);
                        moveSound.play();
                        // Check if the move leaves the king in check
                        if (isKingInCheck(isWhiteTurn, board)) {
                            // Undo the move
                            board[dragPiece.y][dragPiece.x] = board[newY][newX];
                            board[newY][newX] = tempPiece;
                        }
                        else {
                            // Check if the opponent's king is in check or checkmate
                            if (isKingInCheck(!isWhiteTurn, board)) {
                                checkSound.play();
                                cout << (isWhiteTurn ? "Black" : "White") << "'s king is in check!" << endl;
                                if (isCheckmate(!isWhiteTurn, board)) {
                                    mateSound.play();
                                    string str = "checkmate";
                                    new_window(395, 300, 2, str, "D:/assets/New/checkmate.png");
                                    cout << (isWhiteTurn ? player1 : player2) << " congratulations!!!!!" << endl;
                                    cout << (isWhiteTurn ? player2 : player1) << " is in checkmate " << endl;
                                    window.close(); // End the game
                                }
                            }
                            // Stalemate feature
                            if (!isKingInCheck(!isWhiteTurn, board)) {
                                bool hasLegalMove = false;
                                for (int y = 0; y < BOARD_SIZE && !hasLegalMove; ++y) {
                                    for (int x = 0; x < BOARD_SIZE && !hasLegalMove; ++x) {
                                        int piece = board[y][x];
                                        if ((!isWhiteTurn && piece >= 7) || (isWhiteTurn && piece > 0 && piece <= 6)) {
                                            Move legalMoves[MAX_MOVES];
                                            int moveCount = 0;
                                            getLegalMoves(piece, x, y, board, legalMoves, moveCount);
                                            filterMovesToProtectKing(board, legalMoves, moveCount, x, y, !isWhiteTurn);
                                            if (moveCount > 0) {
                                                hasLegalMove = true;
                                            }
                                        }
                                    }
                                }
                                if (!hasLegalMove) {
                                    mateSound.play();
                                    string str = "stalemate";
                                    new_window(250, 250, 2, str, "D:/assets/New/stalemate.png");
                                    cout << "Stalemate! Game over." << endl;
                                    window.close(); // End the game
                                }
                            }
                            isWhiteTurn = !isWhiteTurn; // Alternate turn
                        }
                    }
                    isDragging = false;
                    dragPiece = { -1, -1 };
                }
            }
        }
        window.clear(Color::Black);
        window.draw(boardSprite);
        // Highlight the king if in check
        if (isKingInCheck(isWhiteTurn, board)) {
            RectangleShape checkHighlight(Vector2f(TILE_SIZE, TILE_SIZE));
            checkHighlight.setFillColor(Color(255, 0, 0, 128)); // Semi-transparent red
            checkHighlight.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
            int kingX = -1, kingY = -1;
            for (int y = 0; y < BOARD_SIZE; ++y) {
                for (int x = 0; x < BOARD_SIZE; ++x) {
                    if ((isWhiteTurn && board[y][x] == 12) || (!isWhiteTurn && board[y][x] == 6)) {
                        kingX = x;
                        kingY = y;
                        break;
                    }
                }
            }
            if (kingX != -1 && kingY != -1) { // Only draw if the king is found
                checkHighlight.setPosition(kingX * TILE_SIZE + TILE_SIZE / 2, kingY * TILE_SIZE + TILE_SIZE / 2);
                window.draw(checkHighlight);
            }
        }
        // Highlight legal moves
        if (isDragging) {
            for (int i = 0; i < moveCount; ++i) {
                RectangleShape moveHighlight(Vector2f(TILE_SIZE, TILE_SIZE));
                moveHighlight.setFillColor(Color(0, 255, 0, 128)); // Semi-transparent green
                moveHighlight.setPosition(legalMoves[i].x * TILE_SIZE, legalMoves[i].y * TILE_SIZE);
                window.draw(moveHighlight);
            }
        }
        // Draw pieces
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                int piece = board[y][x];
                if (piece != 0) {
                    Sprite pieceSprite(pieceTextures[piece - 1]);
                    pieceSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                    window.draw(pieceSprite);
                }
            }
        }
        if (isDragging && dragPiece.x != -1) {
            int piece = board[dragPiece.y][dragPiece.x];
            if (piece != 0) {
                Sprite pieceSprite(pieceTextures[piece - 1]);
                Vector2i mousePos = Mouse::getPosition(window);
                pieceSprite.setPosition(mousePos.x - mouseOffset.x, mousePos.y - mouseOffset.y);
                window.draw(pieceSprite);
            }
        }
        // Draw scores
        window.draw(whiteScoreText);
        window.draw(blackScoreText);
        window.display();
    }
    return 0;
}