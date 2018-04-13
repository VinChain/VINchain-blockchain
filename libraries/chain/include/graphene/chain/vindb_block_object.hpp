#pragma once

#include <graphene/chain/protocol/asset.hpp>
#include <graphene/db/object.hpp>
#include <graphene/db/generic_index.hpp>

namespace graphene {
    namespace chain {
        using namespace graphene::db;

        class vindb_block_object;

        class vindb_block_object : public abstract_object<vindb_block_object> {
        public:
            static const uint8_t space_id = protocol_ids;
            static const uint8_t type_id = vindb_block_object_type;

            uint64_t block_id;
            string block_hash;
        };

        struct by_block_id;
        struct by_block_hash;

        using vindb_block_multi_index_type = multi_index_container<
            vindb_block_object,
            indexed_by<
                ordered_unique<tag<by_id>, member<object, object_id_type, &object::id>>,
                ordered_unique<tag<by_block_id>, member<vindb_block_object, uint64_t, &vindb_block_object::block_id>>,
                ordered_unique<tag<by_block_hash>, member<vindb_block_object, string, &vindb_block_object::block_hash>>
            >
        >;

        using vindb_block_index = generic_index<vindb_block_object, vindb_block_multi_index_type>;
    }
}

FC_REFLECT_DERIVED( graphene::chain::vindb_block_object,
        (graphene::db::object),
        (block_id)(block_hash)
)
