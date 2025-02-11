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
#include <stdbool.h>
#include "questions.h"
#include "players.h"
#include "jeopardy.h"

// Put macros or constants here using #define
#define BUFFER_LEN 256
#define NUM_PLAYERS 4

// Put global environment variables here

// Processes the answer from the user containing what is or who is and tokenizes it to retrieve the answer.
void tokenize(char *input, char **tokens) {
    char *token = strtok(input, " ");
    int i = 0;
    
    while (token != NULL) {
        tokens[i++] = token;
        token = strtok(NULL, " ");
    }
}

// Displays the game results for each player, their name and final score, ranked from first to last place
void show_results(player *players, int num_players) {
    // Sorting players based on their scores
    for (int i = 0; i < num_players - 1; i++) {
        for (int j = i + 1; j < num_players; j++) {
            if (players[i].score < players[j].score) {
                player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }

    // Displaying player names and scores
    printf("\nFinal Results:\n");
    for (int i = 0; i < num_players; i++) {
        printf("Player: %s, Score: %d\n", players[i].name, players[i].score);
    }
}

// Plays a round of Jeopardy by presenting questions and checking player answers
void play_round(player *players, int num_players, question *round_questions, int num_questions) {
    char buffer[BUFFER_LEN];
    char *tokens[2];  // To hold the player's answer (What is/Who is and the answer)
    int current_player = 0;

    // Ask questions to each player until all questions are answered
    for (int i = 0; i < num_questions; i++) {
        if (round_questions[i].answered) {
            continue;  // Skip if the question is already answered
        }

        // Display question for current player
        printf("Player %s's turn:\n", players[current_player].name);
        printf("Category: %s\n", round_questions[i].category);
        printf("Question: %s\n", round_questions[i].question);

        // Get the player's answer
        printf("Your answer: ");
        fgets(buffer, BUFFER_LEN, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // Remove newline

        // Tokenize the player's input
        tokenize(buffer, tokens);
        char *answer = tokens[1];  // The answer is the second token

        // Check if the answer is correct
        if (check_answer(&round_questions[i], answer)) {
            printf("Correct!\n");
            // Update player score
            players[current_player].score += round_questions[i].value;
        } else {
            printf("Incorrect! The correct answer was: %s\n", round_questions[i].answer);
            // Deduct score for incorrect answer
            players[current_player].score -= round_questions[i].value;
        }

        // Mark the question as answered
        round_questions[i].answered = true;

        // Move to the next player
        current_player = (current_player + 1) % num_players;
    }
}

// Checks if the answer provided by the player is correct for the given question
bool check_answer(question *q, char *input) {
    // Convert the input to lowercase to handle case insensitivity
    for (int i = 0; input[i]; i++) {
        input[i] = tolower(input[i]);
    }

    // Check if the player's answer matches the correct answer
    if (strstr(input, q->answer) != NULL) {
        return true;  // Correct answer
    }

    return false;  // Incorrect answer
}


int main(int argc, char *argv[]) {
    player players[NUM_PLAYERS];
    char buffer[BUFFER_LEN] = { 0 };
    int num_players = NUM_PLAYERS;

    // Initialize the game
    initialize_game();

    // Prompt for players' names and store them
    for (int i = 0; i < num_players; i++) {
        printf("Enter name for Player %d: ", i + 1);
        fgets(players[i].name, MAX_LEN, stdin);
        players[i].name[strcspn(players[i].name, "\n")] = 0;  // Remove newline
        players[i].score = 0;  // Initialize player score
    }

    // Play Jeopardy round
    play_round(players, num_players, jeopardy_questions, NUM_QUESTIONS);

    // Play Double Jeopardy round
    play_round(players, num_players, double_jeopardy_questions, NUM_QUESTIONS);

    // Final Jeopardy round (only one question)
    printf("\nFinal Jeopardy Round:\n");
    printf("Category: %s\n", final_jeopardy_question.category);
    printf("Question: %s\n", final_jeopardy_question.question);
    printf("Your answer: ");
    fgets(buffer, BUFFER_LEN, stdin);
    buffer[strcspn(buffer, "\n")] = 0;  // Remove newline

    // Check if the answer is correct and update the score
    if (check_answer(&final_jeopardy_question, buffer)) {
        printf("Correct!\n");
        players[0].score += final_jeopardy_question.value;  // Add to player's score (just an example with player[0])
    } else {
        printf("Incorrect! The correct answer was: %s\n", final_jeopardy_question.answer);
        players[0].score -= final_jeopardy_question.value;  // Deduct from score
    }

    // Show the final results
    show_results(players, num_players);

    return EXIT_SUCCESS;
}