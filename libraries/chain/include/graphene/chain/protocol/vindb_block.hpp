#pragma once

#include <graphene/chain/protocol/base.hpp>

namespace graphene {
    namespace chain {
        struct vindb_block_create_operation : public base_operation {
            struct fee_parameters_type {
                uint64_t fee = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
            };

            uint64_t block_id;
            string block_hash;

            account_id_type owner;
            asset fee;

            account_id_type fee_payer() const { return owner; }

            void validate() const;
        };
    }
} // graphene::chain

FC_REFLECT( graphene::chain::vindb_block_create_operation::fee_parameters_type, (fee) )
FC_REFLECT( graphene::chain::vindb_block_create_operation, (block_id)(block_hash)(owner)(fee) )
