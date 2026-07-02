#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Take in a string of 16 or 25 characters

// With the string, use a depth first search to go down every possible path,
// only stopping when I check the dictionary and there are no more possible words
// prefixed with the current words we have

// When I get to the point of no more possible words, I return back to the last time where 
// that wasn't true and start from the next possible letter

// In the loop, for every word that fully matches with the dictionary, I will store it in a
// linked list which automatically sorts by string length and alphabetical order.

// Start from the top left letter
// If we can, go up+left, next up, next up+right, next left, and so on

// Need an array of characters and then an array of bool values to see whether a letter
// has already been selected

size_t num_words = 0;

typedef struct ll_word {
    char* word;
    struct ll_word *next;
} ll_word;

ll_word *ll_word_append(ll_word *node, char *word) {
    if (node == NULL) {
        ll_word *new_node = malloc(sizeof(ll_word));
        new_node->word = strdup(word);
        new_node->next = NULL;
        return new_node;
    }
    ll_word *next_node = node->next;
    while (true) {
        if (next_node == NULL) {
            ll_word *new_node = malloc(sizeof(ll_word));
            new_node->word = strdup(word);
            new_node->next = NULL;
            next_node->next = new_node;
            return node;
        }
        next_node = node->next;
    }
}

// Insert the word so that it will be sorted in order of length and alphabetically
// We automatically go to the next word if the current one is before it in the alphabet
// or longer than it.
ll_word *insert_text_sorted(ll_word *node, char *word) {
    if (node == NULL) {
        ll_word *new_node = malloc(sizeof(ll_word));
        new_node->word = strdup(word);
        new_node->next = NULL;
        return new_node;
    }
    ll_word *curr_node = node;
    ll_word *prev_node = NULL;
    while (curr_node != NULL) {
        if (strlen(curr_node->word) > strlen(word)) {
            prev_node = curr_node;
            curr_node = curr_node->next;
        } else if (strlen(curr_node->word) == strlen(word) && strcmp(curr_node->word, word) < 0) {
            prev_node = curr_node;
            curr_node = curr_node->next;
        } else {
            ll_word *new_node = malloc(sizeof(ll_word));
            new_node->word = strdup(word);
            new_node->next = curr_node;
            if (prev_node != NULL) {
                prev_node->next = new_node;
                return node;
            } else {
                return new_node;
            }
        }
    }
    ll_word *new_node = malloc(sizeof(ll_word));
    new_node->word = strdup(word);
    new_node->next = curr_node;
    prev_node->next = new_node;
    return node;
}


typedef struct direction {
    int x;
    int y;
} direction;

char filename[] = "dictionary.txt";

char **load_vocabulary(char *filename) {
  FILE *fptr = fopen(filename, "r");
  char buffer[128];
  char **out = calloc(1, sizeof(char *));
  size_t i = 0;
  while (fgets(buffer, sizeof(buffer), fptr) != NULL) {
    if (strlen(buffer) > 3) {
        out = realloc(out, (i+1)*sizeof(char *));
        out[i] = strdup(buffer);
        out[i][strlen(out[i])-1] = '\0';
        i++;
    }
  }
  num_words = i;
  fclose(fptr);
  return out;
}

void free_vocabulary(char **vocabulary) {
  for (size_t i = 0; i < num_words; i++) {
    free(vocabulary[i]);
  }
  free(vocabulary);
}

direction directions[8] = {
    {-1, 0}, // Left
    {-1, 1}, // Up Left
    {0, 1},  // Up
    {1, 1},  // Up Right
    {1, 0},  // Right
    {1, -1}, // Down Right
    {0, -1}, // Down
    {-1, -1} // Down Left
};

bool is_prefix(char *input, char *word) {
    return (strncmp(input, word, strlen(input)) == 0);
}

// This function checks if any more words can be made from the input, NOT including if it
// is already a full word itself.
bool is_valid(char *input, char **vocabulary) {
    for (size_t i = 0; i < num_words; i++) {
        if (is_prefix(input, vocabulary[i]) && strlen(input) < strlen(vocabulary[i])) {
            return true;
        }
    }
    return false;
}

bool is_a_word(char *input, char **vocabulary) {
    for (size_t i = 0; i < num_words; i++) {
        if (strcmp(input, vocabulary[i]) == 0) {
            return true;
        }
    }
    return false;
}

void solve_position(char grid[4][4], bool selected[], size_t curr_row, size_t curr_col,
    char *curr_word, char **vocabulary, ll_word **results) {
    //char curr_letter = grid[curr_row][curr_col];
    selected[4*curr_row + curr_col] = true;
    for (size_t dir = 0; dir < 8; dir++) {
        char dir_word[32];
        int next_row = curr_row + directions[dir].x;
        int next_col = curr_col + directions[dir].y;
        // Check if the next character is on the board
        if (next_row < 0 || next_row >= 4 || next_col < 0 || next_col >= 4) {
            continue;
        }
        // Check if the next character has already been picked
        if (selected[4*next_row + next_col]) {
            continue;
        }
        strcpy(dir_word, curr_word);
        dir_word[strlen(curr_word)] = grid[next_row][next_col];
        dir_word[strlen(curr_word) + 1] = '\0';
        // Check if the current letters make up a word. If they do, add them to 
        // the linked list.
        if (is_a_word(dir_word, vocabulary)) {
            //printf("found word: %s\n", dir_word);
            *results = insert_text_sorted(*results, dir_word);
        }
        // Check if the current letters can be made into anything else. If they can,
        // solve their position. This should create a recursive loop that searches 
        // every possible word for every possible position on the board.
        if (is_valid(dir_word, vocabulary)) {
            solve_position(grid, selected, next_row, next_col, dir_word, vocabulary, results);
            selected[4*next_row + next_col] = false;
        } else {
            continue;
        }
    }
    selected[4*curr_row + curr_col] = false;
    return;
}



int main() {
    printf("starting main loop\n");
    //size_t num_words = 0;
    char **vocabulary = load_vocabulary(filename);
    char grid[4][4];
    bool selected_array[16] = { false };
    ll_word *results = NULL;
    // I need a function to fill the grid based on a string input from the user
    char *input = calloc(17, sizeof(char));
    printf("enter board\n");
    scanf("%s", input);
    while (strlen(input) != 16) {
        printf("enter board\n");
        scanf("%s", input);
    }
    printf("%s\n", input);
    for (size_t row = 0; row < 4; row++) {
        for (size_t col = 0; col < 4; col++) {
            printf("current character: %c\n", input[row*4 + col]);
            grid[row][col] = input[row*4 + col];
        }
    }
    // Once the grid is filled, call the solve function on it at position 0
    for (size_t row = 0; row < 4; row++) {
        for (size_t col = 0; col < 4; col++) {
            //printf("current character: %c\n", grid[row][col]);
            char this_word[2];
            this_word[0] = grid[row][col];
            this_word[1] = '\0';
            solve_position(grid, selected_array, row, col, this_word, vocabulary, &results);
            memset(selected_array, false, sizeof(selected_array));
        }
    }
    // A function that iterates through the entire list and frees each node at the same
    // time would be good for this.
    ll_word *curr_node = results;
    ll_word *temp;
    while (curr_node != NULL) {
        printf("%s\n", curr_node->word);
        temp = curr_node->next;
        free(curr_node->word);
        free(curr_node);
        curr_node = temp;
    }
}



