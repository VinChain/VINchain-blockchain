#pragma once

#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/vindb_block_object.hpp>

namespace graphene {
    namespace chain {

        class vindb_block_create_evaluator : public evaluator<vindb_block_create_evaluator> {
        public:
            typedef vindb_block_create_operation operation_type;

            void_result do_evaluate(const vindb_block_create_operation &o);

            object_id_type do_apply(const vindb_block_create_operation &o);
        };

    }
} // graphene::chain
