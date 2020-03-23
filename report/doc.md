# `eau2` Report

## Introduction
eau2 is a distributed system used to run machine learning algorithms and other operations over large amounts of data. The system works by creating data frames (tabular based data structures) and storing their contents in a key-value store over multiple nodes. All of the computation and data management is done in the background, which allows the user to work with the data as if it was one large unified data frame.

## Architecture
The main parts of the system include:
* KVStore and KDStore - stores data across multiple devices using keys and values (data frames for KDStore)
* Dataframes and Columns - holds data in tabular format and works as an easy interface for the user
* Sorer - allows the system to read in ".sor" files 

## Implementation


## Use cases
Store and retrieve data from eau2.
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
* How are the different tiers supposed to interact? The description implies that the tiers will be built on top of each other, but the given code seems to contradict this.

## Status
The KDStore and KVStore have been created and all of the classes that support it to work locally. Next steps include:
- Making the KVStore distributed (having columns distribute their data over multiple KVStores)
- Allow the KVStores to work over a network