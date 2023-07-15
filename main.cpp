#include <SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "sound.cpp" // the module with the sound functions
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

std::vector<std::unique_ptr<RenderableObject>> parseObjects(const std::string& filename) {
    std::vector<std::unique_ptr<RenderableObject>> objects;
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return objects;
    }

    std::string line;
    Object currentObject;

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
    }

    file.close();

    return objects;
}

class RenderingEngine {
public:
    RenderingEngine()
        : screenWidth(800), screenHeight(600)
    {
        // Initialize SDL
        SDL_Init(SDL_INIT_VIDEO);

        // Create the SDL window
        window = SDL_CreateWindow("project x", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
        ALCdevice* device;
        ALCcontext* context;
        init_sound(&device, &context);
        play_sound("audio.wav");

        // Create the SDL renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }

    ~RenderingEngine() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

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
};

int main() {
    RenderingEngine engine;
    engine.run();

    return 0;
}
