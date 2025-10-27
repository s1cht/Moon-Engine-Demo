SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd "$SCRIPT_DIR" || exit
cd .. || exit
[ ! -f "vendor/premake-core/bin/release/premake5" ] && vendor/premake-core/Bootstrap.sh
vendor/premake-core/bin/release/premake5 --cc=clang --verbose gmake