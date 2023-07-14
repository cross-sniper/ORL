#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

struct RenderableObject {
    virtual ~RenderableObject() {}
    virtual void render(SDL_Renderer* renderer) = 0;
};

struct Object : public RenderableObject {
    int x;
    int y;
    int width;
    int height;
    int red;
    int green;
    int blue;

    void render(SDL_Renderer* renderer) override {
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = width;
        rect.h = height;

        SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};

struct Text : public RenderableObject {
    int x;
    int y;
    int r;
    int g;
    int b;
    int height;
    int width;
    std::string content;
    TTF_Font* font;

    Text(int fontSize) {
        TTF_Init();
        font = TTF_OpenFont("arimo.ttf", fontSize); // Use Arial font (change the font name as needed)
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        }
    }

    ~Text() {
        TTF_CloseFont(font);
    }

    void render(SDL_Renderer* renderer) override {
        SDL_Color textColor = { static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b) };

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, content.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = { x, y, width, height };
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
};

std::vector<std::unique_ptr<RenderableObject>> parseObjects(const std::string& filename) {
    std::vector<std::unique_ptr<RenderableObject>> objects;
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return objects;
    }

    std::string line;
    Object currentObject;
    Text currentText(24); // Use font size 24 (change as needed)

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue; // Skip empty lines and comments
        }

        if (line.find("Object {") != std::string::npos) {
            currentObject = Object();
        }
        else if (line.find("Position: (") != std::string::npos) {
            size_t startPos = line.find("(") + 1;
            size_t endPos = line.find(",", startPos);
            currentObject.x = std::stoi(line.substr(startPos, endPos - startPos));
            startPos = endPos + 2;
            endPos = line.find(")", startPos);
            currentObject.y = std::stoi(line.substr(startPos, endPos - startPos));
        }
        else if (line.find("Size: (") != std::string::npos) {
            size_t startPos = line.find("(") + 1;
            size_t endPos = line.find(",", startPos);
            currentObject.width = std::stoi(line.substr(startPos, endPos - startPos));
            startPos = endPos + 2;
            endPos = line.find(")", startPos);
            currentObject.height = std::stoi(line.substr(startPos, endPos - startPos));
        }
        else if (line.find("Color: (") != std::string::npos) {
            size_t startPos = line.find("(") + 1;
            size_t endPos = line.find(",", startPos);
            currentObject.red = std::stoi(line.substr(startPos, endPos - startPos));
            startPos = endPos + 2;
            endPos = line.find(",", startPos);
            currentObject.green = std::stoi(line.substr(startPos, endPos - startPos));
            startPos = endPos + 2;
            endPos = line.find(")", startPos);
            currentObject.blue = std::stoi(line.substr(startPos, endPos - startPos));

            objects.push_back(std::make_unique<Object>(currentObject));
        }
        else if (line.find("Text {") != std::string::npos) {
            while (std::getline(file, line)) {
                if (line.find("}") != std::string::npos) {
                    objects.push_back(std::make_unique<Text>(currentText));
                    break;
                }
                else if (line.find("Position: (") != std::string::npos) {
                    size_t startPos = line.find("(") + 1;
                    size_t endPos = line.find(",", startPos);
                    currentText.x = std::stoi(line.substr(startPos, endPos - startPos));
                    startPos = endPos + 2;
                    endPos = line.find(")", startPos);
                    currentText.y = std::stoi(line.substr(startPos, endPos - startPos));
                }
                else if (line.find("Color: (") != std::string::npos) {
                    size_t startPos = line.find("(") + 1;
                    size_t endPos = line.find(",", startPos);
                    currentText.r = std::stoi(line.substr(startPos, endPos - startPos));
                    startPos = endPos + 2;
                    endPos = line.find(",", startPos);
                    currentText.g = std::stoi(line.substr(startPos, endPos - startPos));
                    startPos = endPos + 2;
                    endPos = line.find(")", startPos);
                    currentText.b = std::stoi(line.substr(startPos, endPos - startPos));
                }
                else if (line.find("Size: (") != std::string::npos) {
                    size_t startPos = line.find("(") + 1;
                    size_t endPos = line.find(",", startPos);
                    currentText.width = std::stoi(line.substr(startPos, endPos - startPos));
                    startPos = endPos + 2;
                    endPos = line.find(")", startPos);
                    currentText.height = std::stoi(line.substr(startPos, endPos - startPos));
                }
                else if (line.find("Content: ") != std::string::npos) {
                    size_t startPos = line.find(":") + 1;
                    currentText.content = line.substr(startPos);
                }
            }
        }
    }

    file.close();

    return objects;
}

class RenderingEngine {
public:
    RenderingEngine(int fontSize)
        : screenWidth(800), screenHeight(600), font(fontSize)
    {
        // Initialize SDL
        SDL_Init(SDL_INIT_VIDEO);

        // Initialize SDL_ttf
        TTF_Init();

        // Create the SDL window
        window = SDL_CreateWindow("project x", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

        // Create the SDL renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }

    ~RenderingEngine() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        TTF_Quit();
        SDL_Quit();
    }

    void renderObjects(const std::vector<std::unique_ptr<RenderableObject>>& objects) {
        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render objects
        for (const auto& obj : objects) {
            obj->render(renderer);
        }

        // Update the screen
        SDL_RenderPresent(renderer);
    }

    void run() {
        std::vector<std::unique_ptr<RenderableObject>> objects = parseObjects("objects.lng");

        bool quit = false;
        SDL_Event event;
        while (!quit) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
                if (event.type == SDL_KEYDOWN) {
                    quit = true;
                }
            }

            renderObjects(objects);
        }
    }

private:
    int screenWidth;
    int screenHeight;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Text font;
};

int main() {
    RenderingEngine engine(24); // Use font size 24 (change as needed)
    engine.run();

    return 0;
}
