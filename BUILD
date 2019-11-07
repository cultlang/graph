# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

cc_library(
    name="graph",
    visibility = ["//visibility:public"],
    hdrs=glob([
        "src/**/*.h*",
    ]),
    srcs=glob([
        "src/**/*.c*"
    ]),
    includes=[
        "src"
    ],
    copts = select({
        "@bazel_tools//src/conditions:windows": ["/std:c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    deps=[
        "@cultlang_stdext//:headers",
        "@spdlog//:headers",
    ]
)



cc_test(
    name = "test",
    srcs = glob([
        "test/**/*.h*", 
        "test/**/*.cpp"
    ]),
    copts = select({
        "@bazel_tools//src/conditions:windows": ["/std:c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    deps = [
        ":graph",
        "@cultlang_stdext//:code",
        "@catch//:single_include",
    ],
)

cc_binary(
    name = "cli",
    srcs = glob([
        "cli/**/*.h*", 
        "cli/**/*.cpp"
    ]),
    copts = select({
        "@bazel_tools//src/conditions:windows": ["/std:c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    deps = [
        ":graph",
        "@cultlang_stdext//:code",
        "@fmt//:fmt",
        "@replxx//:replxx",
        "@pegtl//:pegtl",
    ],
)