# Bazel Build File For Type system


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
        "@bazel_tools//src/conditions:darwin": ["-std=c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    deps=[
        "@stdext//:stdext",
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
        "@bazel_tools//src/conditions:darwin": ["-std=c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    deps = [
        ":graph",
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
        "@bazel_tools//src/conditions:darwin": ["-std=c++17"],
        "//conditions:default": ["-std=c++17"],
    }),
    deps = [
        ":graph",
        "@replxx//:replxx",
        "@catch//:single_include",
        "@pegtl//:pegtl"
    ],
)