import sys
import numpy as np
from sklearn import svm

def load_data(filename):
    training_data = np.genfromtxt(filename, delimiter=',', dtype=np.float32)
    training_data = training_data[1:, :]
    total = training_data[:, 0] + training_data[:, 1] + training_data[:, 2] + training_data[:, 3]
    times = np.arange(0, total.shape[0])*100
    training_data = np.column_stack((training_data, total))
    training_data = np.column_stack((training_data, times))
    return training_data

def train_svm(data):
    x = [0]
    y = [0]
    clf = svm.SVC()
    clf.fit(x,y)
    return clf


def get_segment_features(segment_data):
    max_val = np.amax(segment_data[:,4])
    duration = segment_data[-1,5] - segment_data[0,5]
    impulse = np.trapz(segment_data[:,4])
    peak_pos = segment_data[:,4].argmax() / segment_data.shape[0]
    kurtosis = impulse/(duration*max_val)
    return np.asarray([max_val, duration, impulse, peak_pos, kurtosis])

def create_segments(data, segment_length):
    #Max, duration, impulse, peak, kurtosis
    segments = np.asarray([0, 0, 0, 0, 0])
    for start_idx, row in enumerate(data):
        segment_start_time = row[5]
        segment_end_time = segment_start_time + segment_length
        end_idx = (np.abs(data[:, 5] - segment_end_time)).argmin()
        segment_data = data[start_idx:end_idx, :]
        if segment_data.shape[0] <= 1:
            break
        features = get_segment_features(segment_data)
        segments = np.vstack((segments, features))
    return segments

def parse_log(filename):
    file = open(filename, "r")
    output = open("dummy2.txt", "w")
    record = []
    for row in file:
        print(row)
        if "Sensor 0" in row or "Sensor 1" in row or "Sensor 2" in row or "Sensor 3" in row:
            record.append(row[10:].rstrip())
        if "Sensor 3" in row:
            output.write(record[0] + "," + record[1] + "," + record[2] + "," + record[3] + "\n")
            record = []



if __name__ == "__main__":
    filename = sys.argv[1]
    #parse_log(filename)
    data = load_data(filename)
    features = create_segments(data, 1000)
    train_svm(features)