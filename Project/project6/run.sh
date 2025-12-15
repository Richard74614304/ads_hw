#!/bin/bash

# 批量运行FFD-R算法测试脚本
# 功能：编译代码 → 遍历测试文件 → 逐个运行（直接指定结果文件）→ 保存独立结果文件
# 适配：test.c已修改为接受2个参数：<testfile> <resultfile>

# ===================== 配置项（用户可根据需求修改）=====================
# 1. 测试文件相关配置
TEST_DIR="./dataset/scale"          # 测试文件所在目录（对应3D场景可改为./dataset/3D）
TEST_FILE_PREFIX="scale"   # 测试文件前缀（对应3D场景可改为3D_textures）
TEST_FILE_EXT="txt"              # 测试文件后缀
TEST_FILE_COUNT=12                # 测试文件数量（如5个：scale1~5.txt）

# 新增：结果文件存储目录（用户可自行修改，比如./results/3D）
RESULT_DIR="./result/FFD-R/scale"

# 2. 编译相关配置
FFD_SRC="FFD-R.c"                # FFD-R算法源文件
TEST_SRC="test.c"                # 测试主文件
EXE_NAME="ffd_r_test"            # 编译后的可执行文件名
COMPILE_FLAGS="-Wall -O2"        # 编译选项

# ===================== 颜色与日志配置 =====================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # 重置颜色

# 打印带时间的日志
log() {
    echo -e "[$(date +%H:%M:%S)] $1"
}

# ===================== 前置检查 =====================
log "${YELLOW}【前置检查】开始验证环境和文件...${NC}"

# 1. 检查gcc是否安装
if ! command -v gcc &> /dev/null; then
    log "${RED}错误：未找到gcc编译器，请先安装gcc！${NC}"
    exit 1
fi
log "${GREEN}✓ gcc已安装${NC}"

# 2. 检查算法源文件是否存在
if [ ! -f "${FFD_SRC}" ]; then
    log "${RED}错误：未找到FFD-R算法源文件 ${FFD_SRC}！${NC}"
    exit 1
fi
if [ ! -f "${TEST_SRC}" ]; then
    log "${RED}错误：未找到测试主文件 ${TEST_SRC}！${NC}"
    exit 1
fi
log "${GREEN}✓ 所有源文件已找到${NC}"

# 3. 检查测试文件目录是否存在
if [ ! -d "${TEST_DIR}" ]; then
    log "${RED}错误：测试文件目录 ${TEST_DIR} 不存在！${NC}"
    exit 1
fi
log "${GREEN}✓ 测试文件目录 ${TEST_DIR} 已找到${NC}"

# 新增：检查并创建结果文件夹（-p参数表示递归创建，父目录不存在也会自动创建）
if [ ! -d "${RESULT_DIR}" ]; then
    log "${YELLOW}结果目录 ${RESULT_DIR} 不存在，正在创建...${NC}"
    mkdir -p "${RESULT_DIR}"
    # 检查创建是否成功
    if [ $? -ne 0 ]; then
        log "${RED}错误：创建结果目录 ${RESULT_DIR} 失败！${NC}"
        exit 1
    fi
fi
log "${GREEN}✓ 结果文件目录 ${RESULT_DIR} 已准备好${NC}"

# ===================== 编译代码 =====================
log "\n${YELLOW}【编译代码】开始编译 ${FFD_SRC} 和 ${TEST_SRC}...${NC}"
gcc "${FFD_SRC}" "${TEST_SRC}" -o "${EXE_NAME}" ${COMPILE_FLAGS}

# 检查编译是否成功
if [ $? -ne 0 ]; then
    log "${RED}错误：编译失败！请检查代码或依赖（如TP.h是否存在）${NC}"
    exit 1
fi
log "${GREEN}✓ 编译成功，生成可执行文件 ${EXE_NAME}${NC}"

# 赋予可执行权限（Linux/macOS必要，Windows MinGW可选）
chmod +x "${EXE_NAME}"

# ===================== 批量运行测试文件（关键修改处）=====================
log "\n${YELLOW}【批量测试】开始运行 ${TEST_FILE_COUNT} 个测试文件...${NC}"

for ((i=1; i<=TEST_FILE_COUNT; i++)); do
    # 拼接测试文件路径
    TEST_FILE="${TEST_DIR}/${TEST_FILE_PREFIX}${i}.${TEST_FILE_EXT}"
    # 修改：拼接结果文件路径（加入结果文件夹目录）
    RESULT_FILE="${RESULT_DIR}/result_${TEST_FILE_PREFIX}${i}.${TEST_FILE_EXT}"

    log "\n${YELLOW}===== 运行第 ${i}/${TEST_FILE_COUNT} 个测试文件：${TEST_FILE} =====${NC}"

    # 检查测试文件是否存在
    if [ ! -f "${TEST_FILE}" ]; then
        log "${RED}警告：测试文件 ${TEST_FILE} 不存在，跳过该文件！${NC}"
        continue
    fi

    # ========== 关键修改：传入2个参数（测试文件 + 结果文件） ==========
    log "${YELLOW}正在执行：./${EXE_NAME} ${TEST_FILE} ${RESULT_FILE}${NC}"
    ./"${EXE_NAME}" "${TEST_FILE}" "${RESULT_FILE}"

    # 检查运行是否成功（通过退出码判断）
    if [ $? -eq 0 ]; then
        # 直接检查结果文件是否生成（无需mv操作）
        if [ -f "${RESULT_FILE}" ]; then
            log "${GREEN}✓ 测试完成，结果已保存到 ${RESULT_FILE}${NC}"
        else
            log "${YELLOW}警告：测试运行成功，但未生成 ${RESULT_FILE} 文件！${NC}"
        fi
    else
        log "${RED}错误：运行 ${TEST_FILE} 时失败！${NC}"
    fi
done

# ===================== 收尾工作 =====================
log "\n${YELLOW}【测试完成】清理可执行文件${NC}"
# 可选：删除可执行文件（若需要保留，注释下面这行）
rm -f "${EXE_NAME}"

log "${GREEN}🎉 所有测试文件运行完成！结果文件：${RESULT_DIR}/result_${TEST_FILE_PREFIX}1.${TEST_FILE_EXT} ~ ${RESULT_DIR}/result_${TEST_FILE_PREFIX}${TEST_FILE_COUNT}.${TEST_FILE_EXT}${NC}"

exit 0