'''
Helpers for calculating answers to research questions
If possible, write these to be dimension-count agnostic...
'''

from itertools import combinations, permutations, product
from collections import defaultdict
from visualize_cards import Card

def print_cards_in_row(cards):
    '''
    takes a list of cards and displays them in a row
    '''
    for idx in range(5):
        line = [f'{card.vis[idx]} ' for card in cards]
        line = ''.join(line)
        print(line)

def combo_counter(n, k): 
    '''
    Calculates how many combinations exist given
    n- number of total things
    k- size of a kombination
    '''
    def recur_factorial(a, total=1):
        if a == 0:
            return 1
        elif a > 1:
            return recur_factorial(a-1, total*a)
        elif a == 1:
            return total
    
    n_fac = recur_factorial(n)
    k_fac = recur_factorial(k)

    return n_fac // (k_fac * recur_factorial((n - k)))

def display_grid(board):
    '''
    Creates a grid of 4 cards across
    '''
    chunks = [i for i in range(0, len(board), 4)] + [len(board)]

    for end_idx in range(1, len(chunks)):
        cards_idxs = [j for j in range(chunks[end_idx-1], chunks[end_idx])]
        print('' + '   '.join([f'{i}'.rjust(3) for i in cards_idxs]))
        cards = list(map(lambda x: Card(board[x]), cards_idxs))
        for line in range(5):
            line = [f'{card.vis[line]} ' for card in cards]
            print(''.join(line))

def count_monoattr(a, b, c):
    mono_attr = 0
    for i in range(4):
        if a[i] == b[i] and a[i] == c[i]:
            mono_attr += 1
    return mono_attr

check_hashmap =  {frozenset({0}), frozenset({1}), frozenset({2}), frozenset({0, 1, 2})}
def hash_colinearity(a, b, c):
    for i in range(len(a)):
        if frozenset((a[i], b[i], c[i])) in check_hashmap:
            continue
        else:
            return False
    return True


def board_check(board):
    '''
    Returns
    int number of sets in a given board
    the list of the found sets
    '''
    set_count = 0
    set_list = []
    for comb in combinations(range(len(board)), 3):
        cards = [board[c] for c in comb]
        set_found = hash_colinearity(*cards)
        if set_found:
            set_count += 1
            set_list.append(cards)
    return set_count, set_list


def combo_counter(n: int, k: int) -> int: 
    '''
    Returns the number of combinations 
    for [n] number items
    given [k] combination length
    '''
    def recur_factorial(a, total=1):
        if a == 0:
            return 1
        elif a > 1:
            return recur_factorial(a-1, total*a)
        elif a == 1:
            return total
    
    n_fac = recur_factorial(n)
    k_fac = recur_factorial(k)

    return n_fac // (k_fac * recur_factorial((n - k)))

from itertools import permutations, product

def build_sym_table(n=3, d=4):
    '''
    n is width along each axis
    d = dimensions
    '''
    off = (n - 1) / 2
    points = [tuple(p) for p in product(range(n), repeat=d)]

    def transform(p, perm, signs):
        c = [p[k] - off for k in range(d)]
        c = [signs[k] * c[perm[k]] for k in range(d)]
        return tuple(int(round(v + off)) for v in c)

    tables = []
    for perm in permutations(range(d)):
        for signs in product((1, -1), repeat=d):
            tables.append({p: transform(p, perm, signs) for p in points})
    print(f'For n={n}, d={d}: {len(tables)} symmetries')
    return tables


def all_symmetries(coords, sym_table):
    return {frozenset(t[p] for p in coords) for t in sym_table}

def min_rotation(board, sym_table):
    return min(tuple(sorted(b)) for b in all_symmetries(board, sym_table))

def dedup(boards, sym_table):
    return list(set(map(lambda x: min_rotation(x, sym_table), boards)))


def find_third(a, b):
    c = []
    for i in range(len(a)):
        if a[i] == b[i]:
            c.append(a[i])
        else: 
            c.append([j for j in (0, 1, 2) if j not in (a[i], b[i])][0])
    return tuple(c)

def flat_view(cards):
    every_card = {}
    for a in range(3):
        for b in range(3):
            for c in range(3):
                for d in range(3):
                    every_card[(a, b, c, d)] = '0'
    for card in cards:
        every_card[card] = 'X'
    
    grid = f'''{every_card[(0, 0, 0, 0)]} {every_card[(0, 0, 0, 1)]} {every_card[(0, 0, 0, 2)]} ┃ {every_card[(0, 1, 0, 0)]} {every_card[(0, 1, 0, 1)]} {every_card[(0, 1, 0, 2)]} ┃ {every_card[(0, 2, 0, 0)]} {every_card[(0, 2, 0, 1)]} {every_card[(0, 2, 0, 2)]}
{every_card[(0, 0, 1, 0)]} {every_card[(0, 0, 1, 1)]} {every_card[(0, 0, 1, 2)]} ┃ {every_card[(0, 1, 1, 0)]} {every_card[(0, 1, 1, 1)]} {every_card[(0, 1, 1, 2)]} ┃ {every_card[(0, 2, 1, 0)]} {every_card[(0, 2, 1, 1)]} {every_card[(0, 2, 1, 2)]}
{every_card[(0, 0, 2, 0)]} {every_card[(0, 0, 2, 1)]} {every_card[(0, 0, 2, 2)]} ┃ {every_card[(0, 1, 2, 0)]} {every_card[(0, 1, 2, 1)]} {every_card[(0, 1, 2, 2)]} ┃ {every_card[(0, 2, 2, 0)]} {every_card[(0, 2, 2, 1)]} {every_card[(0, 2, 2, 2)]}
━━━━━━╋━━━━━━━╋━━━━━━
{every_card[(1, 0, 0, 0)]} {every_card[(1, 0, 0, 1)]} {every_card[(1, 0, 0, 2)]} ┃ {every_card[(1, 1, 0, 0)]} {every_card[(1, 1, 0, 1)]} {every_card[(1, 1, 0, 2)]} ┃ {every_card[(1, 2, 0, 0)]} {every_card[(1, 2, 0, 1)]} {every_card[(1, 2, 0, 2)]}
{every_card[(1, 0, 1, 0)]} {every_card[(1, 0, 1, 1)]} {every_card[(1, 0, 1, 2)]} ┃ {every_card[(1, 1, 1, 0)]} {every_card[(1, 1, 1, 1)]} {every_card[(1, 1, 1, 2)]} ┃ {every_card[(1, 2, 1, 0)]} {every_card[(1, 2, 1, 1)]} {every_card[(1, 2, 1, 2)]}
{every_card[(1, 0, 2, 0)]} {every_card[(1, 0, 2, 1)]} {every_card[(1, 0, 2, 2)]} ┃ {every_card[(1, 1, 2, 0)]} {every_card[(1, 1, 2, 1)]} {every_card[(1, 1, 2, 2)]} ┃ {every_card[(1, 2, 2, 0)]} {every_card[(1, 2, 2, 1)]} {every_card[(1, 2, 2, 2)]}
━━━━━━╋━━━━━━━╋━━━━━━
{every_card[(2, 0, 0, 0)]} {every_card[(2, 0, 0, 1)]} {every_card[(2, 0, 0, 2)]} ┃ {every_card[(2, 1, 0, 0)]} {every_card[(2, 1, 0, 1)]} {every_card[(2, 1, 0, 2)]} ┃ {every_card[(2, 2, 0, 0)]} {every_card[(2, 2, 0, 1)]} {every_card[(2, 2, 0, 2)]}
{every_card[(2, 0, 1, 0)]} {every_card[(2, 0, 1, 1)]} {every_card[(2, 0, 1, 2)]} ┃ {every_card[(2, 1, 1, 0)]} {every_card[(2, 1, 1, 1)]} {every_card[(2, 1, 1, 2)]} ┃ {every_card[(2, 2, 1, 0)]} {every_card[(2, 2, 1, 1)]} {every_card[(2, 2, 1, 2)]}
{every_card[(2, 0, 2, 0)]} {every_card[(2, 0, 2, 1)]} {every_card[(2, 0, 2, 2)]} ┃ {every_card[(2, 1, 2, 0)]} {every_card[(2, 1, 2, 1)]} {every_card[(2, 1, 2, 2)]} ┃ {every_card[(2, 2, 2, 0)]} {every_card[(2, 2, 2, 1)]} {every_card[(2, 2, 2, 2)]}'''



    boxes = {
        1: [(0, 0, 0, 0), (0, 0, 0, 1), (0, 0, 0, 2), (0, 0, 1, 0), (0, 0, 1, 1), (0, 0, 1, 2), (0, 0, 2, 0), (0, 0, 2, 1), (0, 0, 2, 2)],
        2: [(0, 1, 0, 0), (0, 1, 0, 1), (0, 1, 0, 2), (0, 1, 1, 0), (0, 1, 1, 1), (0, 1, 1, 2), (0, 1, 2, 0), (0, 1, 2, 1), (0, 1, 2, 2)],
        3: [(0, 2, 0, 0), (0, 2, 0, 1), (0, 2, 0, 2), (0, 2, 1, 0), (0, 2, 1, 1), (0, 2, 1, 2), (0, 2, 2, 0), (0, 2, 2, 1), (0, 2, 2, 2)],
        4: [(1, 0, 0, 0), (1, 0, 0, 1), (1, 0, 0, 2), (1, 0, 1, 0), (1, 0, 1, 1), (1, 0, 1, 2), (1, 0, 2, 0), (1, 0, 2, 1), (1, 0, 2, 2)],
        5: [(1, 1, 0, 0), (1, 1, 0, 1), (1, 1, 0, 2), (1, 1, 1, 0), (1, 1, 1, 1), (1, 1, 1, 2), (1, 1, 2, 0), (1, 1, 2, 1), (1, 1, 2, 2)],
        6: [(1, 2, 0, 0), (1, 2, 0, 1), (1, 2, 0, 2), (1, 2, 1, 0), (1, 2, 1, 1), (1, 2, 1, 2), (1, 2, 2, 0), (1, 2, 2, 1), (1, 2, 2, 2)],
        7: [(2, 0, 0, 0), (2, 0, 0, 1), (2, 0, 0, 2), (2, 0, 1, 0), (2, 0, 1, 1), (2, 0, 1, 2), (2, 0, 2, 0), (2, 0, 2, 1), (2, 0, 2, 2)],
        8: [(2, 1, 0, 0), (2, 1, 0, 1), (2, 1, 0, 2), (2, 1, 1, 0), (2, 1, 1, 1), (2, 1, 1, 2), (2, 1, 2, 0), (2, 1, 2, 1), (2, 1, 2, 2)],
        9: [(2, 2, 0, 0), (2, 2, 0, 1), (2, 2, 0, 2), (2, 2, 1, 0), (2, 2, 1, 1), (2, 2, 1, 2), (2, 2, 2, 0), (2, 2, 2, 1), (2, 2, 2, 2)]}

    box_count = {}
    for k, v in boxes.items():
        count = 0
        for point in v:
            if every_card[point] == 'X':
                count += 1
        box_count[k] = count

    return grid, box_count
