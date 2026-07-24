#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIMS 4
#define NUM_CARDS 81
#define NUM_PERMS 24
#define NUM_SIGNS 16
#define NUM_SYMS (NUM_PERMS * NUM_SIGNS)

#define MAX_BOARD 32
#define MAX_RESULTS_BY_LEN 32

typedef struct {
    uint8_t cards[MAX_BOARD];
    int len;
} Board;

typedef struct {
    bool occupied;
    Board board;
} BoardSetSlot;

typedef struct {
    BoardSetSlot *slots;
    size_t len;
    size_t cap;
} BoardSet;

typedef struct {
    Board *items;
    size_t head;
    size_t len;
    size_t cap;
} BoardQueue;

/* ============================================================
   Card helpers
   ============================================================ */

int card_id(int c[DIMS]) {
    return c[0] * 27 + c[1] * 9 + c[2] * 3 + c[3];
}

void id_to_coords(int id, int c[DIMS]) {
    c[0] = id / 27;
    id %= 27;

    c[1] = id / 9;
    id %= 9;

    c[2] = id / 3;
    c[3] = id % 3;
}

void print_card(uint8_t id) {
    int c[DIMS];
    id_to_coords(id, c);
    printf("(%d,%d,%d,%d)", c[0], c[1], c[2], c[3]);
}

void print_board(const Board *b) {
    printf("{");
    for (int i = 0; i < b->len; i++) {
        print_card(b->cards[i]);
        if (i + 1 < b->len) {
            printf(", ");
        }
    }
    printf("}");
}

/* ============================================================
   Board helpers
   ============================================================ */

int cmp_u8(const void *a, const void *b) {
    uint8_t x = *(const uint8_t *)a;
    uint8_t y = *(const uint8_t *)b;
    return (int)x - (int)y;
}

void sort_board(Board *b) {
    qsort(b->cards, b->len, sizeof(uint8_t), cmp_u8);
}

bool board_contains(const Board *b, uint8_t card) {
    for (int i = 0; i < b->len; i++) {
        if (b->cards[i] == card) {
            return true;
        }
    }
    return false;
}

Board board_add_card(const Board *b, uint8_t card) {
    Board out = *b;

    if (out.len >= MAX_BOARD) {
        fprintf(stderr, "Board exceeded MAX_BOARD\n");
        exit(1);
    }

    out.cards[out.len] = card;
    out.len++;
    sort_board(&out);

    return out;
}

int board_compare(const Board *a, const Board *b) {
    if (a->len != b->len) {
        return a->len - b->len;
    }

    for (int i = 0; i < a->len; i++) {
        if (a->cards[i] != b->cards[i]) {
            return (int)a->cards[i] - (int)b->cards[i];
        }
    }

    return 0;
}

bool board_equal(const Board *a, const Board *b) {
    return board_compare(a, b) == 0;
}

/* ============================================================
   Board hash table
   ============================================================ */

uint64_t board_hash(const Board *b) {
    /*
       FNV-1a style hash.

       Assumes board cards are sorted. That matters because:
       {1, 5, 9} and {9, 1, 5} should hash the same only after sorting.
    */
    uint64_t h = 1469598103934665603ULL;

    h ^= (uint8_t)b->len;
    h *= 1099511628211ULL;

    for (int i = 0; i < b->len; i++) {
        h ^= b->cards[i];
        h *= 1099511628211ULL;
    }

    return h;
}

void board_set_init(BoardSet *set) {
    set->len = 0;
    set->cap = 2048;
    set->slots = calloc(set->cap, sizeof(BoardSetSlot));

    if (set->slots == NULL) {
        fprintf(stderr, "calloc failed\n");
        exit(1);
    }
}

void board_set_free(BoardSet *set) {
    free(set->slots);
    set->slots = NULL;
    set->len = 0;
    set->cap = 0;
}

bool board_set_insert_no_resize(BoardSet *set, Board b) {
    uint64_t h = board_hash(&b);
    size_t idx = h % set->cap;

    while (set->slots[idx].occupied) {
        if (board_equal(&set->slots[idx].board, &b)) {
            return false;
        }

        idx = (idx + 1) % set->cap;
    }

    set->slots[idx].occupied = true;
    set->slots[idx].board = b;
    set->len++;

    return true;
}

void board_set_resize(BoardSet *set, size_t new_cap) {
    BoardSet old = *set;

    set->len = 0;
    set->cap = new_cap;
    set->slots = calloc(set->cap, sizeof(BoardSetSlot));

    if (set->slots == NULL) {
        fprintf(stderr, "calloc failed during resize\n");
        exit(1);
    }

    for (size_t i = 0; i < old.cap; i++) {
        if (old.slots[i].occupied) {
            board_set_insert_no_resize(set, old.slots[i].board);
        }
    }

    free(old.slots);
}

bool board_set_add(BoardSet *set, Board b) {
    /*
       Resize if load factor would exceed about 70%.
    */
    if ((set->len + 1) * 10 > set->cap * 7) {
        board_set_resize(set, set->cap * 2);
    }

    return board_set_insert_no_resize(set, b);
}

bool board_set_contains(const BoardSet *set, const Board *b) {
    uint64_t h = board_hash(b);
    size_t idx = h % set->cap;

    while (set->slots[idx].occupied) {
        if (board_equal(&set->slots[idx].board, b)) {
            return true;
        }

        idx = (idx + 1) % set->cap;
    }

    return false;
}

/* ============================================================
   Queue
   ============================================================ */

void queue_init(BoardQueue *q) {
    q->head = 0;
    q->len = 0;
    q->cap = 2048;
    q->items = malloc(q->cap * sizeof(Board));

    if (q->items == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
}

void queue_free(BoardQueue *q) {
    free(q->items);
    q->items = NULL;
    q->head = 0;
    q->len = 0;
    q->cap = 0;
}

bool queue_empty(const BoardQueue *q) {
    return q->head >= q->len;
}

size_t queue_size(const BoardQueue *q) {
    return q->len - q->head;
}

void queue_compact(BoardQueue *q) {
    if (q->head == 0) {
        return;
    }

    size_t remaining = q->len - q->head;
    memmove(q->items, q->items + q->head, remaining * sizeof(Board));

    q->head = 0;
    q->len = remaining;
}

void queue_push(BoardQueue *q, Board b) {
    if (q->len == q->cap) {
        if (q->head > 0) {
            queue_compact(q);
        }

        if (q->len == q->cap) {
            q->cap *= 2;

            Board *tmp = realloc(q->items, q->cap * sizeof(Board));

            if (tmp == NULL) {
                fprintf(stderr, "realloc failed\n");
                free(q->items);
                exit(1);
            }

            q->items = tmp;
        }
    }

    q->items[q->len++] = b;
}

Board queue_pop(BoardQueue *q) {
    if (queue_empty(q)) {
        fprintf(stderr, "queue_pop from empty queue\n");
        exit(1);
    }

    return q->items[q->head++];
}

/* ============================================================
   SET logic
   ============================================================ */

uint8_t find_third_id(uint8_t a_id, uint8_t b_id) {
    int a[DIMS];
    int b[DIMS];
    int c[DIMS];

    id_to_coords(a_id, a);
    id_to_coords(b_id, b);

    for (int i = 0; i < DIMS; i++) {
        if (a[i] == b[i]) {
            c[i] = a[i];
        } else {
            c[i] = 3 - a[i] - b[i];
        }
    }

    return (uint8_t)card_id(c);
}

bool hash_colinearity_id(uint8_t a_id, uint8_t b_id, uint8_t c_id) {
    int a[DIMS];
    int b[DIMS];
    int c[DIMS];

    id_to_coords(a_id, a);
    id_to_coords(b_id, b);
    id_to_coords(c_id, c);

    for (int i = 0; i < DIMS; i++) {
        if ((a[i] + b[i] + c[i]) % 3 != 0) {
            return false;
        }
    }

    return true;
}

int board_check(const Board *b) {
    int sets = 0;

    for (int i = 0; i < b->len - 2; i++) {
        for (int j = i + 1; j < b->len - 1; j++) {
            for (int k = j + 1; k < b->len; k++) {
                if (hash_colinearity_id(b->cards[i], b->cards[j], b->cards[k])) {
                    sets++;
                }
            }
        }
    }

    return sets;
}

void get_possible_nexts(const Board *board, uint8_t nexts[NUM_CARDS], int *next_count) {
    bool cant[NUM_CARDS] = {false};
    bool in_board[NUM_CARDS] = {false};

    for (int i = 0; i < board->len; i++) {
        in_board[board->cards[i]] = true;
    }

    for (int i = 0; i < board->len - 1; i++) {
        for (int j = i + 1; j < board->len; j++) {
            uint8_t third = find_third_id(board->cards[i], board->cards[j]);
            cant[third] = true;
        }
    }

    *next_count = 0;

    for (int card = 0; card < NUM_CARDS; card++) {
        if (!in_board[card] && !cant[card]) {
            nexts[*next_count] = (uint8_t)card;
            (*next_count)++;
        }
    }
}

/* ============================================================
   Symmetry table
   ============================================================ */

int transform_card(int point_id, int perm[DIMS], int signs[DIMS]) {
    int p[DIMS];
    int centered[DIMS];
    int transformed[DIMS];

    id_to_coords(point_id, p);

    for (int k = 0; k < DIMS; k++) {
        centered[k] = p[k] - 1;
    }

    for (int k = 0; k < DIMS; k++) {
        transformed[k] = signs[k] * centered[perm[k]] + 1;
    }

    return card_id(transformed);
}

void make_signs(int signs[NUM_SIGNS][DIMS]) {
    for (int mask = 0; mask < NUM_SIGNS; mask++) {
        for (int k = 0; k < DIMS; k++) {
            signs[mask][k] = ((mask >> k) & 1) ? -1 : 1;
        }
    }
}

void build_sym_table(uint8_t sym_table[NUM_SYMS][NUM_CARDS]) {
    int signs[NUM_SIGNS][DIMS];
    make_signs(signs);

    int perms[NUM_PERMS][DIMS] = {
        {0, 1, 2, 3}, {0, 1, 3, 2}, {0, 2, 1, 3}, {0, 2, 3, 1},
        {0, 3, 1, 2}, {0, 3, 2, 1},

        {1, 0, 2, 3}, {1, 0, 3, 2}, {1, 2, 0, 3}, {1, 2, 3, 0},
        {1, 3, 0, 2}, {1, 3, 2, 0},

        {2, 0, 1, 3}, {2, 0, 3, 1}, {2, 1, 0, 3}, {2, 1, 3, 0},
        {2, 3, 0, 1}, {2, 3, 1, 0},

        {3, 0, 1, 2}, {3, 0, 2, 1}, {3, 1, 0, 2}, {3, 1, 2, 0},
        {3, 2, 0, 1}, {3, 2, 1, 0}
    };

    int sym_idx = 0;

    for (int p = 0; p < NUM_PERMS; p++) {
        for (int s = 0; s < NUM_SIGNS; s++) {
            for (int card = 0; card < NUM_CARDS; card++) {
                sym_table[sym_idx][card] =
                    (uint8_t)transform_card(card, perms[p], signs[s]);
            }

            sym_idx++;
        }
    }
}

Board min_rotation(const Board *board, uint8_t sym_table[NUM_SYMS][NUM_CARDS]) {
    Board best;
    best.len = board->len;

    for (int i = 0; i < board->len; i++) {
        best.cards[i] = sym_table[0][board->cards[i]];
    }

    sort_board(&best);

    for (int s = 1; s < NUM_SYMS; s++) {
        Board trial;
        trial.len = board->len;

        for (int i = 0; i < board->len; i++) {
            trial.cards[i] = sym_table[s][board->cards[i]];
        }

        sort_board(&trial);

        if (board_compare(&trial, &best) < 0) {
            best = trial;
        }
    }

    return best;
}

/* ============================================================
   Search
   ============================================================ */

void search_from_start(
    const Board *start,
    uint8_t sym_table[NUM_SYMS][NUM_CARDS],
    BoardSet results_by_len[MAX_RESULTS_BY_LEN]
) {
    Board canon_start = min_rotation(start, sym_table);

    BoardSet seen;
    board_set_init(&seen);
    board_set_add(&seen, canon_start);

    BoardQueue q;
    queue_init(&q);
    queue_push(&q, canon_start);

    int longest = canon_start.len;

    while (!queue_empty(&q)) {
        Board trial = queue_pop(&q);

        if (trial.len > longest) {
            longest = trial.len;
            printf("longest: %d, queue: %zu, seen: %zu\n",
                   longest, queue_size(&q), seen.len);
            fflush(stdout);
        }

        uint8_t nexts[NUM_CARDS];
        int next_count = 0;

        get_possible_nexts(&trial, nexts, &next_count);

        if (next_count == 0) {
            if (trial.len >= 0 && trial.len < MAX_RESULTS_BY_LEN) {
                board_set_add(&results_by_len[trial.len], trial);
            }
        } else {
            for (int i = 0; i < next_count; i++) {
                Board new_board = board_add_card(&trial, nexts[i]);
                Board canon = min_rotation(&new_board, sym_table);

                /*
                   This is the hash-table equivalent of:

                   if canon not in seen:
                       seen.add(canon)
                       q.append(canon)
                */
                if (board_set_add(&seen, canon)) {
                    queue_push(&q, canon);
                }
            }
        }
    }

    board_set_free(&seen);
    queue_free(&q);
}

void build_initial_pairs(
    BoardSet *init_pairs,
    uint8_t sym_table[NUM_SYMS][NUM_CARDS]
) {
    for (int a = 0; a < NUM_CARDS - 1; a++) {
        for (int b = a + 1; b < NUM_CARDS; b++) {
            Board pair;
            pair.len = 2;
            pair.cards[0] = (uint8_t)a;
            pair.cards[1] = (uint8_t)b;
            sort_board(&pair);

            Board canon = min_rotation(&pair, sym_table);
            board_set_add(init_pairs, canon);
        }
    }
}

void print_first_board_in_set(const BoardSet *set) {
    for (size_t i = 0; i < set->cap; i++) {
        if (set->slots[i].occupied) {
            print_board(&set->slots[i].board);
            return;
        }
    }
}

/* ============================================================
   Main
   ============================================================ */

int main(void) {
    uint8_t sym_table[NUM_SYMS][NUM_CARDS];

    printf("Building symmetry table...\n");
    build_sym_table(sym_table);
    printf("Built %d symmetries\n", NUM_SYMS);

    BoardSet init_pairs;
    board_set_init(&init_pairs);

    build_initial_pairs(&init_pairs, sym_table);

    printf("Canonical initial pairs: %zu\n", init_pairs.len);

    BoardSet results_by_len[MAX_RESULTS_BY_LEN];

    for (int i = 0; i < MAX_RESULTS_BY_LEN; i++) {
        board_set_init(&results_by_len[i]);
    }

    size_t pair_number = 0;

    for (size_t slot = 0; slot < init_pairs.cap; slot++) {
        if (!init_pairs.slots[slot].occupied) {
            continue;
        }

        pair_number++;

        printf("\nStarting pair %zu / %zu: ", pair_number, init_pairs.len);
        print_board(&init_pairs.slots[slot].board);
        printf("\n");
        fflush(stdout);

        search_from_start(&init_pairs.slots[slot].board, sym_table, results_by_len);
    }

    printf("\nTerminal boards by length:\n");

    for (int len = 0; len < MAX_RESULTS_BY_LEN; len++) {
        if (results_by_len[len].len > 0) {
            printf("%d: %zu\n", len, results_by_len[len].len);
        }
    }

    printf("\nExample terminal boards:\n");

    for (int len = 0; len < MAX_RESULTS_BY_LEN; len++) {
        if (results_by_len[len].len > 0) {
            printf("\nLength %d, first board:\n", len);
            print_first_board_in_set(&results_by_len[len]);
            printf("\n");
        }
    }

    for (int i = 0; i < MAX_RESULTS_BY_LEN; i++) {
        board_set_free(&results_by_len[i]);
    }

    board_set_free(&init_pairs);

    return 0;
}