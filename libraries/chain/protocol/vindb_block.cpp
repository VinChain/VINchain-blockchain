#include <graphene/chain/protocol/vindb_block.hpp>

namespace graphene {
    namespace chain {
        void vindb_block_create_operation::validate() const {
            FC_ASSERT(fee.amount >= 0);

            // check owner permissions
        }
    }
} // graphene::chain
