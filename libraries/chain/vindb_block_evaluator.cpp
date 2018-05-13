#include <graphene/chain/database.hpp>
#include <graphene/chain/vindb_block_evaluator.hpp>
#include <graphene/chain/exclusive_permission_object.hpp>


namespace graphene {
    namespace chain {
        void_result vindb_block_create_evaluator::do_evaluate(const vindb_block_create_operation &o) {
            try {
                const auto &idx = db().get_index_type<exclusive_permission_index>();
                const auto &by_account_idx = idx.indices().get<by_account>();

                bool can_give = false;
                auto records_range = by_account_idx.equal_range(o.owner);
                std::for_each(records_range.first, records_range.second,
                              [&](const exclusive_permission_object &record) {
                                  if (record.permission == "vindb_block_create")
                                      can_give = true;
                              });

                FC_ASSERT(can_give, "Could not permission for this operation");
            } FC_CAPTURE_AND_RETHROW((o));

            return void_result();
        }

        object_id_type vindb_block_create_evaluator::do_apply(const vindb_block_create_operation &o) {
            try {
                const auto &new_vindb_block_object = db().create<vindb_block_object>(
                        [&](vindb_block_object &obj) {
                            obj.block_id = o.block_id;
                            obj.block_hash = o.block_hash;
                            obj.block_num = db().head_block_num() + 1;
                        });

                return new_vindb_block_object.id;
            }
            FC_CAPTURE_AND_RETHROW((o));
        }
    }
} // graphene::chain
