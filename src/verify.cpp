/*
 * -----------------------------------------------------------------
 * COMPANY : Ruhr-Universitšt Bochum, Chair for Security Engineering
 * AUTHOR  : Pascal Sasdrich (pascal.sasdrich@rub.de)
 * DOCUMENT: https://doi.org/10.1007/978-3-030-64837-4_26
 *           https://eprint.iacr.org/2020/634.pdf
 * -----------------------------------------------------------------
 *
 * Copyright (c) 2020, Pascal Sasdrich
 *
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Please see LICENSE and README for license and further instructions.
 */

#include <assert.h>
#include "config.hpp"
#include "Silver.hpp"

#include "parser/verilogParser.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sylvan/sylvan_cache.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

static std::chrono::time_point<std::chrono::high_resolution_clock> start;

static double elapsedTime() {
    std::chrono::duration<double, std::ratio<1>> elapsed = std::chrono::high_resolution_clock::now() - start;
    return elapsed.count();
}

static void INFO(const std::string info) {
    std::chrono::duration<double, std::ratio<1>> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "[" << std::setw(10) << std::fixed << std::setprecision(3) << elapsed.count() << "] " << info;
};

#define str(i) std::to_string(i)
#define time std::chrono::high_resolution_clock::now()

//
// Construct an argument parser using the boost program_options libary.
po::options_description build_argument_parser(
    silver_config_t * cfg
) {
    po::options_description all("Silver arguments");
    all.add_options()
    ("help" , "Show the help message")

    ("cores", po::value<unsigned int>(&cfg->CORES)->default_value(0),
        "Maximum number of CPU cores to use. Set to 0 (default) for auto-detect")

    ("memory", po::value<size_t>(&cfg->MEMORY)->default_value(6*1024*1024*1024ull),
        "RAM (in Bytes) used by Sylvan BDD library.")

    ("verbose", po::value<int>(&cfg->VERBOSE)->default_value(1),
        "Be verbose (or not) in printing detailed test reports.")

    ("verilog", po::value<bool>(&cfg->PARSE_VERILOG)->default_value(false),
        "Parse the verilog design described by the --verilog-* parameters.")

    ("verilog-libfile",po::value<std::string>(&cfg->LIBFILE)->default_value("cell/Library.txt"),
        "Technology library description.")

    ("verilog-libname",po::value<std::string>(&cfg->LIBNAME)->default_value("NANG45"),
        "Technology library name.")

    ("verilog-design_file",po::value<std::string>(&cfg->DESIGN)->default_value("vlog/aes/AES_Sbox_DOM/2-Synthesized/aes_sbox_dom1.v"),
        "Verilg source file containing the design.")

    ("verilog-module_name",po::value<std::string>(&cfg->MODULE)->default_value("aes_sbox"),
        "Module contained within the verilog source to verify.")

    ("insfile",po::value<std::string>(&cfg->INSFILE)->default_value("/home/lucien/projects/SILVER/test/aes/aes_sbox_dom1.nl"),
        "Instruction list to parse and process. Either externally provided or result of verilog parser")

    ("varorder", po::value<int>(&cfg->VARORDER)->default_value(1),
        "Var ordering. 1 is recommended.\n0 : original, shares as bdd variable. \n1 : secrets as bdd variables. \n2 : shares as bdd variables")

    ("timeout", po::value<int>(&cfg->TIMEOUT)->default_value(365*24*60),
        "Exit if exceeding the time limit (unit : minute). The default timeout is 1 year.")

    ("count_node", po::value<bool>(&cfg->COUNT_NODES)->default_value(false),
        "Count the number of internal nodes in functions.")

    ("info", po::value<bool>(&cfg->INFO)->default_value(false),
        "Gives information about the test bench.")

    ("userule", po::value<bool>(&cfg->USE_RULE)->default_value(true),
        "Check glitch-extended probing security using reductaion rules.")

    ("usesubset", po::value<bool>(&cfg->USE_SUBSET)->default_value(true),
        "Enabling the use of subset strategy.")

    ("onlygp", po::value<bool>(&cfg->ONLY_GP)->default_value(false),
        "Only check glitch-extended probing security (for the purpose of comparing Prover and SILVER).")
    // ("prover", po::value<bool>(&cfg->PROVER)->default_value(true),
    //     "Use Prover rather than SILVER.")

    ("debuginfo", po::value<bool>(&cfg->DEBUG_INFO)->default_value(false),
        "Show intermediate information during verification")

    ("notions", po::value<unsigned int>(&cfg->VERIFY_NOTION)->default_value(3),
            "The notions to verify. 0: standard probing security; 1: glitch-extended probing security; 2: uniformity; >=3: all above security notions")
    
    ("uniform", po::value<unsigned int>(&cfg->m_Uniform)->default_value(3),
            "The method to check uniformity. 0: silver; 1: prover; 2: silver but and lemma; >=3: let prover decide")

    ;

    return all;
}

VOID_TASK_0(gc_start)
{
    INFO("(GC) Starting garbage collection...\n");
}

VOID_TASK_0(gc_end)
{
    INFO("(GC) Garbage collection done.\n");
}

void test() {
    std::set<uint32_t>::iterator pos;
    std::set<uint32_t> ivar = { 0,1,2,3,4,5,6 };
    for (int comb = 1; comb < (1 << ivar.size()); comb++) {
        int ivar_comb_elem = 0;
        pos = ivar.begin();
        for (int elem = 0; elem < ivar.size(); elem++, pos++) {
            if (comb & (1 << elem)) {
                ivar_comb_elem ^= (1 << *pos);
            }
        }
        std::cout << ivar_comb_elem << std::endl;
        assert(comb == ivar_comb_elem);
        assert(false);
    }
    exit(0);
}

void check_Security(std::string notion, Circuit model, std::map<int, Probes> inputs, silver_config_t cfg, int order) {
    /* Start LACE framework */
    lace_init(cfg.CORES, 0);
    lace_startup(0, NULL, NULL);

    
    std::vector<Node> probes;
    /* Start BDD session */
    sylvan::sylvan_set_limits(cfg.MEMORY, 0, 10);
    sylvan::sylvan_init_package();
    sylvan::sylvan_init_mtbdd();
    double t1 = elapsedTime();
    /* Elabotare circuit model */
    Silver::elaborate(model, cfg.VARORDER, inputs);
    if(notion == "standard")
    if (cfg.VERBOSE) INFO("Elaborate: " + str(num_vertices(model)) + " gate(s) / " + str(num_edges(model)) + " signal(s)\n");
    else{
        std::cout << str(elapsedTime() - t1) << ",";
        t1 = elapsedTime();
    }
    if (notion == "standard") {
        /* Standard probing security */
        if (cfg.USE_RULE)
            probes = Silver::reduce_Probing(model, inputs, order, false, cfg.VERBOSE, cfg.TIMEOUT, cfg.ONLY_GP, cfg.USE_SUBSET);
        else
            probes = Silver::check_Probing(model, inputs, order, false, cfg.VERBOSE, cfg.TIMEOUT);
        if (cfg.VERBOSE == 0) {
            std::cout << str(elapsedTime() - t1) << ",";
            t1 = elapsedTime();
            std::cout << str(probes.size() - 1) << ",";
            Silver::print_node_vector_e(model, probes, ",");
        }
        else {
            if (probes.size() - 1 != 0) INFO("probing.standard (d \u2264 " + str(probes.size() - 1) + ") -- \033[1;32mPASS\033[0m.");
            else                        INFO("probing.standard (d \u2264 " + str(probes.size() - 0) + ") -- \033[1;31mFAIL\033[0m.");
            std::cout << "\t>> Probes: "; Silver::print_node_vector(model, probes);

        }

    }
    else if (notion == "robust") {
        /* Robust probing security */
        if (cfg.USE_RULE)
            probes = Silver::reduce_Probing(model, inputs, order, true, cfg.VERBOSE, cfg.TIMEOUT, cfg.ONLY_GP, cfg.USE_SUBSET);
        else
            probes = Silver::check_Probing(model, inputs, order, true, cfg.VERBOSE, cfg.TIMEOUT);
        if (cfg.VERBOSE == 0) {
            std::cout << str(elapsedTime() - t1) << ",";
            t1 = elapsedTime();
            std::cout << str(probes.size() - 1) << ",";
            Silver::print_node_vector_e(model, probes, ",");
        }
        else {
            if (probes.size() - 1 != 0) INFO("probing.robust   (d \u2264 " + str(probes.size() - 1) + ") -- \033[1;32mPASS\033[0m.");
            else                        INFO("probing.robust   (d \u2264 " + str(probes.size() - 0) + ") -- \033[1;31mFAIL\033[0m.");
            std::cout << "\t>> Probes: "; Silver::print_node_vector(model, probes);
        }

    }
    else if (notion == "uniformity") {
        /* Standard uniformity check */
        bool uniform;
        std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();
        if (cfg.m_Uniform >= 3) {
            if (Silver::useFresh(model) && cfg.USE_RULE == 1) {
                //std::cout << "great!" << std::endl;
                uniform = Silver::check_Uniform2(model, begin, cfg.TIMEOUT);
            }
            else {
                // std::cout << std::endl;
                uniform = Silver::check_Uniform(model, begin, cfg.TIMEOUT);
            }

        }
        else if (cfg.m_Uniform == 0) {
            uniform = Silver::check_Uniform(model, begin, cfg.TIMEOUT);
        }
        else if (cfg.m_Uniform == 1) {
            uniform = Silver::check_Uniform2(model, begin, cfg.TIMEOUT);
        }
        else if (cfg.m_Uniform == 2) {
            uniform = Silver::check_Uniform3(model);
        }

        // uniform = Silver::check_Uniform3(model);
        if (cfg.VERBOSE == 0) {
            std::cout << str(elapsedTime() - t1) << ",";
            if (uniform) std::cout << "1" << std::endl;
            else std::cout << "0" << std::endl;
        }
        else {
            if (uniform)  INFO("uniformity               -- \033[1;32mPASS\033[0m.\n");
            else          INFO("uniformity               -- \033[1;31mFAIL\033[0m.\n");
        }
    }
    /* Terminate BDD session */
    if (cfg.DEBUG_INFO) sylvan::sylvan_stats_report(stdout);
    sylvan::sylvan_quit();
    lace_exit();

}

int CountFresh(Circuit& model) {
    std::pair<boost::adjacency_list<>::vertex_iterator,
        boost::adjacency_list<>::vertex_iterator> vs = boost::vertices(model);
    int count = 0;
    for (auto node = vs.first; node != vs.second; node++) {
        //if (model[*node].getType() != "ref" && model[*node].getType() != "in") {
        //    use_fresh = false;
        //}
        if (model[*node].getType() == "ref" && out_degree(*node, model) != 0) {
            //use_fresh = true;
            //break;
            count++;
        }
    }
    //std::map<int, std::vector<Node>> sharedOutputs;
    //for (auto node = vs.first; node != vs.second; node++) {
    //    //auto type = model[*node].getType();
    //    if (model[*node].getType() == "out") {
    //        sharedOutputs[model[*node].getSharing().first].push_back(*node);
    //        //std::cout << *node << std::endl;
    //    }
    //    /*else
    //        break;*/
    //}
    //int minimal = 0;
    ///* Find smallest sharing */
    //for (int index = 0; index < sharedOutputs.size(); index++) {
    //    minimal = (sharedOutputs[index].size() < sharedOutputs[minimal].size()) ? index : minimal;
    //}

    //int numberOfOutputShares = sharedOutputs[minimal].size();

    return count;
}



int main (int argc, char * argv[]) {
    //test();

    /* Configuration options / command line arguments */
    silver_config_t cfg;

    /* Build the argument parser and run it on argc/argv */
    po::options_description desc = build_argument_parser(&cfg);
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    /* Variable declarations */    
    Circuit model;
    std::vector<Node> probes;

    int order, minimal = 0;
    bool check;


    // Before and after garbage collection, call gc_start and gc_end
    if (cfg.DEBUG_INFO){
        sylvan::sylvan_gc_hook_pregc(TASK(gc_start));
        sylvan::sylvan_gc_hook_postgc(TASK(gc_end));
    }
    /* Extract Designs Under Test */
    std::string dut = cfg.INSFILE;

    /* Start time tracking */
    start = std::chrono::high_resolution_clock::now();

    /* Parse & convert verilog design to instruction list */
    if(cfg.PARSE_VERILOG) {
        INFO("Parsing verilog design " + cfg.DESIGN + " with top module " + cfg.MODULE +"\n");
        int res = parse_and_convert_wrapper(
            cfg.LIBFILE, cfg.LIBNAME, cfg.DESIGN, cfg.MODULE, cfg.INSFILE
        );
        if (res) {
            std::cout << "Verilog design parsing failed." << std::endl;
            return res;
        }
    }
    ///* Parse circuit from text file*/
    if ((!cfg.ONLY_GP || !cfg.COUNT_NODES) && cfg.VERBOSE) INFO("Netlist: " + dut + "\n");
    std::map<int, Probes> inputs;
    model = Silver::parse(dut, inputs);
    if (cfg.INFO) {
        std::vector<Node> positions = Silver::get_nodes_of_types(model, { "out", "reg" }) ;
        int num_fresh = CountFresh(model);
        printf("%d, %d, %d, %d, %d\n", inputs.size(), inputs[0].size(), num_fresh, num_vertices(model), positions.size());
        exit(0);
    }
    double t1;
    if (cfg.VERBOSE) INFO("Parse: " + str(num_vertices(model)) + " gate(s) / " + str(num_edges(model))  + " signal(s)\n");
    else {
        t1 = elapsedTime();
    }

    /* Find smallest sharing */
    for (int index = 0; index < inputs.size(); index++) {
        minimal = (inputs[index].size() < inputs[minimal].size()) ? index : minimal;
    }

    /* Define order of security (based on minimal sharing) */
    order = inputs[minimal].size() - 1;

    if(cfg.VERIFY_NOTION >= 3 || cfg.VERIFY_NOTION == 0) check_Security("standard", model, inputs, cfg, order);
    if(cfg.VERIFY_NOTION >= 3 || cfg.VERIFY_NOTION == 1) check_Security("robust", model, inputs, cfg, order);
    if(cfg.VERIFY_NOTION >= 3 || cfg.VERIFY_NOTION == 2) check_Security("uniformity", model, inputs, cfg, order);

    if (cfg.COUNT_NODES) {
        probes = Silver::count_BddNode(model, inputs, order, true);
        std::cout << str(elapsedTime() - t1) << ",";
        t1 = elapsedTime();
        if (probes.size() - 1 != 0) std::cout << str(probes.size() - 1) << ",";
        else                        std::cout << str(probes.size() - 0) << ",";
        Silver::print_node_vector(model, probes); 
        exit(0);
    }

    /* Remove model graph from memory */
    model.clear();


    return 0;
}
