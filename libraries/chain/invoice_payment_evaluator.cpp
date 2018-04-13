#include <graphene/chain/database.hpp>
#include <graphene/chain/invoice_object.hpp>
#include <graphene/chain/invoice_payment_evaluator.hpp>
#include <graphene/chain/invoice_payment_object.hpp>
#include <graphene/chain/is_authorized_asset.hpp>
#include <graphene/chain/protocol/asset.hpp>
#include <graphene/chain/protocol/invoice.hpp>
#include <graphene/chain/protocol/invoice_payment.hpp>


namespace graphene {
    namespace chain {

        void_result invoice_payment_create_evaluator::do_evaluate(const invoice_payment_create_operation &o) {
            const database &d = db();
            const account_object &from = o.from(d);

            try {
                const auto &idx = d.get_index_type<invoice_index>();
                const auto &by_report_uuid_idx = idx.indices().get<by_report_uuid>();
                auto itr = by_report_uuid_idx.find(o.report_uuid);
                const invoice_object *invoice = nullptr;

                if (itr != by_report_uuid_idx.end()) {
                    invoice = &*itr;
                }

                FC_ASSERT(invoice != nullptr, "Invoice not found");
                FC_ASSERT(invoice->customer == o.from, "Invoice to different customer");

                const asset_object &asset_type = invoice->amount.asset_id(d);
                FC_ASSERT(invoice->status == invoice->created, "Invoice already paid");

                GRAPHENE_ASSERT(
                    is_authorized_asset(d, from, asset_type),
                    transfer_from_account_not_whitelisted,
                    "'from' account ${from} is not whitelisted for asset ${asset}",
                    ("from", o.from)("asset", invoice->amount.asset_id)
                );

                bool insufficient_balance = d.get_balance(from, asset_type).amount >= invoice->amount.amount;

                FC_ASSERT(insufficient_balance,
                          "Insufficient Balance: ${balance}, unable to pay '${invoice}'",
                          ("balance", d.to_pretty_string(d.get_balance(from, asset_type)))
                          ("invoice", o.report_uuid));

                GRAPHENE_ASSERT(
                    is_authorized_asset(d, invoice->contractor(d), asset_type),
                    transfer_to_account_not_whitelisted,
                    "'to' account ${to} is not whitelisted for asset ${asset}",
                    ("to", invoice->contractor)("asset", invoice->amount.asset_id)
                );

                if (asset_type.is_transfer_restricted()) {
                    GRAPHENE_ASSERT(
                        from.id == asset_type.issuer || invoice->contractor(d).id == asset_type.issuer,
                        transfer_restricted_transfer_asset,
                        "Asset ${asset} has transfer_restricted flag enabled",
                        ("asset", invoice->amount.asset_id)
                    );
                }

                for(invoice_vehicle_record record : invoice->records) {
                    GRAPHENE_ASSERT(
                        is_authorized_asset(d, record.data_source(d), asset_type),
                        transfer_to_account_not_whitelisted,
                        "'to' account ${to} is not whitelisted for asset ${asset}",
                        ("to", record.data_source)("asset", invoice->amount.asset_id)
                    );

                    if (asset_type.is_transfer_restricted()) {
                        GRAPHENE_ASSERT(
                            from.id == asset_type.issuer || record.data_source(d).id == asset_type.issuer,
                            transfer_restricted_transfer_asset,
                            "Asset ${asset} has transfer_restricted flag enabled",
                            ("asset", invoice->amount.asset_id)
                        );
                    }
                }
            } FC_RETHROW_EXCEPTIONS(error, "Unable to create payment ${i} from ${f}",
                                    ("i", o.report_uuid)("f", o.from(d).name));

            return void_result();
        }

        object_id_type invoice_payment_create_evaluator::do_apply(const invoice_payment_create_operation &o) {
            try {
                const auto &new_ipo = db().create<invoice_payment_object>([&](invoice_payment_object &obj) {
                    obj.from = o.from;
                    obj.report_uuid = o.report_uuid;
                });

                const auto &idx = db().get_index_type<invoice_index>();
                const auto &by_report_uuid_idx = idx.indices().get<by_report_uuid>();
                auto itr = by_report_uuid_idx.find(o.report_uuid);
                const invoice_object *invoice = nullptr;

                if (itr != by_report_uuid_idx.end()) {
                    invoice = &*itr;
                }

                db().adjust_balance(o.from, -invoice->amount);

                for(invoice_vehicle_record record : invoice->records) {
                    db().adjust_balance(record.data_source, record.reward);
                }

                db().adjust_balance(invoice->contractor, invoice->contractor_reward);

                db().modify(db().get(invoice->get_id()), [&](invoice_object &inv){
                    inv.status = 1;
                });

                return new_ipo.id;
            }
            FC_CAPTURE_AND_RETHROW((o));
        }
    }
}