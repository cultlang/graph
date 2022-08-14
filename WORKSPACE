workspace(name = "cultlang_graph")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")


########################
# cultlang
git_repository(
    name = "cultlang_stdext",
    remote = "https://github.com/cultlang/stdext",
    commit = "0a5aa357bc3520a4578d176f2869d956a0b91d58",
    shallow_since = "1660444934 -0700",
)

########################
# catch
git_repository(
    name = "catch",
    remote = "https://github.com/cgrinker/Catch2",
    commit = "5e6488fd9949cb41d717a72c8c4603b7e37d68cd",
    shallow_since = "1560275997 -0700",
)

########################
# spdlog:
git_repository(
    name = "spdlog",
    remote = "https://github.com/cgrinker/spdlog",
    commit = "d853f44b345d752538c5e3cd8c5f68e9f45e6806",
    shallow_since = "1611099882 -0800",
)

########################
# replxx
git_repository(
    name = "replxx",
    remote = "https://github.com/cgrinker/replxx",
    commit = "c664b6e528e46725a00e34328320688bf512ce80",
    shallow_since = "1625868483 -0700",
)

########################
# pegtl
git_repository(
    name = "pegtl",
    remote = "https://github.com/mason-bially/PEGTL",
    commit = "5eda0c4f2f0e04e781a6a17dcdededf7b35431c8",
    shallow_since = "1660438370 -0700",
)

