#include <graphene/chain/protocol/invoice_payment.hpp>

namespace graphene {
    namespace chain {
        void invoice_payment_create_operation::validate() const {
            FC_ASSERT(fee.amount >= 0);
        }
    }
} // graphene::chain
