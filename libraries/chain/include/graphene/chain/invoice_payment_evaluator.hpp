#pragma once

#include <graphene/chain/evaluator.hpp>
#include <graphene/chain/invoice_payment_object.hpp>
#include <graphene/chain/invoice_object.hpp>

namespace graphene {
    namespace chain {

        class invoice_payment_create_evaluator : public evaluator<invoice_payment_create_evaluator> {
        public:
            typedef invoice_payment_create_operation operation_type;

            void_result do_evaluate(const invoice_payment_create_operation &o);

            object_id_type do_apply(const invoice_payment_create_operation &o);
        };

    }
} // graphene::chain
