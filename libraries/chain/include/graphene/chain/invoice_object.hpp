#pragma once

#include <graphene/chain/protocol/asset.hpp>
#include <graphene/db/object.hpp>
#include <graphene/db/generic_index.hpp>

namespace graphene {
    namespace chain {
        using namespace graphene::db;

        class invoice_object;

        class invoice_object : public abstract_object<invoice_object> {
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id = invoice_object_type;
            static const uint8_t created = 0;
            static const uint8_t paid = 1;

            account_id_type contractor;
            account_id_type customer;
            string vin;
            string report_uuid;

            asset amount;
            asset contractor_reward;
            uint8_t status;

            vector <invoice_vehicle_record> records;
            uint32_t block_num;

            invoice_object_id_type get_id() const {
                return id;
            };
        };

        struct by_report_uuid;

        using invoice_multi_index_type = multi_index_container<
            invoice_object,
            indexed_by<
                ordered_unique<tag<by_id>, member<object, object_id_type, &object::id>>,
                ordered_unique<tag<by_report_uuid>,
                               member<invoice_object, string, &invoice_object::report_uuid>>
            >
        >;

        using invoice_index = generic_index<invoice_object, invoice_multi_index_type>;


        class invoice_data_source_object;
        class invoice_data_source_object : public abstract_object<invoice_data_source_object> {
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id = invoice_data_source_object_type;

            account_id_type data_source;
            invoice_object_id_type invoice;
        };

        struct by_data_source;

        using invoice_data_source_multi_index_type = multi_index_container<
            invoice_data_source_object,
            indexed_by<
                ordered_unique<tag<by_id>, member<object, object_id_type, &object::id>>,
                ordered_non_unique<tag<by_data_source>,
                               member<invoice_data_source_object, account_id_type, &invoice_data_source_object::data_source>>
            >
        >;

        using invoice_data_source_index = generic_index<invoice_data_source_object, invoice_data_source_multi_index_type>;

    }
} // graphene::chain

FC_REFLECT_DERIVED( graphene::chain::invoice_object,
        (graphene::db::object),
        (contractor)
        (customer)
        (vin)
        (report_uuid)
        (amount)
        (contractor_reward)
        (status)
        (records)
        (block_num)
)

FC_REFLECT_DERIVED( graphene::chain::invoice_data_source_object,
                    (graphene::db::object),
                    (data_source)(invoice)
)