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
#include <graphene/chain/protocol/asset_ops.hpp>

namespace graphene {
    namespace chain {

/**
 *  Valid symbols can contain [A-Z0-9], and '.'
 *  They must start with [A, Z]
 *  They must end with [A, Z]
 *  They can contain a maximum of one '.'
 */
        bool is_valid_symbol(const string &symbol) {
            if (symbol.size() < GRAPHENE_MIN_ASSET_SYMBOL_LENGTH)
                return false;

            if (symbol.substr(0, 3) == "BIT")
                return false;

            if (symbol.size() > GRAPHENE_MAX_ASSET_SYMBOL_LENGTH)
                return false;

            if (!isalpha(symbol.front()))
                return false;

            if (!isalpha(symbol.back()))
                return false;

            bool dot_already_present = false;
            for (const auto c : symbol) {
                if ((isalpha(c) && isupper(c)) || isdigit(c))
                    continue;

                if (c == '.') {
                    if (dot_already_present)
                        return false;

                    dot_already_present = true;
                    continue;
                }

                return false;
            }

            return true;
        }

        share_type asset_issue_operation::calculate_fee(const fee_parameters_type &k) const {
            return k.fee + calculate_data_fee(fc::raw::pack_size(memo), k.price_per_kbyte);
        }

        share_type
        asset_create_operation::calculate_fee(const asset_create_operation::fee_parameters_type &param) const {
            auto core_fee_required = param.long_symbol;

            switch (symbol.size()) {
                case 3:
                    core_fee_required = param.symbol3;
                    break;
                case 4:
                    core_fee_required = param.symbol4;
                    break;
                default:
                    break;
            }

            // common_options contains several lists and a string. Charge fees for its size
            core_fee_required += calculate_data_fee(fc::raw::pack_size(*this), param.price_per_kbyte);

            return core_fee_required;
        }

        void asset_create_operation::validate() const {
            FC_ASSERT(!bitasset_opts.valid(), "Creation of Market Pegged Assets is disabled."); 
            // uia-specific flags do not allow charging market fees and confidential transactions at the moment           
            FC_ASSERT(!(common_options.flags & charge_market_fee), "'charge_market_fee' flag is disabled.");
            FC_ASSERT(!(common_options.flags & disable_confidential), "'disable_confidential' flag is disabled.");
            FC_ASSERT(common_options.market_fee_percent == 0 && common_options.max_market_fee == 0 && !is_prediction_market, "Market fees are disbaled.");

            // mia-specific permissions are disabled at the moment 
            FC_ASSERT((!(common_options.issuer_permissions & disable_force_settle)) && (!(common_options.flags & disable_force_settle)), "'disable_force_settle' permission is disabled.");
            FC_ASSERT((!(common_options.issuer_permissions & global_settle)) && (!(common_options.flags & global_settle)), "'global_settle' permission is disabled.");
            FC_ASSERT((!(common_options.issuer_permissions & witness_fed_asset)) && (!(common_options.flags & witness_fed_asset)), "'witness_fed_asset' permission is disabled.");
            FC_ASSERT((!(common_options.issuer_permissions & committee_fed_asset)) && (!(common_options.flags & committee_fed_asset)), "'committee_fed_asset' permission is disabled.");

            FC_ASSERT(fee.amount >= 0);
            FC_ASSERT(is_valid_symbol(symbol));

            common_options.validate();
            asset fee_asset = asset(1) * common_options.core_exchange_rate;
            FC_ASSERT(fee_asset.asset_id == asset_id_type(1));

            if (common_options.extensions.value.payment_core_exchange_rate.valid()) {
                common_options.extensions.value.payment_core_exchange_rate->validate();
                asset payment_asset = asset(1) * (*common_options.extensions.value.payment_core_exchange_rate);
                FC_ASSERT(payment_asset.asset_id == asset_id_type(1));
            }
            
            FC_ASSERT(precision <= 12);
        }

        void asset_update_operation::validate() const {
            // uia-specific flags do not allow charging market fees and confidential transactions at the moment           
            FC_ASSERT(!(new_options.flags & charge_market_fee), "'charge_market_fee' flag is disabled.");
            FC_ASSERT(!(new_options.flags & disable_confidential), "'disable_confidential' flag is disabled.");
            FC_ASSERT(new_options.market_fee_percent == 0 && new_options.max_market_fee == 0, "Market fees are disbaled.");

            // mia-specific permissions are disabled at the moment 
            FC_ASSERT((!(new_options.issuer_permissions & disable_force_settle)) && (!(new_options.flags & disable_force_settle)), "'disable_force_settle' permission is disabled.");
            FC_ASSERT((!(new_options.issuer_permissions & global_settle)) && (!(new_options.flags & global_settle)), "'global_settle' permission is disabled.");
            FC_ASSERT((!(new_options.issuer_permissions & witness_fed_asset)) && (!(new_options.flags & witness_fed_asset)), "'witness_fed_asset' permission is disabled.");
            FC_ASSERT((!(new_options.issuer_permissions & committee_fed_asset)) && (!(new_options.flags & committee_fed_asset)), "'committee_fed_asset' permission is disabled.");

            FC_ASSERT(fee.amount >= 0);
            if (new_issuer)
                FC_ASSERT(issuer != *new_issuer);

            new_options.validate();
            asset fee_asset = asset(1, asset_to_update) * new_options.core_exchange_rate;
            FC_ASSERT(fee_asset.asset_id == asset_id_type());

            if (new_options.extensions.value.payment_core_exchange_rate.valid()) {
                new_options.extensions.value.payment_core_exchange_rate->validate();
                asset payment_asset = asset(1, asset_to_update) * (*new_options.extensions.value.payment_core_exchange_rate);
                FC_ASSERT(payment_asset.asset_id == asset_id_type());
            }
        }

        share_type asset_update_operation::calculate_fee(const asset_update_operation::fee_parameters_type &k) const {
            return k.fee + calculate_data_fee(fc::raw::pack_size(*this), k.price_per_kbyte);
        }


        void asset_publish_feed_operation::validate() const {
            FC_ASSERT(false, "Operation not permitted right now.");
            FC_ASSERT(fee.amount >= 0);
            feed.validate();

            // maybe some of these could be moved to feed.validate()
            if (!feed.core_exchange_rate.is_null()) {
                feed.core_exchange_rate.validate();
            }
            if ((!feed.settlement_price.is_null()) && (!feed.core_exchange_rate.is_null())) {
                FC_ASSERT(feed.settlement_price.base.asset_id == feed.core_exchange_rate.base.asset_id);
            }

            FC_ASSERT(!feed.settlement_price.is_null());
            FC_ASSERT(!feed.core_exchange_rate.is_null());
            FC_ASSERT(feed.is_for(asset_id));
        }

        void asset_reserve_operation::validate() const {
            FC_ASSERT(false, "Operation not permitted right now.");
            FC_ASSERT(fee.amount >= 0);
            FC_ASSERT(amount_to_reserve.amount.value <= GRAPHENE_MAX_SHARE_SUPPLY);
            FC_ASSERT(amount_to_reserve.amount.value > 0);
        }

        void asset_issue_operation::validate() const {
            FC_ASSERT(fee.amount >= 0);
            FC_ASSERT(asset_to_issue.amount.value <= GRAPHENE_MAX_SHARE_SUPPLY);
            FC_ASSERT(asset_to_issue.amount.value > 0);
            FC_ASSERT(asset_to_issue.asset_id != asset_id_type(0));
        }

        void asset_fund_fee_pool_operation::validate() const {
            FC_ASSERT(fee.amount >= 0);
            FC_ASSERT(fee.asset_id == asset_id_type());
            FC_ASSERT(amount > 0);
        }

        void asset_settle_operation::validate() const {
            FC_ASSERT(false, "Operation not permitted right now.");
            FC_ASSERT(fee.amount >= 0);
            FC_ASSERT(amount.amount >= 0);
        }

        void asset_update_bitasset_operation::validate() const {
            FC_ASSERT(false, "Operation not permitted right now.");
            FC_ASSERT(fee.amount >= 0);
            new_options.validate();
        }

        void asset_update_feed_producers_operation::validate() const {
            FC_ASSERT(false, "Operation not permitted right now.");
            FC_ASSERT(fee.amount >= 0);
        }

        void asset_global_settle_operation::validate() const {
            FC_ASSERT(false, "Operation not permitted right now.");
            FC_ASSERT(fee.amount >= 0);
            FC_ASSERT(asset_to_settle == settle_price.base.asset_id);
        }

        void bitasset_options::validate() const {
            FC_ASSERT(minimum_feeds > 0);
            FC_ASSERT(force_settlement_offset_percent <= GRAPHENE_100_PERCENT);
            FC_ASSERT(maximum_force_settlement_volume <= GRAPHENE_100_PERCENT);
        }

        void asset_options::validate() const {
            FC_ASSERT(max_supply > 0);
            FC_ASSERT(max_supply <= GRAPHENE_MAX_SHARE_SUPPLY);
            FC_ASSERT(market_fee_percent <= GRAPHENE_100_PERCENT);
            FC_ASSERT(max_market_fee >= 0 && max_market_fee <= GRAPHENE_MAX_SHARE_SUPPLY);
            // There must be no high bits in permissions whose meaning is not known.
            FC_ASSERT(!(issuer_permissions & ~ASSET_ISSUER_PERMISSION_MASK));
            // The global_settle flag may never be set (this is a permission only)
            FC_ASSERT(!(flags & global_settle));
            // the witness_fed and committee_fed flags cannot be set simultaneously
            FC_ASSERT((flags & (witness_fed_asset | committee_fed_asset)) != (witness_fed_asset | committee_fed_asset));
            core_exchange_rate.validate();
            FC_ASSERT(core_exchange_rate.base.asset_id.instance.value == 0 ||
                      core_exchange_rate.quote.asset_id.instance.value == 0);

            if (extensions.value.payment_core_exchange_rate.valid()) {
                extensions.value.payment_core_exchange_rate->validate();
                FC_ASSERT(extensions.value.payment_core_exchange_rate->base.asset_id.instance.value == 0 ||
                          extensions.value.payment_core_exchange_rate->quote.asset_id.instance.value == 0);
            }
            
            if (!whitelist_authorities.empty() || !blacklist_authorities.empty())
                FC_ASSERT(flags & white_list);
            for (auto item : whitelist_markets) {
                FC_ASSERT(blacklist_markets.find(item) == blacklist_markets.end());
            }
            for (auto item : blacklist_markets) {
                FC_ASSERT(whitelist_markets.find(item) == whitelist_markets.end());
            }
        }

        void asset_claim_fees_operation::validate() const {
            FC_ASSERT(fee.amount >= 0);
            FC_ASSERT(amount_to_claim.amount > 0);
        }

    }
} // namespace graphene::chain
