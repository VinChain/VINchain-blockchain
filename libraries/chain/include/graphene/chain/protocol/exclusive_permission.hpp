#pragma once

#include <graphene/chain/protocol/base.hpp>

namespace graphene {
    namespace chain {
        struct give_exclusive_permission_operation : public base_operation {
            struct fee_parameters_type {
                uint64_t fee = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
            };

            account_id_type from;
            account_id_type to;

            string permission;

            asset fee;

            account_id_type fee_payer() const { return from; }

            void validate() const;
        };
    }
} // graphene::chain

FC_REFLECT( graphene::chain::give_exclusive_permission_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::give_exclusive_permission_operation, (from)(to)(permission)(fee) )
