#include <stdbool.h>
#include "asteroids.h"

struct ship_action RandomAction() {
    int move = random() % 3 - 1;
    struct ship_action action = {move, 0};
    return action;
}

int distanceToNearestAsteroids(int field[][FIELD_WIDTH], int row, int ship_col) {
    int dist = -1;
    if (row < 0 || row >= FIELD_HEIGHT) {
        return dist;
    }
    for (int col = ship_col + 1; col < FIELD_WIDTH; col++) {
        if (field[row][col] == ASTEROID_VAL) {
            return col - ship_col;
        }
    }
    return FIELD_WIDTH;
}

struct ship_action GreedyAction(int field[][FIELD_WIDTH], void *ship_state) {
    // greedy action
    if (ship_state == NULL) {
        ship_state = malloc(sizeof(int));
        *(int*) ship_state = FIELD_HEIGHT / 2;
    }
    int ship_row = *(int*) ship_state;

    int farthest_row = -1;
    int max_distance = -1;
    bool has_asteroids = false;

    // Iterate over the lanes to find the farthest lane with asteroids
    for (int row = ship_row - 1; row <= ship_row + 1; row++) {
        int distance = -1;
        bool found_asteroid = false;
        for (int col = 1; col < FIELD_WIDTH; col++) {
            if (field[row][col] == ASTEROID_VAL) {
                distance = col;
                found_asteroid = true;
            } else if (!found_asteroid) distance = FIELD_WIDTH;
        }
        if (found_asteroid && distance > max_distance) {
            max_distance = distance;
            farthest_row = row;
            has_asteroids = true;
        }
    }

    // Update ship state with new row position based on the farthest lane
    int move;
    if (!has_asteroids || farthest_row == ship_row) {
        move = MOVE_NO;
    }
    else if (farthest_row > ship_row) {
        move = MOVE_DOWN;
        *(int *) ship_state = ship_row + 1;
    } else {
        move = MOVE_UP;
        *(int *) ship_state = ship_row - 1;
    }

    // Populate and return the ship action
    struct ship_action action = { move, ship_state };
    return action;
}

struct plannedOutcome{
    bool path_clear;
    int direction;
};

struct plannedOutcome PlannedAction(int field[][FIELD_WIDTH], const int ship_row, const int ship_col) {
    struct plannedOutcome outcome = {false, -2};
    struct plannedOutcome up = {false, 0};
    struct plannedOutcome forward = {false, 0};
    struct plannedOutcome down = {false, 0};

    if (ship_col == FIELD_WIDTH - 1) {
        outcome.path_clear = true;
        outcome.direction = MOVE_NO;
        return outcome;
    }



    int distanceForward = distanceToNearestAsteroids(field, ship_row, ship_col);
    if (distanceForward > SHIP_WIDTH) {
        forward = PlannedAction(field, ship_row, ship_col + 1);
        if(forward.path_clear){
            outcome.path_clear = true;
            outcome.direction = MOVE_NO;
            return outcome;
        }
    }

    if (ship_row - 1 >= 0) {
        int distanceUp = distanceToNearestAsteroids(field, ship_row + MOVE_UP, ship_col);
        if (distanceUp > SHIP_WIDTH) {
            up = PlannedAction(field, ship_row + MOVE_UP, ship_col + 1);
            if (up.path_clear){
                outcome.path_clear = true;
                outcome.direction = MOVE_UP;
                return outcome;
            }
        }
    }

    if (ship_row + 1 < FIELD_HEIGHT) {
        int distanceDown = distanceToNearestAsteroids(field, ship_row + MOVE_DOWN, ship_col);
        if (distanceDown > SHIP_WIDTH) {
            down = PlannedAction(field, ship_row + MOVE_DOWN, ship_col + 1);
            if (down.path_clear) {
                outcome.path_clear = true;
                outcome.direction = MOVE_DOWN;
                return outcome;
            }
        }
    }

    outcome.path_clear = false;
    outcome.direction = MOVE_NO;
    return outcome;
}



struct ship_action move_ship(int field[][FIELD_WIDTH], void *ship_state) {
    //return RandomAction();
    //return GreedyAction(field, ship_state);
    int ship_row;
    if (ship_state == NULL) {
        for (int row = 0; row < FIELD_HEIGHT; row++) {
            if (field[row][0] == SHIP_VAL) {
                ship_row = row;
            }
        }
    } else ship_row = *(int*) ship_state;

    struct plannedOutcome planned = PlannedAction(field, ship_row, 0);
    struct ship_action action = {planned.direction, ship_state};
    return action;
}