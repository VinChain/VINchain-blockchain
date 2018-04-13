#include <graphene/chain/database.hpp>
#include <graphene/chain/vindb_block_evaluator.hpp>


namespace graphene {
    namespace chain {
        void_result vindb_block_create_evaluator::do_evaluate(const vindb_block_create_operation &o) {
            return void_result();
        }

        object_id_type vindb_block_create_evaluator::do_apply(const vindb_block_create_operation &o) {
            try {
                const auto &new_vindb_block_object = db().create<vindb_block_object>(
                        [&](vindb_block_object &obj) {
                            obj.block_id = o.block_id;
                            obj.block_hash = o.block_hash;
                        });

                return new_vindb_block_object.id;
            }
            FC_CAPTURE_AND_RETHROW((o));
        }
    }
} // graphene::chain
