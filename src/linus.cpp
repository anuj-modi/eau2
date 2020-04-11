#include "application/application.h"
#include "dataframe/dataframe.h"
#include "util/string.h"
/**
 * The input data is a processed extract from GitHub.
 *
 * projects:  I x S   --  The first field is a project id (or pid).
 *                    --  The second field is that project's name.
 *                    --  In a well-formed dataset the largest pid
 *                    --  is equal to the number of projects.
 *
 * users:    I x S    -- The first field is a user id, (or uid).
 *                    -- The second field is that user's name.
 *
 * commits: I x I x I -- The fields are pid, uid, uid', each row represent
 *                    -- a commit to project pid, written by user uid
 *                    -- and committed by user uid',
 **/

/**************************************************************************
 * A bit set contains size() booleans that are initialize to false and can
 * be set to true with the set() method. The test() method returns the
 * value. Does not grow.
 ************************************************************************/
class Set {
   public:
    bool* vals_;   // owned; data
    size_t size_;  // number of elements

    /** Creates a set of the same size as the dataframe. */
    Set(DataFrame* df) : Set(df->nrows()) {}

    /** Creates a set of the given size. */
    Set(size_t sz) : vals_(new bool[sz]), size_(sz) {
        for (size_t i = 0; i < size_; i++) vals_[i] = false;
    }

    ~Set() {
        delete[] vals_;
    }

    /** Add idx to the set. If idx is out of bound, ignore it.  Out of bound
     *  values can occur if there are references to pids or uids in commits
     *  that did not appear in projects or users.
     */
    void set(size_t idx) {
        if (idx >= size_) return;  // ignoring out of bound writes
        vals_[idx] = true;
    }

    /** Is idx in the set?  See comment for set(). */
    bool test(size_t idx) {
        if (idx >= size_) return true;  // ignoring out of bound reads
        return vals_[idx];
    }

    size_t size() {
        return size_;
    }

    size_t num_tagged() {
        size_t sum = 0;
        for (size_t i = 0; i < size_; i++) {
            if (test(i)) {
                sum += 1;
            }
        }
        return sum;
    }

    /** Performs set union in place. */
    void union_(Set& from) {
        for (size_t i = 0; i < from.size_; i++)
            if (from.test(i)) set(i);
    }
};

/*******************************************************************************
 * A SetUpdater is a reader that gets the first column of the data frame and
 * sets the corresponding value in the given set.
 ******************************************************************************/
class SetUpdater : public Reader {
   public:
    Set& set_;  // set to update

    SetUpdater(Set& set) : set_(set) {}

    /** Assume a row with at least one column of type I. Assumes that there
     * are no missing. Reads the value and sets the corresponding position.
     * The return value is irrelevant here. */
    void visit(Row& row) {
        set_.set(row.get_int(0));
    }
};

/*****************************************************************************
 * A SetWriter copies all the values present in the set into a one-column
 * dataframe. The data contains all the values in the set. The dataframe has
 * at least one integer column.
 ****************************************************************************/
class SetWriter : public Writer {
   public:
    Set& set_;      // set to read from
    size_t i_ = 0;  // position in set

    SetWriter(Set& set) : set_(set) {}

    /** Skip over false values and stop when the entire set has been seen */
    bool done() {
        while (i_ < set_.size_ && set_.test(i_) == false) ++i_;
        return i_ == set_.size_;
    }

    void visit(Row& row) {
        int val = i_;
        row.set(0, val);
        i_++;
    }
};

/***************************************************************************
 * The ProjectTagger is a reader that is mapped over commits, and marks all
 * of the projects to which a collaborator of Linus committed as an author.
 * The commit dataframe has the form:
 *    pid x uid x uid
 * where the pid is the identifier of a project and the uids are the
 * identifiers of the author and committer. If the author is a collaborator
 * of Linus, then the project is added to the set. If the project was
 * already tagged then it is not added to the set of newProjects.
 *************************************************************************/
class ProjectsTagger : public Reader {
   public:
    Set& uSet;        // set of collaborator
    Set& pSet;        // set of projects of collaborators
    Set newProjects;  // newly tagged collaborator projects

    ProjectsTagger(Set& uSet, Set& pSet, DataFrame* proj)
        : uSet(uSet), pSet(pSet), newProjects(proj) {}

    /** The data frame must have at least two integer columns. The newProject
     * set keeps track of projects that were newly tagged (they will have to
     * be communicated to other nodes). */
    void visit(Row& row) override {
        int pid = row.get_int(0);
        int uid = row.get_int(1);
        if (uSet.test(uid)) {
            if (!pSet.test(pid)) {
                pSet.set(pid);
                newProjects.set(pid);
            }
        }
    }
};

/***************************************************************************
 * The UserTagger is a reader that is mapped over commits, and marks all of
 * the users which commmitted to a project to which a collaborator of Linus
 * also committed as an author. The commit dataframe has the form:
 *    pid x uid x uid
 * where the pid is the idefntifier of a project and the uids are the
 * identifiers of the author and committer.
 *************************************************************************/
class UsersTagger : public Reader {
   public:
    Set& pSet;
    Set& uSet;
    Set newUsers;

    UsersTagger(Set& pSet, Set& uSet, DataFrame* users)
        : pSet(pSet), uSet(uSet), newUsers(users->nrows()) {}

    void visit(Row& row) override {
        int pid = row.get_int(0);
        int uid = row.get_int(1);
        if (pSet.test(pid)) {
            if (!uSet.test(uid)) {
                uSet.set(uid);
                newUsers.set(uid);
            }
        }
    }
};

/*************************************************************************
 * This computes the collaborators of Linus Torvalds.
 * is the linus example using the adapter.  And slightly revised
 *   algorithm that only ever trades the deltas.
 **************************************************************************/
class Linus : public Application {
   public:
    size_t DEGREES = 4;  // How many degrees of separation form linus?
    int LINUS = 4967;    // The uid of Linus (offset in the user df)
    const char* PROJ = "data/projects.ltgt";
    const char* USER = "data/users.ltgt";
    const char* COMM = "data/commits.ltgt";
    DataFrame* projects;  //  pid x project name
    DataFrame* users;     // uid x user name
    DataFrame* commits;   // pid x uid x uid
    Set* uSet;            // Linus' collaborators
    Set* pSet;            // projects of collaborators

    Linus(NetworkIfc& net) : Application(net) {}

    ~Linus() {
        delete uSet;
        delete pSet;
        delete projects;
        delete users;
        delete commits;
    }

    /** Compute DEGREES of Linus.  */
    void run() override {
        readInput();
        for (size_t i = 0; i < DEGREES; i++) step(i);
    }

    /** Node 0 reads three files, cointainng projects, users and commits, and
     *  creates thre dataframes. All other nodes wait and load the three
     *  dataframes. Once we know the size of users and projects, we create
     *  sets of each (uSet and pSet). We also output a data frame with a the
     *  'tagged' users. At this point the dataframe consists of only
     *  Linus. **/
    void readInput() {
        Key pK("projs");
        Key uK("usrs");
        Key cK("comts");
        if (this_node() == 0) {
            pln("Reading...");
            projects = DataFrame::fromSorFile(&pK, &kd_, PROJ);
            p("    ").p(projects->nrows()).pln(" projects");
            users = DataFrame::fromSorFile(&uK, &kd_, USER);
            p("    ").p(users->nrows()).pln(" users");
            commits = DataFrame::fromSorFile(&cK, &kd_, COMM);
            p("    ").p(commits->nrows()).pln(" commits");
            Key scalar("users-0-0");
            // This dataframe contains the id of Linus.
            delete DataFrame::fromScalar(&scalar, &kd_, LINUS);
        } else {
            projects = kd_.waitAndGet(pK);
            users = kd_.waitAndGet(uK);
            commits = kd_.waitAndGet(cK);
        }
        uSet = new Set(users);
        pSet = new Set(projects);
    }

    /** Performs a step of the linus calculation. It operates over the three
     *  datafrrames (projects, users, commits), the sets of tagged users and
     *  projects, and the users added in the previous round. */
    void step(int stage) {
        p("Stage ").pln(stage);
        // Key of the shape: users-stage-0
        String* tmp = StrBuff().c("users-").c(stage).c("-0").get();
        Key uK(tmp->c_str());
        delete tmp;
        // A df with all the users added on the previous round
        DataFrame* newUsers = (kd_.waitAndGet(uK));
        Set delta(users);
        SetUpdater upd(delta);
        newUsers->map(upd);  // all of the new users are copied to delta.
        delete newUsers;
        ProjectsTagger ptagger(delta, *pSet, projects);
        commits->local_map(ptagger);  // marking all projects touched by delta
        merge(ptagger.newProjects, "projects-", stage);
        pSet->union_(ptagger.newProjects);  //
        UsersTagger utagger(ptagger.newProjects, *uSet, users);
        commits->local_map(utagger);
        merge(utagger.newUsers, "users-", stage + 1);
        uSet->union_(utagger.newUsers);
        p("    after stage ").p(stage).pln(":");
        p("        tagged projects: ").pln(pSet->num_tagged());
        p("        tagged users: ").pln(uSet->num_tagged());
    }

    /** Gather updates to the given set from all the nodes in the systems.
     * The union of those updates is then published as dataframe.  The key
     * used for the otuput is of the form "name-stage-0" where name is either
     * 'users' or 'projects', stage is the degree of separation being
     * computed.
     */
    void merge(Set& set, char const* name, int stage) {
        if (this_node() == 0) {
            for (size_t i = 1; i < num_nodes(); ++i) {
                String* str = StrBuff().c(name).c(stage).c("-").c(i).get();
                Key nK(str->c_str());
                delete str;
                DataFrame* delta = kd_.waitAndGet(nK);
                p("    received delta of ").p(delta->nrows()).p(" elements from node ").pln(i);
                SetUpdater upd(set);
                delta->map(upd);
                delete delta;
            }
            p("    storing ").p(set.size()).pln(" merged elements");
            SetWriter writer(set);
            String* tmp = StrBuff().c(name).c(stage).c("-0").get();
            Key k(tmp->c_str());
            delete tmp;
            delete DataFrame::fromVisitor(&k, &kd_, "I", writer);
        } else {
            p("    sending ").p(set.size()).pln(" elements to master node");
            SetWriter writer(set);
            String* tmp = StrBuff().c(name).c(stage).c("-").c(this_node()).get();
            Key k(tmp->c_str());
            delete tmp;
            delete DataFrame::fromVisitor(&k, &kd_, "I", writer);

            String* tmp2 = StrBuff().c(name).c(stage).c("-0").get();
            Key mK(tmp2->c_str());
            delete tmp2;
            DataFrame* merged = kd_.waitAndGet(mK);
            p("    receiving ").p(merged->nrows()).pln(" merged elements");
            SetUpdater upd(set);
            merged->map(upd);
            delete merged;
        }
    }
};

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

int server(int argc, char** argv) {
    char usage[] = "./build/server <ip> <port> <total_nodes>";
    if (argc != 4) {
        printf("%s\n", usage);
        return 1;
    }

    char* ip = argv[1];
    uint16_t port = atoi(argv[2]);
    size_t num_nodes = atol(argv[3]);

    Address a(ip, port);
    NetworkIfc ifc(&a, num_nodes);

    // Linus app(ifc);
    WordCount app(ifc, "data/100k.txt");

    app.start();

    app.join();

    return 0;
}

int client(int argc, char** argv) {
    char usage[] =
        "./build/server <server_ip> <server_port> <local_ip> <local_port> <node_num> <total_nodes>";
    if (argc != 7) {
        printf("%s\n", usage);
        return 1;
    }

    char* server_ip = argv[1];
    uint16_t server_port = atoi(argv[2]);
    char* ip = argv[3];
    uint16_t port = atoi(argv[4]);
    size_t node_num = atol(argv[5]);
    size_t num_nodes = atol(argv[6]);

    Address server(server_ip, server_port);
    Address me(ip, port);
    NetworkIfc ifc(&me, &server, node_num, num_nodes);

    // Linus app(ifc);
    WordCount app(ifc, "data/100k.txt");

    app.start();

    app.join();

    return 0;
}

int main(int argc, char** argv) {
    if (strcmp("./build/server", argv[0]) == 0) {
        return server(argc, argv);
    } else if (strcmp("./build/client", argv[0]) == 0) {
        return client(argc, argv);
    } else {
        printf("UNKNOWN STRING %s\n", argv[0]);
        return 1;
    }
}
