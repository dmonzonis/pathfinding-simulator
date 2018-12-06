import csv
from random import choice

DIMENSIONS = 10000


def generate_grid():
    pool = list(range(1, 11, 2)) + [-1]
    with open('randomgrid.csv', 'w') as csvfile:
        writer = csv.writer(csvfile, delimiter=',')
        # Write dimensions
        writer.writerow([DIMENSIONS, DIMENSIONS])
        # Don't need start and goal positions
        writer.writerow([0] * 4)
        # Generate grid with random weights from the pool
        for y in range(DIMENSIONS):
            print(f"Writing row {y+1} of {DIMENSIONS}", sep=' ', end='\r', flush=True)
            writer.writerow([choice(pool) for _ in range(DIMENSIONS)])


def main():
    generate_grid()


if __name__ == "__main__":
    main()
