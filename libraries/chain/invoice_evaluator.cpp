#include <graphene/chain/database.hpp>
#include <graphene/chain/invoice_evaluator.hpp>
#include <graphene/chain/invoice_object.hpp>
#include <graphene/chain/protocol/invoice.hpp>
#include <graphene/chain/exclusive_permission_object.hpp>


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

            try {
                const auto &idx = db().get_index_type<exclusive_permission_index>();
                const auto &by_account_idx = idx.indices().get<by_account>();

                bool can_give = false;
                auto records_range = by_account_idx.equal_range(contractor.get_id());
                std::for_each(records_range.first, records_range.second,
                              [&](const exclusive_permission_object &record) {
                                  if (record.permission == "invoice_create")
                                      can_give = true;
                              });

                FC_ASSERT(can_give, "Could not permission for this operation");
            } FC_CAPTURE_AND_RETHROW((o));

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
                    obj.block_num = db().head_block_num() + 1;
                });

                vector <account_id_type> data_sources;

                for (auto record : new_invoice_object.records) {
                    if (std::find(data_sources.begin(), data_sources.end(), record.data_source) == data_sources.end()) {
                        data_sources.push_back(record.data_source);

                        db().create<invoice_data_source_object>([&](invoice_data_source_object &obj) {
                            obj.invoice = new_invoice_object.get_id();
                            obj.data_source = record.data_source;
                        });

                    }
                }

                return new_invoice_object.id;

            }
            FC_CAPTURE_AND_RETHROW((o));
        }
    }
} // graphene::chain
