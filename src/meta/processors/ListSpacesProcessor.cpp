/* Copyright (c) 2018 - present, VE Software Inc. All rights reserved
 *
 * This source code is licensed under Apache 2.0 License
 *  (found in the LICENSE.Apache file in the root directory)
 */

#include "meta/processors/ListSpacesProcessor.h"

namespace nebula {
namespace meta {

void ListSpacesProcessor::process(const cpp2::ListSpacesReq& req) {
    UNUSED(req);
    guard_ = std::make_unique<std::lock_guard<std::mutex>>(
                                BaseProcessor<cpp2::ListSpacesResp>::lock_);
    auto prefix = MetaUtils::spacePrefix();
    std::unique_ptr<kvstore::KVIterator> iter;
    auto ret = kvstore_->prefix(kDefaultSpaceId_, kDefaultPartId_, prefix, &iter);
    if (ret != kvstore::ResultCode::SUCCEEDED) {
        resp_.set_code(to(ret));
        onFinished();
        return;
    }
    std::vector<cpp2::IdName> spaces;
    while (iter->valid()) {
        auto spaceId = MetaUtils::spaceId(iter->key());
        auto spaceName = MetaUtils::spaceName(iter->val());
        spaces.emplace_back(apache::thrift::FragileConstructor::FRAGILE,
                            to(spaceId, IDType::SPACE),
                            spaceName.str());
        iter->next();
    }
    resp_.set_spaces(std::move(spaces));
    onFinished();
}

}  // namespace meta
}  // namespace nebula
