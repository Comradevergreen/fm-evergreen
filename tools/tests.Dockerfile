# The host tests (tests/run_tests.sh) in Linux, as CI runs them (.github/workflows/firmware-preview.yml): Ubuntu
# 24.04, gcc and CI's pinned Python packages. For a Mac, where run_tests.sh does not compile (Mach-O rejects the ELF
# section attributes). Build and run it natively (arm64 on Apple silicon): the tests need no x86, and under QEMU's
# amd64 emulation they take hours and can hang. -ffp-contract=off keeps arm64 gcc from fusing multiply-adds, so the
# golden renders match x86's. Needs a firmware build first (build/felucca.dis: tests/target_budget.py) and the AC79
# SDK's files (tools/fm1pkg_make.py packages the image).
#   docker build -t fm-evergreen-tests -f tools/tests.Dockerfile tools
#   docker run --rm --init -e 'CC=cc -ffp-contract=off' -v "$PWD":/work \
#       -v "$HOME/fw-AC79_AIoT_SDK":/root/fw-AC79_AIoT_SDK:ro fm-evergreen-tests sh tests/run_tests.sh
FROM ubuntu:24.04
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential python3 python3-venv ca-certificates && rm -rf /var/lib/apt/lists/*
RUN python3 -m venv /opt/py && /opt/py/bin/pip install --no-cache-dir Pillow==12.3.0 numpy==2.3.5 fonttools==4.61.1
ENV PATH=/opt/py/bin:$PATH
WORKDIR /work
