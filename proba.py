from itertools import combinations, product

import math

SIZE = 5
DEBUG = True

def debug(*args, **kwargs):
    if DEBUG:
        print(*args, **kwargs)

def show_board(tiles, n=SIZE):
    print(' '.join(f"{str(i):>3}" for i in range(-1, n)))
    for index, row in enumerate(tiles):
        print(f"{str(index):>3}", end=' ')
        print(' '.join(f"{str(cell):>3}" for cell in row))

def calculate_probabilities_general():
    tiles = [[-1 for _ in range(SIZE)] for _ in range(SIZE)]
    tiles[5][4] = 2
    tiles[5][5] = 2
    tiles[6][4] = 1
    tiles[6][5] = 1
    tiles[6][6] = 1
    tiles[6][7] = 3
    tiles[7][4] = 1
    tiles[7][5] = 0
    tiles[7][6] = 0
    tiles[7][7] = 2
    tiles[8][4] = 3
    tiles[8][5] = 2
    tiles[8][6] = 1
    tiles[8][7] = 1

    show_board(tiles)
    
    mines = []
    for i in range(SIZE):
        for j in range(SIZE):
            if tiles[i][j] >= 0:
                mines.append((i, j))

    groups = []
    for mine in mines:
        group = {'mines': tiles[mine[0]][mine[1]], 'tiles': []}
        if mine[0]-1 >= 0 and mine[1]-1 >= 0: group['tiles'].append((mine[0]-1, mine[1]-1))
        if mine[1]-1 >= 0: group['tiles'].append((mine[0], mine[1]-1))
        if mine[0]+1 < SIZE and mine[1]-1 >= 0: group['tiles'].append((mine[0]+1, mine[1]-1))
        if mine[0]-1 >= 0: group['tiles'].append((mine[0]-1, mine[1]))
        if mine[0]+1 < SIZE: group['tiles'].append((mine[0]+1, mine[1]))
        if mine[0]-1 >= 0 and mine[1]+1 < SIZE: group['tiles'].append((mine[0]-1, mine[1]+1))
        if mine[1]+1 < SIZE: group['tiles'].append((mine[0], mine[1]+1))
        if mine[0]+1 < SIZE and mine[1]+1 < SIZE: group['tiles'].append((mine[0]+1, mine[1]+1))
        for tile in group['tiles'][:]:
            if tile in mines:
                group['tiles'].remove(tile)
        group['tiles'] = set(group['tiles'])
        groups.append(group)

    overlaps = {}

    for i in range(len(groups)):
        for j in range(i+1, len(groups)):
                overlaps[(i, j)] = groups[i]['tiles'] & groups[j]['tiles']
                if len(overlaps[(i, j)]) == 0:
                    del overlaps[(i, j)]

    probabilities = [[0 for _ in range(SIZE)] for _ in range(SIZE)]

    combinations_list = [combinations(groups[i]['tiles'], groups[i]['mines']) for i in range(len(groups))]
    total_valid_configurations = 0
    for combination in product(*combinations_list):
        mines_groups = [set(mines_group) for mines_group in combination]
        # print(mines_groups)
        
        valid_overlaps = True

        for overlap_groups, overlap in overlaps.items():
            # print(overlap_groups, overlap)
            # print(mines_groups[overlap_groups[0]])
            # print(mines_groups[overlap_groups[1]] & overlap)
            if len(mines_groups[overlap_groups[0]] | (mines_groups[overlap_groups[1]] & overlap)) > groups[overlap_groups[0]]['mines']:
                valid_overlaps = False
                break
            if len(mines_groups[overlap_groups[1]] | (mines_groups[overlap_groups[0]] & overlap)) > groups[overlap_groups[1]]['mines']:
                valid_overlaps = False
                break
        if not valid_overlaps:
            continue

        # If a mine is a revealed tile, continue
        valid_mines = True
        for mine_group in mines_groups:
            if not valid_mines:
                break
            for mine in mine_group:
                if tiles[mine[0]][mine[1]] >= 0:
                    valid_mines = False
                    break
                    
        if not valid_mines:
            continue

        valid_tiles = set.union(*mines_groups)
        total_valid_configurations += 1
        for tile in valid_tiles:
            probabilities[tile[0]][tile[1]] += 1

    # Normalize probabilities
    for i in range(SIZE):
        for j in range(SIZE):
            probabilities[i][j] = round(probabilities[i][j] / total_valid_configurations, 2)

    return probabilities

# A B C
# D 2 E
# F 1 G
# H I J

def calculate_probabilities():
    tiles = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J']

    group_2 = {'A', 'B', 'C', 'D', 'E', 'F', 'G'}  # Affected by the '2'
    group_1 = {'D', 'E', 'F', 'G', 'H', 'I', 'J'}  # Affected by the '1'
    overlap = group_2 & group_1  # Overlapping tiles

    # Constraints
    mines_in_2 = 2  # Total mines in group_2
    mines_in_1 = 1  # Total mines in group_1

    # Initialize probabilities for each tile
    probabilities = {tile: 0 for tile in tiles}

    # Enumerate possible mine placements in group_2
    total_valid_configurations = 0
    for mines_group_2 in combinations(group_2, mines_in_2):
        for mines_group_1 in combinations(group_1, mines_in_1):
            # Convert to sets for intersection checks
            mines_group_2 = set(mines_group_2)
            mines_group_1 = set(mines_group_1)
            # print(f'Mines Group 2: {mines_group_2}, Mines Group 1: {mines_group_1}')

            # Validate overlap consistency
            if len(mines_group_1 | (mines_group_2 & overlap)) > mines_in_1:
                continue
            if len(mines_group_2 | (mines_group_1 & overlap)) > mines_in_2:
                continue

            # Valid configuration, update probabilities
            valid_tiles = mines_group_2 | mines_group_1
            total_valid_configurations += 1
            for tile in valid_tiles:
                probabilities[tile] += 1

    # Normalize probabilities
    for tile in probabilities:
        probabilities[tile] = (probabilities[tile] / total_valid_configurations)

    return probabilities


def is_connected(i, j, n):
    if i == j:
        return False
    xi, yi = i % n, i // n
    xj, yj = j % n, j // n

    return abs(xi - xj) <= 1 and abs(yi - yj) <= 1


def gauss_jordan(matrix):
    rows = len(matrix)
    cols = len(matrix[0])
    
    r = 0  # Initialize the pivot row
    
    for j in range(cols):
        if r >= rows:
            break
        # Find the row with the largest pivot in column j
        k = max(range(r, rows), key=lambda i: abs(matrix[i][j]))
        
        if matrix[k][j] != 0:  # Only proceed if the pivot is non-zero
            # Swap the current row with the row of the largest pivot
            if k != r:
                matrix[k], matrix[r] = matrix[r], matrix[k]
            
            # Normalize the pivot row
            pivot = matrix[r][j]
            matrix[r] = [value / pivot for value in matrix[r]]
            
            # Eliminate the current column in other rows
            for i in range(rows):
                if i != r:  # Skip the pivot row
                    factor = matrix[i][j]
                    matrix[i] = [matrix[i][x] - factor * matrix[r][x] for x in range(cols)]
            
            # Move to the next pivot row
            r += 1
    
    return matrix


def find_groups_bonds(system, groups, var_names):
    for equation in system:
        for index, group in enumerate(equation[:-1]):
            if group == 1:
                group_value = var_names[index]
                if equation[-1] < groups[group_value]['lower_bound']:
                    groups[group_value]['upper_bound'] = equation[-1]

def dot_product(v1, v2):
    return sum(x * y for x, y in zip(v1, v2))

def nCr(n, r):
    return math.factorial(n) // (math.factorial(r) * math.factorial(n - r))

def matrix_mult(m1, m2):
    result = [0 for _ in range(len(m1))]
    for i in range(len(m1)):
        for j in range(len(m2)):
            result[i] += m1[i][j] * m2[j]
    return result


def calculate_probabilities_matrices():
    n, m = 3, 4
    tiles = [[-1 for _ in range(n)] for _ in range(m)]
    # tiles[1][1] = 0
    # tiles[1][2] = 2
    # tiles[2][1] = 0
    # tiles[2][2] = 3
    # tiles[3][1] = 0
    # tiles[3][2] = 2

    # tiles[1][1] = 3
    # tiles[1][2] = 1
    # tiles[1][3] = 2
    # tiles[1][4] = 3
    # tiles[2][1] = 2
    # tiles[2][2] = 0
    # tiles[2][3] = 0
    # tiles[2][4] = 3
    # tiles[3][1] = 4
    # tiles[3][2] = 2
    # tiles[3][3] = 1
    # tiles[3][4] = 4

    tiles[1][1] = 2
    tiles[2][1] = 1

    # tiles[2][2] = 2
    # tiles[2][3] = 2
    # tiles[2][4] = 1
    # tiles[3][2] = 1
    # tiles[3][3] = 0
    # tiles[3][4] = 1
    # tiles[4][2] = 3
    # tiles[4][3] = 2
    # tiles[4][4] = 2

    show_board(tiles, n)

    A = [[1 if is_connected(i, j, n) else 0 for i in range(n*m)] for j in range(n*m)]
    v = [tiles[j//n][j%n] for j in range(n*m)]

    debug("A:")
    if DEBUG: show_board(A, n*m)
    debug(f"v: {v}")

    A_prime = []
    v_prime = []

    numbered_tiles = [j*n+i for j in range(m) for i in range(n) if tiles[j][i] != -1]
    debug(f"Numbered tiles: {numbered_tiles}")

    for index in numbered_tiles:
            A_prime.append(A[index])
            v_prime.append(v[index])

    debug("A':")
    if DEBUG: show_board(A_prime, n*m)
    debug(f"v': {v_prime}")

    A_prime_T = list(zip(*A_prime))
    A_prime_T_filtered = [col for index, col in enumerate(A_prime_T) if index not in numbered_tiles]
    A_prime_T_filtered = [col for col in A_prime_T_filtered if any(col)]
    A_prime_filtered = [list(row) for row in zip(*A_prime_T_filtered)]

    if DEBUG: show_board(A_prime_filtered, len(A_prime_filtered[0]))

    groups = {}
    for i in range(len(A_prime[0])):
        column = tuple([row[i] for row in A_prime])
        if i in numbered_tiles:
            continue
        if all(cell == 0 for cell in column):
            continue
        if column not in groups:
            groups[column] = {'cell_count': 0, 'lower_bound': 0, 'upper_bound': 0, 'probability': 0, 'cells': []}
        groups[column]['cell_count'] += 1
        groups[column]['upper_bound'] += 1
        groups[column]['cells'].append(i)

    debug(f"Groups: {groups}")
    debug(f'Groups length: {len(groups)}')

    A_prime = A_prime_filtered

    A_second = []
    for i in range(len(A_prime)):
        A_second.append([])
        for group in groups:
            A_second[i].append(group[i])

    debug("A'':")
    if DEBUG: show_board(A_second, len(A_second[0]))

    system = []
    for i in range(len(A_second)):
        system.append(A_second[i] + [v_prime[i]])
    debug(f"System: {system}")

    var_names = [tuple([row[index] for row in A_second]) for index in range(len(A_second[0]))]
    debug(f"Var names: {var_names}")

    find_groups_bonds(system, groups, var_names)
    debug(f"Groups: {groups}")
    debug(f'Groups length: {len(groups)}')

    reduced = gauss_jordan(system)
    debug(f"Reduced: {reduced}")

    solutions = []

    pivot_cols = []
    for i in range(len(reduced)):
        for j in range(len(reduced[0]) - 1):
            if reduced[i][j] == 1 and all(reduced[k][j] == 0 for k in range(len(reduced)) if k != i):
                pivot_cols.append(j)
                break

    debug(pivot_cols)

    free_vars = [i for i in range(len(reduced[0]) - 1) if i not in pivot_cols]

    debug(free_vars)

    free_var_ranges = []
    for var in free_vars:
        group_value = var_names[var]
        free_var_ranges.append(range(groups[group_value]['lower_bound'], groups[group_value]['upper_bound'] + 1))

    debug(free_var_ranges)

    for values in product(*free_var_ranges):
        solution = [0 for _ in range(len(reduced[0]) - 1)]
        for free_var, value in zip(free_vars, values):
            solution[free_var] = value

        # Back-substitute to calculate pivot variable values
        for i, pivot_col in enumerate(pivot_cols):
            solution[pivot_col] = reduced[i][-1] - dot_product(reduced[i][:-1], solution)

        isValid = True
        for var in range(len(solution)):
            group_value = var_names[var]
            if not groups[group_value]['lower_bound'] <= solution[var] <= groups[group_value]['upper_bound']:
                isValid = False
                break

        if isValid:
            solutions.append(solution)


    for solution in solutions:
        debug(f'Solution: {solution}')
        cells = []
        for index, value in enumerate(solution):
            if value != 0:
                cells.append(groups[var_names[index]]['cells'])
        debug(cells)
    debug(f'Solutions length: {len(solutions)}')

    solutions_probabilities = []
    for solution in solutions:
        probability = 1
        for i, value in enumerate(solution):
            # print(solution)
            # print(groups[var_names[i]])
            # print(groups[var_names[i]]['cell_count'], int(value)) 
            probability *= nCr(groups[var_names[i]]['cell_count'], int(value))
        solutions_probabilities.append(probability)

    solutions_probabilities = [probability / sum(solutions_probabilities) for probability in solutions_probabilities]
    
    debug(solutions_probabilities, '\n\n')

    for group in groups:
        group_probability = 0
        for i, solution in enumerate(solutions):
            for j, value in enumerate(solution):
                if var_names[j] == group:
                    debug(f"Group: {group}")
                    debug(f"Solution Probability: {solutions_probabilities[i]}")
                    debug(f"Value: {value}")
                    debug(f"Group Cell Count: {groups[group]['cell_count']}")
                    group_probability += solutions_probabilities[i] * value / groups[group]['cell_count']
        groups[group]['probability'] = group_probability
    
    print(f"Groups: {groups}")

    probability_map = [[0 for _ in range(n)] for _ in range(m)]
    for group in groups:
        for cell in groups[group]['cells']:
            probability_map[cell//n][cell%n] = round(groups[group]['probability'] * 100)

    for i in range(n):
        for j in range(m):
            if tiles[j][i] != -1:
                probability_map[j][i] = tiles[j][i]


    show_board(probability_map, n)

if __name__ == "__main__":
    # probabilities = calculate_probabilities_general()
    # print("\nProbabilities of a mine being present:\n")
    # show_board(probabilities)

    # probabilities = calculate_probabilities()
    # print("Probabilities of a mine being present:")
    # for tile, prob in probabilities.items():
    #     print(f"{tile}: {prob:.2f}")

    calculate_probabilities_matrices()
    