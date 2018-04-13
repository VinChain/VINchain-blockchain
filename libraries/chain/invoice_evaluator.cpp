#include <graphene/chain/database.hpp>
#include <graphene/chain/invoice_evaluator.hpp>
#include <graphene/chain/invoice_object.hpp>
#include <graphene/chain/protocol/invoice.hpp>


namespace graphene {
    namespace chain {
        void_result invoice_create_evaluator::do_evaluate(const invoice_create_operation &o) {
            const database &d = db();
            const account_object &contractor = o.contractor(d);
            const account_object &customer = o.customer(d);
            const asset_object &amount = o.amount.asset_id(d);
            asset total_amount = asset(0, amount.id);

            try {
                for(invoice_vehicle_record record : o.records) {
                    total_amount += record.reward;
                }

                total_amount += o.contractor_reward;

                FC_ASSERT(o.amount == total_amount, "Incorrect rewards for data sources records");
            } FC_RETHROW_EXCEPTIONS(error, "Unable to create invoice ${i} from ${c} to ${cr}",
                                    ("i", o.report_uuid)("c", contractor.name)("cr", customer.name));

            return void_result();
        }

        object_id_type invoice_create_evaluator::do_apply(const invoice_create_operation &o) {
            try {
                const auto &new_invoice_object = db().create<invoice_object>([&](invoice_object &obj) {
                    obj.contractor = o.contractor;
                    obj.customer = o.customer;
                    obj.vin = o.vin;
                    obj.report_uuid = o.report_uuid;
                    obj.amount = o.amount;
                    obj.contractor_reward = o.contractor_reward;
                    obj.status = obj.created;
                    obj.records = o.records;
                });

                return new_invoice_object.id;

            }
            FC_CAPTURE_AND_RETHROW((o));
        }
    }
} // graphene::chain
