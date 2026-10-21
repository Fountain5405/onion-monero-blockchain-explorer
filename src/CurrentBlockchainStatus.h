//
// Created by mwo on 16/05/17.
//

#ifndef XMRBLOCKS_CURRENTBLOCKCHAINSTATUS_H
#define XMRBLOCKS_CURRENTBLOCKCHAINSTATUS_H

#include "MicroCore.h"

#include <boost/algorithm/string.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

namespace xmreg
{

using namespace std;

namespace bf = boost::filesystem;

struct CurrentBlockchainStatus
{

    // The total emission is above 2^64 atomic units (since block 3165917
    // on mainnet), so it does not fit into uint64_t anymore.
    using amount_t = boost::multiprecision::uint128_t;

    struct Emission
    {
        amount_t coinbase;
        amount_t fee;
        uint64_t blk_no;

        inline amount_t
        checksum() const
        {
            return coinbase + fee + blk_no;
        }

        operator
        std::string() const
        {
            return to_string(blk_no) + "," + coinbase.str()
                   + "," + fee.str() + "," + checksum().str();
        }
    };

    static bf::path blockchain_path;

    static cryptonote::network_type nettype;

    static string output_file;

    static string daemon_url;

    // how many blocks to read before thread goes to sleep
    static uint64_t blockchain_chunk_size;

    // gap from what we store total_emission and
    // current blockchain height. We dont want to store
    // what is on, e.g., top block, as this can get messy
    // if the block gets orphaned or blockchain reorganization
    // occurs. So the top 3 blocks (default number) will always
    // be calculated in flight and added to what we have so far.
    static uint64_t blockchain_chunk_gap;

    // current blockchain height and
    // hash of top block
    static atomic<uint64_t> current_height;


    // Written by the emission thread, read by the http threads. Emission
    // is not trivially copyable, so it cant be a std::atomic. Use
    // get_total_emission() and set_total_emission() to access it.
    static Emission total_emission;

    static mutex total_emission_mutex;


    static boost::thread m_thread;

    static atomic<bool> is_running;

    // make object for accessing the blockchain here
    static MicroCore* mcore;
    static Blockchain* core_storage;

    static void
    start_monitor_blockchain_thread();

    static void
    set_blockchain_variables(MicroCore* _mcore,
                             Blockchain* _core_storage);

    static Emission
    get_total_emission();

    static void
    set_total_emission(const Emission& emission);

    static void
    update_current_emission_amount();

    static Emission
    calculate_emission_in_blocks(uint64_t start_blk, uint64_t end_blk);

    static bool
    save_current_emission_amount();

    static bool
    load_current_emission_amount();

    static Emission
    get_emission();

    static bf::path
    get_output_file_path();

    static bool
    is_thread_running();
};

}

#endif //XMRBLOCKS_CURRENTBLOCKCHAINSTATUS_H
