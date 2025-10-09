#pragma once

#include "../../engine/pipeline.hpp"
#include "../../deps/ZDev/collision.hpp"

struct Astroid {
public:
    Astroid(Pipeline& pipeline, Collision& collision);

    void tick();
    bool checkCollision();

    struct Stats {
        float startPos[2];
        float speed;
        float health;
        unsigned int ID;
    };

    vector<Stats> astroids;
private:
    Pipeline& pipeline;
    Collision& collision;
};