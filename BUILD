# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

COPTS = select({
        "@bazel_tools//src/conditions:windows": ["/std:c++latest"],
        "//conditions:default": ["-std=c++2b"],
    })

cc_library(
    name="headers",
    visibility = ["//visibility:public"],
    hdrs=glob([
        "src/**/*.h*",
    ]),
    includes=[
        "src"
    ],
    deps=[
        "@cultlang_stdext//:headers",
        "@spdlog//:headers",
    ],
    copts = COPTS,
)

cc_library(
    name="ugly",
    visibility = ["//visibility:public"],
    srcs=glob([
        "src/**/*.c*"
    ]),
    deps=[
        ":headers",
        "@cultlang_stdext//:code",
        "@spdlog",
        "@pegtl",
    ],
    copts = COPTS,
)

cc_test(
    name = "ugly-tests",
    srcs = glob([
        "test/**/*.h*", 
        "test/**/*.cpp"
    ]),
    deps = [
        ":ugly",
        "@catch//:single_include",
    ],
    copts = COPTS,
)

cc_test(
    name = "example-literate",
    srcs = glob([
        "example/literate/*.h*", 
        "example/literate/*.cpp",
    ]),
    data = [
        "data/test.nt",
    ],
    deps = [
        ":ugly",
    ],
    copts = COPTS,
)

cc_test(
    name = "example-performance",
    srcs = glob([
        "example/performance/*.h*", 
        "example/performance/*.cpp",
    ]),
    data = [
        "data/test.nt",
    ],
    deps = [
        ":ugly",
    ],
    copts = COPTS,
)

cc_binary(
    name = "uglycli",
    srcs = glob([
        "cli/**/*.h*", 
        "cli/**/*.cpp"
    ]),
    deps = [
        ":ugly",
        "@replxx",
        "@pegtl",
    ],
    copts = COPTS,
)
