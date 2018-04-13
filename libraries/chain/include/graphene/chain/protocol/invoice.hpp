#pragma once

#include <graphene/chain/protocol/base.hpp>

namespace graphene {
    namespace chain {
        struct invoice_vehicle_record {
            account_id_type data_source;
            string hash;
            uint64_t value;
            asset reward;
        };

        struct invoice_create_operation : public base_operation {
            struct fee_parameters_type {
                uint64_t fee = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
            };

            account_id_type contractor;
            account_id_type customer;
            string vin;
            string report_uuid;

            asset amount;
            asset contractor_reward;

            vector <invoice_vehicle_record> records;

            asset fee;

            account_id_type fee_payer() const { return contractor; }

            void validate() const;
        };
    }
} // graphene::chain

FC_REFLECT(graphene::chain::invoice_create_operation::fee_parameters_type, (fee))
FC_REFLECT(graphene::chain::invoice_create_operation,
           (contractor)(customer)(vin)(report_uuid)(amount)(contractor_reward)(records)(fee))
FC_REFLECT(graphene::chain::invoice_vehicle_record, (data_source)(hash)(value)(reward))
