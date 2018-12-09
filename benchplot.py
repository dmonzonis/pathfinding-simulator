import argparse
import matplotlib.pyplot as plt
import numpy as np


def get_data(filename):
    return np.genfromtxt(filename,
                         delimiter=',',
                         skip_header=1,
                         dtype="i8,i8,f8,i8,i8,f8,i8,i8,f8,i8,i8,f8")


def plot(data):
    plt.figure()
    plt.title("Benchmark")
    plt.xlabel('Optimal distance to goal')
    plt.ylabel('Nodes expanded')

    # Dijkstra
    plt.scatter(data['f0'], data['f1'],
                label="Dijkstra", marker='x')

    # A*
    plt.scatter(data['f0'], data['f4'],
                label="A*", marker='x')

    plt.legend()
    plt.show()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-f', type=str, help="Filename with the benchmark data", required=True)
    args = parser.parse_args()

    data = get_data(args.f)
    plot(data)


if __name__ == "__main__":
    main()
