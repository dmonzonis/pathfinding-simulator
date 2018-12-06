import argparse
import csv
from random import choice


def generate_grid(dimension):
    pool = list(range(1, 11, 2)) + [-1]
    with open('randomgrid.csv', 'w') as csvfile:
        writer = csv.writer(csvfile, delimiter=',')
        # Write dimensions
        writer.writerow([dimension, dimension])
        # Don't need start and goal positions
        writer.writerow([0] * 4)
        # Generate grid with random weights from the pool
        for y in range(dimension):
            print(f"Writing row {y+1} of {dimension}", sep=' ', end='\r', flush=True)
            writer.writerow([choice(pool) for _ in range(dimension)])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dimension', type=int,
                        help="dimensions of the grid", required=True)
    args = parser.parse_args()

    dimension = args.dimension
    generate_grid(dimension)


if __name__ == "__main__":
    main()
