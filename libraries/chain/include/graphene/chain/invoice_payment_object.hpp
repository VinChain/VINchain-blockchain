#pragma once

#include <graphene/chain/protocol/asset.hpp>
#include <graphene/db/object.hpp>
#include <graphene/db/generic_index.hpp>

namespace graphene {
    namespace chain {
        using namespace graphene::db;

        class invoice_payment_object;

        class invoice_payment_object : public abstract_object<invoice_payment_object> {
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id = invoice_payment_object_type;

            account_id_type from;
            string report_uuid;
        };

        using invoice_payment_multi_index_type = multi_index_container <
        invoice_payment_object,
        indexed_by<
                ordered_unique < tag < by_id>, member<object, object_id_type, &object::id>>
        >
        >;

        using invoice_payment_index = generic_index<invoice_payment_object, invoice_payment_multi_index_type>;
    }
} // graphene::chain

FC_REFLECT_DERIVED( graphene::chain::invoice_payment_object, (graphene::db::object), (from)(report_uuid) )
