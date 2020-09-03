FROM ubuntu:20.04
RUN apt-get update && apt-get install build-essential curl gnupg git -y
RUN curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel.gpg
RUN mv bazel.gpg /etc/apt/trusted.gpg.d/
RUN echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list
RUN apt update && apt install bazel -y
RUN bazel --help
WORKDIR /usr/src/cultlang/graph

