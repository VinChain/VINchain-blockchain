#pragma once

#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/exclusive_permission_object.hpp>

namespace graphene {
    namespace chain {

        class give_exclusive_permission_evaluator : public evaluator<give_exclusive_permission_evaluator> {
        public:
            typedef give_exclusive_permission_operation operation_type;

            void_result do_evaluate(const give_exclusive_permission_operation &o);

            object_id_type do_apply(const give_exclusive_permission_operation &o);
        };
    }
} // graphene::chain
