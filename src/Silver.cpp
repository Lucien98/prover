/*
 * -----------------------------------------------------------------
 * COMPANY : Ruhr-Universität Bochum, Chair for Security Engineering
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

#include <util.hpp>
#include <Silver.hpp>

#include <sylvan/sylvan_ext.h>

using namespace boost;
using namespace sylvan;

static std::chrono::time_point<std::chrono::high_resolution_clock> start;

static double elapsedTime() {
    std::chrono::duration<double, std::ratio<1>> elapsed = std::chrono::high_resolution_clock::now() - start;
    return elapsed.count();
}

static double elapsedTime(std::chrono::time_point<std::chrono::high_resolution_clock> begin) {
    std::chrono::duration<double, std::ratio<1>> elapsed = std::chrono::high_resolution_clock::now() - begin;
    return elapsed.count();
}

static void INFO(const std::string info) {
    std::chrono::duration<double, std::ratio<1>> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "[*" << std::setw(10) << std::fixed << std::setprecision(3) << elapsed.count() << "] " << info;
};

#define str(i) std::to_string(i)
#define time std::chrono::high_resolution_clock::now()

void setLastShare(Circuit& model, std::map<int, std::vector<Node>> sharedInputs) {
    for (int i = 0; i < sharedInputs.size(); ++i)
    {
        Node n = *(sharedInputs[i].rbegin());
        model[n].getPerfectM()->clear();
        model[n].getSupportV()->clear();
        model[n].getPerfectM()->insert(sharedInputs[i].begin(), sharedInputs[i].end() - 1);
        model[n].getSupportV()->insert(sharedInputs[i].begin(), sharedInputs[i].end() - 1);
    }
}


/**
 * Implementation of Netlist static class functions.
 */

/* Circuit parsing and graph generation */
Circuit
Silver::parse(const std::string filePath, std::map<int, std::vector<Node>>& sharedInputs)
{
    std::vector<std::string> tokens, annotations;
    std::string line, token;
    Circuit model;

    std::ifstream description(filePath);
    if (!description) { std::cout << "File does not exist!\n"; exit(0); }
    while(std::getline(description, line))
    {
        tokens = split(line, ' ');

        Node node = add_vertex(model);

        model[node].setType(tokens[0]);
        if (unary.find(tokens[0]) != unary.end()) {
            add_edge(std::stoi(tokens[1]), node, model);
        } else if (binary.find(tokens[0]) != binary.end()) {
            add_edge(std::stoi(tokens[1]), node, model);
            add_edge(std::stoi(tokens[2]), node, model);
        } else if (!(tokens[0] == "in" || tokens[0] == "ref")) {
            std::cerr << "[ERR-PARSER] Unsupported node detected: line #" << node+1 << std::endl;
        }

        if (model[node].getType() == "in" || model[node].getType() == "reg" || model[node].getType() == "out") {
            if (tokens.size() > 2 && tokens[2].find("#") != 0) {
                annotations = split(tokens[2], '_');
                if (annotations.size() == 2) {
                    model[node].setSharing({std::stoi(annotations[0]),std::stoi(annotations[1])});
                } else {
                    std::cerr << "[ERR-PARSER] Unsupported annotations detected: line #" << node+1 << std::endl;
                }
            } else {
                model[node].setSharing({std::stoi(tokens[1]),0});
            }
        }
        if (model[node].getType() == "in") {
            sharedInputs[model[node].getSharing().first].push_back(node);
            model[node].setNodeId(node);
            model[node].computeAuxiliaryTable();
        }
    }
    setLastShare(model,sharedInputs);

    description.close();

    return model;
}

bool Silver::useFresh(Circuit& model) {
    std::pair<boost::adjacency_list<>::vertex_iterator,
        boost::adjacency_list<>::vertex_iterator> vs = boost::vertices(model);
    //for (auto node = vs.first; node != vs.second; node++) {
    //    if (model[*node].getType() != "ref" && model[*node].getType() != "in") {
    //        return false;
    //    }
    //    if (model[*node].getType() == "ref" && out_degree(*node, model) != 0) {
    //        return true;
    //    }
    //}
    std::map<int, std::vector<Node>> sharedOutputs;
    for (auto node = vs.second; node != vs.first; node--) {
        if (model[*node].getType() == "out") {
            sharedOutputs[model[*node].getSharing().first].push_back(*node);
            //std::cout << *node << std::endl;
        }
        else 
            break;
    }
    int minimal;
    /* Find smallest sharing */
    for (int index = 0; index < sharedOutputs.size(); index++) {
        minimal = (sharedOutputs[index].size() < sharedOutputs[minimal].size()) ? index : minimal;
    }

    return (minimal > 2);
}

/* Circuit elaboration */
std::vector<Bdd>
Silver::elaborate(Circuit &model, int improvedVarOrder,std::map<int, std::vector<Node>> sharedInputs) {
    std::vector<Node> sorted;
    Node op1, op2;
    int num_secrets = sharedInputs.size();
    int num_shares = sharedInputs[0].size();
    // std::cout << "the size of secrets is " << num_secrets << std::endl;
    int varorder = 0;
    std::vector<Bdd> secrets;
    for (int i = 0; i < num_secrets; i++) {
        if (improvedVarOrder == 1) secrets.push_back(sylvan_ithvar(i));
        varorder++;
    }

    boost::topological_sort(model, std::back_inserter(sorted));
    start = std::chrono::high_resolution_clock::now();
    for (auto node = sorted.rbegin(); node != sorted.rend(); ++node) {
        model[*node].setGateId(*node);
        
        if (unary.find(model[*node].getType()) != unary.end()) {
            op1 = source(*(in_edges(*node, model).first+0), model);
            
            model[*node].setLeftChild(&model[op1]);
            model[*node].setRightChild(NULL);
        } else if (binary.find(model[*node].getType()) != binary.end()) {
            op1 = source(*(in_edges(*node, model).first+0), model);
            op2 = source(*(in_edges(*node, model).first+1), model);
            model[*node].setLeftChild(&model[op1]);
            model[*node].setRightChild(&model[op2]);
        } else if (!(model[*node].getType() == "in" || model[*node].getType() == "ref")) {
            std::cerr << "ERR1: Unsupported node detected. (ELABORATE)" << std::endl;
        }
        model[*node].clearVariables();
        if (model[*node].getType() == "in") {
            if (improvedVarOrder == 1)
            {
                int secret_index = model[*node].getSharing().first;
                int share_index = model[*node].getSharing().second;
                
                int last_share_id = *(sharedInputs[secret_index].begin());
                
                if (*node == last_share_id) {
                    sylvan::Bdd func = secrets[secret_index];
                    for (std::vector<Node>::iterator i = sharedInputs[secret_index].begin() + 1; i != sharedInputs[secret_index].end(); ++i)
                    {
                        func ^= model[*i].getFunction();
                    }
                    model[*node].setFunction(func);

                }
                else {
                    model[*node].setFunction(sylvan_ithvar(varorder++));
                }
            }
            else if (improvedVarOrder == 2) {
                    std::pair<int, int> shr = model[*node].getSharing();
                    int ith = shr.first * num_shares + shr.second;
                    // std::cout << ith << std::endl;
                    model[*node].setFunction(sylvan_ithvar(ith));
                }
            else {
                model[*node].setFunction(sylvan_ithvar(*node));
            }
        } else if (model[*node].getType() == "ref") {
            model[*node].setFunction(sylvan_ithvar(*node));
        } else if (model[*node].getType() == "out" || model[*node].getType() == "reg") {
            model[*node].setFunction(model[op1].getFunction());
        } else if (model[*node].getType() == "not" || model[*node].getType() == "regn") {
            model[*node].setFunction(!model[op1].getFunction());
        } else if (model[*node].getType() == "and") {
            model[*node].setFunction((model[op1].getFunction() & model[op2].getFunction()));
        } else if (model[*node].getType() == "nand") {
            model[*node].setFunction(!(model[op1].getFunction() & model[op2].getFunction()));
        } else if (model[*node].getType() == "or") {
            model[*node].setFunction((model[op1].getFunction() | model[op2].getFunction()));
        } else if (model[*node].getType() == "nor") {
            model[*node].setFunction(!(model[op1].getFunction() | model[op2].getFunction()));
        } else if (model[*node].getType() == "xor") {
            model[*node].setFunction((model[op1].getFunction() ^ model[op2].getFunction()));
        } else if (model[*node].getType() == "xnor") {
            model[*node].setFunction(!(model[op1].getFunction() ^ model[op2].getFunction()));
        } else {
            std::cerr << "ERR2: Unsupported node detected. (ELABORATE)" << std::endl;
        }
        model[*node].clearVariables();
        if (model[*node].getType() == "in") {
            model[*node].addVariables(Bdd::bddVar(*node));
        } else if (model[*node].getType() == "ref") {
            model[*node].addVariables(Bdd::bddVar(*node));
        } else if (unary.find(model[*node].getType()) != unary.end()) {
            model[*node].addVariables(model[op1].getVariables());
        } else if (binary.find(model[*node].getType()) != binary.end()) {
            model[*node].addVariables(model[op1].getVariables());
            model[*node].addVariables(model[op2].getVariables());
        } else {
            std::cerr << "ERR: Unsupported node detected. (ELABORATE)" << std::endl;
        }

        model[*node].clearRegisters();
        if (model[*node].getType() == "in") {
            model[*node].addRegisters(Bdd::bddVar(*node));
        } else if (model[*node].getType() == "ref") {
            model[*node].addRegisters(Bdd::bddVar(*node));
        } else if (unary.find(model[*node].getType()) != unary.end()) {
            if (model[op1].getType() == "reg" || model[op1].getType() == "regn")  model[*node].addRegisters(Bdd::bddVar(op1));
            else                                                                  model[*node].addRegisters(model[op1].getRegisters());
        } else if (binary.find(model[*node].getType()) != binary.end()) {
            if (model[op1].getType() == "reg" || model[op1].getType() == "regn")  model[*node].addRegisters(Bdd::bddVar(op1));
            else                                                                  model[*node].addRegisters(model[op1].getRegisters());
            if (model[op2].getType() == "reg" || model[op2].getType() == "regn")  model[*node].addRegisters(Bdd::bddVar(op2));
            else                                                                  model[*node].addRegisters(model[op2].getRegisters());
        } else {
            std::cerr << "ERR: Unsupported node detected. (ELABORATE)" << std::endl;
        }

        if (model[*node].getType() == "in") {
            //sharedInputs[model[*node].getSharing().first].push_back(*node);
        }
        else
        {
            model[*node].computeAuxiliaryTable();
        }
    }

    return secrets;
}

/* Uniformity check */
bool
Silver::check_Uniform(Circuit& model, std::chrono::time_point<std::chrono::high_resolution_clock> begin, int timeout)
{
    LACE_ME;

    int varcount = 0;
    for (auto node = vertices(model).first; node != vertices(model).second; node++)
        if (model[*node].getType() == "in" || model[*node].getType() == "ref") varcount++;

    std::vector<Bdd> inputs, outputs;
    for (auto gate = vertices(model).first; gate != vertices(model).second; ++gate) {
        if (model[*gate].getType() == "in" || model[*gate].getType() == "ref") inputs.push_back(model[*gate].getFunction());
        if (model[*gate].getType() == "out") outputs.push_back(model[*gate].getFunction());
    }

    if (inputs.size() == outputs.size()) {

        std::vector<uint32_t> from(inputs.size()), to(inputs.size());
        for (int index = 0; index < inputs.size(); index++) {
            from[index] = inputs[index].TopVar(); to[index] = 2 * index;
        }

        std::vector<Bdd> relations(outputs.size());
        std::vector<BddSet> variables(outputs.size());

        for (int index = 0; index < outputs.size(); index++) {
            relations[index] = (outputs[index].Permute(from, to) ^ sylvan_nithvar(2 * index + 1));
            variables[index] = (outputs[index].Support());
        }

        for (int index = relations.size() - 1; index >= 0; index--) {
            for (int r = 0; r < index; r++) {
                BddSet curr = variables[r]; curr.remove(variables[index]);

                if (curr.size() != variables[r].size()) {

                    relations[r] &= relations[index]; relations.erase(relations.begin() + index);
                    variables[r].add(variables[index]); variables.erase(variables.begin() + index);
                    break;
                }
            }
        }

        Bdd states = Bdd::bddOne().RelNext(relations[0], BddSet(sylvan_false));
        for (int index = 1; index < relations.size(); index++) {
            states &= Bdd::bddOne().RelNext(relations[index], BddSet(sylvan_false));
        }

        return (mtbdd_satcountln(states.GetBDD(), outputs.size()) == outputs.size());

    }
    else {
        std::map<int, std::vector<Node>> shares;
        for (auto node = vertices(model).first; node != vertices(model).second; node++) {
            if (model[*node].getType() == "out") shares[model[*node].getSharing().first].push_back(*node);
        }

        std::vector<std::vector<Bdd>> intra(shares.size());
        for (int idx = 0; idx < shares.size(); idx++) {
            for (int comb = 1; comb < ((1 << shares[idx].size()) - 1); comb++) {
                intra[idx].push_back(sylvan::sylvan_false);
                for (int elem = 0; elem < shares[idx].size(); elem++) {
                    if (comb & (1 << elem)) intra[idx].back() ^= model[shares[idx][elem]].getFunction();
                }
                if (abs(mtbdd_satcountln(intra[idx].back().GetBDD(), varcount) - varcount + 1) > DOUBLE_COMPARE_THRESHOLD) return false;
            }
        }

        return inter_vector_combinations_xor(intra, 0, Bdd::bddZero(), varcount, begin, timeout);
    }
}
bool
Silver::check_Uniform3(Circuit& model)
{
    LACE_ME;

    int varcount = 0;
    for (auto node = vertices(model).first; node != vertices(model).second; node++)
        if (model[*node].getType() == "in" || model[*node].getType() == "ref") varcount++;

    std::vector<Bdd> inputs, outputs;
    for (auto gate = vertices(model).first; gate != vertices(model).second; ++gate) {
        if (model[*gate].getType() == "in" || model[*gate].getType() == "ref") inputs.push_back(model[*gate].getFunction());
        if (model[*gate].getType() == "out") outputs.push_back(model[*gate].getFunction());
    }

    if (inputs.size() == outputs.size()) {

        std::vector<uint32_t> from(inputs.size()), to(inputs.size());
        for (int index = 0; index < inputs.size(); index++) {
            from[index] = inputs[index].TopVar(); to[index] = 2 * index;
        }

        std::vector<Bdd> relations(outputs.size());
        std::vector<BddSet> variables(outputs.size());

        for (int index = 0; index < outputs.size(); index++) {
            relations[index] = (outputs[index].Permute(from, to) ^ sylvan_nithvar(2 * index + 1));
            variables[index] = (outputs[index].Support());
        }

        for (int index = relations.size() - 1; index >= 0; index--) {
            for (int r = 0; r < index; r++) {
                BddSet curr = variables[r]; curr.remove(variables[index]);

                if (curr.size() != variables[r].size()) {

                    relations[r] &= relations[index]; relations.erase(relations.begin() + index);
                    variables[r].add(variables[index]); variables.erase(variables.begin() + index);
                    break;
                }
            }
        }

        Bdd states = Bdd::bddOne().RelNext(relations[0], BddSet(sylvan_false));
        for (int index = 1; index < relations.size(); index++) {
            states &= Bdd::bddOne().RelNext(relations[index], BddSet(sylvan_false));
        }

        return (mtbdd_satcountln(states.GetBDD(), outputs.size()) == outputs.size());

    }
    else {
        std::map<int, std::vector<Node>> shares;
        for (auto node = vertices(model).first; node != vertices(model).second; node++) {
            if (model[*node].getType() == "out") shares[model[*node].getSharing().first].push_back(*node);
        }

        std::vector<std::vector<Bdd>> intra(shares.size());
        for (int idx = 0; idx < shares.size(); idx++) {
            for (int comb = 1; comb < ((1 << shares[idx].size()) - 1); comb++) {
                intra[idx].push_back(sylvan::sylvan_true);
                for (int elem = 0; elem < shares[idx].size(); elem++) {
                    if (comb & (1 << elem)) intra[idx].back() &= model[shares[idx][elem]].getFunction();
                }
                /* if (abs(mtbdd_satcountln(intra[idx].back().GetBDD(), varcount) - varcount + 1) > DOUBLE_COMPARE_THRESHOLD) return false;*/
            }
        }

        return inter_vector_combinations_and1(intra, 0, Bdd::bddOne(), varcount, 0);
    }
}

bool
Silver::check_Uniform2(Circuit& model, std::chrono::time_point<std::chrono::high_resolution_clock> begin, int timeout)
{
    LACE_ME;

    int varcount = 0;
    for (auto node = vertices(model).first; node != vertices(model).second; node++)
        if (model[*node].getType() == "in" || model[*node].getType() == "ref") varcount++;

    std::vector<Bdd> inputs, outputs;
    for (auto gate = vertices(model).first; gate != vertices(model).second; ++gate) {
        if (model[*gate].getType() == "in" || model[*gate].getType() == "ref") inputs.push_back(model[*gate].getFunction());
        if (model[*gate].getType() == "out") outputs.push_back(model[*gate].getFunction());
    }

    if (inputs.size() == outputs.size()) {

        std::vector<uint32_t> from(inputs.size()), to(inputs.size());
        for (int index = 0; index < inputs.size(); index++) {
            from[index] = inputs[index].TopVar(); to[index] = 2 * index;
        }

        std::vector<Bdd> relations(outputs.size());
        std::vector<BddSet> variables(outputs.size());

        for (int index = 0; index < outputs.size(); index++) {
            relations[index] = (outputs[index].Permute(from, to) ^ sylvan_nithvar(2 * index + 1));
            variables[index] = (outputs[index].Support());
        }

        for (int index = relations.size() - 1; index >= 0; index--) {
            for (int r = 0; r < index; r++) {
                BddSet curr = variables[r]; curr.remove(variables[index]);

                if (curr.size() != variables[r].size()) {

                    relations[r] &= relations[index]; relations.erase(relations.begin() + index);
                    variables[r].add(variables[index]); variables.erase(variables.begin() + index);
                    break;
                }
            }
        }

        Bdd states = Bdd::bddOne().RelNext(relations[0], BddSet(sylvan_false));
        for (int index = 1; index < relations.size(); index++) {
            states &= Bdd::bddOne().RelNext(relations[index], BddSet(sylvan_false));
        }

        return (mtbdd_satcountln(states.GetBDD(), outputs.size()) == outputs.size());

    }
    else {
        std::map<int, std::vector<uint32_t>> shares;
        for (auto node = vertices(model).first; node != vertices(model).second; node++) {
            if (model[*node].getType() == "out") shares[model[*node].getSharing().first].push_back(*node);
        }

        //std::vector< std::vector<std::vector<uint32_t>>> intra(shares.size(), std::vector<std::vector<uint32_t>>(((1 << shares[0].size()) - 1)));
        std::vector< std::vector<std::vector<uint32_t>>> intra;
        
        
        for (int idx = 0; idx < shares.size(); idx++) {
            std::vector<std::vector<uint32_t>> comb_hw_d;
            for (int i = 0; i < shares[idx].size(); i++) {
                std::vector<uint32_t> vec(shares[idx]);
                vec.erase(vec.begin() + i);
                comb_hw_d.push_back(vec);
            }
            intra.push_back(comb_hw_d);
            //for (int comb = 1; comb < ((1 << shares[idx].size()) - 1); comb++) {
            //    intra[idx][comb-1].clear();

            //    for (int elem = 0; elem < shares[idx].size(); elem++) {
            //        if (comb & (1 << elem)) intra[idx][comb-1].push_back(shares[idx][elem]);
            //    }
            //    /* if (abs(mtbdd_satcountln(intra[idx].back().GetBDD(), varcount) - varcount + 1) > DOUBLE_COMPARE_THRESHOLD) return false;*/
            //}
        }
        std::vector<uint32_t> empty;
        return inter_vector_combinations_xor1(model, intra, 0, empty, varcount, begin, timeout);
    }
}

bool
Silver::check_Uniform1(Circuit &model)
{
    LACE_ME;

    int varcount = 0;
    for (auto node = vertices(model).first; node != vertices(model).second; node++)
        if (model[*node].getType() == "in" || model[*node].getType() == "ref") varcount++;

    std::vector<uint32_t> extended;
    for (auto gate = vertices(model).first; gate != vertices(model).second; ++gate) {
        if (model[*gate].getType() == "out" && model[*gate].getSharing().second == 0) {
            extended.push_back(*gate);
        }
    }
    /*std::cout << extended.size() << std::endl;
    for (auto i :extended) {
        std::cout << i << " ";
    }
    std::cout << std::endl;*/
    Bdd observation;
    Bdd observation1;
    int hw;
    for (int comb = 1; comb < (1 << extended.size()); comb++) {
        observation = sylvan::sylvan_true;
        hw = 0;
        std::cout << comb << std::endl;
        for (int elem = extended.size() - 1; elem >= 0; elem--)
            if (comb & (1 << elem)) {
                observation &= model[extended[elem]].getFunction();
                //std::cout << extended[elem] << std::endl;
                hw++;
            }
        double p = mtbdd_satcountln(observation.GetBDD(), varcount);
        double delta = p + hw - varcount;
        //std::cout << "prob: " << p << " hw: " << hw << "\n";
        if (abs(delta) !=0 ) {
            //std::cout << comb << std::endl;
            return false;
        }
        //bool balanced = true;
        //if (!balanced) return false;
        // observation1 = sylvan::sylvan_true;
        // for (int elem = extended.size() - 1; elem >= 0; elem--)
        //     if (comb & (1 << elem)) {
        //         observation1 ^= model[extended[elem]].getFunction();
        //         hw++;
        //     }
        // double p1 = mtbdd_satcountln(observation1.GetBDD(), varcount);
        // double delta1 = p1 - varcount;
        // std::cout << "prob: " << p1  << "\n";
        // if (abs(delta1) != 1) {
        //     return false;
        // }
    }
    return true;
}

/* Count Bdd node*/
std::vector<Node>
Silver::count_BddNode(Circuit& model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel)
{
    LACE_ME;

    std::vector<Node> positions = (robustModel) ? get_nodes_of_types(model, rprobes) : get_nodes_of_types(model, sprobes);

    int minimal = get_minimal_sharing(inputs);

    if (probingOrder == 0 || inputs[minimal].size() < 2 || inputs[minimal].size() < probingOrder) return inputs[minimal];

    std::vector<Bdd> secrets(inputs.size());
    for (int index = 0; index < inputs.size(); index++) {
        secrets[index] = model[inputs[index][0]].getFunction();
        for (int elem = 1; elem < inputs[index].size(); elem++) secrets[index] ^= model[inputs[index][elem]].getFunction();
    }
    uint64_t nc_f = 0;
    uint64_t nc_x = 0;
    std::vector<Bdd> secrets_comb(1 << inputs.size());
    for (int comb = 1; comb < (1 << inputs.size()); comb++) {
        secrets_comb[comb] = Bdd::bddOne();
        for (int elem = 0; elem < inputs.size(); elem++) if (comb & (1 << elem)) secrets_comb[comb] &= secrets[elem];
    }
    for (int i = 1; i < (1 << inputs.size()); i++) {
        int ncx = sylvan_nodecount(secrets_comb[i].GetBDD());
        // std::cout << ncx << std::endl;
        nc_x += ncx;
    }
    // std::cout << "the first variable: " << std::endl;
    // std::cout << sylvan_nodecount(sylvan_ithvar(0)) << std::endl;
    // std::cout << sylvan_nodecount(sylvan_ithvar(1)) << std::endl;
    // std::cout << sylvan_nodecount(sylvan_ithvar(1) ^ sylvan_ithvar(0)) << std::endl;
    // std::cout << sylvan_nodecount(sylvan_ithvar(0) ^ sylvan_ithvar(1)) << std::endl;
    // std::cout << sylvan_nodecount(sylvan_ithvar(0) * sylvan_ithvar(1)) << std::endl;
    std::cout << std::to_string(nc_x) << ", ";

    int varcount = 0;
    for (auto node = vertices(model).first; node != vertices(model).second; node++)
        if (model[*node].getType() == "in" || model[*node].getType() == "ref") varcount++;
    start = std::chrono::high_resolution_clock::now();

    for (int order = 0; order < probingOrder; order++) {
        std::vector<Node> probes(order + 1);
        std::vector<bool> bitmask(positions.size()); std::fill(bitmask.begin(), bitmask.begin() + (order + 1), true);
        do {
            int probe = 0; for (int idx = 0; idx < bitmask.size(); idx++) if (bitmask[idx]) probes[probe++] = positions[idx];

            if (robustModel) {
                Bdd observation = model[probes[0]].getRegisters();
                for (int probe = 1; probe < probes.size(); probe++)
                {
                    observation &= model[probes[probe]].getRegisters();
                }

                std::vector<uint32_t> extended = BddSet(observation).toVector();
                for (int comb = 1; comb < 1 << extended.size() ; comb ++) {
                // for (int comb = (1 << extended.size()) - 1; comb > 0; comb--) {
                    observation = sylvan::sylvan_true;
                    for (int elem = extended.size() - 1; elem >= 0; elem--) {
                        if (comb & (1 << elem)) {
                            observation &= model[extended[elem]].getFunction();
                        }
                    }
                    nc_f += sylvan_nodecount(observation.GetBDD()); 
                    bool independent = true;
                    for (int idx = 0; idx < secrets.size() && independent; idx++) {
                        independent &= CALL(mtbdd_statindependence, observation.GetBDD(), varcount, secrets[idx].GetBDD(), varcount);
                    }
                    if (!independent) {
                        std::cout << std::to_string(nc_f) << ", ";
                        return probes;
                    }
                }
            }
        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }
    std::cout << std::to_string(nc_f) << ", ";
    return inputs[minimal];
}

/* Probing security */
std::vector<Node>
Silver::check_Probing(Circuit &model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel, int verbose, int timeout)
{
    LACE_ME;

    std::vector<Node> positions = (robustModel) ? get_nodes_of_types(model, rprobes) : get_nodes_of_types(model, sprobes);

    int minimal = get_minimal_sharing(inputs);

    if (probingOrder == 0 || inputs[minimal].size() < 2 || inputs[minimal].size() < probingOrder) return inputs[minimal];

    std::vector<Bdd> secrets(1 << inputs.size());
    for (int index = 0; index < inputs.size(); index++) {
        secrets[index] = model[inputs[index][0]].getFunction();
        for (int elem = 1; elem < inputs[index].size(); elem++) secrets[index] ^= model[inputs[index][elem]].getFunction();
    }

    for (int comb = inputs.size() + 1; comb < (1 << inputs.size()); comb++) {
        secrets[comb-1] = Bdd::bddOne();
        for (int elem = 0; elem < inputs.size(); elem++) if (comb & (1 << elem)) secrets[comb-1] &= secrets[elem];
    }

    int varcount = 0;
    for (auto node = vertices(model).first; node != vertices(model).second; node++)
        if (model[*node].getType() == "in" || model[*node].getType() == "ref") varcount++;
    start = std::chrono::high_resolution_clock::now();

    for (int order = 0; order < probingOrder; order++) {
        std::vector<Node> probes(order + 1);
        std::vector<bool> bitmask(positions.size()); std::fill(bitmask.begin(), bitmask.begin() + (order + 1), true);
        do {
            int probe = 0; for (int idx = 0; idx < bitmask.size(); idx++) if (bitmask[idx]) probes[probe++] = positions[idx];

            if (robustModel) {
                Bdd observation = model[probes[0]].getRegisters();
                for (int probe = 1; probe < probes.size(); probe++)
                {
                    observation &= model[probes[probe]].getRegisters();
                }
                std::vector<uint32_t> extended = BddSet(observation).toVector();
                //print observation set
                if (verbose == 2){
                    printf("now the probes are %d ", probes[0]);
                    for (int probe = 1; probe < probes.size(); probe++)
                    {
                        printf("%d ", probes[probe]);
                    }
                    printf("\n");
                    printf("the size of secrets is %d\n", secrets.size());

                    for (int i = 0; i < extended.size(); i++){
                        printf("%d\n", extended[i]);
                    }
                    printf("the size of extended is %d\n", extended.size());
                    printf("so comb is of size %d\n", 1 << extended.size());

                }

                for (int comb = 1; comb < (1 << extended.size()); comb++) {
                    if (comb % 1000 == 0) 
                    {
                        if (elapsedTime() > timeout*60) {
                            printf("Time out!");
                            return probes;
                        }
                        if (verbose == 2) {
                            INFO("");
                            printf("\tcomb = %d\n", comb);
                        }
                    }
                    observation = sylvan::sylvan_true;
                    for (int elem = 0; elem < extended.size(); elem++){
                        if (comb & (1 << elem)) {
                            observation &= model[extended[elem]].getFunction();
                        }
                    }

                    bool independent = true;
                    for (int idx = 0; idx < secrets.size() && independent; idx++) {

                        independent &= CALL(mtbdd_statindependence, observation.GetBDD(), varcount, secrets[idx].GetBDD(), varcount);

                    }
                    //for (int idx = 0; idx < secrets.size(); idx++) independent &= SYNC(mtbdd_statindependence);
                    if (!independent) {
                        if (verbose == 2) printf("independent: comb = %d \n", comb);
                        return probes;
                    }
                }
                if (verbose == 2) printf("\n");
            } else {
                Bdd observation = model[probes[0]].getFunction();
                for (int probe = 1; probe < probes.size(); probe++)
                    observation &= model[probes[probe]].getFunction();

                bool independent = true;
                for (int idx = 0; idx < secrets.size() && independent; idx++) independent &= CALL(mtbdd_statindependence, observation.GetBDD(), varcount, secrets[idx].GetBDD(), varcount);
                if (!independent) return probes;
            }
        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }
    return inputs[minimal];
}

bool is_ind(Circuit &model, std::vector<uint32_t> extended, std::vector<uint32_t>::iterator it)
{
    std::set<uint32_t>* other_randoms = new std::set<uint32_t>();
    std::set<uint32_t>* result = new std::set<uint32_t>();
    for (std::vector<uint32_t>::iterator i = extended.begin(); i != extended.end(); ++i)
    {
        if (*i == *it) continue;
        std::set_union(other_randoms->begin(), other_randoms->end(), model[*i].getSupportV()->begin(),model[*i].getSupportV()->end(), inserter(*result, result->begin()));
        other_randoms->clear();
        std::copy(result->begin(),result->end(), inserter(*other_randoms, other_randoms->begin()));
        result->clear();
    }
    std::set<uint32_t>* unique_randoms = new std::set<uint32_t>();
    std::set_difference(model[*it].getPerfectM()->begin(),model[*it].getPerfectM()->end(), other_randoms->begin(), other_randoms->end(), inserter(*unique_randoms, unique_randoms->begin()));

    if (unique_randoms->size() > 0) return true;
    else return false;
}


std::vector<uint32_t> simplify_ExtendedProbes(Circuit &model, std::vector<uint32_t> &extended){
    bool can_be_simplified = false;
    for(int i = 0; i < extended.size(); i++){
        if (model[extended[i]].getPerfectM()->size() != 0)
        {
            can_be_simplified = true;
            break;
        }
    }
    if (can_be_simplified == false) return extended;
    for (std::vector<uint32_t>::iterator i = extended.begin(); i != extended.end(); ++i)
    {
        if (is_ind(model, extended, i)){
            extended.erase(i);
            return simplify_ExtendedProbes(model, extended);
        }
    }
    return extended;
}

/* Prover: Probing security */
std::vector<Node>
Silver::reduce_Probing(Circuit &model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel, int verbose, int timeout, bool onlygp, bool useSubset)
{
    LACE_ME;

    std::vector<long unsigned int> positions = (robustModel) ? get_nodes_of_types(model, rprobes) : get_nodes_of_types(model, sprobes);
    if (useSubset && robustModel)
        std::sort(positions.begin(), positions.end(), [&model,positions](Node n1, Node n2) ->bool
        {
            int len1 = BddSet(model[n1].getRegisters()).toVector().size();
            int len2 = BddSet(model[n2].getRegisters()).toVector().size();
            if (len1 > len2)
                return true;
            return false;
        });
    if (verbose == 2) {
        for (int i = 0; i < positions.size(); ++i)
        {
            printf("%d ", positions[i]);
        }
        printf("\n");
    }

    int minimal = get_minimal_sharing(inputs);
    if (probingOrder == 0 || inputs[minimal].size() < 2 || inputs[minimal].size() < probingOrder) 
        return inputs[minimal];
    int num_share = inputs[minimal].size();
    std::map<uint32_t, uint32_t> ivarToSec_indexMap;
    std::vector<Bdd> secrets(inputs.size());
    std::vector<Bdd> secrets_comb(1 << inputs.size());
    for (int index = 0; index < inputs.size(); index++) {
        secrets[index] = model[inputs[index][0]].getFunction();
        for (int elem = 1; elem < inputs[index].size(); elem++) 
            secrets[index] ^= model[inputs[index][elem]].getFunction();
    }
    //int curr_secret_id = inputs.size(); //secrets.size();
    int curr_secret_id = 0;
    int varcount = 0;
    for (auto node = vertices(model).first; node != vertices(model).second; node++)
        if (model[*node].getType() == "in" || model[*node].getType() == "ref") varcount++;
    
    std::set<uint32_t> inputs_set;
    for (int i = 0; i < inputs.size(); i++){
        inputs_set.insert(i);
    }
    std::set<uint32_t> ivar;
    std::set<uint32_t> nivar;
    std::set<uint32_t>::iterator pos;
    start = std::chrono::high_resolution_clock::now();

    std::vector<std::set<uint32_t>> merged_obs;
    int num_subset = 0;
    long total_combinations = 0;
    long number_rule = 0;
    for (int order = 0; order < probingOrder; order++) {
        std::vector<Node> probes(order + 1);
        std::vector<bool> bitmask(positions.size()); 
        std::fill(bitmask.begin(), bitmask.begin() + (order + 1), true);
        do {
            int probe = 0;  
            for (int idx = 0; idx < bitmask.size(); idx++) 
                if (bitmask[idx]) 
                    probes[probe++] = positions[idx];
            Bdd observation; 
            std::vector<uint32_t> extended_o;
            total_combinations++;
            if (robustModel) {
                observation = model[probes[0]].getRegisters();
                for (int probe = 1; probe < probes.size(); probe++)
                {
                    observation &= model[probes[probe]].getRegisters();
                }
                extended_o = BddSet(observation).toVector();
            }
            else {
                //observation = model[probes[0]].getFunction();
                for (int probe = 0; probe < probes.size(); probe++)
                    extended_o.push_back(probes[probe]);
                    //observation &= model[probes[probe]].getFunction();
                //bool independent = true;
                //for (int idx = 0; idx < secrets.size() && independent; idx++) independent &= CALL(mtbdd_statindependence, observation.GetBDD(), varcount, secrets[idx].GetBDD(), varcount);
                //if (!independent) return probes;
            }

            //std::vector<uint32_t> extended_o = BddSet(observation).toVector();
            if (verbose == 2){

                INFO("now the");
                printf(" probes are %d ", probes[0]);
                for (int probe = 1; probe < probes.size(); probe++)
                {
                    printf("%d ", probes[probe]);
                }
                printf("\n");
                printf("the size of secrets is %d\n", secrets.size());

                std::cout << "before simplify_ExtendedProbes: " << extended_o.size() << "\n";
                for (int i = 0; i < extended_o.size(); i++){
                    printf("%d: ", extended_o[i]);
                    printIntSet("\n\t perfectM", model[extended_o[i]].getPerfectM());
                    printIntSet("\t supportV", model[extended_o[i]].getSupportV());

                }
            }
            // Reduction rule 1: Uniqueness Rule
            int len_before = extended_o.size();
            std::vector<uint32_t> extended = simplify_ExtendedProbes(model, extended_o);
            std::set<uint32_t> set_of_extended;
            if (verbose == 2)std::cout << "atfer simplify_ExtendedProbes: " << extended.size() << "\n";
            if (0 == extended.size()) {
                if (verbose == 2) std::cout << "This set is reduced to empty set using reductaion rule, continue" << std::endl;
                number_rule++;
                continue;
            }
            if (!robustModel && (extended.size()!=len_before)) {
                continue;
            }
            for (int i = 0; i < extended.size(); i++){
                if (verbose == 2){
                    printf("%d: ", extended[i]);
                    printIntSet("\n\t perfectM", model[extended[i]].getPerfectM());
                    printIntSet("\t supportV", model[extended[i]].getSupportV());
                }
                set_of_extended.insert(extended[i]);
            }
                
            // Subset strategy
            if(useSubset && robustModel)
            {
                int find_subset = 0;
                if (verbose == 2)printf("the size of extended is %d\n", extended.size());
                if (std::find(merged_obs.begin(), merged_obs.end(), set_of_extended) != merged_obs.end()) {
                    if (verbose == 2)printf("this set has already been checked, we skip it to continue\n");
                    continue;
                }
                else
                {
                    for (int i = 0; i < merged_obs.size(); ++i)
                    {
                        std::set<uint32_t> diff;
                        std::set_difference(set_of_extended.begin(), set_of_extended.end(), merged_obs[i].begin(), merged_obs[i].end(), inserter(diff, diff.begin()));
                        if (diff.size() == 0)
                        {
                            num_subset++;
                            find_subset = 1;
                            if (verbose == 2)printf("find %d-th subset, skip it and continue\n", num_subset);
                            break;
                        }

                    }
                    if (find_subset == 1)
                    {
                        continue;
                    }
                    else {
                        merged_obs.push_back(set_of_extended);
                    }
                }
            }
            if (verbose == 2)printf("so comb is of size %d\n", 1 << extended.size());

            //observation = sylvan::sylvan_true;
            //for (int elem = 0; elem < extended.size(); elem++)
            //    observation &= model[extended[elem]].getFunction();
                
            // Reduction rule 2: NI Rule
            ivar.clear();
            nivar.clear();
            std::vector<std::vector<Node>> shares(inputs.size());
            std::set<uint32_t> vars;
            for (int i = 0; i < extended.size(); i++) {
                vars.insert(model[ extended[i]].getNISupportV()->begin(), model[extended[i]].getNISupportV()->end());
            }
            std::vector<uint32_t> variables(vars.begin(),vars.end());// = BddSet(observation.Support()).toVector();
            for (int var = variables.size() - 1; var >= 0; var--) {
                for (int idx = inputs.size() - 1; idx >= 0; idx--) {
                    if (std::find(inputs[idx].begin(), inputs[idx].end(), variables[var]) != inputs[idx].end()) {
                        shares[idx].push_back(variables[var]); 
                        if (shares[idx].size() == num_share){
                            ivar.insert(idx);
                        }
                    }
                }
            }
            // nivar = 
            std::set_difference(inputs_set.begin(), inputs_set.end(), ivar.begin(), ivar.end(), std::inserter(nivar, nivar.begin()));
            if (ivar.size() == 0) {
                if (verbose == 2)printf("the set is ni, skip it and continue\n");
                continue;
            }
            // print ivar and nivar
            if (verbose == 2) {
                std::cout << "the size of interference variables is " << ivar.size() << "\n";
                std::cout << "i vars are: ";
                for (pos = ivar.begin(); pos != ivar.end(); pos++) {
                    std::cout << *pos << " ";
                }
                printf("\n");
                for (pos = nivar.begin(); pos != nivar.end(); pos++) {
                    std::cout << *pos << " ";
                }
                printf("\n");

            }
            std::vector<uint32_t> ivar_comb;
            for (int comb = 1; comb < (1 << ivar.size()); comb++){
                int ivar_comb_elem = 0;
                pos = ivar.begin();
                for (int elem = 0; elem < ivar.size(); elem++, pos++){
                    if (comb & (1 << elem)){
                        ivar_comb_elem ^= (1 << *pos);
                    }
                }
                std::vector<uint32_t> ivar_vec(ivar.begin(), ivar.end());
                if (ivarToSec_indexMap.count(ivar_comb_elem) == 0){
                    secrets_comb[curr_secret_id] = Bdd::bddOne();
                    for (int elem = 0; elem < ivar.size(); elem++, pos++){
                        if (comb & (1 << elem)){
                            secrets_comb[curr_secret_id] &= secrets[ivar_vec[elem]];
                        }
                    }
                    ivarToSec_indexMap.insert({ ivar_comb_elem, curr_secret_id++});
                }
                ivar_comb.push_back(ivar_comb_elem);
            }
            if (verbose == 2)printf("\n");
            for (int comb = (1 << extended.size()) - 1; comb > 0; comb--) {
                if (comb % 1000 == 0)
                {
                    if (elapsedTime() > timeout*60) {
                        if (verbose == 0) printf("%ld,%ld,%ld,", total_combinations, num_subset, number_rule);
                        printf("Time out!");
                        return probes;
                    }
                    if (verbose == 2) {
                        INFO("");
                        printf("\tcomb = %d\n", comb);
                    }
                }
                observation = sylvan::sylvan_true;
                for (int elem = extended.size() - 1; elem >=0 ; elem--)
                    if (comb & (1 << elem)) {
                        observation &= model[extended[elem]].getFunction();
                    }
                bool independent = true;
                for (int idx = 0; idx < ivar_comb.size() && independent; idx++) {
                    independent &= CALL(mtbdd_statindependence, observation.GetBDD(), varcount, secrets_comb[ivarToSec_indexMap[ivar_comb[idx]]].GetBDD(), varcount);
                    if(!independent && verbose == 2){
                        printf("idx = %d\n", ivar_comb[idx]);
                        printf("curr_secret_id = %d\n", ivarToSec_indexMap[ivar_comb[idx]]);
                    }
                }
                if (!independent) {
                    if (verbose == 2) printf("independent: comb = %d \n", comb);
                    if (verbose == 0) printf("%ld,%ld,%ld,", total_combinations, num_subset, number_rule);
                    return probes;
                }
                if (!robustModel) break;
            }
            if (verbose == 2)printf("\n");
        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }

    if(verbose == 0) printf("%ld,%ld,%ld,", total_combinations, num_subset, number_rule);
    return inputs[minimal];
}


/* Non-interference */
std::vector<Node>
Silver::check_NI(Circuit &model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel)
{
    LACE_ME;

    std::vector<Node> positions = (robustModel) ? get_nodes_of_types(model, rprobes) : get_nodes_of_types(model, sprobes);

    int minimal = get_minimal_sharing(inputs);

    if (probingOrder == 0 || inputs[minimal].size() < 2 || inputs[minimal].size() < probingOrder) return inputs[minimal];

    Bdd variables = Bdd::bddOne();
    for (int index = 0; index < inputs.size(); index++)
        for (int elem = 0; elem < inputs[index].size(); elem++) variables &= model[inputs[index][elem]].getFunction();
    std::vector<uint32_t> secvar = BddSet(variables).toVector();

    int varcount = 0;
    for (auto node = vertices(model).first; node != vertices(model).second; node++)
        if (model[*node].getType() == "in" || model[*node].getType() == "ref") varcount++;

    for (int order = 1; order <= probingOrder; order++) {
        std::vector<Node> probes(order);
        std::vector<bool> probemask(positions.size()); std::fill(probemask.begin(), probemask.begin() + order, true);
        do {
            int probe = 0; for (int idx = 0; idx < probemask.size(); idx++) if (probemask[idx]) probes[probe++] = positions[idx];

            if (robustModel) {
                Bdd observation = model[probes[0]].getRegisters();
                for (int probe = 1; probe < probes.size(); probe++)
                    observation &= model[probes[probe]].getRegisters();

                std::vector<uint32_t> extended = BddSet(observation).toVector();

                for (int comb = 1; comb < (1 << extended.size()); comb++) {
                    observation = sylvan::sylvan_true;
                    for (int elem = 0; elem < extended.size(); elem++)
                        if (comb & (1 << elem)) observation &= model[extended[elem]].getFunction();

                    bool trivial_solution = true;
                    std::vector<std::vector<Node>> shares(inputs.size());
                    std::vector<uint32_t> variables = BddSet(observation.Support()).toVector();
                    for (int var = variables.size() - 1; var >= 0; var--) {
                        for (int idx = inputs.size() - 1; idx >= 0; idx--) {
                            if (std::find(inputs[idx].begin(), inputs[idx].end(), variables[var]) != inputs[idx].end()) {
                                shares[idx].push_back(variables[var]); trivial_solution &= (shares[idx].size() <= order); break;
                            }
                        }
                    }

                    if (!trivial_solution) {
                        std::vector<std::vector<Bdd>> intra(shares.size());
                        for (int idx = 0; idx < shares.size(); idx++) {
                            for (int comb = 0; comb < (1 << shares[idx].size()); comb++) {
                                if (__builtin_popcount(comb) <= order) {
                                    intra[idx].push_back(sylvan::sylvan_true);
                                    for (int elem = 0; elem < shares[idx].size(); elem++)
                                        if (comb & (1 << elem)) intra[idx][intra[idx].size() - 1] &= model[shares[idx][elem]].getFunction();
                                }
                            }
                        }

                        std::vector<Bdd> inter; inter_vector_combinations_and(intra, 0, Bdd::bddOne(), inter);

                        bool independent = false;

                        for (int idx = 0; idx < inter.size() && !independent; idx++) {
                            independent = true;
                            std::vector<uint32_t> combination = BddSet(inter[idx]).toVector(), complement;

                            for (int elem = 0; elem < secvar.size(); elem++)
                                if (std::find(combination.begin(), combination.end(), secvar[elem]) == combination.end())
                                    complement.push_back(secvar[elem]);

                            for (int s = 0; s < (1 << combination.size()) && independent; s++) {
                                Bdd simulate = observation;
                                for (int elem = 0; elem < combination.size(); elem++) if (s & (1 << elem)) simulate &= model[combination[elem]].getFunction();

                                for (int r = 1; r < (1 << complement.size()) && independent; r++) {
                                    Bdd free  = sylvan::sylvan_true;
                                    for (int elem = 0; elem < complement.size(); elem++) if (r & (1 << elem)) free &= model[complement[elem]].getFunction();

                                    independent &= CALL(mtbdd_statindependence, simulate.GetBDD(), varcount, free.GetBDD(), varcount);
                                }
                            }
                        }

                        if (!independent) return probes;
                    }
                }
            } else {
                /*
                 * observation is a Bdd
                 */
                Bdd observation = model[probes[0]].getFunction();
                for (int probe = 1; probe < probes.size(); probe++) observation &= model[probes[probe]].getFunction();

                bool trivial_solution = true;
                /*
                 * shares are the vector of node vector, the element node vector contains the shared 
                 * variables of the i-th secret
                 * variables are the ids of Bdd id
                 */
                std::vector<std::vector<Node>> shares(inputs.size());
                std::vector<uint32_t> variables = BddSet(observation.Support()).toVector();
                for (int var = variables.size() - 1; var >= 0; var--) {
                    for (int idx = inputs.size() - 1; idx >= 0; idx--) {
                        if (std::find(inputs[idx].begin(), inputs[idx].end(), variables[var]) != inputs[idx].end()) {
                            shares[idx].push_back(variables[var]); trivial_solution &= (shares[idx].size() <= order); break;
                        }
                    }
                }

                if (!trivial_solution) {
                    std::vector<std::vector<Bdd>> intra(shares.size());
                    for (int idx = 0; idx < shares.size(); idx++) {
                        for (int comb = 0; comb < (1 << shares[idx].size()); comb++) {
                            if (__builtin_popcount(comb) <= order) {
                                intra[idx].push_back(sylvan::sylvan_true);
                                for (int elem = 0; elem < shares[idx].size(); elem++)
                                    if (comb & (1 << elem)) intra[idx][intra[idx].size() - 1] &= model[shares[idx][elem]].getFunction();
                            }
                        }
                    }

                    std::vector<Bdd> inter; inter_vector_combinations_and(intra, 0, Bdd::bddOne(), inter);

                    bool independent = false;

                    for (int idx = 0; idx < inter.size() && !independent; idx++) {
                        independent = true;
                        std::vector<uint32_t> combination = BddSet(inter[idx]).toVector(), complement;

                        for (int elem = 0; elem < secvar.size(); elem++)
                            if (std::find(combination.begin(), combination.end(), secvar[elem]) == combination.end())
                                complement.push_back(secvar[elem]);

                        for (int s = 0; s < (1 << combination.size()) && independent; s++) {
                            Bdd simulate = observation;
                            for (int elem = 0; elem < combination.size(); elem++) if (s & (1 << elem)) simulate &= model[combination[elem]].getFunction();

                            for (int r = 1; r < (1 << complement.size()) && independent; r++) {
                                Bdd free  = sylvan::sylvan_true;
                                for (int elem = 0; elem < complement.size(); elem++) if (r & (1 << elem)) free &= model[complement[elem]].getFunction();

                                independent &= CALL(mtbdd_statindependence, simulate.GetBDD(), varcount, free.GetBDD(), varcount);
                            }
                        }
                    }

                    if (!independent) return probes;
                }
            }
        } while (std::prev_permutation(probemask.begin(), probemask.end()));
    }

    return inputs[minimal];
}

/* Strong Non-interference */
std::vector<Node>
Silver::check_SNI(Circuit &model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel)
{
    LACE_ME;

    std::vector<Node> positions = (robustModel) ? get_nodes_of_types(model, rprobes) : get_nodes_of_types(model, sprobes);

    for (auto gate = vertices(model).first; gate != vertices(model).second; gate++)
        if (!robustModel && model[*gate].getType() == "out") positions.push_back(*gate);

    int minimal = get_minimal_sharing(inputs);

    if (probingOrder == 0 || inputs[minimal].size() < 2 || inputs[minimal].size() < probingOrder) return inputs[minimal];

    Bdd variables = Bdd::bddOne();
    for (int index = 0; index < inputs.size(); index++)
        for (int elem = 0; elem < inputs[index].size(); elem++) variables &= model[inputs[index][elem]].getFunction();
    std::vector<uint32_t> secvar = BddSet(variables).toVector();

    int varcount = 0;
    for (auto node = vertices(model).first; node != vertices(model).second; node++)
        if (model[*node].getType() == "in" || model[*node].getType() == "ref") varcount++;

    for (int order = 1; order <= probingOrder; order++) {

        std::vector<Node> probes(order);
        std::vector<bool> probemask(positions.size()); std::fill(probemask.begin(), probemask.begin() + order, true);
        do {
            int probe = 0; for (int idx = 0; idx < probemask.size(); idx++) if (probemask[idx]) probes[probe++] = positions[idx];

            int outputs = 0; for (int probe = 0; probe < probes.size(); probe++) if(model[probes[probe]].getType() == "out") outputs++;

            if (robustModel) {
                Bdd observation = model[probes[0]].getRegisters();
                for (int probe = 1; probe < probes.size(); probe++)
                    observation &= model[probes[probe]].getRegisters();

                std::vector<uint32_t> extended = BddSet(observation).toVector();

                for (int comb = 1; comb < (1 << extended.size()); comb++) {
                    observation = sylvan::sylvan_true;
                    for (int elem = 0; elem < extended.size(); elem++)
                        if (comb & (1 << elem)) observation &= model[extended[elem]].getFunction();

                    bool trivial_solution = true;
                    std::vector<std::vector<Node>> shares(inputs.size());
                    std::vector<uint32_t> variables = BddSet(observation.Support()).toVector();
                    for (int var = variables.size() - 1; var >= 0; var--) {
                        for (int idx = inputs.size() - 1; idx >= 0; idx--) {
                            if (std::find(inputs[idx].begin(), inputs[idx].end(), variables[var]) != inputs[idx].end()) {
                                shares[idx].push_back(variables[var]); trivial_solution &= (shares[idx].size() <= (order - outputs)); break;
                            }
                        }
                    }

                    if (!trivial_solution) {
                        std::vector<std::vector<Bdd>> intra(shares.size());
                        for (int idx = 0; idx < shares.size(); idx++) {
                            for (int comb = 0; comb < (1 << shares[idx].size()); comb++) {
                                if (__builtin_popcount(comb) <= (order - outputs)) {
                                    intra[idx].push_back(sylvan::sylvan_true);
                                    for (int elem = 0; elem < shares[idx].size(); elem++)
                                        if (comb & (1 << elem)) intra[idx][intra[idx].size() - 1] &= model[shares[idx][elem]].getFunction();
                                }
                            }
                        }

                        std::vector<Bdd> inter; inter_vector_combinations_and(intra, 0, Bdd::bddOne(), inter);

                        bool independent = false;

                        for (int idx = 0; idx < inter.size() && !independent; idx++) {
                            std::vector<uint32_t> combination = BddSet(inter[idx]).toVector(), complement; independent = true;

                            for (int elem = 0; elem < secvar.size(); elem++)
                                if (std::find(combination.begin(), combination.end(), secvar[elem]) == combination.end())
                                    complement.push_back(secvar[elem]);

                            for (int s = 0; s < (1 << combination.size()) && independent; s++) {
                                Bdd simulate = observation;
                                for (int elem = 0; elem < combination.size(); elem++) if (s & (1 << elem)) simulate &= model[combination[elem]].getFunction();

                                for (int r = 1; r < (1 << complement.size()) && independent; r++) {
                                    Bdd free  = sylvan::sylvan_true;
                                    for (int elem = 0; elem < complement.size(); elem++) if (r & (1 << elem)) free &= model[complement[elem]].getFunction();

                                    independent  &= CALL(mtbdd_statindependence, simulate.GetBDD(), varcount, free.GetBDD(), varcount);
                                }
                            }
                        }

                        if (!independent) return probes;
                    }
                }
            } else {
                Bdd observation = model[probes[0]].getFunction();
                for (int probe = 1; probe < probes.size(); probe++) observation &= model[probes[probe]].getFunction();

                bool trivial_solution = true;
                std::vector<std::vector<Node>> shares(inputs.size());
                std::vector<uint32_t> variables = BddSet(observation.Support()).toVector();
                for (int var = variables.size() - 1; var >= 0; var--) {
                    for (int idx = inputs.size() - 1; idx >= 0; idx--) {
                        if (std::find(inputs[idx].begin(), inputs[idx].end(), variables[var]) != inputs[idx].end()) {
                            shares[idx].push_back(variables[var]); trivial_solution &= (shares[idx].size() <= (order - outputs)); break;
                        }
                    }
                }

                if (!trivial_solution) {
                    std::vector<std::vector<Bdd>> intra(shares.size());
                    for (int idx = 0; idx < shares.size(); idx++) {
                        for (int comb = 0; comb < (1 << shares[idx].size()); comb++) {
                            if (__builtin_popcount(comb) <= (order - outputs)) {
                                intra[idx].push_back(sylvan::sylvan_true);
                                for (int elem = 0; elem < shares[idx].size(); elem++)
                                    if (comb & (1 << elem)) intra[idx][intra[idx].size() - 1] &= model[shares[idx][elem]].getFunction();
                            }
                        }
                    }

                    std::vector<Bdd> inter; inter_vector_combinations_and(intra, 0, Bdd::bddOne(), inter);

                    bool independent = false;

                    for (int idx = 0; idx < inter.size() && !independent; idx++) {
                        std::vector<uint32_t> combination = BddSet(inter[idx]).toVector(), complement; independent = true;

                        for (int elem = 0; elem < secvar.size(); elem++)
                            if (std::find(combination.begin(), combination.end(), secvar[elem]) == combination.end())
                                complement.push_back(secvar[elem]);

                        for (int s = 0; s < (1 << combination.size()) && independent; s++) {
                            Bdd simulate = observation;
                            for (int elem = 0; elem < combination.size(); elem++) if (s & (1 << elem)) simulate &= model[combination[elem]].getFunction();

                            for (int r = 1; r < (1 << complement.size()) && independent; r++) {
                                Bdd free  = sylvan::sylvan_true;
                                for (int elem = 0; elem < complement.size(); elem++) if (r & (1 << elem)) free &= model[complement[elem]].getFunction();

                                independent  &= CALL(mtbdd_statindependence, simulate.GetBDD(), varcount, free.GetBDD(), varcount);
                            }
                        }
                    }

                    if (!independent) return probes;
                }
            }
        } while (std::prev_permutation(probemask.begin(), probemask.end()));
    }

    return inputs[minimal];
}

/* Probe-Isolating Non-interference */
std::vector<Node>
Silver::check_PINI(Circuit &model, std::map<int, Probes> inputs, const int probingOrder, const bool robustModel)
{
    LACE_ME;

    std::vector<Node> positions = (robustModel) ? get_nodes_of_types(model, rprobes) : get_nodes_of_types(model, sprobes);

    for (auto gate = vertices(model).first; gate != vertices(model).second; gate++)
        if (!robustModel && model[*gate].getType() == "out") positions.push_back(*gate);

    int minimal = get_minimal_sharing(inputs);

    if (probingOrder == 0 || inputs[minimal].size() < 2 || inputs[minimal].size() < probingOrder) return inputs[minimal];

    Bdd variables = Bdd::bddOne();
    for (int index = 0; index < inputs.size(); index++)
        for (int elem = 0; elem < inputs[index].size(); elem++) variables &= model[inputs[index][elem]].getFunction();
    std::vector<uint32_t> secvar = BddSet(variables).toVector();

    int varcount = 0;
    for (auto node = vertices(model).first; node != vertices(model).second; node++)
        if (model[*node].getType() == "in" || model[*node].getType() == "ref") varcount++;

    for (int order = 1; order <= probingOrder; order++) {
        std::vector<Node> probes(order);
        std::vector<bool> probemask(positions.size()); std::fill(probemask.begin(), probemask.begin() + order, true);
        do {
            int probe = 0; for (int idx = 0; idx < probemask.size(); idx++) if (probemask[idx]) probes[probe++] = positions[idx];

            int outputs = 0; for (int probe = 0; probe < probes.size(); probe++) if(model[probes[probe]].getType() == "out") outputs++;

            std::set<int> odomains;
            for (int probe = 0; probe < probes.size(); probe++)
                if(model[probes[probe]].getType() == "out") odomains.insert(model[probes[probe]].getSharing().second);

            if (robustModel) {
                Bdd observation = model[probes[0]].getRegisters();
                for (int probe = 1; probe < probes.size(); probe++)
                    observation &= model[probes[probe]].getRegisters();

                std::vector<uint32_t> extended = BddSet(observation).toVector();
                std::cout << "extended:";
                for (auto e: extended){
                    std::cout << e << " ";
                }
                std::cout << std::endl;
                for (int comb = 1; comb < (1 << extended.size()); comb++) {
                    observation = sylvan::sylvan_true;
                    for (int elem = 0; elem < extended.size(); elem++)
                        if (comb & (1 << elem)) observation &= model[extended[elem]].getFunction();

                    std::vector<std::vector<Node>> shares(inputs.size());
                    std::vector<uint32_t> variables = BddSet(observation.Support()).toVector();
                    for (int var = variables.size() - 1; var >= 0; var--) {
                        for (int idx = inputs.size() - 1; idx >= 0; idx--) {
                            if (std::find(inputs[idx].begin(), inputs[idx].end(), variables[var]) != inputs[idx].end()) {
                                shares[idx].push_back(variables[var]); break;
                            }
                        }
                    }

                    std::set<int> domains;
                    for (int idx = 0; idx < shares.size(); idx++)
                        for (int elem = 0; elem < shares[idx].size(); elem++) domains.insert(model[shares[idx][elem]].getSharing().second);
                    domains.insert(odomains.begin(), odomains.end());

                    if (domains.size() > order) {
                        std::vector<std::vector<Bdd>> intra(shares.size());
                        for (int idx = 0; idx < shares.size(); idx++) {
                            for (int comb = 0; comb < (1 << shares[idx].size()); comb++) {
                                if (__builtin_popcount(comb) <= order) {
                                    intra[idx].push_back(sylvan::sylvan_true);
                                    for (int elem = 0; elem < shares[idx].size(); elem++)
                                        if (comb & (1 << elem)) intra[idx][intra[idx].size() - 1] &= model[shares[idx][elem]].getFunction();
                                }
                            }
                        }

                        std::vector<Bdd> inter; inter_vector_combinations_and(intra, 0, Bdd::bddOne(), inter);
                        bool independent = false;

                        for (int idx = 0; idx < inter.size() && !independent; idx++) {
                            std::vector<uint32_t> combination = BddSet(inter[idx]).toVector(), complement;

                            domains.clear();
                            for (int idx = 0; idx < combination.size(); idx++)
                                for (int elem = 0; elem < combination.size(); elem++) domains.insert(model[combination[elem]].getSharing().second);
                            domains.insert(odomains.begin(), odomains.end());

                            if (domains.size() <= order) {
                                independent = true;

                                for (int elem = 0; elem < secvar.size(); elem++)
                                    if (std::find(combination.begin(), combination.end(), secvar[elem]) == combination.end())
                                        complement.push_back(secvar[elem]);

                                for (int s = 0; s < (1 << combination.size()) && independent; s++) {
                                    Bdd simulate = observation;
                                    for (int elem = 0; elem < combination.size(); elem++) if (s & (1 << elem)) simulate &= model[combination[elem]].getFunction();

                                    for (int r = 1; r < (1 << complement.size()) && independent; r++) {
                                        Bdd free  = sylvan::sylvan_true;
                                        for (int elem = 0; elem < complement.size(); elem++) if (r & (1 << elem)) free &= model[complement[elem]].getFunction();

                                        independent  &= CALL(mtbdd_statindependence, simulate.GetBDD(), varcount, free.GetBDD(), varcount);
                                    }
                                }
                            }
                            if (!independent) continue;
                            for (auto c: combination){
                                std::cout << c << ", ";
                            }
                            std::cout << std::endl;
                        }

                        if (!independent) return probes;
                    }
                }
            } else {
                Bdd observation  = model[probes[0]].getFunction();
                for (int probe = 1; probe < probes.size(); probe++) observation &= model[probes[probe]].getFunction();

                std::vector<std::vector<Node>> shares(inputs.size());
                std::vector<uint32_t> variables = BddSet(observation.Support()).toVector();
                for (int var = variables.size() - 1; var >= 0; var--) {
                    for (int idx = inputs.size() - 1; idx >= 0; idx--) {
                        if (std::find(inputs[idx].begin(), inputs[idx].end(), variables[var]) != inputs[idx].end()) {
                            shares[idx].push_back(variables[var]); break;
                        }
                    }
                }

                std::set<int> domains;
                for (int idx = 0; idx < shares.size(); idx++)
                    for (int elem = 0; elem < shares[idx].size(); elem++) domains.insert(model[shares[idx][elem]].getSharing().second);
                domains.insert(odomains.begin(), odomains.end());

                if (domains.size() > order) {
                    std::vector<std::vector<Bdd>> intra(shares.size());
                    for (int idx = 0; idx < shares.size(); idx++) {
                        for (int comb = 0; comb < (1 << shares[idx].size()); comb++) {
                            if (__builtin_popcount(comb) <= order) {
                                intra[idx].push_back(sylvan::sylvan_true);
                                for (int elem = 0; elem < shares[idx].size(); elem++)
                                    if (comb & (1 << elem)) intra[idx][intra[idx].size() - 1] &= model[shares[idx][elem]].getFunction();
                            }
                        }
                    }

                    std::vector<Bdd> inter; inter_vector_combinations_and(intra, 0, Bdd::bddOne(), inter);

                    bool independent = false;

                    for (int idx = 0; idx < inter.size() && !independent; idx++) {
                        std::vector<uint32_t> combination = BddSet(inter[idx]).toVector(), complement;

                        domains.clear();
                        for (int idx = 0; idx < combination.size(); idx++)
                            for (int elem = 0; elem < combination.size(); elem++) domains.insert(model[combination[elem]].getSharing().second);
                        domains.insert(odomains.begin(), odomains.end());

                        if (domains.size() <= order) {
                            independent = true;

                            for (int elem = 0; elem < secvar.size(); elem++)
                                if (std::find(combination.begin(), combination.end(), secvar[elem]) == combination.end())
                                    complement.push_back(secvar[elem]);

                            for (int s = 0; s < (1 << combination.size()) && independent; s++) {
                                Bdd simulate = observation;
                                for (int elem = 0; elem < combination.size(); elem++) if (s & (1 << elem)) simulate &= model[combination[elem]].getFunction();

                                for (int r = 1; r < (1 << complement.size()) && independent; r++) {
                                    Bdd free  = sylvan::sylvan_true;
                                    for (int elem = 0; elem < complement.size(); elem++) if (r & (1 << elem)) free &= model[complement[elem]].getFunction();

                                    independent  &= CALL(mtbdd_statindependence, simulate.GetBDD(), varcount, free.GetBDD(), varcount);
                                }
                            }
                        }
                    }

                    if (!independent) return probes;
                }
            }

        } while (std::prev_permutation(probemask.begin(), probemask.end()));
    }

    return inputs[minimal];
}

/* Internal helper functions */
std::vector<Node>
Silver::get_nodes_of_types(Circuit &model, const std::set<std::string> types)
{
    std::vector<Node> selection;

    for (auto gate = vertices(model).first; gate != vertices(model).second; gate++)
        if (types.find(model[*gate].getType()) != types.end())
            selection.push_back(*gate);

    return selection;
}

int
Silver::get_minimal_sharing(std::map<int, Probes> inputs)
{
    int minimal = 0;

    for (int index = 0; index < inputs.size(); index++)
        minimal = (inputs[index].size() < inputs[minimal].size()) ? index : minimal;

    return minimal;
}

void
Silver::print_node_vector(const Circuit& model, const std::vector<Node> nodes)
{
    std::cout << "\"";
    for (int n = 0; n < nodes.size(); n++) {
        std::cout << nodes[n] + 1;
        if (n != nodes.size() - 1) std::cout << ",";
    }
    std::cout << "\"\n";
}

/*e: configurable line ending*/
void
Silver::print_node_vector_e(const Circuit& model, const std::vector<Node> nodes, std::string ending) 
{
    std::cout << "\"";
    for (int n = 0; n < nodes.size(); n++) {
        std::cout << nodes[n] + 1;
        if (n != nodes.size() - 1) std::cout << ",";
    }
    std::cout << "\"" << ending;
}

void
Silver::inter_vector_combinations_and(const std::vector<std::vector<Bdd>> &intra, int offset, Bdd combination, std::vector<Bdd> &inter)
{
    if (offset < intra.size()) {
        for (int idx = 0; idx < intra[offset].size(); idx++) inter_vector_combinations_and(intra, offset + 1, combination & intra[offset][idx], inter);
    } else {
        inter.push_back(combination);
    }
}

bool
Silver::inter_vector_combinations_xor(const std::vector<std::vector<Bdd>> &intra, int offset, Bdd combination, int varcount, std::chrono::time_point<std::chrono::high_resolution_clock> begin, int timeout)
{
    LACE_ME;

    if (offset < intra.size()) {
        bool balanced = true;
        for (int idx = 0; idx < intra[offset].size() && balanced; idx++)
            balanced &= inter_vector_combinations_xor(intra, offset + 1, combination ^ intra[offset][idx], varcount, begin, timeout);
        if (!balanced) return false;
    } else {
        if (elapsedTime(begin) > 60 * timeout) {
            printf("timeout");
            return false;
        }
        return (abs(mtbdd_satcountln(combination.GetBDD(), varcount) - varcount + 1) < DOUBLE_COMPARE_THRESHOLD);
    }

    return true;
}

bool
Silver::inter_vector_combinations_xor1(Circuit& model, const std::vector< std::vector<std::vector<uint32_t>>>& intra, int offset, std::vector<uint32_t> combination, int varcount, std::chrono::time_point<std::chrono::high_resolution_clock> begin, int timeout)
{
    LACE_ME;

    if (offset < intra.size()) {
        bool balanced = true;
        for (int idx = 0; idx < intra[offset].size() && balanced; idx++) {
            //hw += hammingWeight(idx+1);
            for (int i = 0; i < intra[offset][idx].size(); i++) {
                combination.push_back(intra[offset][idx][i]);
            }
            balanced &= inter_vector_combinations_xor1(model, intra, offset + 1, combination , varcount, begin, timeout);
            for (int i = 0; i < intra[offset][idx].size(); i++) {
                combination.pop_back();
            }
        }
        if (!balanced) return false;
    }
    else {
        if (elapsedTime(begin) > 60 * timeout) {
            printf("timeout");
            return false;
        }
        //printf("%d ", combination.size());
        std::vector<uint32_t> reduced = simplify_ExtendedProbes(model, combination);
        // printf("%d\n", reduced.size());
        if (reduced.size() == 0) {
            //printf("red\n");
            return true;
        }
        else {
            Bdd observation = sylvan::sylvan_true;
            int hw = 0;
            for (int elem = 0; elem < reduced.size(); elem++){
                observation &= model[reduced[elem]].getFunction();
                hw += 1;
            }
            //printf("robdd\n");
            return (abs(mtbdd_satcountln(observation.GetBDD(), varcount) - varcount + hw) < DOUBLE_COMPARE_THRESHOLD);
        }
    }

    return true;
}

// Calculate the Hamming Weight of an integer x
int Silver::hammingWeight(int x) {
    int count = 0;
    while (x != 0) {
        count += x & 1; // Add 1 to the count if the least significant bit of x is 1
        x >>= 1; // Right shift x to check the next bit
    }
    return count;
}

bool
Silver::inter_vector_combinations_and1(const std::vector<std::vector<Bdd>>& intra, int offset, Bdd combination, int varcount, int hw)
{
    LACE_ME;

    if (offset < intra.size()) {
        bool balanced = true;
        for (int idx = 0; idx < intra[offset].size() && balanced; idx++) {
            //hw += hammingWeight(idx+1);
            balanced &= inter_vector_combinations_and1(intra, offset + 1, combination & intra[offset][idx], varcount, hw + hammingWeight(idx + 1));
        }
        if (!balanced) return false;
    }
    else {
        double p = mtbdd_satcountln(combination.GetBDD(), varcount);

        return (abs(p - varcount + hw) < DOUBLE_COMPARE_THRESHOLD);
    }

    return true;
}
