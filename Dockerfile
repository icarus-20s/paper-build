# ========================
# Builder stage
# ========================
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
ARG CMAKE_BUILD_TYPE=Release

# Install build dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git wget ca-certificates pkg-config \
    qt6-base-dev qt6-tools-dev qt6-base-dev-tools \
    libgl1-mesa-dev ninja-build \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /src

# Copy source code
COPY . /src

# Configure and build
RUN rm -rf build || true \
  && mkdir -p build \
  && cmake -S . -B build -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
  && cmake --build build --parallel

# ========================
# Runtime stage
# ========================
FROM ubuntu:22.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

# Install only runtime dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    libqt6core6 libqt6gui6 libqt6widgets6 libqt6printsupport6 \
    libgl1-mesa-glx libx11-6 libxcb1 libxkbcommon0 libxcb-glx0 libxcb-render0 libxcb-shm0 libxcb-xfixes0 libxrender1 libfontconfig1 libfreetype6 \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/question_paper

# Copy the built binary
COPY --from=builder /src/build/bin/question_paper_system ./

# Copy the Qt platform plugins from the builder multiarch path into the
# runtime image so the 'xcb' platform plugin is available.
COPY --from=builder /usr/lib/x86_64-linux-gnu/qt6/plugins/platforms /usr/lib/x86_64-linux-gnu/qt6/plugins/platforms

# Tell Qt where to find the plugins
ENV QT_QPA_PLATFORM_PLUGIN_PATH=/usr/lib/x86_64-linux-gnu/qt6/plugins/platforms

# Run the application
ENTRYPOINT ["./question_paper_system"]
