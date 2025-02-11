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
#include <time.h>
#include "cJSON.h"
#include "questions.h"

#define ANSI_RESET "\x1b[0m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_GREY "\x1b[90m"

question jeopardy_questions[NUM_QUESTIONS];
question double_jeopardy_questions[NUM_QUESTIONS];
question final_jeopardy_question;

// Reading from the JSON File
char *read_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = malloc(length + 1);
    if (data)
    {
        fread(data, 1, length, file);
        data[length] = '\0';
    }
    fclose(file);
    return data;
}

// Function to randomly select categories
void random_selection(int total, int select, int chosen[])
{
    bool picked[total];
    memset(picked, false, sizeof(picked));

    for (int i = 0; i < select; i++)
    {
        int rand_index;
        do
        {
            rand_index = rand() % total;
        } while (picked[rand_index]);
        picked[rand_index] = true;
        chosen[i] = rand_index;
    }
}

// Get the game questions for each round
//  Initializes the arrays of questions for the game
void initialize_game(void)
{
    srand(time(NULL)); // Seed randomizer
    char *json_data = read_file("board.json");
    if (!json_data)
        return;

    cJSON *json = cJSON_Parse(json_data);
    if (!json)
    {
        printf("Error parsing JSON\n");
        free(json_data);
        return;
    }

    // Get Jeopardy and Double Jeopardy categories
    cJSON *jeopardy = cJSON_GetObjectItem(json, "jeopardy");
    cJSON *double_jeopardy = cJSON_GetObjectItem(json, "double-jeopardy");
    cJSON *final_jeopardy = cJSON_GetObjectItem(json, "final-jeopardy");

    int num_jeopardy = cJSON_GetArraySize(jeopardy);
    int num_double_jeopardy = cJSON_GetArraySize(double_jeopardy);
    int num_final = cJSON_GetArraySize(final_jeopardy);

    int chosen_jeopardy[6], chosen_double_jeopardy[6];

    random_selection(num_jeopardy, 6, chosen_jeopardy);
    random_selection(num_double_jeopardy, 6, chosen_double_jeopardy);

    // Store Jeopardy round questions
    int index = 0;
    for (int i = 0; i < 6; i++)
    {
        cJSON *category = cJSON_GetArrayItem(jeopardy, chosen_jeopardy[i]);
        cJSON *questions_arr = cJSON_GetObjectItem(category, "questions");
        int num_questions = cJSON_GetArraySize(questions_arr);

        for (int j = 0; j < num_questions; j++)
        {
            cJSON *q = cJSON_GetArrayItem(questions_arr, j);
            strcpy(jeopardy_questions[index].category, cJSON_GetObjectItem(category, "name")->valuestring);
            strcpy(jeopardy_questions[index].question, cJSON_GetObjectItem(q, "question")->valuestring);
            strcpy(jeopardy_questions[index].answer, cJSON_GetObjectItem(q, "answer")->valuestring);
            jeopardy_questions[index].value = cJSON_GetObjectItem(q, "value")->valueint;
            jeopardy_questions[index].answered = false;
            index++;
        }
    }

    // Store Double Jeopardy round questions
    index = 0;
    for (int i = 0; i < 6; i++)
    {
        cJSON *category = cJSON_GetArrayItem(double_jeopardy, chosen_double_jeopardy[i]);
        cJSON *questions_arr = cJSON_GetObjectItem(category, "questions");
        int num_questions = cJSON_GetArraySize(questions_arr);

        for (int j = 0; j < num_questions; j++)
        {
            cJSON *q = cJSON_GetArrayItem(questions_arr, j);
            strcpy(double_jeopardy_questions[index].category, cJSON_GetObjectItem(category, "name")->valuestring);
            strcpy(double_jeopardy_questions[index].question, cJSON_GetObjectItem(q, "question")->valuestring);
            strcpy(double_jeopardy_questions[index].answer, cJSON_GetObjectItem(q, "answer")->valuestring);
            double_jeopardy_questions[index].value = cJSON_GetObjectItem(q, "value")->valueint;
            double_jeopardy_questions[index].answered = false;
            index++;
        }
    }

    // Store Final Jeopardy question
    int final_index = rand() % num_final;
    cJSON *final_category = cJSON_GetArrayItem(final_jeopardy, final_index);

    strcpy(final_jeopardy_question.category, cJSON_GetObjectItem(final_category, "category")->valuestring);
    strcpy(final_jeopardy_question.question, cJSON_GetObjectItem(final_category, "question")->valuestring);
    strcpy(final_jeopardy_question.answer, cJSON_GetObjectItem(final_category, "answer")->valuestring);
    final_jeopardy_question.value = 0; // No monetary value
    final_jeopardy_question.answered = false;

    // Clean up
    cJSON_Delete(json);
    free(json_data);
}

// Displays each of the remaining categories and question dollar values that have not been answered
void display_categories(void)
{
    printf(ANSI_BLUE "+-----------------------------+\n" ANSI_RESET);

    // Print category names in yellow
    for (int i = 0; i < NUM_CATEGORIES; i++)
    {
        printf(ANSI_BLUE "| " ANSI_YELLOW "%-12s" ANSI_BLUE " ", jeopardy_questions[i * 5].category);
    }
    printf("|\n");

    // Print separator
    printf(ANSI_BLUE "+-----------------------------+\n" ANSI_RESET);

    // Print question values
    for (int row = 0; row < 5; row++)
    { // 5 questions per category
        for (int col = 0; col < NUM_CATEGORIES; col++)
        {
            int index = col * 5 + row;
            if (jeopardy_questions[index].answered)
            {
                printf(ANSI_BLUE "| " ANSI_GREY "  ----   " ANSI_BLUE);
            }
            else
            {
                printf(ANSI_BLUE "| " ANSI_YELLOW "  $%-3d   " ANSI_BLUE, jeopardy_questions[index].value);
            }
        }
        printf("|\n");
    }

    // Print bottom border
    printf(ANSI_BLUE "+-----------------------------+\n" ANSI_RESET);
}

// Displays the question for the category and dollar value
void display_question(char *category, int value)
{
    for (int i = 0; i < NUM_QUESTIONS; i++)
    {
        if (strcmp(jeopardy_questions[i].category, category) == 0 && jeopardy_questions[i].value == value)
        {
            if (jeopardy_questions[i].answered)
            {
                printf("This question has already been answered.\n");
                return;
            }
            printf("\nCategory: %s\n", category);
            printf("For $%d: %s\n", value, jeopardy_questions[i].question);
            return;
        }
    }
    printf("Question not found.\n");
}

// Returns true if the answer is correct for the question for that category and dollar value
bool valid_answer(char *category, int value, char *answer)
{
    // Check Jeopardy questions
    for (int i = 0; i < NUM_QUESTIONS; i++)
    {
        if (strcmp(jeopardy_questions[i].category, category) == 0 &&
            jeopardy_questions[i].value == value)
        {
            // Compare answer strings
            if (strcmp(jeopardy_questions[i].answer, answer) == 0)
            {
                return true;
            }
        }
    }

    // Check Double Jeopardy questions
    for (int i = 0; i < NUM_QUESTIONS; i++)
    {
        if (strcmp(double_jeopardy_questions[i].category, category) == 0 &&
            double_jeopardy_questions[i].value == value)
        {
            // Compare answer strings
            if (strcmp(double_jeopardy_questions[i].answer, answer) == 0)
            {
                return true;
            }
        }
    }

    // If not found or answer is incorrect
    return false;
}

// Returns true if the question has already been answered
bool already_answered(char *category, int value)
{
    // Check Jeopardy questions
    for (int i = 0; i < NUM_QUESTIONS; i++)
    {
        if (strcmp(jeopardy_questions[i].category, category) == 0 &&
            jeopardy_questions[i].value == value)
        {
            if (jeopardy_questions[i].answered)
            {
                return true;
            }
        }
    }

    // Check Double Jeopardy questions
    for (int i = 0; i < NUM_QUESTIONS; i++)
    {
        if (strcmp(double_jeopardy_questions[i].category, category) == 0 &&
            double_jeopardy_questions[i].value == value)
        {
            if (double_jeopardy_questions[i].answered)
            {
                return true;
            }
        }
    }

    // If question is not answered
    return false;
}
