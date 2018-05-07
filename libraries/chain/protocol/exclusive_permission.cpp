#include <graphene/chain/protocol/exclusive_permission.hpp>

namespace graphene {
    namespace chain {
        void give_exclusive_permission_operation::validate() const {
            FC_ASSERT(fee.amount >= 0);
        }
    }
} // graphene::chain