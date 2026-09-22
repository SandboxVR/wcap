#!/usr/bin/env bash
set -euo pipefail

target_arch="${1:-x64}"
docker_context="${DOCKER_CONTEXT:-winvm}"
output_dir="${OUTPUT_DIR:-dist}"

case "$target_arch" in
  x64|arm64) ;;
  *)
    echo "usage: $0 [x64|arm64]" >&2
    exit 2
    ;;
esac

image="wcap-windows-build:${target_arch}"
container_id=""

cleanup() {
  if [[ -n "$container_id" ]]; then
    docker --context "$docker_context" rm "$container_id" >/dev/null
  fi
}
trap cleanup EXIT

docker --context "$docker_context" build \
  --file Dockerfile.windows \
  --build-arg "TARGET_ARCH=$target_arch" \
  --tag "$image" \
  .

container_id="$(docker --context "$docker_context" create "$image")"
mkdir -p "$output_dir"
docker --context "$docker_context" cp \
  "${container_id}:C:/out/wcap-${target_arch}.exe" \
  "$output_dir/wcap-${target_arch}.exe"

echo "Built $output_dir/wcap-${target_arch}.exe"
