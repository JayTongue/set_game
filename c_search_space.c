#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define N_CARDS 81
#define DIMS 4
#define BOARD_SIZE 12
#define MAX_SETS 220


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

int count_sets_from_indices(int *all_cards, int *idxs) {
    int sets = 0;

    for (int one = 0; one < 10; one++) {
        for (int two = one + 1; two < 11; two++) {
            for (int three = two + 1; three < 12; three++) {

                int c1 = idxs[one];
                int c2 = idxs[two];
                int c3 = idxs[three];

                bool is_set = true;

                for (int dim = 0; dim < DIMS; dim++) {
                    int x = all_cards[c1 * DIMS + dim];
                    int y = all_cards[c2 * DIMS + dim];
                    int z = all_cards[c3 * DIMS + dim];

                    if ((x + y + z) % 3 != 0) {
                        is_set = false;
                        break;
                    }
                }

                if (is_set) {
                    sets++;
                }
            }
        }
    }

    return sets;
}

int main(int argc, char *argv[]) {
    long long iterations = 100;

    if (argc > 1) {
        iterations = atoll(argv[1]);
    }
    int *all_cards = malloc(N_CARDS * DIMS * sizeof(int));

    if (all_cards == NULL) {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

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

    long long *all_count = calloc(MAX_SETS, sizeof(long long));

    if (all_count == NULL) {
        fprintf(stderr, "calloc failed\n");
        free(all_cards);
        return 1;
    }

    int combo[12];
    for (int i = 0; i < BOARD_SIZE; i++) combo[i] = i;
    
    do {
        int set_count = count_sets_from_indices(all_cards, combo);
        all_count[set_count]++;
        iterations --;
    } while (next_combination(combo, N_CARDS, BOARD_SIZE) && iterations > 0);

    for (int j = 0; j < MAX_SETS; j++) {
        if (all_count[j] > 0) {
            printf("%d: %lld\n", j, all_count[j]);
        }
    }

    free(all_cards);
    free(all_count);

    return 0;
}