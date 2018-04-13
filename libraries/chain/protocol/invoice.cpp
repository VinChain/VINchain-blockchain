#include <graphene/chain/protocol/invoice.hpp>

namespace graphene {
    namespace chain {
        void invoice_create_operation::validate() const {
            FC_ASSERT(fee.amount >= 0);
            FC_ASSERT(amount.amount >= 0);
            FC_ASSERT(contractor_reward.amount >= 0);
            FC_ASSERT(contractor_reward.asset_id == amount.asset_id);

            for (invoice_vehicle_record record : records) {
                FC_ASSERT(amount.asset_id == record.reward.asset_id);
                FC_ASSERT(record.reward.amount >= 0);
            }
        }
    }
} // graphene::chain
