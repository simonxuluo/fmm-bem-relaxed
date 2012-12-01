#pragma once
/** @file INITM.hpp
 * @brief Dispatch methods for the initializing a local expansion
 *
 */

#include "KernelTraits.hpp"
#include <type_traits>

class INITL
{
  /** If no init_multipole dispatcher matches */
  template <typename... Args>
  inline static void eval(Args...) {
    // Do nothing
  }

  template <typename Kernel>
  inline static
  typename std::enable_if<ExpansionTraits<Kernel>::has_init_local>::type
  eval(const Kernel& K,
       typename Kernel::local_type& L,
       double box_size) {
    K.init_local(L, box_size);
  }

public:

  template <typename Kernel, typename BoxContext, typename Box>
  inline static void eval(Kernel& K,
			  BoxContext& bc,
			  const Box& b,
			  double box_size)
  {
#ifdef DEBUG
    printf("initL: %d to %d\n", b.index());
#endif

    INITL::eval(K, bc.local_expansion(b), box_size);
  }
};