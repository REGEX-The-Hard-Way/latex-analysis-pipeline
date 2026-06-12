# Multi-stage Docker build for latex-analysis-pipeline
# Stage 1: Build all components
FROM debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc \
    make \
    ragel \
    libc6-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .

# Build scanner and sentence splitter
RUN cd scanner && make scanner && make sent_split

# Build macro expander
RUN gcc -O2 macro_expander.c -o macro_expander.out

# Build multi_analyzer
RUN cd scanner/multi_analyzer && make all

# Stage 2: Minimal runtime
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    python3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/latex-analysis-pipeline

# Copy built binaries
COPY --from=builder /build/scanner/scanner.out      /opt/latex-analysis-pipeline/scanner/
COPY --from=builder /build/scanner/sent_split.out    /opt/latex-analysis-pipeline/scanner/
COPY --from=builder /build/macro_expander.out        /opt/latex-analysis-pipeline/
COPY --from=builder /build/scanner/multi_analyzer/multi_analyzer.out \
    /opt/latex-analysis-pipeline/scanner/multi_analyzer/

# Copy test suite and scripts
COPY --from=builder /build/tests/                    /opt/latex-analysis-pipeline/tests/
COPY --from=builder /build/scanner/latex.rl          /opt/latex-analysis-pipeline/scanner/
COPY --from=builder /build/scanner/sound1.tex        /opt/latex-analysis-pipeline/scanner/

# Convenience symlinks
RUN ln -s /opt/latex-analysis-pipeline/scanner/scanner.out /usr/local/bin/latex-scanner && \
    ln -s /opt/latex-analysis-pipeline/scanner/sent_split.out /usr/local/bin/latex-sent-split && \
    ln -s /opt/latex-analysis-pipeline/macro_expander.out /usr/local/bin/latex-macro-expand

ENV PATH="/opt/latex-analysis-pipeline:$PATH"

ENTRYPOINT ["/opt/latex-analysis-pipeline/scanner/scanner.out"]
CMD ["--help"]
