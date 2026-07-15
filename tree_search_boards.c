#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DIMS 4
#define NUM_CARDS 81
#define MAX_SETS 220

void make_all_cards(int *all_cards) {
    int card_idx = 0;
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            for (int c = 0; c < 3; c++) {
                for (int d = 0; d < 3; d++) {
                    int offset = card_idx * DIMS;

                    all_cards[offset] = a;
                    all_cards[offset + 1] = b;
                    all_cards[offset + 2] = c;
                    all_cards[offset + 3] = d;

                    card_idx++;
                }
            }
        }
    }
}

//need perm, not combo, do like flat array
void perm_threes(int *all_cards, uint8_t *lookup_table) {
    for (int a = 0; a < NUM_CARDS; a++) {
        for (int b = 0; b < NUM_CARDS; b++) {
            for (int c = 0; c < NUM_CARDS; c++) {
                if (a != b) {
                    bool actual_set = true;
                    int card_a = a * DIMS;
                    int card_b = b * DIMS;
                    int card_c = c * DIMS;

                    for (int i = 0; i < DIMS; i++) {
                        int mod_sum = all_cards[card_a+i] + 
                                      all_cards[card_b+i] + 
                                      all_cards[card_c+i];
                        if (mod_sum % 3 != 0) {
                            actual_set = false;
                            break;
                        }
                    }
                    if (actual_set == true) {
                        lookup_table[a*NUM_CARDS + b] = c;
                        break;
                    }
                }
            }
        }
    }
}

int count_sets_from_indices(int BOARD_SIZE, int *combo, uint8_t *lookup_table) {
    int sets = 0;
    for (int one = 0; one < BOARD_SIZE-2; one++) {
        for (int two = one + 1; two < BOARD_SIZE-1; two++) {
            for (int three = two + 1; three < BOARD_SIZE; three++) {
                int card_one = combo[one];
                int card_two = combo[two];
                int card_three = combo[three];

                if (lookup_table[card_one*NUM_CARDS+card_two] == card_three) sets++;
            }
        }
    }
    return sets;
}

bool next_combination(int *combo, int n, int k) {
    int pos = k-1;
    while (pos >= 0 && combo[pos] == n-k+pos) pos-- ;
    if (pos<0) return false;
    combo[pos]++;
    for (int j = pos+1; j<k; j++) {
        combo[j] = combo[j-1]+1;
    }
    return true;
}

int main(int argc, char *argv[]) {
    long long iterations = 100;

    iterations = atoll(argv[1]); //so pass in -1 if you want to run the whole thing
    int BOARD_SIZE = atoll(argv[2]);

    int *all_cards = malloc(NUM_CARDS * DIMS * sizeof(int));
    make_all_cards(all_cards);

    uint8_t *lookup_table = calloc(NUM_CARDS * NUM_CARDS, sizeof(uint8_t));
    
    perm_threes(all_cards, lookup_table);

    long long *all_count = calloc(MAX_SETS, sizeof(long long));

    int combo[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; i++) combo[i] = i;
    
    long long counter = 0;
    long long total = 70724320184700LL; 
    long long one_percent = total / 100;

    do {
        int set_count = count_sets_from_indices(BOARD_SIZE, combo, lookup_table);
        all_count[set_count]++;
        iterations--;
        counter++;  
        if (counter % one_percent == 0) {
            printf("%lld%%\n", (100LL * counter) / total);
            fflush(stdout);
        }
    } while (iterations != 0 && next_combination(combo, NUM_CARDS, BOARD_SIZE));

    for (int j = 0; j < MAX_SETS; j++) {
        if (all_count[j] > 0) {
            printf("%d: %lld\n", j, all_count[j]);
        }
    }

    free(all_cards);
    free(lookup_table);
    free(all_count);

    return 0;
}