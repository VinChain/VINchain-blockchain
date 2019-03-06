/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <graphene/chain/exclusive_permission_object.hpp>
#include <graphene/chain/asset_object.hpp>
#include <graphene/chain/account_object.hpp>

#include <iostream>

namespace graphene {
    namespace chain {

/**
 * Keep track of vote totals in internal authority object.  See #533.
 */
        struct permissions_validator {
            
            const bool check_permissions_for_operation(const graphene::chain::database &db, const account_id_type &account_id, const string &op_name) const { 

                const auto &idx = db.get_index_type<exclusive_permission_index>();
                const auto &by_account_idx = idx.indices().get<by_account>();
                const auto &by_id_idx = idx.indices().get<by_id>();

                auto records_range = by_account_idx.equal_range(account_id);
                return std::any_of(records_range.first, records_range.second,
                    [&](const exclusive_permission_object &record) {
                        return record.permission == op_name;
                    });
            }
        };
    }
} // graphene::chain
