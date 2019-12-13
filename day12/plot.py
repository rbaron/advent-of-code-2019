from collections import defaultdict
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def read_points(file):
    points_by_moon = defaultdict(list)
    for line in file:
        n, x, y, z = map(int, line.split(' '))
        points_by_moon[n].append((x, y, z))
    return points_by_moon

def main():
    with open('points.dat', 'r') as f:
        points = read_points(f)

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    x, y, z = zip(*points[0][40000:])
    ax.plot(x, y, z)
    plt.show()

if __name__ == '__main__':
    main()