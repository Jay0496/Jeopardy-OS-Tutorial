/*
 * Tutorial 4 Jeopardy Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "players.h"

// Returns true if the player name matches one of the existing players
bool player_exists(player *players, int num_players, char *name) {
    for (int i = 0; i < num_players; i++) {
        if (strcmp(players[i].name, name) == 0) {
            return true;  // Player found
        }
    }
    return false;  // Player not found
}

// Go through the list of players and update the score for the player given their name
void update_score(player *players, int num_players, char *name, int score) {
    for (int i = 0; i < num_players; i++) {
        if (strcmp(players[i].name, name) == 0) {
            players[i].score += score;  // Update the player's score
            break;
        }
    }
}