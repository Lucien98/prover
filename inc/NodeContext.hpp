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

#ifndef __NODECONTEXT_HPP_
#define __NODECONTEXT_HPP_

#include <set>
#include <string>
#include <vector>

#include <sylvan_obj.hpp>


class NodeContext {

public:
    void printIntSet1(std::string info, std::set<uint32_t> *s){
        std::cout << info;
        std::copy(s->begin(),
                s->end(),
                std::ostream_iterator<int>(std::cout, " "));
        std::cout << std::endl;
    }

    void set_union(std::set<uint32_t>* setA, std::set<uint32_t>* setB, std::set<uint32_t>* result) {
        std::set_union(setA->begin(), setA->end(),
            setB->begin(), setB->end(),
            inserter(*result, result->begin())
        );
    }

    /**
     * @brief Return node type (gate functionality) stored in node context
     */
    std::string
    getType() const
    {
        return this->type;
    }

    /**
     * @brief Store node type (gate functionality) in node context
     * @param type Node type in circuit model
     */
    void
    setType(const std::string type)
    {
        this->type = type;
    };

    /**
     * @brief Return sharing details (of node) stored in node context
     */
    std::pair<int,int>
    getSharing() const
    {
        return this->sharing;
    }

    /**
     * @brief Store sharing details (of node) in node context
     * @param sharing Sharing details (of node) in circuit model
     */
    void
    setSharing(std::pair<int,int> sharing)
    {
        this->sharing = sharing;
    };

    /**
     * @brief Return node partition number in node context
     */
    int
    getPartition() const
    {
        return this->partition;
    }

    /**
     * @brief Store node partition number in node context
     * @param partition Node partition number in circuit model
     */
    void
    setPartition(const int p)
    {
        this->partition = p;
    }

    /**
     * @brief Clear input variable dependencies
     */
    void
    clearVariables()
    {
        this->variables = sylvan::Bdd::bddOne();
    }

    /**
     * @brief Add variable(s) to input variable dependencies
     */
    void
    addVariables(sylvan::Bdd var)
    {
        this->variables *= var;
    }

    /**
     * @brief Get all input variable dependencies
     */
    sylvan::Bdd
    getVariables() const
    {
        return this->variables;
    }

    /**
     * @brief Clear input variable dependencies
     */
    void
    clearRegisters()
    {
        this->registers = sylvan::Bdd::bddOne();
    }

    /**
     * @brief Add variable(s) to input variable dependencies
     */
    void
    addRegisters(sylvan::Bdd var)
    {
        this->registers *= var;
    }

    /**
     * @brief Get all input variable dependencies
     */
    sylvan::Bdd
    getRegisters() const
    {
        return this->registers;
    }

    /**
     * @brief Return BDD representation of node function (in circuit model)
     */
    sylvan::Bdd
    getFunction() const
    {
        return this->function;
    }

    /**
     * @brief Store BDD representation of node function (in circuit model)
     * @param function BDD representation of node function (in circuit model)
     */
    void
    setFunction(sylvan::Bdd function)
    {
        this->function = function;
    }

    NodeContext* getLeftChild() {
        return left_child;
    }

    void setLeftChild(NodeContext* node) {
        left_child = node;
    }

    NodeContext *getRightChild() {
        return right_child;
    }

    void setRightChild(NodeContext* node) {
        right_child = node;
    }

    std::set<uint32_t> *getSupportV(){
        // std::vector<uint32_t> v = sylvan::BddSet(function.Support()).toVector();
        // NISupportV = new std::set(v.begin(), v.end());
        return supportV;
    }
    std::set<uint32_t>* getNISupportV() {
        return NISupportV;
    }

    // std::set<uint32_t> *getUniqueM(){
    //     return uniqueM;
    // }
    
    void setNodeId(uint32_t id) {
        node_id = id;
    }

    std::set<uint32_t> *getPerfectM(){
        return perfectM;
    }

    void computeAuxiliaryTable(){
        if(type == "in" || type == "ref"){
            supportV->insert(node_id);
            NISupportV->insert(node_id);
            // uniqueM->insert(node_id);
            perfectM->insert(node_id);
            return;
        }else if(type == "not" || type == "reg" || type == "out"){
            supportV = left_child->getSupportV();
            NISupportV = left_child->getNISupportV();
            // uniqueM = left_child->getUniqueM();
            perfectM = left_child->getPerfectM();

        }else /*if(type == "and" || type == "nand" || type == "or" || type == "nor")*/{
            NodeContext *l = left_child;
            NodeContext *r = right_child;
            std::set<uint32_t> *lsupp = l->getSupportV();
            std::set<uint32_t> *rsupp = r->getSupportV();

            std::set_union(lsupp->begin(), lsupp->end(), rsupp->begin(), rsupp->end(), inserter(*supportV, supportV->begin()));

            std::set<uint32_t>* nilsupp = left_child->getNISupportV();
            std::set<uint32_t>* nirsupp = right_child->getNISupportV();
            set_union(nilsupp, nirsupp, NISupportV);

            
            // std::set<uint32_t> *unqm_union = new std::set<uint32_t>();
            // std::set<uint32_t> *supp_intersec = new std::set<uint32_t>();

            // std::set_union(l->getUniqueM()->begin(), l->getUniqueM()->end(), r->getUniqueM()->begin(), r->getUniqueM()->end(), inserter(*unqm_union, unqm_union->begin()));

            // std::set_intersection(lsupp->begin(), lsupp->end(), rsupp->begin(), rsupp->end(), inserter(*supp_intersec, supp_intersec->begin()));

            // std::set_difference(unqm_union->begin(), unqm_union->end(), supp_intersec->begin(), supp_intersec->end(),inserter(*uniqueM, uniqueM->begin()));

            if (type == "xor" || type == "xnor"){
                
                std::set<uint32_t>* lpm = new std::set<uint32_t>();
                std::set<uint32_t>* rpm = new std::set<uint32_t>();

                std::set_difference(left_child->getPerfectM()->begin(), left_child->getPerfectM()->end(), right_child->getSupportV()->begin(), right_child->getSupportV()->end(),inserter(*lpm, lpm->begin()));
                std::set_difference(right_child->getPerfectM()->begin(), right_child->getPerfectM()->end(), left_child->getSupportV()->begin(), left_child->getSupportV()->end(),inserter(*rpm, rpm->begin()));
                set_union(lpm, rpm, perfectM);

                // std::set<uint32_t> *perfectM_union = new std::set<uint32_t>();
                // std::set_union(l->getPerfectM()->begin(), l->getPerfectM()->end(), r->getPerfectM()->begin(), r->getPerfectM()->end(), inserter(*perfectM_union, perfectM_union->begin()));
                // std::set_intersection(uniqueM->begin(), uniqueM->end(), perfectM_union->begin(), perfectM_union->end(), inserter(*perfectM, perfectM->begin()));

            }
        }
    }
    
    void setGateId(uint32_t id){
        node_id = id;
    }

    uint32_t getGateId(){
        return node_id;
    }
private:

    /**
     * @brief Node type (gate functionality)
     */
    std::string type;

    /**
     * @brief Sharing information <index,domain>
     */
    std::pair<int,int> sharing;

    /**
     * @brief Partitioning information
     */
    int partition;

    /**
     * @brief BDD representation of node function (in circuit model)
     */
    sylvan::Bdd function;

    /**
     * @brief BDD representation of input variable dependecies (in circuit model)
     */
    sylvan::Bdd variables;

    /**
     * @brief BDD representation of register dependecies (in circuit model)
     */
    sylvan::Bdd registers;

    /* eleminate random observation*/
    uint32_t node_id;
    NodeContext *left_child = NULL;
    NodeContext *right_child = NULL;
    std::set<uint32_t>* NISupportV = new std::set<uint32_t>();
    std::set<uint32_t> *supportV = new std::set<uint32_t>();
    // std::set<uint32_t> *uniqueM = new std::set<uint32_t>();
    std::set<uint32_t> *perfectM = new std::set<uint32_t>();

};

#endif // __NODECONTEXT_HPP_