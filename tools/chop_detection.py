import sys
import numpy as np
from sklearn import svm
from sklearn.model_selection import cross_val_score
from sklearn_porter import Porter

def load_data(filename):
    training_data = np.genfromtxt(filename, delimiter=',', dtype=np.float32)
    training_data = training_data[1:, :]
    total = training_data[:, 0] + training_data[:, 1] + training_data[:, 2] + training_data[:, 3]
    times = np.arange(0, total.shape[0])*100
    training_data = np.column_stack((training_data, total))
    print(training_data)
    return training_data

def train_svm(data):
    x = data[:4, :].T
    y = data[5, :].astype(int)
    #y = y.reshape((y.shape[0],1))
    print(x.shape)
    print(y.shape)
    clf = svm.SVC(gamma=0.2)
    clf.fit(x, y)
    scores = cross_val_score(clf, x, y, cv=5)
    print(scores)
    return clf


def get_segment_features(segment_data):
    max_val = np.amax(segment_data[:,6])
    duration = segment_data[-1,4] - segment_data[0,4]
    impulse = np.trapz(segment_data[:,6])
    peak_pos = segment_data[:,6].argmax() / segment_data.shape[0]
    kurtosis = impulse/(duration*max_val)
    containsChop = np.amax(segment_data[:,5])
    return np.asarray([max_val, duration, impulse, peak_pos, kurtosis, containsChop])

def create_segments(data, segment_length):
    #Max, duration, impulse, peak, kurtosis, containsChop
    segments = np.asarray([0, 0, 0, 0, 0, 0])
    for start_idx, row in enumerate(data):
        segment_start_time = row[4]
        segment_end_time = segment_start_time + segment_length
        end_idx = (np.abs(data[:, 4] - segment_end_time)).argmin()
        segment_data = data[start_idx:end_idx, :]
        if segment_data.shape[0] <= 1:
            break
        features = get_segment_features(segment_data)
        segments = np.vstack((segments, features))
    print(segments.T.shape)
    return segments.T

def parse_log(filename):
    file = open(filename, "r")
    output = open("formatted_data.txt", "w")
    record = []
    chop = 0
    time = 0
    for row in file:
        print(row)
        if "Time: " in row:
            time = row[6:].rstrip()
        if "Sensor 0" in row or "Sensor 1" in row or "Sensor 2" in row or "Sensor 3" in row:
            record.append(row[10:].rstrip())
        if "Sensor 3" in row:
            output.write(record[0] + "," + record[1] + "," + record[2] + "," + record[3] + "," + time + "," + str(chop) + "\n")
            record = []
            chop = 0
        if "CHOP: " in row:
            chop = 1
    file.close()



if __name__ == "__main__":
    filename = sys.argv[1]
    #parse_log(filename)
    data = load_data(filename)
    features = create_segments(data, 1000)
    clf = train_svm(features)
    porter = Porter(clf, language='c')
    output = porter.export()
    outFile = open('model', "w")
    outFile.write(output)
    outFile.close()