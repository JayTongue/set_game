class Card:    
    def __init__(self, dims):
        vis_dict = {'numbers': [1, 2, 3],
            'colors': ['red', 'purple', 'green'],
            'fills': ['empty', 'stripe', 'solid'],
            'shapes': ['diamond', 'squiggle', 'capsule']} 
        
        self.number = vis_dict['numbers'][dims[0]]
        self.color = vis_dict['colors'][dims[1]]
        self.fill = vis_dict['fills'][dims[2]]
        self.shape = vis_dict['shapes'][dims[3]]

        self.vis = self._visualize()

    def _visualize(self):
        RED = "\033[31m"
        GREEN = "\033[32m"
        MAGENTA = "\033[35m"
        RESET = "\033[0m"

        #fill
        if self.fill == 'empty':
            middle = '☐'
        elif self.fill == 'stripe':
            middle = '▥'
        else:
            middle = '■'
        #shape
        if self.shape == 'diamond':
            icon = f'<{middle}>'
        elif self.shape == 'squiggle':
            icon = f'╔{middle}╝'
        else:
            icon = f'({middle})'
        #number
        if self.number == 1:
            skeleton = f'''┌───┐
│   │
│{icon}│
│   │
└───┘'''.splitlines()
        elif self.number == 2:
            skeleton = f'''┌───┐
│{icon}│
│   │
│{icon}│
└───┘'''.splitlines()
        else:
            skeleton = f'''┌───┐
│{icon}│
│{icon}│
│{icon}│
└───┘'''.splitlines()
        #color
        if self.color == 'red':
            card = [f'{RED}{s}{RESET}' for s in skeleton]
        elif self.color == 'purple':
            card = [f'{MAGENTA}{s}{RESET}' for s in skeleton]
        else:
            card = [f'{GREEN}{s}{RESET}' for s in skeleton]

        return card
    

def display_grid(twelve):
    for row_idxs in [list(range(4)), list(range(4, 8)), list(range(8, 12))]:
        # print(row_idxs)
        print('' + '   '.join([f'{i}'.rjust(3) for i in row_idxs]))
        cards = list(map(lambda x: Card(twelve[x]), row_idxs))
        for line in range(5):
            line = [f'{card.vis[line]} ' for card in cards]
            print(''.join(line))

def print_cards_in_row(cards):
    for idx in range(5):
        line = [f'{card.vis[idx]} ' for card in cards]
        line = ''.join(line)
        print(line)
