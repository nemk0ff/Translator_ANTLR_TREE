FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV ANTLR_VERSION=4.13.1
ENV ANTLR4_JAR=/opt/antlr/antlr-4.13.1-complete.jar
ENV LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    make \
    default-jre \
    curl \
    git \
    cmake \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Use matching ANTLR tool/runtime versions to avoid C++ API mismatch.
RUN mkdir -p /opt/antlr \
    && curl -L "https://www.antlr.org/download/antlr-${ANTLR_VERSION}-complete.jar" -o "${ANTLR4_JAR}" \
    && git clone --depth 1 --branch "${ANTLR_VERSION}" https://github.com/antlr/antlr4.git /tmp/antlr4src \
    && cmake -S /tmp/antlr4src/runtime/Cpp -B /tmp/antlr4build \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_INSTALL_PREFIX=/usr/local \
         -DBUILD_SHARED_LIBS=ON \
         -DANTLR_BUILD_CPP_TESTS=OFF \
    && cmake --build /tmp/antlr4build --config Release -j"$(nproc)" \
    && cmake --install /tmp/antlr4build --config Release \
    && rm -rf /tmp/antlr4src /tmp/antlr4build

WORKDIR /app
COPY . .

CMD ["make", "test", "ANTLR4_JAR=/opt/antlr/antlr-4.13.1-complete.jar", "RUNTIME_INCLUDE=/usr/local/include/antlr4-runtime", "RUNTIME_LIBDIR=/usr/local/lib"]
