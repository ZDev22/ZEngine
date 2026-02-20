#include "zwindow.hpp"
#include "zengine.hpp"
#include "zcollide.hpp"
#include "zmath.hpp"
#include "ztext.hpp"

struct Game {
public:
    Game(ZWindow& zwindow, ma_engine& audio, Camera& camera)  : zwindow(zwindow), audio(audio), camera(camera) {
        loadFont("assets/fonts/Bullpen3D.ttf", 64.f);
        sprites[0].position[0] = -.8f;
        sprites[0].position[1] = -.8f;
        sprites[0].textureIndex = 1;
    }

    void tick() {
        createSprite(squareModel, 0, 0.f, 0.f, .02f, .02f, 0.f);
        for (unsigned int i = 1; i < sprites.size(); i++) {
            sprites[i].position[0] += .0001f;
            sprites[i].position[1] += .0001f;
        }
        sprites[0].setTexture(createText(0, std::to_string(sprites.size()), 64));
    }
private:
    ZWindow& zwindow;
    ma_engine& audio;
    Camera& camera;
};
