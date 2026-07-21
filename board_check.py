from itertools import combinations

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


def combo_counter(n, k): 
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