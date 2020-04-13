# `eau2` Report

## Introduction
eau2 is a distributed system used to run machine learning algorithms and other operations over large amounts of data. The system works by creating data frames (tabular based data structures) and storing their contents in a key-value store over multiple nodes. All of the computation and data management is done in the background, which allows the user to work with the data as if it was one large unified data frame.

## Architecture
The main parts of the system include:
* Stores - store data across multiple devices using keys and values (data frames for KDStore)
* Dataframe - holds data in tabular format and works as an easy interface
* Sorer - allows the system to read in ".sor" files
* Network - enables true distribution of data across multiple systems to improve performance and overall data capacity.

## Implementation
Classes:
* DataFrame - structure to hold data in a tabular format and works as an interface that the user can work with
* Schema - defines the structure of a data frame
* Column - structure that holds a list of the same data type (int, double, bool, or String) and stores its data in a distributed manner on a KVStore
* KVStore - data structure containing keys and associated values that runs on multiple nodes and acts as one unified store
* KDStore - wrapper around a KVStore to easily put and get DataFrame objects from the store
* Key - represents a key in a store
* Value - holds the data at the key in a KVStore
* SorParser - reads in the ".sor" file and converts it into a DataFrame
* NetworkIfc - defines the API for putting and getting data on remote nodes
* Connection - manages the interactions between 2 nodes over a network connection

## Use cases
Store and retrieve data from eau2.
See demos in the "tests" directory for more example applications.
```cpp
KDStore kd;
Key doubles("doubles");
size_t SZ = 10;
double* double_vals = new double[SZ];
for (size_t i = 0; i < SZ; ++i) {
    double_vals[i] = i;
}
DataFrame* df1 = DataFrame::fromArray(&doubles, &kd, SZ, double_vals);
DataFrame* df1_copy = kd.get(doubles);
```

Simple word count program using eau2 on mulitple nodes.
```cpp
/** Compute word counts on the local node and build a data frame. */
void local_count() {
    DataFrame* words = (kv.waitAndGet(in));
    p("Node ").p(index).pln(": starting local count...");
    SIMap map;
    Adder add(map);
    words->local_map(add);
    delete words;
    Summer cnt(map);
    delete DataFrame::fromVisitor(mk_key(index), &kv, "SI", cnt);
}

void merge(DataFrame* df, SIMap& m) {
    Adder add(m);
    df->map(add);
    delete df;
}

/** Merge the data frames of all nodes */
void reduce() {
    if (index != 0) return;
    pln("Node 0: reducing counts...");
    SIMap map;
    Key* own = mk_key(0);
    merge(kv.get(*own), map);
    for (size_t i = 1; i < arg.num_nodes; ++i) { // merge other nodes
        Key* ok = mk_key(i);
        merge(kv.waitAndGet(*ok), map);
        delete ok;
    }
    p("Different words: ").pln(map.size());
    delete own;
}

int main() {
    if (index == 0) {
        FileReader fr;
        delete DataFrame::fromVisitor(&in, &kv, "S", fr);
    }
    local_count();
    reduce();
    return 0;
}
```


## Open questions
* We think our implementation performs respectably, but we are still curious where our biggest bottlenecks are. Are they where we expect them to be or are other portions of our codebase impacting performance?

## Status
Our implementation is basically complete. All we should have left is upping our column segment size for when we rerun it on an actual distributed set of machines. The column segments were kept lower for this submission so that we could properly test running on multiple segments on multiple nodes with sample data. Lastly, we only included a subset of the `Linus` app data, because it would not be feasible to try and have others try to rerun and valid our test runs with the full datasets.

## Running
* `make test` runs all unit tests
* `make valgrind` runs all unit tests in `valgrind`
* `make test-all` runs all tests in the "tests" directory (including demo applications)
* `make valgrind-all` runs all tests in the "tests" directory (including demo applications) in `valgrind`
* `make m1` runs the `Demo` app provided in the M1 assignment.
* `make m4` runs the `WordCount` app on a file with 100,000 words.
* `make m5` runs the `Linus` app on a subset of Github data (users, projects, and commits)
