#pragma once

#include "EvaluatorBase.hpp"

#include "M2L.hpp"
#include "M2P.hpp"
#include "P2P.hpp"

#include <functional>
#include <deque>

template <typename Context, FMMOptions::EvalType TYPE>
class EvalInteraction : public EvaluatorBase<Context>
{
 public:
  void execute(Context& bc) const {
    auto stree = bc.source_tree();
    auto ttree = bc.target_tree();
    auto K = bc.kernel();

    typedef typename Context::box_type Box;
    typedef typename std::pair<Box, Box> box_pair;
    std::deque<box_pair> pairQ;

    if(stree.root().is_leaf() && ttree.root().is_leaf())
      return P2P::eval(K, bc, stree.root(), ttree.root(), P2P::ONE_SIDED());

    // Queue based tree traversal for P2P, M2P, and/or M2L operations
    pairQ.push_back(box_pair(stree.root(), ttree.root()));

    while (!pairQ.empty()) {
      auto b1 = pairQ.front().first;
      auto b2 = pairQ.front().second;
      pairQ.pop_front();

      if (b2.is_leaf() || (!b1.is_leaf() && b1.side_length() > b2.side_length())) {
        // Split the first box into children and interact
        auto c_end = b1.child_end();
        for (auto cit = b1.child_begin(); cit != c_end; ++cit)
          interact(K, bc, *cit, b2, pairQ);
      } else {
        // Split the second box into children and interact
        auto c_end = b2.child_end();
        for (auto cit = b2.child_begin(); cit != c_end; ++cit)
          interact(K, bc, b1, *cit, pairQ);
      }
    }
  }

  template <typename Kernel, typename BoxContext, typename BOX, typename Q>
  void interact(const Kernel& K, BoxContext& bc,
                const BOX& b1, const BOX& b2,
                Q& pairQ) const {
    if (bc.accept_multipole(b1, b2)) {
      // These boxes satisfy the multipole acceptance criteria
      if (TYPE == FMMOptions::FMM)
        M2L::eval(K, bc, b1, b2);
      else if (TYPE == FMMOptions::TREECODE)
        M2P::eval(K, bc, b1, b2);
    } else if(b1.is_leaf() && b2.is_leaf()) {
      P2P::eval(K, bc, b2, b1, P2P::ONE_SIDED());
    } else {
      pairQ.push_back(std::make_pair(b1,b2));
    }
  }
};


template <typename Context, typename Options>
EvaluatorBase<Context>* make_interact(const Context&, Options& opts) {
  if (opts.evaluator == FMMOptions::FMM) {
    return new EvalInteraction<Context,FMMOptions::FMM>();
  } else if (opts.evaluator == FMMOptions::TREECODE) {
    return new EvalInteraction<Context, FMMOptions::TREECODE>();
  }
  return nullptr;
}
