'''
Helpers for calculating answers to research questions
If possible, write these to be dimension-count agnostic...
'''

from itertools import combinations, permutations, product

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
    Returns the int number of sets in a given board
    '''
    sets = 0
    set_list = []
    for comb in combinations(range(len(board)), 3):
        cards = [board[c] for c in comb]
        set_found = hash_colinearity(*cards)
        if set_found:
            sets += 1
            set_list.append(cards)
    return sets, set_list


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

# def all_symmetries(coords, n=3):
#     '''
#     Returns a set of all possible rotations of a given 
#     for a normalized minimum set, use with min_rotation
#     '''
#     off = (n - 1) / 2

#     def transform(p, perm, signs):
#         c = [p[k] - off for k in range(3)]
#         c = [signs[k] * c[perm[k]] for k in range(3)]
#         return tuple(int(round(v + off)) for v in c)

#     results = set()
#     for perm in permutations(range(3)):
#         for signs in product((1, -1), repeat=3):
#             board = frozenset(transform(p, perm, signs) for p in coords)
#             results.add(board)
#     return results


def min_rotation(board, sym_table):
    return min(tuple(sorted(b)) for b in all_symmetries(board, sym_table))

def dedup(boards, sym_table):
    return list(set(map(lambda x: min_rotation(x, sym_table), boards)))


def find_third(a, b):
    c = []
    for i in range(3):
        if a[i] == b[i]:
            c.append(a[i])
        else: 
            c.append([j for j in (0, 1, 2) if j not in (a[i], b[i])][0])
    return tuple(c)