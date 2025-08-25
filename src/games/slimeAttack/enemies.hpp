#ifndef ENEMIES_HPP
#define ENEMIES_HPP

#define ENEMY_TYPE_SLIME 1
#define ENEMY_TYPE_BAT 2
#define ENEMY_TYPE_OGRE 3

struct Enemy {
    int health;
    int type;
};

void spawnEnemy(const int type);
void simulateEnemies();
bool isTouchingEnemies();

#endif