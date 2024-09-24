#!/bin/bash

configs=("Debug" "Release")
strict_options=(ON OFF)
json_lib_options=(CJSON YYJSON)

GREEN_B='\033[1;32m'
RED_B='\033[1;31m'
GREY_B='\033[1;37m'
GREY='\033[;37m'
YEL_B='\033[1;93m'
NC='\033[0m'

EXIT_CODE=0
result=""

for config in "${configs[@]}"; do
    for strict in "${strict_options[@]}"; do
        for json_lib in "${json_lib_options[@]}"; do
            if [[ ${strict} == "ON" ]]; then
                build_name="${config}_strict_${json_lib}"
            else
                build_name="${config}_${json_lib}"
            fi
            echo -e "\n\n======== ${GREY_B}Building: ${YEL_B}$build_name${NC} ========"

            build_dir="build/${build_name}"
            mkdir -p "$build_dir"
            cd "$build_dir"

            cmake "$OLDPWD" \
                -DCMAKE_BUILD_TYPE=$config \
                -DREVOLT_BUILD_STRICT=$strict \
                -DREVOLT_WITH_$json_lib=On \
                -DREVOLT_BUILD_SHARED=On \
                -DREVOLT_BUILD_STATIC=On
            if [ $? -ne 0 ]; then
                result="${result} - ${GREY}${build_name}${NC}: ${RED_B}Failed${NC}\n"
                let "EXIT_CODE++"
                cd "$OLDPWD"
                continue
            fi

            cmake --build . -j $(($(grep -c ^processor /proc/cpuinfo) * 2))
            if [ $? -ne 0 ]; then
                result="${result} - ${GREY}${build_name}${NC}: ${RED_B}Failed${NC}\n"
                let "EXIT_CODE++"
                cd "$OLDPWD"
                continue
            fi

            result="${result} - ${GREY}${build_name}${NC}: ${GREEN_B}Ok${NC}\n"
            cd "$OLDPWD"
        done
    done
done

echo -e "\nResults:"
echo -e "$result"
exit $EXIT_CODE
