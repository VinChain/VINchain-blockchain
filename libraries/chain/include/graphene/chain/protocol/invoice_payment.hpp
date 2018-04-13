#pragma once

#include <graphene/chain/protocol/base.hpp>

namespace graphene {
    namespace chain {
        struct invoice_payment_create_operation : public base_operation {
            struct fee_parameters_type {
                uint64_t fee = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
            };

            account_id_type from;
            string report_uuid;

            asset fee;

            account_id_type fee_payer() const { return from; }

            void validate() const;
        };
    }
} // graphene::chain

FC_REFLECT( graphene::chain::invoice_payment_create_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::invoice_payment_create_operation, (from)(report_uuid)(fee) )
