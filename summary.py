import argparse
import numpy as np
from benchplot import get_data


def compute_total(data, column):
    nodes_columns = ['f' + str(n) for n in range(1, 12, 3)]
    result = np.sum(data[column])
    if column in nodes_columns:
        print(f"Total nodes: {result}")
    else:
        print(f"Total time: {result}")


def compute_success(data, column):
    # Compare distance to Dijkstra's distance which is in field 0
    success_array = data[column] == data['f0']
    success = np.sum(success_array)
    print(f"Times succeeded: {success}")


def run_summary(data):
    # Dijkstra
    print("Dijkstra\n----------")
    compute_total(data, 'f1')
    compute_total(data, 'f2')
    compute_success(data, 'f0')
    print()

    # A*
    print("A*\n----------")
    compute_total(data, 'f4')
    compute_total(data, 'f5')
    compute_success(data, 'f3')
    print()

    # A*(alt)
    print("A*(alt)\n----------")
    compute_total(data, 'f7')
    compute_total(data, 'f8')
    compute_success(data, 'f6')
    print()

    # Greedy
    print("Greedy\n----------")
    compute_total(data, 'f10')
    compute_total(data, 'f11')
    compute_success(data, 'f9')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-f', type=str, help="Filename with the benchmark data", required=True)
    args = parser.parse_args()

    data = get_data(args.f)
    run_summary(data)


if __name__ == "__main__":
    main()
