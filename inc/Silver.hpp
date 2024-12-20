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

#ifndef __NETLIST_HPP_
#define __NETLIST_HPP_

#include "config.hpp"

#include <set>
#include <map>
#include <chrono>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"

#include "NodeContext.hpp"

#define PARTITION_CLOCK_CYCLES 0

/**
 * @brief Type definitions for circuit (graph), node (vertex), and wire (edge).
 */
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, NodeContext> Circuit;
typedef boost::graph_traits<Circuit>::vertex_descriptor Node;
typedef boost::graph_traits<Circuit>::edge_descriptor Wire;
// typedef adjacency_list_traits<boost::vecS, boost::vecS, boost::bidirectionalS, NodeContext> Traits;

/**
 * @brief Type definition for Probes and Boolean sharing (unshared input / shared input indices)
 */
typedef std::vector<Node> Probes;

class Silver {

public:

    static Circuit parse(const std::string filePath, std::map<int, std::vector<Node>>& sharedInputs);

    static bool useFresh(Circuit& model);

    //static std::map<int, std::vector<Node>> 
    static std::vector<sylvan::Bdd> elaborate(Circuit &model, int improvedVarOrder, std::map<int, std::vector<Node>> sharedInputs);

    static bool check_Uniform(Circuit &model, std::chrono::time_point<std::chrono::high_resolution_clock> begin, int timeout);

    static bool check_Uniform3(Circuit& model);

    static bool check_Uniform2(Circuit& model, std::chrono::time_point<std::chrono::high_resolution_clock> begin, int timeout);

    static bool check_Uniform1(Circuit& model);

    static std::vector<Node> count_BddNode(Circuit& model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel);

    static std::vector<Node> check_Probing(Circuit &model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel, int verbose, int timeout);

    static std::vector<Node> reduce_Probing(Circuit &model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel, int verbose, int timeout, bool onlygp, bool useSubset);

    static std::vector<Node> check_NI(Circuit &model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel);

    static std::vector<Node> check_PINI(Circuit &model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel);

    static std::vector<Node> check_SNI(Circuit &model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel);

    static void print_node_vector(const Circuit &model, const std::vector<Node> nodes);

    static void print_node_vector_e(const Circuit& model, const std::vector<Node> nodes, std::string endin);

    static std::vector<Node> get_nodes_of_types(Circuit& model, const std::set<std::string> types);
private:
    static void inter_vector_combinations_and(const std::vector<std::vector<sylvan::Bdd>> &intra, int offset, sylvan::Bdd combination, std::vector<sylvan::Bdd> &inter);
    static bool inter_vector_combinations_xor(const std::vector<std::vector<sylvan::Bdd>> &intra, int offset, sylvan::Bdd combination, int varcount, std::chrono::time_point<std::chrono::high_resolution_clock> begin, int timeout);

    static bool inter_vector_combinations_xor1(Circuit& model, const std::vector<std::vector<std::vector<uint32_t>>>& intra, int offset, std::vector<uint32_t> combination, int varcount, std::chrono::time_point<std::chrono::high_resolution_clock> begin, int timeout);
 
    static int hammingWeight(int x);

    static bool inter_vector_combinations_and1(const std::vector<std::vector<sylvan::Bdd>>& intra, int offset, sylvan::Bdd combination, int varcount, int hw);

    static int get_minimal_sharing(std::map<int, Probes> inputs);
};
#endif // __NETLIST_HPP_