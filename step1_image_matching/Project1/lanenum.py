from sklearn.cluster import DBSCAN
import numpy as np
import csv
from sklearn import metrics
from sklearn.datasets.samples_generator import make_blobs
from sklearn.preprocessing import StandardScaler
import matplotlib.pyplot as plt

data = []
with open('output.csv') as mycsv:
    csvread = csv.reader(mycsv)
    for row in csvread:
        t = [i for i in row]
        data.append(t)
data = np.array(data)
for k, col in zip(unique_labels, colors):
    plt.plot(xy[:, 0], xy[:, 1], 'o', markerfacecolor=tuple(col),
             markeredgecolor='w', markersize=4)
plt.title('Estimated number of clusters: %d' % n_clusters_)
plt.show()
