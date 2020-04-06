#include <assert.h>

#include "application/application.h"
#include "catch.hpp"
#include "dataframe/row.h"
#include "dataframe/visitor.h"
#include "util/string.h"

class FileReader : public Writer {
   public:
    char* buf_;
    size_t end_ = 0;
    size_t i_ = 0;
    FILE* file_;

    /** Creates the reader and opens the file for reading.  */
    FileReader(const char* file_name) : Writer() {
        file_ = fopen(file_name, "r");
        assert(file_ != nullptr);
        buf_ = new char[BUFSIZE + 1];  //  null terminator
        fillBuffer_();
        skipWhitespace_();
    }

    ~FileReader() {
        delete[] buf_;
        fclose(file_);
    }

    /** Reads next word and stores it in the row. Actually read the word.
     While reading the word, we may have to re-fill the buffer  */
    void visit(Row& r) override {
        assert(i_ < end_);
        assert(!isspace(buf_[i_]));
        size_t wStart = i_;
        while (true) {
            if (i_ == end_) {
                if (feof(file_)) {
                    ++i_;
                    break;
                }
                i_ = wStart;
                wStart = 0;
                fillBuffer_();
            }
            if (isspace(buf_[i_])) break;
            ++i_;
        }
        buf_[i_] = 0;
        String* word = new String(buf_ + wStart, i_ - wStart);
        r.set(0, word);
        ++i_;
        skipWhitespace_();
    }

    /** Returns true when there are no more words to read.  There is nothing
       more to read if we are at the end of the buffer and the file has
       all been read.     */
    bool done() override {
        return (i_ >= end_) && feof(file_);
    }

    static const size_t BUFSIZE = 1024;

    /** Reads more data from the file. */
    void fillBuffer_() {
        size_t start = 0;
        // compact unprocessed stream
        if (i_ != end_) {
            start = end_ - i_;
            memcpy(buf_, buf_ + i_, start);
        }
        // read more contents
        end_ = start + fread(buf_ + start, sizeof(char), BUFSIZE - start, file_);
        i_ = start;
    }

    /** Skips spaces.  Note that this may need to fill the buffer if the
        last character of the buffer is space itself.  */
    void skipWhitespace_() {
        while (true) {
            if (i_ == end_) {
                if (feof(file_)) return;
                fillBuffer_();
            }
            // if the current character is not whitespace, we are done
            if (!isspace(buf_[i_])) return;
            // otherwise skip it
            ++i_;
        }
    }
};

class Adder : public Reader {
   public:
    std::unordered_map<std::string, int> map_;

    Adder(std::unordered_map<std::string, int>& map) : Reader() {
        map_ = map;
    }

    void visit(Row& r) override {
        std::string w = std::string(r.get_string(0)->c_str());
        if (map_.find(w) == map_.end()) {
            map_[w] = 1;
        } else {
            map_[w] += 1;
        }
    }
};

class Merger : public Reader {
   public:
    std::unordered_map<std::string, int> map_;

    Merger(std::unordered_map<std::string, int>& map) : Reader() {
        map_ = map;
    }

    void visit(Row& r) override {
        std::string w = std::string(r.get_string(0)->c_str());
        if (map_.find(w) == map_.end()) {
            map_[w] = r.get_int(1);
        } else {
            map_[w] += r.get_int(1);
        }
    }
};

class Summer : public Writer {
   public:
    std::unordered_map<std::string, int>::iterator it_;
    std::unordered_map<std::string, int> map_;

    Summer(std::unordered_map<std::string, int>& map) : Writer() {
        map_ = map;
        it_ = map_.begin();
    }

    virtual ~Summer() {}

    /**
     * Visits the given row.
     * @arg r  the row
     */
    virtual void visit(Row& r) {
        String* key = new String(it_->first.c_str());
        r.set(0, key);
        r.set(1, it_->second);
        it_++;
    }

    /**
     * Marks when the writer is done visiting the data frame.
     * @return true if done
     */
    virtual bool done() {
        return it_ == map_.end();
    }
};

/****************************************************************************
 * Calculate a word count for given file:
 *   1) read the data (single node)
 *   2) produce word counts per homed chunks, in parallel
 *   3) combine the results
 **********************************************************author: pmaj ****/
class WordCount : public Application {
   public:
    static const size_t BUFSIZE = 1024;
    Key in;
    std::unordered_map<std::string, int> result;
    FileReader fr_;

    WordCount(NetworkIfc& net, const char* file_name)
        : Application(net), in("data"), fr_(file_name) {}

    /** The master nodes reads the input, then all of the nodes count. */
    void run() override {
        if (node_num_ == 0) {
            delete DataFrame::fromVisitor(&in, &kd_, "S", fr_);
        }
        local_count();
        if (node_num_ == 0) {
            reduce();
        }
    }

    /** Returns a key for given node.  These keys are homed on master node
     *  which then joins them one by one. */
    Key* mk_key(size_t idx) {
        StrBuff sb = StrBuff();
        sb.c(idx);
        String* s = sb.get();
        Key* k = new Key(s->c_str());
        delete s;
        return k;
    }

    /** Compute word counts on the local node and build a data frame. */
    void local_count() {
        DataFrame* words = kd_.waitAndGet(in);
        p("Node ").p(node_num_).pln(": starting local count...");
        std::unordered_map<std::string, int> map = std::unordered_map<std::string, int>();
        Adder add(map);
        words->local_map(add);
        delete words;
        Summer cnt(add.map_);
        Key* k = mk_key(node_num_);
        delete DataFrame::fromVisitor(k, &kd_, "SI", cnt);
        delete k;
    }

    /** Merge the data frames of all nodes */
    void reduce() {
        pln("Node 0: reducing counts...");
        std::unordered_map<std::string, int> map = std::unordered_map<std::string, int>();
        Key* own = mk_key(0);
        map = merge(kd_.get(*own), map);
        for (size_t i = 1; i < kv_.num_nodes(); ++i) {  // merge other nodes
            Key* ok = mk_key(i);
            map = merge(kd_.waitAndGet(*ok), map);
            delete ok;
        }
        delete own;
        p("Different words: ").pln(map.size());
        result = map;
    }

    std::unordered_map<std::string, int> merge(DataFrame* df,
                                               std::unordered_map<std::string, int>& m) {
        Merger merge(m);
        df->map(merge);
        delete df;
        return merge.map_;
    }
};

// check output of word count
TEST_CASE("run word count app with our sample", "[m4][application]") {
    Address a0("127.0.0.1", 10000);
    Address a1("127.0.0.1", 10001);
    const char* file_path = "./data/sample.txt";
    printf("STARTING M4 ON %s\n", file_path);

    NetworkIfc net0(&a0, 2);
    NetworkIfc net1(&a1, &a0, 1, 2);

    WordCount w0(net0, file_path);
    WordCount w1(net1, file_path);

    w0.start();
    w1.start();

    w0.join();
    w1.join();

    std::unordered_map<std::string, int> expected = std::unordered_map<std::string, int>();
    expected[std::string("over")] = 1 * 513;
    expected[std::string("dog")] = 2 * 513;
    expected[std::string("jumps")] = 1 * 513;
    expected[std::string("do")] = 1 * 513;
    expected[std::string("not")] = 1 * 513;
    expected[std::string("fox")] = 1 * 513;
    expected[std::string("brown")] = 1 * 513;
    expected[std::string("peanut")] = 1 * 513;
    expected[std::string("butter")] = 1 * 513;
    expected[std::string("give")] = 1 * 513;
    expected[std::string("quick")] = 1 * 513;
    expected[std::string("lazy")] = 1 * 513;
    expected[std::string("the")] = 3 * 513;

    std::unordered_map<std::string, int>::iterator it = w0.result.begin();
    while (it != w0.result.end()) {
        REQUIRE(expected[it->first] == it->second);
        it++;
    }
    printf("FINISHING M4 ON %s\n", file_path);
}

// check output of word count
TEST_CASE("run word count app with their sample", "[m4][application]") {
    Address a0("127.0.0.1", 10000);
    Address a1("127.0.0.1", 10001);
    Address a2("127.0.0.1", 10002);
    const char* file_path = "./data/100k.txt";

    printf("STARTING M4 ON %s\n", file_path);

    NetworkIfc net0(&a0, 3);
    NetworkIfc net1(&a1, &a0, 1, 3);
    NetworkIfc net2(&a2, &a0, 2, 3);

    WordCount w0(net0, file_path);
    WordCount w1(net1, file_path);
    WordCount w2(net2, file_path);

    w0.start();
    w2.start();
    w1.start();

    w2.join();
    w1.join();
    w0.join();

    REQUIRE(w0.result[std::string("lorem")] == 50);
    printf("FINISHING M4 ON %s\n", file_path);
}
