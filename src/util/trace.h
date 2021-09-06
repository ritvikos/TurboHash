#ifndef KV_UTIL_TRACE_H
#define KV_UTIL_TRACE_H

#include <stdint.h>

#include <thread>
#include <vector>

#include "geninfo.h"
#include "slice.h"
namespace util {
const uint64_t kRAND64_MAX = ((((uint64_t)RAND_MAX) << 31) + ((uint64_t)RAND_MAX));
const double kRAND64_MAX_D = ((double)(kRAND64_MAX));
const uint64_t kRANDOM_RANGE = UINT64_C (2000000000000);

const uint64_t kYCSB_SEED = 1729;
const uint64_t kYCSB_LATEST_SEED = 1089;

enum YCSBLoadType { kYCSB_A, kYCSB_B, kYCSB_C, kYCSB_D, kYCSB_E, kYCSB_F };
enum YCSBOpType { kYCSB_Write, kYCSB_Read, kYCSB_Query, kYCSB_ReadModifyWrite };

struct YCSB_Op {
    YCSBOpType type;
    uint64_t key;
};

class Trace {
public:
    Trace (int seed) : seed_ (seed), init_ (seed), gi_ (nullptr) {
        if (seed_ == 0) {
            seed_ = random ();
        }
    }

    virtual ~Trace () {
        if (gi_ != nullptr) delete gi_;
    }
    virtual uint64_t Next () = 0;
    void Reset () { seed_ = init_; }
    uint32_t Random () {
        static thread_local const uint32_t M = 2147483647L;  // 2^31-1
        static thread_local const uint64_t A = 16807;        // bits 14, 8, 7, 5, 2, 1, 0
        // We are computing
        //       seed_ = (seed_ * A) % M,    where M = 2^31-1
        //
        // seed_ must not be zero or M, or else all subsequent computed values
        // will be zero or M respectively.  For all other values, seed_ will end
        // up cycling through every number in [1,M-1]
        uint64_t product = seed_ * A;

        // Compute (product % M) using the fact that ((x << 31) % M) == x.
        seed_ = static_cast<uint32_t> ((product >> 31) + (product & M));
        // The first reduction may overflow by 1 bit, so we may need to
        // repeat.  mod == M is not possible; using > allows the faster
        // sign-bit-based test.
        if ((uint32_t)seed_ > M) {
            seed_ -= M;
        }
        return seed_;
    }

    inline uint64_t Random64 () {
        // 62 bit random value;
        const uint64_t rand64 = (((uint64_t)Random ()) << 31) + ((uint64_t)Random ());
        return rand64;
    }

    inline double RandomDouble () {
        // random between 0.0 - 1.0
        const double r = (double)Random64 ();
        const double rd = r / kRAND64_MAX_D;
        return rd;
    }

    int seed_;
    int init_;
    GenInfo* gi_;
};

class TraceSeq : public Trace {
public:
    explicit TraceSeq (uint64_t start_off = 0, uint64_t interval = 1, uint64_t minimum = 0,
                       uint64_t maximum = kRANDOM_RANGE)
        : Trace (0) {
        start_off_ = start_off;
        interval_ = interval;
        min_ = minimum;
        max_ = maximum;
        cur_ = start_off_;
    }
    inline uint64_t Next () override {
        cur_ += interval_;
        if (cur_ >= max_) cur_ = 0;
        return cur_;
    }

private:
    uint64_t start_off_;
    uint64_t interval_;
    uint64_t min_;
    uint64_t max_;
    uint64_t cur_;
};

class TraceUniform : public Trace {
public:
    explicit TraceUniform (int seed, uint64_t minimum = 0, uint64_t maximum = kRANDOM_RANGE)
        : Trace (seed) {
        gi_ = new GenInfo ();
        gi_->gen.uniform.min = minimum;
        gi_->gen.uniform.max = maximum;
        gi_->gen.uniform.interval = (double)(maximum - minimum);
        gi_->type = GEN_UNIFORM;
    }

    ~TraceUniform () {}
    uint64_t Next () override {
        uint64_t off = (uint64_t) (RandomDouble () * gi_->gen.uniform.interval);
        return gi_->gen.uniform.min + off;
    }
};

}  // namespace util
   // hopman_fast

#endif