#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

const int WIDTH = 800;
const int HEIGHT = 600;
const int BLOCK_SIZE = 20;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

struct SnakeBlock {
    int x, y;
};

class SnakeGame {
private:
    sf::RenderWindow window;
    std::vector<SnakeBlock> snake;
    SnakeBlock food;
    Direction dir;
    bool gameOver;
    int score;
    float speed;
    sf::SoundBuffer eatBuffer, gameOverBuffer;
    sf::Sound eatSound, gameOverSound;

public:
    SnakeGame() : window(sf::VideoMode(WIDTH, HEIGHT), "Snake Game"), dir(RIGHT), gameOver(false), score(0), speed(0.15f) {
        window.setFramerateLimit(60);
        snake.push_back({ WIDTH / 2, HEIGHT / 2 });
        spawnFood();
        srand(static_cast<unsigned>(time(0)));

        // Load sounds
        eatBuffer.loadFromFile("assets/eat.wav");
        gameOverBuffer.loadFromFile("assets/gameover.wav");
        eatSound.setBuffer(eatBuffer);
        gameOverSound.setBuffer(gameOverBuffer);
    }

    void spawnFood() {
        food.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
        food.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
    }

    void run() {
        sf::Clock clock;
        float timer = 0;

        while (window.isOpen()) {
            float time = clock.restart().asSeconds();
            timer += time;

            handleInput();

            if (!gameOver && timer >= speed) {
                timer = 0;
                moveSnake();
            }

            draw();
        }
    }

    void handleInput() {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::KeyPressed) {
                switch (e.key.code) {
                    case sf::Keyboard::Up:    if (dir != DOWN) dir = UP; break;
                    case sf::Keyboard::Down:  if (dir != UP) dir = DOWN; break;
                    case sf::Keyboard::Left:  if (dir != RIGHT) dir = LEFT; break;
                    case sf::Keyboard::Right: if (dir != LEFT) dir = RIGHT; break;
                }
            }
        }
    }

    void moveSnake() {
        SnakeBlock head = snake[0];
        switch (dir) {
            case LEFT:  head.x -= BLOCK_SIZE; break;
            case RIGHT: head.x += BLOCK_SIZE; break;
            case UP:    head.y -= BLOCK_SIZE; break;
            case DOWN:  head.y += BLOCK_SIZE; break;
            default: break;
        }

        // Check wall collision
        if (head.x < 0 || head.y < 0 || head.x >= WIDTH || head.y >= HEIGHT) {
            gameOver = true;
            gameOverSound.play();
            return;
        }

        // Check self collision
        for (auto& s : snake) {
            if (s.x == head.x && s.y == head.y) {
                gameOver = true;
                gameOverSound.play();
                return;
            }
        }

        snake.insert(snake.begin(), head);

        // Check food collision
        if (head.x == food.x && head.y == food.y) {
            eatSound.play();
            score++;
            if (speed > 0.05f) speed -= 0.01f; // Increase speed
            spawnFood();
        } else {
            snake.pop_back(); // normal move
        }
    }

    void draw() {
        window.clear(sf::Color::Black);

        // Draw food
        sf::RectangleShape foodShape(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));
        foodShape.setFillColor(sf::Color::Red);
        foodShape.setPosition(food.x, food.y);
        window.draw(foodShape);

        // Draw snake
        sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));
        for (size_t i = 0; i < snake.size(); i++) {
            block.setPosition(snake[i].x, snake[i].y);
            block.setFillColor(i == 0 ? sf::Color::Green : sf::Color(0, 180, 0));
            window.draw(block);
        }

        window.display();
    }
};

int main() {
    SnakeGame game;
    game.run();
    return 0;
}

