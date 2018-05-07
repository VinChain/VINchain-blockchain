#pragma once

#include <graphene/db/object.hpp>
#include <graphene/db/generic_index.hpp>

namespace graphene {
    namespace chain {
        using namespace graphene::db;

        class exclusive_permission_object;

        class exclusive_permission_object : public abstract_object<exclusive_permission_object> {
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id = exclusive_permission_object_type;

            account_id_type account;
            string permission;
        };

        struct by_account;

        using exclusive_permission_multi_index_type = multi_index_container<
                exclusive_permission_object,
                indexed_by<
                        ordered_unique<tag<by_id>, member<object, object_id_type, &object::id>>,
                        ordered_non_unique<tag<by_account>,
                                           member<exclusive_permission_object,
                                                  account_id_type, &exclusive_permission_object::account>>
                >
        >;

        using exclusive_permission_index = generic_index<exclusive_permission_object,
                                                         exclusive_permission_multi_index_type>;
    }
} // graphene::chain

FC_REFLECT_DERIVED( graphene::chain::exclusive_permission_object,
                    (graphene::db::object), (account)(permission)
)
