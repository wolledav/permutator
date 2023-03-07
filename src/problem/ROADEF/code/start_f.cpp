#include <iostream>
#include <string>
#include <chrono>
#include "util.hpp"
#include "parser.hpp"
#include "instance.hpp"
#include "solution.hpp"
#include "params.hpp"
#include <thread>
#include <future>
#include <omp.h>


using namespace std;

int main(int argc, char *argv[]) {
    /* check if any argument was passed */
    if (argc < 2) {
        cerr << "No argument passed" << endl;
        exit(1);
    }

    //parse_global_params(argc, argv);

    /* parse file placed as first argument*/
    Parser *parser = new Parser(INPUT_PATH);
    parser->load();

    /* interpret data from parsed data */
    ins::Instance instance;
    parser->process(&instance);

    //determine_dependent_params(&instance);

    /* free memory occupied by parser */
    delete parser;

    #if VERBOSE_CONFIG
    dump_config();
    #endif

//    omp_set_num_threads(4);

//     ALNS search(&instance, SEED);
//     auto solution = search.greedy_search();
//     solution.round_up(2);
// //    solution.print_state();
//     solution.save(OUTPUT_PATH);

//    cout << "SEED: " << SEED << endl;
//    solution.print_state();

//    chrono::steady_clock::time_point end = chrono::steady_clock::now();
//    cout << "Time = " << chrono::duration_cast<chrono::milliseconds>(end - BEGIN).count() << "ms" << endl;
//    cout << endl;


    if (RETURN_ID) {
        return TEAM_ID;
    } else {
        return 0;
    }
}
