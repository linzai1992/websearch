// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/base64.h"
#include "thirdparty/gtest/gtest.h"

TEST(Base64Test, Basic) {
  const std::string kText = "hello world";
  const std::string kBase64Text = "aGVsbG8gd29ybGQ=";

  std::string encoded, decoded;
  bool ok;

  ok = base::Base64Encode(kText, &encoded);
  EXPECT_TRUE(ok);
  EXPECT_EQ(kBase64Text, encoded);

  ok = base::Base64Decode(encoded, &decoded);
  EXPECT_TRUE(ok);
  EXPECT_EQ(kText, decoded);
}
