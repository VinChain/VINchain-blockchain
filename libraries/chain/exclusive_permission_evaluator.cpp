#include <graphene/chain/database.hpp>
#include <graphene/chain/protocol/exclusive_permission.hpp>
#include <graphene/chain/exclusive_permission_object.hpp>
#include <graphene/chain/exclusive_permission_evaluator.hpp>


namespace graphene {
    namespace chain {
        void_result give_exclusive_permission_evaluator::do_evaluate(const give_exclusive_permission_operation &o) {
            const database &d = db();
            const account_object &from = o.from(d);
            const account_object &to = o.to(d);

            try {
                const auto &idx = d.get_index_type<exclusive_permission_index>();
                const auto &by_id_idx = idx.indices().get<by_id>();
                const auto &by_account_idx = idx.indices().get<by_account>();

                if (boost::size(by_id_idx)) {
                    bool can_give = false;
                    auto records_range = by_account_idx.equal_range(from.get_id());
                    std::for_each(records_range.first, records_range.second,
                                  [&](const exclusive_permission_object &record) {
                                      if (record.permission == "give_exclusive_permission")
                                          can_give = true;
                                  });

                    FC_ASSERT(can_give, "Could not give exclusive permission");
                }
            } FC_RETHROW_EXCEPTIONS(error, "Unable to give exclusive permission from ${f} to ${t}",
                                    ("f", from.name)("t", to.name));
            return void_result();
        }

        object_id_type give_exclusive_permission_evaluator::do_apply(const give_exclusive_permission_operation &o) {
            try {
                const auto &new_exclusive_permission_object = db().create<exclusive_permission_object>(
                        [&](exclusive_permission_object &obj) {
                            obj.account = o.to;
                            obj.permission = o.permission;
                        });
                return new_exclusive_permission_object.id;
            }
            FC_CAPTURE_AND_RETHROW((o));
        }
    }
} // graphene::chain
